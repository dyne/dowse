#CC=colorgcc
CC=gcc
CFLAGS=-O3 -fPIC

all: dowse.so

dowse.so: hashmap.o dowse.o
	$(CC) -fPIC -shared -o $@ dowse.o hashmap.o

clean:
	rm -f *o

dowse-debug.so: CFLAGS = -ggdb -fPIC -Wall
dowse-debug.so: hashmap.o dowse.o
	$(CC) -ggdb -fPIC -shared -o $@ dowse.o hashmap.o
