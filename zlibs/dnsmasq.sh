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

dnsmasq_conf() {
    func "generating dnsmasq configuration"
    cat <<EOF
address=/$hostname/$dowse
address=/$hostname.$lan/$dowse
# address=/.i2p/$dowse
# address=/.onion/$dowse
bogus-priv
cache-size=300
dhcp-range=$dowse_guests
addn-hosts=$dowse_path/run/hosts
dhcp-leasefile=$dowse_path/run/leases
domain-needed
domain=$lan
expand-hosts
edns-packet-max=4096
interface=$interface
listen-address=$dowse,127.0.0.1
local=//127.0.0.1#53
local=/$lan/
user=$dowse_uid
group=$dowse_gid
EOF
    # read the network configuration of known hosts
    known=`cat $dowse_path/conf/network | grep -v '^#'`

    # DNSMasq LAN resolution
    func "Fixing entries for known peers"
    rm -f $dowse_path/run/dnsmasq.network
    # this is basically a dnsmasq host configuration file
    print "dhcp-option=option:router,$dowse" > $dowse_path/run/dnsmasq.network

    # this is our generated hosts file
    func "Generating hosts file"
    rm -f $dowse_path/run/hosts
    print "127.0.0.1 localhost" > $dowse_path/run/hosts
    for i in ${(f)known}; do
	print "$i" | grep '^..:..:..:..:..:..' > /dev/null
	if [ $? = 0 ]; then # mac address is first
	    host=${i[(w)2]}
	    ip=${i[(w)3]}
	else # no mac address specified
	    host=${i[(w)1]}
	    ip=${i[(w)2]}
	fi
	{ test "$host" = "ignore" } || {
	    # add a line to the hosts list
	    print "$ip $host" >> $dowse_path/run/hosts }
    done

    func "generating dnsmask.network"
    for i in ${(f)known}; do
	print "$i" | grep '^..:..:..:..:..:..' > /dev/null
	{ test $? = 0 } || { continue } # skip if no mac address
	func "$i"
	# gather configuration into variables, line by line
	mac=${i[(w)1]}
	host=${i[(w)2]}
	ip=${i[(w)3]}

	# add a line to the dnsmasq host list
	print "dhcp-host=$mac, $host, $ip" >> $dowse_path/run/dnsmasq.network

    done

    # append network settings to dnsmasq conf
    cat $dowse_path/run/dnsmasq.network

}

dnsmasq-start() {
    fn dnsmasq-start
    conf=$1
    shift 1
    freq($conf)
    ckreq

    act "launching dnsmasq"

    func "dnsmasq --pid-file $dowse_path/run/dnsmasq.pid -C $conf $*"
    dnsmasq --pid-file=$dowse_path/run/dnsmasq.pid -C $conf $*
    return $?
}

dnsmasq-stop() {
    fn dnsmasq-stop
    freq=(run/dnsmasq.pid)
    ckreq
    throw
	pid=`cat $dowse_path/run/dnsmasq.pid`

	act "Stopping dnsmasq ($pid)"
	kill $pid
	waitpid $pid
	rm -f $dowse_path/run/dnsmasq.pid

    catch
}
