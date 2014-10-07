#!/usr/bin/env zsh

privoxy_conf() {
    # Privoxy configuration template
    func "generating privoxy configuration"
    cat <<EOF
user-manual /usr/share/doc/privoxy/user-manual
confdir /etc/privoxy
logdir $DIR/log/privoxy.log
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

privoxy_start() {
    act "Preparing to launch privoxy..."

    # if running, stop to restart
    privoxy_stop

    privoxy --user $dowseuid --pidfile $pid_privoxy "$1"
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
