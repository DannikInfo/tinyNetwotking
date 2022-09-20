/**
 * original author is:
 * https://github.com/gbytegear/TcpServer
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <cstdint>
#include <cstddef>

#include "general.h"
#include <memory.h>
#include "baseClient.h"
#include "threadPool.h"
#include "threadManageType.h"

namespace tcp {

class client : public baseClient {


  union thread {
    std::thread* pthread;
    threadPool* tPool;
    thread() : pthread(nullptr) {}
    thread(threadPool* tPool) : tPool(tPool) {}
    ~thread() {}
  };

  socketAddr_in address;
  socket_t client_socket;

  std::mutex handle_mutex;
  std::function<void(std::string)> handler_func = [](const std::string&){};

  threadManageType tMType;
  thread threads;

  status _status = status::disconnected;

  void handleSingleThread();
  void handleThreadPool();

public:
  typedef std::function<void(std::string)> handler_function_t;
  client() noexcept;
  explicit client(threadPool* thread_pool) noexcept;
  virtual ~client() override;

  status connectTo(uint32_t host, uint16_t port) noexcept;
  client::status disconnect() noexcept override;

  uint32_t getHost() override;
  uint16_t getPort() override;
  status getStatus()  override {return _status;}

  std::string loadData() override;
  const std::string&  loadDataSync();
  void setHandler(handler_function_t handler);
  void joinHandler();

  void sendData(const std::string& buffer, size_t size) override;
  socketType getType()  override {return socketType::client_socket;}
};

}

#endif // TCPCLIENT_H
