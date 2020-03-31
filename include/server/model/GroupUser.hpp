#pragma once

#include "User.hpp"

#define ROLE_OWNER "Owner"
#define ROLE_ADMIN "Admin"
#define ROLE_MEMBER "Member"

// Group user, extends from User
class GroupUser : public User {
 public:
  void setRole(string role) { this->role = role; }

  string getRole() { return role; }

 private:
  string role;
};