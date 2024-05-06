# Dowse | network awareness

## A network visualization for raising privacy consciousness in the Internet of Things era

Dowse is an application that makes all internet connections on its network visible in real-time, showing when we connect to .com, .org or other domains, or to the corporate cloud of social networks. This allows us to immediately see how many connections are opened by our personal devices, often without us even knowing, every time we connect to a network.

For more info and downlads go to [Dyne.org/dowse](https://dyne.org/dowse).

## Technical details

Dowse takes control of a LAN by becoming its DHCP server and thereby assigning itself as main gateway and DNS server for all clients.
ISC DHCP and DNSCRYPT-PROXY v2 are used as daemons running on Devuan GNU/Linux.

Components used:

- https://devuan.org
- https://github.com/dyne/dnscrypt-proxy
- https://github.com/dyne/domain-list
- https://www.isc.org/dhcp/
- https://dyne.org/dohd

## Credits

Application design and development: [Jaromil](https://jaromil.dyne.org)

Code contributions software: Luca Greco, Ivan Jelinčić, Nicola Rossi, Andrea Scarpino and Danilo Spinella

DNSCrypt Proxy v2 framework: Frank Denis

Funded by: [SIDN Fonds](https://www.sidnfonds.nl/projecten/dowse) and [NLNet](https://nlnet.nl/project/dowse/)

With thanks to: Federico Bonelli, Rob van Kranenburg

