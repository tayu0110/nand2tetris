// JackAnalyzer.cpp
// Author       : tayu
// Latest Edit  : 2021/03/28
#include"JackAnalyzer.h"
#include"FileHandler.h"
#include"JackTokenizer.h"
#include"CompilationEngine.h"
#include"VMWriter.h"
#include"ErrorHandler.h"
#include<iostream>
#include<string>
#include<vector>

JackAnalyzer::JackAnalyzer(std::string cmdArg) {
  this->fileHandles = getFileHandle(cmdArg);
}

void JackAnalyzer::Analyze() {
  for(auto files : this->fileHandles) {
    std::string jackCode;

    files.getJackCode(jackCode);

    JackTokenizer jt(jackCode);
    VMWriter vw(files);
    CompilationEngine ce(files, jt, vw);


    try {
      ce.compile();
    } catch(ErrorHandler &eh) {
      eh.quit();
    }
  }

  return;
}