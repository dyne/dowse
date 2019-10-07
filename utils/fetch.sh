#!/bin/sh

if [ $(id -u) != 0 ]; then
	echo "Please call this script with root privileges" && exit 1
fi

export DOWSE_USER=dowse
export DOWSE_DIR=${DOWSE_DIR-/opt/dowse}

if [ "$DOWSE_TAG" != "" ]; then
	params="--branch $DOWSE_TAG"
fi

set -e

useradd -d /var/lib/$DOWSE_USER dowse

git clone https://github.com/dyne/dowse $DOWSE_DIR --recurse-submodules $params
sudo chown $DOWSE_USER:$DOWSE_USER -R $DOWSE_DIR
sudo chown $DOWSE_USER:$DOWSE_USER -R $DOWSE_DIR/.git
cd $DOWSE_DIR

./utils/install.sh

