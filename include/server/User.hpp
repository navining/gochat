#pragma once

#include <string>
using namespace std;

// ORM class for User table
class User {
 public:
  User(int id = -1, string name = "", string pwd = "",
       string state = "offline") {
    this->id = id;
    this->name = name;
    this->password = pwd;
    this->state = state;
  }

  void setId(int id) { this->id = id; }

  void setName(string name) { this->name = name; }

  void setPwd(string pwd) { this->password = pwd; }

  void setState(string state) { this->state = state; }

  int getId() { return id; }

  string getName() { return name; }

  string getPwd() { return password; }

  string getState() { return state; }

 private:
  int id;
  string name;
  string password;
  string state;
};
