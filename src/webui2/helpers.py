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

from operator import itemgetter
from time import time
from werkzeug.datastructures import Headers as werkzeugHeaders
from tldextract import extract

from config import (RDYNA, RSTOR)


def fill_http_headers(respdict):
    """
    Fills a make_response.headers dict with hardcoded headers.
    Wishfully used to make Android figure out a captive portal.
    """
    assert isinstance(respdict, werkzeugHeaders), 'Wrong type of var was fed.'

    respdict['Cache-Control'] = 'no-cache, no-store, must-revalidate, post-check=0, pre-check=0'
    respdict['Pragma'] = 'no-cache'
    respdict['Expires'] = '0'
    return respdict


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


def sort_things(unsorted_things, isadmin):
    """
    Function to sort things by epoch and
    """
    sort_rules = []
    if isadmin:
        sort_rules = itemgetter('ask_permission', 'last')
    else:
        sort_rules = itemgetter('last')
    unsorted_things.sort(key=sort_rules, reverse=True)


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


def fill_default_thing(ip):
    """
    Fill default info for a new thing.

    Used in the 404 handler of webui.py
    """
    thing_defaults = {
        'isadmin': 'no',
        'enable_to_browse': 'no',
        'macaddr': ip2mac(ip),
        'ip4': ip,
        'age': int(time()),
        'last': int(time()),
    }
    return thing_defaults

def get_admin_devices():
    admin_devices = []
    for i in RSTOR.keys('thing_*'):
        if RSTOR.hget(i, 'isadmin') == 'yes':
            admin_devices.append(RSTOR.hgetall(i))

    return admin_devices


def group_stats(stats):
    grouped_stats = {}
    access_stats = {}
    domain_names = {}
    for domain, count in stats.items():
        tld = extract(domain)
        key = tld.registered_domain.replace('.', '')
        if not key in domain_names:
            domain_names[key] = tld.registered_domain
            grouped_stats[key] = {}
            access_stats[key] = 0

        grouped_stats[key][domain] = int(count)
        access_stats[key] += int(count)

    access_stats = sorted(access_stats.items(), key=itemgetter(1), reverse=True)
    sorted_groups = {}
    for domain, group in grouped_stats.items():
        if len(group) == 1:
            sorted_groups[domain] = []
            continue

        sorted_groups[domain] = sorted(group.items(), key=itemgetter(1), reverse=True)

    return domain_names, sorted_groups, access_stats

