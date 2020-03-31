#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
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
int clientfd = -1;

// Main menu exit flag
bool g_mainFlag = false;

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

// Update user information
void updateUserInfo();

// Handle login
void login();

// Handle signup
void signup();

// Thread for receive from client
void recvHandler(int clientfd);

// Get system time
string getTime();

// Chat menu
void mainMenu(const json &response);

// help command handler
void help(string str = "");

// chat command handler
void chat(string str);

// addfriend command handler
void addFriend(string str);

// creategroup command handler
void createGroup(string str);

// addgroup command handler
void addGroup(string str);

// groupchat command handler
void groupChat(string str);

// quit command handler
void quit(string str = "");

// clear command handler
void clear(string str = "");

// show command handler
void show(string str = "");

// Command list
unordered_map<string, string> commandMap = {
    {"help (h)", "Show all commands.\n\t[help]"},
    {"chat (c)", "One-to-one chat.\n\t[chat:id:message]"},
    {"addfriend (af)", "Add a friend.\n\t[addfriend:id]"},
    {"creategroup (cg)", "Create a group\n\t[creategroup:name:description]"},
    {"addgroup (ag)", "Add into a group.\n\t[addgroup:id]"},
    {"groupchat (gc)", "Group chat.\n\t[groupchat:id:message]"},
    {"quit (q)", "Log out.\n\t[quit]"},
    {"clear (cl)", "Clear the screen.\n\t[clear]"},
    {"show (s)", "Show friends and groups.\n\t[show]"}};

// Function for commands
unordered_map<string, function<void(string)>> commandHandlerMap = {
    {"help", help},
    {"chat", chat},
    {"addfriend", addFriend},
    {"creategroup", createGroup},
    {"addgroup", addGroup},
    {"groupchat", groupChat},
    {"quit", quit},
    {"clear", clear},
    {"show", show},

    // Alias
    {"h", help},
    {"c", chat},
    {"af", addFriend},
    {"cg", createGroup},
    {"ag", addGroup},
    {"gc", groupChat},
    {"q", quit},
    {"cl", clear},
    {"s", show}};

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
    cout << ">> ";
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
  cout << "                   GoChat v1.0                  " << endl;
  cout << "===================== User =====================" << endl;
  cout << "  [" << g_user.getId() << "]\t" << g_user.getName() << endl;
  cout << "==================== Friends ===================" << endl;
  if (!g_friendList.empty()) {
    for (User &user : g_friendList) {
      cout << "  [" << user.getId() << "]\t" << user.getName() << "\t"
           << user.getState() << endl;
    }
  }
  cout << "==================== Groups ====================" << endl;
  if (!g_groupList.empty()) {
    for (Group &group : g_groupList) {
      cout << "- [" << group.getId() << "]\t" << group.getName() << "\t["
           << group.getDesc() << "]" << endl;
      for (GroupUser &user : group.getUsers()) {
        cout << "  [" << user.getId() << "]\t" << user.getName() << "\t"
             << user.getState() << "\t" << user.getRole() << endl;
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

        // Start new thread for reading data (only once!)
        thread recvTask(recvHandler, clientfd);
        recvTask.detach();

        // Enter main menu
        g_mainFlag = true;
        mainMenu(response);
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
           << ", do not forget it!" << endl;
    }
  }
}

void recvHandler(int clientfd) {
  for (;;) {
    char buffer[BUFFER_SIZE] = {0};
    int len = recv(clientfd, buffer, BUFFER_SIZE, 0);
    if (len == -1 || len == 0) {
      close(clientfd);
      exit(-1);
    }

    // Receive data from chat server
    json js = json::parse(buffer);

    if (js["msgid"].get<int>() == CHAT_MSG) {
      // Chat message
      cout << "\n"
           << js["time"].get<string>() << " [" << js["id"] << "] "
           << js["name"].get<string>() << ": " << js["msg"].get<string>()
           << endl;
    }

    if (js["msgid"].get<int>() == GROUP_CHAT_MSG) {
      // Group chat message
      cout << "\n"
           << js["time"].get<string>() << " [" << js["groupid"] << "] "
           << js["groupname"].get<string>() << ": "
           << "[" << js["id"] << "] " << js["name"].get<string>() << ": "
           << js["msg"].get<string>() << endl;
    }

    if (js["msgid"].get<int>() == UPDATE_MSG_ACK) {
      // Update user information
      getUserInfo(js);
    }

    if (js["msgid"].get<int>() == LOGOUT_MSG) {
      // Log out
      return;
    }
  }
}

void mainMenu(const json &response) {
  // User information
  system("clear");
  printUserInfo();

  // Show offline message
  if (response.contains("offlinemsg")) {
    vector<string> offlinemsg = response["offlinemsg"];
    for (string &msg : offlinemsg) {
      json js = json::parse(msg);
      if (js["msgid"].get<int>() == CHAT_MSG) {
        // Chat message
        cout << "[History] " << js["time"].get<string>() << " [" << js["id"]
             << "] " << js["name"].get<string>() << ": "
             << js["msg"].get<string>() << endl;
      }

      if (js["msgid"].get<int>() == GROUP_CHAT_MSG) {
        // Group chat message
        cout << "[History] " << js["time"].get<string>() << " ["
             << js["groupid"] << "] " << js["groupname"].get<string>() << ": "
             << "[" << js["id"] << "] " << js["name"].get<string>() << ": "
             << js["msg"].get<string>() << endl;
      }
    }
  }

  // Command line
  cout << "(Try \"help\")" << endl;
  char buffer[BUFFER_SIZE] = {0};

  while (g_mainFlag) {
    cout << ">> ";
    cin.getline(buffer, BUFFER_SIZE);
    string cmdBuffer(buffer);
    string cmd;  // Store the command

    int idx = cmdBuffer.find(":");
    if (idx == -1) {
      cmd = cmdBuffer;
    } else {
      cmd = cmdBuffer.substr(0, idx);
    }

    auto it = commandHandlerMap.find(cmd);
    if (it == commandHandlerMap.end()) {
      cerr << "Invalid input! Try \"help\"." << endl;
      continue;
    }

    // Call event handler
    string arg = cmdBuffer.substr(idx + 1, cmdBuffer.size() - idx);
    it->second(arg);
  }

  printMenu("");
}

void getUserInfo(const json &response) {
  // Record user information
  g_user.setId(response["id"].get<int>());
  g_user.setName(response["name"]);

  // Record friend list
  g_friendList.clear();
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
  g_groupList.clear();
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

void help(string str) {
  cout << "Command lists >>>" << endl;
  for (auto &command : commandMap) {
    cout << " - " << command.first << ": " << command.second << endl;
  }
}

void chat(string str) {
  int idx = str.find(":");
  if (idx == -1) {
    cerr << "Invaild input! Try \"help\"." << endl;
    return;
  }

  int friendid = atoi(str.substr(0, idx).c_str());
  string message = str.substr(idx + 1, str.size() - idx);

  // Find if friend exists in friendlist
  User user(friendid);
  auto it = find(g_friendList.begin(), g_friendList.end(), user);
  if (it == g_friendList.end() && friendid != g_user.getId()) {
    cerr << "Friend not added. Use addfriend first." << endl;
    return;
  }

  json js;
  js["msgid"] = CHAT_MSG;
  js["id"] = g_user.getId();
  js["name"] = g_user.getName();
  js["to"] = friendid;
  js["msg"] = message;
  js["time"] = getTime();

  string buffer = js.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
  if (len == -1) {
    cerr << "Send message failed." << endl;
  }
}

void addFriend(string str) {
  int friendid = atoi(str.c_str());

  if (friendid == 0) {
    cerr << "Invalid input!" << endl;
    return;
  } else if (friendid == g_user.getId()) {
    cerr << "You can not add yourself!" << endl;
    return;
  }

  json js;
  js["msgid"] = ADD_FRIEND_MSG;
  js["id"] = g_user.getId();
  js["friend"] = friendid;

  string buffer = js.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
  if (len == -1) {
    cerr << "Add friend failed." << endl;
  } else {
    cout << "New friend added!" << endl;
    // Update user information
    updateUserInfo();
  }
}

void createGroup(string str) {
  int idx = str.find(":");
  if (idx == -1) {
    cerr << "Invaild input! Try \"help\"." << endl;
    return;
  }

  string groupname = str.substr(0, idx);
  string groupdesc = str.substr(idx + 1, str.size() - idx);

  json js;
  js["msgid"] = CREATE_GROUP_MSG;
  js["id"] = g_user.getId();
  js["groupname"] = groupname;
  js["groupdesc"] = groupdesc;

  string buffer = js.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
  if (len == -1) {
    cerr << "Create group failed." << endl;
  } else {
    cout << "New group created!" << endl;
    // Update user information
    updateUserInfo();
  }
}

void addGroup(string str) {
  int groupid = atoi(str.c_str());
  if (groupid == 0) {
    cerr << "Invalid input!" << endl;
    return;
  }

  json js;
  js["msgid"] = ADD_GROUP_MSG;
  js["id"] = g_user.getId();
  js["groupid"] = groupid;

  string buffer = js.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
  if (len == -1) {
    cerr << "Add group failed." << endl;
  } else {
    cout << "New group added!" << endl;
    // Update user information
    updateUserInfo();
  }
}

void groupChat(string str) {
  int idx = str.find(":");
  if (idx == -1) {
    cerr << "Invaild input! Try \"help\"." << endl;
    return;
  }

  int groupid = atoi(str.substr(0, idx).c_str());
  string message = str.substr(idx + 1, str.size() - idx);

  // Find if group exists in grouplist
  Group group(groupid);
  auto it = find(g_groupList.begin(), g_groupList.end(), group);
  if (it == g_groupList.end()) {
    cerr << "Group not added. Use addgroup first." << endl;
    return;
  }

  json js;
  js["msgid"] = GROUP_CHAT_MSG;
  js["id"] = g_user.getId();
  js["name"] = g_user.getName();
  js["groupid"] = groupid;
  js["groupname"] = it->getName();
  js["msg"] = message;
  js["time"] = getTime();

  string buffer = js.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
  if (len == -1) {
    cerr << "Send message failed." << endl;
  }
}

void quit(string str) {
  json js;
  js["msgid"] = LOGOUT_MSG;
  js["id"] = g_user.getId();
  js["name"] = g_user.getName();

  string buffer = js.dump();
  int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
  if (len == -1) {
    cerr << "Log out: failed!" << endl;
  } else {
    g_mainFlag = false;
  }
}

void clear(string str) {
  updateUserInfo();
  system("clear");
  printUserInfo();
}

void show(string str) {
  updateUserInfo();
  printUserInfo();
}

void updateUserInfo() {
  json js;
  js["msgid"] = UPDATE_MSG;
  js["id"] = g_user.getId();

  string buffer = js.dump();
  send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
}

string getTime() {
  auto tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
  struct tm *ptm = localtime(&tt);
  char date[60] = {0};
  sprintf(
      date, /*"%d-%02d-%02d*/
      "%02d:%02d:%02d",
      /*(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday, */
      (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
  return string(date);
}