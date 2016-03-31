#!/usr/bin/env zsh

[[ -r modules/available ]] && {
    mkdir -p "$1"/modules
    cp -ra modules/available "$1"/modules/
    cp modules/README        "$1"/modules/
    mkdir -p                 "$1"/modules/enabled
}
