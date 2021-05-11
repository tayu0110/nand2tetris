// SymbolTable.cpp
// Author       : tayu
// Latest Edit  : 2021/03/28
#include"SymbolTable.h"
#include"ErrorHandler.h"
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<cstdlib>

SymbolTable::SymbolTable() {
  classScope.clear();
  methodScope.clear();
  varCountTabel[KIND_STATIC] = 0;
  varCountTabel[KIND_FIELD] = 0;
  varCountTabel[KIND_ARG] = 0;
  varCountTabel[KIND_VAR] = 0;
}

SymbolTable::SymbolTable(const SymbolTable &st) {
  classScope = st.classScope;
  methodScope = st.methodScope;
  varCountTabel = st.varCountTabel;
}

void SymbolTable::startSubroutine() {
  methodScope.clear();
  varCountTabel[KIND_ARG] = 0;
  varCountTabel[KIND_VAR] = 0;

  return;
}

void SymbolTable::define(std::string name, std::string type, int kind) {
  if(searchIdentifier(name)) {
    throw ErrorHandler(SYNTAX_ERROR, {
      {"Error: This identifier is already registered."},
      {"Name: " + name},
      {"Type: " + type},
      {"Kind: " + kind}
    });
    // throw std::vector<std::string>({
    //   {"Error: This identifier is already registered."},
    //   {"Name: " + name},
    //   {"Type: " + type},
    //   {"Kind: " + kind}
    // });
  }

  int index = varCount(kind);

  if(kind & (KIND_STATIC | KIND_FIELD)) {
    classScope[name] = TableElement(type, kind, index);

  } else if(kind & (KIND_ARG | KIND_VAR)) {
    methodScope[name] = TableElement(type, kind, index);

  } else {
    throw ErrorHandler(INTERNAL_ERROR, {
      {"Internal Error: Invalid to kind."},
      {"Name: " + name},
      {"Type: " + type},
      {"Kind: " + kind}
    });
    // throw std::vector<std::string>({
    //   {"Internal Error: Invalid to kind."},
    //   {"Name: " + name},
    //   {"Type: " + type},
    //   {"Kind: " + kind}
    // });
  }

  varCountTabel[kind]++;

  return;
}

int SymbolTable::varCount(int kind) {
  return varCountTabel[kind];
}

int SymbolTable::kindOf(std::string name) {
  if(classScope.find(name) != classScope.end())
    return classScope[name].kind;

  if(methodScope.find(name) != methodScope.end())
    return methodScope[name].kind;

  return NONE;
}

std::string SymbolTable::typeOf(std::string name) {
  if(classScope.find(name) != classScope.end())
    return classScope[name].type;

  if(methodScope.find(name) != methodScope.end())
    return methodScope[name].type;

  return "";
}

int SymbolTable::indexOf(std::string name) {
  if(classScope.find(name) != classScope.end())
    return classScope[name].index;
  
  if(methodScope.find(name) != methodScope.end())
    return methodScope[name].index;
  
  return -1;
}

inline bool SymbolTable::searchIdentifier(std::string token) {
  return (classScope.find(token) != classScope.end() ||
          methodScope.find(token) != methodScope.end());
}