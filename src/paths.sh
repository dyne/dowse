# map of permissions
execrules=(
    dnscrypt     user
    redis-cli    user
    redis-server user
    nmap         user

    dnsmasq      root
    dnscap       root
    ifconfig     root
    route        root
    iptables     root
    xtables-multi root
    ebtables     root
    sysctl       root
    kill         root
    # TODO: we shouldn't sup a shell wrapper,
    #       rather start pgld directly ourselves.
    pglcmd       root
)

# paths for Devuan
execmap=(
    ifconfig      /sbin/ifconfig
    route         /sbin/route
    dnscrypt      $R/run/dnscrypt-proxy
    dnsmasq       $R/run/dnsmasq
    dnscap        $R/src/dnscap/dnscap
    redis-cli     $R/run/redis-cli
    redis-server  $R/run/redis-server
    tor           $R/run/tor
    kill          /bin/kill
    xtables-multi /sbin/xtables-multi
    ebtables      /sbin/ebtables
    sysctl        /sbin/sysctl
    pglcmd        $R/run/pgl/bin/pglcmd
    libjemalloc   /usr/lib/x86_64-linux-gnu/libjemalloc.so.1
    nmap          /usr/bin/nmap
)

# check if on Gentoo
command -v emerge >/dev/null && {
    execmap=(
		ifconfig      /bin/ifconfig
		route         /bin/route
		dnscrypt      $R/run/dnscrypt-proxy
		dnscap        $R/src/dnscap/dnscap
		dnsmasq       $R/run/dnsmasq
		redis-cli     $R/run/redis-cli
		redis-server  $R/run/redis-server
		tor           $R/run/tor
		kill          /bin/kill
		xtables-multi /sbin/xtables-multi
        ebtables      /sbin/ebtables
        sysctl        /usr/sbin/sysctl
        pgl           $R/run/pgl/bin/pglcmd
        libjemalloc   /usr/lib64/libjemalloc.so.2
		nmap          /usr/bin/nmap
    )
}

pkgmap=(
	dnsmasq      http://www.thekelleys.org.uk/dnsmasq/dnsmasq-2.75.tar.gz
	redis        http://download.redis.io/releases/redis-3.0.7.tar.gz
	tor          https://www.torproject.org/dist/tor-0.2.7.6.tar.gz
)

case `uname -m` in
    arm*)
        execmap[libjemalloc]=/usr/lib/arm-linux-gnueabihf/libjemalloc.so
        ;;
esac
