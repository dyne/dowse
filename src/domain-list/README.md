# Domain list

A crowdsourced (open to contributions!) list of domains (including CDN domains) belonging to Internet commercial silos i.e. Facebook, Amazon, Adobe etc. but also to indie organizations and groups.

This list is useful for any application willing to filter, block or just recognize DNS queries to the expanding empires of big companies which nowadays accept connections via a variety of hostnames rather than just a domain, be it for CDN/round-robin operations, to distribute load or to deceive clients.

# Organization

The list is organized in files: each file has the name of the conglomerate (i.e. "facebook") and the file contains a newline separated list of domains belonging to the conglomerate.

# Contributing

It's github, just file a pull request!

# How to find out

This is the fun part! We all need to coopearate in order to find out using various techniques, for instance monitoring the DNS queries going out of your home and office, or running rDNS queries on company IP ranges.

Please share your experience, how-to guides and scripts with us here.

# FAQ

> How about a list of IPs?

IPs are another ball of wax. However IP lists are available here https://www.iblocklist.com/lists.php in the form of ranged ip notation that is somehow more complex to parse. The fastest open source parser for those is in portable C++ inside Peerguardian http://sourceforge.net/projects/peerguardian

> What do you intend to do with this list?

The scenarios in which domain-list can be useful may vary. Dyne.org has a project where this data is being used: Dowse http://github.com/dyne/dowse. The whitepaper that explains Dowse is visible on http://dowse.equipment.

> Is this legal?

Of course! all information collected here is already in the public domain: well visible in every connection we make to on-line services, on public documents released by ISC, Whois databases and rDNS lookups. This project just aims to be a point of collection for such information and related scripts.

# Contact

Come over on irc.dyne.org channel #dyne

Also via web: https://irc.dyne.org
