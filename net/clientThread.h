#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <iostream>
#include <thread>

#include "networkDispatcher.h"

class clientThread : public std::thread{
private:
    static int sockfd;
public:
    static void run();
    static void setSockfd(int s);
};

#endif //CLIENTTHREAD_H
