DESTDIR ?=
PREFIX ?= /usr/local/dowse

CC = gcc
CFLAGS = -Wall -fPIE -fPIC -pie -Os -I.
LIBS = -ljemalloc -lhiredis -Llibdowse -l:libdowse.a

all: base parsers springs
	@./import.sh redis-server
	@./import.sh redis-cli
	@./import.sh mariadb
	@install -s -p modprobe        ../build/bin
	@install -s -p dowse-to-gource ../build/bin
	@install -s -p dowse-to-osc    ../build/bin
	@install -s -p dowse-to-mqtt   ../build/bin
	@install -s -p hiredis-lock    ../build/bin
	@install -s -p dowse-cmd-fifo  ../build/bin
	@install -s -p parse-ip-neigh  ../build/bin
	@./checksum.sh
	@./compile.sh sup

libs:
	make -C libdowse
	make -C libparse-datetime

base: config libs modprobe sup hiredis-lock
	@make -C libparse-datetime
	@./compile.sh dnscrypt-proxy
	@./compile.sh dnscrypt_dowse.so
	@./compile.sh pgld
	@./compile.sh netdata
	@./compile.sh netdata-plugins
	@./compile.sh dhcpd
	@./compile.sh libwebsockets
	@./compile.sh mosquitto
	@./compile.sh kore
	@./compile.sh webui
	@./compile.sh seccrond
	@./compile.sh maria2redis

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
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -DDB_HOST=\"localhost\" -DDB_USER=\"root\" -DDB_PASSWORD=\"p4ssw0rd\" -DDB_SID=\"things\" -DDB_SOCK_DIRECTORY=\"$(HOME)/.dowse/run/mysql/mysqld.sock\"  -I/usr/include/mariadb -lmariadb

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
