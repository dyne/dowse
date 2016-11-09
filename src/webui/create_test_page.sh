#!/bin/bash

touch src/assets.h
FILE=src/*.c
echo " # Automatically generated "`date`
cproto ${FILE} -I../libparse-datetime -I/usr/include/mysql -I../kore/includes -Isrc/|sed "s/([^;]*;//"|awk '/___webui___test_unit/ {i++; print "static /test/"i" "$2" "}'| while read f; do
	echo $f
done
echo -e "\n}\n"
