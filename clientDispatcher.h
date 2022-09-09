#ifndef TINYNETWORKING_CLIENTDISPATCHER_H
#define TINYNETWORKING_CLIENTDISPATCHER_H

#include "dispatcher.h"
#include "packet.h"
#include <logger.h>
#include <json.hpp>
#include <AbstractPacketFactory.h>

class clientDispatcher : public dispatcher{
private:
    stcp::ThreadPool threadPool;
public:
    clientDispatcher(stcp::TcpClient &client, uint32_t server, int port, AbstractPacketFactory *pf);
};


#endif //TINYNETWORKING_CLIENTDISPATCHER_H
