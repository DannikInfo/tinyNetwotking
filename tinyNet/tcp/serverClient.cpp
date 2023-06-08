#include "serverClient.h"

using namespace tcp;

#ifdef _WIN32
#define WIN(exp) exp
#define NIX(exp)

inline int convertError() {
    switch (WSAGetLastError()) {
    case 0:
        return 0;
    case WSAEINTR:
        return EINTR;
    case WSAEINVAL:
        return EINVAL;
    case WSA_INVALID_HANDLE:
        return EBADF;
    case WSA_NOT_ENOUGH_MEMORY:
        return ENOMEM;
    case WSA_INVALID_PARAMETER:
        return EINVAL;
    case WSAENAMETOOLONG:
        return ENAMETOOLONG;
    case WSAENOTEMPTY:
        return ENOTEMPTY;
    case WSAEWOULDBLOCK:
        return EAGAIN;
    case WSAEINPROGRESS:
        return EINPROGRESS;
    case WSAEALREADY:
        return EALREADY;
    case WSAENOTSOCK:
        return ENOTSOCK;
    case WSAEDESTADDRREQ:
        return EDESTADDRREQ;
    case WSAEMSGSIZE:
        return EMSGSIZE;
    case WSAEPROTOTYPE:
        return EPROTOTYPE;
    case WSAENOPROTOOPT:
        return ENOPROTOOPT;
    case WSAEPROTONOSUPPORT:
        return EPROTONOSUPPORT;
    case WSAEOPNOTSUPP:
        return EOPNOTSUPP;
    case WSAEAFNOSUPPORT:
        return EAFNOSUPPORT;
    case WSAEADDRINUSE:
        return EADDRINUSE;
    case WSAEADDRNOTAVAIL:
        return EADDRNOTAVAIL;
    case WSAENETDOWN:
        return ENETDOWN;
    case WSAENETUNREACH:
        return ENETUNREACH;
    case WSAENETRESET:
        return ENETRESET;
    case WSAECONNABORTED:
        return ECONNABORTED;
    case WSAECONNRESET:
        return ECONNRESET;
    case WSAENOBUFS:
        return ENOBUFS;
    case WSAEISCONN:
        return EISCONN;
    case WSAENOTCONN:
        return ENOTCONN;
    case WSAETIMEDOUT:
        return ETIMEDOUT;
    case WSAECONNREFUSED:
        return ECONNREFUSED;
    case WSAELOOP:
        return ELOOP;
    case WSAEHOSTUNREACH:
        return EHOSTUNREACH;
    default:
        return EIO;
    }
}


#else
#define WIN(exp)
#define NIX(exp) exp
#endif

#include <iostream>
#include <logger.h>
#include <json.hpp>

int recDepth2;

std::string serverClient::loadData() {
    if(_status != socketStatus::connected) return {};
    using namespace std::chrono_literals;
    std::string buffer;
    uint32_t size;
    int err;

    // Read data length in non-blocking mode
    // MSG_DONTWAIT - Unix non-blocking read
    WIN(if(u_long t = true; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) return DataBuffer();) // Windows non-blocking mode on
    int answ = recv(socket, (char*)&size, sizeof(size), NIX(MSG_DONTWAIT)WIN(0));
    WIN(if(u_long t = false; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) return DataBuffer();) // Windows non-blocking mode off

    // Disconnect
    if(!answ) {
       // disconnect();
        return {};
    } else if(answ == -1) {
        // Error handle (f*ckin OS-dependence!)
        WIN(
                err = convertError();
                if(!err) {
                    SockLen_t len = sizeof (err);
                    getsockopt (socket, SOL_SOCKET, SO_ERROR, WIN((char*))&err, &len);
                }
        )NIX(
                sockLen_t len = sizeof (err);
                getsockopt (socket, SOL_SOCKET, SO_ERROR, WIN((char*))&err, &len);
                if(!err) err = errno;
        )


        switch (err) {
            case 0: break;
                // Keep alive timeout
            case ETIMEDOUT:
            case ECONNRESET:
            case EPIPE:
               // disconnect();
                [[fallthrough]];
                // No data
            case EAGAIN: return {};
            default:
              //  disconnect();
                std::cerr << "Unhandled error!\n"
                          << "Code: " << err << " Err: " << std::strerror(err) << '\n';
                return {};
        }
    }

    if(!size) return {};
    buffer.resize(size);
    recv(socket, buffer.data(), buffer.size(), 0);
    return buffer;
//    if(_status != socketStatus::connected) return {};
//    int n;
//    //data size
//    char *sizeB[1024];
//    std::string data;
//    //while(!receiveSM(SM_END, socket)) {
//        bzero(sizeB, 1024);
//        WIN(if(u_long t = true; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) return std::string();)
//        n = recv(socket, sizeB, 1024, 0);
//        WIN(if(u_long t = false; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) return std::string();)
//        if (error(n, socket)) return {};
//        if (strstr(reinterpret_cast<const char *>(sizeB), SM_OK) ||
//            strstr(reinterpret_cast<const char *>(sizeB), SM_END) ||
//            strstr(reinterpret_cast<const char *>(sizeB), SM_FAIL))
//            return {};
//        int size = atoi(reinterpret_cast<const char *>(sizeB));
//
//        if(size > 0) {
//            sendSM(SM_OK, socket);
//           // logger::info("dataSize: " + std::to_string(size));
////
////          //pieces count
////            char *piecesB[1024];
////            bzero(piecesB, 1024);
////            WIN(if(u_long t = true; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) return "";)
////            n = read(socket, piecesB, 1024);
////            WIN(if(u_long t = false; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) return "";)
////            if (error(n, socket)) return "";
////        try {
////            int pieces = atoi(reinterpret_cast<const char *>(piecesB));
////
//            int pieces = size/4096 + 1;
//
//                //sendSM(SM_OK, socket);
//         //       logger::info("pieces: " + std::to_string(pieces));
//
//                //data receive
//                std::string received;
//                char *preBuffer[4096];
//                int readSize;
//
//                for (int i = 0; i < pieces; i++) {
//                    if (i + 1 == pieces)
//                        readSize = size - 4096 * i;
//                    else
//                        readSize = 4096;
//
//                    bzero(preBuffer, 4096);
//
//                    do {
//                       // WIN(if(u_long t = true; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) continue;)
//                        n = recv(socket, preBuffer, readSize, 0);
//                       // WIN(if(u_long t = true; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) continue;)
//                        if (error(n, socket)) return {};
//                        sendSM(SM_OK, socket);
//                        received = reinterpret_cast<char *const>(preBuffer);
//                    } while (strstr(data.c_str(), received.c_str()));
//                    data += received;
//                }
//                sendSM(SM_OK, socket);
//                //logger::info(data);
//
//                if(receiveSM(SM_FAIL, socket)) return "";
//                try {
//                    nlohmann::json j = nlohmann::json::parse(data);
//                    if(!j.is_object()){
//                        sendSM(SM_FAIL, socket);
//                        return "";
//                    }
//                    receiveSM(SM_END, socket);
//                    sendSM(SM_OK, socket);
//                }catch(...){
//                    sendSM(SM_FAIL, socket);
//                    return "";
//                }
////        }catch(...){}
////    }
//    //    sleep(1);
//    }
//    return data;
}

baseClient::status serverClient::disconnect() {
  _status = status::disconnected;
  if(socket == WIN(INVALID_SOCKET)NIX(-1)) return _status;
  shutdown(socket, SD_BOTH);
  WIN(closesocket)NIX(close)(socket);
  socket = WIN(INVALID_SOCKET)NIX(-1);
  return _status;
}

void serverClient::sendData(const std::string& data, size_t siz2e) {
    if(_status != socketStatus::connected) return;
    ulong size = data.length();
    void* send_buffer = malloc(size + sizeof (uint32_t));
    memcpy(reinterpret_cast<char*>(send_buffer) + sizeof(uint32_t), data.c_str(), size);
    *reinterpret_cast<uint32_t*>(send_buffer) = size;

    if(send(socket, reinterpret_cast<char*>(send_buffer), size + sizeof (int), 0) < 0) return;

    free(send_buffer);
    return;
//    recDepth2++;
//    int dataPieces = 1, n;
//    ulong size = data.length();
//
//    if(size > 4096)
//        dataPieces = (int)(size/4096)+1;
//
//    std::string strDataPieces = std::to_string(dataPieces);
//    //data size
//    n = send(socket, std::to_string(size).c_str(), std::to_string(size).length(), 0);
//    if(error(n, socket)) return;
//    receiveSM(SM_OK, socket);
//
//    //pieces count
//
////    n = send(client_socket, strDataPieces.c_str(), strDataPieces.length(), 0);
////    if(error(n, client_socket)) return;
////    receiveSM(SM_OK, client_socket);
////
//    std::string dataPiece;
//    //data send
//    for(int i = 0; i < dataPieces; i++) {
//        dataPiece = data.substr(i*4096, (i+1)*4096);
//        n = send(socket, dataPiece.c_str(), dataPiece.length(), 0);
//        if(error(n, socket)) return;
//        receiveSM(SM_OK, socket);
//    }
//    sendSM(SM_END, socket);
//    if(recDepth2 < 3) {
//        if (receiveSM(SM_FAIL, socket))
//            sendData(data, size);
//    }else {
//        sendSM(SM_FAIL, socket);
//        logger::error("Failed to send data with size: " + std::to_string(size));
//    }
//    recDepth2--;
}

serverClient::serverClient(sock socket, socketAddr_in address)
  : address(address), socket(socket) {}

serverClient::~serverClient() {
  if(socket == WIN(INVALID_SOCKET)NIX(-1)) return;
  shutdown(socket, SD_BOTH);
  WIN(closesocket(socket);)
  NIX(close(socket);)
}

uint32_t serverClient::getHost() {
    return NIX(address.sin_addr.s_addr) WIN(address.sin_addr.S_un.S_addr);
}

uint16_t serverClient::getPort() {
    return address.sin_port;
}

