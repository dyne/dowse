#ifndef __DOWSE_H__
#define __DOWSE_H__

#include <hiredis/hiredis.h>

// to quickly return 404
#define HTTP404 err("HTTP 404, %s:%u, %s()",	  \
                    __FILE__, __LINE__, __func__);	  \
	http_response(req, 404, NULL, 0); \
	return(KORE_RESULT_OK)

// for use in debugging
#define FLAG func("reached: %s:%u, %s()", __FILE__, __LINE__, __func__)


void func(const char *fmt, ...);

void err(const char *fmt, ...);

void act(const char *fmt, ...);


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

redisReply *cmd_redis(redisContext *redis, const char *format, ...);

redisContext *connect_redis(char *host, int port, int db);

#endif
