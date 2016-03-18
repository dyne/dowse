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
        dnscrypt      /usr/sbin/dnscrypt-proxy
        dnscap        $R/src/dnscap/dnscap
        dnsmasq       /usr/sbin/dnsmasq
        redis-cli     /usr/bin/redis-cli
        redis-server  /usr/sbin/redis-server
        iptables      /sbin/iptables
        ebtables      /sbin/ebtables
        sysctl        /usr/sbin/sysctl
        pgl           $R/run/pgl/bin/pglcmd
        libjemalloc   /usr/lib64/libjemalloc.so.2
    )
}

case `uname -m` in
    arm*)
        execmap[libjemalloc]=/usr/lib/arm-linux-gnueabihf/libjemalloc.so
        ;;
esac
