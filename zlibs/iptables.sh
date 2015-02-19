#!/usr/bin/env zsh

iptables-start() {
    notice "Setting up iptables firewall rules"

    iptables -P OUTPUT ACCEPT
    iptables -P INPUT DROP
    iptables -P FORWARD DROP

    func "allow only local loopback"
    iptables -A INPUT -i eth0 -d 127.0.0.1 -j DROP
    iptables -A INPUT -i eth0 -s 127.0.0.1 -j DROP
    iptables -A FORWARD -i eth0 -s 127.0.0.1 -j DROP
    iptables -A FORWARD -i eth0 -d 127.0.0.1 -j DROP
    iptables -A INPUT -s 127.0.0.1 -j ACCEPT
    iptables -A INPUT -d 127.0.0.1 -j ACCEPT

# Allow packets from private subnets
    iptables -A INPUT -s ${dowse_net} -j ACCEPT
    iptables -A FORWARD -i ${interface} -s ${dowse_net} -j ACCEPT

# Allow DHCP service
    iptables -A INPUT -p udp --sport 67:68 --dport 67:68 -j ACCEPT

# Allow incoming pings (can be disabled)
    iptables -A INPUT -p icmp --icmp-type echo-request -j ACCEPT

# Allow services such as www and ssh (can be disabled)
    iptables -A INPUT -p tcp --dport http -j ACCEPT
    iptables -A INPUT -p tcp --dport ssh -j ACCEPT

# Keep state of connections from local machine and private subnets
    iptables -A OUTPUT -m state --state NEW -o eth0 -j ACCEPT
    iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
    iptables -A FORWARD -m state --state NEW -o eth0 -j ACCEPT
    iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT

    notice "Setting up masquerading (NAT)"
    func "setup route towards wired network"
    iptables --table nat --append POSTROUTING \
    --out-interface $interface -j SNAT --to $dowse
    # Testing with dreamer lead to use of -j MASQUERADE without
    # destination and out-interface as external to ADSL router WAN
    # SNAT has better detection of open connections, beware.



    func "defend the network from ipv6"
    ip6tables -F
    ip6tables -P INPUT DROP
    ip6tables -P FORWARD DROP
    ip6tables -P OUTPUT DROP

}

iptables-stop() {
    act "Flushing iptables (layer 3 firewall)"
    iptables -F
    iptables -X
    iptables -t nat -F
    iptables -t nat -X
    iptables -P INPUT ACCEPT
    iptables -P FORWARD ACCEPT
    iptables -P OUTPUT ACCEPT
}
