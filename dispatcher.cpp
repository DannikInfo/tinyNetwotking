#include "dispatcher.h"

std::string dispatcher::getHostStr(const stcp::TcpClientBase &client) {
    uint32_t ip = client.getHost ();
    return std::string() + std::to_string(int(reinterpret_cast<char*>(&ip)[0])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[1])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[2])) + '.' +
           std::to_string(int(reinterpret_cast<char*>(&ip)[3])) + ':' +
           std::to_string( client.getPort ());
}

void dispatcher::receive(const stcp::DataBuffer& data, stcp::TcpClientBase &client) {
    std::string d;
    for (const auto &item : data)
        d += item;

    nlohmann::json j = nlohmann::json::parse(d);
    std::unique_ptr<packet> packet = factory->create(j["id"].get<int>());

    if(packet->getDirection() == packetDirection::SERVER || packet->getDirection() == packetDirection::UNIVERSAL)
        packet->deserialize(d);

    if(packet->getDirection() == packetDirection::UNIVERSAL && !j["answer"])
        send(client, packet);
}

void dispatcher::send(stcp::TcpClientBase &client, std::unique_ptr<packet>& p) {
    p->setTimeSend(std::time(nullptr));
    p->serialize();
    std::string data = p->getData().dump();
    client.sendData(data.c_str(), data.length());
}
