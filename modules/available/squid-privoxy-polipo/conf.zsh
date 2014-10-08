#!/usr/bin/env dowse


#+MODULE: squid-privoxy-polipo
#+NAME: Squid -> Privoxy -> Polipo
#+DESC: Transparent http proxy pipeline chaining squid to privoxy to polipo.
#+TYPE: http proxy
#+DEPS: none
#+INSTALL: privoxy squid3 polipo
#+AUTHOR: Jaromil, Anatole
#+VERSION: 0.1

require squid3
require privoxy
require polipo

# setup a transparent proxy on port 80
# using squid and privoxy

module_setup() {

    squid_conf > $DIR/run/squid.conf
    cat <<EOF >> $DIR/run/squid.conf
pid_filename $DIR/run/squid.pid
cache_log $DIR/log/squid_cache.log
access_log /dev/null
# access_log $DIR/log/squid_access.log squid
# pass through privoxy
cache_peer $dowse parent 8118 0 default no-query no-digest no-netdb-exchange proxy-only
EOF

    privoxy_conf > $DIR/run/privoxy.conf
    cat <<EOF >> $DIR/run/privoxy.conf
logdir $DIR/log/privoxy.log
pid-file $DIR/log/privoxy.pid
# pass through polipo
forward / $dowse:8123
EOF

    polipo_conf > $DIR/run/polipo.conf
    cat <<EOF >> $DIR/run/polipo.conf
pidFile = $DIR/run/polipo.pid
logFile = $DIR/log/polipo.log
EOF

}

module_start() {

    func "setup transparent proxy to squid"
    iptables -t nat -A PREROUTING -i $interface -s $dowsenet \
        -p tcp --dport 80 -j REDIRECT --to-port 3128
    
    polipo_start $DIR/run/polipo.conf

    privoxy_start $DIR/run/privoxy.conf
    
    squid_start $DIR/run/squid.conf

}

module_stop() {

    squid_stop $DIR/run/squid.pid

    privoxy_stop $DIR/log/privoxy.pid

    polipo_stop $DIR/run/polipo.pid

}
