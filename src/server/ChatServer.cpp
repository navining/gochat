#include "ChatServer.h"
#include <muduo/base/Logging.h>
#include "ChatService.h"
#include "json.hpp"
using json = nlohmann::json;

#include <functional>
#include <string>
using namespace std;
using namespace placeholders;
ChatServer::ChatServer(EventLoop* loop, const InetAddress& listenAddr,
                       const string& nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop) {
  // Register connection callback
  _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
  // Register message callback
  _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));
  // Set number of threads
  _server.setThreadNum(8);
}

void ChatServer::start() {
  LOG_INFO << "Server started, waitting for connection.";
  _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& conn) {
  // Client disconnected
  if (!conn->connected()) {
    ChatService::instance()->clientCloseException(conn);
    conn->shutdown();
  }
}

void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer,
                           Timestamp time) {
  string buf = buffer->retrieveAllAsString();
  // Deserialize data
  json js = json::parse(buf);
  // Get the message handler
  auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
  msgHandler(conn, js, time);
}