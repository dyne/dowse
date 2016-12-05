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
    WEBUI_DEBUG
    char p[256];
    char *H=getenv(H);
    if (H) {
        sprintf(p,"%s/run/webui.pid",H);
    } else {
        sprintf(p,"%s/.dowse/run/webui.pid",getenv("HOME"));
    }
    save_running_pid(p);
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

int save_running_pid(char*path){

    FILE *fp;
    fp=fopen(path,"w");
    if (!fp) {
        kore_log(LOG_ERR,"Error on opening [%s] : %s",path,strerror(errno));
        return -1;
    }
    fprintf(fp,"%d",getpid());
    fclose(fp);
}
