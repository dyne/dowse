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
conf-dir=/etc/dnsmasq.d
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
}

dnsmasq_start() {
    act "Preparing to launch dnsmasq..."
    
    # if running, stop to restart
    dnsmasq_stop
    
    func "dnsmasq --pid-file $DIR/run/dnsmasq.pid -C $DIR/run/dnsmasq.conf"
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
