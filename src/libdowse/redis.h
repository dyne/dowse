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

int okredis(redisContext *r, redisReply *res);

redisReply *cmd_redis(redisContext *redis, const char *format, ...);

redisContext *connect_redis(char *host, int port, int db);

#endif
