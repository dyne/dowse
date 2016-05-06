/*  Kore_template - simple templating engine for embedded Kore.io apps
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
 *      written and maintained by Denis Roio <jaromil@dyne.org>
 *      includes libcgi code by Jon Mayo found in the Public Domain
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

#ifndef ATTR_H
#define ATTR_H
#include <stdlib.h>
#include <stdarg.h>

#if 1
/* use to force most strings to be unsigned */
typedef unsigned char _char; 
#else
/* keep strings as ordinary char */
typedef char _char;
#endif

#define MAX_ATTR_LEN	1024	/* an arbitrary limit */
typedef struct attribute_list *attrlist_t;

/* attr.c */
void attrcatn(attrlist_t al, const char *name, const _char *value, size_t len);
void attrcat(attrlist_t al, const char *name, const _char *value);
void attrsetn(attrlist_t al, const char *name, const _char *value, size_t len);
void attrset(attrlist_t al, const char *name, const _char *value);
void attrset_safe(attrlist_t al, const char *name, const _char *value);
int attrprintf(attrlist_t al, const char *name, const char *fmt, ...) __attribute__ ((format (printf, 3, 4)));
int attrvprintf(attrlist_t al, const char *name, const char *fmt, va_list ap);
const _char *attrget(attrlist_t al, const char *name);
int attrlist(attrlist_t al, const _char **type, const _char **value, int *counter);
attrlist_t attrinit(void);
void attrfree(attrlist_t al);
void namefree(void);
int attrget_int(attrlist_t al, const char *name, long *i);

#ifndef NDEBUF
#include <stdio.h>
void attrdump(FILE *out, attrlist_t al);
#endif
#endif /* ATTR_H */
