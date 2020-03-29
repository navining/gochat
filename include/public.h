/*
 * Public file for server and client
 */

#pragma once

enum EnMsgType {
  LOGIN_MSG = 1,         // Login request
  LOGIN_MSG_ACK = 101,   // Login responce
  SIGNUP_MSG = 2,        // Signup request
  SIGNUP_MSG_ACK = 102,  // Signup responce
  ADD_FRIEND_MSG = 3,    // Add friend request
  CREATE_GROUP_MSG = 4,  // Create group request
  ADD_GROUP_MSG = 5,     // Add group request
  CHAT_MSG = 11,         // Chat request
  GROUP_CHAT_MSG = 12    // Group chat request
};