/*
 * v_ip_validate.c
 *
 *  Created on: 15 nov 2016
 *      Author: Nicola 
 */

#include <webui.h>


int v_ip_validate(struct http_request * req,char*data) {
    /*
     * data in questo caso non è usato ma deve essere passato perchè i validatori
     * prendono 1 parametro che è il valore da validare nel caso delle function è la req stessa
     *
     * */
    char *ipaddr_type;
    char *ipaddr_value;
    attributes_set_t*ptr_attrl;

    get_ip_from_request(req,&ipaddr_type,&ipaddr_value);
    if ((strcmp(ipaddr_type,"ipv4")!=0)&&(strcmp(ipaddr_type,"ipv6")!=0)) {
        kore_log(LOG_ERR,"Can retrieve IP address from request and proc file system");
        return KORE_RESULT_ERROR;
    }

    int rv=_check_if_ip_is_admin(ipaddr_type,ipaddr_value,ptr_attrl);

    if (rv==_IP_IS_ADMIN_) return KORE_RESULT_OK;

    /*--- ... nel DB non è presente un valore di admin e l'IP può essere registrato come admin. */
    //_grant_ip_admin_privileges(ipaddr_type,ipaddr_value);

    /* from kore.io source code: Authentication types of "request" send their own HTTP responses. */

    http_response_header(req, "location", "/reset_admin");
    http_response(req, 302, NULL, 0);

    kore_free(ipaddr_type);
    kore_free(ipaddr_value);

    WEBUI_DEBUG;
    return (KORE_RESULT_ERROR);
}



int _check_if_ip_is_admin(char*ipaddr_type,char*ipaddr_value,attributes_set_t*ptr_attrl){
    char query[]="select * from ( "
            "   select ip4 as ipaddr_value from found where admin='yes' "
            "       union "
            "   select ip6 as ipaddr_value from found where admin='yes' "
            " ) as tmp where tmp.ipaddr_value is not null and tmp.ipaddr_value<>''";


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
        return _IP_IS_NOT_ADMIN_;
    }

    WEBUI_DEBUG
    // Execute the statement
    if (mysql_real_query(db, query, strlen(query))) {
        show_mysql_error(db, ptr_attrl);
        return _IP_IS_NOT_ADMIN_;
    }

    WEBUI_DEBUG
    result = mysql_store_result(db);

    WEBUI_DEBUG
    num_fields = mysql_num_fields(result);
    if (num_fields == 0) {
        kore_log(LOG_ERR,
                "The query [%s] has returned 0 fields. Is it correct?", query);
        return _IP_IS_NOT_ADMIN_;
    }

    WEBUI_DEBUG
    kore_log(LOG_DEBUG,
            "The query [%s] has returned [%d] row with [%u] columns.", query,
            (int) mysql_affected_rows(db), num_fields);

    column = mysql_fetch_fields(result);

    int called=0;
    while ((values = mysql_fetch_row(result)) != 0) {
        called++;
        for (i = 0; i < num_fields; i++) {
            kore_log(LOG_DEBUG, "[%d][%s][%s]", i, column[i].name, values[i]);
        }

        if (strcmp(ipaddr_value,values[0])==0){
            kore_log(LOG_DEBUG, "[%d][%s][%s] is admin ", i, column[0].name, values[0]);
            return _IP_IS_ADMIN_;
        };
    }
    WEBUI_DEBUG;
    mysql_free_result(result);
    mysql_close(db);

    WEBUI_DEBUG;
    if (called==0) return _ADMIN_NOT_CONFIGURED_; /* The db is not configured so we should configure it */
    return _IP_IS_NOT_ADMIN_; /* The db is configured but it's the admin */
}
