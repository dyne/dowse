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

redisContext *redis;
redisReply   *reply;



redisContext *connect_redis(char *host, int port, int db) {
    redisContext   *rx;
    redisReply     *reply;
    fprintf(stderr,"Connecting to redis on %s port %u\n", host, port);
    struct timeval timeout = { 1, 500000 };
    rx = redisConnectWithTimeout(host, port, timeout);
    /* rx = redisConnect(REDIS_HOST, REDIS_PORT); */

    if (rx == NULL || rx->err) {
        if (rx) {
            fprintf(stderr,"Connection error: %s\n", rx->errstr);
            redisFree(rx);
        } else {
            fprintf(stderr,"Connection error: can't allocate redis context\n");
        }
    }
    // select the dynamic database where is dns_query_channel
    reply = redisCommand(rx, "SELECT %u", db);
    // TODO: check if result is OK
    // fprintf(stderr,"SELECT: %s\n", reply->str);
    freeReplyObject(reply);
    return rx;
}
