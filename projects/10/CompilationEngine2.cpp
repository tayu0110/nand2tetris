// CompilationEngine.cpp
// Author       : tayu
// Latest Edit  : 2021/03/09
#include"CompilationEngine.h"
#include"FileHandler.h"
#include"JackTokenizer.h"
#include<iostream>
#include<string>
#include<cstdlib>

void CompilationEngine::compile() {
  while(jt.hasMoreTokens()) {
    Token tk = jt.accessToken();

    std::string token = tk.token;

    if(token == "class") compileClass(token);
  }
}

void CompilationEngine::compileClass(std::string argToken) {
  if(argToken != "class") {
    std::cout << "Internal Error(CompilationEngine2.cpp:23): This token is not class keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<class>");
  fh.writeToXML("<keyword> " + argToken + " </keyword>");

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string className = tk.token;
    int tokenType = tk.type;

    if(tokenType & IDENTIFIER) {
      fh.writeToXML("<identifier> " + className + " </identifier>");
    } else {
      std::cout << "Error(CompilationEngine2.cpp:43): ClassName must be an identifier." << std::endl;
      std::cout << "Token: " << className << std::endl;
      exit(1);
    }
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string openTag = tk.token;

    if(openTag != "{") {
      std::cout << "Error(CompilationEngine2.cpp:60): \'{\' is missing." << std::endl;
      std::cout << "Token: " << openTag << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> " + openTag + " </symbol>");
  }

  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string token = tk.token;
    int tokenType = tk.type;

    if(token == "}") {
      fh.writeToXML("<symbol> " + token + "</symbol>");
      break;
    }

    if(tokenType ^ KEYWORD) {
      std::cout << "Error(CompilationEngine2.cpp:88): The top of elements of class body requires a keyword that specifies the kind of the statement." << std::endl;
      exit(1);
    }

    if(token == "static" || token == "field") {
      compileClassVarDec(token);
    } else if(token == "constructor" || token == "function" || token == "method") {
      compileSubroutine(token);
    } else {
      std::cout << "Error(CompilationEngine2.cpp:80): The element of class body must be variable declarations or subroutine declarations." << std::endl;
      std::cout << "Token: " << token << std::endl;
      exit(1);
    }
  }
  
  fh.writeToXML("</class>");

  return;
}

void CompilationEngine::compileClassVarDec(std::string argToken) {
  if(argToken != "static" && argToken != "field") {
    std::cout << "Internal Error: This token is not class variable declaration keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<classVarDec>");
  fh.writeToXML("<keyword> " + argToken + " </keyword>");

  int tokenType;
  std::string type;
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    type = tk.token;
    tokenType = tk.type;

    if(tokenType & KEYWORD) {
      if(type == "int" || type == "char" || type == "boolean") {
        fh.writeToXML("<keyword> " + type + " </keyword>");

      } else {
        std::cout << "Error(CompilationEngine2.cpp:123): A variable declaration requires a type." << std::endl;
        std::cout << "Token: " << type << std::endl;
        exit(1);
      }

    } else if(tokenType & IDENTIFIER) {
      fh.writeToXML("<identifier> " + type + " </identifier>");

    } else {
      std::cout << "Error(CompilationEngine2.cpp:132): A variable declaration requires a type." << std::endl;
      std::cout << "Token: " << type << std::endl;
      exit(1);
    }
  }

  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string varName = tk.token;
    tokenType = tk.type;

    if(tokenType & IDENTIFIER) {
      fh.writeToXML("<identifier> " + varName + " </identifier>");

      Token nextToken = jt.accessToken(1);

      if(nextToken.type ^ SYMBOL) {
        std::cout << "Error(CompilationEngine2.cpp:156): \',\' is missing." << std::endl;
        std::cout << "Token: " << nextToken.token << std::endl;
        exit(1);
      }

      if(nextToken.token == ",") {
        fh.writeToXML("<symbol> , </symbol>");
        jt.advance();
        continue;

      } else if(nextToken.token == ";") {
        fh.writeToXML("<symbol> ; </symbol>");
        jt.advance();
        break;

      } else {
        std::cout << "Error(CompilationEngine2.cpp:170): Invalid to symbol." << std::endl;
        std::cout << "Token: " << nextToken.token << std::endl;
        exit(1);
      }

    } else {
      std::cout << "Error: A variable name must be an identifier." << std::endl;
      std::cout << "Token: " << varName << std::endl;
      exit(1);
    }
  }

  fh.writeToXML("</classVarDec>");

  return;
}

void CompilationEngine::compileSubroutine(std::string argToken) {
  if(argToken != "constructor" && argToken != "function" && argToken != "method") {
    std::cout << "Internal Error(CompilationEngine2.cpp:191): This token is subroutine declaration keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<subroutineDec>");
  fh.writeToXML("<keyword> " + argToken + " </keyword>");

  int tokenType;
  std::string type;
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    type = tk.token;
    tokenType = tk.type;

    if(tokenType & KEYWORD) {
      if(type == "int" || type == "char" || type == "boolean" || type == "void") {
        fh.writeToXML("<keyword> " + type + " </keyword>");

      } else {
        std::cout << "Error(CompilationEngine2.cpp:213): A declaration of subroutine requires a type of return value." << std::endl;
        std::cout << "Token: " << type << std::endl;
        exit(1);
      }
    } else if(tokenType & IDENTIFIER) {
      fh.writeToXML("<identifier> " + type + " </identifier>");

    } else {
      std::cout << "Error(CompilationEngine2.cpp:221): A declaration of subroutine requires a type of return value." << std::endl;
      std::cout << "Token: " << type << std::endl;
      exit(1);
    }
  }

  std::string subroutineName;
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    subroutineName = tk.token;
    tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      std::cout << "Error(CompilationEngine2.cpp:239): A subroutine name must be a word other than keyword, symbol and integer constant." << std::endl;
      std::cout << "Token: " << subroutineName << std::endl;
      exit(1);
    }

    fh.writeToXML("<identifier> " + subroutineName + " </identifier>");
  }
  
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string startSymbol = tk.token;

    if(startSymbol != "(") {
      std::cout << "Error(CompilationEngine2.cpp:268): \'(\' is missing." << std::endl;
      std::cout << "Symbol: " << startSymbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> " + startSymbol + " </symbol>");

    Token checkNext = jt.accessToken(1);

    if(checkNext.token == ")") {
      fh.writeToXML("<parameterList>");
      fh.writeToXML("</parameterList>");
      fh.writeToXML("<symbol> ) </symbol>");
      jt.advance();
    } else {
      compileParameterList();

      jt.advance();
      Token closeTag = jt.accessToken();

      if(closeTag.token != ")") {
        std::cout << "Error: \')\' is missing." << std::endl;
        std::cout << "Token: " << closeTag.token << std::endl;
        exit(1);
      }

      fh.writeToXML("<symbol> ) </symbol>");
    }
  }


  compileSubroutineBody();

  fh.writeToXML("</subroutineDec>");
}

void CompilationEngine::compileParameterList() {
  fh.writeToXML("<parameterList>");

  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string token = tk.token;
    int tokenType = tk.type;

    if((tokenType & (KEYWORD | IDENTIFIER))) {        
      if((tokenType & KEYWORD) && (token != "int" && token != "char" && token != "boolean")) {
        std::cout << "Error(CompilationEngine2.cpp:285): Type name must be int, char, boolean and class name." << std::endl;
        std::cout << "Token: " << token << std::endl;
        exit(1);
      }

      if(tokenType & KEYWORD) fh.writeToXML("<keyword> " + token + " </keyword>");
      else fh.writeToXML("<identifier> " + token + " </identifier>");

      Token varName = jt.accessToken(1);

      if(varName.type ^ IDENTIFIER) {
        std::cout << "Error(CompilationEngine2.cpp:297): A variable name must be an identifier." << std::endl;
        std::cout << "Token: " << varName.token << std::endl;
        exit(1);
      }

      fh.writeToXML("<identifier> " + varName.token + " </identifier>");

      Token symbol = jt.accessToken(2);

      if(symbol.type ^ SYMBOL) {
        std::cout << "Error(CompilationEngine2.cpp:308): Paramater list requires to mark with \',\' or terminate with \')\'." << std::endl;
        std::cout << "Token: " << symbol.token << std::endl;
        exit(1);
      }

      if(symbol.token == ")") {
        jt.advance();
        break;
      }

      if(symbol.token == ",") {
        jt.advance(2);
        fh.writeToXML("<symbol> " + symbol.token + " </symbol>");

      } else {
        std::cout << "Error(CompilationEngine2.cpp:318): Invalid to symbol." << std::endl;
        std::cout << "Token: " << symbol.token << std::endl;
        exit(1);
      }

    } else {
      std::cout << "Error(CompilationEngine2.cpp:329): Parameters require type." << std::endl;
      std::cout << "Token: " << token << std::endl;
      exit(1);
    }
  }

  fh.writeToXML("</parameterList>");

  return;
}

void CompilationEngine::compileSubroutineBody() {
  fh.writeToXML("<subroutineBody>");
  
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "{") {
      std::cout << "Error(CompilationEngine2.cpp:354): \'{\' is missing." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> " + symbol + " </symbol>");
  }

  while(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string token = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ KEYWORD) {
      std::cout << "Error(CompilationEngine2.cpp:382): A variable declaration and statements requires a keyword that specifies the kind of declaration or statement." << std::endl;
      std::cout << "Token: " << token << std::endl;
      exit(1);
    }

    if(token == "var") {
      jt.advance();
      compileVarDec(token);

    } else if(token == "let" || token == "if" || token == "while" || token == "do" || token == "return") {
      compileStatements();
      break;

    } else {
      std::cout << "Error(CompilationEngine2.cpp:376): A variable declaration and statements requires a keyword that specifies the kind of declaration or statement." << std::endl;
      std::cout << "Token: " << token << std::endl;
      exit(1);
    }
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "}") {
      std::cout << "Error: \'}\' is missing." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> } </symbol>");
  }


  fh.writeToXML("</subroutineBody>");

  return;
}

void CompilationEngine::compileVarDec(std::string argToken) {
  if(argToken != "var") {
    std::cout << "Internal Error(CompilationEngine2.cpp:397): This token is not \"var\" keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<varDec>");
  fh.writeToXML("<keyword> " + argToken + "</keyword>");

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string type = tk.token;
    int tokenType = tk.type;

    if(tokenType & KEYWORD) {
      if(type != "int" && type != "char" && type != "boolean") {
        std::cout << "Error(CompilationEngine2.cpp:414): A variable declaration requires a type." << std::endl;
        std::cout << "Token: " << type << std::endl;
        exit(1);
      }

      fh.writeToXML("<keyword> " + type + " </keyword>");

    } else if(tokenType & IDENTIFIER) {
      fh.writeToXML("<identifier> " + type + " </identifier>");

    } else {
      std::cout << "Error(CompilationEngine2.cpp:422): A variable declaration requires a type." << std::endl;
      std::cout << "Token: " << type << std::endl;
      exit(1);
    }
  }

  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string varName = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      std::cout << "Error: A variable name must be an identifier." << std::endl;
      std::cout << "Token: " << varName << std::endl;
      exit(1);
    }

    fh.writeToXML("<identifier> " + varName + " </identifier>");

    Token symbol = jt.accessToken(1);

    if(symbol.type ^ SYMBOL) {
      std::cout << "Error(CompilationEngine2.cpp:442): \',\' is required to list the variable names." << std::endl;
      std::cout << "Token: " << symbol.token << std::endl;
      exit(1);
    }

    if(symbol.token == ",") {
      fh.writeToXML("<symbol> , </symbol>");
      jt.advance();

    } else if(symbol.token == ";") {
      fh.writeToXML("<symbol> ; </symbol>");
      jt.advance();
      break;

    } else {
      std::cout << "Error(CompilationEngine2.cpp:457): Variable declaration needs to separate variable names with \',\' or to end the declaration with \';\'" << std::endl;
      std::cout << "Token: " << symbol.token << std::endl;
      exit(1);
    }
  }

  fh.writeToXML("</varDec>");
}

void CompilationEngine::compileStatements() {
  fh.writeToXML("<statements>");

  while(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string token = tk.token;
    int tokenType = tk.type;

    if(tokenType & KEYWORD) {
      std::string keyword = token;

      if(keyword == "let") {
        jt.advance();
        compileLet(keyword);

      } else if(keyword == "if") {
        jt.advance();
        compileIf(keyword);

      } else if(keyword == "while") {
        jt.advance();
        compileWhile(keyword);

      } else if(keyword == "do") {
        jt.advance();
        compileDo(keyword);

      } else if(keyword == "return") {
        jt.advance();
        compileReturn(keyword);

      } else if(keyword == "var") {
        std::cout << "Error(CompilationEngine2.cpp:518): Variable declarations in the class put the front of subroutine declarations." << std::endl;
        std::cout << "Token: " << keyword << std::endl;
        exit(1);

      } else {
        std::cout << "Error(CompilationEngine2.cpp:523): This is not keyword that specifies the kind of statement." << std::endl;
        std::cout << "Token: " << keyword << std::endl;
        exit(1);
      }

    } else if(tokenType & SYMBOL){
      std::string symbol = token;

      if(symbol != "}") {
        std::cout << "Error(CompilationEngine2.cpp:538): Symbols other than \'}\' can not put inside of subroutine body." << std::endl;
        std::cout << "Token: " << symbol << std::endl;
        exit(1);
      }

      break;

    } else {
      std::cout << "Error(CompilationEngine2.cpp:542): Other than keyword can not put the top of statements inside of subroutine body." << std::endl;
      std::cout << "Token: " << token << std::endl;
      exit(1);
    }
  }

  fh.writeToXML("</statements>");

  return;
}

void CompilationEngine::compileDo(std::string argToken) {
  if(argToken != "do") {
    std::cout << "Internal Error(CompilationEngine2.cpp:555): This token is not \"do\" keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<doStatement>");
  fh.writeToXML("<keyword> " + argToken + " </keyword>");

  compileSubroutineCall();

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != ";") {
      std::cout << "Error(CompilationEngine2.cpp:576): \';\' is missing." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> ; </symbol>");
  }

  fh.writeToXML("</doStatement>");

  return;
}

void CompilationEngine::compileLet(std::string argToken) {
  if(argToken != "let") {
    std::cout << "Internal Error: This token is not \"let\" keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<letStatement>");
  fh.writeToXML("<keyword> " + argToken + " </keyword>");

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string varName = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      std::cout << "Error: Variable name is missing." << std::endl;
      std::cout << "TokenType: " << tokenType << std::endl;
      exit(1);
    }

    fh.writeToXML("<identifier> " + varName + " </identifier>");
  }

  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ SYMBOL) {
      std::cout << "Error: Invalid to token." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    if(symbol == "[") {
      fh.writeToXML("<symbol> " + symbol + " </symbol>");
      compileExpression();

      Token closeTag = jt.accessToken(1);

      if(closeTag.token != "]") {
        std::cout << "Error: \']\' is missing." << std::endl;
        std::cout << "Token: " << closeTag.token << std::endl;
        exit(1);
      }

      fh.writeToXML("<symbol> ] </symbol>");
      jt.advance();
      continue;

    } else if(symbol == "=") {
      fh.writeToXML("<symbol> " + symbol + " </symbol>");
      break;

    } else {
      std::cout << "Error: Invalid to symbol." << std::endl;
      std::cout << "Symbol: " << symbol << std::endl;
      exit(1);
    }
  }

  compileExpression();

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != ";") {
      std::cout << "Error: \';\' is missing." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> ; </symbol>");
  }

  fh.writeToXML("</letStatement>");

  return;
}

void CompilationEngine::compileWhile(std::string argToken) {
  if(argToken != "while") {
    std::cout << "Internal Error: This token is \'while\' keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<whileStatement>");
  fh.writeToXML("<keyword> " + argToken + " </keyword>");

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "(") {
      std::cout << "Error: \'(\' is missing." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> ( </symbol>");

    compileExpression();

    Token closeTag = jt.accessToken(1);

    if(closeTag.token != ")") {
      std::cout << "Error: \')\' is missing." << std::endl;
      std::cout << "Token: " << closeTag.token << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> ) </symbol>");
    jt.advance();
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "{") {
      std::cout << "Error: \'{\' is missing." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> " + symbol + " </symbol>");

    compileStatements();

    jt.advance();
    Token closeTag = jt.accessToken();

    if(closeTag.token != "}") {
      std::cout << "Error: \'}\' is missing." << std::endl;
      std::cout << "Token: " << closeTag.token << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> " + closeTag.token + " </symbol>");
  }

  fh.writeToXML("</whileStatement>");

  return;
}

void CompilationEngine::compileReturn(std::string argToken) {
  if(argToken != "return") {
    std::cout << "Internal Error: This token is not \"return\" keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<returnStatement>");
  fh.writeToXML("<keyword> " + argToken + " </keyword>");

  if(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string token = tk.token;

    if(token == ";") {
      fh.writeToXML("<symbol> " + token + " </symbol>");
      fh.writeToXML("</returnStatement>");
      jt.advance();
      return;

    }
    
    compileExpression();
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != ";") {
      std::cout << "Error: \';\' is missing." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> " + symbol + " </symbol>");
  }

  fh.writeToXML("</returnStatement>");

  return;
}

void CompilationEngine::compileIf(std::string argToken) {
  if(argToken != "if") {
    std::cout << "Internal Error: This token is neither \"if\" nor \"else\" keyword." << std::endl;
    std::cout << "Token: " << argToken << std::endl;
    exit(2);
  }

  fh.writeToXML("<ifStatement>");
  fh.writeToXML("<keyword> " + argToken + " </keyword>");

  if(argToken == "if") {
    if(jt.hasMoreTokens()) {
      jt.advance();
      Token tk = jt.accessToken();

      std::string symbol = tk.token;

      if(symbol != "(") {
        std::cout << "Error: If statement requires a conditional branching expression." << std::endl;
        std::cout << "Token: " << symbol << std::endl;
        exit(1);
      }

      fh.writeToXML("<symbol> ( </symbol>");

      Token nextToken = jt.accessToken(1);
        
      if(nextToken.token == ")") {
        fh.writeToXML("<symbol> ) </symbol>");
        jt.advance();

      } else {
        compileExpression();
        jt.advance();

        Token closeTag = jt.accessToken();

        if(closeTag.token != ")") {
          std::cout << "Error: \')\' is missing." << std::endl;
          std::cout << "Token: " << closeTag.token << std::endl;
          exit(1);
        }

        fh.writeToXML("<symbol> ) </symbol>");
      }

      jt.advance();
      tk = jt.accessToken();

      symbol = tk.token;

      if(symbol != "{") {
        std::cout << "Error: \'{\' is missing." << std::endl;
        std::cout << "Token: " << symbol << std::endl;
        exit(1);
      }

      fh.writeToXML("<symbol> { </symbol>");

      compileStatements();

      jt.advance();
      tk = jt.accessToken();

      symbol = tk.token;

      if(symbol != "}") {
        std::cout << "Error: \'}\' is missing." << std::endl;
        std::cout << "Token: " << symbol << std::endl;
        exit(1);
      }

      fh.writeToXML("<symbol> } </symbol>");
    }
  }

  if(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string keyword = tk.token;

    if(keyword != "else") {
      fh.writeToXML("</ifStatement>");
      return;
    }

    fh.writeToXML("<keyword> else </keyword>");
    jt.advance(2);
    tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "{") {
      std::cout << "\'{\' is missing." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> { </symbol>");

    compileStatements();

    jt.advance();
    tk = jt.accessToken();

    symbol = tk.token;

    if(symbol != "}") {
      std::cout << "\'}\' is missing." << std::endl;
      std::cout << "Token:" << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> } </symbol>");
  }

  fh.writeToXML("</ifStatement>");

  return;
}

void CompilationEngine::compileExpression() {
  fh.writeToXML("<expression>");

  while(jt.hasMoreTokens()) {
    compileTerm();

    Token tk = jt.accessToken(1);

    std::string token = tk.token;

    if(!isOp(token)) break;

    fh.writeToXML("<symbol> " + token + " </symbol>");
    jt.advance();
  }

  fh.writeToXML("</expression>");
  
  return;
}

void CompilationEngine::compileTerm() {
  fh.writeToXML("<term>");

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string token = tk.token;
    int tokenType = tk.type;

    if(tokenType & INT_CONST) {
      fh.writeToXML("<integerConstant> " + token + " </integerConstant>");

    } else if(tokenType & STRING_CONST) {
      fh.writeToXML("<stringConstant> " + token + " </stringConstant>");

    } else if(tokenType & KEYWORD) {
      fh.writeToXML("<keyword> " + token + " </keyword>");

    } else if(tokenType & SYMBOL) {
      if(token == "(") {
        fh.writeToXML("<symbol> ( </symbol>");

        compileExpression();

        jt.advance();
        Token closeTag = jt.accessToken();

        if(closeTag.token != ")") {
          std::cout << "Error: \')\' is missing." << std::endl;
          std::cout << "Token: " << closeTag.token << std::endl;
          exit(1);
        }

        fh.writeToXML("<symbol> ) </symbol>");

      } else if(isUnaryOp(token)) {
        fh.writeToXML("<symbol> " + token + " </symbol>");

        compileTerm();

      } else {
        std::cout << "Error: Invalid to symbol." << std::endl;
        std::cout << "Token: " << token << std::endl;
        exit(1);
      }
    } else if(tokenType & IDENTIFIER) {
      fh.writeToXML("<identifier> " + token + " </identifier>");

      Token nextToken = jt.accessToken(1);

      if(nextToken.token == "(") {
        jt.advance();
        Token checkNext = jt.accessToken(1);

        if(checkNext.token != ")") {
          compileExpressionList();
        }

        jt.advance();
        fh.writeToXML("<symbol> ) </symbol>");

      } else if(nextToken.token == ".") {
        jt.advance();
        fh.writeToXML("<symbol> . </symbol>");

        compileSubroutineCall();

      } else if(nextToken.token == "[") {
        jt.advance();
        fh.writeToXML("<symbol> [ </symbol>");

        compileExpression();

        jt.advance();
        Token symbol = jt.accessToken();

        if(symbol.token != "]") {
          std::cout << "Error: Invalid to symbol." << std::endl;
          std::cout << "Token: " << symbol.token << std::endl;
          exit(1);
        }

        fh.writeToXML("<symbol> ] </symbol>");

      }
    }
  }

  fh.writeToXML("</term>");

  return;
}

void CompilationEngine::compileSubroutineCall() {
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string token = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      std::cout << "Error: Subroutine name must be an identifier." << std::endl;
      std::cout << "Token: " << token << std::endl;
      exit(1);
    }

    fh.writeToXML("<identifier> " + token + " </identifier>");

    Token symbol = jt.accessToken(1);

    if(symbol.token == ".") {
      fh.writeToXML("<symbol> . </symbol>");
      jt.advance(2);
      
      Token method = jt.accessToken();

      if(method.type ^ IDENTIFIER) {
        std::cout << "Error: A method name of the class must be an identifier." << std::endl;
        std::cout << "Token: " << method.token << std::endl;
        exit(1);
      }

      fh.writeToXML("<identifier> " + method.token + " </identifier>");
    }
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "(") {
      std::cout << "Error: Invalid to token." << std::endl;
      std::cout << "Token: " << symbol << std::endl;
      exit(1);
    }

    fh.writeToXML("<symbol> ( </symbol>");

    Token checkNext = jt.accessToken(1);

    if(checkNext.token == ")") {
      fh.writeToXML("<expressionList>");
      fh.writeToXML("</expressionList>");
      fh.writeToXML("<symbol> ) </symbol>");
      jt.advance();
    } else {
      compileExpressionList();

      jt.advance();
      Token closeTag = jt.accessToken();

      if(closeTag.token != ")") {
        std::cout << "Error: \')\' is missing." << std::endl;
        std::cout << "Token: " << closeTag.token << std::endl;
        exit(1);
      }

      fh.writeToXML("<symbol> ) </symbol>");
    }
  }

  return;
}

void CompilationEngine::compileExpressionList() {
  fh.writeToXML("<expressionList>");

  while(jt.hasMoreTokens()) {
    compileExpression();

    Token tk = jt.accessToken(1);

    std::string symbol = tk.token;

    if(symbol != ",") break;

    jt.advance();
    fh.writeToXML("<symbol> , </symbol>");
  }

  fh.writeToXML("</expressionList>");

  return;
}

inline bool CompilationEngine::isOp(std::string token) {
  return (token == "+" || token == "-" || token == "*" ||
          token == "/" || token == "&" || token == "|" ||
          token == "<" || token == ">" || token == "=");
}

inline bool CompilationEngine::isUnaryOp(std::string token) {
  return (token == "-" || token == "~");
}