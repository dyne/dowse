command -v squid3 > /dev/null
{ test $? = 0 } || { print "squid3 not found."; return 1 }
command -v privoxy > /dev/null
{ test $? = 0 } || { print "privoxy not found."; return 1 }

# setup a transparent proxy on port 80
# using squid and privoxy

# pid files for our daemons
pid_squid=$DIR/run/squid.pid
pid_privoxy=$DIR/run/privoxy.pid

module_setup() {

# Privoxy configuration template
    func "generating privoxy.conf"
    cat <<EOF > $DIR/run/privoxy.conf
user-manual /usr/share/doc/privoxy/user-manual
confdir /etc/privoxy
logdir $DIR/log/privoxy
listen-address  0.0.0.0:8118
toggle  1
enable-remote-toggle  0
enable-remote-http-toggle  0
enable-edit-actions 1
enforce-blocks 0
buffer-limit 64000

forwarded-connect-retries  0
accept-intercepted-requests 1
allow-cgi-request-crunching 0
split-large-forms 0
keep-alive-timeout 5
socket-timeout 300
handle-as-empty-doc-returns-ok 1

# pass through i2p for urls.i2p
# forward .i2p $dowse:4444
# forward .i2p 127.0.0.1:4444

# direct access
forward $hostname .

filterfile default.filter
actionsfile match-all.action # Actions that are applied to all sites and maybe overruled later on.
actionsfile default.action   # Main actions file
actionsfile user.action      # User customizations
EOF

    func "generating squid.conf"

# Squid2 configuration template
    cat <<EOF > $DIR/run/squid.conf
pid_filename $pid_squid
cache_effective_user $dowseuid
cache_store_log none
cache_log $DIR/log/squid_cache.log
access_log /dev/null
# access_log $DIR/log/squid_access.log squid

# avoid having a physical cache directory
cache deny all
cache_mem 16 MB
# cache_dir null /tmp
# cache_dir aufs /dev/shm/dowse 64 16 64
maximum_object_size 16 MB
maximum_object_size_in_memory 1 MB
minimum_object_size 16 KB
memory_pools off

# dns client section
dns_nameservers 127.0.0.1
hosts_file $DIR/run/hosts
append_domain .$lan
positive_dns_ttl 8 hours
negative_dns_ttl 30 seconds

acl all src all
# acl manager proto cache_object
acl localhost src 127.0.0.1/32
acl to_localhost dst 127.0.0.0/8 0.0.0.0/32

acl localnet src $dowsenet

acl SSL_ports port 443		# https
acl Safe_ports port 80		# http
acl Safe_ports port 443		# https
acl purge method PURGE
acl CONNECT method CONNECT

# http_access allow manager localhost
# http_access deny manager
# http_access allow purge localhost
http_access deny purge
http_access deny !Safe_ports
http_access deny CONNECT !SSL_ports

http_access allow localnet
# http_access allow localhost

http_access deny all

icp_access allow localnet

http_port $dowse:3128 transparent
visible_hostname ${hostname}.${lan}

hierarchy_stoplist cgi-bin ?

refresh_pattern ^ftp:		1440	20%	10080
refresh_pattern ^gopher:	1440	0%	1440
refresh_pattern -i (/cgi-bin/|\?) 0	0%	0
refresh_pattern -i (deb|tar|gz|tgz|bz2|zip|rar|msi|exe|rpm)$ 0 90% 1440
refresh_pattern (Release|Packages(.gz)*)$	0	20%	2880
refresh_pattern .		0	20%	4320

acl shoutcast rep_header X-HTTP09-First-Line ^ICY.[0-9]
# upgrade_http0.9 deny shoutcast

acl apache rep_header Server ^Apache
# broken_vary_encoding allow apache

# extension_methods REPORT MERGE MKACTIVITY CHECKOUT

cache_mgr Dowse

hosts_file $DIR/run/hosts

coredump_dir $DIR/log

cache_peer localhost parent 8118 0 default no-query no-digest no-netdb-exchange
never_direct allow all

# header_access From deny all

# the settings below are restrictive:
# they grant more privacy but break many websites!
# header_access Link deny all
# header_access Server deny all
# header_access Referer deny all
# header_access User-Agent deny all
# header_access WWW-Authenticate deny all


EOF

}

module_start() {

    func "setup transparent proxy to squid"
    iptables -t nat -A PREROUTING -i $interface -s $dowsenet \
	-p tcp --dport 80 -j REDIRECT --to-port 3128

    privoxy_start

    squid_start

}

module_stop() {

    squid_stop

    privoxy_stop

}


squid_stop() {
    { test -r $pid_squid } && {
	pid=`cat $pid_squid`
	ps -p "$pid" > /dev/null
	{ test $? = 0 } || {
	    func "removing stale pid for squid"
	    rm -f $pid_squid
	    return 1 }
	act "Stopping squid ($pid)"
	setuidgid $dowseuid squid3 -f $DIR/run/squid.conf -k kill
	{ test $? = 0 } || {
	    error "Error running squid3, the daemon might be left running."
	    return 1 }
	waitpid $pid
	rm -f $pid_squid
    }
}
squid_start() {
    act "Preparing to launch Squid..."

    func "setuidgid $dowseuid squid -f $DIR/run/squid.conf"
    # cleanup all previous cache in ram
    rm -rf /dev/shm/dowse/*

    # populate the volatile cache
    setuidgid $dowseuid squid3 -z -f $DIR/run/squid.conf
    # launch the squid
    setuidgid $dowseuid squid3 -f $DIR/run/squid.conf
}


privoxy_stop() {
    { test -r $pid_privoxy } && {
	pid=`cat $pid_privoxy`
	act "Stopping privoxy ($pid)"
	kill $pid
	waitpid $pid
	rm -f $pid_privoxy
    }
}
privoxy_start() {
    act "Preparing to launch privoxy..."

    # if running, stop to restart
    privoxy_stop

    privoxy --user $dowseuid --pidfile $pid_privoxy $DIR/run/privoxy.conf
}
