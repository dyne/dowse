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

    /* choose the action to execute */
    if (strcmp(action,"enable_browse")==0) {
        snprintf(action_sql,sizeof(action_sql),
                " UPDATE found SET authorized='%s' WHERE macaddr='%s'",
                macaddr,
                __ENABLE_TO_BROWSE_STR
                );
        snprintf(recognize_sql, sizeof(recognize_sql),
                " UPDATE event SET recognized=true where macaddr='%s' and description='new_mac_address'",
                macaddr);
    }

    if (strcmp(action,"disable_browse")==0) {
        snprintf(action_sql,sizeof(action_sql),
        "UPDATE found SET authorized='%s' WHERE macaddr='%s'",
        __DISABLE_TO_BROWSE_STR,
        macaddr
        );
        snprintf(recognize_sql, sizeof(recognize_sql),
                " UPDATE event SET recognized=true where macaddr='%s' and description='new_mac_address'",
                macaddr);
    }
    int rv1 = sqlexecute(action_sql, &attr);
    if (rv1 != KORE_RESULT_OK) {
        return show_generic_message_page(req,attr);
    }

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
    http_response_header(req, "location", "http://www.dowse.it/captive_admin#event");
    http_response(req, 302, NULL, 0);

    /**/
    WEBUI_DEBUG
    ;
    attrfree(attr);

    return (KORE_RESULT_OK);
}
