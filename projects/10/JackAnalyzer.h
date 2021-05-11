// JackAnalyzer.h
// Author       : tayu
// Latest Edit  : 2021/02/27
#ifndef JACK_JACKANALYZER_H
#define JACK_JACKANALYZER_H

#include<string>
#include<vector>
#include"FileHandler.h"

class JackAnalyzer {
  std::vector<FileHandler> fileHandles;
public:
  JackAnalyzer() : fileHandles(0) {}
  JackAnalyzer(std::string);
  void Analyze();
};

#endif