/*  Dowse - hiredis helpers
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
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <dowse.h>

int okredis(redisContext *r, redisReply *res) {
	if(!res) {
		err("redis error: %s", r->errstr);
		return(0);
	} else if( res->type == REDIS_REPLY_ERROR ) {
		err("redis error: %s", res->str);
		return(0);
	} else {
		return(1);
	}
}

redisReply *cmd_redis(redisContext *redis, const char *format, ...) {
	va_list args;

	redisReply *res;
	char command[512];

	va_start(args, format);
	vsnprintf(command, 511, format, args);
	func("cmd_redis: %s", command);
	res = redisCommand(redis, command);
	va_end(args);

	if( okredis(redis, res) )
		return res;
	else
		return NULL;
}

redisContext *connect_redis(char *host, int port, int db) {
	redisContext   *rx;
	redisReply     *reply;
	func("Connecting to redis on %s port %u", host, port);
	struct timeval timeout = { 1, 500 };
	rx = redisConnectWithTimeout(host, port, timeout);
	/* rx = redisConnect(REDIS_HOST, REDIS_PORT); */

	if (rx == NULL || rx->err) {
		if (rx) {
			err("Connection error: %s", rx->errstr);
			redisFree(rx);
			return NULL;
		} else {
			err("Connection error: can't allocate redis context");
		}
	}
	// select the dynamic database where is dns_query_channel
	reply = cmd_redis(rx, "SELECT %u", db);
	// TODO: check if result is OK
	// fprintf(stderr,"SELECT: %s\n", reply->str);
	if(reply) freeReplyObject(reply);
	return rx;
}

