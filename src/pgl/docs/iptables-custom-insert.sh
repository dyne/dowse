#!/bin/sh
# iptables-custom-insert.sh - example custom iptables insertion script

# Every file in the IPTABLES_CUSTOM_DIR directory (/etc/pgl), that ends
# in ...insert.sh will be executed on every "pglcmd start" for 2 settings:

# Default setup (IPTABLES_SETTINGS="1"):
# pglcmd will first insert its iptables setup and afterwards this script
# gets executed.

# IPTABLES_SETTINGS="2" is set in pglcmd.conf
# (/etc/pgl/pglcmd.conf):
# Only this script will be executed.

# The IP block daemon checks traffic that is sent to the iptables target NFQUEUE
# (default queue number is 92).

# Examples for IPTABLES_SETTINGS="1". This is advanced stuff, have a look at
# `man iptables` to understand it. You need both kernel support and a
# complimentary user-space module for these things to work. With other words,
# both netfilter (iptables) and the kernel must support what you want - so it
# depends on your distribution if this will be the case.

# Whitelist outgoing TCP traffic to port 80 for the application firefox-bin:
# My system doesn't support this, feedback welcome ;-)
iptables -I pgl_out -p tcp --dport 80 -m owner --pid-owner [processid] -j RETURN
iptables -I pgl_out -p tcp --dport 80 -m owner --cmd-owner firefox-bin -j RETURN

# Whitelist outgoing TCP traffic on port 80 if it does not belong to user
# [username]. This may be used e.g. if there are normal desktop users (who are
# allowed to surf the web without being controlled), while the
# applications whose complete traffic shall be checked are run by the separate
# user [username].
iptables -I pgl_out -p tcp --dport 80 -m owner ! --uid-owner [username] -j RETURN

# Whitelist outgoing TCP traffic on port 80 if it does belong to user
# [username]. This may be used e.g. if there is a normal desktop user [username]
# (who is allowed to surf the web without being controlled), while
# the applications whose complete traffic shall be checked are run by separate
# users.
iptables -I pgl_out -p tcp --dport 80 -m owner --uid-owner [username] -j RETURN

# Whitelist outgoing TCP traffic on port [port] to the destination iprange [x.x.x.x-y.y.y.y].
# Be careful with the syntax: there must be no spaces in the iprange!
#
# Example: Your mail reader fails to get mails because pgl blocked it.
# "tail -f /var/log/syslog.log" shows that the blocked IPs are 72.14.192.73
# and 72.14.192.75. To find out the destination port, set
# LOG_IPTABLES="LOG --log-level info" in /etc/pgl/pglcmd.conf and
# do a "pglcmd restart". "sudo tail -f /var/log/syslog" shows that the
# blocked packets had the destination port 993. "whois 72.14.192.73" shows that
# the range 72.14.192.0 - 72.14.255.255 is assigned to the same entity. You feel
# safe to accept all outgoing TCP traffic on port 993 to this range so you do:
iptables -I pgl_out -p tcp --dport 993 -m iprange --dst-range 72.14.192.0-72.14.255.255 -j RETURN

# You may also insert rules for IPv6. Please note that neither the block
# daemons, nore any known blocklist supports IPv6 currently.
# Remember to remove these rules after usage in iptables-custom-remove.sh
# Block IPv6 completely:
ip6tables -I OUTPUT -j REJECT
ip6tables -I INPUT -j DROP
ip6tables -I FORWARD -j DROP
