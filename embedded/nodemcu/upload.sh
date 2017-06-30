#!/bin/sh

[ -n "$*" ] || {
	printf "usage: %s [luascript] [luascript] [...]\n" "$0)"
	exit 1
}

for i in $@; do
	sudo ./luatool.py -b 115200 -p /dev/ttyUSB0 -f "$i" -t "$(basename $i)"
done
