/**
 * original author is:
 * https://github.com/gbytegear/TcpServer
 */

#ifndef GENERAL_H
#define GENERAL_H

#ifdef _WIN32
#else
#define SD_BOTH 0
#include <sys/socket.h>
#endif

#include "iostream"
#include "vector"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#endif

namespace tcp {

#define SM_OK "OK"
#define SM_END "END"
#define SM_FAIL "FAIL"

#ifdef _WIN32 // Windows NT
    typedef int sockLen_t;
    typedef SOCKADDR_IN socketAddr_in;
    typedef SOCKET socket;
    typedef u_long ka_prop_t;
    typedef SOCKET socket_t;
#else // POSIX
    typedef socklen_t sockLen_t;
    typedef struct sockaddr_in socketAddr_in;
    typedef int sock;
    typedef int ka_prop_t;
    typedef int socket_t;
#endif

    constexpr uint32_t LOCALHOST_IP = 0x0100007f;
}

#endif // GENERAL_H
