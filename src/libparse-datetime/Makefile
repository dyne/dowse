CC = gcc
CFLAGS = -Wall -fPIC -Os -I.

all: libparse-datetime.a

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

libparse-datetime.a: parse-datetime.o xmalloc.o xalloc-die.o exitfail.o gettime.o
	ar rcs $@ $^

clean:
	rm -f *.o
	rm -f *.a
