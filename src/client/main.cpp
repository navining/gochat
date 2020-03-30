#include <iostream>
#include <string>
#include <thread>
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

#define BUFFER_SIZE 4096

// Client fd
int clientfd;

// Record current user
User g_user;

// Record friend list
vector<User> g_friendList;

// Record group list
vector<Group> g_groupList;

// Print menu
void printMenu(string msg);

// Print current user's information
void printUserInfo();

// Get user information
void getUserInfo(const json &response);

// Handle login
void login();

// Handle signup
void signup();

// Thread for read from client
void readHandler(int clientfd);

// Get system time
string getTime();

// Chat menu
void mainMenu();

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

  printMenu("");
  // Main thread used to read user input and send data to server
  for (;;) {
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
      {
        login();
        break;
      }
      case 2:  // Signup
      {
        signup();
        break;
      }
      case 3:  // Quit
        close(clientfd);
        cout << "Bye!" << endl;
        exit(0);
      default:
        printMenu("Invalid input!\n");
        break;
    }
  }
  return 0;
}

void printMenu(string msg = "") {
  system("clear");
  // Menu
  cout << "     GoChat v1.0     " << endl;
  cout << "=====================" << endl;
  cout << "|    1 - Log In     |" << endl;
  cout << "|    2 - Sign Up    |" << endl;
  cout << "|    3 - Quit       |" << endl;
  cout << "=====================" << endl;
  cout << msg;
}

void printUserInfo() {
  system("clear");
  cout << "                   GoChat v1.0                  " << endl;
  cout << "===================== User =====================" << endl;
  cout << "  " << g_user.getId() << "\t" << g_user.getName() << endl;
  cout << "==================== Friends ===================" << endl;
  if (!g_friendList.empty()) {
    for (User &user : g_friendList) {
      cout << "  " << user.getId() << "\t" << user.getName() << "\t"
           << user.getState() << endl;
    }
  }
  cout << "==================== Groups ====================" << endl;
  if (!g_groupList.empty()) {
    for (Group &group : g_groupList) {
      cout << "  " << group.getId() << "\t" << group.getName() << "\t"
           << group.getDesc() << endl;
      for (GroupUser &user : group.getUsers()) {
        cout << "  \t-" << user.getId() << "\t" << user.getName() << "\t"
             << user.getState() << endl;
      }
    }
  }
  cout << "================================================" << endl;
}

void login() {
  printMenu("[Log In]\n");
  int id = 0;
  char pwd[50] = {0};

  cout << "User ID: ";
  cin >> id;
  cin.get();
  while (cin.fail()) {
    cin.clear();
    cin.ignore();
  }
  if (id == 0) {
    printMenu("Invalid ID!\n");
    return;
  }

  cout << "Password: ";
  cin.getline(pwd, 50);

  json js;
  js["msgid"] = LOGIN_MSG;
  js["id"] = id;
  js["password"] = pwd;
  string request = js.dump();

  int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
  if (len == -1) {
    printMenu("Send request: failed!\n");
  } else {
    char buffer[BUFFER_SIZE] = {0};
    len = recv(clientfd, buffer, BUFFER_SIZE, 0);
    if (len == -1) {
      printMenu("Receive response: failed!\n");
    } else {
      json response = json::parse(buffer);
      if (response["errid"].get<int>() != 0) {
        // Log in failed
        printMenu();
        cout << response["errmsg"].get<string>() << endl;
      } else {
        // Log in succeed
        // Get user information
        getUserInfo(response);

        // Start new thread for reading data
        thread readTask(readHandler, clientfd);
        readTask.detach();

        // Enter main menu
        mainMenu();
      }
    }
  }
}

void signup() {
  printMenu("[Sign Up]\n");
  char name[50] = {0};
  char pwd[50] = {0};
  cout << "Username: ";
  cin.getline(name, 50);
  cout << "Password: ";
  cin.getline(pwd, 50);

  json js;
  js["msgid"] = SIGNUP_MSG;
  js["name"] = name;
  js["password"] = pwd;
  string request = js.dump();

  int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
  if (len == -1) {
    printMenu("Send request: failed!\n");
  } else {
    char buffer[BUFFER_SIZE] = {0};
    len = recv(clientfd, buffer, BUFFER_SIZE, 0);
    if (len == -1) {
      printMenu("Receive response: failed!\n");
    }

    json response = json::parse(buffer);
    if (response["errid"].get<int>() != 0) {
      // Sign up failed
      printMenu();
      cout << name << " already exists!" << endl;
    } else {
      // Sign up succeed
      cout << "Sign up success, your user ID is: " << response["id"]
           << ", do not forget!" << endl;
    }
  }
}

void readHandler(int clientfd) {}

void mainMenu() {
  printUserInfo();
  for (;;) {
    cout << "Choice: ";
    int choice = 0;
    cin >> choice;
    cin.get();
    while (cin.fail()) {
      cin.clear();
      cin.ignore();
    }
  }
}

void getUserInfo(const json &response) {
  // Record user information
  g_user.setId(response["id"].get<int>());
  g_user.setName(response["name"]);

  // Record friend list
  if (response.contains("friends")) {
    vector<string> friends = response["friends"];
    for (string &f : friends) {
      json js = json::parse(f);
      User user;
      user.setId(js["id"].get<int>());
      user.setName(js["name"]);
      user.setState(js["state"]);
      g_friendList.push_back(user);
    }
  }

  // Record group list
  if (response.contains("groups")) {
    vector<string> groups = response["groups"];
    for (string &g : groups) {
      json groupjs = json::parse(g);
      Group group;
      group.setId(groupjs["id"].get<int>());
      group.setName(groupjs["groupname"]);
      group.setDesc(groupjs["groupdesc"]);

      vector<string> users = groupjs["users"];
      for (string &u : users) {
        json js = json::parse(u);
        GroupUser user;
        user.setId(js["id"].get<int>());
        user.setName(js["name"]);
        user.setState(js["state"]);
        user.setRole(js["role"]);
        group.getUsers().push_back(user);
      }
      g_groupList.push_back(group);
    }
  }
}