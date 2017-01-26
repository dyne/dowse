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
CFLAGS="-Wall -fPIC -fPIE -Os"
LDFLAGS="-fPIC -fPIE -pie -lb64"


[[ -x $R/build/bin/$1 ]] && {
    act "$1 found in $R/build/bin/$1"
    act "delete it from build/bin to force recompilation"
    return 0 }

notice "Compiling $1"

case $1 in
	log)
		[[ -r $R/src/log/liblog.a ]] && return 0
		pushd $R/src/log
		CFLAGS="$CFLAGS" \
			  LDFLAGS="$LDFLAGS" \
			  make
		popd
		;;

	maria2redis)
		pushd $R/src/maria2redis
		case `uname -m` in
			i686*) CFLAGS+=" -D_LARGEFILE64_SOURCE=1"
		esac
		CFLAGS+=" -Os" \
			  make
		popd
		;;

	libwebsockets)
		[[ -r $R/src/libwebsockets/lib/libwebsockets.a ]] && return 0
		pushd $R/src/libwebsockets
		# fix for broken path detection
		cmakeflags=""
		case `uname -m` in
			arm*) cmakeflags+="-DZLIB_LIBRARY:FILEPATH=/usr/lib/arm-linux-gnueabih" ;;
		esac
		CFLAGS="$CFLAGS" \
			  LDFLAGS="$LDFLAGS" \
			  cmake -DLWS_WITH_SSL=OFF -DLWS_WITH_SHARED=OFF \
			  -DLWS_WITHOUT_TESTAPPS=ON -DLWS_IPV6=ON -DLWS_STATIC_PIC=ON $cmakeflags . &&
			make
		popd
		;;

	mosquitto)
		pushd $R/src/mosquitto
		make -C lib &&
		CFLAGS="$CFLAGS" \
			  LDFLAGS="$LDFLAGS" \
			  make &&
			install -s -p src/mosquitto $R/build/bin &&
		# make WITH_BRIDGE=no WITH_TLS=no WITH_WEBSOCKETS=yes WITH_DOCS=no \
		# LWS_LIBRARY_VERSION_NUMBER=2.0 &&
		popd
		;;

	dhcpd)
		pushd $R/src/dhcp
		act "please wait while preparing the build environment"
		act "also prepare to wait more for the BIND export libs"
		act "when you see ISC_LOG_ROLLINFINITE then is almost there"
		autoreconf -i &&
		CFLAGS="$CFLAGS" \
			  LDFLAGS="$LDFLAGS" \
			  ./configure --enable-paranoia --enable-execute &&
		CFLAGS="$CFLAGS" \
			  LDFLAGS="$LDFLAGS" \
			  make && {
			install -s -p server/dhcpd    $R/build/bin &&
			install -s -p dhcpctl/omshell $R/build/bin
		} &&
		popd
		;;

    seccrond)
        pushd $R/src/seccrond
        CFLAGS="$CFLAGS" make &&
        install -s -p seccrond $R/build/bin
        popd
        ;;

    # first kore, then webui (which is built with kore)
    kore)
        [[ -x $R/build/kore ]] || {
            pushd $R/src/kore  &&
            make NOTLS=1 DEBUG=1 &&
            popd
        }
        ;;
    webui)
        pushd $R/src/webui 
        notice "Generating WebUI configuration"
        act "chroot: $HOME/.dowse"
        act "uid:    $USER"
        cat <<EOF > conf/webui.conf
chroot    $HOME/.dowse
runas     $USER
pidfile   $HOME/.dowse/run/webui.pid
EOF
        cat conf/webui.conf.dist >> conf/webui.conf

		cat <<EOF > conf/build.conf
# generated at build time
cflags = -DDB_SOCK_DIRECTORY="$HOME/.dowse/run/mysql/mysqld.sock"
EOF
        cat conf/build.conf.dist >> conf/build.conf

	act "launch the actual build"
	make all &&
        install -s -p webui $R/build/bin &&
        popd
        ;;

    netdata)
        pushd $R/src/netdata &&
        ./autogen.sh &&
        CFLAGS="$CFLAGS" \
              ./configure --prefix=${PREFIX}/netdata \
              --datarootdir=${PREFIX}/netdata \
              --with-webdir=${PREFIX}/netdata \
              --localstatedir=$HOME/.dowse \
              --sysconfdir=/etc/dowse &&
            make &&
            install -s -p src/netdata $R/build/bin &&
        popd

        ;;
    netdiscover)
        pushd $R/src/netdiscover &&
        autoreconf && \
            CFLAGS="$CFLAGS" ./configure --prefix=${PREFIX} && \
            make && \
            install -s -p src/netdiscover $R/build/bin &&
        popd
        ;;

    sup)
        pushd $R/src/sup
        # make sure latest config.h is compiled in
        rm -f $R/src/sup/sup.o
        make && install -s -p $R/src/sup/sup $R/build &&
        popd
        ;;

    dnscrypt-proxy)
        pushd $R/src/dnscrypt-proxy
	## least bloated solution
		git checkout -- src/proxy &&
		patch -p1 < $R/src/patches/dnscrypt-noreuseableport.patch &&
		./autogen.sh &&
         ./configure --without-systemd --enable-plugins --prefix=${PREFIX} &&
            make && 
            install -s -p src/proxy/dnscrypt-proxy $R/build/bin &&
         popd
        ;;

    dnscrypt_dowse.so)
        pushd $R/src/dnscrypt-plugin
		autoreconf -i &&
			./configure &&
			make &&
            install -s -p .libs/dnscrypt_dowse.so $R/build/bin &&
        popd
        ;;

    pgld)
        pushd $R/src/pgld &&
		CFLAGS="$CFLAGS" \
			  LDFLAGS="$LDFLAGS" \
              make &&
        install -s -p $R/src/pgld/pgld $R/build/bin &&
        popd
        ;;

    *)
        act "usage: ./src/compile.sh [ clean ]"
        ;;
esac
