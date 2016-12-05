/*
 * kore_preload.c
 *
 *  Created on: 23 nov 2016
 *      Author: Nicola 
 */

#include <webui.h>

void kore_preload() {
    WEBUI_DEBUG
    global_attributes=attrinit();
    int rv;

    WEBUI_DEBUG
    rv=load_global_attributes();

    if (rv==KORE_RESULT_ERROR) {
        /* La welcome page gestira' gli errori del kore_preload() */
    }

}

int check_if_reset_admin_device(){
    struct stat stat_buf;
    int rv;

    /* return if present RESET_ADMIN_FILE */
    rv=stat(RESET_ADMIN_FILE,&stat_buf);
    if (rv==0) {

    }

    /* */
    return (rv==0);
}


int reset_admin_device(){
    int rv=sqlexecute("UPDATE found set admin=null",&global_attributes);
    if (rv==KORE_RESULT_OK) {
        unlink(RESET_ADMIN_FILE);
    }
    return rv;
}

