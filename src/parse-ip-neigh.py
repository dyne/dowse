#!/usr/bin/env python3

import sys
from time import time
from redis import StrictRedis

RSTOR = StrictRedis(host='127.0.0.1', port=6379, db=2)

table = sys.stdin.readlines()

thing_defaults = {'isadmin': 'no', 'enable_to_browse': 'no'}

for i in table:
    thing = i.split()
    if thing[6] == 'REACHABLE' and len(thing[5]) is 17:
        if RSTOR.exists('thing_%s' % thing[5]):
            RSTOR.hset('thing_%s' % thing[5], 'ip4', thing[0])
        else:
            thing_defaults = {
                'isadmin': 'no',
                'enable_to_browse': 'no',
                'macaddr': thing[5],
                'ip4': thing[0],
                'age': int(time()),
                'last': int(time()),
            }
            RSTOR.hmset('thing_%s' % thing[5], thingdefaults)

