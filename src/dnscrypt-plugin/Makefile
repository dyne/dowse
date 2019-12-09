CC := gcc
CFLAGS := -O3 -fstack-protector-all -DPLUGINS=1 -D_FORTIFY_SOURCE=2 -fno-strict-overflow -Wall -Wextra -I.. -I.
LDADD := -lhiredis -L../libdowse -l:libdowse.a -lldns -lsodium -ldl -lm -lpsl -Wl,-soname -Wl,dnscrypt_dowse.so
SOURCES := dnscrypt-dowse.o domainlist.o ip2mac.o
LDFLAGS := -shared  -fPIC -DPIC

PSL_LIST=https://publicsuffix.org/list/public_suffix_list.dat

.c.o:
	$(CC) $(CFLAGS) -c $< -fPIC -DPIC -o $@

all: ${SOURCES}
	${CC} ${LDFLAGS} ${SOURCES} ${LDADD} -o dnscrypt_dowse.so
	wget -nc ${PSL_LIST}


main: main.o
	${CC} main.o -o test -ldl -ggdb -Og

clean:
	rm -f *.o *.so
