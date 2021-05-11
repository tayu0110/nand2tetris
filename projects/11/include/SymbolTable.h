// SymbolTable.h
// Author       : tayu
// Latest Edit  : 2021/03/26
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include<string>
#include<map>

#define NONE          0b0000
#define KIND_STATIC   0b0001
#define KIND_FIELD    0b0010
#define KIND_ARG      0b0100
#define KIND_VAR      0b1000

class TableElement {
public:
  std::string type;
  int kind;
  int index;
  TableElement() : type(""), kind(0), index(0) {}
  TableElement(std::string type, int kind, int index) : type(type), kind(kind), index(index) {}
  TableElement(const TableElement &te) : type(te.type), kind(te.kind), index(te.index) {}
};

using Table = std::map<std::string, TableElement>;

class SymbolTable {
  Table classScope;
  Table methodScope;
  std::map<int, int> varCountTabel;
  inline bool searchIdentifier(std::string);
public:
  SymbolTable();
  SymbolTable(const SymbolTable &st);
  void startSubroutine();
  void define(std::string name, std::string type, int kind);
  int varCount(int kind);
  int kindOf(std::string name);
  std::string typeOf(std::string name);
  int indexOf(std::string name);
};

#endif