#ifndef TINYNETWORKING_ABSTRACTNETWORKHANDLER_H
#define TINYNETWORKING_ABSTRACTNETWORKHANDLER_H

#include <iostream>
#include "packet.h"
#include "tcp/baseClient.h"
#include "tcp/general.h"

class abstractNetworkHandler {
public:
    virtual void handlePacket(std::unique_ptr<packet> &p, tcp::baseClient &client) = 0;
};


#endif //TINYNETWORKING_ABSTRACTNETWORKHANDLER_H
