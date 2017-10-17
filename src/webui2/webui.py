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
main webui module
"""

from time import time
from flask import Flask, request, redirect, render_template
from redis import StrictRedis

from config import (redis_host, redis_port, redis_dynamic, redis_storage)


APP = Flask(__name__)

RDYNA = StrictRedis(host=redis_host, port=redis_port, db=redis_dynamic,
                    decode_responses=True)
RSTOR = StrictRedis(host=redis_host, port=redis_port, db=redis_storage,
                    decode_responses=True)


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


@APP.route('/')
def main():
    """
    main routine
    """
    admin_devices = []
    for i in RSTOR.keys('thing_*'):
        if RSTOR.hget(i, 'isadmin') == 'yes':
            admin_devices.append(RSTOR.hgetall(i))

    # XXX: fill there properly
    caller_info = {}
    caller_info['ip'] = request.environ['REMOTE_ADDR']
    caller_info['mac'] = ip2mac(caller_info['ip'])
    caller_info['name'] = RSTOR.hget('thing_%s' % caller_info['mac'], 'name')

    return render_template('welcome.html', admin_devices=admin_devices,
                           cur_info=caller_info)


@APP.route('/things')
def things():
    """
    table and control of things
    """
    thingslist = []
    keys = RSTOR.keys('thing_*')
    for i in keys:
        thingslist.append(RSTOR.hgetall(i))

    caller_info = {}
    caller_info['ip'] = request.environ['REMOTE_ADDR']
    caller_info['mac'] = ip2mac(caller_info['ip'])
    caller_info['name'] = RSTOR.hget('thing_%s' % caller_info['mac'], 'name')
    if RSTOR.hget('thing_%s' % caller_info['mac'], 'isadmin') == 'yes':
        isadmin = True
    else:
        isadmin = False

    cur_state = RSTOR.get('state_all_things')
    party_mode = RSTOR.get('party_mode')

    return render_template('thing_show.html', things=thingslist,
                           isadmin=isadmin, cur_state=cur_state,
                           party_mode=party_mode, cur_info=caller_info)


@APP.route('/modify_things', methods=['POST'])
def modify_things():
    """
    modify thing infos through POST requests
    currently handles thing's name
    """
    # XXX: do validation
    thing_mac = request.form['macaddr']
    thing_name = request.form['name']
    thing_name = thing_name.replace(' ', '_')

    if not thing_mac or not thing_name:
        return '<h1>400 - Bad Request</h1>\n'

    # set it in redis-storage
    RSTOR.hset('thing_%s' % thing_mac, 'name', thing_name)

    return redirect(request.form['url_from'], code=302)


@APP.route('/modify_priv_things', methods=['POST'])
def modify_priv_things():
    """
    modifies thing privileges
    currently handles admin/nonadmin
    """
    # XXX: do validation
    thing_mac = request.form['macaddr']
    if request.form['column'] != 'admin':
        return "<h1>400 - Bad request</h1>\n"

    state = request.form['value']
    if not state:
        return '<h1>400 - Bad request</h1>\n'

    RSTOR.hset('thing_%s' % thing_mac, 'isadmin', state)
    if state == 'yes':
        RSTOR.hset('thing_%s' % thing_mac, 'enable_to_browse', state)

    return redirect(request.form['url_from'], code=302)


@APP.route('/test_admin', methods=['POST'])
def test_admin():
    """
    configures an initial admin device
    """
    thing_mac = request.form['curmac']
    thing_name = request.form['name']
    thing_name = thing_name.replace(' ', '_')
    if not thing_mac or not thing_name:
        return '<h1>400 - Bad request</h1>\n'

    RSTOR.hset('thing_%s' % thing_mac, 'isadmin', 'yes')
    RSTOR.hset('thing_%s' % thing_mac, 'name', thing_name)

    return redirect('/', code=302)


@APP.route('/cmd', methods=['GET', 'POST'])
def cmd():
    """
    executes commands called from the webui
    """

    oper = request.args.get('op')
    if oper == 'THING_OFF' or oper == 'THING_ON':
        macaddr = request.args.get('macaddr')
        ip4 = request.args.get('ip4')
        if not macaddr:
            return 'Missing MAC address in request.\n'
        if not ip4:
            return 'Missing IP address in request.\n'
    else:
        return 'Invalid request.\n'

    caller_info = {}
    caller_info['ip'] = request.environ['REMOTE_ADDR']
    caller_info['mac'] = ip2mac(caller_info['ip'])
    if RSTOR.hget('thing_%s' % caller_info['mac'], 'isadmin') != 'yes':
        return 'You are unauthorized to perform this action.\n'

    RDYNA.publish('command-fifo-pipe', 'CMD,%s,%s,%d,%s' % (caller_info['ip'],
                                                            oper, int(time()),
                                                            macaddr))
    # print('CMD,%s,%s,%d,%s' % (caller_info['ip'], oper, int(time()), macaddr))

    return redirect('/things', code=302)


@APP.route('/websocket')
def websocket():
    """
    renders the websocket example
    """
    caller_info = {}
    caller_info['ip'] = request.environ['REMOTE_ADDR']
    caller_info['mac'] = ip2mac(caller_info['ip'])
    caller_info['name'] = RSTOR.hget('thing_%s' % caller_info['mac'], 'name')

    return render_template('websocket.html', cur_info=caller_info)


if __name__ == '__main__':
    APP.run(host='0.0.0.0', port=8000)
