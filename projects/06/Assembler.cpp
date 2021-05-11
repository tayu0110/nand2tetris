#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<cctype>
#include<algorithm>
#include<string>
#include<vector>
#include<utility>
#include<set>
#include<map>

#define MAX_MNEMONIC 128
#define A_COMMAND 1
#define C_COMMAND 2
#define L_COMMAND 4

char* split_ext(char*);
bool isNumber(std::string);
std::string acomToBinary(std::string);

class Parser {
    FILE *f;
    bool isNext;
    char nowLine[MAX_MNEMONIC], nowParsedLine[MAX_MNEMONIC];
    std::string d,c,j;
    bool removeComment();
public:
    Parser(FILE*);
    bool hasMoreCommands();
    char* advance();
    int commandType();
    std::string symbol();
    void ccParse(std::string);
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
    int len = strlen(l);
    memset(this->nowParsedLine, '\0', sizeof(this->nowParsedLine));
    for(int i = 0; i < len; i++) {
        if((i + 1 < len) && (*(l + i) == '/') && (*(l + i + 1) == '/')) break;
        char c[] = {*(l + i), '\0'};
        if((strcmp(c, " ") == 0) || (strcmp(c, "\t") == 0)) continue;
        if((strcmp(c, "\n") == 0) || (strcmp(c, "\r\n") == 0)) continue;
        strcat(this->nowParsedLine, c);
    }
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
std::string Parser::symbol() {
    char* com = this->nowParsedLine;
    std::string sym = "";
    int cType = this->commandType();
    int len = strlen(com);
    if(cType == A_COMMAND) {
        for(int i = 1; i < len; i++) {
            char c[] = {*(com + i), '\0'};
            sym += c;
        }
        return sym;
    } else if(cType == L_COMMAND) {
        for(int i = 1; i < len - 1; i++) {
            char c[] = {*(com + i), '\0'};
            sym += c;
        }
        return sym;
    } else {
        printf("Error: Invalid command type.");
        exit(1);
    }
}
void Parser::ccParse(std::string cc) {
    std::string dest="", comp="", jump="";
    for(int i = 0; i < (int)cc.size(); i++) {
        if(cc[i] == '=') {
            dest = cc.substr(0, i);
        } else if(cc[i] == ';') {
            if(dest.empty()) comp = cc.substr(0, i);
            else comp = cc.substr(dest.size() + 1, i - dest.size() - 1);
        }
    }
    if(comp.empty()) {
        if(dest.empty()) comp = cc;
        else comp = cc.substr(dest.size() + 1, cc.size() - dest.size() - 1);
    } else {
        if(dest.empty()) jump = cc.substr(comp.size() + 1, cc.size() - comp.size() -1);
        else jump = cc.substr(dest.size() + comp.size() + 2, cc.size() - dest.size() - comp.size() -2);
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
        printf("Syntax Error: Invalid dest mnemonic.(D mnemonic: %s)\n", d);
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
    else if(strcmp(c, "A-1") == 0) cBinary = "0110010";
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
        printf("Syntax Error: Invalid comp mnemonic.(C mnemonic: %s)\n", c);
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
        printf("Syntax Error: Invalid jump mnemonic.(J mnemonic: %s)\n", j);
        exit(1);
    }
    return jBinary;
}

class SymbolTable {
    std::set<int> usedAddresses;
    std::map<std::string, int> table;
public:
    SymbolTable();
    void addEntry(std::string, int);
    bool contains(std::string);
    int symbol(std::string);
    bool isUsedAddress(int);
};
SymbolTable::SymbolTable() {
    this->table.insert(std::make_pair("SP", 0));
    this->usedAddresses.insert(0);
    this->table.insert(std::make_pair("LCL", 1));
    this->usedAddresses.insert(1);
    this->table.insert(std::make_pair("ARG", 2));
    this->usedAddresses.insert(2);
    this->table.insert(std::make_pair("THIS", 3));
    this->usedAddresses.insert(3);
    this->table.insert(std::make_pair("THAT", 4));
    this->usedAddresses.insert(4);
    this->table.insert(std::make_pair("R0", 0));
    this->usedAddresses.insert(0);
    this->table.insert(std::make_pair("R1", 1));
    this->usedAddresses.insert(1);
    this->table.insert(std::make_pair("R2", 2));
    this->usedAddresses.insert(2);
    this->table.insert(std::make_pair("R3", 3));
    this->usedAddresses.insert(3);
    this->table.insert(std::make_pair("R4", 4));
    this->usedAddresses.insert(4);
    this->table.insert(std::make_pair("R5", 5));
    this->usedAddresses.insert(5);
    this->table.insert(std::make_pair("R6", 6));
    this->usedAddresses.insert(6);
    this->table.insert(std::make_pair("R7", 7));
    this->usedAddresses.insert(7);
    this->table.insert(std::make_pair("R8", 8));
    this->usedAddresses.insert(8);
    this->table.insert(std::make_pair("R9", 9));
    this->usedAddresses.insert(9);
    this->table.insert(std::make_pair("R10", 10));
    this->usedAddresses.insert(10);
    this->table.insert(std::make_pair("R11", 11));
    this->usedAddresses.insert(11);
    this->table.insert(std::make_pair("R12", 12));
    this->usedAddresses.insert(12);
    this->table.insert(std::make_pair("R13", 13));
    this->usedAddresses.insert(13);
    this->table.insert(std::make_pair("R14", 14));
    this->usedAddresses.insert(14);
    this->table.insert(std::make_pair("R15", 15));
    this->usedAddresses.insert(15);
    this->table.insert(std::make_pair("SCREEN", 16384));
    this->usedAddresses.insert(16384);
    this->table.insert(std::make_pair("KBD", 24576));
    this->usedAddresses.insert(24576);
    return;
}
void SymbolTable::addEntry(std::string sym, int address) {
    if(this->contains(sym)) {
        printf("Error: (Symbol: %s)This symbol has already been registered.\n", sym.c_str());
        exit(1);
    }
    this->table.insert(std::make_pair(sym, address));
    return;
}
bool SymbolTable::contains(std::string sym) {
    if(this->table.find(sym) != this->table.end()) return true;
    else return false;
}
int SymbolTable::symbol(std::string sym) {
    return this->table[sym];
}
bool SymbolTable::isUsedAddress(int address) {
    if(this->usedAddresses.find(address) != this->usedAddresses.end()) return true;
    else return false;
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

    // Load symbols
    if((f = fopen(filename, "r")) == NULL) {
        printf("Error: Failed to open the source file.");
        exit(1);
    }
    Parser pt(f);
    SymbolTable sTable;
    int romAddress = 0;
    while(pt.hasMoreCommands()) {
        int cType = pt.commandType();
        if((cType == A_COMMAND) || (cType == C_COMMAND)) {
            romAddress++;
        } else {
            std::string sym(pt.symbol());
            sTable.addEntry(sym, romAddress);
        }
    }
    if(fclose(f) == EOF) {
        printf("Error: Failed to close the source file.");
        exit(1);
    }

    // Generate binary codes
    FILE* f2;
    if((f2 = fopen(filename, "r")) == NULL) {
        printf("Error: Failed to open the source file.");
        exit(1);
    }
    Parser pf(f2);
    int ramAddress = 16;
    std::vector<std::string> execCodes(0);
    while(pf.hasMoreCommands()) {
        int cType = pf.commandType();
        std::string bCode;
        if(cType == L_COMMAND) continue;
        else if(cType == A_COMMAND) {
            std::string sym(pf.symbol());
            int address;
            if(isNumber(sym)) {
                bCode = acomToBinary(sym);
            } else {
                if(sTable.contains(sym)) {
                    address = sTable.symbol(sym);
                } else {
                    while(sTable.isUsedAddress(ramAddress)) ramAddress++;
                    sTable.addEntry(sym, ramAddress);
                    address = ramAddress;
                    ramAddress++;
                }
                bCode = acomToBinary(std::to_string(address));
            }
        } else if (cType == C_COMMAND) {
            std::string cCom(pf.advance());
            pf.ccParse(cCom);
            Code toBinary;
            std::string dBinary = toBinary.dest(pf.dest());
            std::string cBinary = toBinary.comp(pf.comp());
            std::string jBinary = toBinary.jump(pf.jump());
            bCode = "111" + cBinary + dBinary + jBinary;
        } else {
            printf("Error: An internal error has occurred.");
            exit(1);
        }
        printf("%s\n", bCode.c_str());
        execCodes.push_back(bCode + "\n");
    }
    if(fclose(f2) == EOF) {
        printf("Error: Failed to close the source file.\n");
        exit(1);
    }

    // Generate executable file
    FILE* execFile;
    char* execFileName;
    char* fileBaseName;
    fileBaseName = strtok(filename, ".");
    sprintf(execFileName, "%s.hack", fileBaseName);
    if((execFile = fopen(execFileName, "w")) == NULL) {
        printf("Error: Failed to open the executive file.\n");
        exit(1);
    }
    for(int i = 0; i < (int)execCodes.size(); i++) {
        if(fprintf(execFile, execCodes[i].c_str()) < 0) {
            printf("Error: Failed to write to the executive file.\n");
            exit(1);
        }
    }
    if(fclose(execFile) == EOF) {
        printf("Error: Failed to close the executive file.\n");
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

bool isNumber(std::string symbol) {
    if(std::all_of(symbol.cbegin(), symbol.cend(), isdigit)) return true;
    else return false;
}

std::string acomToBinary(std::string sym) {
    if(!isNumber(sym)) {
        printf("Error: acomToBinary: This symbol is not numeric.\n");
        exit(1);
    }
    int symNumber = std::stoi(sym);
    std::string symBinary;
    for(int i = 0; i < 16; i++) {
        if(symNumber & (1 << (15 - i))) symBinary += "1";
        else symBinary += "0";
    }
    return symBinary;
}
