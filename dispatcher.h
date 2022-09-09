#ifndef TINYNETWORKING_DISPATCHER_H
#define TINYNETWORKING_DISPATCHER_H

#include <iostream>

#include "TcpServer.h"
#include "TcpClient.h"
#include "packet.h"
#include <logger.h>
#include <json.hpp>
#include <AbstractPacketFactory.h>

class dispatcher {
protected:
    std::string name;
    AbstractPacketFactory *factory;
public:
    static std::string getHostStr(const stcp::TcpClientBase& client);
    void receive(const stcp::DataBuffer& data, stcp::TcpClientBase& client);
    void send(stcp::TcpClientBase& client, std::unique_ptr<packet>& p);
};


#endif //TINYNETWORKING_DISPATCHER_H
