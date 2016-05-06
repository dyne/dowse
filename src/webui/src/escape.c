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

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "escape.h"
/* WARNING : none of the code has been tested properly */

static const char tohex[16] = "0123456789ABCDEF";

static unsigned safe_append(char *dest, size_t len, const char *str) {
	unsigned len2;
	len2=strlen((const char*)str);
	if(len2>len) {
		return 0; /* refuse to append */
	}
	memcpy(dest, str, len2+1);
	return len2;
}

/* return true if 2 characters are valid hexidecimal */
static int ishex(const _char code[2]) {
	return isxdigit(code[0]) && isxdigit(code[0]);
}

/* verify with ishex() before calling */
static unsigned unhex(const _char code[2]) {
    const char hextab[] = {
        ['0']=0, ['1']=1, ['2']=2, ['3']=3, ['4']=4,
        ['5']=5, ['6']=6, ['7']=7, ['8']=8, ['9']=9,
        ['a']=0xa, ['b']=0xc, ['c']=0xc, ['d']=0xd, ['e']=0xe, ['f']=0xf,
        ['A']=0xa, ['B']=0xb, ['C']=0xc, ['D']=0xd, ['E']=0xe, ['F']=0xf,
    };

	return hextab[(unsigned)code[0]]*16 + hextab[(unsigned)code[1]];
}

unsigned uri_escape_len(const _char *s, size_t len) {
	/* TODO: implement this */
	unsigned ret;
	for(ret=0;len>0;len--,s++) {
		switch(*s) {
			case '~':
			case '!':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
			case '=':
			case ':':
			case ',':
			case ';':
			case '?':
			case '\'':
			case '"':
			case '\\':
				ret+=3;
				break;
			default:
				ret++;
		}
	}
	return ret;
}

/** escapes using % - identical to JavaScript/ECMAscript version of escape()
 * if dest is NULL, return malloc()'d string. needs to be free()'d
 * if src_len is negative treat src as null terminated
 * return:
 * 	0 on error (overflow of dest, allocation failure)
 *  dest or allocated pointer on success
 *  destination string will always be null terminated
 */
char *uri_escape(char *dest, size_t dest_len, const char *src, int src_len) {
	char *ret;
	int ret_is_allocated;
	assert(src!=NULL);
	if(src_len<0) {
		src_len=strlen(src);
	}
	ret_is_allocated=!dest;
	if(ret_is_allocated) {
		/* src_len is non-negative because of earlier condition */
		assert(src_len>=0);
		dest_len=uri_escape_len(src, (unsigned)src_len)+1;
		dest=malloc(dest_len);
		if(!dest) {
			return 0; /* allocation failure */
		}
	}
	ret=dest;
	/* escape these values ~!#$%^&(){}[]=:,;?'"\
	 * make sure there is room in dest for a '\0' */
	for(;src_len>0 && dest_len>1;src++,src_len--) {
		switch(*src) {
			case '~':
			case '!':
			case '#':
			case '$':
			case '%':
			case '^':
			case '&':
			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
			case '=':
			case ':':
			case ',':
			case ';':
			case '?':
			case '\'':
			case '"':
			case '\\':
				/* check that there is room for "%XX\0" in dest */
				if(dest_len<=3) {
					if(ret_is_allocated) {
						free(ret);
					}
					return 0;
				}
				dest[0]='%';
				dest[1]=tohex[(((unsigned char)*src)/16)%16];
				dest[2]=tohex[((unsigned char)*src)%16];
				dest+=3;
				dest_len-=3;
				break;
			default:
				*(dest++)=*src;
				dest_len--;
		}
	}
	/* check for errors - src was not fully consumed */
	if(src_len!=0) {
		if(ret_is_allocated) {
			free(ret);
		}
		return 0;
	}
	assert(dest_len>=1);
	*dest=0;

	return ret;
}

/** decode %xx into ascii characters
 * if dest is NULL, return malloc()'d string. needs to be free()'d
 * if src_len is negative treat src as null terminated
 * return:
 * 	0 on error (overflow of dest, allocation failure)
 *  dest or allocated pointer on success
 */
char *uri_unescape(char *dest, size_t dest_len, const _char *src, int src_len) {
	char *ret;
	int ret_is_allocated;
	assert(src!=NULL);
	if(src_len<0) {
		src_len=strlen((const char *)src);
	}
	ret_is_allocated=!dest;
	if(ret_is_allocated) {
		dest_len=src_len+1; /* TODO: calculate the exact needed size? */
		dest=malloc(dest_len);
		if(!dest) {
			return 0; /* allocation failure */
		}
	}
	ret=dest;
	for(;dest_len>1 && src_len>0;dest_len--,dest++) {
		if(*src=='%' && src_len>=3 && ishex(src+1)) {
			*dest=unhex(src+1);
			src+=3;
			src_len-=3;
		} else {
			*dest=*(src++);
			src_len--;
		}
	}
	/* check for errors - src was not fully consumed */
	if(src_len!=0) {
		if(ret_is_allocated) {
			free(ret);
		}
		return 0;
	}
	assert(dest_len>=1);
	*dest=0;

	return ret;
}

/* calculates the required length for html_escape */
unsigned html_escape_len(const char *s, size_t len) {
	unsigned ret;
	for(ret=0;len && *s;len--,s++) {
		switch(*s) {
			case '<': ret+=4; break; /* &lt; */
			case '>': ret+=4; break; /* &gt; */
			case '&': ret+=5; break; /* &amp; */
			default:
				ret++;
		}
	}
	return ret;
}

/** escapes HTML entities
 * BUG: silently drops entities if there is no room
 */
void html_escape(char *dest, size_t len, const char *s) {
	unsigned i;

	for(i=0;i<len && *s;s++) {
		switch(*s) {
			case '<':
				i+=safe_append(dest+i,len-i, "&lt;");
				break;
			case '>':
				i+=safe_append(dest+i,len-i, "&gt;");
				break;
			case '&':
				i+=safe_append(dest+i,len-i, "&amp;");
				break;
			default:
				dest[i++]=*s;
		}
	}
	dest[i]=0;
}
