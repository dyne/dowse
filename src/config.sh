#!/usr/bin/env zsh

# This are the schemes of dowse databases and lists in redis.

# This information is used to generate code that is then included at
# compile time, so any change of it requires a recompilation.


R=`pwd`/..
[[ -r $R/src ]] || {
    print "error: config.sh must be run from the src"
    return 1
}

mkdir -p $R/run

zkv=1
source $R/zlibs/zuper
vars=(tmp)
maps=(db mod execmap execsums execrules)
source $R/zlibs/zuper.init

source paths.sh


deb-download() {
    fn deb-download $*
    deb="$1"
    req=(deb tmp)
    ckreq || return 1

    [[ $? = 0 ]] || {
        error "cannot create temporary directory"
        return 1 }

    pushd $tmp > /dev/null

    apt-get -q download $deb
    [[ $? = 0 ]] || {
        error "error downloading $deb"
        return 1 }

    debfile=`find . -name "${deb}_*.deb"`

    popd > /dev/null

    freq=($tmp/$debfile)
    ckreq || return 1

    act "extracting $R/tmp/$debfile"
    dpkg -x $tmp/$debfile $tmp
    [[ $? = 0 ]] || {
        error "error extracting $tmp/$debfile"
        return 1 }

    return 0
}

# for the db keys namespace see doc/HACKING.md

dbindex='
0 dynamic
1 runtime
2 storage
'

# Check if Apt based
command -v apt-get >/dev/null && {
    notice "Importing binary packages from apt repositories..."
    tmp=`mktemp -d`


    [[ -r $execmap[dnsmasq] ]] || {
        act "fetching dnsmasq"
        deb-download dnsmasq-base
        cp -v $tmp/usr/sbin/dnsmasq $R/run
    }

    [[ -r $execmap[redis-server] ]] || {
        act "fetching redis server"
        deb-download redis-server
        cp $tmp/usr/bin/redis-server $R/run }

    [[ -r $execmap[redis-cli] ]] || {
        act "fetching redis tools"
        deb-download redis-tools
        cp $tmp/usr/bin/redis-cli $R/run
    }

    [[ -r $execmap[tor] ]] || {
        act "fetching tor"
        deb-download tor
        cp $tmp/usr/bin/tor $R/run
    }

    rm -rf $tmp

}


### Database index

# save databases for the shell scripts
for i in ${(f)dbindex}; do
    # this is reverse order: names are the indexes
    db+=( ${i[(w)2]} ${i[(w)1]} )
done
zkv.save db $R/src/database.zkv

# save databases for the C code
rm -rf $R/src/database.h
touch  $R/src/database.h
for i in ${(k)db}; do
    print "#define db_$i ${db[$i]}" >> $R/src/database.h
done






### Modules index

# # save modules for the shell scripts
# mod=()
# for i in ${(f)modcodes}; do
#     mod+=( ${i[(w)2]} ${i[(w)1]} )
# done

# # save modules for the C code
# rm -rf $R/src/module.h
# touch  $R/src/module.h
# for i in ${(k)mod}; do
#     print "#define mod_$i ${mod[$i]}" >> $R/src/module.h
# done

# mod=()
# for i in {1..${(f)#modfields}}; do
#     mod+=( $i ${modfields[$i]} )
# done
# zkv.save mod $R/src/module.zkv


print "# Compile-time configuration generated"
