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
#include <stdlib.h>
#include <string.h>
#ifdef _XOPEN_SOURCE
# include <strings.h>
#endif
#include <stdio.h>
#include <stdarg.h>
#include "attr.h"
#include "escape.h"

#include <kore.h>

struct attribute_list
{
    unsigned len;
    attr_t *data;
};

static struct name_list
{
    unsigned len;
    _char **data;
    /* TODO: keep a reference count */
} name_list;

static int nameadd(const char *name)
{
    _char **entry;
    int type;

    if(!name) abort();
    name_list.data=kore_realloc(name_list.data, (name_list.len+1)*sizeof *name_list.data);
    type=name_list.len;
    entry=name_list.data+type;
    name_list.len++;
    *entry=(_char*)kore_strdup((const char*)name);
    return type;
}

/** gets the index in the name list of a given attribute name **/
static int attrtype(const char *name)
{
    unsigned i;
    for(i=0;i<name_list.len;i++) {
        if(!strcasecmp((const char*)name, (const char*)name_list.data[i])) {
            return i;
        }
    }
    return -1; /* no name */
}

static attr_t *attrlookup(attrlist_t al, int type)
{
    unsigned i;

    if(type<0) return NULL; /* not found */

    for(i=0;i<al->len;i++)
    {
        if(type==al->data[i].type) return al->data+i;
    }
    return NULL;
}

static attr_t *attradd(attrlist_t al, int type)
{
	//--- Ne aggiungo uno vuoto di tipo type in fondo e lo restituisco
    attr_t *at;

    void *old_buf,*new_buf;
    old_buf=al->data;
    new_buf=kore_buf_alloc((al->len+1)*sizeof(*(al->data)));
    if (old_buf!=NULL) {
        memcpy(new_buf,old_buf,sizeof(*(al->data)));
        kore_buf_free(old_buf);
    }
    al->data=new_buf;

    al->data=kore_realloc(al->data, (al->len+1)*sizeof *al->data);
    at=al->data+al->len;
    al->len++;
    at->type=type;
    at->len=0;
    at->value=NULL;
    return at;
}

static void attrdel(attrlist_t al, attr_t *at)
{
    int ofs;
    ofs=at-al->data;
    al->len--;
    al->data[ofs]=al->data[al->len];
}

static const _char *attrname(int type)
{
    if(type<0 || (unsigned)type>=name_list.len) return NULL; /* no name */
    return name_list.data[type];
}

static attr_t *setup_access(attrlist_t al, const char *name) {
    attr_t *at;
    int type;

    type=attrtype(name);
    if(type==-1) { /* type not found add it to the global list */
        type=nameadd(name);
    }
    at=attrlookup(al, type);
    if(!at) {
        /* create a new attribute if it doesn't exist */
        at=attradd(al, type);
    }

    return at;
}

/*** EXPORTED STUFF ***/

// TODO: split this in two functions: one to retrieve attr and one to
// strcat contents into it.
void attrcatn(attrlist_t al, const char *name, const _char *value, size_t len) {
    attr_t *at;
    size_t oldlen;
    _char *newvalue;
    at = setup_access(al, name);

    /* a null value would delete the attribute */
    if(value==NULL) {
        if(at) attrdel(al, at); /* delete if there is no value */
        return;
    }

    /* append the string */
    oldlen = at->len;

    // kore_log(LOG_DEBUG,"attr realloc from %u to %u for value: %s",
    //          oldlen, oldlen+len+1, value);

    newvalue = kore_realloc(at->value, oldlen+len+1);
    if(!newvalue) {
        kore_log(LOG_ERR,"attrcatn realloc to size %u",
                 oldlen+len+1);
        return; }

    at->len = oldlen+len+1;
    strncat(newvalue,value,len);
    at->value=newvalue;
}

void attrcat(attrlist_t al, const char *name, const _char *value) {
    attrcatn(al, name, value, value ? strlen((const char*)value) : 0);
}


/* set an attribute */
static void attrsetn_internal(attrlist_t al, const char *name, int safe_fl, const _char *value, size_t len)
{
    attr_t *at;
    at=setup_access(al, name);
    /* a null value would delete the attribute */
    if(value==NULL) {
        if(at) attrdel(al, at); /* delete if there is no value */
        return;
    }
    /* discard the old attribute values */
    kore_free(at->value);
    at->value=NULL;
    /* set the attribute */
    if(safe_fl) {
        at->len=html_escape_len(value, len);
        at->value=kore_malloc(at->len+1);
        html_escape(at->value, at->len, value);
    } else {
        at->len=len;
        at->value=kore_malloc(len+1);
        memcpy(at->value, value, at->len+1);
    }
}

#ifndef NDEBUF
void attrdump(FILE *out, attrlist_t al)
{
    unsigned cnt;
    fprintf(out, "CNT:%u\n", al->len);
    for(cnt=0;cnt<al->len;cnt++)
    {
        fprintf(out, "%-20d|%40s\n", al->data[cnt].type, al->data[cnt].value);
    }
    fprintf(out, "\n");
}
#endif

void attrsetn(attrlist_t al, const char *name, const _char *value, size_t len) {
    attrsetn_internal(al, name, 0, value, len);
}

void attrset(attrlist_t al, const char *name, const _char *value) {
    attrsetn_internal(al, name, 0, value, value ? strlen((const char*)value) : 0);
}

void attrset_safe(attrlist_t al, const char *name, const _char *value) {
    attrsetn_internal(al, name, 1, value, value ? strlen((const char*)value) : 0);
}

int attrvprintf(attrlist_t al, const char *name, const char *fmt, va_list ap)
{
    _char buf[MAX_ATTR_LEN];
    int len;
    len=vsnprintf((char*)buf, sizeof buf, fmt, ap);
    if(len>=0)
        attrset(al, name, buf);
    return len;
}

int attrprintf(attrlist_t al, const char *name, const char *fmt, ...)
{
    int len;
    va_list ap;
    va_start(ap, fmt);
    len=attrvprintf(al, name, fmt, ap);
    va_end(ap);
    return len;
}

/* get an attribute, return NULL if not found */
attr_t *attrget(attrlist_t al, const char *name)
{
    attr_t *at;
    int type;

    type = attrtype(name);
    at = attrlookup(al, type);
    return(at);
}

int attrlist(attrlist_t al, const _char **name, const _char **value, int *counter)
{
    if((unsigned)(*counter)<al->len) {
        *name=attrname(al->data[*counter].type);
        *value=al->data[*counter].value;
        (*counter)++;
        return 1;
    } else {
        return 0;
    }
}

attrlist_t attrinit(void)
{
    attrlist_t al;
    al=kore_malloc(sizeof *al);
    al->len=0;
    al->data=NULL;
    return al;
}

void attrfree(attrlist_t al)
{
    unsigned i;
    for(i=0;i<al->len;i++) {
        if(al->data[i].value)
            kore_free(al->data[i].value);
    }
    if(al->data) kore_free(al->data);
}

void namefree(void)
{
    unsigned i;
    for(i=0;i<name_list.len;i++) {
        kore_free(name_list.data[i]);
    }
    kore_free(name_list.data);
    name_list.data=NULL;
    name_list.len=0;
}

/* gets an attribute as a numeric value */
int attrget_int(attrlist_t al, const char *name, long *i) {
    long ret;
    attr_t *tmp;
    char *endptr;

    assert(i!=NULL);
    assert(al!=NULL);
    assert(name!=NULL);
    tmp = attrget(al, name);
    if(!tmp) {
        return 0; /* failure */
    }
    ret=strtoul(tmp->value, &endptr, 10);
    assert(endptr!=NULL);
    if(endptr == (char*)tmp->value || *endptr) {
        return 0; /* failure */
    }
    *i=ret;
    return 1; /* success */
}
