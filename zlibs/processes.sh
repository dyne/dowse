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

waitpid() {
	# takes a pid
	pid="$1"
	lastnewline=0
	while true; do
		ps -p "$pid" > /dev/null
		if [ $? = 0 ]; then print -n . ; lastnewline=1; sleep 1
		else break; fi
		# todo: timeout with kill -9
	done
	# just because we care to look good on the console
	{ test $lastnewline = 1 } && { print }
}

