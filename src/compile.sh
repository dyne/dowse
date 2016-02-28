#!/usr/bin/env zsh

R=`pwd`
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

pushd $R/src/dnscap
./configure
make
make -C plugins/dowse
# make install
popd
