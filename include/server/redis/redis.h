#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>

#include <functional>
#include <thread>
using namespace std;

/*
redis作为集群服务器通信的基于发布-订阅消息队列时，会遇到两个难搞的bug问题，参考我的博客详细描述：
https://blog.csdn.net/QIANGWEIYUAN/article/details/97895611
*/
class Redis {
 public:
  Redis();
  ~Redis();

  // Connect Redis server
  bool connect();

  // Publish message
  bool publish(int channel, string message);

  // Subscribe message
  bool subscribe(int channel);

  // Unsubscribe message
  bool unsubscribe(int channel);

  // Receive message from channel, in an independent thread
  void observer_channel_message();

  // Initialize callback function
  void init_notify_handler(function<void(int, string)> fn);

 private:
  // hiredis context for publish message
  redisContext *_publish_context;

  // hiredis contest for subscribe message
  redisContext *_subcribe_context;

  // Callback function, report to service module
  function<void(int, string)> _notify_message_handler;
};

#endif