command -v dnscrypt-proxy > /dev/null
{ test $? = 0 } || { print "dnscrypt-proxy not found."; return 1 }

module_setup() {
    cat <<EOF >> $DOWSE/run/dnsmasq.conf
server=127.0.0.1#5353
no-resolv
EOF
}

module_start() {
	# DNSCrypt.EU (Netherlands)
    setuidgid $dowseuid dnscrypt-proxy -a 127.0.0.1:5353 -d -p $DOWSE/run/dnscrypt-proxy.pid \
	-r 176.56.237.171:443 -k 67C0:0F2C:21C5:5481:45DD:7CB4:6A27:1AF2:EB96:9931:40A3:09B6:2B8D:1653:1185:9C66 -N 2.dnscrypt-cert.resolver1.dnscrypt.eu 

        # OpenDNS
	# -r 208.67.220.220:443 -k B735:1140:206F:225D:3E2B:D822:D7FD:691E:A1C3:3CC8:D666:8D0C:BE04:BFAB:CA43:FB79 -N 2.dnscrypt-cert.opendns.com

}

module_stop() {
    { test -r $DOWSE/run/dnscrypt-proxy.pid } && {
	kill `cat $DOWSE/run/dnscrypt-proxy.pid` }
}
