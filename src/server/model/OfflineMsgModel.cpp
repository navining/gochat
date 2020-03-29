#include "OfflineMsgModel.h"
#include <iostream>
#include "Database.h"
using namespace std;

void OfflineMsgModel::insert(int id, string msg) {
  // Assemble sql command
  char sql[256] = {0};
  sprintf(sql, "insert into offlinemessage values('%d', '%s')", id,
          msg.c_str());

  MySQL mysql;
  if (mysql.connect()) {
    mysql.update(sql);
  }
}

void OfflineMsgModel::remove(int id) {
  // Assemble sql command
  char sql[256] = {0};
  sprintf(sql, "delete from offlinemessage where userid = %d", id);

  MySQL mysql;
  if (mysql.connect()) {
    mysql.update(sql);
  }
}

vector<string> OfflineMsgModel::query(int id) {
  // Assemble sql command
  char sql[256] = {0};
  sprintf(sql, "select message from offlinemessage where userid = %d", id);

  vector<string> vec;
  MySQL mysql;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != NULL) {
      // Put all offline messages into vector
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != NULL) {
        vec.push_back(row[0]);
      }
      mysql_free_result(res);
    }
  }

  return vec;
}
