
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
#include <b64/cdecode.h>

/* TODO this mapping can be made also on CSS */
char *dowse2bootstrap(char*log_level){
    if (strcmp(log_level,"DEBUG")==0) return "info";
    if (strcmp(log_level,"NOTICE")==0) return "info";
    if (strcmp(log_level,"ACT")==0) return "success";
    if (strcmp(log_level,"WARN")==0) return "warning";
    if (strcmp(log_level,"ERROR")==0) return "danger";
    return "ERROR";
}

int print_error_list(struct http_request * req) {
    log_entering();
     template_t tmpl;
    attributes_set_t attributes;
    struct kore_buf *buf;
    char *address;

    // allocate output buffer
    buf = kore_buf_alloc(1024*1000);

    // load template
    template_load
        ("assets/print_error_list.html", &tmpl);
    attributes = attrinit();

    address = getenv("address");
    if(!address) address = "127.0.0.1";

    attrcat(attributes, "address", address);


    template_apply(&tmpl,attributes,buf);

    http_response_header(req, "content-type", "text/html");
    http_response(req, 200, buf->data, buf->offset);


    template_free(&tmpl);
    attrfree(attributes);

    kore_buf_free(buf);

    return(KORE_RESULT_OK);
}
