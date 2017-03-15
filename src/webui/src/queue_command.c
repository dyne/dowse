/*
 * queue_command.c
 *
 *  Created on: 16 nov 2016
 *      Author: Nicola 
 */

#include <webui.h>

#include <libdowse/dowse.h>

redisContext *redis_context;

int queue_command(struct http_request * req) {
    log_entering();
    struct kore_buf *buf;
    u_int8_t *message;
    size_t len;
    int bad_parsing=0;
    attributes_set_t attr=attrinit();
    redisContext *redis = NULL;
    redisReply   *reply = NULL;

    /* Parsing parameter */
    http_populate_get(req);

    PARSE_PARAMETER(op);
    PARSE_PARAMETER(macaddr);


    /* If "op" command not set or no parameter specified*/
    if (bad_parsing)
    {
        err("%s command not well defined",__where_i_am__);
        buf=kore_buf_alloc(0);
        char m[]="<html><strong>command not well specified</strong></html>";
        kore_buf_append(buf,m,strlen(m));
        message = kore_buf_release(buf, &len);
        http_response(req, 404, message, len);
        return (KORE_RESULT_OK);
    }
    PARSE_PARAMETER(ip4);
    PARSE_PARAMETER(ip6);


    /* Connecting with Redis */
    redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
    if(!redis) {
        attributes_set_t att=attrinit();
        const char m[]="Redis server is not running";
        webui_add_error_message(&att,m);
        err(m);

        return show_generic_message_page(req,att);
    }
    redis_context=redis;


    if ((strcmp(op,"THING_ON")==0)||(strcmp(op,"THING_OFF")==0)) {
        int rv=change_authorization_to_browse(req,macaddr,"","",redis,op);
        if (rv!=KORE_RESULT_OK) {
            return rv;
        }
    }
    if (strcmp(op,"ALL_THINGS_OFF")==0) {
        char *ipaddr_type,*calling_ipaddr;
        char ip4[64],ip6[256];
        char calling_macaddr[32];

        get_ip_from_request(req,&ipaddr_type,&calling_ipaddr);
        ip4[0]=ip6[0]=0;
        if (strcmp(ipaddr_type,"ipv4")==0) {
            sprintf(ip4,"%s",calling_ipaddr);
        }
        if (strcmp(ipaddr_type,"ipv6")==0) {
            sprintf(ip6,"%s",calling_ipaddr);
        }

        ip2mac(ipaddr_type,calling_ipaddr,calling_macaddr,&attr);

        int rv=change_authorization_to_browse(req,calling_macaddr,ip4,ip6,redis,"ALL_THINGS_OFF");
        if (rv!=KORE_RESULT_OK) {
            return rv;
        }
    }


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
