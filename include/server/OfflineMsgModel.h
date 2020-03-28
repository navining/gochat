#pragma once

#include <string>
#include <vector>
using namespace std;
// Data operation for offlinemessage table
class OfflineMsgModel {
 public:
  // Save offline message
  void insert(int id, string msg);

  // Delete offline message
  void remove(int id);

  // Query offline message
  vector<string> query(int id);
};