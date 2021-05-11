// FileHandler.cpp
// Author       : tayu
// Latest Edit  : 2021/03/24
#include"FileHandler.h"
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<cstdlib>
#include<dirent.h>

FileHandler::FileHandler(std::string file) {
  this->fullPath = file;

  std::string fileBaseName;
  std::string fileName;
  std::string dirName;
  auto pos = file.find_last_of('/');

  if(pos == std::string::npos) {
    fileName = file;
    dirName = "";
  } else {
    fileName = file.substr(pos+1);
    dirName = file.substr(0, pos);
  }

  pos = fileName.find_last_of('.');
  fileBaseName = fileName.substr(0, pos);

  this->fileBaseName = fileBaseName;
  this->fileName = fileName;
  this->dirName = dirName;
}

FileHandler::FileHandler(const FileHandler& fh) {
  this->fullPath = fh.fullPath;
  this->fileBaseName = fh.fileBaseName;
  this->fileName = fh.fileName;
  this->dirName = fh.dirName;
}

bool FileHandler::isEmptyOrOneLineComment(std::string line) {
  int len = line.length();

  for(int i = 0; i < len; i++) {
    if(isspace(line[i])) continue;
    if(i+1 < len && line[i] == '/' && line[i+1] == '/') return true;

    return false;
  }

  return true;
}

void FileHandler::removeOneLineComment(std::string &line) {
  int pos = 0;
  int len = line.length();

  for(pos = 0; pos < len; pos++) {
    if(pos+1 < len && line[pos] == '/' && line[pos+1] == '/') break;
  }

  if(pos != len-1) line = line.substr(0, pos);

  return;
}

std::string FileHandler::getFullPath() {
  return this->fullPath;
}

std::string FileHandler::getFileBaseName() {
  return this->fileBaseName;
}

std::string FileHandler::getFileName() {
  return this->fileName;
}

std::string FileHandler::getDirName() {
  return this->dirName;
}

void FileHandler::getJackCode(std::string &jackCode) {
  this->ifs.open(this->fullPath);

  if(!this->ifs.is_open()) {
    std::cout << "Error: Failed to open file." << std::endl;
    std::cout << "File: " << this->fullPath << std::endl;
    exit(1);
  }

  std::string nowLine;
  while(std::getline(this->ifs, nowLine)) {
    if(this->isEmptyOrOneLineComment(nowLine)) continue;

    this->removeOneLineComment(nowLine);
    jackCode += nowLine;
    nowLine = "";
  }

  return;
}

void FileHandler::writeToXML(std::string code) {
  if(!this->ofs.is_open()) {
    try {
      this->ofs.open(this->fileBaseName + ".xml");
    } catch(std::exception &e) {
      std::cerr << "Error: Failed to open .xml file." << std::endl;
      std::cerr << e.what() << std::endl;
      exit(1);
    }
  }

  this->ofs << code << std::endl;

  return;
}

void FileHandler::writeToVM(std::string code) {
  if(!this->ofs.is_open()) {
    try {
      this->ofs.open(this->dirName + "/" + this->fileBaseName + ".vm");
    } catch(std::exception &e) {
      std::cerr << "Error: Failed to open .vm file." << std::endl;
      std::cerr << e.what() << std::endl;
      exit(1);
    }
  }

  this->ofs << code << std::endl;

  return;
}

void FileHandler::writeToVM(std::vector<std::string> &codes) {
  if(!this->ofs.is_open()) {
    try {
      this->ofs.open(this->dirName + "/" + this->fileBaseName + ".vm");
    } catch(std::exception &e) {
      std::cerr << "Error: Failed to open .vm file." << std::endl;
      std::cerr << e.what() << std::endl;
      exit(1);
    }
  }

  for(auto code : codes) this->ofs << code << std::endl;

  return;
}

FileHandler::~FileHandler() {
  if(this->ifs.is_open()) this->ifs.close();
  if(this->ofs.is_open()) this->ofs.close();
}

bool isJackFile(std::string argument) {
  auto pos = argument.find_last_of('.');

  if(pos == std::string::npos) return false;

  std::string extension = argument.substr(pos);

  if(extension == ".jack") return true;
  else return false;
}

std::vector<FileHandler> getFileHandle(std::string argument) {
  std::vector<std::string> files(0);

  if(isJackFile(argument)) {
    files.push_back(argument);
  } else {
    int len = argument.length();
    if(argument[len-1] != '/') argument += '/';

    DIR* dir;
    struct dirent *direntry;

    dir = opendir(argument.c_str());
    direntry = readdir(dir);
    while(direntry != NULL) {
      std::string fn = direntry->d_name;

      if(isJackFile(fn)) files.push_back(argument + fn);

      direntry = readdir(dir);
    }

    closedir(dir);
  }

  if(files.size() == 0) {
    std::cout << "Error: Not found target files. Please make sure that the argumenst is correct." << std::endl;
    exit(1);
  }

  std::vector<FileHandler> res(0);
  for(auto file : files) {
    FileHandler fh(file);
    res.emplace_back(fh);
  }

  return res;
}