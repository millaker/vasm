# vasm, A simple assembler for customized risc ISA

This assembler was written specifically for VLSI system design course at NCKUEE.
The complete specification of the customize instruction set is shown in `instruction_set_spec.md`

## Compile
The given makefile is compiled under Windows using cmder. Using `make` to compile all the files.
If `make` is not installed, a simple command can do the trick.
```> g++ lexer.cpp parser.cpp vasm.cpp -o vasm```
Note that g++ is required.

## Usage

```> vasm INPUT_FILE_NAME [OUTPUT_FILE_NAME]```

If `"OUTPUT_FILE_NAME"` is not specified, the output file name is set to default, which is `"INPUT_FILE_NAME_WITHOUT_EXTENSION.mem"`

## Warning
Although the assembler is able to translate correct assembly codes, some error-formed codes may not be caught by the assembler. I tried to include all the errors that might occur which I can think of. Feel free to report errors in the assembler.

## Adding and Removing instructions
Currently working on this.