#pragma once

#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer {
 public:
  // Initialize ChatServer object
  ChatServer(EventLoop* loop, const InetAddress& listenAddr,
             const string& nameArg);

  // Start the service
  void start();

 private:
  TcpServer _server;
  EventLoop* _loop;

  // Callback function for user connection
  void onConnection(const TcpConnectionPtr& conn);

  // Callback function for I/O message
  void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time);
};
