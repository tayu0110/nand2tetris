// CompilationEngine.h
// Author       : tayu
// Latest Edit  : 2021/03/27
#ifndef JACK_COMPILATIONENGINE_H
#define JACK_COMPILATIONENGINE_H

#include"FileHandler.h"
#include"JackTokenizer.h"
#include"VMWriter.h"
#include"SymbolTable.h"
#include<string>

class CompilationEngine {
  FileHandler &fh;
  JackTokenizer &jt;
  VMWriter &vw;
  SymbolTable st;
  std::string className;
  std::string methodName;
  std::string methodType;
  std::string functionTypePrefix;
  inline bool isOp(std::string);
  inline bool isUnaryOp(std::string);
  inline bool isPrimitiveType(std::string);
  inline int convertKindToSegment(int);
public:
  CompilationEngine(FileHandler &fh, JackTokenizer &jt, VMWriter &vw) : fh(fh), jt(jt), vw(vw) {}
  void compile();
  void compileClass(std::string);
  void compileClassVarDec(std::string);
  void compileSubroutine(std::string);
  int compileParameterList();
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
  void compileSubroutineCall(std::string cName="");
  int compileExpressionList();
};

#endif