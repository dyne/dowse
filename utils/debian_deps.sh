#!/usr/bin/env zsh

# Dowse, automatic install script
#
# This script will install all dependencies on a Debian system

is_deb_installed() {
    deb=$1
    [[ "$deb" = "" ]] && {
        print "error: is_deb_installed called without argument"
        return 1 }

    dpkg-query -W --showformat '${Status}' "$deb" \
               2>/dev/null >/dev/null

    return $?
}

am_i_root() {
    [[ $EUID = 0 ]] && return 0
    return 1
}

install_policy() {
    # deactivate start at boot
    # since Dowse will control daemons
    cat <<EOF > /usr/sbin/policy-rc.d
#!/bin/sh

# naive script by Evgeni Golov to avoid launching daemons
# on install, see Debian Bug 804018
# https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=804018#35

# doc
#  either "echo NO_START_SERVICES=1 > /etc/policy-rc.conf"
#  or "echo NO_START_SERVICES=1 > /etc/policy-rc.d/\$service"
#  or FORCE_NO_START_SERVICES=1 dpkg -i ...

NAME=policy-rc
NO_START_SERVICES=0

# load system settings
[ ! -f /etc/\${NAME}.conf ] || . /etc/\${NAME}.conf

# load package specific settings
[ ! -f /etc/\${NAME}.d/\$1 ] || . /etc/\${NAME}.d/\$1

if [ \${NO_START_SERVICES} -eq 1 ] && [ -n "\${DPKG_RUNNING_VERSION}" ]; then
  exit 101
elif [ -n "\${FORCE_NO_START_SERVICES}" ]; then
  exit 101
else
  exit 104
fi
EOF
    chmod a+x /usr/sbin/policy-rc.d
    mkdir -p /etc/policy-rc.d

    print "NO_START_SERVICES=0" > /etc/policy-rc.conf
}

no_start_policy() {
    deb=$1
    [[ "$deb" = "" ]] && {
        print "error: no_start_policy called without arguments"
        return 1 }

    print "NO_START_SERVICES=1" > /etc/policy-rc.d/$1
}

### MAIN()

am_i_root || {
    print "Error: install needs to be run as root"
    return 1 }

print "Installing Dowse on `hostname`"
print

# list of programs to be installed:
daemons=(dnsmasq privoxy squid3) # tor etc.
pkgs=(daemontools iptables ebtables gettext-base procps net-tools libssl-dev libbind-dev libpcap-dev unzip wget gcc make)


install_policy

# installation process for daemons
# (deactivates start at boot)
for i in $daemons; do
    is_deb_installed "$i" || {
        no_start_policy $i
        apt-get install -y $i
        update-rc.d -f $i remove
    }
done


# installation of packages
for i in $pkgs; do
    is_deb_installed "$i" || { apt-get install -y $i }
done

print
print "Done installing Dowse!"
