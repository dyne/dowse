CC = gcc
CFLAGS = -Wall -Os -O2

all: dowse-to-gource dowse-to-osc

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

dowse-to-gource: dowse-to-gource.o redis.o epoch.o
	$(CC) $(CFLAGS) -o $@ $^ -lhiredis

dowse-to-osc: dowse-to-osc.o redis.o epoch.o
	$(CC) $(CFLAGS) -o $@ $^ -lhiredis -llo

clean:
	rm -f *.o
	rm -f dowse-to-osc dowse-to-gource
