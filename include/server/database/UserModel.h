#pragma once

#include "User.hpp"

// Data operation class for User table
class UserModel {
 public:
  // Insert method for User table
  bool insert(User &user);
};