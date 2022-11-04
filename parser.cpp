#include "lexer.h"
#include "vasm.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <fstream>

using namespace std;

//File
ofstream outfile;

//For generating error codes
static int curr_line = 1;

//Token list
static vector<Token> token_list;
static size_t curr_token = 0;
int peek_token(){
    if(curr_token < token_list.size())
        return token_list[curr_token].type;
    return -1;
}
Token get_token(){
    if(curr_token < token_list.size())
        return token_list[curr_token++];
    else{
        printf("Consumed too much token\n");
        exit(1);
    }
}


//Symbol table
map<string,int> symtab;

//Parse result
vector<Inst_node> prog;

//Reserved word
map<string, int> reserved_ops;
void build_ops_table(){
    reserved_ops.insert(pair<string, int>("NOP", 0));
    reserved_ops.insert(pair<string, int>("BRA", 2));
    reserved_ops.insert(pair<string, int>("LD", 2));
    reserved_ops.insert(pair<string, int>("STR", 2));
    reserved_ops.insert(pair<string, int>("ADD", 2));
    reserved_ops.insert(pair<string, int>("MUL", 2));
    reserved_ops.insert(pair<string, int>("CMP", 2));
    reserved_ops.insert(pair<string, int>("SHRA", 2));
    reserved_ops.insert(pair<string, int>("SHL", 2));
    reserved_ops.insert(pair<string, int>("SHR", 2));
    reserved_ops.insert(pair<string, int>("AND", 2));
    reserved_ops.insert(pair<string, int>("OR", 2));
    reserved_ops.insert(pair<string, int>("XOR", 2));
    reserved_ops.insert(pair<string, int>("HLT", 0));
}
//Matching function, return required operand number, -1 for not found
int search_ops(string s){
    map<string, int>::iterator it = reserved_ops.find(s);
    if(it == reserved_ops.end())
        return -1;
    else
        return it->second;
}

//Parser function, no semantic check done here
void parse(){
    build_ops_table();
    //Checks maybe required
    token_list = get_token_list();
    //Parse
    while(peek_token() != -1 && peek_token() != EOF_T){
        Inst_node n;
        Token t;
        //Parse empty line
        if(peek_token() == NEWLINE_T){
            get_token();
            continue;
        }
        //Parse hex memory value
        if(peek_token() == HEX_T){
            t = get_token();
            n.label.type = EMPTY;
            n.op.type = EMPTY;
            n.dst = t;
            n.src.type = EMPTY;
            if(t.hex.size() != 8){
                printf("Line %d, hex value must be 8 characters long\n", curr_line + 1);
                exit(1);
            }
            //parse new line
            if(peek_token() != EOF_T && peek_token() != NEWLINE_T){
                printf("Line %d, expect new line\n", curr_line);
                exit(1);
            }
            //Consume newline
            get_token();
            curr_line++;
            prog.push_back(n);
            continue;
        }
        if(peek_token() == IDENT_T){
            t = get_token();
            //Check is label
            if(peek_token() == COLON_T){
                n.label = t;
                //Consume colon
                get_token();
                //Check for labeled memory variable need to be added
                /*****NEED TO ADD FUNCTION HERE******/
                if(peek_token() == HEX_T){ //Labeled memory
                    t = get_token();
                    n.op.type = EMPTY;
                    n.dst = t;
                    n.src.type = EMPTY;
                    if(t.hex.size() != 8){
                        printf("Line %d, hex value must be 8 characters long\n", curr_line + 1);
                        exit(1);
                    }
                    //Parse new line
                    if(peek_token() != EOF_T && peek_token() != NEWLINE_T){
                        printf("Line %d, expect new line\n", curr_line);
                        exit(1);
                    }
                    get_token();
                    curr_line++;
                    prog.push_back(n);
                    continue;
                }
                if(peek_token() != IDENT_T){
                    printf("Line %d, Expect identifier\n", curr_line);
                    exit(1);
                }
                //Renew token
                t = get_token();
            }else{
                n.label.type = EMPTY;
            }
        }
        //Match operation
        int op_num = search_ops(t.ident);
        if(op_num == -1){
            printf("Line %d, Error : no such instruction\n", curr_line);
            exit(1);
        }
        n.op = t;
        if(op_num == 0){
            if(peek_token() != EOF_T && peek_token() != NEWLINE_T){
                printf("Line %d, Too many operands\n", curr_line);
                exit(1);
            }
            //Consume newline
            get_token();
            curr_line++;
            n.src.type = EMPTY;
            n.dst.type = EMPTY;
            prog.push_back(n);
            continue;
        }
        if(op_num == 2){
            if(peek_token() != IMM_T && peek_token() != IDENT_T){
                printf("Line %d, error destination operand\n", curr_line);
                exit(1);
            }
            t = get_token();
            n.dst = t;
            //parse comma
            if(peek_token() != COMMA_T){
                printf("Line %d, expect ,\n", curr_line);
                exit(1);
            }
            get_token();
            //parse second operand
            if(peek_token() != IMM_T && peek_token() != IDENT_T){
                printf("Line %d, error source operand\n", curr_line);
                exit(1);
            }
            t = get_token();
            n.src = t;
            //parse new line
            if(peek_token() != EOF_T && peek_token() != NEWLINE_T){
                printf("Line %d, expect new line\n", curr_line);
                exit(1);
            }
            //Consume newline
            get_token();
            curr_line++;
            prog.push_back(n);
            continue;
        }
    }
    return ;
}
void print_prog_line(size_t i, int label){
    //Print label
    if(label){
        if(prog[i].label.type == EMPTY)
            printf("\t\t\t");
        else
            printf("%s:\t\t\t", prog[i].label.ident.c_str());
    }
    //Print inst
    if(prog[i].op.type != EMPTY)
        printf("%s ", prog[i].op.ident.c_str());
    //Print dst
    if(prog[i].dst.type == IDENT_T)
        printf("%s, ", prog[i].dst.ident.c_str());
    else if(prog[i].dst.type == IMM_T)
        printf("%d, ", prog[i].dst.imm);
    else if(prog[i].dst.type == HEX_T)
        printf("0x%s ", prog[i].dst.hex.c_str());
    //Print src
    if(prog[i].src.type == IDENT_T)
        printf("%s ", prog[i].src.ident.c_str());
    else if(prog[i].src.type == IMM_T)
        printf("#%d ", prog[i].src.imm);
    printf("\n");
}

void print_prog(){
    for(size_t i = 0; i < prog.size(); i++){
        print_prog_line(i, 1);
    }
}

//Semantic check
void build_symtab(){
    for(size_t i = 0; i < prog.size(); i++){
        if(prog[i].label.type != EMPTY){
            if(symtab.find(prog[i].label.ident) != symtab.end()){
                printf("Line %d, redifinition of label %s\n",i + 1, prog[i].label.ident.c_str());
                exit(1);
            }
            symtab.insert(pair<string, int>(prog[i].label.ident, i));
        }
    }
}

string int_to_bin(int n){
    string res;
      while(n!=0) {res=(n%2==0 ?"0":"1")+res; n/=2;}
    return res;
}

string hex_to_bin(string s){
    string res = "";
    for(size_t i = 0; i < s.size(); i++){
        switch(s[i]){
            case '0':
                res += "0000";
                break;
            case '1':
                res += "0001";
                break;
            case '2':
                res += "0010";
                break;
            case '3':
                res += "0011";
                break;
            case '4':
                res += "0100";
                break;
            case '5':
                res += "0101";
                break;
            case '6':
                res += "0110";
                break;
            case '7':
                res += "0111";
                break;
            case '8':
                res += "1000";
                break;
            case '9':
                res += "1001";
                break;
            case 'a':
                res += "1010";
                break;
            case 'b':
                res += "1011";
                break;
            case 'c':
                res += "1100";
                break;
            case 'd':
                res += "1101";
                break;
            case 'e':
                res += "1110";
                break;
            case 'f':
                res += "1111";
                break;
            default:
                printf("Error hex value,got %s\n", s.c_str());
                exit(1);
        }
    }
    return res;
}
//build instruction table
map<string,string> insttab;
void build_insttab(){
    insttab.insert(pair<string, string>("NOP", "0000"));
    insttab.insert(pair<string, string>("BRA", "0001"));
    insttab.insert(pair<string, string>("LD", "0010"));
    insttab.insert(pair<string, string>("STR", "0011"));
    insttab.insert(pair<string, string>("ADD", "0100"));
    insttab.insert(pair<string, string>("MUL", "0101"));
    insttab.insert(pair<string, string>("CMP", "0110"));
    insttab.insert(pair<string, string>("SHRA", "0111"));
    insttab.insert(pair<string, string>("SHL", "1000"));
    insttab.insert(pair<string, string>("SHR", "1001"));
    insttab.insert(pair<string, string>("AND", "1010"));
    insttab.insert(pair<string, string>("OR", "1011"));
    insttab.insert(pair<string, string>("XOR", "1100"));
    insttab.insert(pair<string, string>("HLT", "1101"));
}
//build condition table
map<string, string> condtab;
void build_condtab(){
    //insert condition mapping here
    condtab.insert(pair<string, string>("ALW", "0000"));
    condtab.insert(pair<string, string>("CCC", "0001"));
    condtab.insert(pair<string, string>("CCE", "0010"));
    condtab.insert(pair<string, string>("CCP", "0011"));
    condtab.insert(pair<string, string>("CCZ", "0100"));
    condtab.insert(pair<string, string>("CCN", "0101"));
}

//parse reg identifier
//Return register number if success, -1 otherwise
int parse_reg_id(string s) {
    if(s.size() <= 1 || s[0] != 'R')
        return -1;
    int res = 0;
    for(size_t i = 1; i < s.size(); i++){
        res *= 10;
        res += s[i] - '0';
    }
    if(res > 15)
        return -1;
    return res;
}


//Map program to machine code
//Branching condition have special reserved word
//ZERO, EVEN, PARITY, NEGATIVE, ALWAYS
//These five condition check must be specified in the specs
//More clarifications needed
vector<string> machine_code;
void asm_to_machine(){
    build_symtab();
    build_condtab();
    build_insttab();
    for(size_t i = 0; i < prog.size(); i++){
        string res;
        //Labeled memory
        if(prog[i].op.type == EMPTY && prog[i].dst.type == HEX_T){
            res = hex_to_bin(prog[i].dst.hex.c_str());
            machine_code.push_back(res);
            continue;
        }
        //Translate opcode
        map<string, string>::iterator ops = insttab.find(prog[i].op.ident);
        if(ops != insttab.end()){
            res += ops->second;
        }else{
            goto NO_OP;
        }
        //NOP HLT
        if(prog[i].op.ident == "NOP" || prog[i].op.ident == "HLT"){
            res += "1111111111111111111111111111";
            machine_code.push_back(res);
            continue;
        }
        //BRA
        if(prog[i].op.ident == "BRA"){
            map<string, string>::iterator conds = condtab.find(prog[i].src.ident);
            if(conds == condtab.end()){
                printf("Line %d, Error condition code\n", i + 1);
                exit(1);
            }
            if(prog[i].dst.type != IDENT_T){
                printf("Line %d, Branch destination must be a memory location\n", i + 1);
                exit(1);
            }
            map<string, int>::iterator dst = symtab.find(prog[i].dst.ident);
            if(dst == symtab.end()){
                printf("Line %d, Unknown label\n", i + 1);
                exit(1);
            }
            //27~24
            res += conds->second;
            //src
            res += "000000000000";
            //dst
            res += bitset<12>(dst->second).to_string();
            machine_code.push_back(res);
            continue;
        }
        //ADD, MUL, CMP, AND, OR, XOR ==> reg, src
        //Source can be immediate or register
        if (prog[i].op.ident == "ADD" ||
            prog[i].op.ident == "MUL" ||
            prog[i].op.ident == "CMP" ||
            prog[i].op.ident == "AND" ||
            prog[i].op.ident == "OR"  ||
            prog[i].op.ident == "XOR") {
            //27 ~ 24
            //determine source type
            if(prog[i].src.type == IMM_T){
                res += "1000";
            }else if(prog[i].src.type == IDENT_T){
                res += "0000";
            }else{
                printf("Line %d, Source must be immediate or register\n", i + 1);
                exit(1);
            }
            //src
            if(prog[i].src.type == IMM_T){
                //Immediate
                res += bitset<12>(prog[i].src.imm).to_string();
            }else{
                int reg = parse_reg_id(prog[i].src.ident);
                if(reg == -1){
                    printf("Line %d, Error source register\n", i + 1);
                    exit(1);
                }
                res += bitset<12>(reg).to_string();
            }
            //dst
            if(prog[i].dst.type != IDENT_T){
                printf("Line %d, Error destination, must be a resigter\n", i + 1);
                exit(1);
            }else{
                int reg = parse_reg_id(prog[i].dst.ident);
                if(reg == -1){
                    printf("Line %d, Error destination register\n", i + 1);
                    exit(1);
                }
                res += bitset<12>(reg).to_string();
            }
            machine_code.push_back(res);
            continue;
        }
        //SHL, SHRA, SHR
        if (prog[i].op.ident == "SHRA" ||
            prog[i].op.ident == "SHL" ||
            prog[i].op.ident == "SHR"){
            //27 ~ 24
            if(prog[i].src.type == IMM_T){
                res += "1000";
            }else if(prog[i].src.type == IDENT_T){
                res += "0000";
            }else{
                printf("Line %d, cnt must be immediate or register\n", i + 1);
                exit(1);
            }
            //src
            if(prog[i].src.type == IMM_T){
                if(prog[i].src.imm >= 32)
                    printf("Warning: shift count larger than 32 is unreasonable\n");
                //Immediate
                res += bitset<12>(prog[i].src.imm).to_string();
            }else{
                int reg = parse_reg_id(prog[i].src.ident);
                if(reg == -1){
                    printf("Line %d, Error source register\n", i + 1);
                    exit(1);
                }
                res += bitset<12>(reg).to_string();
            }
            //dst
            if(prog[i].dst.type != IDENT_T){
                printf("Line %d, Error destination, must be a resigter\n", i + 1);
                exit(1);
            }else{
                int reg = parse_reg_id(prog[i].dst.ident);
                if(reg == -1){
                    printf("Line %d, Error destination register\n", i + 1);
                    exit(1);
                }
                res += bitset<12>(reg).to_string();
            }
            machine_code.push_back(res);
            continue;
        }
        //Load  source of LD must be immediate or memory
        if(prog[i].op.ident == "LD"){
            //27 ~ 24 
            if(prog[i].src.type == IMM_T){
                res += "1000";
            }else if(prog[i].src.type == IDENT_T){
                res += "0000";
            }else{
                printf("Line %d, src must be immediate or memory\n", i + 1);
                exit(1);
            }
            //Semantic check
            if(prog[i].src.type != IDENT_T && prog[i].src.type != IMM_T){
                printf("Line %d, Load destination must be a memory location or immediate\n", i + 1);
                exit(1);
            }
            map<string, int>::iterator src_loc = symtab.find(prog[i].src.ident);
            if(prog[i].src.type == IDENT_T && src_loc == symtab.end()){
                printf("Line %d, Load: Unknown memory label\n", i + 1);
                exit(1);
            }
            //src
            if(prog[i].src.type == IMM_T){
                //Immediate
                res += bitset<12>(prog[i].src.imm).to_string();
            }else{
                res += bitset<12>(src_loc->second).to_string();
            }
            //dst
            if(prog[i].dst.type != IDENT_T){
                printf("Line %d, Error destination, must be a resigter\n", i + 1);
                exit(1);
            }else{
                int reg = parse_reg_id(prog[i].dst.ident);
                if(reg == -1){
                    printf("Line %d, Error destination register\n", i + 1);
                    exit(1);
                }
                res += bitset<12>(reg).to_string();
            }
            machine_code.push_back(res);
            continue;
        }
        if(prog[i].op.ident == "STR"){
            //27 ~ 24 
            if(prog[i].src.type == IDENT_T){
                res += "0000";
            }else{
                printf("Line %d, Str destination must be a memory location\n", i + 1);
                exit(1);
            }
            //Semantic check
            map<string, int>::iterator dst_loc = symtab.find(prog[i].dst.ident);
            if(prog[i].dst.type == IDENT_T && dst_loc == symtab.end()){
                printf("Line %d, Load: Unknown memory label\n", i + 1);
                exit(1);
            }
            //src
            if(prog[i].src.type != IDENT_T){
                printf("Line %d, Error source, must be a resigter\n", i + 1);
                exit(1);
            }else{
                int reg = parse_reg_id(prog[i].src.ident);
                if(reg == -1){
                    printf("Line %d, Error soruce register\n", i + 1);
                    exit(1);
                }
                res += bitset<12>(reg).to_string();
            }
            //dst
            res += bitset<12>(dst_loc->second).to_string();
            machine_code.push_back(res);
            continue;
        }
        //Store Like load but opposite
        NO_OP:
        res = "Instruction not yet implemented";
        machine_code.push_back(res);
    }
}

void print_machine_code(){
    for(size_t i = 0; i < machine_code.size(); i++){
        printf("%.4d\t", i);
        string s = machine_code[i];
        string res = s.substr(0, 4) + "_" + s.substr(4, 4) + "_" + s.substr(8, 4) + "_" + s.substr(12, 4);
        res += "_" + s.substr(16, 4) + "_" + s.substr(20, 4) + "_" + s.substr(24, 4) + "_" + s.substr(28, 4);
        printf("%s\t\t//", res.c_str());
        print_prog_line(i, 0);
    }
    cout << endl;
    //print label location
    for(auto it: symtab){
        printf("%s: %.4d\n", it.first.c_str(), it.second);
    }
}

bool init_outfile(string s){
    outfile.open(s);
    if(!outfile.is_open()){
        return false;
    }
    return true;
}

void write_out(){
    for(size_t i = 0; i < machine_code.size(); i++){
        outfile << machine_code[i] << endl;
    }
}