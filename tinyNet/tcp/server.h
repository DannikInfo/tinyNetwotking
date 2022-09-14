#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <functional>
#include <list>

#include <thread>
#include <mutex>
#include <shared_mutex>

#ifdef _WIN32 // Windows NT
#include <WinSock2.h>
#include <mstcpip.h>
#else // *nix

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#include "general.h"
#include "threadPool.h"
#include "serverStatus.h"
#include "serverClient.h"

namespace tcp {

    struct KeepAliveConfig{
      ka_prop_t ka_idle = 120;
      ka_prop_t ka_intvl = 3;
      ka_prop_t ka_cnt = 5;
    };

    class server {
        class clientList;
        typedef std::function<void(std::string, serverClient&)> handler_function_t;
        typedef std::function<void(serverClient&)> con_handler_function_t;
        static constexpr auto default_data_handler = [](const std::string&, serverClient&){};
        static constexpr auto default_connsection_handler = [](serverClient&){};

    private:
        sock serv_socket;
        uint16_t port;
        serverStatus _status = serverStatus::close;
        handler_function_t handler = default_data_handler;
        con_handler_function_t connect_hndl = default_connsection_handler;
        con_handler_function_t disconnect_hndl = default_connsection_handler;

        threadPool tPool;
        typedef std::list<std::unique_ptr<serverClient>>::iterator clientIterator;
        KeepAliveConfig ka_conf;
        std::list<std::unique_ptr<serverClient>> client_list;
        std::mutex client_mutex;

        bool enableKeepAlive(sock socket);
        void handlingAcceptLoop();
        void waitingDataLoop();

    public:
        server(const uint16_t port,
             KeepAliveConfig ka_conf = {},
             handler_function_t handler = default_data_handler,
             con_handler_function_t connect_hndl = default_connsection_handler,
             con_handler_function_t disconnect_hndl = default_connsection_handler,
             uint thread_count = std::thread::hardware_concurrency()
        );

        ~server();

        //! Set client handler
        void setHandler(handler_function_t handler);

      threadPool& getThreadPool() {return tPool;}

      // Server properties getters
      uint16_t getPort() const;
      uint16_t setPort(const uint16_t port);
      serverStatus getStatus() const {return _status;}

      // Server status manip
      serverStatus start();
      void stop();
      void joinLoop();

    //  std::list<std::unique_ptr<Client>> getClients();

      // Server client management
      bool connectTo(uint32_t host, uint16_t port, con_handler_function_t connect_hndl);
      void sendData(const std::string& buffer, const size_t size);
      bool sendDataBy(uint32_t host, uint16_t port, const std::string& buffer, const size_t size);
      bool disconnectBy(uint32_t host, uint16_t port);
      void disconnectAll();
    };

}

#endif // TCPSERVER_H
