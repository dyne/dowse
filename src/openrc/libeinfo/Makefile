CFLAGS ?= -ggdb -Wall -DDEBUG=1
CC = gcc
AR = ar

all: libeinfo.o
	${AR} rcs libeinfo.a libeinfo.o

.c.o:
	$(CC) $(CFLAGS) -I../includes -c $< -o $@

clean:
	rm -f *.o
	rm -f libeinfo.a
