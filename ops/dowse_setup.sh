#!/usr/bin/zsh

# simple script to create dowse/conf/settings via ansible
# takes for arguments: address, interface, wan and lan domain
a=192.168.0.254
n=192.168.0.0/24
g=192.168.0.101,192.168.0.199,48h
m=255.255.255.0
i=eth1
w=10.0.2.2
d=8.8.8.8
l=dowse.equipment

[[ "$l" = "" ]] && {
    print "Error in arguments to dowse_setup.sh script"
    return 1
}

cat <<EOF > /opt/dowse/conf/settings
address=$a
interface=$i
hostname=\$(hostname)
wan=$w
dns=$d
lan=$l
firewall=no
dowse_uid=proxy
dowse_gid=proxy
#
dowse_net=$n
netmask=$m
dowse_guests=$g
EOF

cat <<EOF > /opt/dowse/conf/network
# keep
EOF

rm -f /etc/resolv.conf
print "nameserver $a" > /etc/resolv.conf
