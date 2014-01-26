command -v dnscrypt-proxy > /dev/null
{ test $? = 0 } || { print "dnscrypt-proxy not found."; return 1 }

module_setup() {
    cat <<EOF >> $DOWSE/run/dnsmasq.conf
server=127.0.0.1#5353
no-resolv
EOF
}

module_start() {
    setuidgid $dowseuid dnscrypt-proxy -a 127.0.0.1:5353 -d -p $DOWSE/run/dnscrypt-proxy.pid
}

module_stop() {
    { test -r $DOWSE/run/dnscrypt-proxy.pid } && {
	kill `cat $DOWSE/run/dnscrypt-proxy.pid` }
}
