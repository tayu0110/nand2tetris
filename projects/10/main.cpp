// main.cpp
// Author       : tayu
// Latest Edit  : 2021/02/27
#include<iostream>
#include<cstdlib>
#include"FileHandler.h"
#include"JackTokenizer2.h"
#include"CompilationEngine2.h"
#include"JackAnalyzer.h"

int main(int argc, char* argv[]) {
  if(argc < 2) {
    std::cout << "Error: (main.cpp:13): Argument is too less." << std::endl;
    exit(1);
  }
  if(argc > 2) {
    std::cout << "Error (main.cpp:17): Arguments are too more." << std::endl;
    exit(1);
  }

  std::string argument = argv[1];

  JackAnalyzer ja(argument);

  ja.Analyze();
  
  exit(0);
}