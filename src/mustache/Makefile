CFLAGS = -ggdb -Wall -I.
CC = gcc
AR = ar
BISON = bison
FLEX = flex

all: parser.tab.o parser.lex.o
	${AR} rcs libmustache.a parser.tab.o parser.lex.o

parser.tab.o: parser.tab.c
	${CC} ${CFLAGS} -c parser.tab.c

parser.lex.o: parser.lex.c
	${CC} ${CFLAGS} -c parser.lex.c

parser.lex.c:
	${FLEX}  -Pmustache_p_ --bison-bridge -o parser.lex.c parser.l

parser.tab.c: parser.lex.c
	${BISON} -p mustache_p_ -d -b parser parser.y

clean:
	rm -f *.o
	rm -f parser.tab.c parser.tab.h parser.lex.c
	rm -f libmustache.a
