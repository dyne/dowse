#CC=colorgcc
CC = gcc
CFLAGS = -Wall -Os -fPIC -I../../../
LIBS = -ljemalloc -lhiredis
all: dowse.so

dowse.so: hashmap.o dowse.o
	$(CC) -fPIC -shared -o $@ dowse.o hashmap.o ../../../epoch.o $(LIBS)

clean:
	rm -f *o

dowse-debug.so: CFLAGS = -ggdb -fPIC -Wall -I../../../
dowse-debug.so: hashmap.o dowse.o
	$(CC) -ggdb -fPIC -shared -o $@ dowse.o hashmap.o $(LIBS)
