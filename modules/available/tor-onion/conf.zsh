command -v tor > /dev/null
{ test $? = 0 } || { print "tor not found."; return 1 }

pid_tor=$DIR/run/tor.pid

module_setup() {
    cat <<EOF > $DOWSE/run/tor.conf
User $dowseuid
PidFile $pid_tor
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

    cat <<EOF >> $DOWSE/run/privoxy.conf
# pass through tor for urls.onion
forward-socks4a .onion 127.0.0.1:9050 .
EOF
}

module_start() {
    tor -f $DIR/run/tor.conf
    # tor drops privileges to uid set in config
}

module_stop() {
    { test -r $pid_tor } && {
	pid=`cat $pid_tor`
	act "Stopping tor ($pid)"
	kill $pid
	waitpid $pid
	# pid file is deleted by tor
    }
}
