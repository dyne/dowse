#!/usr/bin/env zsh

command -v dnscrypt-proxy > /dev/null

{ test $? = 0 } || { print "dnscrypt-proxy not found."; return 1 }

cat <<EOF >> $DOWSE/run/dnsmasq.conf
server=127.0.0.1#5353
no-resolv
EOF

dnscrypt-proxy -a 127.0.0.1:5353 -d
