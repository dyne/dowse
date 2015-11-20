#CC=colorgcc
CC=gcc
CFLAGS=-Wall -O3 -fPIC

all: dowse.so

dowse.so: hashmap.o dowse.o
	$(CC) -shared -o $@ dowse.o hashmap.o

clean:
	rm -f *o
