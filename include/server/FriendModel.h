#pragma once

#include <vector>
#include "User.hpp"
using namespace std;

// Data operation for friend table
class FriendModel {
 public:
  // Add friend
  void insert(int userid, int friendid);

  // Return user friend list
  vector<User> query(int userid);
};