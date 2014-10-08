#!/usr/bin/env zsh

dnsmasq_conf() {
    func "generating dnsmasq configuration"
    cat <<EOF
address=/$hostname/$dowse
address=/$hostname.$lan/$dowse
# address=/.i2p/$dowse
# address=/.onion/$dowse
bogus-priv
cache-size=300
dhcp-range=$dowseguests
addn-hosts=$DIR/run/hosts
dhcp-leasefile=$DIR/run/leases
domain-needed
domain=$lan
expand-hosts
edns-packet-max=4096
interface=$interface
listen-address=$dowse,127.0.0.1
local=//127.0.0.1#53
local=/$lan/
user=$dowseuid
group=$dowsegid
EOF
    # read the network configuration of known hosts
    known=`cat $DIR/conf/network | grep -v '^#'`
  
    # DNSMasq LAN resolution
    func "Fixing entries for known peers"
    rm -f $DIR/run/dnsmasq.network
    # this is basically a dnsmasq host configuration file
    print "dhcp-option=option:router,$dowse" > $DIR/run/dnsmasq.network

    # this is our generated hosts file
    func "Generating hosts file"
    rm -f $DIR/run/hosts
    print "127.0.0.1 localhost" > $DIR/run/hosts
    for i in ${(f)known}; do
    print "$i" | grep '^..:..:..:..:..:..' > /dev/null
    if [ $? = 0 ]; then # mac address is first
        host=${i[(w)2]}
        ip=${i[(w)3]}
    else # no mac address specified
        host=${i[(w)1]}
        ip=${i[(w)2]}
    fi
    { test "$host" = "ignore" } || {
    # add a line to the hosts list
        print "$ip $host" >> $DIR/run/hosts }
    done

    func "generating dnsmask.network"
    for i in ${(f)known}; do
    print "$i" | grep '^..:..:..:..:..:..' > /dev/null
    { test $? = 0 } || { continue } # skip if no mac address
    func "$i"
    # gather configuration into variables, line by line
    mac=${i[(w)1]}
    host=${i[(w)2]}
    ip=${i[(w)3]}

    # add a line to the dnsmasq host list
    print "dhcp-host=$mac, $host, $ip" >> $DIR/run/dnsmasq.network

    done

    # append network settings to dnsmasq conf
    cat $DIR/run/dnsmasq.network

}

dnsmasq_start() {
    act "Preparing to launch dnsmasq..."
 
    # if running, stop to restart
    dnsmasq_stop
    
    func "dnsmasq --pid-file $DIR/run/dnsmasq.pid -C $1"
    dnsmasq --pid-file=$DIR/run/dnsmasq.pid -C "$1"
}

dnsmasq_stop() {
    [[ -r $DIR/run/dnsmasq.pid ]] && {
        pid=`cat $DIR/run/dnsmasq.pid`
        act "Stopping dnsmasq ($pid)"
        kill $pid
        waitpid $pid
        rm -f $DIR/run/dnsmasq.pid
    }
}
