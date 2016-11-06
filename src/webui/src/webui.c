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
#include "webui_debug.h"

#define mb (1024*500)
#define ml (1024*3)

// used with kore_buf_(create/appendf/free)
struct kore_buf *buf;

char line[ml];
char where_condition[ml];
char *thingsdb;

MYSQL *db = NULL;

map_t thing = NULL;

//typedef int (*callback_type)(void *data, int argc, MYSQL_ROW argv, MYSQL_FIELD *azColName)  ;

char *thing_get(char *key);
int sqlquery(char *query,
		int (*callback)(void *data, int , MYSQL_ROW , MYSQL_FIELD *),
             //int (*callback)(void*,int,char**,char**),
//		callback_type callback,
		attributes_hm_t attrl);

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


//int thing_show_cb(void *data, int argc, char **argv, char **azColName)
int thing_show_cb(void *data, int argc, MYSQL_ROW argv, MYSQL_FIELD *azColName)
{
    int i;
    char humandate[256];
    memset(humandate,0,256);
    attributes_hm_t t;
    t=attrinit();

    for(i=0; i<argc; i++){ // save all fields into the template
        if(!(argv[i])) continue;

        if(strcmp(azColName[i].name,"last")==0) {
             kore_log(LOG_DEBUG,"last: %s",argv[i]);
            relative_time(argv[i],humandate);
//            attrset(data, "last", humandate);
            attrcat(t, "last", humandate);
        } else if(strcmp(azColName[i].name,"age")==0) {
             kore_log(LOG_DEBUG,"age: %s",argv[i]);
            relative_time(argv[i],humandate);
//            attrset(data, "age",  humandate);
            attrcat(t, "age",  humandate);
        } else {
          kore_log(LOG_DEBUG,"%s: [%s]",azColName[i].name,argv[i]);
           	//attrprintf(data, azColName[i].name, "%s","n/a");
           	attrcat(t, azColName[i].name,argv[i]);
        }
    }
    //--- In the hashmap data we add to the key "things" the hm element we created.
    attr_add(data,"things",t);
    WEBUI_DEBUG
    return 0;

}


//int things_list_cb(void *data, int argc, char **argv, char **azColName)
int things_list_cb(void *data, int argc, MYSQL_ROW argv, MYSQL_FIELD *azColName)
{
    int i;
    struct tm tt;
    char *laststr;
    char humandate[256];
     char *button_group_start="<div class=\"btn-group\" role=\"group\" aria-label=\"actions\">";
     char *button_start="<div type=\"button\" class=\"btn btn-default\">";
    WEBUI_DEBUG

    for(i=0; i<argc; i++){ // save all fields into the hashmap
        hashmap_put(thing , azColName[i].name,
                    (argv[i] ? argv[i] : "NULL"));
    }
    attrcat((attributes_hm_t)data,"list_of_things","<tr>");

    snprintf(line,ml,
"<td><a href=\"/things?macaddr=%s\">"
"%s</td><td>%s</td></a>",
             thing_get("macaddr"),
             thing_get("hostname"), thing_get("os"));
    attrcat((attributes_hm_t)data,"list_of_things",line);

    // get last datestamp
    laststr = thing_get("last");
    if(laststr) relative_time(laststr,humandate);
    snprintf(line, ml, "<td>%s</td><td>", humandate);
    attrcat((attributes_hm_t)data,"list_of_things",line);

    // action buttons
    attrcat((attributes_hm_t)data,"list_of_things",button_group_start);

    // info button
    attrcat((attributes_hm_t)data,"list_of_things",button_start);
    snprintf(line,ml,
             "<a href=\"/things?macaddr=%s\">info</a></div>",
             thing_get("macaddr"));
    attrcat((attributes_hm_t)data,"list_of_things",line);

    attrcat((attributes_hm_t)data,"list_of_things","</div></td></tr>");

    // snprintf(line,ml,"<td>%s</td><td>%s</td>\n",
    //          thing_get("macaddr"), thing_get("ip4"));

    // attrcat(data,"list_of_things",line);


    return 0;
}
char *macaddr;
template_t tmpl;

attributes_hm_t attributes;


int thing_show(struct http_request *req) {
    int rc;
    u_int8_t	 *data;
    int len;
    char *mess;
    attributes_hm_t studente,studente2;

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

	WEBUI_DEBUG
    // prepare query
	snprintf(line,ml,"SELECT * FROM found %s ORDER BY age DESC",where_condition);
    
    WEBUI_DEBUG
    // allocate output buffer
    buf = kore_buf_alloc(mb);

    WEBUI_DEBUG
    // load template
    template_load
        (asset_thing_show_html, asset_len_thing_show_html, &tmpl);
    WEBUI_DEBUG
    attributes = attrinit();

    WEBUI_DEBUG

    attrcat(attributes, "title", "Dowse information panel");
    attrcat(attributes, "title", "Secondo titolo ");

    studente =attrinit();
    attrcat(studente,"nome","Nicola");
    attrcat(studente,"cognome","Rossi");
    attrcat(studente,"indirizzo","Casa Sua");
    attr_add(attributes,"studente",studente);

    studente2 =attrinit();
    attrcat(studente2,"nome","Mario");
    attrcat(studente2,"cognome","Bianchi");
    attrcat(studente2,"indirizzo","Un altro posto");
    attr_add(attributes,"studente",studente2);

//        attr_add(attributes,"studente",studente);

    WEBUI_DEBUG
    kore_log(LOG_DEBUG," Il cielo cade? %d",((hashmap_map*)(attributes))->size );
    WEBUI_DEBUG

    debug_attributes(attributes);
	attrget(attributes,"title",0,(any_t*)&mess);
    WEBUI_DEBUG
    kore_log(LOG_DEBUG," title[%s]",mess);
    WEBUI_DEBUG

/*
    // SQL query
    sqlquery(line, thing_show_cb, attributes);
    WEBUI_DEBUG;

    template_apply(&tmpl,attributes,buf);
    WEBUI_DEBUG;

    data=kore_buf_release(buf,&len);

//	http_response(req, 200, buf->data, buf->offset);
	http_response(req, 200, data,len);
    WEBUI_DEBUG;

    template_free(&tmpl);
    WEBUI_DEBUG;
    kore_log(LOG_DEBUG,"Log attributes [%p %d]",attributes->data,attributes->len);*/
    WEBUI_DEBUG
     attrfree(attributes);

//    kore_buf_free(buf);
   WEBUI_DEBUG;
//  kore_free(data);
  WEBUI_DEBUG;
	return (KORE_RESULT_OK);

}

int things_list(struct http_request *req) {
    int rc;
    char *zErrMsg = 0;
    char *query = "SELECT * FROM found ORDER BY last DESC";
    template_t tmpl;
	attributes_hm_t attributes;

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
		int (*callback)( void*,int , MYSQL_ROW , MYSQL_FIELD *),
		//             int (*callback)(void*,int,MYSQL_ROW,MYSQL_FIELD *),
//		callback_type callback,
//             int (*callback)(void*,int,char**,char**),
             attributes_hm_t attrl) {
	MYSQL_RES *result;
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

	/*
	char **keys=(char **)kore_buf_alloc(num_fields*sizeof(char*));
	for (i=0;i<num_fields;i++) {
		keys[i]=(char*)kore_buf_alloc((strlen(column[i].name)+1)*sizeof(char));
		sprintf(keys[i],"%s",column[i].name);
	}*/

	while ((values = mysql_fetch_row(result))!=0) {
	   for (i=0;i<num_fields;i++){
		   kore_log(LOG_DEBUG,"[%d][%s][%s]",i,column[i].name,values[i]);
   	   }

   	   //--- executing "callback" function pointer that add result in the attributes_hm
   	   callback((void*)attrl,(int)num_fields,values,column);
	}
    WEBUI_DEBUG ;

//    kore_buf_free(keys);
	  WEBUI_DEBUG ;

	mysql_free_result(result);
	WEBUI_DEBUG ;
	return 0;

}
