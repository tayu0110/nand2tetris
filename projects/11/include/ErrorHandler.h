// ErrorHandler.h
// Author       : tayu
// Latest Edit  : 2021/03/28
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include<vector>
#include<string>

#define SYNTAX_ERROR    1
#define INTERNAL_ERROR  2

class ErrorHandler {
  int errorCode;
  std::vector<std::string> message;
public:
  ErrorHandler() : errorCode(0), message(0) {}
  ErrorHandler(int ec, std::vector<std::string> msg) : errorCode(ec), message(msg) {}
  ErrorHandler(const ErrorHandler &eh) : errorCode(eh.errorCode), message(eh.message) {}
  void quit();
};

#endif