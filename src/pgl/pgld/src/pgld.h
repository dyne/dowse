// pgld - netfilter blocking daemon
//
// Copyright (C) 2011 Dimitris Palyvos-Giannas <jimaras@gmail.com>
// Copyright (C) 2009 Cader <cade.robinson@gmail.com>
// Copyright (C) 2008 Jindrich Makovicka <makovick@gmail.com>
// Copyright (C) 2004 Morpheus <ebutera@users.berlios.de>
//
// This file is part of pgl.
//
// pgl is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pgl is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pgl.  If not, see <http://www.gnu.org/licenses/>.


#ifndef INC_PGLD_H
#define INC_PGLD_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <linux/netfilter_ipv4.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

#include "blocklist.h"
#include "parser.h"

#ifdef HAVE_DBUS
#include <dlfcn.h>
#include "dbus.h"
#endif


#define RECVBUFFSIZE 1500
#define PAYLOADSIZE 24
// http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xml
#define ICMP    1
#define TCP     6
#define UDP     17

#define NIPQUAD(addr) \
((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]

#define CHECK_OOM(ptr)                                      \
    do {                                                    \
        if (!ptr) {                                         \
            do_log(LOG_CRIT, "Out of memory in %s (%s:%d)", \
            __func__, __FILE__, __LINE__);                  \
            exit (-1);                                      \
        }                                                   \
    } while(0);                                             \

void do_log(int priority, const char *format, ...);
void do_log_xdbus(int priority, const char *format, ...);
void int2ip (uint32_t ipint, char *ipstr);

#endif /* INC_PGLD_H */
