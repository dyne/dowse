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
