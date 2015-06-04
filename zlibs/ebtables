#!/usr/bin/env zsh

ebtables-start() {
    require ebtables
    { test $? = 0 } || { func "ebtables not found, skipping"; return 1 }

    notice "Setting up ebtables rules (layer 2 firewall)"
    ebtables -P FORWARD DROP
    ebtables -A FORWARD -p IPv4 -j ACCEPT
    ebtables -A FORWARD -p ARP -j ACCEPT
    ebtables -A FORWARD -p LENGTH -j ACCEPT
    ebtables -A FORWARD --log-level info --log-ip --log-prefix EBFW
    ebtables -P INPUT DROP
    ebtables -A INPUT -p IPv4 -j ACCEPT
    ebtables -A INPUT -p ARP -j ACCEPT
    ebtables -A INPUT -p LENGTH -j ACCEPT
    ebtables -A INPUT --log-level info --log-ip --log-prefix EBFW
    ebtables -P OUTPUT DROP
    ebtables -A OUTPUT -p IPv4 -j ACCEPT
    ebtables -A OUTPUT -p ARP -j ACCEPT
    ebtables -A OUTPUT -p LENGTH -j ACCEPT
    ebtables -A OUTPUT --log-level info --log-ip --log-arp --log-prefix EBFW -j DROP

    act "pinning down known MAC addresses to IP"
    for i in ${(f)known}; do
    # check if its a mac address
    echo "$i" | grep '^..:..:..:..:..:..' > /dev/null
    { test $? = 0 } || { continue } # skip if no mac address

    mac=${i[(w)1]}
    host=${i[(w)2]}
    ip=${i[(w)3]}

    { test "$host" = "ignore" } && { continue }

    func "$i"

    ebtables -A FORWARD -p IPv4 --ip-src ${ip} -s ! ${mac} -j DROP

    done

}

ebtables-stop() {
    require ebtables

    act "flushing ebtables (layer 2 firewall)"
    ebtables -P FORWARD ACCEPT
    ebtables -P OUTPUT ACCEPT
    ebtables -P INPUT ACCEPT
    ebtables -F
}
