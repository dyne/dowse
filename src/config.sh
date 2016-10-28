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
source $S/zuper/zuper
vars=(dbindex thingindex)
maps=(db execmap execrules)
source $S/zuper/zuper.init

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
# for the db keys namespace see doc/HACKING.md
#########

mkdir -p $S/build/db

thingindex='
macaddr  varchar(18) primary key
ip4      text
ip6      text
hostname text
iface    text
state    text
os       text
dhcp     text
gateway  text
network  text
notes    text
last     date
age      date
'
print - "$thingindex" > $S/build/db/thing.idx

# map of permissions
execrules=(
    redis-cli    user
    redis-server user
    nmap         user
    arp          user
    ip           user
    netdata      user
	seccrond     user
	omshell      user
	mosquitto    user
	mysqld       user
	mysql        user

# springs
	dowse-to-mqtt user

	dhcpd         root
    webui         root
    dnscrypt-proxy root
    ifconfig      root
    route         root
    iptables      root
    xtables-multi root
    ebtables      root
    # TODO: sup list of authorized /proc and /sys paths to write
    sysctl        root
    # TODO: sup list of authorized modules to load (using libkmod)
    modprobe      root
    # TODO: sup list of authorized signals to emit (using killall)
    kill          root
    pgld          root
)
zkv.save execrules $S/build/db/execrules.zkv



# paths for Devuan
execmap=(
    ifconfig      /sbin/ifconfig
    route         /sbin/route
    dnscrypt-proxy $PREFIX/bin/dnscrypt-proxy
    redis-cli     $PREFIX/bin/redis-cli
    redis-server  $PREFIX/bin/redis-server
	tinyproxy     $PREFIX/bin/tinyproxy
    netdata       $PREFIX/bin/netdata
    webui         $PREFIX/bin/webui
	seccrond      $PREFIX/bin/seccrond
	mosquitto     $PREFIX/bin/mosquitto
	dhcpd         $PREFIX/bin/dhcpd
	omshell       $PREFIX/bin/omshell
	mysqld        $PREFIX/mysql/bin/mysqld
	mysql         $PREFIX/mysql/bin/mysql

	dowse-to-mqtt $PREFIX/bin/dowse-to-mqtt
	dowse-to-osc  $PREFIX/bin/dowse-to-osc

    kill          /bin/kill
    xtables-multi /sbin/xtables-multi
    ebtables      /sbin/ebtables
    sysctl        /sbin/sysctl
    ip            /sbin/ip
    arp           /usr/sbin/arp
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

zkv.save execmap $S/build/db/execmap.zkv


### Database index

# save databases for the shell scripts
for i in ${(f)dbindex}; do
    # this is reverse order: names are the indexes
    db+=( ${i[(w)2]} ${i[(w)1]} )
done
zkv.save db $S/build/db/database.zkv

# save databases for the C code
rm -rf $S/src/database.h
touch  $S/src/database.h
for i in ${(k)db}; do
    print "#define db_$i ${db[$i]}" >> $S/src/database.h
done

# db=()
# for i in ${(f)thingindex}; do
#     db+=( ${i[(w)1]} ${i[(w)1]} )
# done

rm -rf $S/src/thingsdb.h
c=1
print "#define THINGS_DB \"$HOME/.dowse/run/things.db\"" > $S/src/thingsdb.h

# for i in "${(f)thingindex}"; do
#     [[ "$i" = "" ]] && continue
#     print "#define ${i[(w)1]} $c" >> $S/src/thingsdb.h
#     c=$(( $c + 1 ))
# done

print "#define all_things_fields $(( $c - 1 ))" >> $S/src/thingsdb.h

# print "static char *get[] = {" >> $S/src/thingsdb.h
# for i in "${(f)thingindex}"; do
#     [[ "$i" = "" ]] && continue
#     print "\"${i[(w)1]}\"," >> $S/src/thingsdb.h
# done
# print " NULL };" >> $S/src/thingsdb.h

notice "Database indexes generated"

#####
_id=`id -un`
_gid=`id -gn`
cat <<EOF > privileges
dowse_uid=$_id
dowse_gid=$_gid
EOF
notice "Configured privileges for caller: $_id $_gid"
######
