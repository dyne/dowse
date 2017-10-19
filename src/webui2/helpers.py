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

def ip2mac(ipaddr):
    """
    returns mac address mapped to the given ip
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


def parsetime(then):
    # redis returns us a string
    if not isinstance(then, int):
        then = int(then)

    delta_seconds = int(time()) - then
    delta_minutes = int(delta_seconds / 60)

    if delta_seconds < 60:
        return '%d seconds ago' % delta_seconds
    elif delta_minutes < 60:
        return '%d minutes ago' % delta_minutes
    elif delta_minutes < (24 * 60):
        return '%d hours ago' % int(delta_minutes / 60)
    elif delta_minutes < (24 * 60 * 7):
        return '%d days ago' % int(delta_minutes / (60 * 24))
    elif delta_minutes < (24 * 60 * 31):
        return '%d weeks ago' % int(delta_minutes / (60 * 24 * 7))
    elif delta_minutes < (24 * 60 * 365.25):
        return '%d months ago' % int(delta_minutes / (60 * 24 * 30))
    return '%d years ago' % int(delta_minutes / (60 * 24 * 365))
