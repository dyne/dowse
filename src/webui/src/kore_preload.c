/*
 * kore_preload.c
 *
 *  Created on: 23 nov 2016
 *      Author: Nicola 
 */

#include <webui.h>

 extern redisContext *log_redis;

void kore_preload() {
    log_entering();

    startup_attributes = attrinit();
    error_during_startup=0;

    log_redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
    if (!log_redis) {
        const char m[] = "Redis server is not running";
        webui_add_error_message(&startup_attributes, m);
        err(m);

        error_during_startup =1;
    }
    act("Kore preload");

    /* Setup in redis the authorization-mac-* entry for all authorized */
    setup_authorization(&startup_attributes);

    /* Setup in redis the dns-lease-* entry for all things recognized and gived that a "name" */
    setup_dns_lease_name(&startup_attributes);

    /* */
    if (load_global_attributes(startup_attributes)!=KORE_RESULT_OK){
        error_during_startup=1;
    }

    if (error_during_startup ) {
        /* La welcome page gestira' gli errori del kore_preload() */
        err("Failed to startup Error during startup webui");
      //  exit(-1);
    }

}

int check_if_reset_admin_device() {
    log_entering();

    struct stat stat_buf;
    int rv;

    /* return if present RESET_ADMIN_FILE */
    rv = stat(RESET_ADMIN_FILE, &stat_buf);
    if (rv == 0) {

    }

    /* */
    return (rv == 0);
}

int reset_admin_device() {
    log_entering();

//    int rv = sqlexecute("UPDATE found SET admin=null", &global_attributes);
    int rv = sqlexecute("UPDATE found SET admin=null", &startup_attributes);
    if (rv == KORE_RESULT_OK) {
        unlink(RESET_ADMIN_FILE);
    }
    return rv;
}

