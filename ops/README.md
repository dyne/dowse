
To use the Vagrant environment (that we created with a lot of <3 ) you should disable the Vangrant DHCP server, with :

VBoxManage dhcpserver remove --netname HostInterfaceNetworking-vboxnet0
