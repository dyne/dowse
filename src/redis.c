/*  Dowse - auxilary hiredis functions
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
 * TODO: catch more redis errors
 *
 */

#include <hiredis/hiredis.h>

extern redisContext *redis;
extern redisReply   *reply;



void connect_redis(char *host, int port, int db) {

    fprintf(stderr,"Connecting to redis on %s port %u\n", host, port);
    struct timeval timeout = { 1, 500000 };
    redis = redisConnectWithTimeout(host, port, timeout);
    /* redis = redisConnect(REDIS_HOST, REDIS_PORT); */

    if (redis == NULL || redis->err) {
        if (redis) {
            fprintf(stderr,"Connection error: %s\n", redis->errstr);
            redisFree(redis);
        } else {
            fprintf(stderr,"Connection error: can't allocate redis context\n");
        }
    }
    // select the dynamic database where is dns_query_channel
    reply = redisCommand(redis, "SELECT %u", db);
    // TODO: check if result is OK
    // fprintf(stderr,"SELECT: %s\n", reply->str);
    freeReplyObject(reply);

}
