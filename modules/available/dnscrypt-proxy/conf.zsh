#!/usr/bin/env zsh

command -v dnscrypt-proxy > /dev/null

{ test $? = 0 } || { print "dnscrypt-proxy not found."; return 1 }

module_setup() {
    cat <<EOF >> $DOWSE/run/dnsmasq.conf
server=127.0.0.1#5353
no-resolv
EOF
}

module_start() {
    dnscrypt-proxy -a 127.0.0.1:5353 -d
}

module_stop() {
    killall dnscrypt-proxy
}
