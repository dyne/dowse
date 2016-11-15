# Makefile to generate dhtest

CC=gcc
#CFLAGS=-Wall -g

all: dhtest
	./runtests

dhtest: dhtest.o functions.o
	$(CC) dhtest.o functions.o -o dhtest

clean:
	rm -f dhtest functions.o dhtest.o
