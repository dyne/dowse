#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// to quickly return 404
#define HTTP404 err("HTTP 404, %s:%u, %s()",	  \
                      __FILE__, __LINE__, __func__);	  \
	http_response(req, 404, NULL, 0); \
	return(KORE_RESULT_OK)

// for use in debugging
#define FLAG func("reached: %s:%u, %s()", __FILE__, __LINE__, __func__)


void func(const char *fmt, ...) {
#if (DEBUG==1)
	va_list args;

	char msg[256];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, "[D] ", 4);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);
#endif
}

void err(const char *fmt, ...) {
	va_list args;

	char msg[256];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, "[!] ", 4);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);
}


#endif
