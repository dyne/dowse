CFLAGS ?= -ggdb -Wall -DDEBUG=1
CC = gcc

all: libs start-stop-daemon supervise-daemon

libs:
	make -C librc
	make -C libeinfo

start-stop-daemon: start-stop-daemon.o _usage.o rc-misc.o
	$(CC) $(CFLAGS) -o start-stop-daemon start-stop-daemon.o _usage.o rc-misc.o librc/librc.a libeinfo/libeinfo.a

supervise-daemon: supervise-daemon.o _usage.o rc-misc.o
	$(CC) $(CFLAGS) -o supervise-daemon supervise-daemon.o _usage.o rc-misc.o librc/librc.a libeinfo/libeinfo.a

.c.o:
	$(CC) $(CFLAGS) -Iincludes -Ilibeinfo -Ilibrc -c $< -o $@

clean:
	make -C librc clean
	make -C libeinfo clean
	rm -f *.o
	rm -f supervise-daemon
	rm -f start-stop-daemon
