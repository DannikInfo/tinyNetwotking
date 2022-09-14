#ifndef TINYNETWORKING_DISPATCHER_H
#define TINYNETWORKING_DISPATCHER_H

#include <iostream>

#include "tcp/server.h"
#include "tcp/client.h"
#include "packet.h"
#include <logger.h>
#include <json.hpp>
#include "abstractPacketFactory.h"
#include "abstractNetworkHandler.h"

class dispatcher {
protected:
    std::string name;
    abstractPacketFactory *factory;
    abstractNetworkHandler *networkHandler;
public:
    static std::string getHostStr(tcp::baseClient& client);
    void receive(const std::string& data, tcp::baseClient& client);
    void send(tcp::baseClient& client, std::unique_ptr<packet>& p);

    int ipStringToNumber(const char *pDottedQuad, unsigned int *pIpAddr);
};


#endif //TINYNETWORKING_DISPATCHER_H
