
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

int captive_portal_client(struct http_request * req) {
    log_entering();
    template_t tmpl;
    u_int8_t *html_rendered;
    struct kore_buf *out;
    attributes_set_t attr;
    size_t len;
    out = kore_buf_alloc(0);
    int bad_parsing=0;
    attr=attrinit();

    sprintf(line,"select value as cur_state from parameter where variable='state all things'");
    sql_select_into_attributes(line,NULL,&attr);

    load_current_identity(req,&attr);
	/**/

    http_populate_get(req);

    PARSE_PARAMETER(macaddr);

    WEBUI_DEBUG;
    CHECK_PARAMETER();

    char sql[256];
    char *ipaddr_type;
    char *ipaddr_value;
    char ip4[32],ip6[64];
    get_ip_from_request(req,&ipaddr_type,&ipaddr_value);

    if (strcmp(ipaddr_type,"ipv4")==0) {
        sprintf(ip4,"%s",ipaddr_value);
        ip6[0]=0;
    } else {
        ip4[0]=0;
        sprintf(ip6,"%s",ipaddr_value);
    }
    kore_free(ipaddr_type); kore_free(ipaddr_value);


    snprintf(sql,sizeof(sql),"INSERT INTO event (level,macaddr,ip4,ip6,description) "
            "VALUES ('warning',upper('%s'),'%s','%s','%s') ",macaddr,ip4,ip6, __EVENT_NEW_MAC_ADDRESS);
    WEBUI_DEBUG;

    int rv = sqlexecute(sql, &attr);
    if (rv != KORE_RESULT_OK) {
        return show_generic_message_page(req,attr);
    }
    /* */
    sprintf(sql,"select upper(macaddr) as client_macaddr,name as client_name from found where upper(macaddr)=upper('%s')",macaddr);
    rv = sql_select_into_attributes(sql,NULL,&attr);
    if (rv != KORE_RESULT_OK) {
            char m[1024];
            sprintf(m,"macaddr not in found table ");
            webui_add_error_message(&attr,m);
            err("%s %d %s -> (may be a installation error: clean database and restart",__FILE__,__LINE__,m);
            return show_generic_message_page(req,attr);
    }

    load_global_attributes(attr);

    template_load(asset_captive_portal_client_html,asset_len_captive_portal_client_html,&tmpl);
   // template_apply(&tmpl,global_attributes,out);
    template_apply(&tmpl,attr,out);

	/**/
    html_rendered = kore_buf_release(out, &len);
    http_response(req, 200, html_rendered, len);

    /**/
    kore_free(html_rendered);

    return (KORE_RESULT_OK);
}
