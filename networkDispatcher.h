#ifndef NETWORKDISPATCHER_H
#define NETWORKDISPATCHER_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "logger.h"
#include "packet.h"
#include <netdb.h>
#include <map>
#include <functional>
#include <thread>
#include <mutex>

#include "AbstractPacketFactory.h"
#include "clientThread.h"

#define SM_OK "OK"
#define SM_END "END"

/**
 * Client server messaging by sockets
 */
class networkDispatcher {
private:
    static int sockfd, newsockfd, port, n;
    static socklen_t clilen;
    static struct sockaddr_in serv_addr, cli_addr;
    static struct hostent *server;
    static std::string hostname;
    static AbstractPacketFactory *factory;
    static packetDirection dispatcherDirection;
public:
    static bool connected;
    static void setPort(int portG);
    static void setHost(const std::string &hostnameG);
    static void setFactory(AbstractPacketFactory *factoryG);

    static void startListen();
    static int con();
    static void closeCon();
    static void send(std::unique_ptr<packet>& p, const std::string &to);
    static void receive();
    static void sendSM(char* sm);
    static bool receiveSM(char* sm);
};


#endif //NETWORKDISPATCHER_H
