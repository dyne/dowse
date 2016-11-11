/*
 * webui_debug.c
 *
 *  Created on: 10 nov 2016
 *      Author: nop
 */

#include "webui_debug.h"


void _webui_add_message_level(attributes_set_t *ptr_attrl, const char*level_message,const char *body_message){

    attributes_set_t t=attrinit();
    t=attrcat(t,TMPL_VAR_LEVEL_MESSAGE,level_message);
    t=attrcat(t,TMPL_VAR_TEXT_MESSAGE,body_message);

    *ptr_attrl=attr_add(*ptr_attrl,TMPL_VAR_MESSAGE_LOOP,t);
}

WEBUI_DEF_ERROR_LEVEL_MESSAGE(info) ;
WEBUI_DEF_ERROR_LEVEL_MESSAGE(warning) ;
WEBUI_DEF_ERROR_LEVEL_MESSAGE(success) ;
WEBUI_DEF_ERROR_LEVEL_MESSAGE(error);

