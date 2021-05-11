// JackAnalyzer.cpp
// Author       : tayu
// Latest Edit  : 2021/02/27
#include"JackAnalyzer.h"
#include"FileHandler.h"
#include"JackTokenizer2.h"
#include"CompilationEngine2.h"
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
    CompilationEngine ce(files, jt);

    ce.compile();
  }

  return;
}