#include "dispatcher.h"
#include <cstdio>

std::string dispatcher::getHostStr( tcp::baseClient &client) {
    uint32_t ip = client.getHost ();
    return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
           std::to_string( client.getPort ());
}

void dispatcher::receive(const std::string& data, tcp::baseClient &client) {
    if(data.empty()) return;
    try {
        nlohmann::json j = nlohmann::json::parse(data);
        if(j["id"].get<int>() == -1)
            return;

        nlohmann::json defJ;
        std::unique_ptr<packet> packet = factory->create(j["id"].get<int>(), defJ);

        if(packet->getDirection() == packetDirection::SERVER || packet->getDirection() == packetDirection::UNIVERSAL)
            packet->deserialize(j);

        if(packet->isProcessed())
            networkHandler->handlePacket(packet, client);

        if(packet->getDirection() == packetDirection::UNIVERSAL && !j["answer"])
            send(client, packet);

    }catch(nlohmann::detail::parse_error &pe){
        logger::error(pe.what());
        return;
    }
}

void dispatcher::send(tcp::baseClient &client, std::unique_ptr<packet>& p) {
    p->setTimeSend(std::time(nullptr));
    p->serialize();
    std::string data = p->getData().dump();
    client.sendData(data, sizeof(data.c_str()));
}


/**
 * Convert human readable IPv4 address to UINT32
 * @param pDottedQuad   Input C string e.g. "192.168.0.1"
 * @param pIpAddr       Output IP address as UINT32
 * return 1 on success, else 0
 */
int dispatcher::ipStringToNumber(const char* pDottedQuad, unsigned int * pIpAddr){
    unsigned int            byte3;
    unsigned int            byte2;
    unsigned int            byte1;
    unsigned int            byte0;
    char              dummyString[2];

    /* The dummy string with specifier %1s searches for a non-whitespace char
    * after the last number. If it is found, the result of sscanf will be 5
    * instead of 4, indicating an erroneous format of the ip-address.
    */
    if (sscanf (pDottedQuad, "%u.%u.%u.%u%1s", &byte3, &byte2, &byte1, &byte0, dummyString) == 4){
        if ((byte3 < 256)
            && (byte2 < 256)
            && (byte1 < 256)
            && (byte0 < 256)
                ){
            *pIpAddr  =   (byte3 << 24)
                          + (byte2 << 16)
                          + (byte1 << 8)
                          +  byte0;
            return 1;
        }
    }
    return 0;
}