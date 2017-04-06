
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


int sleep_before_reset;

void  timeout_reset_handler(int sig)
{
    char m[1024];

    if (sleep_before_reset>0) {
        snprintf(m,sizeof(m)," Waiting other %d seconds",sleep_before_reset);
        notice(m);
        sleep_before_reset--;
        alarm(1);
    } else {
        unlink(RESET_ADMIN_FILE);
    }
}


int start_reset_procedure(struct http_request * req) {
    log_entering();
    template_t tmpl;
    attributes_set_t attr;
    u_int8_t *html_rendered;
    struct kore_buf *out;
    size_t len;
    FILE *fp;
    out = kore_buf_alloc(0);
	attr=attrinit();

	/* Create reset file */
	fp=fopen(RESET_ADMIN_FILE,"w");
	if (fp==NULL) {
	    char m[1024];
	    snprintf(m,sizeof(m),"Error on creation of file [%s] [%s]", RESET_ADMIN_FILE , strerror(errno));
	    attributes_set_t att=attrinit();
	    webui_add_error_message(&att,m);
	    err(m);
	    return show_generic_message_page(req,att);
	}
	fclose(fp);

    /* Start a process to delete the file after 30seconds
     *      Note: the thread should listen SIGPOWER signal ? */

	 signal(SIGALRM, timeout_reset_handler);

	 sleep_before_reset = 30;

	 /* Now we wait the ACK of the command */
	 alarm(1);

	/* */
    template_load("assets/start_reset_procedure.html",&tmpl);
    template_apply(&tmpl,attr,out);

	/**/
    html_rendered = kore_buf_release(out, &len);
    http_response(req, 200, html_rendered, len);

    /**/
    kore_free(html_rendered);
	attrfree(attr);

    return (KORE_RESULT_OK);
}
