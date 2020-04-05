#pragma once

#include <string>
using namespace std;

// Main menu exit flag
extern bool g_mainFlag;

// The login menu
void loginMenu();

// Handle login
void login();

// Handle signup
void signup();

// Print menu
void printMenu(string msg = "");