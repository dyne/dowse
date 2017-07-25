[![Dowse logo](http://dowse.equipment/dowse-logo.png)](http://dowse.eu)

A digital rod for local area network rabdomancy

[![software by Dyne.org](https://www.dyne.org/wp-content/uploads/2015/12/software_by_dyne.png)](https://www.dyne.org)

Updates: http://dowse.eu

Whitepaper: https://files.dyne.org/dowse/dowse_whitepaper.pdf


[![Build Status](https://api.travis-ci.org/dyne/dowse.svg)](https://travis-ci.org/dyne/dowse)

[![Dowse project stats](https://www.openhub.net/p/dowse/widgets/project_thin_badge.gif)](https://www.openhub.net/p/dowse)

# Introduction

Dowse is a **transparent proxy** facilitating the awareness of ingoing
and outgoing connections, from, to, and within a local area network.

Dowse provides a **central point of soft control for all local
traffic**: from ARP traffic (layer 2) to TCP/IP (layers 3 and 4) as
well as application space, by chaining a firewall setup to a
trasparent proxy setup. A core feature for Dowse is that of **hiding
all the complexity** of such a setup.

Dowse is also a **highly extensible platform**: interoperability
between modules is available using Socks4/5, UNIX pipes, local TCP/IP
sockets and port redirection, conforming to specific daemon
implementations. At the core of Dowse is a very portable shell script
codebase implementing a modular plugin architecture that isolates
processes and supports any executable written in any language: Shell,
C, Perl, Python etc.

Dowse is an ongoing development effort rapidly gaining momentum for
its simplicity and usefulness. Here a recent backstage video:

[![The making of Dowse](https://img.youtube.com/vi/wDLyYk_TQtI/0.jpg)](https://www.youtube.com/watch?v=wDLyYk_TQtI)

# Features

  Dowse takes control of a LAN by becoming its DHCP server and thereby
  assigning itself as main gateway and DNS server for all clients. It
  keeps tracks of assigned leases by MAC Address. DNSMasq is the DHCP
  and DNS daemon.

  All network traffic is passed through NAT rules for masquerading.
  HTTP traffic (TCP port 80) can be filtered through a transparent
  proxy using an application layer chain of Squid2 and Privoxy.

  All IP traffic is filtered using configurable blocklists to keep out
  malware, spyware and known bad peers, using Peerguardian2 and Iptables.

  All DNS traffic (UDP port 53) is filtered through Dnscap and
  analysed to render a graphical representation of traffic. It is also
  possible to tunnel it via DNSCrypt-proxy, encrypting all traffic
  (AES/SHA256) before sending it to DNSCrypt.eu or other configurable
  servers supporting this protocol.

  In the future, traffic of all kinds may be transparently proxied for
  monitoring, filtering, and transformation by other applications
  loaded on the Dowse device.

  All daemons are running as a unique non-privileged UID. The future
  plan is to separate them using a different UID for each daemon.

# Installation

Installation and activation takes a few steps, only `make install` needs root:

1. Download dowse on a GNU/Linux box (we use Devuan Jessie)

	git clone https://github.com/dyne/dowse dowse-src
	git submodule update --init

2. Install all requirements, here below the list of packages. To avoid installing more than needed, consider using the `--no-install-recommends` flag in APT or similar for other package managers.

```
zsh iptables ebtables sqlite3 procps gettext-base net-tools autoconf automake libssl-dev libbind-dev libpcap-dev unzip wget gcc g++ make cmake libtool liblo-dev libnetfilter-conntrack3 libnetfilter-queue-dev libsqlite3-dev libjemalloc-dev libseccomp2 libsodium-dev libhiredis-dev libkmod-dev bind9-host bison gawk libevent-dev libjansson-dev asciidoc libldns-dev  libreadline5 libpcre3 libaio1 libfile-mimeinfo-perl libmariadb-client-lgpl-dev cproto xmlstarlet nmap libaprutil1-dev libltdl-dev patch libb64-dev uuid-dev python-redis python-hiredis dnsutils valgrind build-essential libmysqld-dev libapr1 libapr1-dev libaprutil1-dev curl
```

3. Choose which user should be running dowse: your own is fine, or
   eventually create one just for that to separate filesystem
   permissions.

4. As the user of choice, run `make` inside the dowse source

5. As root, run `make install`

6. If necessary edit the files in the `/etc/dowse` folder, especially
   `settings` where it should be indicated the address for the local
   network you like to create.

7. As the dowse user of choice and inside the source, fire up the
   startup script `./start.sh`

Dowse is now running with a web interface on port 80.

To interact with dowse there is also a console with commands prefixed
with `dowse-` (tab completion available). To enter it run zsh without
extensions and source the main script: first type `zsh -f` and press
enter, then type `source /usr/local/dowse/zshrc` and press enter.

If you like the dowse user to have an interactive console every time
it logs in, then do `ln -s /usr/local/dowse/zshrc $HOME/.zshrc`.

If all went well now one should be able to connect any device to the
internet as you did before, via Dowse.

## Embedded ARM devices

Using https://www.devuan.org just compile and install Dowse following
the procedure above. Images are available for a several popular ARM
devices including RaspberryPI2 and 3, BananaPI, Cubieboard etc.

# Starting Dowse

Here below an example start script launching all services in
Dowse. Some can be commented / expunged ad-hoc depending from use
cases, since the only vital functions are `redis-server`, `dhcpd` and
`dnscrypt-proxy`.

```zsh
#/usr/bin/env zsh

source /etc/dowse/settings
source /usr/local/dowse/zshrc

    notice "Starting Dowse"

    # start the redis daemon (core k/v service)
    start redis-server

	notice "Starting all daemons in Dowse"

    # launch the dhcp daemon
    start dhcpd

    # start the dns encrypted tunneling
    start dnscrypt-proxy

    # start the sql database
    start mysqld

    # start web interface
    start webui

	# start the mqtt/websocket hub
	start mosquitto

    # netdata dashboard for the technical status
    start netdata

	# nodejs/node-red
	start node-red

	# start the cronjob handler (with resolution to seconds)
	start seccrond

    notice "Dowse succesfully started"

}
```

Adding the following line one can set up an open network, what we call it "party mode":

```
echo "set party-mode ON" | redis-cli
```

As a good practice, such a script can be launched from `/etc/rc.local` for user dowse using `setuidgid` from the `daemontools` package.

The next is an example on how to stop dowse, for instance from a stop.sh script:

```zsh
#/usr/bin/env zsh

source /usr/local/dowse/zshrc

	notice "Stopping all daemons in Dowse"

	stop seccrond

	stop mosquitto

    stop webui

    stop mysqld

	# stop nodejs/node-red
	stop node-red

    # stop the dashboard
    stop netdata

    # stop the dns crypto tunnel
    stop dnscrypt-proxy

    # stop the dhcp server
    stop dhcpd
    
    # remove the layer 2 firewall rules
    ebtables-stop

    # remove the layer 3 firewall rules
    iptables-snat-off
    iptables-stop

    # restore backup if present
    # [[ -r /etc/resolv.conf.dowse-backup ]] &&  {
    #     mv /etc/resolv.conf.dowse-backup /etc/resolv.conf
    # }

    stop redis-server

    notice "Dowse has stopped running."
```

The scripts above are found in dowse source as `start.sh` and `stop.sh` and can be customised and called from the system at boot. It is also possible to run an interactive console with completion where dowse commands are available using the `console.sh` script. Once in the console all the above start/stop commands and even more internals will be available to be launched interactively.


# Visualization

The DNS visualization is produced in a custom format which can be
easily processed by `gource`. This is the best way to "see dowse
running": if you are running it locally, then install `gource` and do:

```
dowse-to-gource | gource --log-format custom -
```

or from remote:

```
ssh dowse@dowse.it -- dowse-to-gource | gource --log-format custom -
```

Sidenote: dowse-to-gource must be in the user's `$PATH`. To achieve
this, as mentioned above, you can change the user's shell to zsh and do:
`ln -sf /usr/local/dowse/zshrc $HOME/.zshrc`.

This will live render all the DNS activity occurring on your computer
or local network, with the sort of animation that is also showcased on
our website.

One can also experiment with gource arguments and render all the
output of dowse-to-gource into a video file.

# Experimentation

Open Sound Control (OSC) messaging is implemented to interface
low-latency devices that are running on the same network. To start it
one must know the IP address of the device, then do:

```
dowse-to-osc osc.udp://10.0.0.2:999
```

This will start sending OSC messages over UDP to IP 10.0.0.2 port 999

# Development

The main development repository is on https://github.com/dyne/dowse

Inside the `ops` directory an Ansible recipe is found along a ready to
use Vagrant configuration to build two virtual machines (leader and
client) that simulate a LAN to do further testing of Dowse.

```
cd ops
vagrant up
```

Plus the usual vagrant commands. The devops in Dowse is based on
http://Devuan.org and will run two virtual machines connected to each
other, one "leader" running Dowse and serving DHCP, one "client"
connected to it and to the Internet via the leader.

Help with development is welcome, manuals on how to write new modules
and daemons are in the making and there is a sister project to
categorize all domains used by Internet's conglomerates which also
welcomes contributions: https://github.com/dyne/domain-list

# Disclaimer

Dowse development is supported by: NLNET foundation (2015)
                                   SIDNfonds   (2015-2016)

Dowse is Copyright (C) 2012-2016 by the Dyne.org Foundation

	This source code is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

	This source code is distributed in the hope that it will be
	useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	Please refer to the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this source code; if not, write to: Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
