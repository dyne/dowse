#!/bin/bash


LIST="webui dnscrypt-proxy mysqld pendulum redis-server dhcpd ebtables iptables pgld mosquitto node-red seccrond netdata"

PIDLIST=""
for i in $LIST ; do
    R=`pidof $i| tr '  ' ' '`
    printf "%s\t\t\t-> [%s]\n" $i "$R"
    if [ -z "$R" ] ; then
	continue;
    fi
    if [ -z "$PIDLIST" ] ; then
	PIDLIST="$R"
    else
	PIDLIST="$PIDLIST $R"
    fi
done

echo ""
for i in $PIDLIST ; do
 ls -ld /proc/$i ;
done

echo ""
#set -x
ps -p "$PIDLIST" -o pid,cmd,command,pcpu,pmem,state,nice,pri,rtprio,sched,sgi_p,policy,wchan=WIDE-WCHAN-COLUMN,etime

