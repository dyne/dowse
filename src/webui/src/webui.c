#include <kore/kore.h>
#include <kore/http.h>

int	page(struct http_request *);

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

#define mb 1024*1000
#define ml 1024*3
char buf[mb];
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
        hashmap_put(thing , azColName[i], argv[i] ? argv[i] : "NULL");
    }

    // fprintf(stderr, "elements: %u\n", hashmap_length(thing));

    snprintf(line,ml,"<h3>%s [ %s ] ",
             thing_get("hostname"), thing_get("os"));
    strncat(buf,line,mb);

    // get last datestamp
    laststr = thing_get("last");
    // parse last into a tm struct
    memset(&tt,0,sizeof(struct tm));
    strptime(laststr, "%Y-%m-%dT%H:%M:%S", &tt);

    strftime(line, ml, "last seen: %d %m %Y - %H:%M:%S</h3>", &tt);
    strncat(buf,line,mb);

    snprintf(line,ml,"<li style=\"font-family: courier\">%s - %s</li>\n",
             thing_get("macaddr"), thing_get("ip4"));
    strncat(buf,line,mb);
    return 0;
}

int
things_list(struct http_request *req)
{
    int rc;
    char *zErrMsg = 0;
    char *query = "SELECT * FROM found ORDER BY last DESC";
    struct timespec when;

    if(!db) {
        rc = sqlite3_open(THINGS_DB, &db);
        if( rc ){
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            exit(0);
        }
    }

    if(!thing) thing = hashmap_new();

    strcat(buf,
           "<html>"
           "<head>"
           "<title>Dowse</title>"
           "</head>"
           "<body>");

    if( ! parse_datetime(&when, "now", NULL) )
        fprintf(stderr,"parse-datetime error\n");
    else {
        struct tm *tt;
        tt = localtime (&when.tv_sec);
        mktime(tt);
        strftime(line, ml, "<h1>Dowse :: %d %m %Y - %H:%M:%S</h1>", tt);
        strncat(buf,line,mb);
    }

    sqlite3_exec(db, query, things_list_cb, "things/list/all", &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    strcat(buf,"</body></html>");


	http_response(req, 200, buf, strlen(buf));
    memset(buf,0,strlen(buf));
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
