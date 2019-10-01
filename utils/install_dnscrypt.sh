#!/bin/sh

set -e

git clone https://github.com/dyne/dnscrypt-proxy
cd dnscrypt-proxy && ./autogen.sh && ./configure --enable-plugins --disable-systemd && make && make install
cp src/proxy/dnscrypt-proxy /usr/bin/
mkdir -p /usr/local/dowse/share && cp dnscrypt-resolvers.csv /usr/local/dowse/share/
