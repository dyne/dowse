#!/bin/sh

if [ $(id -u) != 0 ]; then
	echo "Please call this script with root privileges" && exit 1
fi

export DOWSE_USER=${DOWSE_USER-dowse}
export DOWSE_HOME=${DOWSE_HOME-/home/$DOWSE_USER}
export DOWSE_PREFIX=${DOWSE_PREFIX-/usr/local/dowse}
export DOWSE_DNSCRYPT=${DOWSE_DNSCRYPT-/home/$DOWSE_USER/dnscrypt_proxy}

set -e

useradd -d /home/$DOWSE_USER dowse

cd $DOWSE_HOME
git clone https://github.com/dyne/dowse repository
cd repository
if [ "$DOWSE_TAG" != "" ]; then
	git checkout $DOWSE_TAG
fi
git submodule init
git submodule update --recursive

./utils/install.sh

