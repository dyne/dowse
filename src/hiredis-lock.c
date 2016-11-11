/*  Dowse - Lock system for redis
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

#include <log.h>

#include "redis.h"
#include "database.h"

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
	char *action, *name, *timeout;
	long int key;
	int res = 1;

	// args: name on/off timeout
	if(argc < 4) {
		err("usage: %s on/off name ttl/key", argv[0]);
		exit(1);
	}

	action  = argv[1];
	name    = argv[2];
	timeout = argv[3];

	signal(SIGINT, ctrlc);

	redis = connect_redis(REDIS_HOST, REDIS_PORT, db_runtime);
	if(!redis) {
		err("Dowse is not running");
		exit(1);
	}

	if( strncmp(action, "on", 2) == 0 ) {
		srandom( time(NULL) );

		key = random();
		reply = cmd_redis(redis, "SET lock_%s %lu NX PX %s", name, key, timeout);
		if(!reply) { redisFree(redis); exit(1); }

		switch(reply->type) {

		case REDIS_REPLY_STATUS:
			if( strncmp(reply->str, "OK", 2) == 0) {
				fprintf(stdout,"%lu\n", key);
				res = 0;
			}
			break;

		default: break;
		}

		// prints out "0" as key on failure
		if(res==1) fprintf(stdout,"0\n");

	} else if(strncmp(action, "off", 3) == 0 ) {

		reply = cmd_redis(redis, "GET lock_%s", name);
		if(!reply) { redisFree(redis); exit(1); }

		switch(reply->type) {

		case REDIS_REPLY_STRING:
			if( strcmp(reply->str, timeout) == 0 ) { // key match, remove lock
				freeReplyObject(reply);
				reply = cmd_redis(redis, "DEL lock_%s", name);
				if(!reply) { redisFree(redis); exit(1); }
				res = 0;
				func("succesful unlock: %s (key %s)", name, timeout);

			} else // key mismatch, cannot remove lock
				err("resource locked: %s", name);
				
			break;

		case REDIS_REPLY_NIL:
			// no lock found with that name
			res = 0; // we assume a success
			break;

			// TODO: better detail on statuses (lock not found, lock taken, etc...)
		default: 
			func("lock off reply: %u str %s", reply->type, reply->str);
			break;
		}
		
	} else {
		err("action not supported: %s",action);
		redisFree(redis);
		exit(1);
	}

	if(reply) freeReplyObject(reply);
	redisFree(redis);
	exit(res);
}
