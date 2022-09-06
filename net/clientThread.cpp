#include "clientThread.h"

void clientThread::run() {
    while(sockfd != -1) {
        networkDispatcher::receive(sockfd);
    }
    networkDispatcher::closeCon(sockfd);
}

void clientThread::setSockfd(int s) {
    sockfd = s;
}