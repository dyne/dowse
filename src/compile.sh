#!/usr/bin/env zsh

R=`pwd`
R=${R%/*}
[[ -r $R/src ]] || {
    print "error: compile.sh must be run from the source base dir"
    return 1
}

source $R/zuper/zuper
source $R/zuper/zuper.init

PREFIX=${PREFIX:-/usr/local/dowse}
DOWSE_HOME=${DOWSE_HOME:-$HOME}
CFLAGS="-Wall -fPIC -fPIE -O3"
LDFLAGS="-fPIC -fPIE -pie"

# 2nd argument when present is number of threads used building
THREADS=${2:-1}

[[ -x $R/build/bin/$1 ]] && {
    act "$1 found in $R/build/bin/$1"
    act "delete it from build/bin to force recompilation"
    return 0 }

notice "Compiling $1 using ${THREADS} threads"



case $1 in
	log)
		[[ -r $R/src/log/liblog.a ]] && return 0
		pushd $R/src/log
		CFLAGS="$CFLAGS" \
			  LDFLAGS="$LDFLAGS" \
			  make -j${THREADS}
		popd
		;;

	netdata-plugins)
		pushd $R/src/netdata
		make -j${THREADS}
		popd
		;;

    # dnscrypt-proxy)
    #     pushd $R/src/dnscrypt-proxy
	# ## least bloated solution
	# 	git checkout -- src/proxy &&
	# 		patch -NEp1 < $R/src/patches/dnscrypt-noreuseableport.patch &&
	# 		./autogen.sh &&
	# 		./configure --without-systemd --enable-plugins --prefix=${PREFIX} &&
	# 		make -j${THREADS} &&
	# 		install -s -p src/proxy/dnscrypt-proxy $R/build/bin
    #     popd
    #     ;;

    dnscrypt_dowse.so|dnscrypt-plugin)
        pushd $R/src/dnscrypt-plugin
		[[ -r configure ]] || autoreconf -i
		./configure &&
			make -j${THREADS} &&
            install -s -p .libs/dnscrypt_dowse.so $R/build/bin
        popd
        ;;

    pgld)
        pushd $R/src/pgld &&
	    CFLAGS="$CFLAGS" \
		  LDFLAGS="$LDFLAGS" \
		  make -j${THREADS} &&
        install -s -p $R/src/pgld/pgld $R/build/bin
        popd
        ;;

    *)
        act "usage: ./src/compile.sh [ clean ]"
        ;;
esac
