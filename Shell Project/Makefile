CC = gcc
CFLAGS = -ansi -Wall -g -O0 -Wwrite-strings -Wshadow \
         -pedantic-errors -fstack-protector-all 
PROGS = lexer.o parser.tab.o executor.o d8sh.o d8sh

all: $(PROGS)

clean:
	rm -f *.o $(PROGS) *.tmp

lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

parser.tab.o: parser.tab.c command.h
	$(CC) $(CFLAGS) -c parser.tab.c

executor.o: executor.c command.h executor.h
	$(CC) $(CFLAGS) -c executor.c

d8sh.o: d8sh.c executor.h lexer.h
	$(CC) $(CFLAGS) -c d8sh.c

d8sh: lexer.o parser.tab.o executor.o d8sh.o
	$(CC) -lreadline -o d8sh lexer.o parser.tab.o executor.o d8sh.o

	