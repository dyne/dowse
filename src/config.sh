#!/usr/bin/env zsh

# This are the schemes of dowse databases and lists in redis.

# This information is used to generate code that is then included at
# compile time, so any change of it requires a recompilation.

S=${$(pwd)%/*}
[[ -r $S/src ]] || {
    print "error: config.sh must be run from the src"
    return 1
}


zkv=1
source $S/zlibs/zuper
maps=(db dbindex execmap execrules)
source $S/zlibs/zuper.init

fn config $*

PREFIX="$1"
req=(PREFIX)
ckreq || return 1


#########
dbindex='
0 dynamic
1 runtime
2 storage
'
#########
# for the db keys namespace see doc/HACKING.md

mkdir -p $S/build

# map of permissions
execrules=(
    dnscrypt     user
    redis-cli    user
    redis-server user
    nmap         user
    arp          user
    ip           user

    dnsmasq       root
    dnscap        root
    ifconfig      root
    route         root
    iptables      root
    xtables-multi root
    ebtables      root
    sysctl        root
    # TODO: sup list of authorized modules to load (using libkmod)
    modprobe      root
    # TODO: sup list of authorized signals to emit (using killall)
    kill          root
    pgld          root
    netdiscover   root
)
zkv.save execrules $S/build/execrules.zkv



# paths for Devuan
execmap=(
    ifconfig      /sbin/ifconfig
    route         /sbin/route
    dnscrypt      $PREFIX/bin/dnscrypt-proxy
    dnsmasq       $PREFIX/bin/dnsmasq
    dnscap        $PREFIX/bin/dnscap
    redis-cli     $PREFIX/bin/redis-cli
    redis-server  $PREFIX/bin/redis-server
    tor           $PREFIX/bin/tor
    kill          /bin/kill
    xtables-multi /sbin/xtables-multi
    ebtables      /sbin/ebtables
    sysctl        /sbin/sysctl
    ip            /sbin/ip
    arp           /usr/sbin/arp
    netdiscover   $PREFIX/bin/netdiscover
    # TODO: sup list of authorized modules (using libkmod)
    modprobe      $PREFIX/bin/modprobe
    pgld          $PREFIX/bin/pgld
    libjemalloc   /usr/lib/x86_64-linux-gnu/libjemalloc.so.1
    nmap          /usr/bin/nmap
)

# check if on Gentoo
command -v emerge >/dev/null && {
    execmap=(
        dnscrypt      /usr/sbin/dnscrypt-proxy
        dnscap        $PREFIX/src/dnscap/dnscap
        dnsmasq       /usr/sbin/dnsmasq
        redis-cli     /usr/bin/redis-cli
        redis-server  /usr/sbin/redis-server
        iptables      /sbin/iptables
        ebtables      /sbin/ebtables
        sysctl        /usr/sbin/sysctl
        pgl           $PREFIX/bin/pgld
        libjemalloc   /usr/lib64/libjemalloc.so.2
    )
}

case `uname -m` in
    arm*)
        execmap[libjemalloc]=/usr/lib/arm-linux-gnueabihf/libjemalloc.so
        ;;
esac

zkv.save execmap $S/build/execmap.zkv


### Database index

# save databases for the shell scripts
for i in ${(f)dbindex}; do
    # this is reverse order: names are the indexes
    db+=( ${i[(w)2]} ${i[(w)1]} )
done
zkv.save db $S/build/database.zkv

# save databases for the C code
rm -rf $S/src/database.h
touch  $S/src/database.h
for i in ${(k)db}; do
    print "#define db_$i ${db[$i]}" >> $S/src/database.h
done

notice "Database indexes generated"
