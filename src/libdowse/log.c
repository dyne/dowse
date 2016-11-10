#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
	return;
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


void act(const char *fmt, ...) {
	va_list args;

	char msg[256];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, " .  ", 4);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);
}
