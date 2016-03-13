#CC=colorgcc
CC=gcc
CFLAGS=-O3 -fPIC -I../../../

all: dowse.so

dowse.so: hashmap.o dowse.o
	$(CC) -fPIC -shared -o $@ dowse.o hashmap.o -lhiredis

clean:
	rm -f *o

dowse-debug.so: CFLAGS = -ggdb -fPIC -Wall -I../../../
dowse-debug.so: hashmap.o dowse.o
	$(CC) -ggdb -fPIC -shared -o $@ dowse.o hashmap.o -lhiredis
