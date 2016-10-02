
#ifndef __REDIS_H__
#define __REDIS_H__

#include <hiredis/hiredis.h>

#define REDIS_HOST "127.0.0.1"
#define REDIS_PORT 6379

#define MAX_OUTPUT 512

/* typedef struct redisReply { */
/*     int type;            // REDIS_REPLY */
/*     long long integer;  // The integer when type is REDIS_REPLY_INTEGER */
/*     int len;           // Length of string */
/*     char *str;        // Used for both REDIS_REPLY_ERROR and REDIS_REPLY_STRING */
/*     size_t elements; // number of elements, for REDIS_REPLY_ARRAY */
/*     struct redisReply **element; // elements vector for REDIS_REPLY_ARRAY */
/* } redisReply; */

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

#endif
