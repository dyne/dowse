CFLAGS ?= -ggdb -Wall  -fPIE -fPIC -pie
#-DDEBUG=1
CC = gcc
AR = ar
OBJS = log.o redis.o hashmap.o parsetime.o find_nearest_asset.o

all: libdowse.a

libdowse.a: ${OBJS}
	${AR} rcs libdowse.a ${OBJS}

.c.o:
	$(CC) $(CFLAGS) -I. -I.. -I../libb64/include -c $< -o $@

clean:
	rm -f *.o
	rm -f libdowse.a
