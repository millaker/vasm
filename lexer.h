#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include "vasm.h"

bool file_init(std::string s);
bool lex();
void print_token();
std::vector<Token> get_token_list();

#endif