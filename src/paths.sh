# map of permissions
execrules=(
    dnscrypt     user
    dnsmasq      root
    dnscap       user
    redis-cli    user
    redis-server user
    iptables     root
    ebtables     root
    sysctl       root
)

# paths for Devuan
execmap=(
    dnscrypt      $R/run/dnscrypt-proxy
    dnsmasq       $R/run/dnsmasq
    dnscap        $R/src/dnscap/dnscap
    redis-cli     $R/run/redis-cli
    redis-server  $R/run/redis-server
    tor           $R/run/tor
    iptables      /sbin/iptables
    ebtables      /sbin/ebtables
    sysctl        /sbin/sysctl
    pgl           $R/run/pgl/bin/pglcmd
    libjemalloc   /usr/lib/x86_64-linux-gnu/libjemalloc.so.1

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
