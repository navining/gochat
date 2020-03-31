#include "login_menu.h"

#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "global.h"
#include "json.hpp"
#include "main_menu.h"
#include "public.h"

using namespace std;
using json = nlohmann::json;

// Main menu exit flag
bool g_mainFlag = false;

void loginMenu() {
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
        printMenu("");
        cout << response["errmsg"].get<string>() << endl;
      } else {
        // Log in succeed
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
      printMenu("");
      cout << name << " already exists!" << endl;
    } else {
      // Sign up succeed
      cout << "Sign up success, your user ID is: " << response["id"]
           << ", do not forget it!" << endl;
    }
  }
}

void printMenu(string msg) {
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