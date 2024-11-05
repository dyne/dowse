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

## Setup

Use [this list for dnscrypt-proxy v2](https://download.dnscrypt.info/resolvers-list/v2/public-resolvers.md) to chose a resolver. Pick one supporting DNSCrypt protocol.


Take the `sdns://...` url and decode it using the [dnscrypt stamps online tool](https://dnscrypt.info/stamps/). Make sure the dropbox on the left confirms it is using the DNSCrypt protocol.

Copy and Paste the values obtained in a new dnscrypt-proxy.conf and use it to start:
```
ProviderName    [name here]
ProviderKey     31337... sequence of hex numbers here
ResolverAddress [IP here]
LocalAddress 192.168.0.2:53 (or your local ip and port)
LocalCache on
Daemonize on
Plugin libdcplugin_dowse.so
```

The last line is important because it will load the Dowse plugin, which is a shared library built by this repository and installed in `/usr/local/lib/dnscrypt-proxy/`.

## Credits

Application design and development: [Jaromil](https://jaromil.dyne.org)

Code contributions software: Luca Greco, Ivan Jelinčić, Nicola Rossi, Andrea Scarpino and Danilo Spinella

DNSCrypt Proxy v2 framework: Frank Denis

Funded by: [SIDN Fonds](https://www.sidnfonds.nl/projecten/dowse) and [NLNet](https://nlnet.nl/project/dowse/)

With thanks to: Federico Bonelli, Rob van Kranenburg

