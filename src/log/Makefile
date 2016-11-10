CFLAGS ?= -ggdb -Wall -DDEBUG=1
CC = gcc
AR = ar

all: log.o
	${AR} rcs liblog.a log.o

.c.o:
	$(CC) $(CFLAGS) -I. -I.. -c $< -o $@

clean:
	rm -f *.o
	rm -f liblog.a
