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
# pass .onion urls directly through tor
forward-socks4a .onion 127.0.0.1:9050 .
EOF
}

module_start() {
    tor -f $DIR/run/tor.conf
    # tor drops privileges to uid set in config
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
