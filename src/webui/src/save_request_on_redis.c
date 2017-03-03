
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

int save_request_on_redis(struct http_request * req,char*macaddr) {
    /* Print command on redis channel */
    redisContext *redis = NULL;
    redisReply   *reply = NULL;

    if ((req->host==NULL)||(strstr(req->host,"www.dowse.it")!=NULL)||(strcmp(req->host,"dowse.it")==0)) {
        /**/
        return 0;
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

   /* TODO Settare una "scadenza" della request? */
   char *serialized_request=serialize_request(req);
   reply = cmd_redis(redis,"SET captured-request-of-%s %s", macaddr,serialized_request);
   WEBUI_DEBUG;
   /* Free resources */
   if(reply) freeReplyObject(reply);
   if(redis) redisFree(redis);

 //  free(serialized_request);

   return 1;
}

char*serialize_request(struct http_request *req){
    /* TODO migliorare questa per gestire argomenti della POST cookie etc etc */
    char tmp[16];
    snprintf(tmp,sizeof(tmp),"%d", req->method );
    func("%s %d [%s]",__FILE__,__LINE__,req->host);
    func("%s %d [%s]",__FILE__,__LINE__,req->path);
    func("%s %d [%s]",__FILE__,__LINE__,(req->query_string?req->query_string:""));

    int len=strlen(tmp)+1+strlen(req->host)+strlen(req->path)+(req->query_string?strlen(req->query_string):0)+1;

    char *out=(char*)malloc(len+1);

    sprintf(out,"%d|%s%s%s",req->method,req->host,req->path,(req->query_string?req->query_string:""));
    WEBUI_DEBUG;

    return out;
}

int deserialize_request(char *in,u_int8_t *ptr_method,char*url){
    /* TODO migliorare questa per gestire argomenti della POST cookie etc etc */
    char tmp[1024];
    char tmp2[1024];
    char ch;
    sscanf(in,"%c|%s",&ch,tmp);
    (*ptr_method)=(ch-'0');

    if (strncmp(tmp,"www.",4)) {
        sprintf(tmp2,"www.%s",tmp);
    } else {
        sprintf(tmp2,"%s",tmp);
    }
    if (!((strncmp(tmp2,"http://",7)==0) || (strncmp(tmp2,"https://",8)==0))) {
        sprintf(url,"http://%s",tmp);
    } else {
        sprintf(url,"%s",tmp);
    }

    err("deserialize in [%s]",url);
    return 0;
}

int set_no_caching_header(struct http_request*req){
    http_response_header(req,"Cache-Control", "no-cache, no-store, must-revalidate");
    http_response_header(req,"Pragma","no-cache");
    http_response_header(req,"Expires","0");
    return 0;
}

int load_request_from_redis(char*macaddr,struct http_request * req){
    /* Print command on redis channel */
       redisContext *redis = NULL;
       redisReply   *reply = NULL;

       WEBUI_DEBUG;
       /* Connecting with Redis */
      redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
      if(!redis) {
          attributes_set_t att=attrinit();
          const char m[]="Redis server is not running";
          webui_add_error_message(&att,m);
          err(m);

          return show_generic_message_page(req,att);
      }

      /**/
      reply = cmd_redis(redis,"GET captured-request-of-%s", macaddr);
      u_int8_t method;
      char url_to_redirect[BUFSIZ];
      if (reply->len>0) {
          func("GET returned [%s]",reply->str);
          deserialize_request(reply->str,&method,url_to_redirect);
          func("returning on navigation of [%s]",url_to_redirect);
          if(reply) freeReplyObject(reply);
          reply=cmd_redis(redis,"DEL captured-request-of-%s", macaddr);

      } else {
          func("GET not returned");
          sprintf(url_to_redirect,"http://www.dowse.it/");
      }
      /* Free resources */
      if(reply) freeReplyObject(reply);
      if(redis) redisFree(redis);

      func("%s %d Redirecting to [%s]",__FILE__,__LINE__,url_to_redirect);
      http_response_header(req, "location", url_to_redirect);
      set_no_caching_header(req);
      http_response(req, 302, NULL, 0);

      return 0;
}
