DESTDIR ?=
PREFIX ?= /usr/local/dowse

CC = gcc
CFLAGS = -Wall -fPIC -Os
LIBS = -ljemalloc -lhiredis

all: base springs
	@./import.sh redis-server
	@./import.sh redis-cli
	@./import.sh mariadb
	@install -s -p modprobe        ../build/bin
	@install -s -p dowse-to-gource ../build/bin
	@install -s -p dowse-to-osc    ../build/bin
	@install -s -p dowse-to-mqtt   ../build/bin
	@./checksum.sh
	@./compile.sh sup

base: config modprobe sup
	@make -C libparse-datetime
	@./compile.sh log
	@./compile.sh dnscrypt-proxy
	@./compile.sh dnscrypt_dowse.so
	@./compile.sh pgld
	@./compile.sh netdata
	@./compile.sh dhcpd
	@./compile.sh libwebsockets
	@./compile.sh mosquitto
	@./compile.sh kore
	@./compile.sh webui
	@./compile.sh seccrond

springs: dowse-to-gource dowse-to-osc dowse-to-mqtt
	@echo "Dowse springs compiled"

config:
	@./config.sh ${DESTDIR}${PREFIX}

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

modprobe: modprobe.o kmod_log.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -lkmod

dowse-to-gource: dowse-to-gource.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dowse-to-osc: dowse-to-osc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -llo

dowse-to-mqtt: dowse-to-mqtt.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) mosquitto/lib/libmosquitto.a -lpthread

install:
	install -d ${DESTDIR}${PREFIX}
	install -d ${DESTDIR}${PREFIX}/domain-list/data
	install -p -m 644 domain-list/data/* ${DESTDIR}${PREFIX}/domain-list/data
	install -d ${DESTDIR}${PREFIX}/share
	install dnscrypt-proxy/dnscrypt-resolvers.csv ${DESTDIR}${PREFIX}/share/
	install -d ${DESTDIR}${PREFIX}/webui
	cp -rav webui/* ${DESTDIR}${PREFIX}/webui/
	make -C netdata install
#	@./compile.sh install

clean:
	@./compile.sh clean
	rm -f *.o
	rm -f *.zkv
	rm -f database.h execrules.h
	rm -f modprobe dowse-to-osc dowse-to-gource
	make -C dhcp                 clean
	make -C sup                  clean
	make -C dnscrypt-proxy       clean
	make -C dnscrypt-plugin      clean
	make -C pgl                  clean
	make -C netdata              clean
	make -C kore				 clean
	make -C libwebsockets	     clean
	make -C mosquitto            clean
	make -C seccrond			 clean


