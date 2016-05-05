#include <kore/kore.h>
#include <kore/http.h>

int		page(struct http_request *);

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

// liblo
// #include <lo/lo.h>

// jemalloc
// #include <jemalloc/jemalloc.h>

#include <sqlite3.h>
#include "thingsdb.h"
#include "hashmap.h"

#define mb 1024*1000
#define ml 1024*3
char buf[mb];
char line[ml];

sqlite3 *db = NULL;

map_t thing = NULL;

char *thing_get(char *key);

int things_list_cb(void *data, int argc, char **argv, char **azColName){
    int i;
    // fprintf(stderr, "callback: %s\n", (const char*)data);

    for(i=0; i<argc; i++){ // save all fields into the hashmap
        hashmap_put(thing , azColName[i], argv[i] ? argv[i] : "NULL");
    }

    // fprintf(stderr, "elements: %u\n", hashmap_length(thing));

    snprintf(line,ml,"<h3>%s [ %s ]</h3>\n",
             thing_get("hostname"), thing_get("os"));
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
    char *query = "SELECT * FROM found WHERE state IS NOT NULL";

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

    sqlite3_exec(db, query, things_list_cb, "things/list/all", &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }


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
