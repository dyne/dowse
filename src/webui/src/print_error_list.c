
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
	attributes_set_t attr;
    u_int8_t *html_rendered;
    struct kore_buf *out;
    size_t len;
    out = kore_buf_alloc(0);
	attr=attrinit();


	extern redisContext *log_redis;
	redisContext *backup_log_redis=log_redis;
	log_redis=NULL; /* To disable redis logging (otherwise infinite loop) */

	redisContext *redis_shell = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
    if (!redis_shell) {
        const char m[] = "Problem during rendering of error list: Redis server is not running";
        webui_add_error_message(&attr, m);
        err(m);
        log_redis=backup_log_redis; /* Restore redis logging*/
        return apply_template_and_return(req, attr,
                "assets/print_error_list.html",
                200);
    }

	/**/
    redisReply*reply=NULL;
    base64_decodestate b64_state;

    while (1) {
        reply= minimal_cmd_redis(redis_shell,"RPOP log-queue");

        if (reply->len ) {

            char *tmp=(char*)malloc(reply->len+1);
            sprintf(tmp,"%s",reply->str);

            char *sep=index(tmp,':');
            char *sep2=index(sep+1,':');

            char *level_message=tmp;
            char *time_message=sep+1; (*sep)=0;
            char *body_message=sep2+1; (*sep2)=0;

            /* Decode message was encoded in b64 to escaping character */
            char command[256];
            base64_init_decodestate(&b64_state);
            int rv=base64_decode_block(body_message, strlen(body_message), command, &b64_state);

            command[rv]=0;

            /* Decode the time in time_human_readable */
            char time_human_readable[256];
            relative_time(time_message,time_human_readable);

            /**/
            attributes_set_t t=attrinit();

            t=attrcat(t,TMPL_VAR_LEVEL_MESSAGE,dowse2bootstrap(level_message));
            t=attrcat(t,TMPL_VAR_TEXT_MESSAGE,command);
            t=attrcat(t,TMPL_VAR_TIME_MESSAGE,time_human_readable);

            attr=attr_add(attr,TMPL_VAR_MESSAGE_LOOP,t);

            freeReplyObject(reply);
            free(tmp);
        } else {
            break;
        }

    }


    template_load("assets/print_error_list.html",&tmpl);
    template_apply(&tmpl,attr,out);

	/**/
    html_rendered = kore_buf_release(out, &len);
    http_response(req, 200, html_rendered, len);

    /**/
    kore_free(html_rendered);
	attrfree(attr);


	redisFree(redis_shell);
    log_redis=backup_log_redis; /* Restore redis logging*/

    return (KORE_RESULT_OK);
}
