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

case $1 in
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
	                --prefix ${PREFIX}/pgl \
                        --sysconfdir ${HOME}/.dowse/pgl/etc \
	                --with-initddir=${PREFIX}/pgl/init.d \
                && \
                make src/pgl/pgld/pgld && \
                install -s -p $R/src/pgl/pgld/pgld $R/build && \
                install    -p $R/src/pgl/pglcmd/pglcmd $R/build
            # install prefix is local to dowse
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
        ;;

    *)
        print "usage; ./src/compile.sh [ pgl | dnscap | clean ]"
        ;;
esac
