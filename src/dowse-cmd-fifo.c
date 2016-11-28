/*  Dowse - command fifo reader
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
#include <time.h>

// jemalloc
#include <jemalloc/jemalloc.h>

#include <libdowse/dowse.h>

#define CHAN "command-fifo-pipe"


redisContext *redis = NULL;
redisReply   *reply = NULL;


static int quit = 0;
void ctrlc(int sig) {
	fprintf(stderr,"\nQuit.\n");
	if(redis) redisFree(redis);
	if(reply) freeReplyObject(reply);
	quit = 1;
}


int main(int argc, char **argv) {

	signal(SIGINT, ctrlc);

	redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
	if(!redis) {
		err("Dowse is not running");
		exit(1);
	}

	reply = cmd_redis(redis,"SUBSCRIBE %s", CHAN);
	freeReplyObject(reply);

	if(redisGetReply(redis,(void**)&reply) == REDIS_OK) {

		// func("reply type: %u", reply->type);
		if(reply->type == REDIS_REPLY_ARRAY
		   && reply->elements == 3) {
			redisReply *r;
			// channels return arrays
			// 1st 2 arrays: message command-fifo-pipe
			// so we are interested only in the 3rd csv string
			r = reply->element[2];
			fprintf(stdout, "%s\n", r->str);
			fflush(stdout);
		}

		freeReplyObject(reply);
		reply = NULL;
	}
	if(reply) freeReplyObject(reply);
	if(redis) redisFree(redis);
	return(0);
}
