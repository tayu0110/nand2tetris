// JackTokenizer.cpp
// Author       : tayu
// Latest Edit  : 2021/03/08
#include"JackTokenizer.h"
#include<iostream>
#include<string>
#include<regex>
#include<cstdlib>
#include<cctype>

std::string keywordsList[] = {
  "class",      "constructor",      "function",     "method",
  "field",      "static",           "var",          "int",
  "char",       "boolean",          "void",         "true",
  "false",      "null",             "this",         "let",
  "do",         "if",               "else",         "while",
  "return"
};
char symbolsList[] = {
  '{',    '}',    '(',    ')',    '[',    ']',
  '.',    ',',    ';',    '+',    '-',    '*',
  '/',    '&',    '|',    '<',    '>',    '=',
  '~'
};

JackTokenizer::JackTokenizer(std::string &code) {
  bool isComment = false;
  bool isStrConst = false;
  jackcode = code;
  pos = 0;

  for(auto keyword : keywordsList) keywords.insert(keyword);
  for(auto symbol : symbolsList) symbols.insert(symbol);

  int now = 0;
  int codeLength = jackcode.length();
  std::string token;

  while(now < codeLength) {
    if(isComment) {
      if(now-1 >= 0 && code[now] == '/' && code[now-1] == '*') isComment = false;

      now++;
      continue;
    }

    if(now+1 < codeLength && code[now] == '/' && code[now+1] == '*') {
      isComment = true;
      now++;
      continue;
    }

    if(isStrConst) {
      if(code[now] == '\"') {
        token += code[now];
        isStrConst = false;

        make_token_pair(token);
        now++;

      } else {
        token += code[now];
        now++;
        continue;
      }
    }

    if(code[now] == '\"') {
      isStrConst = true;
      
      if(token == "") {
        token += code[now];
        now++;
        
      } else {
        make_token_pair(token);
        token = code[now];
        now++;
      }
      continue;
    }

    if(isspace(code[now])) {
      if(token == "") {
        now++;
        continue;
      }

      make_token_pair(token);
      now++;
    }

    if(isSymbol(code[now])) {
      if(token == "") {
        std::string symbol = {code[now]};

        make_token_pair(symbol);
        now++;
        continue;

      } else {
        make_token_pair(token);
        continue;
      }
    }

    token += code[now];
    now++;

  }

  len = tokens.size();
}

JackTokenizer::JackTokenizer(const JackTokenizer& jt) {
  keywords = jt.keywords;
  symbols = jt.symbols;
  jackcode = jt.jackcode;
  tokens = jt.tokens;
  pos = jt.pos;
  len = jt.len;
}

bool JackTokenizer::hasMoreTokens() {
  return (pos+1 < len);
}

void JackTokenizer::advance(int add) {
  pos += add;
  return;
}

Token JackTokenizer::accessToken(int p) {
  // int pos = (p == -1 ? this->pos : p);
  int accessPos = (p == -1 ? pos : pos + p);
  Token tk = tokens[accessPos];

  return tk;
}

int JackTokenizer::getPos() {
  return pos;
}

int JackTokenizer::getTokenType(std::string token) {
  bool isInteger;
  bool isString;
  bool isIdentifier;
  try {
    isInteger = std::regex_match(token, std::regex("^[0-9]+$"));
    isString = std::regex_match(token, std::regex("^\".*\"$"));
    isIdentifier = std::regex_match(token, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
  } catch(std::regex_error &e) {
    std::cerr << e.code() << std::endl;
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  if(isKeyword(token)) return KEYWORD;
  else if(token.length() == 1 && isSymbol(token[0])) return SYMBOL;
  else if(isInteger) return INT_CONST;
  else if(isString) return STRING_CONST;
  else if(isIdentifier) return IDENTIFIER;
  else {
    std::cout << "Internal Error: Token do not match any lexical elements." << std::endl;
    std::cout << "Token: " << token << std::endl;
    exit(2);
  }
}

std::string JackTokenizer::stringVal(std::string token) {
  int tokenLen = token.length();

  if(tokenLen == 2) token = "";
  else token = token.substr(1, tokenLen-2);

  return token;
}

void JackTokenizer::make_token_pair(std::string &token) {
  int tokenType = getTokenType(token);

  if(tokenType & (KEYWORD | SYMBOL | IDENTIFIER | INT_CONST)) tokens.emplace_back(Token(token, tokenType));
  else if(tokenType & STRING_CONST) tokens.emplace_back(Token(stringVal(token), tokenType));
  else {
    std::cout << "Internal Error: Invalid to token type." << std::endl;
    std::cout << "Token: " << token << std::endl;
    std::cout << "TokenType: " << tokenType << std::endl;
    exit(2);
  }

  token = "";

  return;
}

inline bool JackTokenizer::isKeyword(std::string token) {
  return (keywords.find(token) != keywords.end());
}

inline bool JackTokenizer::isSymbol(char token) {
  return (symbols.find(token) != symbols.end());
}