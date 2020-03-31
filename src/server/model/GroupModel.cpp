#include "GroupModel.h"

#include "Database.h"

bool GroupModel::create(Group &group) {
  // Assemble sql command
  char sql[256] = {0};
  sprintf(sql, "insert into `group`(groupname, groupdesc) values('%s', '%s')",
          group.getName().c_str(), group.getDesc().c_str());

  MySQL mysql;
  if (mysql.connect()) {
    if (mysql.update(sql)) {
      // Set group ID
      group.setId(mysql_insert_id(mysql.getConnection()));
      return true;
    }
  }

  return false;
}

void GroupModel::add(int userid, int groupid, string role = ROLE_MEMBER) {
  // Assemble sql command
  char sql[256] = {0};
  sprintf(sql, "insert into `groupuser` values(%d, %d, '%s')", groupid, userid,
          role.c_str());

  MySQL mysql;
  if (mysql.connect()) {
    mysql.update(sql);
  }
}

vector<Group> GroupModel::query(int userid) {
  vector<Group> groupVec;

  // Assemble sql command
  char sql[256] = {0};
  // 1. Get group information from userid
  sprintf(sql,
          "select a.id, a.groupname, a.groupdesc from `group` a inner join "
          "`groupuser` b on a.id = b.groupid where b.userid = %d",
          userid);

  MySQL mysql;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != NULL) {
      // Put all groups into vector
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != NULL) {
        Group group;
        group.setId(atoi(row[0]));
        group.setName(row[1]);
        group.setDesc(row[2]);
        groupVec.push_back(group);
      }
      mysql_free_result(res);
    }
  }

  // 2. Get users information from group id
  for (Group &group : groupVec) {
    sprintf(sql,
            "select a.id, a.name, a.state, b.grouprole from `user` a inner "
            "join `groupuser` b on b.userid = a.id where b.groupid = %d",
            group.getId());
    MYSQL_RES *res = mysql.query(sql);
    if (res != NULL) {
      MYSQL_ROW row;
      // Put group users into vector
      while ((row = mysql_fetch_row(res)) != NULL) {
        GroupUser groupuser;
        groupuser.setId(atoi(row[0]));
        groupuser.setName(row[1]);
        groupuser.setState(row[2]);
        groupuser.setRole(row[3]);
        group.getUsers().push_back(groupuser);
      }
      mysql_free_result(res);
    }
  }
  return groupVec;
}

vector<int> GroupModel::queryUsers(int userid, int groupid) {
  // Assemble sql command
  char sql[256] = {0};
  sprintf(sql, "select userid from groupuser where groupid = %d", groupid);

  vector<int> vec;

  MySQL mysql;
  if (mysql.connect()) {
    MYSQL_RES *res = mysql.query(sql);
    if (res != NULL) {
      // Put all user ids into vector
      MYSQL_ROW row;
      while ((row = mysql_fetch_row(res)) != NULL) {
        vec.push_back(atoi(row[0]));
      }
      mysql_free_result(res);
    }
  }

  return vec;
}