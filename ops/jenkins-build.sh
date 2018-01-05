#!/bin/sh
# This is executed as root when jenkins chroots

cd /home/dowse/dowse-src

sudo -u dowse git clean -xdf && \
sudo -u dowse git pull origin master && \
sudo -u dowse make && \
mkdir -p destbuild && \
make DESTDIR=/home/dowse/dowse-src/destbuild install
