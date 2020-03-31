#pragma once
#include <string>

#include "json.hpp"
#include "login_menu.h"

using namespace std;
using json = nlohmann::json;

// Get user information
void getUserInfo(const json &response);

// Print current user's information
void printUserInfo();

// Update user information
void updateUserInfo();

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