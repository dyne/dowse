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

from time import time, sleep
from flask import Flask, request, redirect, render_template
from redis import StrictRedis

from config import (redis_host, redis_port, redis_dynamic, redis_storage)
from helpers import (ip2mac, parsetime, sort_things)


APP = Flask(__name__)

RDYNA = StrictRedis(host=redis_host, port=redis_port, db=redis_dynamic,
                    decode_responses=True)
RSTOR = StrictRedis(host=redis_host, port=redis_port, db=redis_storage,
                    decode_responses=True)


@APP.route('/')
def main():
    """
    main routine
    """
    admin_devices = []
    for i in RSTOR.keys('thing_*'):
        if RSTOR.hget(i, 'isadmin') == 'yes':
            admin_devices.append(RSTOR.hgetall(i))

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
        sinthing = RSTOR.hgetall(i)
        # sinthing['age'] = parsetime(sinthing['age'])
        # sinthing['last'] = parsetime(sinthing['last'])
        if not sinthing['last']: sinthing['last'] = int(time())
        thingslist.append(sinthing)

    thingslist_sorted = sort_things(thingslist)

    thingslist = []
    for i in thingslist_sorted:
        i['last'] = parsetime(i['last'])
        thingslist.append(i)

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

    return render_template('things.html', things=thingslist,
                           isadmin=isadmin, cur_state=cur_state,
                           party_mode=party_mode, cur_info=caller_info)


@APP.route('/thing_show', methods=['GET'])
def thing_show():
    """
    shows information about a specific thing
    """
    mac = request.args.get('mac')
    if not mac:
        return 'Invalid MAC address\n'

    caller_info = {}
    caller_info['ip'] = request.environ['REMOTE_ADDR']
    caller_info['mac'] = ip2mac(caller_info['ip'])
    caller_info['name'] = RSTOR.hget('thing_%s' % caller_info['mac'], 'name')
    if RSTOR.hget('thing_%s' % caller_info['mac'], 'isadmin') == 'yes':
        isadmin = True
    else:
        isadmin = False

    thinginfo = RSTOR.hgetall('thing_%s' % mac)
    return render_template('thing_show.html', thing=thinginfo,
                           cur_info=caller_info, isadmin=isadmin)


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

    # set it in redis-dynamic, for reverse dns
    thing_ip = RSTOR.hget('thing_%s' % thing_mac, 'ip4')
    RDYNA.set('dns-lease-%s' % thing_name, '%s' % thing_ip)

    return redirect(request.form['url_from'], code=302)


@APP.route('/modify_priv_things', methods=['POST'])
def modify_priv_things():
    """
    modifies thing privileges
    currently handles admin/nonadmin
    """
    caller_info = {}
    caller_info['ip'] = request.environ['REMOTE_ADDR']
    caller_info['mac'] = ip2mac(caller_info['ip'])
    if RSTOR.hget('thing_%s' % caller_info['mac'], 'isadmin') != 'yes':
        return 'You are unauthorized to perform this action.\n'

    thing_mac = request.form['macaddr']
    if request.form['column'] != 'admin':
        return "<h1>400 - Bad request</h1>\n"

    state = request.form['value']
    if not state:
        return '<h1>400 - Bad request</h1>\n'

    RSTOR.hset('thing_%s' % thing_mac, 'isadmin', state)

    return redirect(request.form['url_from'], code=302)


@APP.route('/test_admin', methods=['POST'])
def test_admin():
    """
    configures an initial admin device
    """
    for i in RSTOR.keys('thing_*'):
        if RSTOR.hget(i, 'isadmin') == 'yes':
            return 'An admin device already exists. Sorry.\n'

    thing_mac = request.form['curmac']
    thing_name = request.form['name']
    thing_name = thing_name.replace(' ', '_')
    if not thing_mac or not thing_name:
        return '<h1>400 - Bad request</h1>\n'

    vals = {'isadmin': 'yes', 'name': thing_name}
    RSTOR.hmset('thing_%s' % thing_mac, vals)

    return redirect('/', code=302)


@APP.route('/cmd', methods=['GET', 'POST'])
def cmd():
    """
    executes commands called from the webui
    """
    caller_info = {}
    caller_info['ip'] = request.environ['REMOTE_ADDR']
    caller_info['mac'] = ip2mac(caller_info['ip'])
    if RSTOR.hget('thing_%s' % caller_info['mac'], 'isadmin') != 'yes':
        return 'You are unauthorized to perform this action.\n'

    ipb = ''
    oper = request.args.get('op')
    if oper == 'THING_OFF' or oper == 'THING_ON':
        macaddr = request.args.get('macaddr')
        ipb = RSTOR.hget('thing_%s' % macaddr, 'ip4')
        ip4 = request.args.get('ip4')
        if not macaddr:
            return 'Missing MAC address in request.\n'
        if not ip4:
            return 'Missing IP address in request.\n'
    elif oper == 'ALL_THINGS_OFF' or oper == 'ALL_THINGS_ON':
        macaddr = caller_info['mac']
    elif oper == 'PARTY_MODE_OFF' or oper == 'PARTY_MODE_ON':
        RSTOR.set('party_mode', oper.split('_')[2].lower())
        sleep(2)
        return redirect('/things', code=302)
    else:
        return 'Invalid request.\n'

    RDYNA.publish('command-fifo-pipe', 'CMD,%s,%s,%d,%s,%s' % (caller_info['ip'],
                                                               oper, int(time()),
                                                               macaddr, ipb))
    #print('CMD,%s,%s,%d,%s' % (caller_info['ip'], oper, int(time()), macaddr))
    sleep(2)

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

    return render_template('websocket.html', cur_info=caller_info,
                           srv=request.host.split(':')[0])

@APP.route('/nmap')
def nmap():
    """
    renders the nmap scan log
    """
    caller_info = {}
    caller_info['ip'] = request.environ['REMOTE_ADDR']
    caller_info['mac'] = ip2mac(caller_info['ip'])
    caller_info['name'] = RSTOR.hget('thing_%s' % caller_info['mac'], 'name')

    return render_template('nmap.html', cur_info=caller_info,
                           srv=request.host.split(':')[0])

@APP.errorhandler(404)
def page_not_found(e):
    return render_template('404.html'), 404


if __name__ == '__main__':
    APP.run(host='0.0.0.0', port=8000, debug=True)
