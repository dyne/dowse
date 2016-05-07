# 0.9
## May 2016
### Refactoring and new features

The internal messaging system is now relying on Redis as a core
component for Dowse. Kore is adopted as web interface, Pendulum is
added as a process to monitor the presence of things on LAN, Netdata
is adopted as status graph dashboard. Events can be exported to
OSC. Developers documentation is complete. This stage of development
has been kindly supported by NLNET and SIDNfonds and will be used as a
base for experimentation at the Live Performers Meeting 2016.

# 0.8
## November 2015
### Fixes and cleanups, DNS graphic visualization

Dnscap is adopted on top of libpcap for DNS traffic analysis and
visualization with a first implementation supporting Gource's
format. The code is being tested in various environments with the
result of fixing instabilities, also a devops setup is now available
to simulate a test LAN using vagrant and ansible. The Consul (GOSSIP)
functionality is suspended until a use case arises. This development
is informed by the Dowse design study kindly sponsored by NLNET.

# 0.7
## June 2015
### Refactoring, GOSSIP networking via Consul

The workflow has been refactored to function as interactive sourced
extension to the running shell. Consul.io is adopted as k/v
communication hub for LAN and WAN networking over GOSSIP. The module
format has been reviewed for better identification. Services, modules
and things discovered are registered on Consul. Zuper is adopted for
internal functions and RESTful communication.


# 0.6
## November 2014
### Fixes, process handling

Process life is handled by Dowse. Commands are available to scan the
known objects from the DHCP leases.

# 0.5
## June 2014
### Module enhancements, whitepaper and code cleanup

The start/stop is now working faster, new safer iptables rules have
been added and overall the code has been reorganized. The
squid/privoxy functionality has been made into a module, preserving
only firewalling and dnsmasq as the core functionalities of Dowse. The
whitepaper is published along the code now, including all
contributions gathered so far. Dowse project applies for CHEST
funding.

# 0.4
## January 2014
### Network model refactoring and module system

The need for a bridge interface was removed: now Dowse works just on a
single physical interface.  Ebtables is adopted for layer 2 mac
address filtering against arp spoofing. A module system is in place
now to activate / deactivate features. The first module supports
dnscrypt-proxy to protect all DNS traffic.
	
# 0.3
### Tor integration and proxy performance fixes

Testing, code refactoring and stability fixes Squid has been tuned to
use reasonable amounts of RAM Tor is now configured and started.

# 0.2
### Renaming and documentation

Several checks for smooth deployement on Debian 7 Renamed to "dowse" -
local area network rabdomancy

# 0.1
### Initial release

Versioning the set of scripts used to build and administer my home
network Working name: ghettobox (from an old thread on the hackmeeting
mailinglist)
