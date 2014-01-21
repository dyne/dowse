        oooo
   ooooo888   ooooooo  oooo  o  oooo oooooooo8   ooooooooo8
 888    888 888     888 888 888 888 888ooooooo  888oooooo8
 888    888 888     888  888888888          888 888
   88ooo888o  88ooo88     88   88   88oooooo88    88oooo888

       a digital rod for local area network rabdomancy

       http://www.dyne.org/software/dowse       version 0.4

Dowse is a minimalistic script that can turn an old GNU/Linux box into
a local network firewall, privacy proxy and gateway to hidden networks
as Tor or I2P.

Installation and activation takes a few steps and needs root:

1- Download dowse on a GNU/Linux box (we use Debian 7)

   # git clone https://github.com/dyne/dowse /usr/src/dowse

2- Install ZSh, needed to run all scripts in Dowse: apt-get zsh
   then go into the dowse directory ( cd /usr/src/dowse in example)

3- Run ./utils/debian-install.sh as root, it fires up some commands:
   apt-get, update-rc.d and invoke-rc.d to install dependencies like
   dnsmasq, privoxy, squid, tor

4- Configure the files in the conf/ folder: settings and network
   The files are plain text and include documentation in comments.

5- Launch the dowse script as root, using full path. In our example:
   # /usr/src/dowse/dowse start
   Dowse will launch all daemons dropping root privileges and using
   the user configured (default user is 'proxy')

6- Deactivate the DHCP service (Automatic IP configuration) on any
   other object on the network, typically your ADSL router.

If all went well now one should be able to connect any device to the
internet as you did before, but now all the traffic is passing via
Dowse's transparent proxy configuration, which weeds out adverts and
takes care of browser's privacy.

To make sure that dowse is started at every boot, just add it to the
/etc/rc.local file, in our example that would be the line:

 /usr/src/dowse/dowse start


* Recommended

Here below a list of software we recommend using with dowse:

 - PeerGuardian http://sourceforge.net/projects/peerguardian/
	maintains blocklists on the firewall

 - Tiger http://savannah.nongnu.org/projects/tiger/
	audits the system's security

 - SshGuard http://sshguard.sf.net
	bans failed attempts to log into ssh

 - MiniUPNPc http://miniupnp.free.fr
	setup port forwardings on gateway routers
