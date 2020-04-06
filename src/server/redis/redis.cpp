#include "redis.h"

#include <iostream>
using namespace std;

Redis::Redis() : _publish_context(nullptr), _subcribe_context(nullptr) {}

Redis::~Redis() {
  if (_publish_context != nullptr) {
    redisFree(_publish_context);
  }

  if (_subcribe_context != nullptr) {
    redisFree(_subcribe_context);
  }
}

bool Redis::connect() {
  // Context for publish message
  _publish_context = redisConnect("127.0.0.1", 6379);
  if (nullptr == _publish_context) {
    cerr << "connect redis failed!" << endl;
    return false;
  }

  // Context for subscribe message
  _subcribe_context = redisConnect("127.0.0.1", 6379);
  if (nullptr == _subcribe_context) {
    cerr << "connect redis failed!" << endl;
    return false;
  }

  // Monitor events on the channel, int an independent thread
  thread t([&]() { observer_channel_message(); });
  t.detach();

  cout << "connect redis-server success!" << endl;

  return true;
}

bool Redis::publish(int channel, string message) {
  redisReply *reply = (redisReply *)redisCommand(
      _publish_context, "PUBLISH %d %s", channel, message.c_str());
  if (nullptr == reply) {
    cerr << "publish command failed!" << endl;
    return false;
  }
  freeReplyObject(reply);
  return true;
}

bool Redis::subscribe(int channel) {
  // SUBSCRIBE command will block the thread, here we only subscribe the channel
  // Message receive is handled in a specialized method
  // observer_channel_message, with an independent thread
  if (REDIS_ERR ==
      redisAppendCommand(this->_subcribe_context, "SUBSCRIBE %d", channel)) {
    cerr << "subscribe command failed!" << endl;
    return false;
  }
  // redisBufferWrite repeatedly send buffer data to context, untill finished
  int done = 0;
  while (!done) {
    if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done)) {
      cerr << "subscribe command failed!" << endl;
      return false;
    }
  }
  // No need to call redisGetReply here

  return true;
}

bool Redis::unsubscribe(int channel) {
  if (REDIS_ERR ==
      redisAppendCommand(this->_subcribe_context, "UNSUBSCRIBE %d", channel)) {
    cerr << "unsubscribe command failed!" << endl;
    return false;
  }

  int done = 0;
  while (!done) {
    if (REDIS_ERR == redisBufferWrite(this->_subcribe_context, &done)) {
      cerr << "unsubscribe command failed!" << endl;
      return false;
    }
  }
  return true;
}

void Redis::observer_channel_message() {
  redisReply *reply = nullptr;
  while (REDIS_OK == redisGetReply(this->_subcribe_context, (void **)&reply)) {
    // Message is an array with three elements
    if (reply != nullptr && reply->element[2] != nullptr &&
        reply->element[2]->str != nullptr) {
      // Report message on the channel to service module
      _notify_message_handler(atoi(reply->element[1]->str),
                              reply->element[2]->str);
    }

    freeReplyObject(reply);
  }

  cerr << ">>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<" << endl;
}

void Redis::init_notify_handler(function<void(int, string)> fn) {
  this->_notify_message_handler = fn;
}