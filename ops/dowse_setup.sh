#!/usr/bin/zsh

# simple script to create dowse/conf/settings via ansible
# takes for arguments: address, interface, wan and lan domain

a=192.168.0.254
n=192.168.0.0/24
g=192.168.0.101,192.168.0.199,48h
m=255.255.255.0
i=eth2
w=10.0.2.2
d=8.8.8.8
l=dowse.equipment

# Fill in default Vagrant settings for Dowse
cat <<EOF > /opt/dowse/conf/settings
address=$a
interface=$i
hostname=\$(hostname)
wan=$w
dns=$d
lan=$l
firewall=yes
#
dowse_net=$n
netmask=$m
dowse_guests=$g
EOF

cat <<EOF > /opt/dowse/conf/network
# keep
EOF

# Set Dowse as system-wide DNS
rm -f /etc/resolv.conf
print "nameserver $a" > /etc/resolv.conf

# prepare root to use ZSh by default with Dowse
cat << EOF > /root/.zshrc
pushd /opt/dowse
source dowse
popd
EOF
chsh -s /usr/bin/zsh
