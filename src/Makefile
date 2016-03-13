CC = gcc
CFLAGS = -Wall -O3

all: dns-to-gource

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

dns-to-gource: dns-to-gource.o redis.o
	$(CC) $(CFLAGS) -o $@ $^ -lhiredis

clean:
	rm -f *.o
