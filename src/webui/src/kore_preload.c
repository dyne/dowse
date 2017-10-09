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

extern redisContext *log_redis;

// declared in webui_preload
extern redisContext *redis_storage;

// this is called on initialisation

void kore_parent_configure() {
    log_entering();
    assetmap = asset_init();

    _init_performance(&perf_cont);
    startup_attributes = attrinit();
    error_during_startup=0;

    log_redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
    if (!log_redis) {
        const char m[] = "Error connecting to redis dynamic database for logging";
        webui_add_error_message(&startup_attributes, m);
        err(m);

        error_during_startup =1;
    }

    redis_storage = connect_redis(REDIS_HOST, REDIS_PORT, db_storage);
    if (!redis_storage) {
	    const char m[] = "Error connecting to redis storage database";
	    webui_add_error_message(&startup_attributes, m);
	    err(m);
	    error_during_startup =1;
    }

    act("Kore preload");
    if (check_if_reset_admin_device()) {
        WEBUI_DEBUG
        reset_admin_device();
    }


    /* Setup in redis the authorization-mac-* entry for all authorized */
    setup_authorization(&startup_attributes);

    /* Setup in redis the dns-lease-* entry for all things recognized and gived that a "name" */
    setup_dns_lease_name(&startup_attributes);

    /* Setup party_mode */
    setup_party_mode(&startup_attributes);


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

