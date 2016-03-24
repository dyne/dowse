DESTDIR ?=
PREFIX ?= /usr/local/dowse

CC = gcc
CFLAGS = -Wall -fPIC -Os -O2
LIBS = -ljemalloc -lhiredis

all: config dowse-to-gource dowse-to-osc dowse-dnscap-plugin sup
	@./compile.sh dnscrypt-proxy
	@./compile.sh pgl
	@./compile.sh dnscap
	@./checksum.sh
	@./compile.sh sup

config:
	@./config.sh ${DESTDIR}${PREFIX}

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

dowse-to-gource: dowse-to-gource.o redis.o epoch.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dowse-to-osc: dowse-to-osc.o redis.o epoch.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -llo

dowse-dnscap-plugin:
	make -C dnscap/plugins/dowse

pgl-install: PREFIX=${DESTDIR}${PREFIX}/pgl
pgl-install:
	make -C pgl install

# here sup is installed with suid bit. sup is a secure application we
# use for privilege escalation when needed. sup executes only certain
# binaries on a fixed path and they must match a sha256 hash which is
# compiled in. for more information see: https://github.com/dyne/sup
# to see what binaries are compiled in, do `sup -l`
install: all pgl-install
	install -d ${DESTDIR}${PREFIX} ${DESTDIR}${PREFIX}/bin
	install -s -p -m 6755 sup/sup       ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755  dnscap/dnscap ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755  dnscap/plugins/dowse/dowse.so ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755  dnscrypt-proxy/src/proxy/dnscrypt-proxy ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755  dowse-to-osc    ${DESTDIR}${PREFIX}/bin
	install -s -p -m 755  dowse-to-gource ${DESTDIR}${PREFIX}/bin


#	@./compile.sh install

clean:
	@./compile.sh clean
	rm -f *.o
	rm -r *~
	rm -f *.zkv
	rm -f database.h execrules.h
	rm -f dowse-to-osc dowse-to-gource
	rm -f dnscap/plugins/dowse/*.o
	make -C sup clean
