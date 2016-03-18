CC?=gcc
LDD?=ld
DESTDIR?=
PREFIX?=/usr/local
VERSION=1.1
USER=root
GROUP=root

CFLAGS?=-Os -O2

all: shared

shared: CFLAGS+=-fPIC -fPIE -Wall
shared: LDFLAGS=-fPIC -fPIE -pie
shared: config.h sup.o sha256.o
	${CC} ${LDFLAGS} sup.o sha256.o -o sup

musl: musl=/usr/local/musl
musl: CC=${musl}/bin/musl-gcc
musl: CFLAGS+=-I${musl}/include
musl: LDFLAGS+=-static ${musl}/lib/libc.a
musl: config.h sup.o sha256.o
	${CC} ${LDFLAGS} sup.o sha256.o -o sup

test: CC=colorgcc
test: CFLAGS+=--std=gnu99
test: LDFLAGS=-lcrypto -lm
test: config.h test.o sha256.o
	${CC} ${LDFLAGS} test.o sha256.o -o test

debug: CFLAGS+=-ggdb
debug: sup.o
	${CC} ${LDFLAGS} sup.o sha256.o -o sup

config.h:
	cp config.def.h config.h

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@ -DVERSION=${VERSION}

clean:
	rm -f *.o sup test

mrproper: clean
	rm -f config.h

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f sup ${DESTDIR}${PREFIX}/bin
	-chown ${USER}:${GROUP} ${DESTDIR}/${PREFIX}/bin/sup
	-chmod 4111 ${DESTDIR}${PREFIX}/bin/sup
	mkdir -p ${DESTDIR}${PREFIX}/share/man/man1
	sed s,VERSION,${VERSION}, sup.1 \
	  > ${DESTDIR}${PREFIX}/share/man/man1/sup.1
