#!/usr/bin/env zsh

R=${$(pwd)%/*}
[[ -r $R/src ]] || {
    print "error: config.sh must be run from the src"
    return 1
}

zkv=1
source $R/zuper/zuper
vars=(tmp)
maps=(execmap execrules execsums)
source $R/zuper/zuper.init

zkv.load $R/build/db/execmap.zkv
zkv.load $R/build/db/execrules.zkv

act "generating execution rules"

builduid=`id -u $1`
buildgid=`id -g $1`
notice "Computing checksums to lock superuser privileges for user $1"

execsums=()

for x in ${(k)execmap}; do
    [[ "$execrules[$x]" = "root" ]] && {
        if [[ -r $R/build/bin/$x ]]; then
            cksum=`sha256sum $R/build/bin/$x`
        elif [[ -r ${execmap[$x]} ]]; then
            cksum=`sha256sum ${execmap[$x]}`
        else
            warning "$x: binary not found in build or path (${execmap[$x]})"
            continue
        fi
        cksum=${cksum[(w)1]}
        [[ "$cksum" = "" ]] && {
            warning "missing checksum for: $x"
            continue }
        execsums+=($x $cksum)
        act "$cksum $x"
    }
done
zkv.save execsums $R/build/db/execsums.zkv

notice "Dowse build complete on `hostname` (`date`)"
