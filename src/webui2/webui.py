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
Main webui module
"""

# For subprocess.run
import subprocess
from time import time
from os import environ, getpid
from argparse import ArgumentParser
from flask import (Flask, request, redirect, render_template,
                   make_response)

from config import (RDYNA, RSTOR)
from helpers import (parsetime, sort_things, get_admin_devices, get_caller_info,
                     fill_default_thing, fill_http_headers, ip2mac)


APP = Flask(__name__)


@APP.route('/')
def main():
    """
    Main routine
    """
    admin_devices = get_admin_devices()

    caller_info = get_caller_info(request.remote_addr)
    if caller_info.get('enable_to_browse', 'no') != 'yes' \
        and caller_info.get('isadmin', 'no') != 'yes':
        return redirect('http://dowse.it/captive_portal', code=302)


    return render_template('welcome.html', admin_devices=admin_devices,
                           cur_info=caller_info)


@APP.route('/things')
def things():
    """
    Table and control of things
    """
    thingslist = []
    keys = RSTOR.keys('thing_*')
    for i in keys:
        sinthing = RSTOR.hgetall(i)
        mac = sinthing.get('macaddr')
        if not mac:
            continue  # perhaps log; this means an incomplete thing
        thingslist.append(sinthing)

    thingslist_sorted = sort_things(thingslist)

    thingslist = []
    for i in thingslist_sorted:
        i['last'] = parsetime(i['last'])
        thingslist.append(i)

    caller_info = get_caller_info(request.remote_addr)
    isadmin = caller_info.get('isadmin', 'no')  == 'yes'

    cur_state = RSTOR.get('state_all_things')
    party_mode = RSTOR.get('party_mode')

    return render_template('things.html', things=thingslist,
                           isadmin=isadmin, cur_state=cur_state,
                           party_mode=party_mode, cur_info=caller_info)


@APP.route('/thing_show', methods=['GET'])
def thing_show():
    """
    Shows information about a specific thing
    """
    mac = request.args.get('mac')
    if not mac:
        return 'Invalid MAC address\n'

    caller_info = get_caller_info(request.remote_addr)
    isadmin = caller_info.get('isadmin', 'no')  == 'yes'

    thinginfo = RSTOR.hgetall('thing_%s' % mac)
    return render_template('thing_show.html', thing=thinginfo,
                           cur_info=caller_info, isadmin=isadmin)


@APP.route('/modify_things', methods=['POST'])
def modify_things():
    """
    Modify thing information through POST requests
    Currently handles thing's name
    """
    # XXX: do validation
    thing_mac = request.form['macaddr']
    thing_name = request.form['name']
    thing_name = thing_name.replace(' ', '_')
    thing_ip = RSTOR.hget('thing_%s' % thing_mac, 'ip4')

    act = request.form.get('action')
    if not act:
        return '<h1>400 - Bad Request</h1>\n'

    if act == 'update':
        if not thing_mac or not thing_name:
            return '<h1>400 - Bad Request</h1>\n'

        _ip = RDYNA.get('dns-lease-%s' % thing_name)
        if _ip and ip2mac(_ip) != thing_mac:
            return '<h1>This name already exists and is not yours. Choose another one.</h1>\n'

        # set it in redis-storage
        RSTOR.hset('thing_%s' % thing_mac, 'name', thing_name)

        # set it in redis-dynamic, for reverse dns
        RDYNA.set('dns-lease-%s' % thing_name, '%s' % thing_ip)
    elif act == 'delete':
        if not thing_mac:
            return '<h1>400 - Bad Request</h1>\n'

        thing_name = RSTOR.hget('thing_%s' % thing_mac, 'name')

        # del it from redis-storage
        RSTOR.delete('thing_%s' % thing_mac)

        # del it from redis-dynamic, to free reverse dns
        RDYNA.delete('dns-lease-%s' % thing_name)

        RDYNA.publish('command-fifo-pipe', 'CMD,%s,%s,%d,%s' %
                (thing_ip, 'THING_OFF', int(time()), thing_mac))
    else:
        return '<h1>400 - Bad Request</h1>\n'

    return redirect(request.form['url_from'], code=302)


@APP.route('/modify_priv_things', methods=['POST'])
def modify_priv_things():
    """
    Modifies thing privileges
    This currently handles admin/nonadmin
    """
    caller_info = get_caller_info(request.remote_addr)
    if caller_info.get('isadmin', 'no') != 'yes':
        return 'You are unauthorized to perform this action.\n'

    thing_mac = request.form['macaddr']
    if request.form['column'] != 'admin':
        return '<h1>400 - Bad request</h1>\n'

    state = request.form['value']
    if not state:
        return '<h1>400 - Bad request</h1>\n'

    RSTOR.hset('thing_%s' % thing_mac, 'isadmin', state)

    return redirect(request.form['url_from'], code=302)


@APP.route('/test_admin', methods=['POST'])
def test_admin():
    """
    Configures an initial admin device
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
    RDYNA.set('dns-lease-%s' % thing_name, request.remote_addr)

    RDYNA.publish('command-fifo-pipe', 'CMD,%s,%s,%d,%s' %
                  (request.remote_addr, 'SET_ADMIN', int(time()), thing_mac))

    return redirect('/', code=302)


@APP.route('/reset_admin', methods=['POST'])
def reset_admin():
    """
    Procedure to reset all admin devices
    """
    caller_info = get_caller_info(request.remote_addr)
    if caller_info.get('isadmin', 'no') != 'yes':
        return 'You are unauthorized to perform this action.\n'

    for i in RSTOR.keys('thing_*'):
        RSTOR.hmset(i, {'isadmin': 'no'})

    return redirect('/', code=302)


@APP.route('/captive_portal')
def captive_portal():
    """
    Renders the actual captive portal page
    """
    caller_info = get_caller_info(request.remote_addr)
    if not bool(caller_info):
        definfo = fill_default_thing(request.remote_addr)
        RSTOR.hmset('thing_%s' % definfo['macaddr'], definfo)
        RSTOR.hset('thing_%s' % definfo['macaddr'], 'enable_to_browse', 'no')
        caller_info = definfo

    admin_button = not bool(get_admin_devices())
    resp = make_response(render_template('captive_portal.html',
                                         cur_info=caller_info,
                                         admin_button=admin_button))
    resp.headers = fill_http_headers(resp.headers)
    return resp


@APP.route('/cmd', methods=['GET', 'POST'])
def cmd():
    """
    Executes commands called from the webui
    """
    caller_info = get_caller_info(request.remote_addr)
    if caller_info.get('isadmin', 'no') != 'yes':
        return 'You are unauthorized to perform this action.\n'

    ipb = ''
    macaddr = ''
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
        if caller_info:
            macaddr = caller_info['macaddr']
        else:
            macaddr = request.args.get('macaddr')
    elif oper == 'PARTY_MODE_OFF' or oper == 'PARTY_MODE_ON':
        RSTOR.set('party_mode', oper.split('_')[2].lower())
        return redirect('/things', code=302)
    else:
        return 'Invalid request.\n'

    RDYNA.publish('command-fifo-pipe', 'CMD,%s,%s,%d,%s,%s' %
                  (caller_info['ip4'], oper, int(time()), macaddr, ipb))
    # print('CMD,%s,%s,%d,%s' % (caller_info['ip4'], oper, int(time()), macaddr))

    return redirect('/things', code=302)


@APP.route('/help')
def helppage():
    """
    Renders the help page
    """
    caller_info = get_caller_info(request.remote_addr)

    return render_template('help.html', cur_info=caller_info)


@APP.route('/websocket')
def websocket():
    """
    Renders the websocket example
    """
    caller_info = get_caller_info(request.remote_addr)

    return render_template('websocket.html', cur_info=caller_info,
                           srv=request.host.split(':')[0])


@APP.route('/nmap')
def nmap():
    """
    Renders the nmap scan log
    """
    caller_info = get_caller_info(request.remote_addr)

    return render_template('nmap.html', cur_info=caller_info,
                           srv=request.host.split(':')[0])


@APP.errorhandler(404)
def page_not_found(e):
    """
    HTTP 404 handling function

    Returns actual 404, or directs you to the captive portal, depending if you
    are enabled to browse or should be redirected.
    """
    caller_info = get_caller_info(request.remote_addr)
    if caller_info.get('enable_to_browse', 'no') == 'yes':
        return render_template('404.html', cur_info=caller_info, msg=e), 404

    return redirect('http://dowse.it/captive_portal', code=302)


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument('-p', '--pidfile')
    parser.add_argument('-d', '--debug', action='store_true')
    args = parser.parse_args()

    if args.pidfile:
        with open(args.pidfile, 'w') as pidf:
            pidf.write(str(getpid()))

    dbg = False
    if args.debug:
        dbg = True

    APP.run(host='0.0.0.0', port=80, threaded=True, debug=dbg)
