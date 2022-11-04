#include "lexer.h"
#include "parser.h"
#include "vasm.h"
#include <iostream>
#include <string>

using namespace std;

string infile_name;
string outfile_name;

void parse_args(int argc, char *argv[]){
    if(argc <= 1){
        cout << "Usage: vasm.exe INPUT_FILE (OUTPUT_FILE_NAME) \n";
        exit(1);
    }
    infile_name = argv[1];
    if(argc >= 3)
        outfile_name = argv[2];
    else{
        size_t found = infile_name.find_last_of("/\\");
        string file_name = infile_name.substr(found + 1);
        found = file_name.find_last_of(".");
        file_name = infile_name.substr(0, found);
        outfile_name = file_name + ".mem";
    }
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);
    cout << "Input file name: " << infile_name << endl;
    cout << "Output file name: " << outfile_name << endl;
    if(file_init(infile_name))
        cout << "---Read file success---\n";
    else {
        cout << "Open input file failed\n";
        exit(1);
    }
    if(lex())
        cout << "---Lex success---\n";
    print_token();
    //parse
    cout << "\n---Start parsing---\n";
    parse();
    cout << "Parser returned" <<endl;
    print_prog();
    cout << "\n---Start translating---\n";
    asm_to_machine();
    print_machine_code();
    if(!init_outfile(outfile_name)){
        cout << "Output file error\n";
    }
    write_out();
    return 0;
}