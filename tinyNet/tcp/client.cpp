#include "client.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <utility>
#include <logger.h>

using namespace tcp;

#ifndef _WIN32
#define WIN(exp)
#define NIX(exp) exp
#define WINIX(win_exp, nix_exp) nix_exp
#endif


void client::handleSingleThread() {
    try {
        while(_status == status::connected) {
            if(std::string data = loadData(); !data.empty()) {
                std::lock_guard lock(handle_mutex);
                handler_func(std::move(data));
            } else if (_status != status::connected) return;
        }
    }  catch (std::exception& except) {
        logger::error(except.what());
        return;
    }
}

void client::handleThreadPool() {
    try {
        if(std::string data = loadData(); !data.empty()) {
            std::lock_guard lock(handle_mutex);
            handler_func(std::move(data));
        }
        if(_status == status::connected) threads.tPool->addJob([this]{handleThreadPool();});
    } catch (std::exception& except) {
        logger::error(except.what());
        return;
    } catch (...) {
        logger::error("Unhandled exception! - tinyNet::tcp::client.cpp::handleThreadPool");
        return;
    }
}

client::client() noexcept : _status(status::disconnected) {}
client::client(threadPool* thread_pool) noexcept :
    tMType(threadManageType::thread_pool),
    threads(thread_pool),
    _status(status::disconnected) {}

client::~client() {
    disconnect();
    WIN(WSACleanup();)

    switch (tMType) {
        case threadManageType::single_thread:
            if(threads.pthread) threads.pthread->join();
            delete threads.pthread;
            break;
        case threadManageType::thread_pool:
            break;
    }
}

client::status client::connectTo(uint32_t host, uint16_t port) noexcept {
    WIN(if(WSAStartup(MAKEWORD(2, 2), &w_data) != 0) {})

    if((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) WIN(== INVALID_SOCKET) NIX(< 0)) return _status = status::err_socket_init;

    new(&address) socketAddr_in;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = host;
    WINIX(
       address.sin_addr.S_un.S_addr = host;
       ,
       address.sin_addr.s_addr = host;
    )
    address.sin_port = htons(port);

    if(connect(client_socket, (sockaddr *)&address, sizeof(address))
        WINIX(== SOCKET_ERROR,!= 0)
        ) {
        WINIX(closesocket(client_socket); ,close(client_socket);)
        return _status = status::err_socket_connect;
    }
    return _status = status::connected;
}

client::status client::disconnect() noexcept {
    if(_status != status::connected)
        return _status;
    _status = status::disconnected;
    switch (tMType) {
        case threadManageType::single_thread:
            if(threads.pthread) threads.pthread->join();
            delete threads.pthread;
            break;
        case threadManageType::thread_pool:
            break;
    }
    shutdown(client_socket, SD_BOTH);
    WINIX(closesocket(client_socket), close(client_socket));
    return _status;
}

std::string client::loadData() {
    if(_status != socketStatus::connected) return "";
    int n;
    //data size
    char *sizeB[1024];
    n = recv(client_socket, sizeB, 1024, 0);
    if (error(n, client_socket)) return "";
    int size = atoi(reinterpret_cast<const char *>(sizeB));
    std::string data;
    if(size > 0) {
        sendSM(SM_OK, client_socket);
        logger::info("dataSize: " + std::to_string(size));

        //pieces count
        char *piecesB[1024];
        n = recv(client_socket, piecesB, 1024, 0);
        if (error(n, client_socket)) return "";
        try {
            int pieces = atoi(reinterpret_cast<const char *>(piecesB));
            if(pieces > 0) {
                sendSM(SM_OK, client_socket);
                logger::info("pieces: " + std::to_string(pieces));

                //data receive
                std::string received;
                char *preBuffer[4096];
                int readSize;

                for (int i = 0; i < pieces; i++) {
                    if (i + 1 == pieces)
                        readSize = size - 4096 * i;
                    else
                        readSize = 4096;

                    bzero(preBuffer, 4096);

                    do {
                        n = recv(client_socket, preBuffer, readSize, 0);
                        if (error(n, client_socket)) return "";
                        received = reinterpret_cast<char *const>(preBuffer);
                        sendSM(SM_OK, client_socket);
                    } while (strstr(data.c_str(), received.c_str()));

                    data += received;
                }

                if (!receiveSM(SM_END, client_socket)) return "";
            }
        }catch(...){}
    }
    //sleep(1);
    return data;
}

const std::string& client::loadDataSync() {
    std::string data;
    uint32_t size = 0;
    int answ = recv(client_socket, reinterpret_cast<char*>(&size), sizeof(size), 0);
    if(size && answ == sizeof (size)) {
        data.resize(size);
        recv(client_socket, reinterpret_cast<char*>(data.data()), data.size(), 0);
    }
    return data;
}

void client::setHandler(client::handler_function_t handler) {
    {
        std::lock_guard lock(handle_mutex);
        handler_func = std::move(handler);
    }

    switch (tMType) {
        case threadManageType::single_thread:
            if(threads.pthread) return;
            threads.pthread = new std::thread(&client::handleSingleThread, this);
            break;
        case threadManageType::thread_pool:
            threads.tPool->addJob([this]{handleThreadPool();});
            break;
    }
}

void client::joinHandler() {
    switch (tMType) {
        case threadManageType::single_thread:
            if(threads.pthread) threads.pthread->join();
            break;
        case threadManageType::thread_pool:
            threads.tPool->join();
            break;
    }
}

void client::sendData(const std::string& data, size_t si2ze) {
    int dataPieces = 1, n;
    int size = data.length();
    if(size > 4096)
        dataPieces = size/4096+1;

    std::string strDataPieces = std::to_string(dataPieces);
    //data size
    n = send(client_socket, std::to_string(size).c_str(), std::to_string(size).length(), 0);
    if(error(n, client_socket)) return;
    if(!receiveSM(SM_OK, client_socket)) return;

    //pieces count
    n = send(client_socket, strDataPieces.c_str(), strDataPieces.length(), 0);
    if(error(n, client_socket)) return;
    if(!receiveSM(SM_OK, client_socket)) return;

    std::string dataPiece;
    //data send
    for(int i = 0; i < dataPieces; i++) {
        dataPiece = data.substr(i*4096, (i+1)*4096);
        n = send(client_socket, dataPiece.c_str(), dataPiece.length(), 0);
        if(error(n, client_socket)) return;
        if(!receiveSM(SM_OK, client_socket)) return;
    }
    sendSM(SM_END, client_socket);
}

uint32_t client::getHost() {
    return NIX(address.sin_addr.s_addr) WIN(address.sin_addr.S_un.S_addr);
}
uint16_t client::getPort() {
    return address.sin_port;
}
