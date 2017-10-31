#!/usr/bin/env python3
# Copyright (c) 2017 Dyne.org Foundation
# webui is written and maintained by Ivan J. <parazyd@dyne.org>
#
# This file is part of Dowse
#
# This source code is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this source code. If not, see <http://www.gnu.org/licenses/>.

"""
webui helper module
"""

from time import time

from config import (RDYNA, RSTOR)


def ip2mac(ipaddr):
    """
    Returns MAC address mapped to the given IP
    """
    # XXX: what do do on dual entries there one mac has multiple ips
    arptable = open('/proc/net/arp').read()
    arptable = arptable.split('\n')
    arptable.pop(0)  # removes header
    pairs = {}
    for i in arptable:
        line = i.split()
        if line:
            pairs[line[0]] = line[3]
    return pairs.get(ipaddr, 'n/a')


def sort_things(unsorted_things):
    """
    Function to sort things by epoch
    """
    return sorted(unsorted_things, key=lambda k: k['last'], reverse=True)


def parsetime(then):
    """
    Parse epoch to return human-readable time
    """
    # redis returns us a string
    if not isinstance(then, int):
        then = int(then)

    ds = int(time()) - then
    dm = int(ds / 60)

    def f(x):
        if int(x) > 1:
            return 's'
        return ''

    if ds < 60:
        return '%d second%s ago' % (ds, f(ds))
    elif dm < 60:
        return '%d minute%s ago' % (dm, f(dm))
    elif dm < (24 * 60):
        return '%d hour%s ago' % (int(dm / 60), f(dm/60))
    elif dm < (24 * 60 * 7):
        return '%d day%s ago' % (int(dm / (60*24)), f(dm/(60*24)))
    elif dm < (24 * 60 * 31):
        return '%d week%s ago' % (int(dm / (60*24*7)), f(dm/(60*24*7)))
    elif dm < (24 * 60 * 365.25):
        return '%d month%s ago' % (int(dm / (60*24*30), f(dm/60*24*30)))
    return '%d year%s ago' % (int(dm / (60*24*365)), f(dm/(60*24*365)))


def get_caller_info(ip):
    """
    Returns a dictionary with the information of a request origin
    """
    if not ip:
        return None
    return RSTOR.hgetall('thing_%s' % ip2mac(ip))
