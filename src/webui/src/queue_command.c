/*
 * queue_command.c
 *
 *  Created on: 16 nov 2016
 *      Author: Nicola 
 */

#include <webui.h>

#include <libdowse/dowse.h>

#define CHAN "command-fifo-pipe"


int queue_command(struct http_request * req) {
    struct kore_buf *buf;
    char *message;
    char command[256];
    int len;
    int bad_parsing=0;
    attributes_set_t attr=attrinit();
    redisContext *redis = NULL;
    redisReply   *reply = NULL;

    /* Parsing parameter */
    http_populate_get(req);

    PARSE_PARAMETER(op);
    PARSE_PARAMETER(macaddr);
    PARSE_PARAMETER(ip4);
    PARSE_PARAMETER(ip6);


    /* If "op" command not set or no parameter specified*/
    if ((strcmp(op,"")==0) ||
        ( (strcmp(macaddr,"")==0)&&(strcmp(ip4,"")==0)&&(strcmp(ip6,"")==0)))
    {
        err("%s command not well defined",__where_i_am__);
        buf=kore_buf_alloc(0);
        char m[]="<html><strong>command not well specified</strong></html>";
        kore_buf_append(buf,m,strlen(m));
        message = kore_buf_release(buf, &len);
        http_response(req, 404, message, len);
        return (KORE_RESULT_OK);
    }

    /* Connecting with Redis */
    redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
    if(!redis) {

        attributes_set_t att=attrinit();
        const char m[]="Redis server is not running";
        webui_add_error_message(&att,m);
        err(m);

        return show_generic_message_page(req,att);
    }

    /* Calculating calling IP extracting from request */
    char *ipaddr_type,*calling_ipaddr;
    get_ip_from_request(req,&ipaddr_type,&calling_ipaddr);

    /* calculating Epoch time*/
    struct timeval tp;
    struct timezone tz;
    gettimeofday(&tp,&tz);

    char epoch[256];
    snprintf(epoch,sizeof(epoch),"%d",tp.tv_sec);

    /* Construct command to publish on Redis channel */
    snprintf(command,sizeof(command),"CMD,%s,%s,%s,%s,%s,%s",calling_ipaddr,op,epoch,macaddr,ip4,ip6);

    /* Print command on redis channel */
    reply = cmd_redis(redis,"PUBLISH %s %s", CHAN,command,calling_ipaddr);

    /* Setup the URI for redirection , if it's present referer otherwise we redirect to /things */

    char *from;
    if ((!http_request_header(req, "Referer", &from))
            && (!http_request_header(req, "referer", &from))) {
        from = "/things";
    }

    http_response_header(req,"location",from);
    http_response(req,302,NULL,0);

    /* Free resources */
    if(reply) freeReplyObject(reply);
    if(redis) redisFree(redis);

    return (KORE_RESULT_OK);
}
