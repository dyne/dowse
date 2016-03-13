CC = gcc
CFLAGS = -Wall -O3

all: dns-to-gource dns-to-osc

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

dns-to-gource: dns-to-gource.o redis.o
	$(CC) $(CFLAGS) -o $@ $^ -lhiredis

dns-to-osc: dns-to-osc.o redis.o
	$(CC) $(CFLAGS) -o $@ $^ -lhiredis -llo

clean:
	rm -f *.o
