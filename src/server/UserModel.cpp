#include "UserModel.h"
#include <iostream>
#include "Database.h"
using namespace std;

bool UserModel::insert(User &user) {
  // Assemble sql command
  char sql[256] = {0};
  sprintf(
      sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
      user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

  MySQL mysql;
  if (mysql.connect()) {
    if (mysql.update(sql)) {
      // Set user ID
      user.setId(mysql_insert_id(mysql.getConnection()));
      return true;
    }
  }

  return false;
}