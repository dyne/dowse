#include <webui.h>

int sqlquery(char *query,
        int (*callback)(attributes_set_t*, int, MYSQL_ROW, MYSQL_FIELD *),
        attributes_set_t *ptr_attrl) {
    log_entering();

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
        show_mysql_error(db, ptr_attrl);
        db = NULL;
        return (KORE_RESULT_ERROR);
    }

    // Execute the statement
    if (mysql_real_query(db, query, strlen(query))) {
        show_mysql_error(db, ptr_attrl);
        return KORE_RESULT_ERROR;
    }

    result = mysql_store_result(db);

    num_fields = mysql_num_fields(result);
    if (num_fields == 0) {
        err(
                "The query [%s] has returned 0 fields. Is it correct?", query);
        return KORE_RESULT_ERROR;
    }

    func(
            "The query [%s] has returned [%d] row with [%u] columns.", query,
            (int) mysql_affected_rows(db), num_fields);

    column = mysql_fetch_fields(result);

    int called=0;
    while ((values = mysql_fetch_row(result)) != 0) {
        for (i = 0; i < num_fields; i++) {
            func( "[%d][%s][%s]", i, column[i].name, values[i]);
        }

        //--- executing "callback" function pointer that add result in the attributes_set
        int rv=callback(ptr_attrl, (int) num_fields, values, column);
        if (rv<0) {
            break;
        }

        called+=rv;
    }
    mysql_free_result(result);
    mysql_close(db);
    return called;
}
