#ifndef PARSER_H
#define PARSER_H

#include <string>

void parse();
void print_prog();
void asm_to_machine();
void print_machine_code();
bool init_outfile(std::string s);
void write_out();
#endif