#ifndef TINYNETWORKING_SOCKETSTATUS_H
#define TINYNETWORKING_SOCKETSTATUS_H

#include "iostream"
namespace tcp {
    enum class socketStatus : uint8_t {
        connected = 0,
        err_socket_init = 1,
        err_socket_bind = 2,
        err_socket_connect = 3,
        disconnected = 4
    };
}

#endif //TINYNETWORKING_SOCKETSTATUS_H
