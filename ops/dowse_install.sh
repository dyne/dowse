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
blend_packages=($(curl -Ls https://github.com/dyne/dowse/raw/master/.travis.yml | awk '/no-install-recommends/ {for (i=11; i<=NF; i++) print $i}'))


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

#cat <<EOF > /etc/rc.local
##!/bin/sh -e
#
#sudo -u dowse "zsh -f 'source /usr/local/dowse/zshrc && dowse-start'" &
#
#exit 0
#EOF
#
#chmod +x /etc/rc.local
