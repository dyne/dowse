
ifeq ($(wildcard /usr/bin/mariadb_config),) 
  DB_CFG=/usr/bin/mysql_config
else
  DB_CFG=/usr/bin/mariadb_config
endif
INCLUDE =`${DB_CFG} --include` -I/usr/local/include `apr-1-config --includes` -I./
LIBS    =-lhiredis -L/usr/local/dowse/mysql/plugin  -L/usr/local/apr/lib  -lapr-1  -laprutil-1

#	gcc $(LDFLAGS) $(CFLAGS) -g $(INCLUDE)  -I. -shared -rdynamic lib_mysqludf_redis.c  $(LIBS) -o lib_mysqludf_redis_v2.so

compile: lib_mysqludf_redis.o
	gcc  -shared -rdynamic lib_mysqludf_redis.o -o lib_mysqludf_redis_v2.so $(LDFLAGS) $(LIBS)


lib_mysqludf_redis.o:lib_mysqludf_redis.c
	gcc -c $(LDFLAGS) $(CFLAGS) -g $(INCLUDE) -I. lib_mysqludf_redis.c -o lib_mysqludf_redis.o

clean:
	rm -f *~
	rm -f *.so
	rm -f *.out
	rm -f *.o
