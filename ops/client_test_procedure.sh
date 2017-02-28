#!/usr/bin/env bash

apt-get update
apt-get install -y dnsutils

DEV=eth1
# DH Client
#route del default eth0 && rm -f /var/lib/dhcp/dhclient.eth2.leases && dhclient eth2
route del default eth0 && dhclient -r $eth0
#rm -f /var/lib/dhcp/dhclient.eth2.leases && dhclient eth2

# Reset $DEV file (auto_config: false not works)

ifconfig $DEV down
ifconfig $DEV up

# Use the dhclient exposed by dowse on leader
dhclient $DEV

echo "#DNS dowse query."
dig www.google.it

echo "... if it should be redirect on dowse IP address (192.168.0.254) all works"

