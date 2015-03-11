#!/usr/bin/env zsh


require() {
    # check if an executable is found in $PATH
    command -v "$1" > /dev/null
    [[ $? = 0 ]] || {
        error "::1:: not found." "$1"
        error "You need it to run dowse, please install it."
        exit 1 }
    # check if there are related zlibs to load
    [[ -r "$DIR/zlibs/${1}.sh" ]] && {
        # some requires will call this source multiple times
        # but this is not such a big overhead after all
        source "$DIR/zlibs/${1}.sh"
        func "loaded zlib for $1"
    }
    return 0
}

isrunning() {
    fn isrunning
    _pid="$1"
    req=(_pid)
    ckreq || return $?

    # TODO: check if pid is running on CYGWIN and OSX
    ps -p $_pid > /dev/null
    return $?
}

killpid() {
    fn killpid
    _pid="$1"
    req=(_pid)
    ckreq || return $?

    if isrunning $_pid; then
        func "sending INT signal to pid: $_pid"
        kill -INT $_pid
        return 0
    else
        func "cannot kill pid not running: $_pid"
        return 1
    fi
}

killpidfile() {
    fn killpidfile
    _pidfile="$1"
    freq=($_pidfile)
    ckreq || return $?

    _pid=`cat "$_pidfile"`
    killpid $_pid # kill INT
    waitpid $_pid # blocking
    rm -f "$_pidfile"
}

waitpid() {
    fn waitpid
    _pid="$1"
    req=(_pid)
    ckreq || return $?

    lastnewline=0
    while true; do
        if isrunning $_pid; then
            print -n . ; lastnewline=1
            sleep 1
        else
            break
        fi
        # todo: timeout with kill -9
    done

    # this because we care to look good on the console
    [[ $lastnewline = 1 ]] && print

}
