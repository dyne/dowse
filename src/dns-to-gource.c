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

#include "redis.h"
#include "database.h"


int main(int argc, char **argv) {

    connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);

    reply = redisCommand(redis,"SUBSCRIBE dns_query_channel");
    freeReplyObject(reply);
    while(redisGetReply(redis,(void**)&reply) == REDIS_OK) {
        fprintf(stdout,"%s\n",reply->element[2]->str);
        fflush(stdout);
        freeReplyObject(reply);
    }
    redisFree(redis);

    exit(0);
}
