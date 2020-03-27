/*
 * Public file for server and client
 */

#pragma once

enum EnMsgType {
  LOGIN_MSG = 1,        // Login request
  SIGNUP_MSG = 2,       // Signup request
  LOGIN_MSG_ACK = 101,  // Login responce
  SIGNUP_MSG_ACK = 102  // Signup responce
};