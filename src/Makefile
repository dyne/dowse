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
	./config.sh

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

dowse-to-gource: dowse-to-gource.o redis.o epoch.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

dowse-to-osc: dowse-to-osc.o redis.o epoch.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -llo

dowse-dnscap-plugin:
	make -C dnscap/plugins/dowse

clean:
	./compile.sh clean
	rm -f *.o
	rm -f *.zkv
	rm -f database.h execrules.h
	rm -f dowse-to-osc dowse-to-gource
	rm -f dnscap/plugins/dowse/*.o
	make -C sup clean
