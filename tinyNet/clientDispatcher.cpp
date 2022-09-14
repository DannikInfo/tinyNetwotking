#include "clientDispatcher.h"

clientDispatcher::clientDispatcher(tcp::client &client, const std::string &name, uint32_t server, int port, abstractPacketFactory *pf, abstractNetworkHandler *nh){
    factory = pf;
    networkHandler = nh;
    this->name = name;
    using namespace std::chrono_literals;
    while(client.getStatus() != tcp::socketStatus::connected){
        if(client.connectTo(server, port) == tcp::socketStatus::connected) {
            logger::info("Connected to server " + getHostStr(client));

            client.setHandler([&client, this](const std::string& data) {
                receive(data, client);
            });
        } else {
            logger::error("Failed connect to server, try again after 5 sec");
            sleep(5);
        }
    }
}

