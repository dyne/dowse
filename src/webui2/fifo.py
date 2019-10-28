#!/usr/bin/env python3
# Copyright (c) 2019 Dyne.org Foundation
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

from subprocess import run

from config import (RDYNA, RSTOR)
from helpers import (get_caller_info)

def call_pendulum(args):
    # TODO(danyspin97): exec cannot add variables to local scope
    # but only to global scope, fix this to read the value in
    # /etc/dowse/dir
    #exec(open('/etc/dowse/dir').read(), locals())
    #if 'DOWSE_DIR' in locals():
    #    dir = DOWSE_DIR
    #else:
    dir = '/opt/dowse'
    run([dir + '/pendulum'] + args)

def accept_thing(data):
    mac_addr = data[4]
    call_pendulum(['set-thing', 'accept', mac_addr])

def drop_thing(data):
    mac_addr = data[4]
    call_pendulum(['set-thing', 'drop', mac_addr])

def set_admin(data):
    caller_info = get_caller_info(data[0])
    if caller_info.get('enable_to_browse', 'no') == 'yes':
        # Thing is already enabled to browse, do not add any new rule
        return
    accept_thing(data)

def exec_cmd(message):
    if str(message).isnumeric():
        return

    if message[0:3] != 'CMD':
        return

    print('Received command: %s' % message)
    data = message.split(',')
    if data[2] == 'SET_ADMIN':
        set_admin(data)
    elif data[2] == 'THING_ON':
        accept_thing(data)
    elif data[2] == 'THING_OFF':
        drop_thing(data)
    else:
        print("Requested command is not supported")

def parse_fifo():
    pubsub = RDYNA.pubsub(ignore_subscribe_messages=True)
    pubsub.subscribe('command-fifo-pipe')

    last_data = ''
    for message in pubsub.listen():
        if message['data'] == last_data:
            continue
        last_data = message['data']
        exec_cmd(message['data'])

if __name__== "__main__":
      parse_fifo()
