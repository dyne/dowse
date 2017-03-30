
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

int load_current_identity(struct http_request * req,attributes_set_t *ptr_attr) {
    char *ipaddr_type;
       char *ipaddr_value;

       char req_macaddr[32];
    get_ip_from_request(req,&ipaddr_type,&ipaddr_value);

    ip2mac(ipaddr_type,ipaddr_value,req_macaddr,ptr_attr);

    (*ptr_attr)=attrcat(*ptr_attr, "cur_macaddr", req_macaddr );
    (*ptr_attr)=attrcat(*ptr_attr, "cur_ip", ipaddr_value );

    sprintf(identity.ipaddr_type,"%s",ipaddr_type);
    sprintf(identity.ipaddr_value,"%s",ipaddr_value);
    sprintf(identity.macaddr,"%s",req_macaddr);


    /* */
    int rv;
    char sql[256];

    sprintf(sql,"select "
            "  case when exists "
            "    (select  coalesce(name,'n/a') as cur_name from found where upper(macaddr) in (upper('%s')))"
            "  then (select  coalesce(name,'n/a') as cur_name from found where upper(macaddr) in (upper('%s')))"
            "  else 'n/a'"
            " end"
            " from dual",req_macaddr,req_macaddr);

    rv = sql_select_into_attributes(sql,NULL,ptr_attr);
    if (rv != KORE_RESULT_OK) {
        char m[1024];
        sprintf(m,"during load identity : macaddr not in found table ");
        webui_add_error_message(ptr_attr,m);
        err("%s %d %s -> (may be a installation error: clean database and restart",__FILE__,__LINE__,m);
        return KORE_RESULT_ERROR;
     }

    return KORE_RESULT_OK;
}
