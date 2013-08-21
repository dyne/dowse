
Dowse is a minimalistic script that can turn an old GNU/Linux box
into a local network firewall, privacy proxy and gateway to hidden
networks as Tor or I2P.

It takes a few steps to use:

1- Install it on a GNU/Linux box, Debian 7 OS is recommended

2- Install dependencies: dnsmasq, privoxy, squid, tor

3- Configure the files in the conf/ folder: settings and network

4- Launch the dowse script in its own source directory

5- Make sure no other DHCP server is running (ADSL routers etc.)

6- (only this step requires root) run 'dowse start'



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
