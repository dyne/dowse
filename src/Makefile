DESTDIR ?=
PREFIX ?= /usr/local/dowse
THREADS ?= 1

CC = gcc
CFLAGS = -Wall -fPIE -fPIC -pie -O3 -I. -Ihiredis
LIBS = -L src/redis/deps/hiredis -l:libhiredis.a -Llibdowse -l:libdowse.a

all: base springs
	./import.sh nmap-macs
	install -s -p modprobe        ../build/bin
	install -s -p dowse-to-gource ../build/bin
	install -s -p dowse-to-osc    ../build/bin
	install -s -p dowse-to-mqtt   ../build/bin
	install -s -p dowse-cmd-fifo  ../build/bin
	cp -f parse-ip-neigh.py  ../build/bin
	./checksum.sh $(DOWSE_USER)

libs:
	make -C libdowse

.PHONY: modprobe seccrond pgld

base: config libs modprobe dnscrypt_dowse.so seccrond
	@echo "Compiled base"

dnscrypt_dowse.so:
	make -C dnscrypt-plugin

pgld:
	@./compile.sh pgld ${THREADS}

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

modprobe: modprobe.o kmod_log.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -lkmod

dowse-to-gource: dowse-to-gource.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dowse-to-osc: dowse-to-osc.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -llo

dowse-to-mqtt: dowse-to-mqtt.o
	$(CC) $(CFLAGS) -o $@ dowse-to-mqtt.o $(LIBS) -lmosquitto -lpthread


install:
	install -d ${DESTDIR}${PREFIX}
	install -d ${DESTDIR}${PREFIX}/domain-list/data
	install -p -m 644 domain-list/data/* ${DESTDIR}${PREFIX}/domain-list/data
	install -d ${DESTDIR}${PREFIX}/share
	mkdir -p /usr/lib/netdata/plugins.d
	make -C netdata-plugins install

#	@./compile.sh install

clean:
	@./compile.sh clean
	rm -f *.o
	rm -f *.zkv
	rm -f database.h execrules.h
	rm -f modprobe dowse-to-osc dowse-to-gource dowse-to-mqtt
	make -C dnscrypt-plugin      clean
	make -C pgld                 clean
	make -C netdata-plugins      clean
	make -C seccrond			 clean
	make -C libdowse			 clean
