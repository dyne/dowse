/*  Dowse Spring to MQTT via libmosquitto
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
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

// libmosquitto
#include "mosquitto/lib/mosquitto.h"

#include "redis.h"
#include "database.h"

static char output[MAX_OUTPUT];
static int quit = 0;


redisContext *redis;
redisReply   *reply;

void ctrlc(int sig) {
    fprintf(stderr,"\nQuit.\n");
    redisFree(redis);
    quit = 1;
}

int main(int argc, char **argv) {
    // settings for
    char *channel  = "dns-query-channel";
    char id[25] = "this_should_be_randomized"; //BUGBUG
    char *host  = "127.0.0.1";
    int port    = 1883;
    int keepalive = 60;
    struct mosquitto *mosq;

    char *dns, *ip, *action, *epoch, *domain, *tld, *group;
    long long int hits;



    redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);

    mosquitto_lib_init();
    mosq = mosquitto_new(id, true, NULL);

    if(!mosq) {
        fprintf(stderr, "failed to connect to local mosquitto mqtt server\n");
    }

    signal(SIGINT, ctrlc);

    reply = redisCommand(redis,"SUBSCRIBE dns-query-channel");
    freeReplyObject(reply);
    while(redisGetReply(redis,(void**)&reply) == REDIS_OK) {
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


        hits = atoll(action);

        // // render
        // if(!group)
        //     snprintf(output,MAX_OUTPUT,"%s|%s|%c|%s/%s",
        //              epoch,ip,(hits==1)?'A':'M',tld,domain);
        // else
        //     snprintf(output,MAX_OUTPUT,"%s|%s|%c|%s/%s/%s",
        //              epoch,ip,(hits==1)?'A':'M',tld,group,domain);
        //
        // fprintf(stdout,"%s\n",output);
        // fflush(stdout);
        mosquitto_publish(mosq, NULL, channel, reply->element[2]->len, reply->element[2]->str, 1/*qos*/, false);

        freeReplyObject(reply);
    }
    sleep(1);

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    exit(0);
}
