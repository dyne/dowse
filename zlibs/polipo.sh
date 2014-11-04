#!/usr/bin/env zsh

polipo_conf() {
    func "generating polipo.conf"
    cat <<EOF
# own address (restricts to ipv4)
proxyAddress = "$dowse"
proxyName = "Dowse"

# allow local network
allowedClients = 127.0.0.1, $dowsenet

# avoid proxy users to see what others do
disableLocalInterface = true

# no disk cache
diskCacheRoot = ""

# no local web server
localDocumentRoot = ""

# make sure we are private
disableIndexing = true
disableServersList = true

# avoid ipv6 to go faster
dnsQueryIPv6 = no

# Uncomment this to disable Polipo's DNS resolver and use the system's
# default resolver instead.  If you do that, Polipo will freeze during
# every DNS query: (TODO: test and benchmark)
# dnsUseGethostbyname = yes
dnsNameServer = $dowse

daemonise = true

# to be specified by caller module
# pidFile = $DIR/log/polipo.pid
# logFile = $DIR/log/polipo.log

# to be tested
# disableVia=false
# censoredHeaders = from, accept-language
# censorReferer = maybe

# Uncomment this if you're paranoid.  This will break a lot of sites
# censoredHeaders = set-cookie, cookie, cookie2, from, accept-language
# censorReferer = true
EOF

}


polipo_start() {
    act "Preparing to launch polipo..."
    # if running, stop to restart
    polipo_stop $2
    setuidgid $dowseuid polipo -c "$1"
}

polipo_stop() {
    { test -r "$1" } && {
        pid=`cat $1`
        act "Stopping polipo ($pid)"
        kill $pid
        waitpid $pid
        rm -f "$1"
    }
}
