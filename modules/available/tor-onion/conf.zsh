#!/usr/bin/env zsh

#+MODULE: tor
#+NAME: Tor onion router
#+DESC: Transparent http proxy for .onion urls
#+TYPE: http proxy
#+DEPS: squid-privoxy-polipo
#+INSTALL: tor
#+AUTHOR: Jaromil
#+VERSION: 0.1

require tor

module_setup() {

    # Tor has a dedicated polipo instance
    polipo_conf > $DIR/run/polipo-tor.conf
    cat <<EOF >> $DIR/run/polipo-tor.conf
pidFile = $DIR/run/polipo-tor.pid
logFile = $DIR/log/polipo-tor.log
maxConnectionAge = 5m
maxConnectionRequests = 120
serverMaxSlots = 8
serverSlots = 2
tunnelAllowedPorts = 1-65535
proxyPort = 1337
socksParentProxy = "127.0.0.1:9050"
socksProxyType = socks5
EOF

    cat <<EOF > $DOWSE/run/tor.conf
User $dowseuid
PidFile $DIR/run/tor.pid
SocksPort 127.0.0.1:9050
SocksPolicy accept 127.0.0.1
SocksPolicy accept $dowse
SocksPolicy reject *
Log notice file $DOWSE/log/tor.log
RunAsDaemon 1
DataDirectory $DOWSE/run
ControlPort 9051
CookieAuthentication 1
ExitPolicy reject *:*
EOF

    # add the forward for .onion urls
    cat <<EOF >> $DOWSE/run/privoxy.conf
# pass through tor for urls.onion
forward .onion $dowse:1337 .
EOF
}

module_start() {
    tor -f $DIR/run/tor.conf
    # tor drops privileges to uid set in config
    polipo_start $DIR/run/polipo-tor.conf
}

module_stop() {
    { test -r $DIR/run/tor.pid } && {
	    pid=`cat $DIR/run/tor.pid`
	    act "Stopping tor ($pid)"
	    kill $pid
	    waitpid $pid
	    # pid file is deleted by tor
    }
}
