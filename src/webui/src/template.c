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
#include <stdio.h>
#include <string.h>
#include "attr.h"
#include "template.h"
#ifdef WIN32
#else
/* POSIX systems can use write() to send mmap'd regions quickly.
 * otherwise use ISO C fwrite/fprintf, which buffers and will be slower. */
#include <unistd.h>
#endif

#include <kore.h>

struct entry {
	unsigned type; /* 0=raw 1=attribute */
	unsigned len;
	const char *data;
	struct entry *next;
};


static struct entry *create_entry(unsigned type, const char *data, unsigned len) {
	struct entry *ret;
	ret=kore_malloc(sizeof *ret);
	ret->type=type;
	ret->len=len;
	ret->data=data;
	ret->next=0;
	return ret;
}

static struct entry *parse_string(const char *str, unsigned len) {
	struct entry *ret=0, **curr=&ret;
	const char *end;

	assert(str!=NULL);

	while(len && (end=memchr(str, '$', len))) {
		char ch;
		/* first check for $$ */
		if(len && str[0]=='$' && str[1]=='$') {
			/* we have a $$, fine the next $ so we can setup a run of raw data
			 * in the next condition */
			end=memchr(str+2, '$', len-2);
			if(!end) { /* no second $ found */
				str++;
				len--;
				break; /* use check after loop to save string */
			}
			/* this will fall into the next condition because end!=str */
		}
		/* str to end is "raw" data */
		if(end!=str) {
			unsigned sublen;
			sublen=end-str;
			/* found $$ near the end of our rawstring */
			if(len>=sublen+2 && end[1]=='$') {
				sublen++;
				end+=2;
			}
			/* look for $ at end of string and add it to the end */
			if(len==sublen+1 && *end=='$') {
				sublen++;
				end++;
			}
			/* TODO: look for $$ at the end and add $ in that case */
			*curr=create_entry(0, str, sublen);
			curr=&(*curr)->next;
			len-=end-str;
			str=end;
		}
		if(!len) break; /* used up all the data */

		/* str and end is "${...}" */
		assert(str==end);
		assert(*end=='$');
		str++;
		len--;
		ch=*str;
		if(len && (ch=='(' || ch=='{')) {
			/* $(...) or ${...} found */
			str++;
			len--;
			end=memchr(str, ch=='('?')':'}' , len);
			if(end) {
				*curr=create_entry(1, str, (unsigned)(end-str));
				curr=&(*curr)->next;
				if(len) end++; /* eat ) or } */
				len-=end-str;
				str=end;
			} else {
				/* closing ) or } not found. treat as raw */
				/* TODO: if prev is a type 0 then append to it */
				*curr=create_entry(0, str-2, 2);
				curr=&(*curr)->next;
			}
		} else {
			/* $XXX found */
			for(end=str;(unsigned)(end-str)<len && (isalnum(*end) || *end=='_');end++) ;
			if((end-str)>1) {
				*curr=create_entry(1, str, (unsigned)(end-str));
				curr=&(*curr)->next;
			} else {
				/* $ at end of string */
				*curr=create_entry(0, str-1, (unsigned)(end-str+1));
				curr=&(*curr)->next;
			}
			len-=end-str;
			str=end;
		}
	}
	if(len) { /* use the remaining as raw data */
		*curr=create_entry(0, str, len);
		curr=&(*curr)->next;
	}
	return ret;
}


/** str parameter will be used until template_free() is called. no allocation
 * or duplication will occur on str. */
int template_load(const char *str, int len, template_t *ret) {
    assert(str!=NULL);
	if(len<0)
		len=strlen(str);

	ret->cstr=str;
	ret->entry_list=parse_string(str, (unsigned)len);
    return(0); // TODO: return number of entries
}

/* al - NULL to use a "dumping" mode */
void template_apply(template_t *t, attrlist_t al, kore_buf_t *out) {
	struct entry *e;
	char buf[64]; /* TODO: max macro length */
	attr_t *tmp;

	/* TODO: we could cache the attrget lookup */
	// fflush(stdout); /* we'll be using posix i/o instead of iso c */
	for(e=t->entry_list;e;e=e->next) {
        // kore_log(LOG_DEBUG,"::%u:%u:%.*s",
        //          e->type, e->len, e->len, e->data);

		/* ignore macro type if the macro is too large */
		if(al && e->type && (e->len<(sizeof buf-1))) {
			memcpy(buf, e->data, e->len);
			buf[e->len]=0;
			tmp = attrget(al, buf);
            if(tmp) kore_buf_append(out, tmp->value, tmp->len);

		} else if(!al && e->type) { /* debug mode because al==NULL */
			kore_log(LOG_DEBUG,"%s (%u)", e->data, e->len);
		} else { /* raw data */
            if(e->data) kore_buf_append(out,e->data,e->len);
        }
	}
}

void template_free(template_t *t) {
	struct entry *curr, *next;
	if(!t)
		return; /* t=NULL then just leave */
	for(curr=t->entry_list;curr;curr=next) {
		next=curr->next;
		kore_free(curr);
	}
//	free(t);
}

