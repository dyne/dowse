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

#include <kore.h>
#include <http.h>
#include <assets.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <mysql.h>

#include "thingsdb.h"
#include "hashmap.h"

#include <parse-datetime.h>
#include "template.h"

#define mb (1024*500)
#define ml (1024*3)

// used with kore_buf_(create/appendf/free)
struct kore_buf *buf;

char line[ml];
char where_condition[ml];
char *thingsdb;

MYSQL *db = NULL;

map_t thing = NULL;

#define WEBUI_DEBUG {}

//{fprintf(stderr,"WEBUI_DEBUG: %s>%s:%d\n",__FILE__,__func__,__LINE__);}

char *thing_get(char *key);
int sqlquery(char *query,
             int (*callback)(void*,int,char**,char**),
             attrlist_t attrl);

int relative_time(char *utc, char *out) {
    time_t nowutc;
    time_t now;
    time_t then;
    long int deltaSeconds;
    long int deltaMinutes;

    struct tm tt;
    // parse utc into a tm struct
    memset(&tt,0,sizeof(struct tm));
    if( ! strptime(utc, "%Y-%m-%dT%H:%M:%SZ", &tt) ) {
        // kore_log(LOG_ERR,"relative_time failed parsing UTC string: %s",utc);
        if( ! strptime(utc, "%Y-%m-%dT%H:%M:%S", &tt) ) {
         	sprintf(out,"n/a");

            return 1;

        }
    	}
    then = mktime(&tt);
    
    // gets what UTC time is now
    time( &nowutc );
    now = mktime( gmtime ( &nowutc ) );

    // localtime_r( &rnow, &now );
    deltaSeconds = now - then;
    deltaMinutes = (now / 60) - (then / 60);

    if(deltaSeconds < 60)
        snprintf(out,256,"%u seconds ago", deltaSeconds);
    else if (deltaMinutes < 60)
        snprintf(out,256,"%u minutes ago", deltaMinutes);
    else if (deltaMinutes < (24 * 60))
        snprintf(out,256,"%u hours ago",(int)floor(deltaMinutes/60));
    else if (deltaMinutes < (24 * 60 * 7))
        snprintf(out,256,"%u days ago",(int)floor(deltaMinutes/(60 * 24)));
    else if (deltaMinutes < (24 * 60 * 31))
        snprintf(out,256,"%u weeks ago",(int)floor(deltaMinutes/(60 * 24 * 7)));
    else if (deltaMinutes < (24 * 60 * 365.25))
        snprintf(out,256,"%u months ago",(int)floor(deltaMinutes/(60 * 24 * 30)));
    else
        snprintf(out,256,"%u years ago",(int)floor(deltaMinutes/(60 * 24 * 365)));

    return 0;
}

#define blankify(s) (((s==NULL) || (strcmp("",s)==0)) ?"n/a":s)

int thing_show_cb(void *data, int argc, char **argv, char **azColName)
{
    int i;
    char humandate[256];
    memset(humandate,0,256);
    for(i=0; i<argc; i++){ // save all fields into the template
        if(!(argv[i])) continue;

        if(strcmp(azColName[i],"last")==0) {
             kore_log(LOG_DEBUG,"last: %s",argv[i]);
            relative_time(argv[i],humandate);
            attrset(data, "last", humandate);
        } else if(strcmp(azColName[i],"age")==0) {
             kore_log(LOG_DEBUG,"age: %s",argv[i]);
            relative_time(argv[i],humandate);
            attrset(data, "age",  humandate);
        } else {
          kore_log(LOG_DEBUG,"%s: [%s]",azColName[i],argv[i]);
        //  	attrprintf(data, azColName[i], "%s",blankify(argv[i]));
          	attrprintf(data, azColName[i], "%s","n/a");
        }
    }
    WEBUI_DEBUG
    return 0;

}
int things_list_cb(void *data, int argc, char **argv, char **azColName){
    int i;
    struct tm tt;
    char *laststr;
    char humandate[256];
    const char *button_group_start="<div class=\"btn-group\" role=\"group\" aria-label=\"actions\">";
    const char *button_start="<div type=\"button\" class=\"btn btn-default\">";
    WEBUI_DEBUG

    // fprintf(stderr, "callback: %s\n", (const char*)data);

    for(i=0; i<argc; i++){ // save all fields into the hashmap
        hashmap_put(thing , azColName[i],
                    argv[i] ? argv[i] : "NULL");
    }
    attrcat(data,"list_of_things","<tr>");

    snprintf(line,ml,
"<td><a href=\"/things?macaddr=%s\">"
"%s</td><td>%s</td></a>",
             thing_get("macaddr"),
             thing_get("hostname"), thing_get("os"));
    attrcat(data,"list_of_things",line);

    // get last datestamp
    laststr = thing_get("last");
    if(laststr) relative_time(laststr,humandate);
    snprintf(line, ml, "<td>%s</td><td>", humandate);
    attrcat(data,"list_of_things",line);

    // action buttons
    attrcat(data,"list_of_things",button_group_start);

    // info button
    attrcat(data,"list_of_things",button_start);
    snprintf(line,ml,
             "<a href=\"/things?macaddr=%s\">info</a></div>",
             thing_get("macaddr"));
    attrcat(data,"list_of_things",line);

    attrcat(data,"list_of_things","</div></td></tr>");

    // snprintf(line,ml,"<td>%s</td><td>%s</td>\n",
    //          thing_get("macaddr"), thing_get("ip4"));

    // attrcat(data,"list_of_things",line);


    return 0;
}
char *macaddr;
template_t tmpl;
attrlist_t attributes;

int thing_show(struct http_request *req) {
    int rc;
    u_int8_t	 *data;
    int len;

    WEBUI_DEBUG
	http_populate_get(req);

    // we shouldn't free the result in macaddr
	if (http_argument_get_string(req, "macaddr", &macaddr)) {
		kore_log(LOG_DEBUG, "thing_show macaddr %s",macaddr);
		//--- prepare where condition
		snprintf(where_condition,ml,"WHERE macaddr='%s'",macaddr);
	} else {
        kore_log(LOG_ERR,"thing_show get argument error");
        kore_log(LOG_DEBUG,"thing_show called without argument");
		//--- prepare where condition
		snprintf(where_condition,ml,"");
    }

    // prepare query
	snprintf(line,ml,"SELECT * FROM found %s ORDER BY age DESC",where_condition);
    
    // allocate output buffer
    buf = kore_buf_alloc(mb);

    // load template
    template_load
        (asset_thing_show_html, asset_len_thing_show_html, &tmpl);
    attributes = attrinit();

    attrcat(attributes, "title", "Dowse information panel");

    // SQL query
    sqlquery(line, thing_show_cb, attributes);

    template_apply(&tmpl,attributes,buf);

    data=kore_buf_release(buf,&len);

//	http_response(req, 200, buf->data, buf->offset);
	http_response(req, 200, data,len);

    template_free(&tmpl);
    attrfree(attributes);

    kore_free(data);

	return (KORE_RESULT_OK);

}

int things_list(struct http_request *req) {
    int rc;
    char *zErrMsg = 0;
    char *query = "SELECT * FROM found ORDER BY last DESC";
    template_t tmpl;
	attrlist_t attributes;

    struct timespec when;
    WEBUI_DEBUG
    buf = kore_buf_alloc(mb);

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
    
    sqlquery(query, things_list_cb, attributes);

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



inline void show_error(MYSQL *mysql) {
 char log_message[2048];
 snprintf(log_message, sizeof(log_message),
                       "Error(%d) [%s] \"%s\"", mysql_errno(mysql),
                                                mysql_sqlstate(mysql),
                                                mysql_error(mysql));
  kore_log(LOG_ERR, "%s: [%s]\n", log_message, thingsdb);
  mysql_close(mysql);
}

// same as sqlite3_exec
int sqlquery(char *query,
             int (*callback)(void*,int,char**,char**),
             attrlist_t attrl) {
	MYSQL_RES * result;
	MYSQL_ROW values; //  it as an array of char pointers (MYSQL_ROW),
	MYSQL_FIELD*column;
	unsigned int num_fields;
    unsigned int i;

//    thingsdb=getenv("db_things");
    thingsdb="things";
    if (thingsdb==NULL) {
    	kore_log(LOG_ERR,
    			"Error db_things(=$db[things]) environment variable not defined %s>%s:%d",
				__FILE__,__func__,__LINE__);
    	return KORE_RESULT_ERROR;
    }
    WEBUI_DEBUG ;
    char *zErrMsg = 0;
    // open db connection
    if(!db) {
    	db=mysql_init(NULL);
    	if (!mysql_real_connect(db, "localhost", "root","p4ssw0rd",
    	                          thingsdb, 0, "/home/nop/.dowse/run/mysql/mysqld.sock", 0))
    	{
			show_error(db);
            return(KORE_RESULT_ERROR);
    	}
    }

    WEBUI_DEBUG ;
	//    sqlite3_exec(db, query, callback, attrl, &zErrMsg);

    // Execute the statement
	if (mysql_real_query(db, query, strlen(query))) {
	    show_error(db);
	   	return KORE_RESULT_ERROR;
	}

    WEBUI_DEBUG ;

	result=mysql_store_result(db);

    WEBUI_DEBUG ;

    num_fields = mysql_num_fields(result);
	if (num_fields == 0) {
		kore_log(LOG_ERR,"The query [%s] has returned 0 fields. Is it correct?",query);
		return KORE_RESULT_ERROR;
	}

    WEBUI_DEBUG ;

	kore_log(LOG_DEBUG,"The query [%s] has returned [%d] row with [%u] columns.",
			query, (int)mysql_affected_rows(db), num_fields);

	column= mysql_fetch_fields(result);

	/**/
	char **keys=(char **)kore_buf_alloc(num_fields*sizeof(char*));
	for (i=0;i<num_fields;i++) {
		keys[i]=(char*)kore_buf_alloc((strlen(column[i].name)+1)*sizeof(char));
		sprintf(keys[i],"%s",column[i].name);
	}

	while ((values = mysql_fetch_row(result))!=0) {
	   for (i=0;i<num_fields;i++){
		   kore_log(LOG_DEBUG,"[%d][%s][%s]",i,keys[i],values[i]);
   	   }
   	   //--- executing "callback" function pointer
   	//   callback((void*)attrl,(int)num_fields,(char**)values, (char**)keys);
	}
    WEBUI_DEBUG ;

	for ( i=0;i<num_fields;i++) {
		kore_free(keys[i]);
	}
//    kore_free(keys);

	mysql_free_result(result);
	return 0;

}
