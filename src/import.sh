#!/usr/bin/env zsh


S=${$(pwd)%/*}
[[ -r $S/src ]] || {
    print "error: config.sh must be run from the src"
    return 1
}



zkv=1
source $S/zlibs/zuper
source $S/zlibs/zuper.init

fn import $*

mkdir -p $S/build

deb-download() {
    fn deb-download $*
    deb="$1"
    req=(deb tmp)
    ckreq || return 1

    bckfile=`find $S/debs -name "${deb}_*.deb"`
    if [[ "$bckfile" = "" ]]; then
        apt-get -q download $deb
        [[ $? = 0 ]] || {
            error "error downloading $deb"
            return 1 }

        debfile=`find . -name "${deb}_*.deb"`

        freq=($debfile)
        ckreq || return 1

        act "backupping $debfile"
        mkdir -p $S/debs
        cp $debfile $S/debs/
    else
        debfile=$bckfile
    fi

    act "extracting $debfile"
    dpkg -x $debfile .
    [[ $? = 0 ]] || {
        error "error extracting $tmp/$debfile"
        return 1 }

    return 0
}



# Check if Apt based
command -v apt-get >/dev/null && {
    notice "Importing binary packages from apt repositories..."
    mkdir -p $S/build
    tmp=`mktemp -d`
    act "using temporary directory: $tmp"
    pushd $tmp > /dev/null

    [[ -r $S/build/dnsmasq ]] || {
        act "fetching dnsmasq"
        deb-download dnsmasq-base
        cp $tmp/usr/sbin/dnsmasq $S/build
    }

    [[ -r $S/build/redis-server ]] || {
        act "fetching redis server"
        deb-download redis-server
        cp $tmp/usr/bin/redis-server $S/build }

    [[ -r $S/build/redis-cli ]] || {
        act "fetching redis tools"
        deb-download redis-tools
        cp $tmp/usr/bin/redis-cli $S/build
    }

    [[ -r $S/build/tor ]] || {
        act "fetching tor"
        deb-download tor
        cp $tmp/usr/bin/tor $S/build
    }

    popd
    rm -rf $tmp

}
