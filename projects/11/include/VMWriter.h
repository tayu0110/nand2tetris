// VMWriter.h
// Author       : tayu
// Latest Edit  : 2021/03/26
#ifndef VMWRITER_H
#define VMWRITER_H

#include"FileHandler.h"
#include<string>
#include<map>

#define SEG_CONST   0b00000001
#define SEG_ARG     0b00000010
#define SEG_LOCAL   0b00000100
#define SEG_STATIC  0b00001000
#define SEG_THIS    0b00010000
#define SEG_THAT    0b00100000
#define SEG_POINTER 0b01000000
#define SEG_TEMP    0b10000000

#define COM_ADD     0b000000000001
#define COM_SUB     0b000000000010
#define COM_NEG     0b000000000100
#define COM_EQ      0b000000001000
#define COM_GT      0b000000010000
#define COM_LT      0b000000100000
#define COM_AND     0b000001000000
#define COM_OR      0b000010000000
#define COM_NOT     0b000100000000

class VMWriter {
  FileHandler &fh;
  std::map<int, std::string> segmentList;
  std::map<int, std::string> commandList;
public:
  VMWriter(FileHandler &fh)
    : fh(fh),
      segmentList({
          {SEG_CONST, "constant"},
          {SEG_ARG, "argument"},
          {SEG_LOCAL, "local"},
          {SEG_STATIC, "static"},
          {SEG_THIS, "this"},
          {SEG_THAT, "that"},
          {SEG_POINTER, "pointer"},
          {SEG_TEMP, "temp"}
        }),
      commandList({
          {COM_ADD, "add"},
          {COM_SUB, "sub"},
          {COM_NEG, "neg"},
          {COM_EQ, "eq"},
          {COM_GT, "gt"},
          {COM_LT, "lt"},
          {COM_AND, "and"},
          {COM_OR, "or"},
          {COM_NOT, "not"}
        }) {}
  void writePush(int segment, int index);
  void writePop(int segment, int index);
  void writeArithmetic(int command);
  void writeLabel(std::string label);
  void writeGoto(std::string label);
  void writeIf(std::string label);
  void writeCall(std::string name, int nArgs);
  void writeFunction(std::string name, int nLocals);
  void writeReturn();
};

#endif