#!/usr/bin/env zsh

R=`pwd`
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

[[ -x /usr/local/bin/dnscap ]] || {
    pushd $R/src/dnscap
    ./configure
    make
    make install
    popd
}

[[ -r $R/src/dnscap/plugins/dowse/dowse.so ]] || {
    make -C $R/src/dnscap/plugins/dowse
}

