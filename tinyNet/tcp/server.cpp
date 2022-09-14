/**
 * original author is:
 * https://github.com/gbytegear/TcpServer
 */

#include "server.h"
#include "serverClient.h"
#include <chrono>
#include <cstring>
#include <mutex>

using namespace tcp;


#ifdef _WIN32
#define WIN(exp) exp
#define NIX(exp)

#else
#define WIN(exp)
#define NIX(exp) exp
#endif

server::server(const uint16_t port,
               KeepAliveConfig ka_conf,
               handler_function_t handler,
               con_handler_function_t connect_hndl,
               con_handler_function_t disconnect_hndl,
               uint thread_count
    )
    : port(port),
    handler(handler),
    connect_hndl(connect_hndl),
    disconnect_hndl(disconnect_hndl),
    tPool(thread_count),
    ka_conf(ka_conf)
    {}

server::~server() {
    if(_status == serverStatus::up)
        stop();
}

void server::setHandler(server::handler_function_t handler) { this->handler = handler;}

uint16_t server::getPort() const {return port;}
uint16_t server::setPort(const uint16_t port) {
	this->port = port;
	start();
	return port;
}

serverStatus server::start() {
    int flag;
    if(_status == serverStatus::up) stop();

  socketAddr_in address;
  address.sin_addr
      WIN(.S_un.S_addr)NIX(.s_addr) = INADDR_ANY;
  address.sin_port = htons(port);
  address.sin_family = AF_INET;


  if((serv_socket = socket(AF_INET, SOCK_STREAM NIX(| SOCK_NONBLOCK), 0)) WIN(== INVALID_SOCKET)NIX(== -1))
     return _status = serverStatus::err_socket_init;

  // Set nonblocking accept
//  NIX( // not needed becouse socket created with flag SOCK_NONBLOCK
//  if(fcntl(serv_socket, F_SETFL, fcntl(serv_socket, F_GETFL, 0) | O_NONBLOCK) < 0) {
//    return _status = status::err_socket_init;
//  })
  WIN(
  if(unsigned long mode = 0; ioctlsocket(serv_socket, FIONBIO, &mode) == SOCKET_ERROR) {
    return _status = status::err_socket_init;
  })


  // Bind address to socket
  if(flag = true;
     (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, WIN((char*))&flag, sizeof(flag)) == -1) ||
     (bind(serv_socket, (struct sockaddr*)&address, sizeof(address)) WIN(== SOCKET_ERROR)NIX(< 0)))
     return _status = serverStatus::err_socket_bind;

  if(listen(serv_socket, SOMAXCONN) WIN(== SOCKET_ERROR)NIX(< 0))
    return _status = serverStatus::err_socket_listening;
  _status = serverStatus::up;
  tPool.addJob([this]{handlingAcceptLoop();});
  tPool.addJob([this]{waitingDataLoop();});
  return _status;
}

void server::stop() {
  tPool.dropUnstartedJobs();
  _status = serverStatus::close;
  WIN(closesocket)NIX(close)(serv_socket);
  client_list.clear();
}

void server::joinLoop() {tPool.join();}

bool server::connectTo(uint32_t host, uint16_t port, con_handler_function_t connect_hndl) {
    sock client_socket;
    socketAddr_in address;
    if((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) WIN(== INVALID_SOCKET) NIX(< 0)) return false;

  new(&address) socketAddr_in;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = host;
  WIN(address.sin_addr.S_un.S_addr = host;)
  NIX(address.sin_addr.s_addr = host;)

  address.sin_port = htons(port);

  if(connect(client_socket, (sockaddr *)&address, sizeof(address))
     WIN(== SOCKET_ERROR)NIX(!= 0)) {
    WIN(closesocket(client_socket);)NIX(close(client_socket);)
    return false;
  }

  if(!enableKeepAlive(client_socket)) {
    shutdown(client_socket, 0);
    WIN(closesocket)NIX(close)(client_socket);
  }

  std::unique_ptr<serverClient> client(new serverClient(client_socket, address));
  connect_hndl(*client);
  client_mutex.lock();
  client_list.emplace_back(std::move(client));
  client_mutex.unlock();
  return true;
}

void server::sendData(const std::string& buffer, const size_t size) {
  for(std::unique_ptr<serverClient>& client : client_list)
    client->sendData(buffer, size);
}

bool server::sendDataBy(uint32_t host, uint16_t port, const std::string& buffer, const size_t size) {
  bool data_is_sended = false;
  for(std::unique_ptr<serverClient>& client : client_list)
    if(client->getHost() == host &&
       client->getPort() == port) {
      client->sendData(buffer, size);
      data_is_sended = true;
    }
  return data_is_sended;
}

bool server::disconnectBy(uint32_t host, uint16_t port) {
  bool client_is_disconnected = false;
  for(std::unique_ptr<serverClient>& client : client_list)
    if(client->getHost() == host &&
       client->getPort() == port) {
      client->disconnect();
      client_is_disconnected = true;
    }
  return client_is_disconnected;
}

void server::disconnectAll() {
  for(std::unique_ptr<serverClient>& client : client_list)
    client->disconnect();
}

void server::handlingAcceptLoop() {
  sockLen_t addrlen = sizeof(socketAddr_in);
  socketAddr_in client_addr;
  if (sock client_socket =
      WIN(accept(serv_socket, (struct sockaddr*)&client_addr, &addrlen))
      NIX(accept4(serv_socket, (struct sockaddr*)&client_addr, &addrlen, SOCK_NONBLOCK));
      client_socket WIN(!= 0)NIX(>= 0) && _status == serverStatus::up) {

    // Enable keep alive for client
    if(enableKeepAlive(client_socket)) {
      std::unique_ptr<serverClient> client(new serverClient(client_socket, client_addr));
      connect_hndl(*client);
      client_mutex.lock();
      client_list.emplace_back(std::move(client));
      client_mutex.unlock();
    } else {
      shutdown(client_socket, 0);
      WIN(closesocket)NIX(close)(client_socket);
    }
  }

  if(_status == serverStatus::up)
    tPool.addJob([this](){handlingAcceptLoop();});
}

void server::waitingDataLoop() {
  {
    std::lock_guard lock(client_mutex);
    for(auto it = client_list.begin(), end = client_list.end(); it != end; ++it) {
      auto& client = *it;
      if(client){
        if(std::string data = client->loadData(); !data.empty()) {

          tPool.addJob([this, _data = std::move(data), &client]{
            client->access_mtx.lock();
            handler(_data, *client);
            client->access_mtx.unlock();
          });
        } else if(client->_status == socketStatus::disconnected) {

          tPool.addJob([this, &client, it]{
            client->access_mtx.lock();
            serverClient* pointer = client.release();
            client = nullptr;
            pointer->access_mtx.unlock();
            disconnect_hndl(*pointer);
            client_list.erase(it);
            delete pointer;
          });
        }
      }
    }
  }

  if(_status == serverStatus::up)
    tPool.addJob([this](){waitingDataLoop();});
}

bool server::enableKeepAlive(tcp::sock socket) {
  int flag = 1;
#ifdef _WIN32
  tcp_keepalive ka {1, ka_conf.ka_idle * 1000, ka_conf.ka_intvl * 1000};
  if (setsockopt (socket, SOL_SOCKET, SO_KEEPALIVE, (const char *) &flag, sizeof(flag)) != 0) return false;
  unsigned long numBytesReturned = 0;
  if(WSAIoctl(socket, SIO_KEEPALIVE_VALS, &ka, sizeof (ka), nullptr, 0, &numBytesReturned, 0, nullptr) != 0) return false;
#else //POSIX
  if(setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag)) == -1) return false;
  if(setsockopt(socket, IPPROTO_TCP, TCP_KEEPIDLE, &ka_conf.ka_idle, sizeof(ka_conf.ka_idle)) == -1) return false;
  if(setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, &ka_conf.ka_intvl, sizeof(ka_conf.ka_intvl)) == -1) return false;
  if(setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, &ka_conf.ka_cnt, sizeof(ka_conf.ka_cnt)) == -1) return false;
#endif
  return true;
}