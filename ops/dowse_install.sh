#!/usr/bin/env zsh
# Copyright (c) 2016-2017 Dyne.org Foundation
#
# dowse.blend is maintained by Ivan J. <parazyd@dyne.org>
#
# This file is part of Dowse
#
# This source code is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this source code. If not, see <http://www.gnu.org/licenses/>.

## libdevuansdk build script for Dowse

blend_name="dowse"

## array of dependencies from travis-ci file
blend_packages=(zsh iptables ebtables sqlite3 procps gettext-base net-tools autoconf automake libssl-dev libbind-dev libpcap-dev unzip wget gcc g++ make cmake libtool liblo-dev libnetfilter-conntrack3 libnetfilter-queue-dev libsqlite3-dev libjemalloc-dev libseccomp2 libsodium-dev libhiredis-dev libkmod-dev bind9-host bison gawk libevent-dev libjansson-dev asciidoc libldns-dev  libreadline5 libpcre3 libaio1 libfile-mimeinfo-perl libmariadb-client-lgpl-dev cproto xmlstarlet nmap libaprutil1-dev libltdl-dev patch libb64-dev uuid-dev python-redis python-hiredis dnsutils valgrind build-essential libmysqld-dev libapr1 libapr1-dev libaprutil1-dev)

#($(curl -Ls https://github.com/dyne/dowse/raw/master/.travis.yml | sed --in-place 's/libmysqlclient-dev/libmariadb-client-lgpl-dev/g'| awk '/no-install-recommends/ {for (i=11; i<=NF; i++) print $i}'))

print "executing $blend_name preinst"

useradd -m dowse
echo "dowse:dowse" | chpasswd
chsh -s /bin/zsh dowse

apt-get -qq -y update
apt-get --yes --force-yes upgrade
apt-get --yes --force-yes --no-install-recommends install ${blend_packages}
apt-get clean

cd /home/dowse
sudo -u dowse git clone https://github.com/dyne/dowse.git dowse-src
cd dowse-src
sudo -u dowse git submodule update --init
sudo -u dowse make

make install

printf "source /usr/local/dowse/zshrc\n" > /home/dowse/.zshrc

# Created to compensate mysql-server packages not installed
[[ -d /var/lib/mysql-files ]] || { mkdir -p /var/lib/mysql-files ; chown dowse:dowse /var/lib/mysql-files }

# Put in the rc.local to automatically startup dowse
# TODO FIX? is it needly?
sed --in-place 's|exit 0||' /etc/rc.local

#
cat <<EOF >> /etc/rc.local
sudo -u dowse zsh -f -c '/bin/rm -f $HOME/.dowse/run/*.pid'

sudo -u dowse zsh -f -c 'source /usr/local/dowse/zshrc && dowse-start' &
exit 0
EOF

chmod +x /etc/rc.local

# configure dowse for Vagrant environment
FILE=/etc/dowse/settings.dist
OUT=/etc/dowse/settings
cp $FILE $OUT
sed --in-place 's/interface=lo/interface=eth1/g' $OUT
sed --in-place 's/#internet_interface=eth0/interface=eth0/g' $OUT
sed --in-place 's/wan=127.0.0.1/wan=10.0.2.2/g' $OUT
sed --in-place 's|address=127.0.0.1|address=192.168.0.254|g' $OUT
sed --in-place 's|dowse_net=10.0.0.0/24|dowse_net=192.168.0.0/24|g' $OUT
sed --in-place 's|dowse_guests=10.0.0.101,10.0.0.199,48h|dowse_guests=192.168.0.101,192.168.0.199,48h|g' $OUT
# Todo : deve catturare l'indirizzo su eth0 ?
#sed --in-place 's|wan=127.0.0.1|wan=10.0.2.15|g' $OUT
#sed --in-place 's|dns=$wan|dns=208.67.222.222|g' $OUT

# 
echo '# Starting up dowse...'
sudo -u dowse zsh -f -c 'source /usr/local/dowse/zshrc && dowse-start ' 

echo "...done"
