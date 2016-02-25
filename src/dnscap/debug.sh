#!/usr/bin/env zsh

# run dnscap and its dowse's plugin in debug mode
# to overcome valgrind's problems on dlclose() see:
# http://kristiannielsen.livejournal.com/11783.html
# perhaps we may want to adopt the RTLD_NODELETE approach

interface=${1:-eth0}

make -C plugins/dowse/ clean
make -C plugins/dowse/ dowse-debug.so
mv plugins/dowse/dowse-debug.so dowse.so
sudo valgrind --leak-resolution=med --leak-check=full --track-origins=yes\
     dnscap -1 -i $interface -x . -X in-addr.arpa \
     -P ./dowse.so \
     -o ./debug.log \
     -l ../domain-list/data -4 -q
