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

int modify_event(struct http_request * req) {
    log_entering();
    attributes_set_t attr;
    int bad_parsing=0;
    attr = attrinit();
    redisContext *redis = NULL;
    redisReply *reply = NULL;
    http_populate_get(req);

    PARSE_PARAMETER(id);
    PARSE_PARAMETER(action);
    PARSE_PARAMETER(macaddr);

    CHECK_PARAMETER();

    /**/
    WEBUI_DEBUG
    ;
    char action_sql[1024];
    char recognize_sql[1024];
    /* default recognize_sql */
    snprintf(recognize_sql, sizeof(recognize_sql),
            " UPDATE event SET recognized=true where id='%s'",id);

    /* Connecting with Redis */
    redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
    if (!redis) {
        attributes_set_t att = attrinit();
        const char m[] = "Redis server is not running";
        webui_add_error_message(&att, m);
        err(m);
        return show_generic_message_page(req, att);
    }

    /* choose the action to execute */
    if (strcmp(action,"enable_browse")==0) {
        int rv=change_authorization_to_browse(req,macaddr,"","",redis,1);
        if (rv!=KORE_RESULT_OK) {
            return rv;
        }
        snprintf(recognize_sql, sizeof(recognize_sql),
                " UPDATE event SET recognized=true where macaddr='%s' and description='new_mac_address'",
                macaddr);
    }

    if (strcmp(action,"disable_browse")==0) {
        int rv=change_authorization_to_browse(req,macaddr,"","",redis,0);
        if (rv!=KORE_RESULT_OK) {
            return rv;
        }
        snprintf(recognize_sql, sizeof(recognize_sql),
                " UPDATE event SET recognized=true where macaddr='%s' and description='new_mac_address'",
                macaddr);
    }
/*    int rv1 = sqlexecute(action_sql, &attr);
    if (rv1 != KORE_RESULT_OK) {
        return show_generic_message_page(req,attr);
    }*/

    /* event is recognized update table using the recognize_sql selected */
    int rv2 = sqlexecute(recognize_sql, &attr);
    if (rv2 != KORE_RESULT_OK) {
        return show_generic_message_page(req,attr);
    }
    /**/
    WEBUI_DEBUG;

    /* TODO :
     *  1) captive_admin (tramite modify_event) deve rilasciare l'admin sono quando avrà marcato tutti gli event a "recognized"
     *    perchè se vengo intercettato mentre che scarico una risorsa eg: (repubblica.it/bootstrap.css)
     *    quella risorsa non la scarico e il captive_admin viene comunque skippato.
     * FIXME
     *  2) se sto su captive_portal perchè sono stato intercettato devo essere rediretto
     *  su dowse.it/captive_admin ... e non sul www.sito_che_volevo_andare.it
     *
     *
     *  */

    if ( there_are_event_not_recognized() ) {
        WEBUI_DEBUG;
        http_response_header(req, "location", "http://dowse.it/captive_admin#event");
        set_no_caching_header(req);
        http_response(req, 302, NULL, 0);
    } else {
        WEBUI_DEBUG;

        /* Redirect to the saved request */
        char admin_macaddr[32];
        char *ipaddr_value;
        char *ipaddr_type;

        get_ip_from_request(req,&ipaddr_type,&ipaddr_value);
        ip2mac(ipaddr_type,ipaddr_value,admin_macaddr,&attr);
        load_request_from_redis(admin_macaddr,req);
        kore_free(ipaddr_type);
        kore_free(ipaddr_value);
    }


    /**/
    WEBUI_DEBUG;
    attrfree(attr);
    /* Free resources */
    if(reply) freeReplyObject(reply);
    if(redis) redisFree(redis);
    return (KORE_RESULT_OK);
}
