#ifndef TINYNETWORKING_SERVERDISPATCHER_H
#define TINYNETWORKING_SERVERDISPATCHER_H

#include <iostream>

#include "dispatcher.h"
#include "packet.h"
#include <logger.h>
#include <json.hpp>
#include "abstractPacketFactory.h"
#include "abstractNetworkHandler.h"


class serverDispatcher : public dispatcher {
private:
    tcp::server server;
public:
    serverDispatcher(int port, const std::string &serverName, abstractPacketFactory *pf, abstractNetworkHandler *nh, int idle = 1, int interval = 1, int pk_count = 1);
};




#endif //TINYNETWORKING_SERVERDISPATCHER_H
