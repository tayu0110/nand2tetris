// JackTokenizer2.h
// Author       : tayu
// Latest Edit  : 2021/02/27
#ifndef JACK_JACKTOKENIZER_H
#define JACK_JACKTOKENIZER_H

#include<vector>
#include<string>
#include<set>
#include<utility>

#define KEYWORD       1
#define SYMBOL        2
#define IDENTIFIER    4
#define INT_CONST     8
#define STRING_CONST  16
#define COMMENT       32

class Token {
public:
  std::string token;
  int type;
  Token() : token(""), type(0) {}
  Token(std::string tk, int tp) : token(tk), type(tp) {}
  Token(const Token &tk) {
    this->token = tk.token;
    this->type = tk.type;
  }
};

class JackTokenizer {
  std::set<std::string> keywords;
  std::set<char> symbols;
  std::string jackcode;
  std::vector<Token> tokens;
  int pos;
  int len;
  void make_token_pair(std::string&);
  inline bool isKeyword(std::string);
  inline bool isSymbol(char);
  int getTokenType(std::string);
  std::string stringVal(std::string);
public:
  JackTokenizer() : jackcode(""), tokens(0), pos(0), len(0) {}
  JackTokenizer(std::string&);
  JackTokenizer(const JackTokenizer&);
  bool hasMoreTokens();
  void advance(int add=1);
  Token accessToken(int p=-1);
  int getPos();
};

#endif