#ifndef VASM_H
#define VASM_H

#include <string>

//Token def
typedef struct Token{
    int type;
    int imm;
    std::string ident;
    std::string hex;
} Token;

enum {
    EMPTY,
    IMM_T,
    HEX_T, //HEX will be stored in string hex
    IDENT_T, //labels and operations
    COLON_T,
    COMMA_T,
    NEWLINE_T,
    EOF_T
};

//Inst_node def
typedef struct Inst_node{
    Token label;
    Token op;
    Token src;
    Token dst;
} Inst_node;
#endif