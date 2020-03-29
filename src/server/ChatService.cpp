#include "ChatService.h"
#include <muduo/base/Logging.h>
#include <vector>
#include "public.h"

using namespace muduo;
using namespace std;

ChatService* ChatService::instance() {
  static ChatService service;
  return &service;
}

ChatService::ChatService() {
  // Bind message type and message handler
  _msgHandlerMap.insert(
      {LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)});
  _msgHandlerMap.insert(
      {SIGNUP_MSG, bind(&ChatService::signup, this, _1, _2, _3)});
  _msgHandlerMap.insert({CHAT_MSG, bind(&ChatService::chat, this, _1, _2, _3)});
  _msgHandlerMap.insert(
      {ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3)});
  _msgHandlerMap.insert(
      {CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3)});
  _msgHandlerMap.insert(
      {ADD_GROUP_MSG, bind(&ChatService::addGroup, this, _1, _2, _3)});
  _msgHandlerMap.insert(
      {GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3)});
}

msgHandler ChatService::getHandler(int msgid) {
  auto it = _msgHandlerMap.find(msgid);
  if (it == _msgHandlerMap.end()) {
    // Return the default handler, write error into log
    return [=](const TcpConnectionPtr& conn, json& js, Timestamp& time) {
      LOG_ERROR << "Can't find the handler! Message ID: " << msgid;
    };
  } else {
    return _msgHandlerMap[msgid];
  }
}

void ChatService::clientCloseException(const TcpConnectionPtr& conn) {
  User user;
  {
    lock_guard<mutex> lock(_connMutex);
    for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++) {
      if (it->second == conn) {
        user.setId(it->first);
        // Delete user connection from map
        _userConnMap.erase(it);
        break;
      }
    }
  }

  // Update user state
  if (user.getId() != -1) {
    user.setState("offline");
  }
  _userModel.updateState(user);
}

void ChatService::reset() {
  // Set all user state to offline
  _userModel.resetState();
}

void ChatService::login(const TcpConnectionPtr& conn, json& js,
                        Timestamp& time) {
  int id = js["id"].get<int>();
  string pwd = js["password"];

  User user = _userModel.query(id);
  if (user.getId() == id && user.getPwd() == pwd) {
    if (user.getState() == "online") {
      // User already login
      json response;
      response["msgid"] = LOGIN_MSG_ACK;
      response["errid"] = 2;
      response["errmsg"] = "User already login";
      conn->send(response.dump());
    } else {
      // Login succeed
      LOG_INFO << "[" << user.getId() << ": " << user.getName()
               << "] logged in";
      // Record user connection
      {
        lock_guard<mutex> lock(_connMutex);
        _userConnMap.insert({user.getId(), conn});
      }

      // Update user state
      user.setState("online");
      _userModel.updateState(user);

      json response;
      response["msgid"] = LOGIN_MSG_ACK;
      response["errid"] = 0;
      response["id"] = user.getId();
      response["name"] = user.getName();

      // Query offline message, and delete
      vector<string> offlineMsgVec = _offlineMsgModel.query(user.getId());
      if (!offlineMsgVec.empty()) {
        response["offlinemsg"] = offlineMsgVec;
        _offlineMsgModel.remove(user.getId());
      }

      // Query friend information
      vector<User> friendVec = _friendModel.query(user.getId());
      if (!friendVec.empty()) {
        vector<string> userVec;
        for (User& u : friendVec) {
          json js;
          js["id"] = u.getId();
          js["name"] = u.getName();
          js["state"] = u.getState();
          userVec.push_back(js.dump());
        }
        response["friends"] = userVec;
      }

      conn->send(response.dump());
    }

  } else {
    // Login fail
    json response;
    response["msgid"] = LOGIN_MSG_ACK;
    response["errid"] = 1;
    response["errmsg"] = "Invalid ID or password";
    conn->send(response.dump());
  }
}

void ChatService::signup(const TcpConnectionPtr& conn, json& js,
                         Timestamp& time) {
  string name = js["name"];
  string pwd = js["password"];

  User user;
  user.setName(name);
  user.setPwd(pwd);
  bool state = _userModel.insert(user);
  if (state) {
    // Signup succeed
    LOG_INFO << "[" << user.getId() << ": " << user.getName() << "] signed up";

    json response;
    response["msgid"] = SIGNUP_MSG_ACK;
    response["errid"] = 0;
    response["id"] = user.getId();
    conn->send(response.dump());
  } else {
    // Signup fail
    json response;
    response["msgid"] = SIGNUP_MSG_ACK;
    response["errid"] = 1;
    response["errmsg"] = "User already exists";
    conn->send(response.dump());
  }
}

void ChatService::chat(const TcpConnectionPtr& conn, json& js,
                       Timestamp& time) {
  int toId = js["to"].get<int>();

  {
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(toId);
    if (it != _userConnMap.end()) {
      // User online, send message
      it->second->send(js.dump());
      return;
    }
  }

  // User offline, save offline message
  _offlineMsgModel.insert(toId, js.dump());
}

void ChatService::addFriend(const TcpConnectionPtr& conn, json& js,
                            Timestamp& time) {
  int userid = js["id"].get<int>();
  int friendid = js["friend"].get<int>();

  // Save friend information
  _friendModel.insert(userid, friendid);
}

void ChatService::createGroup(const TcpConnectionPtr& conn, json& js,
                              Timestamp& time) {
  int userid = js["id"].get<int>();
  string name = js["groupname"];
  string desc = js["groupdesc"];

  // Store group information
  Group group(-1, name, desc);
  if (_groupModel.create(group)) {
    // Store group creator information
    _groupModel.add(userid, group.getId(), ROLE_OWNER);
  }
}

void ChatService::addGroup(const TcpConnectionPtr& conn, json& js,
                           Timestamp& time) {
  int userid = js["id"].get<int>();
  int groupid = js["groupid"].get<int>();
  _groupModel.add(userid, groupid, ROLE_MEMBER);
}

void ChatService::groupChat(const TcpConnectionPtr& conn, json& js,
                            Timestamp& time) {
  int userid = js["id"].get<int>();
  int groupid = js["groupid"].get<int>();
  vector<int> useridVec = _groupModel.queryUsers(userid, groupid);

  lock_guard<mutex> lock(_connMutex);
  for (int id : useridVec) {
    auto it = _userConnMap.find(id);
    if (it != _userConnMap.end()) {
      // Send message to user
      it->second->send(js.dump());
    } else {
      // Store offline message
      _offlineMsgModel.insert(id, js.dump());
    }
  }
}