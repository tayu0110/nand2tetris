// ErrorHandler.cpp
// Author       : tayu
// Latest Edit  : 2021/03/28
#include"ErrorHandler.h"
#include<iostream>
#include<vector>
#include<string>
#include<cstdlib>

void ErrorHandler::quit() {
  for(auto msg : message) std::cout << msg << std::endl;

  exit(errorCode);
}