// VMWriter.cpp
// Author       : tayu
// Latest Edit  : 2021/03/26
#include"VMWriter.h"
#include"FileHandler.h"
#include<string>
#include<map>

void VMWriter::writePush(int segment, int index) {
  std::string code = "push " + segmentList[segment] + " " + std::to_string(index);

  fh.writeToVM(code);
  return;
}

void VMWriter::writePop(int segment, int index) {
  std::string code = "pop " + segmentList[segment] + " " + std::to_string(index);

  fh.writeToVM(code);
  return;
}

void VMWriter::writeArithmetic(int command) {
  fh.writeToVM(commandList[command]);
  return;
}

void VMWriter::writeLabel(std::string label) {
  std::string code = "label " + label;

  fh.writeToVM(code);
  return;
}

void VMWriter::writeGoto(std::string label) {
  std::string code = "goto " + label;

  fh.writeToVM(code);
  return;
}

void VMWriter::writeIf(std::string label) {
  std::string code = "if-goto " + label;

  fh.writeToVM(code);
  return;
}

void VMWriter::writeCall(std::string name, int nArgs) {
  std::string code = "call " + name + " " + std::to_string(nArgs);

  fh.writeToVM(code);
  return;
}

void VMWriter::writeFunction(std::string name, int nLocals) {
  std::string code = "function " + name + " " + std::to_string(nLocals);

  fh.writeToVM(code);
  return;
}

void VMWriter::writeReturn() {
  fh.writeToVM("return");
  return;
}