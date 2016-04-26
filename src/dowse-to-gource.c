/*  Dowse - Gource listener for DNS query events
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
 * TODO: trap signals for clean quit, catch more redis errors
 * and most importantly establish an internal data structure for dns query
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "redis.h"
#include "database.h"

static char output[MAX_OUTPUT];
static int quit = 0;

void ctrlc(int sig) {
    fprintf(stderr,"\nQuit.\n");
    redisFree(redis);
    quit = 1;
}

int main(int argc, char **argv) {


    connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);

    signal(SIGINT, ctrlc);

    reply = redisCommand(redis,"SUBSCRIBE dns-query-channel");
    freeReplyObject(reply);
    while(redisGetReply(redis,(void**)&reply) == REDIS_OK) {
        char *dns, *ip, *action, *epoch, *domain, *tld, *group;
        if(quit) break;

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
        group = strtok(NULL,","); // optional
        
        // render
        if(!group)
            snprintf(output,MAX_OUTPUT,"%s|%s|%c|%s/%s",
                     epoch,ip,(action[0]=='K')?'M':'A',tld,domain);
        else
            snprintf(output,MAX_OUTPUT,"%s|%s|%c|%s/%s/%s",
                     epoch,ip,(action[0]=='K')?'M':'A',tld,group,domain);
            
        fprintf(stdout,"%s\n",output);
        fflush(stdout);

        freeReplyObject(reply);
    }

    exit(0);
}
