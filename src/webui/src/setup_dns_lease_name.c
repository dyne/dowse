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

int setup_dns_lease_name(attributes_set_t *attributes_result) {
    MYSQL_RES *result;
    MYSQL_ROW values; //  it as an array of char pointers (MYSQL_ROW),
    MYSQL_FIELD*column;
    unsigned int num_fields;
    unsigned int i;
    MYSQL *db;
    redisContext *redis = NULL;
    redisReply *reply = NULL;
    // open db connection
    db = mysql_init(NULL);
    //     Constant parameted created at compile time
    if (!mysql_real_connect(db, DB_HOST, DB_USER, DB_PASSWORD, DB_SID, 0,
    DB_SOCK_DIRECTORY, 0)) {
        show_mysql_error(db, attributes_result);
        db = NULL;
        return (KORE_RESULT_ERROR);
    }

    /* Connecting with Redis */
    redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);

    char query[1024];

    snprintf(query, sizeof(query),
                "select name,ip4 as address from found where name is not null and coalesce(ip4,'')<>''"
                " union  "
                "select name,ip6 as address from found where name is not null and coalesce(ip6,'')<>''");

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
        char address[32];
        char name[128];
        for (i = 0; i < num_fields; i++) {
            func("[%d][%s][%s]", i, column[i].name, values[i]);
            if (strcmp(column[i].name, "name") == 0) {
                sprintf(name, values[i]);
            }
            if (strcmp(column[i].name, "address") == 0) {
                sprintf(address, values[i]);
            }
        }

        if (strcmp(address,"")==0) {
            err("at %s %d > Invalid state : address is null => I can't set redis entry .");
        } else {
            /* Print command on redis channel */
            reply = cmd_redis(redis, "SET dns-lease-%s %s", name, address);
            if (reply) {
                freeReplyObject(reply);
            }
        }


    }
    WEBUI_DEBUG
    ;
    /* Free resources */
    mysql_free_result(result);
    mysql_close(db);

    if (redis)
        redisFree(redis);
    return KORE_RESULT_OK;
    return 0;
}
