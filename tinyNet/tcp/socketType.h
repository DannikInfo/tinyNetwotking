#ifndef TINYNETWORKING_SOCKETTYPE_H
#define TINYNETWORKING_SOCKETTYPE_H

#include "iostream"
namespace tcp {
    enum class socketType : uint8_t {
        client_socket = 0,
        server_socket = 1
    };
}

#endif //TINYNETWORKING_SOCKETTYPE_H
