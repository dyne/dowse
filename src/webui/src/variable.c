
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

int variable(struct http_request * req) {
    template_t tmpl;
	attributes_set_t attr;
    u_int8_t  *html_rendered;
    struct kore_buf *out;
    size_t len;
    out = kore_buf_alloc(0);
	attr=attrinit();

	/**/
    WEBUI_DEBUG;

    http_populate_get(req);
    int bad_parsing=0;
    PARSE_PARAMETER(v);
    CHECK_PARAMETER();

    sprintf(line,"select '%s' as name_of_variable , value as value_of_variable from parameter where variable='%s'",v,v);
    sql_select_into_attributes(line,NULL,&attr);

    template_load("assets/variable.json",&tmpl);
    template_apply(&tmpl,attr,out);

	/**/
    WEBUI_DEBUG;
    html_rendered = kore_buf_release(out, &len);
    http_response_header(req,"content-type","application/json");
    http_response(req, 200, html_rendered, len);

    /**/
    WEBUI_DEBUG;
    kore_free(html_rendered);
	attrfree(attr);

    return (KORE_RESULT_OK);
}
