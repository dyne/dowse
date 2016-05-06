/*  Dowse - embedded WebUI based on Kore.io
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
 *  Written by Denis Roio aka jaromil <jaromil@dyne.org>
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

#include <kore/kore.h>
#include <kore/http.h>
#include <assets.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#include <sqlite3.h>

#include "thingsdb.h"
#include "hashmap.h"

#include <parse-datetime.h>
#include "template.h"

#define mb 1024*500
#define ml 1024*3

// used with kore_buf_(create/appendf/free)
struct kore_buf *buf;

char line[ml];

sqlite3 *db = NULL;

map_t thing = NULL;

char *thing_get(char *key);

int things_list_cb(void *data, int argc, char **argv, char **azColName){
    int i;
    struct tm tt;
    char *laststr;
    // fprintf(stderr, "callback: %s\n", (const char*)data);

    for(i=0; i<argc; i++){ // save all fields into the hashmap
        hashmap_put(thing , azColName[i],
                    argv[i] ? argv[i] : "NULL");
    }
    attrcat(data,"list_of_things","<tr>");

    snprintf(line,ml,"<td>%s</td><td>%s</td>",
             thing_get("hostname"), thing_get("os"));
    attrcat(data,"list_of_things",line);

    // get last datestamp
    laststr = thing_get("last");

    // parse last into a tm struct
    memset(&tt,0,sizeof(struct tm));
    strptime(laststr, "%Y-%m-%dT%H:%M:%S", &tt);

    strftime(line, ml, "<td>%d %m %Y - %H:%M:%S</td>", &tt);
    attrcat(data,"list_of_things",line);

    snprintf(line,ml,"<td>%s</td><td>%s</td>\n",
             thing_get("macaddr"), thing_get("ip4"));

    attrcat(data,"list_of_things",line);

    attrcat(data,"list_of_things","</tr>");

    return 0;
}

int
things_list(struct http_request *req)
{
    int rc;
    char *zErrMsg = 0;
    char *query = "SELECT * FROM found ORDER BY last DESC";
    template_t tmpl;
	attrlist_t attributes;

    struct timespec when;

    if(!db) {
        rc = sqlite3_open(THINGS_DB, &db);
        if( rc ) {
            kore_log(LOG_ERR, "Can't open database: %s\n",
                     sqlite3_errmsg(db));
            return(KORE_RESULT_ERROR);
        }
    }

    buf = kore_buf_create(mb);

    if(!thing) thing = hashmap_new();

    // load template from assets
    template_load
        (asset_things_list_html, asset_len_things_list_html, &tmpl);

    // initialise attribute list
    attributes = attrinit();

    if( ! parse_datetime(&when, "now", NULL) )
        kore_log(LOG_ERR,"parse-datetime error");
    else {
        struct tm *tt;
        tt = localtime (&when.tv_sec);
        mktime(tt);
        strftime(line, ml, "Dowse :: %d %m %Y - %H:%M:%S", tt);
        attrcat(attributes, "title", line);
    }
    
    sqlite3_exec(db, query, things_list_cb, attributes, &zErrMsg);

    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }


    template_apply(&tmpl, attributes, buf);


	http_response(req, 200, buf->data, buf->offset);

    template_free(&tmpl);
    attrfree(attributes);

    kore_buf_free(buf);


	return (KORE_RESULT_OK);
}

char *thing_get(char *key) {
    int res;
    char *sval = NULL;
    res = hashmap_get(thing, key, (void**)&sval);
    if(res==MAP_OK)
        return(sval);
    else
        return("NULL");
}
