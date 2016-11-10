CFLAGS ?= -ggdb -Wall -DDEBUG=1
CC = gcc
AR = ar

all: librc.o librc-daemon.o librc-depend.o librc-misc.o librc-stringlist.o
	${AR} rcs librc.a librc.o librc-daemon.o librc-depend.o librc-misc.o librc-stringlist.o

.c.o:
	$(CC) $(CFLAGS) -I../includes -c $< -o $@

clean:
	rm -f *.o
	rm -f librc.a

# Massage our header file for our dirs
# SED_CMD=	-e 's:@PREFIX@:${PREFIX}:g'
# SED_CMD+=	-e 's:@LIB@:${LIBNAME}:g'
# SED_CMD+=	-e 's:@SYSCONFDIR@:${SYSCONFDIR}:g'
# SED_CMD+=	-e 's:@LIBEXECDIR@:${LIBEXECDIR}:g'
# SED_CMD+=	-e 's:@BINDIR@:${BINDIR}:g'
# SED_CMD+=	-e 's:@SBINDIR@:${SBINDIR}:g'
# _PKG_PREFIX=	-e 's:@PKG_PREFIX@:${PKG_PREFIX}:g'
# _LCL_PREFIX=	-e 's:@LOCAL_PREFIX@:${LOCAL_PREFIX}:g'
