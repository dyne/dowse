#!/usr/bin/env python3

import sys
from time import time
from redis import StrictRedis

RSTOR = StrictRedis(host='127.0.0.1', port=6379, db=2)

table = sys.stdin.readlines()

thing_defaults = {'isadmin': 'no', 'enable_to_browse': 'no'}


def fill_thing_info(thing, router=False):
    if RSTOR.exists('thing_%s' % thing[4]):
        RSTOR.hset('thing_%s' % thing[4], 'ip4', thing[0])
    else:
        enable = 'no'
        if router: enable = 'yes'

        thing_defaults = {
            'isadmin': 'no',
            'enable_to_browse': enable,
            'macaddr': thing[4],
            'ip4': thing[0],
            'age': int(time()),
            'last': int(time()),
        }
        RSTOR.hmset('thing_%s' % thing[4], thing_defaults)


def main():
    for i in table:
        thing = i.split()
        if len(thing) is 6:
            # we prefer that a thing is first reachable, otherwise we fallback to stale
            if thing[5] == 'REACHABLE' and len(thing[4]) is 17:
                fill_thing_info(thing)
            elif thing[5] == 'STALE' and len(thing[4]) is 17:
                fill_thing_info(thing)


if __name__ == '__main__':
    main()
