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

int reset_admin(struct http_request * req) {
    log_entering();
    template_t tmpl;
    attributes_set_t attr;
    u_int8_t *html_rendered;
    struct kore_buf *out;
    size_t len;
    int rv;


    attr=attrinit();

    sprintf(line,"select value as cur_state from parameter where variable='state all things'");
    sql_select_into_attributes(line,NULL,&attr);

    load_current_identity(req,&attr);


    if ((strcmp(identity.ipaddr_type, "ipv4") != 0)
            && (strcmp(identity.ipaddr_type, "ipv6") != 0)) {
        err("Can't retrieve IP address from request and proc file system");
        return KORE_RESULT_ERROR;
    }

    rv = check_if_ip_admin_configured(&attr);

    out = kore_buf_alloc(0);

    func(" %s %d rv== %d",__FILE__,__LINE__,rv);
    if (rv == _ADMIN_NOT_CONFIGURED_) {
        /* admin is not configured so take the ip from connection and grant it the admin privileges */
        char m[1024];


        /* Se la tupla da aggiornare non c'e' la aggiunge */
        char *iptype=(strcmp(identity.ipaddr_type,"ipv4")==0?"ip4":"ip6");

        /* In this if branch the admin is not configured so
         * we should insert an entry in the found table to describe the admin
         *
         * */
        snprintf(m,sizeof(m),
                       "INSERT INTO found (%s,%s,admin,authorized) VALUES (upper('%s'),'%s','yes','%s') "
                       " ON DUPLICATE KEY UPDATE admin='yes' , authorized='%s'",
                       "macaddr",iptype,
                       identity.macaddr,identity.ipaddr_value,__ENABLE_TO_BROWSE_STR,__ENABLE_TO_BROWSE_STR
               );
        rv=sqlexecute(m,&attr);
        if (rv==KORE_RESULT_ERROR) {
            err("Error during admin found insert or update");
            return show_generic_message_page(req,attr);
        } else {
            act("Admin found insert or updated");
            /*
            if (global_attributes!=NULL) {
                attrfree(global_attributes);
                global_attributes=attrinit();
            }
            load_global_attributes(global_attributes);
            */

            /* HTML message to say new admin device */
            http_response_header(req, "location", "/things");
            http_response(req, 302, NULL, 0);

            return KORE_RESULT_OK;
       }
    } else {
        /**/
        WEBUI_DEBUG
        ;
        template_load("assets/reset_admin.html",&tmpl);
        template_apply(&tmpl, attr, out);

        /**/
        WEBUI_DEBUG
        ;
        html_rendered = kore_buf_release(out, &len);
        http_response(req, 200, html_rendered, len);

        /**/
        WEBUI_DEBUG
        ;
        kore_free(html_rendered);
        attrfree(attr);

        return (KORE_RESULT_OK);
    }

}

int check_if_ip_admin_configured(attributes_set_t *ptr_attrl) {
    log_entering();
   /* Deve essere associato al MAC ADDRESS l'utenza di admin */
   char query[]="select count(*) from found where admin='yes' ";
/*       char query[] =
            "select count(*) from ( "
                    "   select ip4 as ipaddr_value from found where admin='yes' "
                    "       union "
                    "   select ip6 as ipaddr_value from found where admin='yes' "
                    " ) as tmp where tmp.ipaddr_value is not null and tmp.ipaddr_value<>''";*/

    MYSQL_RES *result;
    MYSQL_ROW values; //  it as an array of char pointers (MYSQL_ROW),
    MYSQL_FIELD*column;
    MYSQL *db;
    unsigned int num_fields;
    unsigned int i;
    int rv;

    rv= _IP_IS_NOT_ADMIN_;

    // open db connection
    db = mysql_init(NULL);

    //     Constant parameted created at compile time
    if (!mysql_real_connect(db, DB_HOST, DB_USER, DB_PASSWORD, DB_SID, 0,
            DB_SOCK_DIRECTORY, 0)) {
        show_mysql_error(db, ptr_attrl);
        db = NULL;
        return _SQL_ERROR_;
    }

    WEBUI_DEBUG
    ;
    // Execute the statement
    if (mysql_real_query(db, query, strlen(query))) {
        show_mysql_error(db, ptr_attrl);
        return _SQL_ERROR_;
    }

    WEBUI_DEBUG
    result = mysql_store_result(db);

    WEBUI_DEBUG
    num_fields = mysql_num_fields(result);
    if (num_fields == 0) {
        err(
                "The query [%s] has returned 0 fields. Is it correct?", query);
        return _SQL_ERROR_;
    }

    WEBUI_DEBUG
    func(
            "The query [%s] has returned [%d] row with [%u] columns.", query,
            (int) mysql_affected_rows(db), num_fields);

    column = mysql_fetch_fields(result);

    while ((values = mysql_fetch_row(result)) != 0) {
       for (i = 0; i < num_fields; i++) {
            func( "[%d][%s][%s]", i, column[i].name, values[i]);
        }

        if (atoi(values[0]) == 0) {
            rv= _ADMIN_NOT_CONFIGURED_;
        } else {
            rv= _IP_IS_NOT_ADMIN_;
        }
    }

    mysql_free_result(result);
    mysql_close(db);

   return rv;
}
