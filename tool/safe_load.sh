#!/bin/bash

if [ $(whoami) != "root" ] ; then
    echo "This toy need root privileges"
    exit -1
fi

while true ; do
    uptime |awk '{print $8}' | sed 's/,//'| while read L ; do
				   R=$( echo "$L > 2" |bc -l)
				   if [ "$R" == "1" ]; then
				       echo "Grande $L "
				       PID=$(pidof webui)
				       echo "try to stop $PID"
				       kill -11 ${PID}
				       echo "....done"
				   else
				       T=$(date +%s)
				       echo "$T : Piccolo $L"
				   fi
			       done
    sleep 1
done
