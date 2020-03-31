/*
 * Public file for server and client
 */

#pragma once

enum EnMsgType {
  LOGIN_MSG,         // Login request
  LOGIN_MSG_ACK,     // Login response
  SIGNUP_MSG,        // Signup request
  SIGNUP_MSG_ACK,    // Signup response
  ADD_FRIEND_MSG,    // Add friend request
  CREATE_GROUP_MSG,  // Create group request
  ADD_GROUP_MSG,     // Add group request
  CHAT_MSG,          // Chat request
  GROUP_CHAT_MSG,    // Group chat request
  UPDATE_MSG,        // Update User information request
  UPDATE_MSG_ACK,    // Update User information response
  LOGOUT_MSG,        // Logout request
  LOGOUT_MSG_ACK     // Logout response
};