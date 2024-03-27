ACLOCAL_AMFLAGS = -I m4

pkglib_LTLIBRARIES = dnscrypt_dowse.la

dnscrypt_dowse_la_LIBTOOLFLAGS = --tag=disable-static

dnscrypt_dowse_la_SOURCES = dnscrypt-dowse.c domainlist.c ip2mac.c

dnscrypt_dowse_la_LIBADD = @LDNS_LIBS@ -ljemalloc -lhiredis -L../libdowse -l:libdowse.a

dnscrypt_dowse_la_CFLAGS = -O3 -Wall -I../dnscrypt-proxy/src/include -I../redis/deps -I../redis/deps/hiredis -I..

dnscrypt_dowse_la_LDFLAGS = $(AM_LDFLAGS) \
-avoid-version \
-export-dynamic \
-module \
-no-undefined

dnscrypt_dowse_la_CPPFLAGS = $(LTDLINCL) -I../../include
