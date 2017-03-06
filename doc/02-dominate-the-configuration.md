# Dowse Configuration

To properly configure Dowse, we suggest to keep in mind the role of any configuration file.

They are:
* `/etc/dowse/settings`
* `/etc/dowse/network`

## /etc/dowse/settings

In this file you can configure all parameter that characterize the Dowse box, to instance the value you should use the typical syntax "`_key_=_value_` and the hash `#` to comment .

The parameter more frequently configured are :
- _interface_ : it's the network interface to expose the internal dowse network.
- _address_ : it's the dowse host address.
- *dowse_net* : it's the dowse network mask.
- *dowse_guests* : it's DHCP server parameter to distribute the address (it's expressed in the form _start address_,_end address_,_lease time_.
- _wan_ : it's the network address behing to masquerade the network.
- *internet_interface* : it's the network interface should be routed to provide internet access.
- _dns_ : it's the external DNS providing dns query resolution services.

# First navigation

1) You should connect to :
```
http://www.dowse.it/
```

and you look an web page like this:

![](img/01.png)

Click on "Configure it now" from the device that you want to grant admin privileges.

go to

```
www.dowse.it/captive_admin
```

At the page bottom you are going to find a panel like:
![](img/02.png)

describing the event occured.
In this case your device has tried to browse the internet 4 times.

Click on `Ok` to enable the device to navigate onn the network.
