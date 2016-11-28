/*
 * kore_preload.c
 *
 *  Created on: 23 nov 2016
 *      Author: Nicola 
 */

#include <webui.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void kore_preload() {
    global_attributes=attrinit();
    int rv;

    /**/
    if (check_if_reset_admin_device()) {
        rv=reset_admin_device();
    } else {
        rv = sql_select_into_attributes( "SELECT macaddr,ip4,ip6 FROM found WHERE admin='yes'",
            "admin_device",
            &global_attributes);
    }

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
    return !(rv==0);
}


int reset_admin_device(){
    int rv=sqlexecute("UPDATE found set admin=null",&global_attributes);
    if (rv==KORE_RESULT_OK) {
        unlink(RESET_ADMIN_FILE);
    }
    return rv;
}
