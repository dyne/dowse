#!/usr/bin/env zsh

# Dowse, automatic install script
#
# This script will install all dependencies on a Debian system

# list of programs to be installed:
daemons=(privoxy squid3 dnsmasq tor)
pkgs=(daemontools iptables ebtables)


# installation process for daemons
# (deactivates start at boot)
for i in $daemons; do
	apt-get install $i
	invoke-rc.d $i stop
	update-rc.d -f $i remove
done

# installation of packages
for i in $pkgs; do
	apt-get install $i
done

