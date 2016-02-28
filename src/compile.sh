#!/usr/bin/env zsh

R=`pwd`
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

pushd $R/src/pgl
./configure --without-qt4 --disable-dbus --enable-lowmem \
	--prefix $R/run/pgl --sysconfdir $R/run/pgl/etc \
	--with-initddir=$R/run/pgl/init.d
make
make install
popd

pushd $R/src/dnscap
./configure
make
make -C plugins/dowse
# make install
popd


