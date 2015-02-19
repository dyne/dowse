#!/usr/bin/env zsh
#
# Copyright (C) 2012-2015 Dyne.org Foundation
#
# Dowse is designed, written and maintained by Denis Roio <jaromil@dyne.org>
#
# This source code is free software; you can redistribute it
# and/or modify it under the terms of the GNU Public License
# as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later
# version.
#
# This source code is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE.  Please refer to the GNU Public License for more
# details.
#
# You should have received a copy of the GNU Public License
# along with this source code; if not, write to: Free
# Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
# 02139, USA.

# Messaging function with pretty coloring
function _msg() {
    local msg="$2"
    command -v gettext 1>/dev/null 2>/dev/null && msg="$(gettext -s "$2")"
    for i in $(seq 3 ${#});
    do
        msg=${(S)msg//::$(($i - 2))*::/$*[$i]}
    done

    local command="print -P"
    local progname="$fg[magenta]${TOMBEXEC##*/}$reset_color"
    local message="$fg_bold[normal]$fg_no_bold[normal]$msg$reset_color"
    local -i returncode

    case "$1" in
        inline)
            command+=" -n"; pchars=" > "; pcolor="yellow"
            ;;
        message)
            last_act="$msg"
            pchars=" . "; pcolor="white"; message="$fg_no_bold[$pcolor]$msg$reset_color"
            ;;
        verbose)
            last_func="$msg"
            pchars="[D]"; pcolor="blue"
            ;;
        success)
            last_notice="$msg"
            pchars="(*)"; pcolor="green"; message="$fg_no_bold[$pcolor]$msg$reset_color"
            ;;
        warning)
            pchars="[W]"; pcolor="yellow"; message="$fg_no_bold[$pcolor]$msg$reset_color"
            ;;
        failure)
            pchars="[E]"; pcolor="red"; message="$fg_no_bold[$pcolor]$msg$reset_color"
            returncode=1
            ;;
        print)
            progname=""
            ;;
        *)
            pchars="[F]"; pcolor="red"
            message="Developer oops!  Usage: _msg MESSAGE_TYPE \"MESSAGE_CONTENT\""
            returncode=127
            ;;
    esac
    ${=command} "${progname} $fg_bold[$pcolor]$pchars$reset_color ${message}$color[reset_color]" >&2
    return $returncode
}

function _message say act() {
    local notice="message"
    [[ "$1" = "-n" ]] && shift && notice="inline"
    [[ $debug = 1 ]] || _msg "$notice" $@
    return 0
}

function _verbose xxx func() {
    [[ $debug = 1 ]] && _msg verbose $@
    return 0
}

function _success yes notice() {
    [[ $quiet = 1 ]] || _msg success $@
    return 0
}

function _warning no warn warning() {
    [[ $quiet = 1 ]] || _msg warning $@
    return 0
}

function _failure fatal die error() {
    #    typeset -i exitcode=${exitv:-1}
    [[ $quiet = 1 ]] || _msg failure $@
    return 1
}

function _print() {
    [[ $quiet = 1 ]] || _msg print $@
    return 0
}
