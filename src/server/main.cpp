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

int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << "Invalid input!\nExample: ./gochat-client 127.0.0.1 8888" << endl;
    exit(-1);
  }

  // Parse ip and port
  char *ip = argv[1];
  uint16_t port = atoi(argv[2]);

  signal(SIGINT, resetHandler);

  EventLoop loop;
  InetAddress addr(ip, port);
  ChatServer server(&loop, addr, "GoChat-Server");

  server.start();
  loop.loop();

  return 0;
}
