INCLUDE =`mariadb_config --include` -I/usr/local/include  -I/usr/local/apr/include -I./
LIBS    =-lhiredis -L/usr/local/dowse/mysql/plugin  -L/usr/local/apr/lib  -lapr-1  -laprutil-1
CFLAGS ?=-Wall -O2 -g -D_LARGEFILE64_SOURCE
LDFLAGS?=-fPIC

compile:
	gcc $(LDFLAGS) $(CFLAGS) -g $(INCLUDE)  -I. -shared -rdynamic lib_mysqludf_redis.c  $(LIBS) -o lib_mysqludf_redis_v2.so

clean:
	rm -f *~
	rm -f *.so
	rm -f *.out
