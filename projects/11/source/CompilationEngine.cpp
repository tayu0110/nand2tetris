// CompilationEngine.cpp
// Author       : tayu
// Latest Edit  : 2021/03/28
#include"CompilationEngine.h"
#include"FileHandler.h"
#include"JackTokenizer.h"
#include"VMWriter.h"
#include"SymbolTable.h"
#include"ErrorHandler.h"
#include<iostream>
#include<string>
#include<cstdlib>

void CompilationEngine::compile() {
  while(jt.hasMoreTokens()) {
    Token tk = jt.accessToken();

    std::string token = tk.token;

    if(token == "class") compileClass(token);
  }

  return;
}

void CompilationEngine::compileClass(std::string argToken) {
  if(argToken != "class") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error(CompilationEngine.cpp:compileClass()): This token is not class keyword.",
      "Token: " + argToken
    });
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    int tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileClass()): ClassName must be an identifier.",
        "Token: " + tk.token
      });
    }

    // Set class name and make symbol table
    className = tk.token;
    st = SymbolTable();
  }

  // Check "{" symbol. If "{" is not found, make error occur and exit program.
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    if(tk.token != "{") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileClass()): \'{\' is missing.",
        "Token: " + tk.token
      });
    }
  }

  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string token = tk.token;
    int tokenType = tk.type;

    // When "}" is found, leave for complieClass.
    if(token == "}") break;

    if(tokenType ^ KEYWORD) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileClass()): The top of elements of class body requires a keyword that specifies the kind of the statement."
      });
    }

    if(token == "static" || token == "field") {
      compileClassVarDec(token);

    } else if(token == "constructor" || token == "function" || token == "method") {
      compileSubroutine(token);

    } else {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileClass()): The element of class body must be variable declarations or subroutine declarations.",
        "Token: " + token
      });
    }
  }
  
  return;
}

void CompilationEngine::compileClassVarDec(std::string argToken) {
  if(argToken != "static" && argToken != "field") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error: This token is not class variable declaration keyword.",
      "Token: " + argToken
    });
  }

  std::string type;

  // Check a static or field variable type.
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    type = tk.token;
    int tokenType = tk.type;

    if(!(tokenType & (KEYWORD | IDENTIFIER))) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileClassVarDec()): A variable declaration requires a type.",
        "Token: " + type
      });
    }

    if((tokenType & KEYWORD) && !isPrimitiveType(type)) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileClassVarDec()): A variable declaration requires a type.",
        "Token: " + type
      });
    }
  }


  // Extract variable names and register it in the symbol table
  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string varName = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileClassVarDec()): A variable name must be an identifier.",
        "Token: " + varName
      });
    }

    int kind = (argToken == "static" ? KIND_STATIC : KIND_FIELD);

    // Register in the symbol table.
    try {
      st.define(varName, type, kind);
    } catch(ErrorHandler &eh) {
      throw eh;
    }

    jt.advance();
    Token nextToken = jt.accessToken();

    if(nextToken.type ^ SYMBOL) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine2.cpp:compileClassVarDec()): \',\' is missing.",
        "Token: " + nextToken.token
      });
    }

    std::string symbol = nextToken.token;

    if(symbol == ",") continue;
    else if(symbol == ";") break;
    else {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine2.cpp:compileClassVarDec()): Invalid to symbol.",
        "Token: " + symbol
      });
    }
  }

  return;
}

void CompilationEngine::compileSubroutine(std::string argToken) {
  if(argToken != "constructor" && argToken != "function" && argToken != "method") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error: This token is subroutine declaration keyword.",
      "Token: " + argToken
    });
  }

  functionTypePrefix = argToken;

  // Initialize the symbol table of method scope.
  st.startSubroutine();

  int tokenType;
  std::string type;

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    type = tk.token;
    tokenType = tk.type;

    if(!(tokenType & (KEYWORD | IDENTIFIER))) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutine()): A declaration of subroutine requires a type of return value.",
        "Token: " + type
      });
    }

    if((tokenType & KEYWORD) && (type != "void") && !isPrimitiveType(type)) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutine()): A declaration of subroutine requires a type of return value.",
        "Token: " + type
      });
    }
  }

  // Extract subtoutine name.
  std::string subroutineName;
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    subroutineName = tk.token;
    tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutine()): A subroutine name must be a word other than keyword, symbol and integer constant.",
        "Token: " + subroutineName
      });
    }
  }

  // If subroutine is "method", add "this" to arguments.
  if(functionTypePrefix == "method") {
    try {
      st.define("this", className, KIND_ARG);
    } catch(ErrorHandler &eh) {
      throw eh;
    }
  }

  subroutineName = className + "." + subroutineName;
  methodName = subroutineName;
  methodType = type;

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string startSymbol = tk.token;

    // Check "(" is found or not
    if(startSymbol != "(") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutine()): \'(\' is missing.",
        "Symbol: " + startSymbol
      });
    }

    Token checkNext = jt.accessToken(1);

    if(checkNext.token != ")") compileParameterList();

    jt.advance();
    Token closeTag = jt.accessToken();

    if(closeTag.token != ")") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutine()): \')\' is missing.",
        "Token: " + closeTag.token
      });
    }
  }

  compileSubroutineBody();

  return;
}

int CompilationEngine::compileParameterList() {
  int paramNum = 0;
  int kind = KIND_ARG;

  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string type = tk.token;
    int tokenType = tk.type;

    if(!(tokenType & (KEYWORD | IDENTIFIER))) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileParameterList()): Parameters require type.",
        "Token: " + type
      });
    }

    if((tokenType & KEYWORD) && !isPrimitiveType(type)) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileParameterList()): Type name must be int, char, boolean and class name.",
        "Token: " + type
      });
    }

    Token varName = jt.accessToken(1);

    if(varName.type ^ IDENTIFIER) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileParameterList()): A variable name must be an identifier.",
        "Token: " + varName.token
      });
    }

    try {
      st.define(varName.token, type, kind);
    } catch(ErrorHandler &eh) {
      throw eh;
    }

    paramNum++;

    Token symbol = jt.accessToken(2);

    if(symbol.type ^ SYMBOL) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileParameterList()): Paramater list requires to mark with \',\' or terminate with \')\'.",
        "Token: " + symbol.token
      });
    }

    if(symbol.token == ")") {
      jt.advance();
      break;

    } else if(symbol.token == ",") {
      jt.advance(2);

    } else {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileParameterList()): Invalid to symbol.",
        "Token: " + symbol.token
      });
    }
  }

  return paramNum;
}

void CompilationEngine::compileSubroutineBody() {
  // Check "{" is found or not
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "{") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineBody()): \'{\' is missing.",
        "Token: " + symbol
      });
    }
  }

  while(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string token = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ KEYWORD) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineBody()): A variable declaration requires a keyword that specifies the kind of declaration or statement.",
        "Token: " + token
      });
    }

    if(token != "var") break;

    jt.advance();
    compileVarDec(token);
  }

  int varNum = st.varCount(KIND_VAR);

  vw.writeFunction(methodName, varNum);

  if(functionTypePrefix == "method") {
    // Set the 0th hidden argument that points to the current object to the POINTER segment.
    vw.writePush(SEG_ARG, 0);
    vw.writePop(SEG_POINTER, 0);

  } else if(functionTypePrefix == "constructor") {
    // Set the number of field variables as an argument.
    vw.writePush(SEG_CONST, st.varCount(KIND_FIELD));
    // Allocate memory for the size of the object and set it to the POINTER segment.
    vw.writeCall("Memory.alloc", 1);
    vw.writePop(SEG_POINTER, 0);
  }

  while(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string token = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ KEYWORD) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineBody()): A variable declaration and statements requires a keyword that specifies the kind of declaration or statement.",
        "Token: " + token
      });
    }

    if(token == "let" || token == "if" || token == "while" || token == "do" || token == "return") {
      compileStatements();
      break;

    } else {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineBody()): A statements requires a keyword that specifies the kind of declaration or statement.",
        "Token: " + token
      });
    }
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "}") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineBody()): \'}\' is missing.",
        "Token: " + symbol
      });
    }
  }

  return;
}

void CompilationEngine::compileVarDec(std::string argToken) {
  if(argToken != "var") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error: This token is not \"var\" keyword.",
      "Token: " + argToken
    });
  }

  int kind = KIND_VAR;
  std::string type;

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    type = tk.token;
    int tokenType = tk.type;

    if(!(tokenType & (KEYWORD | IDENTIFIER))) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileVarDec()): A variable declaration requires a type.",
        "Token: " + type
      });
    }

    if((tokenType & KEYWORD) && !isPrimitiveType(type)) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileVarDec()): A variable declaration requires a type.",
        "Token: " + type
      });
    }
  }

  // Extract variable name and register it in the symbol table.
  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string varName = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileVarDec()): A variable name must be an identifier.",
        "Token: " + varName
      });
    }

    try {
      st.define(varName, type, kind);
    } catch(ErrorHandler &eh) {
      throw eh;
    }

    Token symbol = jt.accessToken(1);

    if(symbol.type ^ SYMBOL) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileVarDec()): \',\' is required to list the variable names.",
        "Token: " + symbol.token
      });
    }

    if(symbol.token != "," && symbol.token != ";") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileVarDec()): Variable declaration needs to separate variable names with \',\' or to end the declaration with \';\'",
        "Token: " + symbol.token
      });
    }

    jt.advance();

    if(symbol.token == ";") break;
  }

  return;
}

void CompilationEngine::compileStatements() {
  while(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string token = tk.token;
    int tokenType = tk.type;

    if(tokenType & KEYWORD) {
      std::string keyword = token;
      jt.advance();

      if(keyword == "let") compileLet(keyword);
      else if(keyword == "if") compileIf(keyword);
      else if(keyword == "while") compileWhile(keyword);
      else if(keyword == "do") compileDo(keyword);
      else if(keyword == "return") compileReturn(keyword);
      else if(keyword == "var") {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileStatement()): Variable declarations in the class put the front of subroutine declarations.",
          "Token: " + keyword
        });

      } else {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileStatement()): This is not keyword that specifies the kind of statement.",
          "Token: " + keyword
        });
      }

    } else if(tokenType & SYMBOL){
      std::string symbol = token;

      if(symbol != "}") {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileStatement()): Symbols other than \'}\' can not put inside of subroutine body.",
          "Token: " + symbol
        });
      }

      break;

    } else {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileStatement()): Other than keyword can not put the top of statements inside of subroutine body.",
        "Token: " + token
      });
    }
  }

  return;
}

void CompilationEngine::compileDo(std::string argToken) {
  if(argToken != "do") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error: This token is not \"do\" keyword.",
      "Token: " + argToken
    });
  }

  compileSubroutineCall();

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != ";") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileDo()): \';\' is missing.",
        "Token: " + symbol
      });
    }
  }

  // The return value is not needed, or is always zero, so pop the return value.
  vw.writePop(SEG_TEMP, 0);

  return;
}

void CompilationEngine::compileLet(std::string argToken) {
  if(argToken != "let") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error: This token is not \"let\" keyword.",
      "Token: " + argToken
    });
  }

  std::string varName;

  // Extract variable name
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    varName = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileLet()): Variable name is missing.",
        "TokenType: " + std::to_string(tokenType),
        "Token: " + varName
      });
    }
  }

  bool isArray = false;
  int index = st.indexOf(varName);
  int kind = st.kindOf(varName);

  while(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ SYMBOL) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileLet()): Invalid to token.",
        "Token: " + symbol
      });
    }

    if(symbol == "[") {
      compileExpression();

      int segment = convertKindToSegment(kind);

      if(segment == -1) {
        throw ErrorHandler(INTERNAL_ERROR, {
          "Internal Error: Invalid to kind.",
          "VarName: " + varName,
          "Kind: " + std::to_string(kind)
        });
      }

      vw.writePush(segment, index);
      vw.writeArithmetic(COM_ADD);

      Token closeTag = jt.accessToken(1);

      if(closeTag.token != "]") {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileLet()): \']\' is missing.",
          "Token: " + closeTag.token
        });
      }

      isArray = true;

      jt.advance();
      continue;

    } else if(symbol == "=") {
      break;

    } else {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileLet()): Invalid to symbol.",
        "Symbol: " + symbol
      });
    }
  }

  compileExpression();

  if(isArray) {
    vw.writePop(SEG_TEMP, 0);
    vw.writePop(SEG_POINTER, 1);
    vw.writePush(SEG_TEMP, 0);
    vw.writePop(SEG_THAT, 0);

  } else {
    int segment = convertKindToSegment(kind);

    if(segment == -1) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileLet()): This variable name is not found.",
        "Token: " + varName
      });
    }

    vw.writePop(segment, index);
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != ";") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileLet()): \';\' is missing.",
        "Token: " + symbol
      });
    }
  }

  return;
}

void CompilationEngine::compileWhile(std::string argToken) {
  if(argToken != "while") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error: This token is \'while\' keyword.",
      "Token: " + argToken
    });
  }

  std::string whileLabel = methodName + "_WHILE" + std::to_string(jt.getPos());
  std::string breakLabel = methodName + "_BREAK" + std::to_string(jt.getPos());

  vw.writeLabel(whileLabel);

  // Compile the conditional expression
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "(") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileWhile()): \'(\' is missing.",
        "Token: " + symbol
      });
    }

    compileExpression();

    jt.advance();
    Token closeTag = jt.accessToken();

    if(closeTag.token != ")") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileWhile()): \')\' is missing.",
        "Token: " + closeTag.token
      });
    }
  }

  vw.writePush(SEG_CONST, 0);
  vw.writeArithmetic(COM_EQ);
  vw.writeIf(breakLabel);

  // Compile statements
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "{") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileWhile()): \'{\' is missing.",
        "Token: " + symbol
      });
    }

    compileStatements();

    jt.advance();
    Token closeTag = jt.accessToken();

    if(closeTag.token != "}") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileWhile()): \'}\' is missing.",
        "Token: " + closeTag.token
      });
    }
  }

  vw.writeGoto(whileLabel);
  vw.writeLabel(breakLabel);

  return;
}

void CompilationEngine::compileReturn(std::string argToken) {
  if(argToken != "return") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error: This token is not \"return\" keyword.",
      "Token: " + argToken
    });
  }

  if(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string token = tk.token;

    if(token == ";") {
      if(methodType != "void") {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileReturn()): The function and method which is not void function is required a return value.",
          "MethodName: " + methodName
        });
      }

      // The void function tentatively returns 0.
      vw.writePush(SEG_CONST, 0);
      vw.writeReturn();

      jt.advance();
      return;
    }

    if(methodType == "void") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileReturn()): The void function must not return value.",
        "MethodName: " + methodName
      });
    }
    
    compileExpression();
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != ";") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileReturn()): \';\' is missing.",
        "Token: " + symbol
      });
    }
  }

  vw.writeReturn();

  return;
}

void CompilationEngine::compileIf(std::string argToken) {
  if(argToken != "if") {
    throw ErrorHandler(INTERNAL_ERROR, {
      "Internal Error: This token is neither \"if\" nor \"else\" keyword.",
      "Token: " + argToken
    });
  }

  std::string ifLabel = methodName + "_IF" + std::to_string(jt.getPos());
  std::string elseLabel = methodName + "_ELSE" + std::to_string(jt.getPos());

  // Compile conditional expression
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "(") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileIf()): If statement requires a conditional branching expression.",
        "Token: " + symbol
      });
    }

    Token nextToken = jt.accessToken(1);

    if(nextToken.token == ")") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileIf()): If statement requires a conditional branching expression.",
        "Token: " + nextToken.token
      });
    }
        
    compileExpression();
    jt.advance();
    Token closeTag = jt.accessToken();

    if(closeTag.token != ")") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileIf()): \')\' is missing.",
        "Token: " + closeTag.token
      });
    }

    vw.writePush(SEG_CONST, 0);
    vw.writeArithmetic(COM_EQ);
    vw.writeIf(elseLabel);

    jt.advance();
    tk = jt.accessToken();
    symbol = tk.token;

    if(symbol != "{") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileIf()): \'{\' is missing.",
        "Token: " + symbol
      });
    }

    compileStatements();

    jt.advance();
    tk = jt.accessToken();
    symbol = tk.token;

    if(symbol != "}") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileIf()): \'}\' is missing.",
        "Token: " + symbol
      });
    }
  }

  if(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string keyword = tk.token;

    // If "else" is not found, exit from the if statement
    if(keyword != "else") {
      vw.writeLabel(elseLabel);
      return;
    }

    vw.writeGoto(ifLabel);
    vw.writeLabel(elseLabel);

    jt.advance(2);
    tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "{") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileIf()): \'{\' is missing.",
        "Token: " + symbol
      });
    }

    compileStatements();

    jt.advance();
    tk = jt.accessToken();
    symbol = tk.token;

    if(symbol != "}") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileIf()): \'}\' is missing.",
        "Token: " + symbol
      });
    }
  }

  vw.writeLabel(ifLabel);

  return;
}

void CompilationEngine::compileExpression() {
  compileTerm();

  while(jt.hasMoreTokens()) {
    Token tk = jt.accessToken(1);

    std::string token = tk.token;

    if(!isOp(token)) break;

    jt.advance();

    compileTerm();
    
    int command;
    if(token == "+") command = COM_ADD;
    else if(token == "-") command = COM_SUB;
    else if(token == "=") command = COM_EQ;
    else if(token == "<") command = COM_LT;
    else if(token == ">") command = COM_GT;
    else if(token == "&") command = COM_AND;
    else if(token == "|") command = COM_OR;
    else if(token == "*") {
      vw.writeCall("Math.multiply", 2);
      continue;

    } else if(token == "/") {
      vw.writeCall("Math.divide", 2);
      continue;

    } else {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileExpression()): Invalid to symbol.",
        "Token: " + token
      });
    }
    
    vw.writeArithmetic(command);
  }
  
  return;
}

void CompilationEngine::compileTerm() {
  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string token = tk.token;
    int tokenType = tk.type;

    if(tokenType & INT_CONST) {
      vw.writePush(SEG_CONST, std::stoi(token));

    } else if(tokenType & STRING_CONST) {
      int len = token.length();

      vw.writePush(SEG_CONST, len);
      vw.writeCall("String.new", 1);
      
      for(int i = 0; i < len; i++) {
        vw.writePush(SEG_CONST, token[i]);
        vw.writeCall("String.appendChar", 2);
      }

    } else if(tokenType & KEYWORD) {
      if(token == "true") {
        vw.writePush(SEG_CONST, 1);
        vw.writeArithmetic(COM_NEG);

      } else if(token == "false" || token == "null") {
        vw.writePush(SEG_CONST, 0);

      } else if(token == "this") {
        vw.writePush(SEG_POINTER, 0);

      } else {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileTerm()): This keyword does not have a constant value.",
          "Token: " + token
        });
      }

    } else if(tokenType & SYMBOL) {
      if(token == "(") {
        compileExpression();

        jt.advance();
        Token closeTag = jt.accessToken();

        if(closeTag.token != ")") {
          throw ErrorHandler(SYNTAX_ERROR, {
            "Error(CompilationEngine.cpp:compileTerm()): \')\' is missing.",
            "Token: " + closeTag.token
          });
        }

      } else if(isUnaryOp(token)) {
        compileTerm();

        if(token == "-") vw.writeArithmetic(COM_NEG);
        else if(token == "~") vw.writeArithmetic(COM_NOT);
        else {
          throw ErrorHandler(INTERNAL_ERROR, {
            "Internal Error: This token is not an unary symbol.",
            "Token: " + token
          });
        }

      } else {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileTerm()): Invalid to symbol.",
          "Token: " + token
        });
      }

    } else if(tokenType & IDENTIFIER) {
      Token nextToken = jt.accessToken(1);

      if(nextToken.token == "(") {
        jt.advance();

        Token checkNext = jt.accessToken(1);
        int argNum = 0;

        if(checkNext.token != ")") argNum = compileExpressionList();

        vw.writeCall(className + "." + token, argNum);

        jt.advance();

      } else if(nextToken.token == ".") {
        jt.advance();

        compileSubroutineCall(token);

      } else if(nextToken.token == "[") {
        jt.advance();

        // Compile the expression inside of [].
        compileExpression();

        int kind = st.kindOf(token);
        int index = st.indexOf(token);

        int segment = convertKindToSegment(kind);

        if(segment == -1) {
          throw ErrorHandler(INTERNAL_ERROR, {
            "Internal Error: Invalid to kind.",
            "Token: " + token,
            "Kind: " + kind
          });
        }

        vw.writePush(segment, index);

        vw.writeArithmetic(COM_ADD);
        vw.writePop(SEG_POINTER, 1);
        vw.writePush(SEG_THAT, 0);

        jt.advance();
        Token symbol = jt.accessToken();

        if(symbol.token != "]") {
          throw ErrorHandler(SYNTAX_ERROR, {
            "Error(CompilationEngine.cpp:compileTerm()): Invalid to symbol.",
            "Token: " + symbol.token
          });
        }

      } else {
        int kind = st.kindOf(token);
        int index = st.indexOf(token);

        int segment = convertKindToSegment(kind);

        if(segment == -1) {
          throw ErrorHandler(INTERNAL_ERROR, {
            "Internal Error: Invalid to kind.",
            "Token: " + token,
            "Kind: " + kind
          });
        }

        vw.writePush(segment, index);
      }
    }
  }

  return;
}

void CompilationEngine::compileSubroutineCall(std::string cName) {
  std::string subroutineClassName;
  std::string subroutineName;

  // If this subroutine is a class method, connect subroutine name to class name.
  if(cName != "") subroutineClassName = cName;

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    subroutineName = tk.token;
    int tokenType = tk.type;

    if(tokenType ^ IDENTIFIER) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineCall()): Subroutine name must be an identifier.",
        "Token: " + subroutineName
      });
    }

    Token symbol = jt.accessToken(1);

    if(symbol.token == ".") {
      if(cName != "") {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileSubroutineCall()): A class cannot be placed inside a class.",
          "ClassName: " + cName,
          "SubroutineName: " + subroutineName
        });
      }
      jt.advance(2);
      
      Token method = jt.accessToken();

      if(method.type ^ IDENTIFIER) {
        throw ErrorHandler(SYNTAX_ERROR, {
          "Error(CompilationEngine.cpp:compileSubroutineCall()): A method name of the class must be an identifier.",
          "Token: " + method.token
        });
      }

      subroutineClassName = subroutineName;
      subroutineName = method.token;
    }
  }

  int argNum = 0;
  std::string vmFunctionName;

  if(subroutineClassName != "") {
    if(st.kindOf(subroutineClassName) != NONE) {
      int index = st.indexOf(subroutineClassName);
      int kind = st.kindOf(subroutineClassName);

      int segment = convertKindToSegment(kind);

      if(segment == -1) {
        throw ErrorHandler(INTERNAL_ERROR, {
          "Internal Error: Invalid to kind.",
          "Kind: " + kind,
          "ClassName: " + subroutineClassName
        });
      }

      vw.writePush(segment, index);

      subroutineClassName = st.typeOf(subroutineClassName);
      argNum++;
    }

    vmFunctionName = subroutineClassName + "." + subroutineName;

  } else {
    if(st.kindOf(subroutineName) != NONE) {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineCall()): Invalid to subroutine call.",
        "SubroutineName: " + subroutineName
      });
    }

    vw.writePush(SEG_POINTER, 0);

    vmFunctionName = className + "." + subroutineName;
    argNum++;
  }

  if(jt.hasMoreTokens()) {
    jt.advance();
    Token tk = jt.accessToken();

    std::string symbol = tk.token;

    if(symbol != "(") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineCall()): Invalid to token.",
        "Token: " + symbol
      });
    }

    Token checkNext = jt.accessToken(1);

    if(checkNext.token != ")") argNum += compileExpressionList();

    jt.advance();
    Token closeTag = jt.accessToken();

    if(closeTag.token != ")") {
      throw ErrorHandler(SYNTAX_ERROR, {
        "Error(CompilationEngine.cpp:compileSubroutineCall()): \')\' is missing.",
        "Token: " + closeTag.token
      });
    }
  }

  vw.writeCall(vmFunctionName, argNum);

  return;
}

int CompilationEngine::compileExpressionList() {
  int argNum = 0;

  while(jt.hasMoreTokens()) {
    compileExpression();

    Token tk = jt.accessToken(1);
    argNum++;

    std::string symbol = tk.token;

    if(symbol != ",") break;

    jt.advance();
  }

  return argNum;
}

inline bool CompilationEngine::isOp(std::string token) {
  return (token == "+" || token == "-" || token == "*" ||
          token == "/" || token == "&" || token == "|" ||
          token == "<" || token == ">" || token == "=");
}

inline bool CompilationEngine::isUnaryOp(std::string token) {
  return (token == "-" || token == "~");
}

inline bool CompilationEngine::isPrimitiveType(std::string token) {
  return (token == "int" || token == "char" || token == "boolean");
}

inline int CompilationEngine::convertKindToSegment(int kind) {
  if(kind & KIND_ARG) return SEG_ARG;
  if(kind & KIND_FIELD) return SEG_THIS;
  if(kind & KIND_STATIC) return SEG_STATIC;
  if(kind & KIND_VAR) return SEG_LOCAL;
  return -1;
}