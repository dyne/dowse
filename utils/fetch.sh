#!/bin/sh

if [ $(id -u) != 0 ]; then
	echo "Please call this script with root privileges" && exit 1
fi

export DOWSE_USER=dowse
export DOWSE_DIR=${DOWSE_DIR-/opt/dowse}

set -e

useradd -d /var/lib/$DOWSE_USER dowse

git clone https://github.com/dyne/dowse $DOWSE_DIR
sudo chown $DOWSE_USER:$DOWSE_USER -R $DOWSE_DIR
cd $DOWSE_DIR
if [ "$DOWSE_TAG" != "" ]; then
	git checkout $DOWSE_TAG
fi
git submodule init
git submodule update --recursive

./utils/install.sh

