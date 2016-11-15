/*
 * configure_admin.c
 *
 *  Created on: 15 nov 2016
 *      Author: nop
 */

#include <webui.h>

int configure_admin(struct http_request * req) {
    char message[256];
    int len;

    WEBUI_DEBUG;
    sprintf(message,"Hello this is the configure admin page");
    http_response(req, 200, message, strlen(message));

    return (KORE_RESULT_OK);
}
