#ifndef TINYNETWORKING_BASECLIENT_H
#define TINYNETWORKING_BASECLIENT_H

#include <logger.h>
#include "iostream"
#include "general.h"
#include "socketStatus.h"
#include "socketType.h"

namespace tcp {

#ifdef _WIN32
#define WIN(exp) exp
#define NIX(exp)
#define WINIX(win_exp, nix_exp) win_exp

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
#endif
#ifndef _WIN32
#define WIN(exp)
#define NIX(exp) exp
#define WINIX(win_exp, nix_exp) nix_exp
#endif
    class baseClient {
    public:
        typedef socketStatus status;

        virtual ~baseClient() {};

        virtual status disconnect() = 0;

        virtual status getStatus() = 0;

        virtual void sendData(const std::string& buffer, size_t size) = 0;

        virtual std::string loadData() = 0;

        virtual uint32_t getHost() = 0;

        virtual uint16_t getPort() = 0;

        virtual socketType getType() = 0;

        void sendSM(const char* sm, socket_t socket) {
            int n = send(socket, sm, sizeof(sm), 0);
            if(error(n, socket)) return;
            //std::cout << "send " << sm << std::endl;
        }

        bool receiveSM(const char* sm, socket_t socket) {
            char *sizeB[1024];
            bzero(sizeB, 1024);
            bool isReceived = false;
            long timeout = time(nullptr)+1;

            while(!isReceived && timeout >= time(nullptr)) {
               // logger::info(std::to_string(timeout) + " realTime: " + std::to_string(time(nullptr)));
                WIN(if (u_long t = true; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) continue;)
                int n = read(socket, sizeB, sizeof(sm));
                WIN(if (u_long t = false; SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &t)) continue;)
                if (error(n, socket)) {
                    isReceived = false;
                    continue;
                }
                if (strstr(reinterpret_cast<const char *>(sizeB), sm)) {
                    remove(reinterpret_cast<const char *>(sizeB));
                    isReceived = true;
                    //std::cout << time(nullptr) << " rec " <<reinterpret_cast<const char *>(sizeB) << std::endl;
                    continue;
                } else {
                    //logger::error("Expected '"+std::string(sm)+"', but received " + std::string(reinterpret_cast<const char *>(sizeB)));
                    remove(reinterpret_cast<const char *>(sizeB));
                    isReceived = false;
                    continue;
                }
               // sleep(0.1);
            }

            return true;
        }

        bool error(int n, socket_t socket){
            int err;
            //logger::info(strerror(errno));
            if(!n) {
                disconnect();
                return true;
            } else if(n == -1) {
                WIN(
                    err = convertError();
                    if(!err) {
                        SockLen_t len = sizeof (err);
                        getsockopt (socket, SOL_SOCKET, SO_ERROR, WIN((char*))&err, &len);
                    }
                )NIX(
                    sockLen_t len = sizeof (err);
                    getsockopt (socket, SOL_SOCKET, SO_ERROR, WIN((char *)) & err, &len);
                    if(!err) err = errno;
                )

                switch (err) {
                    case 0: break;
                        // Keep alive timeout
                    case ETIMEDOUT:
                       // logger::error("error socket! ETIMEDOUT");
                    case ECONNRESET:
                       // logger::error("error socket! ECONNRESET");
                    case EPIPE:
                       // logger::error("error socket! EPIPE");
                        disconnect();
                        [[fallthrough]];
                        // No data
                    case EAGAIN:
                       // logger::error("error socket! EAGAIN");
                        return false;
                    default:
                       // logger::error("error socket! UNKNOWN");
                        disconnect();
                        return true;
                }
            }
            return false;
        }
    };

#ifdef _WIN32 // Windows NT
    namespace {
    class _WinSocketIniter {
      static WSAData w_data;
    public:
      _WinSocketIniter() {
        WSAStartup(MAKEWORD(2, 2), &w_data)
      }

      ~_WinSocketIniter() {
        WSACleanup()
      }
    };

    static inline _WinSocketIniter _winsock_initer;
    }
#endif
}
#endif //TINYNETWORKING_BASECLIENT_H
