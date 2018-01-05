#!/bin/sh
# This is executed as root when jenkins chroots

set -x

cd /home/dowse/dowse-src

git clean -xdf && \
sudo -u dowse git pull origin master && \
sudo -u dowse git submodule update --init --checkout && \
sudo -u dowse make && \
mkdir -p destbuild && \
make DESTDIR=/home/dowse/dowse-src/destbuild install || exit 1

cd destbuild
tar czf /home/dowse/dowse-src/dowse-armhf-"$(date +%Y%m%d)".tar.gz . || exit 1
