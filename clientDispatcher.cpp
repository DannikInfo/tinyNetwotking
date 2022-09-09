#include "clientDispatcher.h"

clientDispatcher::clientDispatcher(stcp::TcpClient &client, uint32_t server, int port, AbstractPacketFactory *pf){
    factory = pf;
    using namespace std::chrono_literals;
    while(client.getStatus() != stcp::SocketStatus::connected){
        if(client.connectTo(server, port) == stcp::SocketStatus::connected) {
            logger::info("Connected to server " + getHostStr(client));

            client.setHandler([&client, this](stcp::DataBuffer data) {
                receive(data, client);
            });
        } else {
            logger::error("Failed connect to server, try again after 5 sec");
            sleep(5);
        }
    }
}