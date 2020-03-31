#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "global.h"
#include "login_menu.h"

using namespace std;

// Client fd
int clientfd = -1;

int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << "Invalid input!\nExample: ./gochat-client 127.0.0.1 8888" << endl;
    exit(-1);
  }
  // Parse ip and port
  char *ip = argv[1];
  uint16_t port = atoi(argv[2]);

  // Create socket
  clientfd = socket(AF_INET, SOCK_STREAM, 0);
  if (clientfd == -1) {
    cerr << "Create socket: failed!" << endl;
    exit(-1);
  }

  // Bind ip and port to server
  sockaddr_in server;
  memset(&server, 0, sizeof(sockaddr_in));

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = inet_addr(ip);

  // Connect server
  if (connect(clientfd, (sockaddr *)&server, sizeof(sockaddr_in)) == -1) {
    cerr << "Connect server: failed! " << endl;
    close(clientfd);
    exit(-1);
  }

  // Enter login menu
  loginMenu();

  return 0;
}