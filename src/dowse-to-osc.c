/*  Dowse - Open Sound Control listener for DNS query events
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
 *  Written by Denis Roio aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published
 * by the Free Software Foundation; either version 3 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// liblo
#include <lo/lo.h>

// jemalloc
#include <jemalloc/jemalloc.h>

#include "redis.h"
#include "database.h"

redisContext *redis;
redisReply   *reply;

static int quit = 0;
void ctrlc(int sig) {
    fprintf(stderr,"\nQuit.\n");
    redisFree(redis);
    quit = 1;
}

int main(int argc, char **argv) {

    int err;
    lo_address osc;
    char *dns, *ip, *action, *epoch, *domain, *tld;

    if(argv[1] == NULL) {
        fprintf(stderr, "usage: dns-to-osc osc.URL (i.e: osc.udp://localhost:666/pd)\n");
        exit(0);
    }

    osc = lo_address_new_from_url( argv[1] );
    lo_address_set_ttl(osc, 1); // subnet scope

    connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);


    reply = redisCommand(redis,"SUBSCRIBE dns-query-channel");
    freeReplyObject(reply);

    signal(SIGINT, ctrlc);

    while(redisGetReply(redis,(void**)&reply) == REDIS_OK) {

        dns = strtok(reply->element[2]->str,",");
        if(!dns) continue;
        ip = strtok(NULL,",");
        if(!ip) continue;
        action = strtok(NULL,",");
        if(!action) continue;
        epoch = strtok(NULL,",");
        if(!epoch) continue;
        domain = strtok(NULL,",");
        if(!domain) continue;
        tld = strtok(NULL,",");
        if(!tld) continue;

        // TODO: use a more refined lo_send with low-latency flags
        err = lo_send(osc, "/dowse/dns", "ssss", 
                      ip, (action[0]=='N')?"new":"known", domain, tld);
        if(err == -1)
            fprintf(stderr,"OSC send error: %s\n",lo_address_errstr(osc));
        // just for console debugging
        else
            fprintf(stderr,"/dowse/dns %s %s %s %s\n",
                    ip, (action[0]=='N')?"new":"known", domain, tld);

        fflush(stderr);

        freeReplyObject(reply);
        if(quit) break;
    }

    lo_address_free(osc);

    exit(0);
}
