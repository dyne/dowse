
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

/* Send command to change the authorization level to redis cmd fifo ,
 *
 * ARGUMENT :
 *  - macaddress (required)
 *  - ip4 (facultative)
 *  - ip6 (facultative)
 * */
int change_authorization_to_browse(struct http_request * req,const char*macaddr,const char*ip4,const char*ip6,redisContext *redis ,int enable_or_disable){
    char command[256];
    redisReply   *reply = NULL;

    /* Calculating calling IP extracting from request */
    char *ipaddr_type,*calling_ipaddr;
    get_ip_from_request(req,&ipaddr_type,&calling_ipaddr);

    /* calculating Epoch time*/
    struct timeval tp;
    struct timezone tz;
    gettimeofday(&tp,&tz);

    char epoch[256];
    snprintf(epoch,sizeof(epoch),"%lu",tp.tv_sec);

    char op[256];
    sprintf(op,"%s",(enable_or_disable?"THING_ON":"THING_OFF"));

    /* Construct command to publish on Redis channel */
    snprintf(command,sizeof(command),"CMD,%s,%s,%s,%s,%s,%s",calling_ipaddr,op,epoch,macaddr,ip4,ip6);

    /* Print command on redis channel */
    reply = cmd_redis(redis,"PUBLISH %s %s", CHAN,command,calling_ipaddr);

    if(reply) freeReplyObject(reply);

    return KORE_RESULT_OK;
}
