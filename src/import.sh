#!/usr/bin/env zsh


S=${$(pwd)%/*}
[[ -r $S/src ]] || {
    print "error: import.sh must be run from the src subdirectory"
    return 1
}



zkv=1
source $S/zuper/zuper
source $S/zuper/zuper.init

fn import $*

mkdir -p $S/build/bin
mkdir -p $S/build/db
mkdir -p $S/debs

deb-download() {
    fn deb-download $*
    deb="$1"
    req=(deb tmp)
    ckreq || return 1

    bckfile=`find $S/debs -name "${deb}_*.deb"`
    if [[ "$bckfile" = "" ]]; then
        apt-get -q download $deb
        [[ $? = 0 ]] || {
            error "error downloading $deb"
            return 1 }

        debfile=`find . -name "${deb}_*.deb"`

        freq=($debfile)
        ckreq || return 1

        act "backupping $debfile"
        mkdir -p $S/debs
        cp $debfile $S/debs/
    else
        debfile=$bckfile
    fi

    act "extracting $debfile"
    dpkg -x $debfile .
    [[ $? = 0 ]] || {
        error "error extracting $tmp/$debfile"
        return 1 }

    return 0
}

pkg-download() {
    fn pkg-download $*
    pkg="$1"
    req=(pkg tmp)
    ckreq || return 1

    bckfile=`find $S/debs -name "${pkg}-*.pkg.tar.xz"`
    if [[ "$bckfile" = "" ]]; then
	# pacman needs root: https://bugs.archlinux.org/task/33369
	#pacman -Swdd --noconfirm --cachedir $S/debs ${pkg}
	cd $S/debs && curl -O -L $(pacman -Spdd ${pkg}) && cd -
        [[ $? = 0 ]] || {
            error "error downloading $pkg"
            return 1 }

        pkgfile=`find $S/debs -name "${pkg}-*.pkg.tar.xz"`

        freq=($pkgfile)
        ckreq || return 1
    else
        pkgfile=$bckfile
    fi

    act "extracting $pkgfile"
    tar -xf $pkgfile -C $tmp
    [[ $? = 0 ]] || {
        error "error extracting $tmp/$pkgfile"
        return 1 }

    return 0
}


[[ "$1" = "" ]] && {
    notice "deb-download() function loaded from import.sh"
    notice "pkg-download() function loaded from import.sh"
    act "first argument name of package to be extracted in $S/debs"
    return 0
}

[[ "$1" = "nmap-macs" ]] && {
	local nmap_macs="https://svn.nmap.org/nmap/nmap-mac-prefixes"
	notice "importing latest nmap MAC database"
	curl -L -o "$S/build/nmap-mac" "$nmap_macs"
	exit 0
}

# Check CPU architecture
target=unknown
case `gcc -v 2>&1 | awk '/^Target:/ { print $2 }` in
	x86_64*) target=x64   ;;
	armv*)   target=armv7 ;;
	*) warning "unknown target architecture, add to src/import.sh"
	   ;;
esac

node-red-addmod() {
	[[ -d "$S/build/node-red/node_modules/$1" ]] || {
		act "installing node-red module: $1"
		pushd $S/build/node-red
		$S/build/nodejs/node_dir/bin/npm install "$1"
		popd
	}
}

[[ "$1" = "node-red" ]] && {
	node_dist=""
	case $target in
		x64)   node_dist="node-v6.10.3-linux-x64" ;;
		armv7) node_dist="node-v6.11.0-linux-armv7l" ;;
		*) error "Unknown machine architecture for nodejs"
		   return 1
		   ;;
	esac

    notice "Import nodejs and npm for node-red dashboard"
    mkdir -p $S/build/nodejs
    pushd $S/build/nodejs
    [[ -r ${node_dist}.tar.xz ]] ||
        curl https://nodejs.org/dist/v6.10.3/${node_dist}.tar.xz \
             -o ${node_dist}.tar.xz
    [[ -d ${node_dist} ]] || {
        tar xf ${node_dist}.tar.xz
        rm -rf node_dir
        mv ${node_dist} node_dir
    }
    popd

    path+=($S/build/nodejs/node_dir/bin)
    rehash
    [[ -r $S/build/node-red/README.md ]] || {
        act "copying node-red source to build location"
        cp -ra $S/src/node-red $S/build/node-red
    }

    [[ -d $S/build/node-red/node_modules/bcrypt ]] || {
        act "installing dependencies for node-red"
        pushd $S/build/node-red
        $S/build/nodejs/node_dir/bin/npm update
        $S/build/nodejs/node_dir/bin/npm install
        popd }

    node-red-addmod node-red-dashboard
    node-red-addmod node-red-contrib-ifttt
    node-red-addmod node-red-contrib-got
    node-red-addmod node-red-contrib-get-hrefs
    node-red-addmod node-red-contrib-get-feeds
    node-red-addmod node-red-contrib-device-stats
    node-red-addmod node-red-contrib-chatbot
    node-red-addmod node-red-ddm
    node-red-addmod node-red-node-pisrf
    node-red-addmod node-red-node-random
    node-red-addmod node-red-node-smooth
    node-red-addmod node-red-node-twitter

    [[ -d $S/build/node-red/node_modules/node-red-dashboard ]] || {
        act "installing module node-red-dashboard"
        NODE_PATH=$S/build/node-red/node_modules \
                 npm install node-red-dashboard
    }

    [[ -d $S/build/node-red/node_modules/node-red-dashboard ]] || {
        act "installing module node-red-dashboard"
        NODE_PATH=$S/build/node-red/node_modules \
                 npm install node-red-dashboard
    }

    [[ -r $S/build/node-red/public/red/red.js ]] || {
        notice "building node-red source"
        pushd $S/build/node-red
        npm run build
        popd
    }
    return 0
}


# Check if Apt based
command -v apt-get >/dev/null && {
    notice "Importing binary packages from apt repositories..."
    tmp=`mktemp -d`
    act "using temporary directory: $tmp"
    pushd $tmp > /dev/null

    # [[ -r $S/build/dnsmasq ]] || {
    #     act "fetching dnsmasq"
    #     deb-download dnsmasq-base
    #     cp $tmp/usr/sbin/dnsmasq $S/build/bin
    # }
    case "$1" in
        redis-server)
            [[ -r $S/build/bin/redis-server ]] || {
                act "fetching redis server"
                deb-download redis-server
                cp $tmp/usr/bin/redis-server $S/build/bin
            }
            ;;
        redis-cli)
            [[ -r $S/build/bin/redis-cli ]] || {
                act "fetching redis cli"
                deb-download redis-tools
                cp $tmp/usr/bin/redis-cli $S/build/bin
            }
            ;;
        isc-dhcp-server)
            [[ -r $S/build/bin/dhcpd ]] || {
                act "fetching ISC dhcp server"
                deb-download isc-dhcp-server
                cp $tmp/usr/sbin/dhcpd $S/build/bin
            }
            ;;

        mysqldb)
            [[ -r $S/build/mysql ]] || {
				### temporary for older de*an version
					mysqlver=5.6
					grep 'jessie' /etc/os-release >/dev/null && mysqlver=5.5
				###
				act "fetching mysqldb sql server"
				deb-download mysql-server-core-$mysqlver
				deb-download mysql-server
				deb-download mysql-client-$mysqlver
				mkdir -p $S/build/mysql/bin
				mkdir -p $S/build/mysql/share
				mkdir -p $S/build/mysql/plugin
				cp -ra $tmp/usr/share/mysql/*      $S/build/mysql/share/
				cp -ra $S/src/maria2redis/lib_mysqludf_redis_v2.so \
				   $S/build/mysql/plugin
				cp -ra $tmp/usr/lib/mysql/plugin/* $S/build/mysql/plugin/
				cp $tmp/usr/sbin/mysqld    $S/build/mysql/bin
				cp $tmp/usr/bin/mysql      $S/build/mysql/bin
				cp $tmp/usr/bin/mysqlcheck $S/build/mysql/bin
				cp $tmp/usr/bin/mysql_install_db $S/build/mysql/bin
				cp $tmp/usr/bin/my_print_defaults $S/build/mysql/bin
            }
            ;;
        mariadb)
            [[ -r $S/build/mysql ]] || {
				act "fetching mariadb sql server"
				# deb-download mariadb-server-10.0
				deb-download mariadb-server-core-10.0
				deb-download mariadb-server-10.0
				deb-download mariadb-client-core-10.0
				mkdir -p $S/build/mysql/bin
				mkdir -p $S/build/mysql/share
				mkdir -p $S/build/mysql/plugin
				cp -ra $tmp/usr/share/mysql/*      $S/build/mysql/share/
				cp -ra $S/src/maria2redis/lib_mysqludf_redis_v2.so \
				   $S/build/mysql/plugin
				cp -ra $tmp/usr/lib/mysql/plugin/* $S/build/mysql/plugin/
				cp $tmp/usr/sbin/mysqld    $S/build/mysql/bin
				cp $tmp/usr/bin/mysql      $S/build/mysql/bin
				cp $tmp/usr/bin/mysqlcheck $S/build/mysql/bin
				cp $tmp/usr/bin/mysql_install_db $S/build/mysql/bin
				cp $tmp/usr/bin/my_print_defaults $S/build/mysql/bin
            }
            ;;
        *)
            error "package not known: $1"
            act "add package extraction procedure to src/import.sh"
            ;;

        # [[ -r $S/build/tor ]] || {
        #     act "fetching tor"
        #     deb-download tor
        #     cp $tmp/usr/bin/tor $S/build/bin
        # }
    esac

    popd
    if [[ $DEBUG = 1 ]]; then
        func "MariaDB packages downloaded in $tmp"
    else
        rm -rf $tmp
    fi

}

# Check if is Arch Linux
command -v pacman >/dev/null && {
    notice "Importing binary packages from pacman repositories..."
    tmp=`mktemp -d`
    act "using temporary directory: $tmp"
    pushd $tmp > /dev/null

    # [[ -r $S/build/dnsmasq ]] || {
    #     act "fetching dnsmasq"
    #     pkg-download dnsmasq
    #     cp $tmp/usr/bin/dnsmasq $S/build/bin
    # }
    case "$1" in
        redis-server)
            [[ -r $S/build/bin/redis-server ]] || {
                act "fetching redis server"
                pkg-download redis
                cp $tmp/usr/bin/redis-server $S/build/bin
            }
            ;;
        redis-cli)
            [[ -r $S/build/bin/redis-cli ]] || {
                act "fetching redis cli"
                pkg-download redis
                cp $tmp/usr/bin/redis-cli $S/build/bin
            }
            ;;
        isc-dhcp-server)
            [[ -r $S/build/bin/dhcpd ]] || {
                act "fetching ISC dhcp server"
                pkg-download dhcp
                cp $tmp/usr/bin/dhcpd $S/build/bin
            }
            ;;

        mariadb)
            [[ -r $S/build/mysql ]] || {
				act "fetching mariadb sql server"
				pkg-download mariadb
				pkg-download libmariadbclient
				pkg-download mariadb-clients
				mkdir -p $S/build/mysql/bin
				mkdir -p $S/build/mysql/share
				mkdir -p $S/build/mysql/plugin
				cp -ra $tmp/usr/share/mysql/*      $S/build/mysql/share/
				cp -ra $S/src/maria2redis/lib_mysqludf_redis_v2.so \
				   $S/build/mysql/plugin
				cp -ra $tmp/usr/lib/mysql/plugin/* $S/build/mysql/plugin/
				cp $tmp/usr/bin/mysqld     $S/build/mysql/bin
				cp $tmp/usr/bin/mysql      $S/build/mysql/bin
				cp $tmp/usr/bin/mysqlcheck $S/build/mysql/bin
				cp $tmp/usr/bin/mysql_install_db $S/build/mysql/bin
				cp $tmp/usr/bin/my_print_defaults $S/build/mysql/bin
            }
            ;;
        *)
            error "package not known: $1"
            act "add package extraction procedure to src/import.sh"
            ;;

        # [[ -r $S/build/tor ]] || {
        #     act "fetching tor"
        #     pkg-download tor
        #     cp $tmp/usr/bin/tor $S/build/bin
        # }
    esac

    popd
    if [[ $DEBUG = 1 ]]; then
        func "MariaDB packages downloaded in $tmp"
    else
        rm -rf $tmp
    fi

}

return 0
