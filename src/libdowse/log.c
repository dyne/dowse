#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <b64/cencode.h>


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#include <hiredis/hiredis.h>

redisContext *log_redis  = NULL;//connect_redis("127.0.0.1", 6379, 0);
base64_encodestate b64_state;
redisReply *minimal_cmd_redis(redisContext *redis, const char *format, ...) ;
  
void toredis(char *pfx, char *msg) {
    if (log_redis) {

        redisReply *reply;
        reply=redisCommand(log_redis, "PUBLISH log-channel %s:%ld:%s", pfx,time(NULL), msg);

        if (reply && reply->len) {
            fprintf(stderr,"%s %d redis_reply %s\n",
                    __FILE__,__LINE__,reply->str);
        }

        char command[256];
        char b64_encoded[512];
        base64_init_encodestate(&b64_state);
        int rv=base64_encode_block(msg, strlen(msg), b64_encoded, &b64_state);

        int rv2=base64_encode_blockend(b64_encoded+rv,&b64_state);
        b64_encoded[rv+rv2-1]=0;

        sprintf(command,"LPUSH log-queue %s:%ld:%s", (pfx),time(NULL), b64_encoded);
        /* Using the plain msg variable the values are splitted by the blank character */
        //        sprintf(command,"LPUSH log-queue %s:%s", pfx,msg);

        /**/
        reply=minimal_cmd_redis(log_redis,"%s",command);

        if (reply && reply->len) {
            fprintf(stderr,"%s %d redis_reply %s\n",
                    __FILE__, __LINE__,reply->str);
        }
    }
}

void func(const char *fmt, ...) {
#if (DEBUG==1)
	va_list args;

	char msg[256];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, ANSI_COLOR_BLUE " [D] " ANSI_COLOR_RESET, 5+5+4);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);

	toredis("DEBUG", msg);

#endif
	return;
}

void err(const char *fmt, ...) {
	va_list args;

	char msg[256];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, ANSI_COLOR_RED " [!] " ANSI_COLOR_RESET, 5+5+4);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);

	toredis("ERROR", msg);

}


void notice(const char *fmt, ...) {
	va_list args;

	char msg[256];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, ANSI_COLOR_GREEN " (*) " ANSI_COLOR_RESET, 5+5+4);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);

	toredis("NOTICE", msg);
}


void act(const char *fmt, ...) {
	va_list args;

	char msg[256];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, "  .  ", 5);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);

	toredis("ACT", msg);
}



void warn(const char *fmt, ...) {
	va_list args;

	char msg[256];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, ANSI_COLOR_YELLOW " (*) " ANSI_COLOR_RESET, 5+5+4);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);

	toredis("WARN", msg);
}
