#pragma once

#include "User.hpp"

// Data operation class for User table
class UserModel {
 public:
  // Insert method for User table
  bool insert(User &user);

  // Query user information according to user ID
  User query(int id);

  // Update user state
  bool updateState(User user);
};