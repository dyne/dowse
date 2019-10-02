#!/bin/sh

DOWSE_PREFIX=${DOWSE_PREFIX-/usr/local/dowse}
DOWSE_DNSCRYPT=${DOWSE_DNSCRYPT-dnscrypt-proxy}

set -e

git clone https://github.com/dyne/dnscrypt-proxy $DOWSE_DNSCRYPT
cd $DOWSE_DNSCRYPT && ./autogen.sh && ./configure --enable-plugins --disable-systemd && make && make install
cp src/proxy/dnscrypt-proxy /usr/bin/
mkdir -p $DOWSE_PREFIX/share && cp dnscrypt-resolvers.csv $DOWSE_PREFIX
