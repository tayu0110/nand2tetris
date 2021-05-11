#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<set>
#include<algorithm>
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
bool isVMfile(std::string);
bool isNumber(std::string);
inline bool isSpace(char c) {return ((c == ' ') || (c == '\t'));}

class Parser {
    std::ifstream ifs;
    std::string nowLine;
    std::string parsedCommands[3];
    std::string arithmeticCommands[9] = {"add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"};
    std::set<std::string> checkArithmetic;
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
    int countSpace = 0;
    int prevSpace = 0;
    
    for(int i = 0; i < 3; i++) this->parsedCommands[i] = "";
    
    while(isSpace(line[prevSpace])) prevSpace++;
    
    for(size_t i = prevSpace; i < line.length(); i++) {
        if(i-1 < 0) continue;
        
        if(isSpace(line[i-1]) && !isSpace(line[i])) {
            prevSpace = i;
            countSpace++;
        }
        
        if(countSpace == 3) {
            std::cout << "Error: The Following Line is invalie to syntax." << std::endl;
            std::cout << "\"" << line << "\"" << std::endl;
            exit(1);
        }
        
        if(isSpace(line[i]) && !isSpace(line[i-1])) {
            std::string command = line.substr(prevSpace, i-prevSpace);
            std::cout << "command: " << command << std::endl;
            if((command.length() > 1) && (command.substr(0, 2) == "//")) break;
            this->parsedCommands[countSpace] = command;
        }
        
        if((i == line.length()-1) && (countSpace < 3)) {
            this->parsedCommands[countSpace] = line.substr(prevSpace, line.length()-prevSpace);
            std::cout << "command: " << line.substr(prevSpace, line.length()-prevSpace) << std::endl;
        }
    }
    
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
    else if(command == "if") return C_IF;
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

class CodeWriter {
    std::string fileBaseName;
    std::ofstream ofs;
    int jumpTo;
    void makeBasicArithmetic(std::string, std::vector<std::string>&);
    void makeComparison(std::string, std::string, std::vector<std::string>&);
    void makeAndOr(std::string, std::vector<std::string>&);
    void makeBasicPush(std::string, int, std::vector<std::string>&);
    void makeBasicPop(std::string, int, std::vector<std::string>&);
    void makePushForPointerAndTemp(std::string, int, std::vector<std::string>&);
    void makePopForPointerAndTemp(std::string, int, std::vector<std::string>&);
public:
    CodeWriter(std::string);
    void writeArithmetic(std::string);
    void writePushPop(int, std::string, int);
    void writeEndPoint();
    ~CodeWriter();
};
CodeWriter::CodeWriter(std::string fn) {
    this->jumpTo = 0;
    
    size_t pos = fn.find_last_of('/');
    if(pos != std::string::npos) this->fileBaseName = fn.substr(pos+1);
    else this->fileBaseName = fn;
    
    this->ofs.open(fn + ".asm", std::ios::out);
    
    std::vector<std::string> asmCode(0);
    asmCode.push_back("@256\n");
    asmCode.push_back("D=A\n");
    asmCode.push_back("@SP\n");
    asmCode.push_back("M=D\n");
    
    for(size_t i = 0; i < asmCode.size(); i++) {
        this->ofs << asmCode[i];
    }
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

    for(size_t i = 0; i < asmCode.size(); i++) {
        this->ofs << asmCode[i] << "\n";
    }

    return;
}
void CodeWriter::writePushPop(int command, std::string segment, int index) {
    std::vector<std::string> asmCode(0);
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
            asmCode.push_back("M=M-1");
            asmCode.push_back("A=M");
            asmCode.push_back("D=M");
        } else {
            std::cout << "Error: This command is neither push or pop.\n";
            exit(1);
        }
    } else if(segment == "local") {
        if(command == C_PUSH) {
            this->makeBasicPush("LCL", index, asmCode);
        } else if(command == C_POP) {
            this->makeBasicPop("LCL", index, asmCode);
        } else {
            std::cout << "Error: This command is neither push or pop.\n";
            exit(1);
        }
    } else if(segment == "argument") {
        if(command == C_PUSH) {
            this->makeBasicPush("ARG", index, asmCode);
        } else if(command == C_POP) {
            this->makeBasicPop("ARG", index, asmCode);
        } else {
            std::cout << "Error: This command is neither push or pop.\n";
            exit(1);
        }
    } else if(segment == "this") {
        if(command == C_PUSH) {
            this->makeBasicPush("THIS", index, asmCode);
        } else if(command == C_POP) {
            this->makeBasicPop("THIS", index, asmCode);
        } else {
            std::cout << "Error: This command is neither push or pop.\n";
            exit(1);
        }
    } else if(segment == "that") {
        if(command == C_PUSH) {
            this->makeBasicPush("THAT", index, asmCode);
        } else if(command == C_POP) {
            this->makeBasicPop("THAT", index, asmCode);
        } else {
            std::cout << "Error: This command is neither push or pop.\n";
            exit(1);
        }
    } else if(segment == "pointer") {
        if(command == C_PUSH) {
            this->makePushForPointerAndTemp("THIS", index, asmCode);
        } else if(command == C_POP) {
            this->makePopForPointerAndTemp("THIS", index, asmCode);
        } else {
            std::cout << "Error: This command is neither push or pop.\n";
            exit(1);
        }
    } else if(segment == "temp") {
        if(command == C_PUSH) {
            this->makePushForPointerAndTemp("R5", index, asmCode);
        } else if(command == C_POP) {
            this->makePopForPointerAndTemp("R5", index, asmCode);
        } else {
            std::cout << "Error: This command is neither push or pop.\n";
            exit(1);
        }
    } else if(segment == "static") {
        if(command == C_PUSH) {
            asmCode.push_back("@" + this->fileBaseName + "." + std::to_string(index));
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
            asmCode.push_back("@" + this->fileBaseName + "." + std::to_string(index));
            asmCode.push_back("M=D");
        } else {
            std::cout << "Error: This command is neither push or pop.\n";
            exit(1);
        }
    } else {
        std::cout << "This segment is invalid.\n";
        exit(1);
    }

    for(size_t i = 0; i < asmCode.size(); i++) {
        this->ofs << asmCode[i] << "\n";
    }

    return;
}
void CodeWriter::writeEndPoint() {
    std::vector<std::string> asmCode(0);
    asmCode.push_back("(END)");
    asmCode.push_back("@END");
    asmCode.push_back("0;JMP");

    for(size_t i = 0; i < asmCode.size(); i++) {
        this->ofs << asmCode[i] << "\n";
    }

    return;
}
CodeWriter::~CodeWriter() {
    this->ofs.close();
}

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
                filename.push_back(std::string(direntry->d_name));
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

    for(size_t i = 0; i < filename.size(); i++) {
        std::cout << filename[i] << std::endl;
        std::string fileBasename = fnBasename(filename[i]);
        Parser pf(filename[i]);
        CodeWriter cw(fileBasename);
        
        while(pf.hasMoreCommands()) {
            int cType = pf.commandType();
            std::string argument1;
            int argument2;
            
            if(cType & (C_ARITHMETIC | C_LABEL | C_GOTO | C_IF)) {
                argument1 = pf.arg1();
                if(cType == C_ARITHMETIC) {
                    cw.writeArithmetic(argument1);
                }
            } else if(cType & (C_PUSH | C_POP | C_FUNCTION | C_CALL)) {
                argument1 = pf.arg1();
                argument2 = pf.arg2();
                if(cType & (C_PUSH | C_POP)) {
                    cw.writePushPop(cType, argument1, argument2);
                }
            }
        }
        
        cw.writeEndPoint();
    }
    
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

bool isVMfile(std::string fn) {
    std::string extension = fnExtension(fn);
    
    if(extension == "vm") return true;
    else return false;
}

bool isNumber(std::string str) {
    return std::all_of(str.cbegin(), str.cend(), isdigit);
}
