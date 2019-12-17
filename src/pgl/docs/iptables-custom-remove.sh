#!/bin/sh
# iptables-custom-remove.sh - example custom iptables deletion script

# Every file in the IPTABLES_CUSTOM_DIR directory (/etc/pgl), that ends
# in ...remove.sh will be executed on every "pglcmd stop" for 2 settings:

# Default setup (IPTABLES_SETTINGS="1"):
# pglcmd will first remove its iptables setup, afterwards this script gets
# executed. Note that you don't need to remove custom iptables rules from the
# chains pgl_in, pgl_out and pgl_fwd, since these
# chains get flushed by pglcmd.

# IPTABLES_SETTINGS="2" is set in pglcmd.conf
# (/etc/pgl/pglcmd.conf):
# Only this script will be executed.

# pgl checks traffic that is sent to the iptables target NFQUEUE.
# (default queue number is 92).

# Remove the rules for complete blocking of IPv6:
ip6tables -D OUTPUT -j REJECT
ip6tables -D INPUT -j DROP
ip6tables -D FORWARD -j DROP
