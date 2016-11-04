CFLAGS ?= -ggdb -Wall -I.
CC = gcc
AR = ar
BISON = bison
FLEX = flex

all: parser.tab.o parser.lex.o
	${AR} rcs libmustache.a parser.tab.o parser.lex.o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

parser.lex.c:
	${FLEX}  -Pmustache_p_ --bison-bridge -o parser.lex.c parser.l

parser.tab.c: parser.lex.c
	${BISON} -p mustache_p_ -d -b parser parser.y

clean:
	rm -f *.o
	rm -f parser.tab.c parser.tab.h parser.lex.c
	rm -f libmustache.a
