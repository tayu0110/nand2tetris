// CompilationEngine2.h
// Author       : tayu
// Latest Edit  : 2021/02/27
#ifndef JACK_COMPILATIONENGINE2_H
#define JACK_COMPILATIONENGINE2_H

#include"FileHandler.h"
#include"JackTokenizer2.h"
#include<string>

class CompilationEngine {
  FileHandler &fh;
  JackTokenizer &jt;
  inline bool isOp(std::string);
  inline bool isUnaryOp(std::string);
public:
  CompilationEngine(FileHandler &fh, JackTokenizer &jt) : fh(fh), jt(jt) {}
  void compile();
  void compileClass(std::string);
  void compileClassVarDec(std::string);
  void compileSubroutine(std::string);
  void compileParameterList();
  void compileSubroutineBody();
  void compileVarDec(std::string);
  void compileStatements();
  void compileDo(std::string);
  void compileLet(std::string);
  void compileWhile(std::string);
  void compileReturn(std::string);
  void compileIf(std::string);
  void compileExpression();
  void compileTerm();
  void compileSubroutineCall();
  void compileExpressionList();
};

#endif