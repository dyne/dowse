#!/bin/sh

if [ ! -e dowse ]; then
	echo "Please call this script from dowse root" && exit 1
fi

if [ $(id -u) != 0 ]; then
	echo "Please call this script with root privileges" && exit 1
fi

set -e

# Use current user to run dowse
export DOWSE_USER=${DOWSE_USER-`id -nu`}
export DOWSE_HOME=${DOWSE_HOME-`pwd`}
export DOWSE_PREFIX=${DOWSE_PREFIX-/usr/local/dowse}
export DOWSE_DNSCRYPT=${DOWSE_DNSCRYPT-$DOWSE_HOME/dnscrypt_proxy}

mkdir -p /var/log/dowse
chown root:dowse /var/log/dowse

./utils/install_dependencies.sh
./utils/install_dnscrypt.sh

make && make install

echo "source $DOWSE_PREFIX/dowse/zshrc" > $DOWSE_HOME/.zshrc

