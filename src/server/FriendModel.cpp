#include "FriendModel.h"
#include "Database.h"

void FriendModel::insert(int userid, int friendid) {
  // Assemble sql command
  char sql[256] = {0};
  sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);

  MySQL mysql;
  if (mysql.connect()) {
    mysql.update(sql);
  }
}

vector<User> FriendModel::query(int userid) {
  // Assemble sql command
  char sql[256] = {0};
  // Inner join query
  sprintf(sql,
          "select a.id, a.name, a.state from user a inner join friend b on "
          "b.friendid = a.id where b.userid = %d",
          userid);

  vector<User> vec;
  MySQL mysql;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != NULL) {
      // Put all friends into vector
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != NULL) {
        User user;
        user.setId(atoi(row[0]));
        user.setName(row[1]);
        user.setState(row[2]);
        vec.push_back(user);
      }
      mysql_free_result(res);
    }
  }

  return vec;
}