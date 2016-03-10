#!/usr/bin/env zsh

R=`pwd`
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

case $1 in
    pgl)
        [[ -r $R/run/pgl ]] || {
            pushd $R/src/pgl
            ./configure --without-qt4 --disable-dbus --enable-lowmem \
	                --prefix $R/run/pgl --sysconfdir $R/run/pgl/etc \
	                --with-initddir=$R/run/pgl/init.d \
                && \
                make \
                && \
                make install
            # install prefix is local to dowse
            popd
        }
        ;;

    dnscap)
        [[ -r $R/run/dnscap ]] || {
            pushd $R/src/dnscap
            ./configure \
                && \
                make \
                && \
                make -C plugins/dowse \
                && \
                cp $R/src/dnscap/dnscap $R/run \
                && \
                cp $R/src/dnscap/plugins/dowse/dowse.so $R/run
            popd
        }
        ;;

    clean)

        make -C $R/src/dnscap clean
        rm $R/run/dnscap $R/run/dowse.so

        rm -rf $R/run/pgl
        make -C $R/src/pgl    clean
        ;;
    *)
        print "usage; ./src/compile.sh [ pgl | dnscap | clean ]"
        ;;
esac
