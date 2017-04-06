
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
#include <sys/stat.h>
#include <sys/types.h>
/*
 *  TO BE DISCUSSED :
 * This page is filtered by the v_ip_authorized validator to be routed on captive portal functionalities
 * if that filter is passed (without routing) :
 *  there are 2 motivation:
 *   - the DNS has redirected to captive portal and the v_ip_authorized_browse counter not.
 *      (eg: the admin has enabled the macaddr after the DNS redirect)
 *       Solution: redirect on last request saved?
 *   - some Things on the network is making URL-scan for CVE
 *
 * The this page doesn't do nothing, because it doesn't need nothing.
 * */
int proxy(struct http_request * req) {
    char*out;
    size_t len;


    char path_file[256];

    if (strstr(req->path,"..")) {
        return _404(req);
    }

    sprintf(path_file,"./assets/%s",req->path);
    
    int fd;

    /* */
    struct stat buf;
    stat(path_file, &buf);

    func("load_dynamic_asset of \n[%s]  %d\n", path_file, buf.st_size);

    out = (char*) malloc(sizeof(char) * (buf.st_size + 1));

    len = buf.st_size;
    fd = open(path_file, O_SYNC | O_RDONLY);
    if (fd < 0) {
        warn("Will return 404 due to error at line %s %d : %s ", __FILE__, __LINE__, strerror(errno));
        return _404(req);
   }

    int rv = read(fd, out, len);
    if (rv != len) {
        warn("Will return 404 due to error at line %s %d : %s trying to open [%s][%d] readed [%d]",
                __FILE__, __LINE__, strerror(errno), path_file, len, rv);
               return _404(req);
   }
   out[len] = 0;
   close(fd);

   http_response_header(req,"content-type",identify_content_type(path_file,out,len));
   http_response(req, 200, out, len);
   free(out);
   return KORE_RESULT_OK;
}

int _404(struct http_request*req){

    template_t tmpl;
    attributes_set_t attr;
    u_int8_t  *html_rendered;
    struct kore_buf *out;
    size_t len;
    out = kore_buf_alloc(0);
    attr=attrinit();
    /**/
    WEBUI_DEBUG;

    /* TODO leggere gli asset ? */
    template_load("assets/proxy.html",&tmpl);
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
