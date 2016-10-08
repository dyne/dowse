#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void func(const char *fmt, ...) {
	va_list args;

	char msg[512];
	size_t len;

	va_start(args, fmt);

	vsnprintf(msg, 512, fmt, args);
	len = strlen(msg);
	write(2, msg, len);
	write(2, "\n", 1);
	fsync(2);

	va_end(args);
}
