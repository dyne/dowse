#!/usr/bin/env zsh

R=`pwd`
R=${R%/*}
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

source /usr/share/zuper/zuper
source /usr/share/zuper/zuper.init

PREFIX=${PREFIX:-/usr/local/dowse}
CFLAGS="-Wall -fPIC -fPIE -Os"
LDFLAGS="-fPIC -fPIE -pie"

# exceptions
[[ "$1" = "dnscap" ]] && {
	[[ -r $R/build/bin/dowse.so ]] || {

		pushd $R/src/dnscap
		./configure --prefix=${PREFIX} \
			&& \
			make && \
			install -s -p $R/src/dnscap/dnscap $R/build/bin && \
		popd
	}
	# copy plugin over every pass, easier for debugging changes
	install -s -p $R/src/dnscap/plugins/dowse/dowse.so $R/build/bin
}


[[ -x $R/build/bin/$1 ]] && {
	act "$1 found in $R/build/bin/$1"
	act "delete it from build/bin to force recompilation"
	return 0 }

notice "Compiling $1"

case $1 in
	tinyproxy)
		pushd $R/src/tinyproxy
		CFLAGS="$CFLAGS" \
			  ./configure --enable-reverse --enable-transparent
		make -C src -f Makefile.gnu
        install -s -p src/tinyproxy $R/build/bin
		popd
		;;

    seccrond)
        pushd $R/src/seccrond
        CFLAGS="$CFLAGS" make
		install -s -p seccrond $R/build/bin
        popd
        ;;

    webdis)
        pushd $R/src/webdis
        make
        install -s -p webdis $R/build/bin
        popd
        ;;

    webui)
        pushd $R/src/webui
        $R/build/bin/kore build
        popd
        ;;

    kore)
        [[ -x $R/build/kore ]] || {
            pushd $R/src/kore
            make NOTLS=1 DEBUG=1
            install -s -p kore $R/build/bin
            popd
        }
        ;;
    netdata)
        [[ -x $R/build/netdata ]] || {
            pushd $R/src/netdata
            ./autogen.sh
            CFLAGS="$CFLAGS" \
                  ./configure --prefix=${PREFIX}/netdata \
                  --datarootdir=${PREFIX}/netdata \
                  --with-webdir=${PREFIX}/netdata \
                  --localstatedir=$HOME/.dowse \
                  --sysconfdir=/etc/dowse &&
                make &&
                install -s -p src/netdata $R/build/bin
            popd

        }
        ;;
    netdiscover)
        [[ -x $R/build/netdiscover ]] || {
            pushd $R/src/netdiscover
			autoreconf && \
            CFLAGS="$CFLAGS" ./configure --prefix=${PREFIX} && \
                make && \
                install -s -p src/netdiscover $R/build/bin
            popd
        }
        ;;

    sup)
        pushd $R/src/sup

        # make sure latest config.h is compiled in
        rm -f $R/src/sup/sup.o

        make && install -s -p $R/src/sup/sup $R/build

        popd
        ;;

    dnscrypt-proxy)
        pushd $R/src/dnscrypt-proxy
        ./configure --without-systemd --enable-plugins --prefix=${PREFIX} \
            && \
            make && \
            install -s -p src/proxy/dnscrypt-proxy $R/build/bin
        popd
        ;;

	dnscrypt_dowse.so)
		pushd $R/src/dnscrypt-plugin
		./configure && make && \
			install -s -p .libs/dnscrypt_dowse.so $R/build/bin
		popd
		;;

    pgld)
        pushd $R/src/pgl
        ./configure --without-qt4 --disable-dbus --enable-lowmem \
					--disable-networkmanager \
                    --prefix ${PREFIX}/pgl \
                    --sysconfdir ${HOME}/.dowse/pgl/etc \
                    --with-initddir=${PREFIX}/pgl/init.d \
            && \
            make -C pgld && \
            install -s -p $R/src/pgl/pgld/pgld $R/build/bin
        popd
        ;;

    *)
        act "usage; ./src/compile.sh [ pgl | dnscap | clean ]"
        ;;
esac
