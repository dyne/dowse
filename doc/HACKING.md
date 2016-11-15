# Objects

Known and unknown objects on the network are listed in the things.db sqlite3 database located in `$H/run/things.db`. Each object is contained in an sql table like this:

```
1  macaddr  text primary key
2  ip4      text
3  ip6      text
4  hostname text :: hostname resolved and declared
5  iface    text :: interface to which this object connected last
6  state    text :: connectivity state as reported by ip neigh
7  os       text :: operating system deducted from macaddress magic
8  dhcp     text :: ip assigned to this object by our dhcp
9  gateway  text :: gateway configured when this thing was last seen
10 network  text :: network configured when this thing was last seen
11 notes    text :: user made custom notes about the thing
12 identity text :: user assigned name, marks it as known thing
13 last     date :: last time this object was seen (in UTC)
14 age      date :: first time this object was seen (in UTC)
```

To list stored known and unknown objects from commandline, `source dowse` and then do:

```
print - "SELECT * from found;" | $sql -batch $H/run/things.db
```

## Services

This table links all services found on an object with the object entry

```
macaddr:port  text foreign key
service       text
product       text
version       text
extra         text
notes         text
identity      text
```

# Events

Event data formats are geared towards performance and simplicity.

They are comma separated strings (csv) where position is semantically
relevant depending from the data type.

Fixed positions:
```
{TYPE},IP,ACTION,EPOCH,...

TYPE: { DNS | OBJ | PGL | CMD }
```

 1. `TYPE`: The data type

 2. `IP`: The originating object IP

 3. `ACTION`: The action performed (type dependent)

 4. `EPOCH`: The time at which the event occurs, in seconds since the Epoch

Below is an updated list of existing types and their actions.

## DNS

Channel: `dns-query-channel`

Represents the event of a dns query by an object on the net.

Sources:
- dnscap (dowse plugin)

Format:

```
DNS,IP,HITS,EPOCH,DOMAIN,TLD[,GROUP]

```


Arguments:
- `IP`: IPv4 address where the dns query comes from
- `HITS`: number of hits so far on this domain
- `EPOCH`: time when query occurred, number of seconds since Epoch
- `DOMAIN`: is the domain string to be resolved
- `TLD`: is the last string of the domain, i.e: .org, .com, .net etc.
- `GROUP`: optionally, a group the domain belongs (see domain-list)

Test from CLI:
```shell
db=`awk '/db_dynamic/ { print $3 }' src/database.h`
cat <<EOF | redis-cli -n $db --raw
SUBSCRIBE dns-query-channel
EOF
```


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
  - `COMPUTER | MOBILE | SENSOR | TV ..`
- `OS`: the operating system we guess is running on the object

Sources
- dhcp server


## PGL

Channel: `pgl-info-channel`

Peerguardian is a IP blocklist based firewall manager to protect the
DOWSE network from connecting to known malware sources. It emits
events every time an object tries to connect to a blocked IP

```
PGL,IP,BLOCK,EPOCH,IPBLOCKED
```

Actions: `BLOCK`

Sources
- pgl (peerguardian blocklist firewall)

## CMD

Channel: `cmd-fifo-pipe`

Command is a request for operations that can be executed on
request. Commands end up in a FIFO pipe which functions as Closure of
function calls during runtime: the worker thread will then take out
the commands and execute them one by one, treating the results
accordingly.

```
CMD,IP,{ACTION},EPOCH[,ARGS..]

ACTION: SCAN, ON, OFF
```

Sources
- web interface
- rest api

# Channels

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
