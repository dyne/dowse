DESTDIR ?=
PREFIX ?= /usr/local/dowse
THREADS ?= 1

CC = gcc
CFLAGS = -Wall -fPIE -fPIC -pie -Os -I.
LIBS = -ljemalloc -lhiredis -Llibdowse -l:libdowse.a -lb64 -Llibb64

all: base springs
	./import.sh redis-server
	./import.sh redis-cli
	./import.sh nmap-macs
	#./import.sh node-red
	install -s -p modprobe        ../build/bin
	install -s -p dowse-to-gource ../build/bin
	install -s -p dowse-to-osc    ../build/bin
	install -s -p dowse-to-mqtt   ../build/bin
	install -s -p hiredis-lock    ../build/bin
	install -s -p dowse-cmd-fifo  ../build/bin
	install -p parse-ip-neigh.py  ../build/bin
	./checksum.sh
	./compile.sh sup

libs: b64
	make -C libdowse

.PHONY: dnscrypt-proxy libwebsockets modprobe netdata seccrond hiredis-lock mosquitto pgld sup

base: config libs modprobe sup hiredis-lock dnscrypt-proxy dnscrypt_dowse.so pgld netdata dhcpd libwebsockets mosquitto seccrond
	@echo "Compiled base"

b64:
	@make -C libb64

dnscrypt-proxy:
	@./compile.sh dnscrypt-proxy ${THREADS}

dnscrypt_dowse.so:
	@./compile.sh dnscrypt_dowse.so ${THREADS}

pgld:
	@./compile.sh pgld ${THREADS}

netdata:
	@rm -f netdata/web/version.txt
	@./compile.sh netdata ${THREADS}
	@./compile.sh netdata-plugins ${THREADS}

dhcpd:
	@./compile.sh dhcpd ${THREADS}

libwebsockets:
	@./compile.sh libwebsockets ${THREADS}

mosquitto: libwebsockets
	@./compile.sh mosquitto ${THREADS}

seccrond:
	@./compile.sh seccrond ${THREADS}

springs: dowse-to-gource dowse-to-osc dowse-to-mqtt dowse-cmd-fifo
	@echo "Dowse springs compiled"

config:
	@./config.sh ${DESTDIR}${PREFIX}

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

dowse-log: dowse-log.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dowse-cmd-fifo: dowse-cmd-fifo.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

hiredis-lock: hiredis-lock.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

modprobe: modprobe.o kmod_log.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -lkmod

dowse-to-gource: dowse-to-gource.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dowse-to-osc: dowse-to-osc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -llo

dowse-to-mqtt: mosquitto dowse-to-mqtt.o
	$(CC) $(CFLAGS) -o $@ dowse-to-mqtt.o $(LIBS) -L./mosquitto/lib/ -lmosquitto    -lpthread


install:
	install -d ${DESTDIR}${PREFIX}
	install -d ${DESTDIR}${PREFIX}/domain-list/data
	install -p -m 644 domain-list/data/* ${DESTDIR}${PREFIX}/domain-list/data
	install -d ${DESTDIR}${PREFIX}/share
	install dnscrypt-proxy/dnscrypt-resolvers.csv ${DESTDIR}${PREFIX}/share/
	make -C netdata install
	make -C netdata-plugins install
#	@./compile.sh install

clean:
	@./compile.sh clean
	rm -f *.o
	rm -f *.zkv
	rm -f database.h execrules.h
	rm -f modprobe dowse-to-osc dowse-to-gource dowse-to-mqtt
	rm -f hiredis-lock
	rm -f netdata/web/version.txt
	make -C dhcp                 clean
	make -C sup                  clean
	make -C dnscrypt-proxy       clean
	make -C dnscrypt-plugin      clean
	make -C pgld                 clean
	make -C netdata              clean
	make -C netdata-plugins      clean
	make -C libwebsockets	     clean
	make -C mosquitto            clean
	make -C seccrond			 clean
	make -C libdowse			 clean
