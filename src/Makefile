DESTDIR ?=
PREFIX ?= /usr/local/dowse
THREADS ?= 1

ifeq ($(wildcard /usr/bin/mariadb_config),)
  DB_CFG=/usr/bin/mysql_config
  DB_TYPE=mysqldb
else
  DB_CFG=/usr/bin/mariadb_config
  DB_TYPE=mariadb
endif

CC = gcc
CFLAGS = -Wall -fPIE -fPIC -pie -Os -I.
LIBS = -ljemalloc -lhiredis -Llibdowse -l:libdowse.a -lb64 -Llibb64

all: base parsers springs
	./import.sh redis-server
	./import.sh redis-cli
	./import.sh ${DB_TYPE}
	./import.sh nmap-macs
	./import.sh node-red
	install -s -p modprobe        ../build/bin
	install -s -p dowse-to-gource ../build/bin
	install -s -p dowse-to-osc    ../build/bin
	install -s -p dowse-to-mqtt   ../build/bin
	install -s -p hiredis-lock    ../build/bin
	install -s -p dowse-cmd-fifo  ../build/bin
	install -s -p parse-ip-neigh  ../build/bin
	./checksum.sh
	./compile.sh sup

libs: b64
	make -C libdowse
	make -C libparse-datetime

.PHONY: dnscrypt-proxy kore libwebsockets modprobe netdata seccrond webui hiredis-lock libparse-datetime maria2redis mosquitto pgld sup

base: config libs modprobe sup hiredis-lock libparse-datetime dnscrypt-proxy dnscrypt_dowse.so pgld netdata dhcpd libwebsockets mosquitto kore webui seccrond maria2redis
	@echo "Compiled base"

b64:
	@make -C libb64

libparse-datetime:
	@make -C libparse-datetime -j${THREADS}

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

kore:
	@./compile.sh kore ${THREADS}

webui: kore
	@./compile.sh webui ${THREADS}

seccrond:
	@./compile.sh seccrond ${THREADS}

maria2redis:
	@./compile.sh maria2redis ${THREADS}


springs: dowse-to-gource dowse-to-osc dowse-to-mqtt dowse-cmd-fifo
	@echo "Dowse springs compiled"

parsers: parse-ip-neigh
	@echo "Dowse parsers compiled"

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

parse-ip-neigh: parse-ip-neigh.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -DDB_HOST=\"localhost\" -DDB_USER=\"root\" -DDB_PASSWORD=\"p4ssw0rd\" -DDB_SID=\"things\" -DDB_SOCK_DIRECTORY=\"$(HOME)/.dowse/run/mysql/mysqld.sock\"  `${DB_CFG} --include` `${DB_CFG} --libs`

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
	install -d ${DESTDIR}${PREFIX}/webui
	cp -rav webui/* ${DESTDIR}${PREFIX}/webui/
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
	make -C kore				 clean
	make -C libwebsockets	     clean
	make -C mosquitto            clean
	make -C seccrond			 clean
	make -C libdowse			 clean
	make -C libparse-datetime    clean
