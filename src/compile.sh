#!/usr/bin/env zsh

R=`pwd`
R=${R%/*}
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

source $R/zlibs/zuper
source $R/zlibs/zuper.init

PREFIX=${PREFIX:-/usr/local/dowse}
CFLAGS="-Wall -fPIC -fPIE -Os"
LDFLAGS="-fPIC -fPIE -pie"

case $1 in
    webdis)
        pushd $R/src/webdis
        make
        install -s -p webdis $R/build
        popd
        ;;

    webui)
        pushd $R/src/webui
        $R/build/kore build
        popd
        ;;

    kore)
        [[ -x $R/build/kore ]] || {
            pushd $R/src/kore
            make NOTLS=1 DEBUG=1
            install -s -p kore $R/build
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
                install -s -p src/netdata $R/build             
            popd

        }
        ;;
    netdiscover)
        [[ -x $R/build/netdiscover ]] || {
            pushd $R/src/netdiscover
			autoreconf && \
            CFLAGS="$CFLAGS" ./configure --prefix=${PREFIX} && \
                make && \
                install -s -p src/netdiscover $R/build
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
        [[ -x $R/build/dnscrypt-proxy ]] || {
            pushd $R/src/dnscrypt-proxy
            ./configure --without-systemd --prefix=${PREFIX} \
                && \
                make && \
                install -s -p src/proxy/dnscrypt-proxy $R/build
            popd
        }
        ;;

    pgl)
        [[ -x $R/build/pgld ]] || {
            pushd $R/src/pgl
            ./configure --without-qt4 --disable-dbus --enable-lowmem \
						--disable-networkmanager \
                        --prefix ${PREFIX}/pgl \
                        --sysconfdir ${HOME}/.dowse/pgl/etc \
                        --with-initddir=${PREFIX}/pgl/init.d \
                && \
                make -C pgld && \
                install -s -p $R/src/pgl/pgld/pgld $R/build
            popd
        }
        ;;

    dnscap)
        [[ -x $R/build/dnscap ]] || {
            pushd $R/src/dnscap
            ./configure --prefix=${PREFIX} \
                && \
                make && \
                install -s -p $R/src/dnscap/dnscap $R/build && \
                install -s -p $R/src/dnscap/plugins/dowse/dowse.so $R/build
            popd
        }
        # copy plugin over every pass, easier for debugging changes
        install -s -p $R/src/dnscap/plugins/dowse/dowse.so $R/build
        ;;

    *)
        print "usage; ./src/compile.sh [ pgl | dnscap | clean ]"
        ;;
esac
