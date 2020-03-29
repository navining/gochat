#include <iostream>
#include <vector>
using namespace std;

#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "Group.hpp"
#include "User.hpp"
#include "json.hpp"
#include "public.h"
using json = nlohmann::json;

// Record current user
User g_user;

// Record friend list
vector<User> g_friendList;

// Record group list
vector<Group> g_groupList;

// Error message buffer
char g_errMsg[128] = {0};

// Write into error buffer
void setErr(string err) { sprintf(g_errMsg, err.c_str(), 0); }

// Print from error buffer and clear it
void printErr() {
  cerr << g_errMsg;
  g_errMsg[0] = '\0';
}

// Print current user's information
void printUserInfo();

// Thread for read from client
void readHandler(int clientfd);

// Get system time
string getTime();

// Chat menu
void mainMenu(int);

int main(int argc, char **argv) {
  if (argc < 3) {
    cerr << "Invalid input!\nExample: ./gochat-client 127.0.0.1 8888" << endl;
    exit(-1);
  }
  // Parse ip and port
  char *ip = argv[1];
  uint16_t port = atoi(argv[2]);

  // Create socket
  int clientfd = socket(AF_INET, SOCK_STREAM, 0);
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

  // Main thread used to read user input and send data to server
  for (;;) {
    system("clear");
    // Menu
    cout << "     GoChat v1.0     " << endl;
    cout << "=====================" << endl;
    cout << "|    1 - Log In     |" << endl;
    cout << "|    2 - Sign Up    |" << endl;
    cout << "|    3 - Quit       |" << endl;
    cout << "=====================" << endl;
    printErr();
    cout << "Choice: ";
    int choice = 0;
    cin >> choice;
    cin.get();
    while (cin.fail()) {
      cin.clear();
      cin.ignore();
    }

    switch (choice) {
      case 1:  // Login
        break;
      case 2:  // Signup
        break;
      case 3:  // Quit
        close(clientfd);
        cout << "Bye!" << endl;
        exit(0);
      default:
        setErr("Invalid input!\n");
        break;
    }
  }
  return 0;
}

void printUserInfo() {
  cout << "                   GoChat v1.0                  " << endl;
  cout << "===================== User =====================" << endl;
  cout << "ID: " << g_user.getId() << " Name: " << g_user.getName() << endl;
  cout << "==================== Friends ===================" << endl;
  if (!g_friendList.empty()) {
    for (User &user : g_friendList) {
      cout << "" << user.getId() << "\t" << user.getName() << "\t"
           << user.getState() << endl;
    }
  }
  cout << "==================== Groups ====================" << endl;
  if (!g_groupList.empty()) {
    for (Group &group : g_groupList) {
      cout << "" << group.getId() << "\t" << group.getName() << "\t"
           << group.getDesc() << endl;
      for (GroupUser &user : group.getUsers()) {
        cout << "\t-" << user.getId() << "\t" << user.getName() << "\t"
             << user.getState() << endl;
      }
    }
  }
  cout << "================================================" << endl;
}