#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<set>
#include<algorithm>
#include<regex>
#include<cstdlib>
#include<cctype>
#include<dirent.h>

#define NO_ARGUMENTS    -1
#define C_ARITHMETIC    1
#define C_PUSH          2
#define C_POP           4
#define C_LABEL         8
#define C_GOTO          16
#define C_IF            32
#define C_FUNCTION      64
#define C_RETURN        128
#define C_CALL          256

std::string fnExtension(std::string);
std::string fnBasename(std::string);
std::string dirBasename(std::string);
bool isVMfile(std::string);
bool isNumber(std::string);
inline bool isSpace(char c) {return ((c == ' ') || (c == '\t'));}

class Parser {
    std::ifstream ifs;
    std::string nowLine;
    std::string arithmeticCommands[9] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};
    std::set<std::string> checkArithmetic;
    std::vector<std::string> parsedCommands;
    Parser() : parsedCommands(0) {}
public:
    Parser(std::string);
    bool hasMoreCommands();
    void parse(std::string);
    std::string advance();
    int commandType();
    std::string arg1();
    int arg2();
    ~Parser();
};

class CodeWriter {
    std::ofstream ofs;
    int jumpTo;
    int flame;
    int returnTo;
    int returnAddress;
    std::string fileBaseName;
    std::string className;
    void makeBasicArithmetic(std::string, std::vector<std::string>&);
    void makeComparison(std::string, std::string, std::vector<std::string>&);
    void makeAndOr(std::string, std::vector<std::string>&);
    void makeBasicPush(std::string, int, std::vector<std::string>&);
    void makeBasicPop(std::string, int, std::vector<std::string>&);
    void makePushForPointerAndTemp(std::string, int, std::vector<std::string>&);
    void makePopForPointerAndTemp(std::string, int, std::vector<std::string>&);
    void makePushInCall(std::string, std::vector<std::string>&);
public:
    CodeWriter(std::string);
    void setClassName(std::string);
    void writeBootstrap();
    void writeArithmetic(std::string);
    void writePushPop(int, std::string, int);
    void writeLabel(std::string);
    void writeGoto(std::string);
    void writeIf(std::string);
    void writeCall(std::string, int);
    void writeReturn();
    void writeFunction(std::string, int);
    void writeEndPoint();
    ~CodeWriter();
};

int main(int argc, char* argv[]) {
    std::string argument;
    std::vector<std::string> filename(0);

    if(argc < 2) {
        std::cout << "\nError: Argument is too less.\n";
        exit(1);
    }
    if(argc > 2) {
        std::cout << "\nError: Arguments is too many.\n";
        exit(1);
    }

    argument = argv[1];
    if(isVMfile(argument)) {
        filename.push_back(argument);
    } else {
        DIR *dir;
        struct dirent *direntry;

        dir = opendir(argument.c_str());
        if(dir == NULL) {
            std::cout << "Error: Invalid to argument.\n";
            exit(1);
        }

        direntry = readdir(dir);
        while(direntry != NULL) {
            if(isVMfile(std::string(direntry->d_name))) {
                filename.push_back(argument + std::string(direntry->d_name));
            }

            direntry = readdir(dir);
        }
        
        if(dir != NULL) {
            closedir(dir);
        }
        
        if(filename.empty()) {
            std::cout << "Error: This directory does not have .vm file.\n";
            exit(1);
        }
    }

    std::string fileBasename;
    
    if(filename.size() > 1) {
        fileBasename = argument + dirBasename(argument);
        std::cout << fileBasename << std::endl;
    } else {
        fileBasename = fnBasename(filename[0]);
        std::cout << fileBasename << std::endl;
    }
    
    CodeWriter cw(fileBasename);
    cw.writeBootstrap();
    
    for(size_t i = 0; i < filename.size(); i++) {
        Parser pf(filename[i]);
        std::cout << filename[i] << std::endl;
        
        cw.setClassName(dirBasename(fnBasename(filename[i])));
        std::cout << dirBasename(fnBasename(filename[i])) << std::endl;
        
        while(pf.hasMoreCommands()) {
            int cType = pf.commandType();
            
            if(cType & (C_ARITHMETIC | C_LABEL | C_GOTO | C_IF)) {
                std::string argument1;
                
                argument1 = pf.arg1();
                
                if(cType == C_ARITHMETIC) {
                    cw.writeArithmetic(argument1);
                } else if(cType == C_LABEL) {
                    cw.writeLabel(argument1);
                } else if(cType == C_GOTO) {
                    cw.writeGoto(argument1);
                } else if(cType == C_IF) {
                    cw.writeIf(argument1);
                }
            } else if(cType & (C_PUSH | C_POP | C_FUNCTION | C_CALL)) {
                std::string argument1;
                int argument2;
                
                argument1 = pf.arg1();
                argument2 = pf.arg2();
                
                if(cType & (C_PUSH | C_POP)) {
                    cw.writePushPop(cType, argument1, argument2);
                } else if(cType == C_FUNCTION) {
                    cw.writeFunction(argument1, argument2);
                } else if(cType == C_CALL) {
                    cw.writeCall(argument1, argument2);
                }
            } else if(cType == C_RETURN) {
                cw.writeReturn();
            }
        }
        
    }

    cw.writeEndPoint();

    exit(0);
}

std::string fnExtension(std::string fn) {
    std::string extension;
    size_t len = fn.size();
    
    for(size_t i = len-1; i >= 0; i--) {
        if(fn[i] == '.') {
            if(i+1 < len) {
                extension = fn.substr(i+1);
            } else extension = "";
            
            break;
        }
    }
    
    return extension;
}

std::string fnBasename(std::string fn) {
    std::string basename;
    
    for(size_t i = fn.size()-1; i >= 0; i--) {
        if(fn[i] == '.') {
            basename = fn.substr(0, i);
            break;
        }
    }
    
    return basename;
}

std::string dirBasename(std::string dn) {
    int back = dn.length()-1;
    int len = dn.length();
    std::string basename = "";

    for(int top = dn.length()-1; top >= 0; top--) {
        if(dn[top] == '/' && top+1 < len) {
            basename = dn.substr(top+1, back-top);
            if(basename != "") break;
        } else if(dn[top] == '/' && top+1 >= len) {
            back = top-1;
        }
    }

    if(basename == "") {
        basename = dn.substr(0, back+1);
    }

    return basename;
}

bool isVMfile(std::string fn) {
    std::string extension = fnExtension(fn);
    
    if(extension == "vm") return true;
    else return false;
}

bool isNumber(std::string str) {
    return std::all_of(str.cbegin(), str.cend(), isdigit);
}

Parser::Parser(std::string fn) {
    this->ifs.open(fn, std::ios::in);
    
    for(int i = 0; i < 9; i++) checkArithmetic.insert(arithmeticCommands[i]);
}

bool Parser::hasMoreCommands() {
    while(std::getline(this->ifs, this->nowLine)) {
        this->parse(this->nowLine);
        
        if(parsedCommands[0] != "" && parsedCommands[0] != "\n" && parsedCommands[0] != "\r\n") return true;
    }
    
    return false;
}

void Parser::parse(std::string line) {
    int prevSpace = 0;
    
    while(isSpace(line[prevSpace])) prevSpace++;

    int num = 0;
    int len = line.length();
    
    if(line.find_first_of("//") != std::string::npos) len = line.find_first_of("//");

    this->parsedCommands.assign(0, "");
    
    for(int i = prevSpace; i < len; i++) {
        if(i-1 < 0) continue;

        if(isSpace(line[i-1]) && !isSpace(line[i])) {
            prevSpace = i;
        }
        if(isSpace(line[i]) && !isSpace(line[i-1])) {
            std::string command = line.substr(prevSpace, i-prevSpace);
            std::cout << "command: " << command << std::endl;
            if(num == 3) {
                std::cout << "Error: The Following Line is invalie to syntax." << std::endl;
                std::cout << "\"" << line << "\"" << std::endl;
                exit(1);
            }
            this->parsedCommands.push_back(command);
            num++;
            prevSpace = i + 1;
        }
    }

    if(parsedCommands.size() < 3) parsedCommands.push_back(line.substr(prevSpace, len-prevSpace));

    return;
}

std::string Parser::advance() {
    return this->nowLine;
}

int Parser::commandType() {
    std::string command = this->parsedCommands[0];
    
    if(this->checkArithmetic.find(command) != this->checkArithmetic.end()) return C_ARITHMETIC;
    else if(command == "push") return C_PUSH;
    else if(command == "pop") return C_POP;
    else if(command == "label") return C_LABEL;
    else if(command == "goto") return C_GOTO;
    else if(command == "if-goto") return C_IF;
    else if(command == "function") return C_FUNCTION;
    else if(command == "return") return C_RETURN;
    else if(command == "call") return C_CALL;
    else {
        std::cout << "Error: Invalid to command. : " << command << std::endl;
        std::cout << "The following line : " << this->nowLine << std::endl;
        exit(1);
    }
}

std::string Parser::arg1() {
    if(commandType() == C_ARITHMETIC) return this->parsedCommands[0];
    else return this->parsedCommands[1];
}

int Parser::arg2() {
    std::string argument = this->parsedCommands[2];

    if(argument == "") return NO_ARGUMENTS;

    if(isNumber(argument)) return std::stoi(argument);
    else {
        std::cout << "Error: Invalid to the argument. The error is occured on the following line.\n";
        std::cout << "\"" << this->nowLine << "\"" << std::endl;
        exit(1);
    }
}

Parser::~Parser() {
    this->ifs.close();
}

CodeWriter::CodeWriter(std::string fn) {
    this->jumpTo = 0;
    this->flame = 0;
    this->returnTo = 0;
    this->returnAddress = 0;
    this->className = "";
    
    size_t pos = fn.find_last_of('/');
    
    if(pos != std::string::npos) this->fileBaseName = fn.substr(pos+1);
    else this->fileBaseName = fn;
    
    this->ofs.open(fn + ".asm", std::ios::out);
    if(!ofs) {
        std::cout << "Error: Failed to open file." << std::endl;
        exit(1);
    }
}

void CodeWriter::setClassName(std::string className) {
    this->className = className;

    return;
}

void CodeWriter::makeBasicArithmetic(std::string op, std::vector<std::string> &asmCode) {
    asmCode.push_back("@SP");
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("A=A-1");
    asmCode.push_back("M=M" + op + "D");
    
    return;
}

void CodeWriter::makeComparison(std::string op, std::string jt, std::vector<std::string> &asmCode) {
    asmCode.push_back("@SP");
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("A=A-1");
    asmCode.push_back("D=M-D");
    asmCode.push_back("@TRUE" + jt);
    asmCode.push_back("D;" + op);
    asmCode.push_back("@SP");
    asmCode.push_back("A=M-1");
    asmCode.push_back("M=0");
    asmCode.push_back("@CONTINUE" + jt);
    asmCode.push_back("0;JMP");
    asmCode.push_back("(TRUE" + jt + ")");
    asmCode.push_back("@SP");
    asmCode.push_back("A=M-1");
    asmCode.push_back("M=-1");
    asmCode.push_back("(CONTINUE" + jt + ")");
    
    return;
}

void CodeWriter::makeAndOr(std::string op, std::vector<std::string> &asmCode) {
    asmCode.push_back("@SP");
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("A=A-1");
    asmCode.push_back("M=M" + op + "D");
    
    return;
}

void CodeWriter::makeBasicPush(std::string segment, int index, std::vector<std::string> &asmCode) {
    std::string idx = std::to_string(index);
    
    asmCode.push_back("@" + segment);
    asmCode.push_back("D=M");
    asmCode.push_back("@" + idx);
    asmCode.push_back("A=D+A");
    asmCode.push_back("D=M");
    asmCode.push_back("@SP");
    asmCode.push_back("A=M");
    asmCode.push_back("M=D");
    asmCode.push_back("@SP");
    asmCode.push_back("M=M+1");
    
    return;
}

void CodeWriter::makeBasicPop(std::string segment, int index, std::vector<std::string> &asmCode) {
    std::string idx = std::to_string(index);
    
    asmCode.push_back("@SP");
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("@R13");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + segment);
    asmCode.push_back("D=M");
    asmCode.push_back("@" + idx);
    asmCode.push_back("D=D+A");
    asmCode.push_back("@R14");
    asmCode.push_back("M=D");
    asmCode.push_back("@R13");
    asmCode.push_back("D=M");
    asmCode.push_back("@R14");
    asmCode.push_back("A=M");
    asmCode.push_back("M=D");
    
    return;
}

void CodeWriter::makePushForPointerAndTemp(std::string segment, int index, std::vector<std::string> &asmCode) {
    std::string idx = std::to_string(index);
    
    asmCode.push_back("@" + segment);
    asmCode.push_back("D=A");
    asmCode.push_back("@" + idx);
    asmCode.push_back("A=D+A");
    asmCode.push_back("D=M");
    asmCode.push_back("@SP");
    asmCode.push_back("A=M");
    asmCode.push_back("M=D");
    asmCode.push_back("@SP");
    asmCode.push_back("M=M+1");
    
    return;
}

void CodeWriter::makePopForPointerAndTemp(std::string segment, int index, std::vector<std::string> &asmCode) {
    std::string idx = std::to_string(index);
    
    asmCode.push_back("@SP");
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("@R13");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + segment);
    asmCode.push_back("D=A");
    asmCode.push_back("@" + idx);
    asmCode.push_back("D=D+A");
    asmCode.push_back("@R14");
    asmCode.push_back("M=D");
    asmCode.push_back("@R13");
    asmCode.push_back("D=M");
    asmCode.push_back("@R14");
    asmCode.push_back("A=M");
    asmCode.push_back("M=D");
    
    return;
}

void CodeWriter::makePushInCall(std::string segment, std::vector<std::string> &asmCode) {
    asmCode.push_back("@" + segment);
    asmCode.push_back("D=M");
    asmCode.push_back("@SP");
    asmCode.push_back("A=M");
    asmCode.push_back("M=D");
    asmCode.push_back("@SP");
    asmCode.push_back("M=M+1");
    return;
}

void CodeWriter::writeBootstrap() {
    std::vector<std::string> asmCode(0);
    
    asmCode.push_back("@256");
    asmCode.push_back("D=A");
    asmCode.push_back("@SP");
    asmCode.push_back("M=D");
    
    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }

    this->writeCall("Sys.init", 0);

    return;
}

void CodeWriter::writeArithmetic(std::string command) {
    std::vector<std::string> asmCode(0);
    
    if(command == "add") {
        this->makeBasicArithmetic("+", asmCode);
    } else if(command == "sub") {
        this->makeBasicArithmetic("-", asmCode);
    } else if(command == "neg") {
        asmCode.push_back("@SP");
        asmCode.push_back("A=M-1");
        asmCode.push_back("M=-M");
    } else if(command == "eq") {
        std::string jt = std::to_string(this->jumpTo);
        jumpTo++;
        this->makeComparison("JEQ", jt, asmCode);
    }else if(command == "gt") {
        std::string jt = std::to_string(this->jumpTo);
        jumpTo++;
        this->makeComparison("JGT", jt, asmCode);
    } else if(command == "lt") {
        std::string jt = std::to_string(this->jumpTo);
        jumpTo++;
        this->makeComparison("JLT", jt, asmCode);
    } else if(command == "and") {
        this->makeAndOr("&", asmCode);
    } else if(command == "or") {
        this->makeAndOr("|", asmCode);
    } else if(command == "not") {
        asmCode.push_back("@SP");
        asmCode.push_back("A=M-1");
        asmCode.push_back("M=!M");
    } else {
        std::cout << "Error: This command is not arithmetic command.\n";
        exit(1);
    }

    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }

    return;
}

void CodeWriter::writePushPop(int command, std::string segment, int index) {
    std::vector<std::string> asmCode(0);
    bool error = false;
    
    if(segment == "constant") {
        if(command == C_PUSH) {
            asmCode.push_back("@" + std::to_string(index));
            asmCode.push_back("D=A");
            asmCode.push_back("@SP");
            asmCode.push_back("A=M");
            asmCode.push_back("M=D");
            asmCode.push_back("@SP");
            asmCode.push_back("M=M+1");
        } else if(command == C_POP) {
            asmCode.push_back("@SP");
            asmCode.push_back("AM=M-1");
            asmCode.push_back("D=M");
        } else error = true;
    } else if(segment == "local") {
        if(command == C_PUSH) {
            this->makeBasicPush("LCL", index, asmCode);
        } else if(command == C_POP) {
            this->makeBasicPop("LCL", index, asmCode);
        } else error = true;
    } else if(segment == "argument") {
        if(command == C_PUSH) {
            this->makeBasicPush("ARG", index, asmCode);
        } else if(command == C_POP) {
            this->makeBasicPop("ARG", index, asmCode);
        } else error = true;
    } else if(segment == "this") {
        if(command == C_PUSH) {
            this->makeBasicPush("THIS", index, asmCode);
        } else if(command == C_POP) {
            this->makeBasicPop("THIS", index, asmCode);
        } else error = true;
    } else if(segment == "that") {
        if(command == C_PUSH) {
            this->makeBasicPush("THAT", index, asmCode);
        } else if(command == C_POP) {
            this->makeBasicPop("THAT", index, asmCode);
        } else error = true;
    } else if(segment == "pointer") {
        if(command == C_PUSH) {
            this->makePushForPointerAndTemp("THIS", index, asmCode);
        } else if(command == C_POP) {
            this->makePopForPointerAndTemp("THIS", index, asmCode);
        } else error = true;
    } else if(segment == "temp") {
        if(command == C_PUSH) {
            this->makePushForPointerAndTemp("R5", index, asmCode);
        } else if(command == C_POP) {
            this->makePopForPointerAndTemp("R5", index, asmCode);
        } else error = true;
    } else if(segment == "static") {
        if(command == C_PUSH) {
            asmCode.push_back("@" + this->className + "." + std::to_string(index));
            asmCode.push_back("D=M");
            asmCode.push_back("@SP");
            asmCode.push_back("A=M");
            asmCode.push_back("M=D");
            asmCode.push_back("@SP");
            asmCode.push_back("M=M+1");
        } else if(command == C_POP) {
            asmCode.push_back("@SP");
            asmCode.push_back("AM=M-1");
            asmCode.push_back("D=M");
            asmCode.push_back("@" + this->className + "." + std::to_string(index));
            asmCode.push_back("M=D");
        } else error = true;
    } else {
        std::cout << "This segment is invalid.\n";
        exit(1);
    }

    if(error) {
        std::cout << "Error: This command is neither push or pop.\n";
        exit(1);
    }

    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }

    return;
}

void CodeWriter::writeLabel(std::string label) {
    std::regex validator("^[a-zA-Z_.:][a-zA-Z0-9_.:]*$");
    std::smatch smatch;
    
    if(!std::regex_match(label, smatch, validator)) {
        std::cout << "Error: Invalid label." << std::endl;
        std::cout << "Label -> " << label << std::endl;
        exit(1);
    }
    this->ofs << "(" + label + ")" << std::endl;
    return;
}

void CodeWriter::writeGoto(std::string label) {
    std::vector<std::string> asmCode(0);
    
    asmCode.push_back("@" + label);
    asmCode.push_back("0;JMP");
    
    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }
    return;
}

void CodeWriter::writeIf(std::string label) {
    std::vector<std::string> asmCode(0);
    
    asmCode.push_back("@SP");
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("@" + label);
    asmCode.push_back("D;JNE");
    
    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }
    return;
}

void CodeWriter::writeCall(std::string functionName, int numArgs) {
    std::vector<std::string> asmCode(0);
    std::string returnLabel = "RETURN:" + functionName + ":" + std::to_string(this->returnTo);
    
    asmCode.push_back("@" + returnLabel);
    asmCode.push_back("D=A");
    asmCode.push_back("@SP");
    asmCode.push_back("A=M");
    asmCode.push_back("M=D");
    asmCode.push_back("@SP");
    asmCode.push_back("M=M+1");
    this->makePushInCall("LCL", asmCode);
    this->makePushInCall("ARG", asmCode);
    this->makePushInCall("THIS", asmCode);
    this->makePushInCall("THAT", asmCode);
    asmCode.push_back("@SP");
    asmCode.push_back("D=M");
    asmCode.push_back("@LCL");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + std::to_string(numArgs + 5));
    asmCode.push_back("D=D-A");
    asmCode.push_back("@ARG");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + functionName);
    asmCode.push_back("0;JMP");
    asmCode.push_back("(" + returnLabel + ")");
    
    this->returnTo++;
    
    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }

    return;
}

void CodeWriter::writeReturn() {
    std::vector<std::string> asmCode(0);
    std::string funcFlame = "FLAME:" + std::to_string(this->flame);
    std::string retAddress = "RET:" + std::to_string(this->returnAddress);
    
    asmCode.push_back("@LCL");
    asmCode.push_back("D=M");
    asmCode.push_back("@" + funcFlame);
    asmCode.push_back("M=D");
    asmCode.push_back("@5");
    asmCode.push_back("A=D-A");
    asmCode.push_back("D=M");
    asmCode.push_back("@" + retAddress);
    asmCode.push_back("M=D");
    asmCode.push_back("@SP");
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("@ARG");
    asmCode.push_back("A=M");
    asmCode.push_back("M=D");
    asmCode.push_back("@ARG");
    asmCode.push_back("D=M+1");
    asmCode.push_back("@SP");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + funcFlame);
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("@THAT");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + funcFlame);
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("@THIS");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + funcFlame);
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("@ARG");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + funcFlame);
    asmCode.push_back("AM=M-1");
    asmCode.push_back("D=M");
    asmCode.push_back("@LCL");
    asmCode.push_back("M=D");
    asmCode.push_back("@" + retAddress);
    asmCode.push_back("A=M");
    asmCode.push_back("0;JMP");

    this->flame++;
    this->returnAddress++;
    
    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }
    
    return;
}

void CodeWriter::writeFunction(std::string functionName, int numArgs) {
    std::vector<std::string> asmCode(0);
    
    asmCode.push_back("(" + functionName + ")");

    if(numArgs > 0) {
        asmCode.push_back("@0");
        asmCode.push_back("D=A");
        asmCode.push_back("@SP");
        asmCode.push_back("A=M");
    }
    
    for(int i = 0; i < numArgs; i++) {
        asmCode.push_back("M=D");
        asmCode.push_back("@SP");
        asmCode.push_back("AM=M+1");
    }
    
    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }
    
    return;
}

void CodeWriter::writeEndPoint() {
    std::vector<std::string> asmCode(0);
    
    asmCode.push_back("(END)");
    asmCode.push_back("@END");
    asmCode.push_back("0;JMP");

    for(auto code : asmCode) {
        this->ofs << code << std::endl;
    }

    return;
}

CodeWriter::~CodeWriter() {
    this->ofs.close();
}
