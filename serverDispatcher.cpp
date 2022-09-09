#include "serverDispatcher.h"

serverDispatcher::serverDispatcher(int port, const std::string &serverName, AbstractPacketFactory *pf,int idle, int interval, int pk_count) : server(port,
    {idle, interval, pk_count}, // Keep alive{idle:1s, interval: 1s, pk_count: 1}

    [this](const stcp::DataBuffer& data, stcp::TcpServer::Client& client){ // Data handler
        this->receive(data, client);
    },

    [](stcp::TcpServer::Client& client) { // Connect handler
        logger::info("Client " + getHostStr(client) + " connected");
    },

    [](stcp::TcpServer::Client& client) { // Disconnect handler
       logger::info("Client " + getHostStr(client) + " disconnected");
    },

    std::thread::hardware_concurrency()
    ){
    name = serverName;
    factory = pf;
    try {
        //Start server
        if(server.start() == stcp::TcpServer::status::up) {
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


