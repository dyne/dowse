
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

int modify_things(struct http_request * req) {
	attributes_set_t attr;
    http_populate_post(req);
    int bad_parsing=0;
    attr=attrinit();

    PARSE_PARAMETER(macaddr);
    PARSE_PARAMETER(column);
//    PARSE_PARAMETER(value);
    char *value="";
      http_argument_get_string(req, "value" ,&value);
      func( "%s Parameter %s:%d [%s]",value,__FILE__,__LINE__,value);
      if (strcmp(value,"")==0 ){
        char m[1024];
        snprintf(m, sizeof(m), "<strong>The name of the things is not validated</strong>. Might contain only letter number an dash or underscore (- , _)<br>"
                " Please come back and change it.<br>"
                "<a href=\"/things\",>/things</a>)");
        webui_add_error_message(&attr, m );
        err(m);
        bad_parsing=1;
      }

    PARSE_PARAMETER(url_from);

    CHECK_PARAMETER();

    /* We don't use the Referer HTTP header field because it's not mandatory
     * a browser can eliminate it. */
    

    
    /**/
    WEBUI_DEBUG;
    char sql[512];
    snprintf(sql,sizeof(sql),
            "UPDATE found set %s='%s' where upper(macaddr)=upper('%s')",
            column,value,macaddr);

    int rv=sqlexecute(sql,&attr);
    if (rv!=KORE_RESULT_OK)
	/**/
    WEBUI_DEBUG;
    http_response_header(req, "location", url_from);
    http_response(req, 302, NULL, 0);

    /**/
    WEBUI_DEBUG;
	attrfree(attr);

    return (KORE_RESULT_OK);
}
