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

int sql_select_into_attributes(const char*query,
         char *item_loop_name,
        attributes_set_t *attributes_result) {
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

    // Execute the statement
    if (mysql_real_query(db, query, strlen(query))) {
        show_mysql_error(db, attributes_result);
        return KORE_RESULT_ERROR;
    }

    result = mysql_store_result(db);

    num_fields = mysql_num_fields(result);
    if (num_fields == 0) {
        err(
                "The query [%s] has returned 0 fields. Is it correct?", query);
        return KORE_RESULT_ERROR;
    }

    func("The query [%s] has returned [%d] row with [%u] columns.", query,
            (int) mysql_affected_rows(db), num_fields);

    column = mysql_fetch_fields(result);

    int called = 0;
    while ((values = mysql_fetch_row(result)) != 0) {
        for (i = 0; i < num_fields; i++) {
            func( "[%d][%s][%s]", i, column[i].name, values[i]);
        }

        //--- executing "callback" function pointer that add result in the attributes_set
        int rv = __internal_callback(attributes_result, item_loop_name ,(int) num_fields,
                values, column);
        if (rv < 0) {
            break;
        }

        called += rv;
    }
    WEBUI_DEBUG;
    mysql_free_result(result);
    mysql_close(db);

    return called;
}

int __internal_callback(attributes_set_t *data, char *item_loop_name, int argc,
        MYSQL_ROW argv, MYSQL_FIELD *azColName) {
#define SIZE (256)
    int i;

    attributes_set_t t;
    t = attrinit();

    for (i = 0; i < argc; i++) { // save all fields into the template
        if (!(argv[i]))
            continue;

        if (strcmp(azColName[i].name, "last") == 0) {
            char *humandate;
            humandate = (char*) calloc(1, SIZE);

            func( "last: %s", argv[i]);
            relative_time(argv[i], humandate);
            t = attrcat(t, "last", humandate);
        } else if ((strcmp(azColName[i].name, "age") == 0)
            || (strstr(azColName[i].name, "_age") !=NULL) ) {
            char *humandate;
            humandate = (char*) calloc(1, SIZE);

            func( "%s: %s", azColName[i].name,argv[i]);
            relative_time(argv[i], humandate);
            t = attrcat(t, azColName[i].name , humandate);
        } else {
            char *key, *value;
            key = (char*) calloc(1, SIZE);
            value = (char*) calloc(1, SIZE);

            snprintf(key, SIZE, "%s", azColName[i].name);
            snprintf(value, SIZE, "%s", argv[i]);
            func( "%s: [%s]", key, value);
            t = attrcat(t, key, value);
        }
    }
    //--- In the hashmap data we add to the key "things" the hm element we created.
    (*data) = attr_add(*data, item_loop_name, t);
    return 1;

}

