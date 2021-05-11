// FileHandler.h
// Author       : tayu
// Latest Edit  : 2021/02/27
#ifndef JACK_FILEHANDLER_H
#define JACK_FILEHANDLER_H

#include<string>
#include<vector>
#include<fstream>

class FileHandler {
  std::ifstream ifs;
  std::ofstream ofs;
  std::string fullPath;
  std::string fileBaseName;
  std::string fileName;
  std::string dirName;
  bool isEmptyOrOneLineComment(std::string);
  void removeOneLineComment(std::string&);
public:
  FileHandler() : fullPath(""), fileBaseName(""), fileName(""), dirName("") {}
  FileHandler(std::string);
  FileHandler(const FileHandler&);
  std::string getFullPath();
  std::string getFileBaseName();
  std::string getFileName();
  std::string getDirName();
  void getJackCode(std::string&);
  void writeToXML(std::string);
  ~FileHandler();
};

bool isJackFile(std::string);
std::vector<FileHandler> getFileHandle(std::string);

#endif