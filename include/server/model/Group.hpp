#pragma once

#include <string>
#include <vector>
#include "GroupUser.hpp"
using namespace std;

// ORM class for Group table
class Group {
 public:
  Group(int id = -1, string name = "", string desc = "") {
    this->id = id;
    this->name = name;
    this->desc = desc;
  }

  void setId(int id) { this->id = id; }

  void setName(string name) { this->name = name; }

  void setDesc(string desc) { this->desc = desc; }

  int getId() { return id; }

  string getName() { return name; }

  string getDesc() { return desc; }

  vector<GroupUser> getUsers() { return users; }

 private:
  int id;
  string name;
  string desc;
  vector<GroupUser> users;
};