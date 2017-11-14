CFLAGS ?= -ggdb -Wall  -fPIE -fPIC -pie
#-DDEBUG=1

CC = gcc
AR = ar
OBJS = log.o redis.o hashmap.o parsetime.o

all: libdowse.a

libdowse.a: ${OBJS}
	${AR} rcs libdowse.a ${OBJS}

.c.o:
	$(CC) $(CFLAGS) -I. -I.. -c $< -o $@

clean:
	rm -f *.o
	rm -f libdowse.a
