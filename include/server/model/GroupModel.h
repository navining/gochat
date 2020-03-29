#pragma once

#include <string>
#include <vector>
#include "Group.hpp"
using namespace std;

// Data operation for Group table
class GroupModel {
 public:
  // Create a group
  bool create(Group &group);

  // Add user into a group
  void add(int userid, int groupid, string role);

  // Query group information (including group users)
  vector<Group> query(int userid);

  // Query group users from a group in order to send messages
  vector<int> queryUsers(int userid, int groupid);
};