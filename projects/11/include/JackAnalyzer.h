// JackAnalyzer.h
// Author       : tayu
// Latest Edit  : 2021/03/24
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