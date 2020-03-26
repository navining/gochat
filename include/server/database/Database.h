#pragma once

#include <mysql/mysql.h>
#include <string>

using namespace std;

class MySQL {
 public:
  // Initialize database connection
  MySQL();

  // Release database connection resources
  ~MySQL();

  // Connect database
  bool connect();

  // Update database
  bool update(string sql);

  // Query database
  MYSQL_RES *query(string sql);

  // Get connection
  MYSQL *getConnection();

 private:
  MYSQL *_conn;
};