#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "redis.h"
#include "log.h"

redisReply *cmd_redis(redisContext *redis, const char *format, ...) {
	va_list args;

	redisReply *res;
	char command[512];

	va_start(args, format);
	vsnprintf(command, 511, format, args);
	func("cmd_redis: %s", command);
	res = redisCommand(redis, command);
	va_end(args);

	if(!res) {
		err("redis error: %s", redis->errstr);
		return(NULL);
	} else if( res->type == REDIS_REPLY_ERROR ) {
		err("redis error: %s", res->str);
		return(NULL);
	} else {
		return(res);
	}
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
		} else {
			err("Connection error: can't allocate redis context");
		}
	}
	// select the dynamic database where is dns_query_channel
	reply = cmd_redis(rx, "SELECT %u", db);
	// TODO: check if result is OK
	// fprintf(stderr,"SELECT: %s\n", reply->str);
	freeReplyObject(reply);
	return rx;
}

