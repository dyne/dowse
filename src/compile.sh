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

        make

        popd
        ;;

    dnscrypt-proxy)
        [[ -x $R/src/dnscrypt-proxy/src/proxy/dnscrypt-proxy ]] || {
            pushd $R/src/dnscrypt-proxy
            ./configure --without-systemd --prefix=${PREFIX} \
                && \
                make
            popd
        }
        ;;

    pgl)
        [[ -x $R/src/pgl/pgld/pgld ]] || {
            pushd $R/src/pgl
            ./configure --without-qt4 --disable-dbus --enable-lowmem \
	                --prefix ${PREFIX}/pgl \
                        --sysconfdir ${PREFIX}/pgl/etc \
	                --with-initddir=${PREFIX}/pgl/init.d \
                && \
                make
            # install prefix is local to dowse
            popd
        }
        ;;

    dnscap)
        [[ -x $R/src/dnscap/dnscap ]] || {
            pushd $R/src/dnscap
            ./configure --prefix=${PREFIX} \
                && \
                make
            popd
        }
        ;;

    install)
        [[ "$UID" = "0" ]] || {
            print
            print "need to run make install as root"
            print
            return 1 }
        pushd $R/src/pgl
        make install
        popd
        ;;


    clean)
        make -C $R/src/dnscap               clean
        make -C $R/src/dnscap/plugins/dowse clean
        make -C $R/src/dnscrypt-proxy       clean
        make -C $R/src/pgl                  clean
        make -C $R/src/sup                  clean

        ;;
    *)
        print "usage; ./src/compile.sh [ pgl | dnscap | clean ]"
        ;;
esac
