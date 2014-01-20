#!/bin/zsh

print "Please wait while downloading latest rules..."

if [ -r https-everywhere/.git ]; then
    pushd https-everywhere
    git checkout .
    git pull --rebase
    popd
else
    git clone git://git.torproject.org/https-everywhere.git
    { test $? = 0 } || { print "Error downloading rules."; return 1 }
fi

print "Done."