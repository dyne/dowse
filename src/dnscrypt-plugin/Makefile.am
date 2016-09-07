ACLOCAL_AMFLAGS = -I m4

pkglib_LTLIBRARIES = dnscrypt_dowse.la

dnscrypt_dowse_la_LIBTOOLFLAGS = --tag=disable-static

dnscrypt_dowse_la_SOURCES = dnscrypt-dowse.c

dnscrypt_dowse_la_LIBADD = @LDNS_LIBS@

dnscrypt_dowse_la_CFLAGS = -I../dnscrypt-proxy/src/include

dnscrypt_dowse_la_LDFLAGS = $(AM_LDFLAGS) \
-avoid-version \
-export-dynamic \
-module \
-no-undefined

dnscrypt_dowse_la_CPPFLAGS = $(LTDLINCL) -I../../include
