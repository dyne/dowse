[![Dowse logo](http://dowse.equipment/dowse-logo.png)](http://dowse.eu)

A digital rod for local area network rabdomancy

[![software by Dyne.org](https://www.dyne.org/wp-content/uploads/2015/12/software_by_dyne.png)](http://www.dyne.org)

Updates: http://dowse.eu

Whitepaper: https://files.dyne.org/dowse/dowse_whitepaper.pdf

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
https://www.youtube.com/watch?v=vquh3IXcduc

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

Installation and activation takes a few steps and needs root:

1. Download dowse on a GNU/Linux box (we use Debian 7)

	git clone https://github.com/dyne/dowse /usr/src/dowse

2. Install all requirements, here below the list of packages:

```
zsh daemontools iptables ebtables gettext-base procps net-tools
libssl-dev libbind-dev libpcap-dev unzip wget gcc make
libnetfilter-queue-dev
```

3. Run `make`

4. Configure the files in the `conf/` folder: settings and network
   The files are plain text and include documentation in comments.

5. Fire up the startup script **as root**: `sudo ./start.sh`

5.1 Please note that if you are root and inside a ZSh shell, then an
   interactive console is available: do `source dowse conf/settings`
   (or another custom config file) and then proceed launching commands
   prefixed with `dowse-` (tab completion available)

6. Remember to deactivate the DHCP service (Automatic IP
   configuration) on any other object on the network, typically your
   ADSL router.

If all went well now one should be able to connect any device to the
internet as you did before, via Dowse.

# Visualization

The DNs visualization log is produced in `log/dnscap.log` in a custom
format which can be easily processed by `gource`. Assuming one is
connected to a network managed by Dowse running on IP `192.168.0.254`,
then from another PC one can do

```
ssh 192.168.0.254 cat /opt/dowse/log/dnscat.log | gource --log-format custom -
```

To quickly render all the logged DNS activity found in that file. For
a realtime visualization is possible to experiment with gource
arguments, it is also easy to render all into a video file.

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

Dowse is Copyright (C) 2012-2016 by the Dyne.org Foundation

	This source code is free software; you can redistribute it and/or
	modify it under the terms of the GNU Public License as published
	by the Free Software Foundation; either version 3 of the License,
	or (at your option) any later version.

	This source code is distributed in the hope that it will be
	useful, but WITHOUT ANY WARRANTY; without even the implied
	warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	Please refer to the GNU Public License for more details.

	You should have received a copy of the GNU Public License along
	with this source code; if not, write to: Free Software Foundation,
	Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
