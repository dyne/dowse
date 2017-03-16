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
#define __WEBUI_MAIN_FILE__
#include <webui.h>

int thing_show_cb(attributes_set_t *data, int argc, MYSQL_ROW argv,
        MYSQL_FIELD *azColName) {
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
        } else if (strcmp(azColName[i].name, "age") == 0) {
            char *humandate;
            humandate = (char*) calloc(1, SIZE);

            func( "age: %s", argv[i]);
            relative_time(argv[i], humandate);
            t = attrcat(t, "age", humandate);
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
    (*data) = attr_add(*data, "things", t);
    WEBUI_DEBUG
    return 1;

}

int thing_show(struct http_request *req) {
    u_int8_t *response;
    size_t len;
    attributes_set_t attributes;
    struct kore_buf *buf;
    char *macaddr;
    char req_macaddr[32];

    char *ipaddr_type;
    char *ipaddr_value;

    WEBUI_DEBUG
    ;
    http_populate_get(req);

    // we shouldn't free the result in macaddr
    if (http_argument_get_string(req, "macaddr", &macaddr)) {
        func( "thing_show macaddr %s", macaddr);
        //--- prepare where condition
        snprintf(where_condition, ml, "WHERE upper(F.macaddr)=upper('%s') and F.macaddr <>'00:00:00:00:00:00'", macaddr);
    } else {
        err( "thing_show get argument error");
        func( "thing_show called without argument");
        //--- prepare where condition
        snprintf(where_condition, ml, " WHERE F.macaddr<>'00:00:00:00:00:00'");
    }

    WEBUI_DEBUG;

    attributes=attrinit();


    sprintf(line,"select value as cur_state from parameter where variable='state all things'");
    sql_select_into_attributes(line,NULL,&attributes);

    load_current_identity(req,&attributes);

    get_ip_from_request(req,&ipaddr_type,&ipaddr_value);

    ip2mac(ipaddr_type,ipaddr_value,req_macaddr,&attributes);
    func("Request from [%s] [%s]",ipaddr_value,req_macaddr);

    /*
     * 0 -> my_macaddr not in (select macaddr from found where admin='yes')
     * 1 -> F.macaddr <> my_macaddr
     */
    // prepare query
    snprintf(line, ml, "SELECT (@seq := @seq +1) AS seq_number, F.*, "
            " ( CASE WHEN upper('%s') not in (select UPPER(macaddr) from found where admin='yes') THEN 0" /* se il macaddr della req non e' in admin => 0*/
            "        WHEN UPPER(F.macaddr) <> upper('%s') THEN 1 " /* se il found.macaddr e' diverso da quello della request => 1 */
            "        ELSE 0 " /* altri casi non dovrebbero esserci ma vale la : "se non esplicitamente detto => 0" */
            "  END  ) as can_i_disable_it "
            "FROM found F "
            " JOIN ( SELECT @seq := 0 ) r "
            "%s "
            "ORDER BY F.age DESC",
            req_macaddr,
            req_macaddr,
            where_condition);

    fprintf(stderr,"query [%s]",line);
    WEBUI_DEBUG
    ;
    buf = kore_buf_alloc(mb);

    WEBUI_DEBUG
    ;
    // load template
    template_load(asset_thing_show_html, asset_len_thing_show_html, &tmpl);
    WEBUI_DEBUG
    ;
   // attributes = attrinit();

    WEBUI_DEBUG
    ;
    attributes = attrcat(attributes, "title", "Dowse information panel");

    WEBUI_DEBUG
    ;
    // SQL query
    sqlquery(line, thing_show_cb, &attributes);

    template_apply(&tmpl, attributes, buf);

    WEBUI_DEBUG
    ;
    response = kore_buf_release(buf, &len);

    http_response(req, 200, response, len);
    WEBUI_DEBUG
    ;

    template_free(&tmpl);

    WEBUI_DEBUG
    ;
    attrfree(attributes);

    //    kore_buf_free(buf);

    //  kore_free(data);

    return (KORE_RESULT_OK);
}

