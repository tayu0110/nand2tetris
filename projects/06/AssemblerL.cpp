// #include<cstdio>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cctype>
#include<algorithm>
#include<string>

#define A_COMMAND 1
#define C_COMMAND 2
#define L_COMMAND 4

char* split_ext(char*);
bool isNumber(char*);
std::string acomToBinary(char*);

class Parser {
    FILE *f;
    bool isNext;
    char nowLine[64], nowParsedLine[64];
    std::string d,c,j;
    bool removeComment();
public:
    Parser(FILE*);
    bool hasMoreCommands();
    char* advance();
    int commandType();
    char* symbol();
    void ccParse(char*);
    const char* dest();
    const char* comp();
    const char* jump();
};
Parser::Parser(FILE* fp) {
    this->f = fp;
}
bool Parser::hasMoreCommands() {
    while((fgets(this->nowLine, sizeof(this->nowLine) - 1, this->f)) != NULL) {
        if(this->removeComment()) continue;
        this->isNext = true;
        return this->isNext;
    }
    this->isNext = false;
    return this->isNext;
}
bool Parser::removeComment() {
    char* parsedLine = this->advance();
    char prefix[] = {'\0'};
    int len = strlen(parsedLine);
    if(strcmp(parsedLine, "") == 0) return true;
    if(strcmp(parsedLine, "\n") == 0) return true;
    if(strcmp(parsedLine, "\r\n") == 0) return true;
    if(len < 2) return false;
    for(int i = 0; i < 2; i++) {
        char c[] = {*(parsedLine + i), '\0'};
        strcat(prefix, c);
    }
    if(strcmp(prefix, "//") == 0) return true;
    else return false;
}
char* Parser::advance() {
    char* l = this->nowLine;
    // char* parsed_l = "\0";
    int len = strlen(l);
    memset(this->nowParsedLine, '\0', sizeof(this->nowParsedLine));
    for(int i = 0; i < len; i++) {
        char c[] = {*(l + i), '\0'};
        if((strcmp(c, " ") == 0) || (strcmp(c, "\t") == 0)) continue;
        if((strcmp(c, "\n") == 0) || (strcmp(c, "\r\n") == 0)) continue;
        // strcat(parsed_l, c);
        strcat(this->nowParsedLine, c);
    }
    // return parsed_l;
    return this->nowParsedLine;
}
int Parser::commandType() {
    char* l = this->nowParsedLine;
    int len = strlen(l);
    len--;
    if(*l == '(') {
        if(*(l + len) == ')') return L_COMMAND;
        else {
            printf("Syntax Error: L_COMMAND require \')\'.");
            exit(1);
        }
    } else if(*l == '@') {
        return A_COMMAND;
    } else {
        return C_COMMAND;
    }
}
char* Parser::symbol() {
    // char* com = this->advance();
    char* com = this->nowParsedLine;
    static char sym[] = {'\0'};
    memset(sym, '\0', sizeof(sym));
    int cType = this->commandType();
    int len = strlen(com);
    if(cType == A_COMMAND) {
        for(int i = 1; i < len; i++) {
            char c[] = {*(com + i), '\0'};
            strcat(sym, c);
        }
        return sym;
    } else if(cType == L_COMMAND) {
        for(int i = 1; i < len - 1; i++) {
            char c[] = {*(com + i), '\0'};
            strcat(sym, c);
        }
        return sym;
    } else {
        printf("Error: Invalid command type.");
        exit(1);
    }
}
void Parser::ccParse(char* cc) {
    std::string mnemonic(cc);
    std::string dest="", comp="", jump="";
    for(int i = 0; i < (int)mnemonic.size(); i++) {
        if(mnemonic[i] == '=') {
            dest = mnemonic.substr(0, i);
        } else if(mnemonic[i] == ';') {
            if(dest.empty()) comp = mnemonic.substr(0, i);
            else comp = mnemonic.substr(dest.size() + 1, i - dest.size() - 1);
        }
    }
    if(comp.empty()) {
        if(dest.empty()) comp = mnemonic;
        else comp = mnemonic.substr(dest.size() + 1, mnemonic.size() - dest.size() - 1);
    } else {
        if(dest.empty()) jump = mnemonic.substr(comp.size() + 1, mnemonic.size() - comp.size() -1);
        else jump = mnemonic.substr(dest.size() + comp.size() + 2, mnemonic.size() - dest.size() - comp.size() -2);
    }
    this->d = dest;
    this->c = comp;
    this->j = jump;
    return;
}
const char* Parser::dest() {
    return this->d.c_str();
}
const char* Parser::comp() {
    return this->c.c_str();
}
const char* Parser::jump() {
    return this->j.c_str();
}

class Code {
public:
    const char* dest(const char*);
    const char* comp(const char*);
    const char* jump(const char*);
};
const char* Code::dest(const char* d) {
    const char* dBinary;
    if((strcmp(d, "null") == 0) || (strcmp(d, "") == 0)) dBinary = "000";
    else if(strcmp(d, "M") == 0) dBinary = "001";
    else if(strcmp(d, "D") == 0) dBinary = "010";
    else if((strcmp(d, "MD") == 0) || (strcmp(d, "DM") == 0)) dBinary = "011";
    else if(strcmp(d, "A") == 0) dBinary = "100";
    else if((strcmp(d, "AM") == 0) || (strcmp(d, "MA") == 0)) dBinary = "101";
    else if((strcmp(d, "AD") == 0) || (strcmp(d, "DA") == 0)) dBinary = "110";
    else if((strcmp(d, "AMD") == 0) || (strcmp(d, "ADM") == 0) ||
            (strcmp(d, "DAM") == 0) || (strcmp(d, "DMA") == 0) ||
            (strcmp(d, "MAD") == 0) || (strcmp(d, "MDA") == 0)) dBinary = "111";
    else {
        printf("Syntax Error: Invalid dest mnemonic.\n");
        exit(1);
    }
    return dBinary;
}
const char* Code::comp(const char* c) {
    const char* cBinary;
    if(strcmp(c, "0") == 0) cBinary = "0101010";
    else if(strcmp(c, "1") == 0) cBinary = "0111111";
    else if(strcmp(c, "-1") == 0) cBinary = "0111010";
    else if(strcmp(c, "D") == 0) cBinary = "0001100";
    else if(strcmp(c, "A") == 0) cBinary = "0110000";
    else if(strcmp(c, "!D") == 0) cBinary = "0001101";
    else if(strcmp(c, "!A") == 0) cBinary = "0110001";
    else if(strcmp(c, "-D") == 0) cBinary = "0001111";
    else if(strcmp(c, "-A") == 0) cBinary = "0110011";
    else if(strcmp(c, "D+1") == 0) cBinary = "0011111";
    else if(strcmp(c, "A+1") == 0) cBinary = "0110111";
    else if(strcmp(c, "D-1") == 0) cBinary = "0001110";
    else if(strcmp(c, "A-1") == 0) cBinary = "0110111";
    else if((strcmp(c, "D+A") == 0) || (strcmp(c, "A+D") == 0)) cBinary = "0000010";
    else if(strcmp(c, "D-A") == 0) cBinary = "0010011";
    else if(strcmp(c, "A-D") == 0) cBinary = "0000111";
    else if((strcmp(c, "D&A") == 0) || (strcmp(c, "A&D") == 0)) cBinary = "0000000";
    else if((strcmp(c, "D|A") == 0) || (strcmp(c, "A|D") == 0)) cBinary = "0010101";
    else if(strcmp(c, "M") == 0) cBinary = "1110000";
    else if(strcmp(c, "!M") == 0) cBinary = "1110001";
    else if(strcmp(c, "-M") == 0) cBinary = "1110011";
    else if(strcmp(c, "M+1") == 0) cBinary = "1110111";
    else if(strcmp(c, "M-1") == 0) cBinary = "1110010";
    else if((strcmp(c, "D+M") == 0) || (strcmp(c, "M+D") == 0)) cBinary = "1000010";
    else if(strcmp(c, "D-M") == 0) cBinary = "1010011";
    else if(strcmp(c, "M-D") == 0) cBinary = "1000111";
    else if((strcmp(c, "D&M") == 0) || (strcmp(c, "M&D") == 0)) cBinary = "1000000";
    else if((strcmp(c, "D|M") == 0) || (strcmp(c, "M|D") == 0)) cBinary = "1010101";
    else {
        printf("Syntax Error: Invalid comp mnemonic.\n");
        exit(1);
    }
    return cBinary;
}
const char* Code::jump(const char* j) {
    const char* jBinary;
    if((strcmp(j, "null") == 0) || (strcmp(j, "") == 0)) jBinary = "000";
    else if(strcmp(j, "JGT") == 0) jBinary = "001";
    else if(strcmp(j, "JEQ") == 0) jBinary = "010";
    else if(strcmp(j, "JGE") == 0) jBinary = "011";
    else if(strcmp(j, "JLT") == 0) jBinary = "100";
    else if(strcmp(j, "JNE") == 0) jBinary = "101";
    else if(strcmp(j, "JLE") == 0) jBinary = "110";
    else if(strcmp(j, "JMP") == 0) jBinary = "111";
    else {
        printf("Syntax Error: Invalid jump mnemonic.\n");
        exit(1);
    }
    return jBinary;
}

int main(int argc, char *argv[]) {
    FILE* f;
    char* filename;
    char* extension;

    if(argc == 1) {
        printf("Error: Not enough arguments.");
        exit(1);
    } else if(argc > 2) {
        printf("Error: Too many arguments.");
        exit(1);
    } else {
        filename = argv[1];
        extension = split_ext(filename);
    }
    if(strcmp(extension, "asm") != 0) {
        printf("Error: Invalid argument.");
        exit(1);
    }

    if((f = fopen(filename, "r")) == NULL) {
        printf("Error: Failed to open file.");
        exit(1);
    }

    // Test code
    Parser pf(f);
    while(pf.hasMoreCommands()) {
        char* nowLine = pf.advance();
        int cType = pf.commandType();
        printf("nowLine: %s \t| commandType: %d\n", nowLine, cType);
        if(cType == A_COMMAND) {
            char* sym = pf.symbol();
            std::string bCode;
            // printf("\tsymbol: %s\n", sym);
            if(isNumber(sym)) {
                bCode = acomToBinary(sym);
                printf("Binary Code: %s\n", bCode.c_str());
            } else printf("Unimplemented.\n");
        } else if(cType == C_COMMAND) {
            pf.ccParse(nowLine);
            Code toBinary;
            std::string d(pf.dest());
            std::string c(pf.comp());
            std::string j(pf.jump());
            std::string dBinary = toBinary.dest(d.c_str());
            std::string cBinary = toBinary.comp(c.c_str());
            std::string jBinary = toBinary.jump(j.c_str());
            // printf("\tdest: %s\tcomp: %s\tjump: %s\n", d, c, j);
            std::string bCode = "111" + cBinary + dBinary + jBinary;
            printf("Binary Code: %s\n", bCode.c_str());
        } else if(cType == L_COMMAND) {
            char* sym = pf.symbol();
            // printf("\tsymbol: %s\n", sym);
            printf("Unimplemented.\n");
        } else {
            printf("Error: An internal error has occurred.");
            exit(1);
        }
    }

    if(fclose(f) == EOF) {
        printf("Error: Failed to close file.");
        exit(1);
    }
    
    exit(0);
}

char* split_ext(char* fn) {
    int len = strlen(fn);
    bool flag = false;
    static char extension[] = {'\0'};

    for(int i = 0; i < len; i++) {
        char c[] = {*(fn + i), '\0'};
        if(strcmp(c, ".") == 0) {
            flag = true;
            continue;
        }
        if(flag) strcat(extension, c);
    }

    return extension;
}

bool isNumber(char* symbol) {
    std::string str{symbol};
    if(std::all_of(str.cbegin(), str.cend(), isdigit)) return true;
    else return false;
}

std::string acomToBinary(char* sym) {
    if(!isNumber(sym)) {
        printf("Error: acomToBinary: This symbol is not numeric.\n");
        exit(1);
    }
    int symNumber = atoi(sym);
    std::string symBinary;
    for(int i = 0; i < 16; i++) {
        if(symNumber & (1 << (15 - i))) symBinary += "1";
        else symBinary += "0";
    }
    return symBinary;
}
