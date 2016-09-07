#!/usr/bin/env zsh

rc=${1}/zshrc

[[ -r "$rc" ]] && {
    source "$rc"
    source src/privileges
    print "Setting privileges for caller: $dowse_uid $dowse_gid"
    chown -R $dowse_uid:$dowse_gid "$1"
}
