#ifndef TINYNETWORKING_SERVERCLIENT_H
#define TINYNETWORKING_SERVERCLIENT_H

#include <netinet/in.h>
#include "iostream"
#include "baseClient.h"
#include "general.h"
#include "mutex"
#include "socketType.h"

namespace tcp{
    class serverClient : public baseClient {
        friend class server;

        std::mutex access_mtx;
        socketAddr_in address;
        sock socket;
        status _status = status::connected;


    public:
        serverClient(sock socket, socketAddr_in address);
        virtual ~serverClient() override;
        uint32_t getHost()  override;
        uint16_t getPort()  override;
        status getStatus()  override {return _status;}
        status disconnect() override;

        std::string loadData() override;
        void sendData(const std::string& buffer,  size_t size) override;
        socketType getType()  override {return socketType::server_socket;}
    };
}
#endif //TINYNETWORKING_SERVERCLIENT_H
