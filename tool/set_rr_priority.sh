#!/bin/bash


LIST="webui dnscrypt-proxy mysqld pendulum redis-server dhcpd ebtables iptables pgld mosquitto node-red seccrond"

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
 chrt -p 42 $i
done


