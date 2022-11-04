CC = g++
CFLAGS = -Wall -g
OBJECTS = lexer.o vasm.o parser.o
OUTFILE = vasm

all: $(OUTFILE)

$(OUTFILE) : $(OBJECTS)
	@printf "[CC]\t$@\n"
	@$(CC) $(CFLAGS) -o $@ $^

%.o : %.cpp
	@printf "[CC]\t$@\n"
	@$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf *.o *.exe