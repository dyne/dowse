INCLUDE =-I/usr/local/include `apr-1-config --includes` -I./
LIBS    =-lhiredis -L/usr/local/dowse/mysql/plugin  -L/usr/local/apr/lib  -lapr-1  -laprutil-1
CFLAGS ?=-Wall -O2 -g -D_LARGEFILE64_SOURCE
LDFLAGS?=-fPIC

ifeq ($(WITH_MYSQL),yes)
  INCLUDE+=`mysql_config --include`
else
  INCLUDE+=`mariadb_config --include`
endif

compile:
	gcc $(LDFLAGS) $(CFLAGS) -g $(INCLUDE)  -I. -shared -rdynamic lib_mysqludf_redis.c  $(LIBS) -o lib_mysqludf_redis_v2.so

clean:
	rm -f *~
	rm -f *.so
	rm -f *.out
