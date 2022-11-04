#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctype.h>
#include "vasm.h"

using namespace std;

static ifstream infile;
vector<Token> token_list;

vector<Token> get_token_list() {
    return token_list;
}

/*
    Init input file for reading
*/
bool file_init(string s) {
    infile.open(s);
    if(!infile.is_open())
        return false;
    return true;
}

/*
    Function for checking variable characters
*/
inline bool lex_var_check(char s){
    if((s >= 'A' && s <= 'Z') || isdigit(s))
        return true;
    return false;
}

/*
    Lexer will lex the entire file and store sequence of tokens in token list
*/
bool lex(){
    while(infile.peek() != EOF){
        Token t;
        //Skip spaces
        while(infile.peek() == ' ')
            infile.get();
        //PUNCT
        if(infile.peek() == ','){
            infile.get();
            t.type = COMMA_T;
            token_list.push_back(t);
            continue;
        }
        if(infile.peek() == ':'){
            infile.get();
            t.type = COLON_T;
            token_list.push_back(t);
            continue;
        }
        if(infile.peek() == '\n'){
            infile.get();
            t.type = NEWLINE_T;
            token_list.push_back(t);
            continue;
        }
        //IMM
        if(infile.peek() == '#'){
            infile.get();
            int num = 0;
            while(isdigit(infile.peek())){
                num *= 10;
                num += infile.get() - '0';
            }
            t.type = IMM_T;
            t.imm = num;
            token_list.push_back(t);
            continue;
        }
        //Hex immediate will start with 0~9 a~e
        if(isdigit(infile.peek())
           || (infile.peek() >= 'a'
           && infile.peek() <= 'f')){
            string s;
            while(isdigit(infile.peek()) ||
                  (infile.peek() >= 'a' && 
                  infile.peek() <= 'f')){
                    s += infile.get();
                  }
            t.type = HEX_T;
            t.hex = s;
            token_list.push_back(t);
            continue;
        }
        //Variable will be all capital letters
        //No variables start with a digit will go through previous checks
        if(lex_var_check(infile.peek())){
            string s;
            while(lex_var_check(infile.peek())){
                s += infile.get();
            }
            t.type = IDENT_T;
            t.ident = s;
            token_list.push_back(t);
            continue;
        }
        //Error
        cout << "Error lexing, unknown character(" << (char)infile.peek() << ")\n";
        return false;
    }
    Token t;
    t.type = EOF_T;
    token_list.push_back(t);
    return true;
}

//Debugging
void print_token(){
    cout << "Token: " << endl;
    for(auto it = token_list.begin(); it != token_list.end(); it++){
        switch(it->type){
            case COMMA_T:
                cout << "(,) ";
                break;
            case NEWLINE_T:
                cout << endl;
                break;
            case COLON_T:
                cout << "(:) ";
                break;
            case IMM_T:
                cout << "IMM(" << it->imm << ") " ;
                break;
            case HEX_T:
                cout << "IMM_H(" << it->hex << ") ";
                break;
            case IDENT_T:
                cout << "ID(" << it->ident << ") ";
                break;
            case EOF_T:
                cout << "EOF " ;
                break;
            default:
                cout << "Error token ";
        }
    }
}