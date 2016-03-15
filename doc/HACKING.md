# Data format

Internal data formats are geared towards performance and simplicity:
they are omma separated strings where position is semantically
relevant depending from the data type.

Fixed positions:
```
TYPE,IP,ACTION,TIME,...
```

1- `TYPE`: The data type

2- `IP`: The originating object IP

3- `ACTION`: The action performed (type dependent)

4- `TIME`: The time at which the event occurs, in seconds since the Epoch


Below is an updated list of existing types and their actions.

## DNS

Represents the event of a dns query by an object on the net.

```
DNS,IP,ACTION,EPOCH,DOMAIN,TLD[,GROUP]
```

Actions:
- NEW: the domain is visited for the first time
- KNOWN: the domain is known and has been visited before

Arguments:
- DOMAIN: is the domain string to be resolved
- TLD: is the last string of the domain, i.e: .org, .com, .net etc.
- GROUP: optionally, a group the domain belongs (see domain-list)

Sources:
- dnscap (dowse plugin)

## OBJ

Represents the event of establishing contact with a known or unknown network object, joining or leaving the network. Objects can also be found through scans and information about them can be updated.

```
OBJ,IP,{ACTION},EPOCH,MACADDR,HOSTNAME,NEW|KNOWN[,DEVICE,OS,]

ACTION: { JOIN | LEAVE | FOUND | UPDATE }
```

Arguments:
- `MACADDR`: the unique MAC address of the network object
- `HOSTNAME`: the hostname announced by the network object
- `NEW | KNOWN`: wether the object is new or already known
- `DEVICE`: the type of device we guess the object is:
  - `COMPUTER | MOBILE | SENSOR | TV`
- `OS`: the operating system we guess is running on the object

Sources
- dnsmasq (dhcp server)
- nmap (network scanner)

## PGL

Peerguardian is a IP blocklist based firewall manager to protect the
DOWSE network from connecting to known malware sources. It emits
events every time an object tries to connect to a blocked IP

```
PGL,IP,BLOCK,EPOCH,BLOCKEDIP
```

Actions: `BLOCK`


Sources
- pgl (peerguardian blocklist firewall)

# Event channels

In Dowse the 'dynamic' redis database (indexed by `src/database.h`
macros) is gathering a number of live events occurring across its
namespaces, so that clients can subscribe to one or more channels and
handle these events.

Inside the dynamic database the following channels are present:

- dns-query-channel (produced by dnscap/plugin/dowse)
- pgl-info-channel  (produced by pgld.c)

As the listener API shapes up, this namespace may change in the close
future. Here below some examples for commandline use:

## DNS (dns-query-channel)

To subscribe to DNS events from CLI, do from the dowse source root:

```shell
db=`awk '/db_dynamic/ { print $3 }' src/database.h`
cat <<EOF | redis-cli -n $db --raw
SUBSCRIBE dns-query-channel
EOF
```

## PGL (pgl-info-channel)

To subscribe to PeerGuardian's events from CLI:

```shell
db=`awk '/db_dynamic/ { print $3 }' src/database.h`
cat <<EOF | redis-cli -n $db --raw
SUBSCRIBE pgl-info-channel
EOF
```


# Modules

# Daemons
