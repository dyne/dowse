#!/usr/bin/env dowse

#+MODULE: Squid -> Privoxy -> Polipo
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

# pid files for our daemons
pid_squid=$DIR/run/squid.pid
pid_privoxy=$DIR/run/privoxy.pid
pid_polipo=$DIR/run/polipo.pid

module_setup() {

    squid_conf > $DIR/run/squid.conf
    cat <<EOF >> $DIR/run/squid.conf
# pass through privoxy
cache_peer $dowse parent 8118 0 default no-query no-digest no-netdb-exchange proxy-only
EOF

    privoxy_conf > $DIR/run/privoxy.conf
    cat <<EOF >> $DIR/run/privoxy.conf
# pass through polipo
forward / $dowse:8123
EOF

    polipo_conf > $DIR/run/polipo.conf
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

    squid_stop

    privoxy_stop

    polipo_stop
}
