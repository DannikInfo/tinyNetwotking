#include "serverDispatcher.h"

serverDispatcher::serverDispatcher(int port, const std::string &serverName, abstractPacketFactory *pf, abstractNetworkHandler *nh, int idle, int interval, int pk_count) : server(port,
    {idle, interval, pk_count}, // Keep alive{idle:1s, interval: 1s, pk_count: 1}

    [this](const std::string& data, tcp::serverClient& client){ // Data handler
        this->receive(data, client);
    },

    [this](tcp::serverClient& client) { // Connect handler
        logger::info("Client " + getHostStr(client) + " connected");
    },

    [this](tcp::serverClient& client) { // Disconnect handler
        logger::info("Client " + getHostStr(client) + " disconnected");
    },

    std::thread::hardware_concurrency()
    ){
    name = serverName;
    factory = pf;
    networkHandler = nh;
    try {
        //Start server
        if(server.start() == tcp::serverStatus::up) {
            logger::info("Server listen on port: " + std::to_string(server.getPort()));
            logger::info("Server handling thread pool size: " + std::to_string(server.getThreadPool().getThreadCount()));
            server.joinLoop();
            return;
        } else {
            logger::error("Server start error! Error code:" + std::to_string(int(server.getStatus())));
            return;
        }

    } catch(std::exception& except) {
        logger::error(except.what());
        return;
    }
}


