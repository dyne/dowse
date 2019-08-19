#!/usr/bin/env zsh

R=`pwd`
R=${R%/*}
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

source $R/zuper/zuper
source $R/zuper/zuper.init

PREFIX=${PREFIX:-/usr/local/dowse}
DOWSE_HOME=${DOWSE_HOME:-$HOME}
CFLAGS="-Wall -fPIC -fPIE -O3"
LDFLAGS="-fPIC -fPIE -pie"

# 2nd argument when present is number of threads used building
THREADS=${2:-1}

[[ -x $R/build/bin/$1 ]] && {
    act "$1 found in $R/build/bin/$1"
    act "delete it from build/bin to force recompilation"
    return 0 }

notice "Compiling $1 using ${THREADS} threads"



case $1 in
	redis)
		[[ -r $R/src/redis/src/redis-server ]] && return 0
		pushd $R/src/redis
		git checkout -- . && \
		patch -NEp1 < $R/src/patches/redis_nodebug.patch
		echo "MALLOC=jemalloc" > src/.make-settings
		make -j${THREADS} V=1
		popd
		;;

	log)
		[[ -r $R/src/log/liblog.a ]] && return 0
		pushd $R/src/log
		CFLAGS="$CFLAGS" \
			  LDFLAGS="$LDFLAGS" \
			  make -j${THREADS}
		popd
		;;

	dhcpd)
		pushd $R/src/dhcp
		act "please wait while preparing the build environment"
		act "also prepare to wait more for the BIND export libs"
		act "when you see ISC_LOG_ROLLINFINITE then is almost there"
		autoreconf -i
		# dhcpd fails building with multiple threads (still?)
		CFLAGS="-Wall -Os" LDFLAGS="" \
		./configure --enable-paranoia --enable-execute \
			--disable-dhcpv6 &&
		    make && {
			install -s -p server/dhcpd    $R/build/bin &&
			    install -s -p dhcpctl/omshell $R/build/bin
		    }
		popd
		;;

    seccrond)
        pushd $R/src/seccrond
        CFLAGS="$CFLAGS" make -j${THREADS} &&
			install -s -p seccrond $R/build/bin
        popd
        ;;

    netdata)
        pushd $R/src/netdata
		git checkout -- web
		patch -NEp1 < $R/src/patches/netdata-dowse-integration.patch
        ./autogen.sh
        CFLAGS="$CFLAGS" \
              ./configure --prefix=${PREFIX}/netdata \
              --datarootdir=${PREFIX}/netdata \
              --with-webdir=${PREFIX}/netdata \
              --localstatedir=${DOWSE_HOME}/.dowse \
              --sysconfdir=/etc/dowse &&
            make -j${THREADS} &&
            install -s -p src/netdata $R/build/bin
        popd
        ;;

	netdata-plugins)
		pushd $R/src/netdata
		make -j${THREADS}
		popd
		;;

    sup)
        pushd $R/src/sup
        # make sure latest config.h is compiled in
        rm -f $R/src/sup/sup.o
        make -j${THREADS} && install -s -p $R/src/sup/sup $R/build
        popd
        ;;

    dnscrypt-proxy)
        pushd $R/src/dnscrypt-proxy
	## least bloated solution
		git checkout -- src/proxy &&
			patch -NEp1 < $R/src/patches/dnscrypt-noreuseableport.patch &&
			./autogen.sh &&
			./configure --without-systemd --enable-plugins --prefix=${PREFIX} &&
			make -j${THREADS} &&
			install -s -p src/proxy/dnscrypt-proxy $R/build/bin
        popd
        ;;

    dnscrypt_dowse.so|dnscrypt-plugin)
        pushd $R/src/dnscrypt-plugin
		[[ -r configure ]] || autoreconf -i
		./configure &&
			make -j${THREADS} &&
            install -s -p .libs/dnscrypt_dowse.so $R/build/bin
        popd
        ;;

    pgld)
        pushd $R/src/pgld &&
	    CFLAGS="$CFLAGS" \
		  LDFLAGS="$LDFLAGS" \
		  make -j${THREADS} &&
        install -s -p $R/src/pgld/pgld $R/build/bin
        popd
        ;;

    *)
        act "usage: ./src/compile.sh [ clean ]"
        ;;
esac
