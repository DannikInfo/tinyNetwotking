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
    static int port, n;
    static socklen_t clilen;
    static struct sockaddr_in serv_addr, cli_addr;
    static struct hostent *server;
    static std::string hostname;
    static AbstractPacketFactory *factory;
public:
    static void setPort(int portG);
    static void setHost(std::string &hostnameG);
    static void setFactory(AbstractPacketFactory *factoryG);

    static int getSockfd();

    static void startListen();
    static int con();
    static void closeCon(int sockfd, int newsockfd = -1);
    static void send(std::unique_ptr<packet>& p, std::string &to, int sockfd);
    static void receive(int sockfd);
    static void sendSM(char* sm, int sockfd);
    static bool receiveSM(char* sm, int sockfd);
};


#endif //NETWORKDISPATCHER_H
