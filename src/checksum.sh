#!/usr/bin/env zsh

R=${$(pwd)%/*}
[[ -r $R/src ]] || {
    print "error: config.sh must be run from the src"
    return 1
}

zkv=1
source $R/zlibs/zuper
vars=(tmp)
maps=(execmap execsums execrules pkgmap)
source $R/zlibs/zuper.init

source paths.sh
zkv.save execmap $R/src/execmap.zkv
zkv.save pkgmap $R/src/pkgmap.zkv

deb-download() {
    fn deb-download $*
    deb="$1"
    req=(deb tmp)
    ckreq || return 1

    [[ $? = 0 ]] || {
        error "cannot create temporary directory"
        return 1 }

    pushd $tmp > /dev/null

    apt-get -q download $deb
    [[ $? = 0 ]] || {
        error "error downloading $deb"
        return 1 }

    debfile=`find . -name "${deb}_*.deb"`

    popd > /dev/null

    freq=($tmp/$debfile)
    ckreq || return 1

    act "extracting $R/tmp/$debfile"
    dpkg -x $tmp/$debfile $tmp
    [[ $? = 0 ]] || {
        error "error extracting $tmp/$debfile"
        return 1 }

    return 0
}

# Build needed program from source
source-build() {
	fn source-build $*
	pkg="$1"
	req=(pkg tmp)
	ckreq || return 1

	[[ $? = 0 ]] || {
		error "cannot create temporary directory"	
		return 1
	}

	pushd $tmp > /dev/null

	wget ${pkgmap[$pkg]}
	[[ $? = 0 ]] || {
		error "error downloading $pkg"	
		return 1
	}

	tar xf $pkg-*.tar.gz && \
	rm $pkg-*.tar.gz && \
	pushd $pkg-*

	if [[ -f configure ]]; then
		./configure && \
			make
	else
		make
	fi
}

act "generating execution rules"

builduid=`id -u`
buildgid=`id -g`
# generate configuration for sup
cat <<EOF > $R/src/sup/config.h

#define ENFORCE 1

#define HASH 1

#ifndef FLAGSONLY

#define USER $builduid
#define GROUP $buildgid

#define SETUID 0
#define SETGID 0

#define CHROOT ""
#define CHRDIR ""

static struct rule_t rules[] = {
EOF


# Build needed programs
if [[ `command -v apt-get` ]]; then
    notice "Importing binary packages from apt repositories..."
    tmp=`mktemp -d`

    [[ -r $execmap[dnsmasq] ]] || {
        act "fetching dnsmasq"
        deb-download dnsmasq-base
        cp -v $tmp/usr/sbin/dnsmasq $R/run
    }

    [[ -r $execmap[redis-server] ]] || {
        act "fetching redis server"
        deb-download redis-server
        cp $tmp/usr/bin/redis-server $R/run 
	}

    [[ -r $execmap[redis-cli] ]] || {
        act "fetching redis tools"
        deb-download redis-tools
        cp $tmp/usr/bin/redis-cli $R/run
    }

    [[ -r $execmap[tor] ]] || {
        act "fetching tor"
        deb-download tor
        cp $tmp/usr/bin/tor $R/run
    }

    rm -rf $tmp
else
	notice "Downloading and building from source..."
	tmp=`mktemp -d`

	[[ -r $execmap[dnsmasq] ]] || {
		act "fetching dnsmasq"
		source-build dnsmasq
		cp -v $tmp/dnsmasq-*/src/dnsmasq $R/run/
	}

	[[ -r $execmap[redis-server] && -r $execmap[redis-cli] ]] || {
		act "fetcing redis"
		source-build redis
		cp -v $tmp/redis-*/src/{redis-cli,redis-server} $R/run/
	}

	[[ -r $execmap[tor] ]] || {
		act "fetching tor"	
		source-build tor
		cp -v $tmp/tor-*/src/or/tor $R/run/
	}

	rm -rf $tmp
fi



notice "Computing checksums to lock superuser privileges"

execsums=()

for x in ${(k)execmap}; do
    [[ "$execrules[$x]" = "root" ]] && {
        cksum=`sha256sum ${execmap[$x]}`
        cksum=${cksum[(w)1]}
        [[ "$cksum" = "" ]] && {
            warning "missing checksum for: $x"
            continue }
        execsums+=($x $cksum)
        act "$cksum $x"
        cat <<EOF >> $R/src/sup/config.h
{ USER, GROUP, "$x", "*", "$cksum" },
EOF
    }
done
zkv.save execsums $R/src/execsums.zkv

cat <<EOF >> $R/src/sup/config.h
{ 0 },
};
#endif
EOF

notice "Dowse build complete on `hostname` (`date`)"
