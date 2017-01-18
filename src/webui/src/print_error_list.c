
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

int print_error_list(struct http_request * req) {
    template_t tmpl;
	attributes_set_t attr;
    char *html_rendered;
    struct kore_buf *out;
    int len;
    out = kore_buf_alloc(0);
	attr=attrinit();

	/**/
    WEBUI_DEBUG;

    sql_select_into_attributes("SELECT 1 from dual","message_loop",&attr);

    template_load(asset_print_error_list_html,asset_len_print_error_list_html,&tmpl);
    template_apply(&tmpl,attr,out);

	/**/
    WEBUI_DEBUG;
    html_rendered = kore_buf_release(out, &len);
    http_response(req, 200, html_rendered, len);

    /**/
    WEBUI_DEBUG;
    kore_free(html_rendered);
	attrfree(attr);

    return (KORE_RESULT_OK);
}
