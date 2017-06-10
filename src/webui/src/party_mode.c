/*  Dowse - embedded WebUI based on Kore.io
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
 *  Written by Nicola Rossi <nicola@dyne.org>
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

#include <webui.h>

/* it's called in Kore_Preload*/
int setup_party_mode(attributes_set_t *attributes_result){
    MYSQL_RES *result;
     MYSQL_ROW values; //  it as an array of char pointers (MYSQL_ROW),
     MYSQL_FIELD*column;
     unsigned int num_fields;
     unsigned int i;
     MYSQL *db;
     // open db connection
     db = mysql_init(NULL);
     //     Constant parameted created at compile time
     if (!mysql_real_connect(db, DB_HOST, DB_USER, DB_PASSWORD, DB_SID, 0,
     DB_SOCK_DIRECTORY, 0)) {
         show_mysql_error(db, attributes_result);
         db = NULL;
         return (KORE_RESULT_ERROR);
     }

     char query[1024];

     snprintf(query, sizeof(query),
                 "select value as status from parameter where variable='party mode'");

     // Execute the statement
     if (mysql_real_query(db, query, strlen(query))) {
         show_mysql_error(db, attributes_result);
         return KORE_RESULT_ERROR;
     }

     result = mysql_store_result(db);

     num_fields = mysql_num_fields(result);
     if (num_fields == 0) {
         err("The query [%s] has returned 0 fields. Is it correct?", query);
         return KORE_RESULT_ERROR;
     }

     func("The query [%s] has returned [%d] row with [%u] columns.", query,
             (int) mysql_affected_rows(db), num_fields);

     column = mysql_fetch_fields(result);

     while ((values = mysql_fetch_row(result)) != 0) {
         char status[128];
         for (i = 0; i < num_fields; i++) {
             func("[%d][%s][%s]", i, column[i].name, values[i]);
             if (strcmp(column[i].name, "status") == 0) {
                 sprintf(status, values[i]);
                 change_party_mode_to(NULL,attributes_result,strcmp(status, "ON") == 0);
             }
         }
     }
     WEBUI_DEBUG
     ;
     /* Free resources */
     mysql_free_result(result);
     mysql_close(db);

     return KORE_RESULT_OK;
}



int change_party_mode_to(struct http_request *req,   attributes_set_t *ptr_attr, int party_mode_on) {
    redisContext *redis = NULL;
    redisReply *reply = NULL;

    /* Connecting with Redis */
    redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
    if (!redis) {
        attributes_set_t att = attrinit();
        const char m[] = "Redis server is not running";
        webui_add_error_message(&att, m);
        err(m);

        return show_generic_message_page(req, *ptr_attr);
    }

    char sql[256];

    if (party_mode_on) {
        /* set party-mode on Redis */
        reply = cmd_redis(redis, "SET party-mode ON");
        if (reply)
            freeReplyObject(reply);

        /* update the DB */
        snprintf(sql, sizeof(sql),
                "update parameter set value='ON' where variable='party mode'");

        if (sqlexecute(sql, ptr_attr) != KORE_RESULT_OK) {
            return show_generic_message_page(req, *ptr_attr);
        }
        return KORE_RESULT_OK;

    } else {
        /* delete party-mode on Redis to set off*/
        reply = cmd_redis(redis, "DEL party-mode");
        if (reply)
            freeReplyObject(reply);

        /* update the DB */
        snprintf(sql, sizeof(sql),
                "update parameter set value='OFF' where variable='party mode'");

        if (sqlexecute(sql, ptr_attr) != KORE_RESULT_OK) {
            return show_generic_message_page(req, *ptr_attr);
        }

        /* reset the authorization level to starting revoking and setting like startup */
        reply = cmd_redis(redis, "KEYS authorization-mac-*");

        int j;
        if (reply && reply->type == REDIS_REPLY_ARRAY) {
            for (j = 0; j < reply->elements; j++) {
                func(" revoking authorization for %u) %s \n", j,
                        reply->element[j]->str);

                redisReply *r2;
                r2 = cmd_redis(redis, "DEL %s", reply->element[j]->str);
                if (r2) {
                    freeReplyObject(r2);
                }
            }
        }
        if (reply)
            freeReplyObject(reply);

        /* set the authorization level to starting */
        return setup_authorization(ptr_attr);
    }
    return KORE_RESULT_ERROR;
}
