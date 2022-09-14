#ifndef TINYNETWORKING_SERVERSTATUS_H
#define TINYNETWORKING_SERVERSTATUS_H

#include "iostream"
namespace tcp {
    enum class serverStatus : uint8_t {
        up = 0,
        err_socket_init = 1,
        err_socket_bind = 2,
        err_scoket_keep_alive = 3,
        err_socket_listening = 4,
        close = 5
    };
}
#endif //TINYNETWORKING_SERVERSTATUS_H
