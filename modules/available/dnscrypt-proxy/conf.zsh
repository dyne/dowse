command -v dnscrypt-proxy > /dev/null
{ test $? = 0 } || { print "dnscrypt-proxy not found."; return 1 }

module_setup() {
    cat <<EOF >> $DOWSE/run/dnsmasq.conf
server=127.0.0.1#5353
no-resolv
EOF
}

dnscrypt_pid="$DIR/run/dnscrypt-proxy.pid"

dnscrypt() {
setuidgid $dowseuid dnscrypt-proxy -a 127.0.0.1:5353 -p $dnscrypt_pid -l $DIR/log/dnscrypt.log -r "$1" -k "$2" -N "$3"  -d
}

module_start() {

# DNSCrypt.EU (Netherlands)
dnscrypt 176.56.237.171:443 67C0:0F2C:21C5:5481:45DD:7CB4:6A27:1AF2:EB96:9931:40A3:09B6:2B8D:1653:1185:9C66 2.dnscrypt-cert.resolver1.dnscrypt.eu

# DNSCrypt.EU (Denmark)
# dnscrypt 77.66.84.233:443 3748:5585:E3B9:D088:FD25:AD36:B037:01F5:520C:D648:9E9A:DD52:1457:4955:9F0A:9955 2.dnscrypt-cert.resolver2.dnscrypt.eu

# Soltysiak.com (Poland)
# dnscrypt 178.216.201.222:2053 25C4:E188:2915:4697:8F9C:2BBD:B6A7:AFA4:01ED:A051:0508:5D53:03E7:1928:C066:8F21 2.dnscrypt-cert.soltysiak.com

# OpenDNS
# dnscrypt 208.67.220.220:443 B735:1140:206F:225D:3E2B:D822:D7FD:691E:A1C3:3CC8:D666:8D0C:BE04:BFAB:CA43:FB79 2.dnscrypt-cert.opendns.com

    iptables -t nat -A PREROUTING -i $interface -s $dowsenet \
        -p udp --dport 53 -j REDIRECT --to-port 53
 
    # close outgoing dns queries
    # iptables -A OUTPUT  -p udp --dport 53 -j DROP

}

module_stop() {
    { test -r $dnscrypt_pid } && {
	kill `cat $dnscrypt_pid`
	rm -f $dnscrypt_pid
    }
    # reopen routing of dns queries
    # iptables -A OUTPUT  -p udp --dport 53 -j ACCEPT
}
