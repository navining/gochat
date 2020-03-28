#include <signal.h>
#include <iostream>
#include "ChatServer.h"
#include "ChatService.h"

using namespace std;

// Handle server exit execption
void resetHandler(int) {
  ChatService::instance()->reset();
  exit(0);
}

int main() {
  signal(SIGINT, resetHandler);

  EventLoop loop;
  InetAddress addr("127.0.0.1", 8888);
  ChatServer server(&loop, addr, "GoChat-Server");

  server.start();
  loop.loop();

  return 0;
}
