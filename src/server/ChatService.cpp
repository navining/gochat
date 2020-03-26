#include "ChatService.h"
#include <muduo/base/Logging.h>
#include "public.h"

using namespace muduo;

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

void ChatService::login(const TcpConnectionPtr& conn, json& js,
                        Timestamp& time) {
  LOG_INFO << "Successfully Login!";
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
    // Signup success
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
    conn->send(response.dump());
  }
}