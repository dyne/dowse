
build:
	echo "#define db_dynamic 0\n#define db_runtime 1\n#define db_storage 2" \
    | tee ../src/libdowse/database.h | tee ../src/dnscrypt-plugin/database.h
	cd ../src/libdowse && make
	cd ../src/dnscrypt-plugin && autoreconf -i && ./configure && make
	cp ../src/dnscrypt-plugin/.libs/dnscrypt_dowse.so .
	gcc -o dowse-to-gource -I ../src -I ../src/libdowse -I /usr/include/hiredis ../src/dowse-to-gource.c -lhiredis -L../src/libdowse -l:libdowse.a

deps:
	apt-get install -q -y git gcc g++ libltdl-dev libsodium-dev libldns-dev \
		libjemalloc-dev libhiredis-dev

#	$(wildcard ../src/dnscrypt-proxy/README.md,,$(shell git clone https://github.com/dyne/dnscrypt-proxy ../src/dnscrypt-proxy)).md
#	cd dnscrypt-proxy && autoreconf -i --force && ./configure && make

#	$(wildcard dhcp,,$(shell git clone https://github.com/isc-projects/dhcp))
#	cd dhcp && autoreconf -i \
	&& ./configure --enable-paranoia --enable-execute --disable-dhcpv6 \
        && make
