#!/bin/sh

if [ ! -e dowse ]; then
	echo "Please call this script from dowse root" && exit 1
fi

if [ $(id -u) != 0 ]; then
	echo "Please call this script with root privileges" && exit 1
fi

set -e

export DOWSE_DIR=${DOWSE_DIR-`pwd`}

mkdir -p /var/log/dowse
chown root:dowse /var/log/dowse

./utils/install_dependencies.sh

make

echo "source $DOWSE_DIR/dowse" > /var/lib/dowse/.zshrc
echo "DOWSE_DIR=$DOWSE_DIR" > /etc/dowse/dir

