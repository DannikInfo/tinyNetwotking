#ifndef TINYNETWORKING_CLIENTDISPATCHER_H
#define TINYNETWORKING_CLIENTDISPATCHER_H

#include "dispatcher.h"
#include "packet.h"
#include <logger.h>
#include <json.hpp>
#include "abstractPacketFactory.h"
#include "abstractNetworkHandler.h"

class clientDispatcher : public dispatcher{
private:
    tcp::threadPool threadPool;
public:
    clientDispatcher(tcp::client &client, const std::string &name, uint32_t server, int port, abstractPacketFactory *pf, abstractNetworkHandler *nh);
};


#endif //TINYNETWORKING_CLIENTDISPATCHER_H
