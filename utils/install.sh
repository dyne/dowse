#!/bin/sh

if [ ! -e dowse ]; then
	echo "Please call this script from dowse root" && exit 1
fi

if [ $(id -u) != 0 ]; then
	echo "Please call this script with root privileges" && exit 1
fi

set -e

U=${DOWSE_USER-dowse}
H=${DOWSE_HOME-`pwd`}
R=${DOWSE_PREFIX-/usr/local/dowse}

mkdir -p /var/log/dowse

./utils/install_dependencies.sh
./utils/install_dnscrypt.sh

make && make install

echo "source $R/dowse/zshrc" > $H/.zshrc

