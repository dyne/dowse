CC = gcc
CFLAGS = -Wall -fPIC -Os -O2
LIBS = -ljemalloc -lhiredis

all: dowse-to-gource dowse-to-osc

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

dowse-to-gource: dowse-to-gource.o redis.o epoch.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dowse-to-osc: dowse-to-osc.o redis.o epoch.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -llo

clean:
	rm -f *.o
	rm -f dowse-to-osc dowse-to-gource
