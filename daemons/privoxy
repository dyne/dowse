#!/usr/bin/env zsh

privoxy-conf() {
    # Privoxy configuration template
    func "generating privoxy configuration"
    cat <<EOF
user-manual /usr/share/doc/privoxy/user-manual
confdir /etc/privoxy
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

}

# $1 arg is path to configuration
privoxy-start() {
    fn privoxy-start
    conf=$1
    shift 1
    req=(conf)
    freq=($conf)
    ckreq

    pushd $dowse_path

    # pid-file is not really a privoxy directive
    pidfile=`awk '/^pid-file/ { print $2 }' $conf`
    rm -f $conf.real
    sed 's/pid-file.*//' $conf > $conf.real

    act "starting privoxy: $conf $*"
    if [[ -z $root ]]; then
        privoxy --pidfile $pidfile $conf.real $*
    else
        privoxy --user $dowse_uid --pidfile $pidfile $conf.real $*
    fi
        
    popd
}

# $1 arg is path to pid
privoxy-stop() {
    fn privoxy-stop
    pidfile=$1

    [[ -r $pidfile ]] || {
        warn "privoxy not running"
        return 0
    }

    pushd $dowse_path
    
    pid=`cat $pidfile`
    act "stopping privoxy pid: ::1 pid::" $pid
    kill $pid
    waitpid $pid

    popd
}
