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

int admin_should_handle_event(char*macaddr) {
    char key[256];
    char value[256];
    sprintf(key,"authorization-mac-%s",macaddr);
    redis_get_key(key,value,sizeof(value));

    return (strcmp(value,__R_ADMIN_SHOULD_CHECK)==0);

}

int redis_get_key(const char* key, char*value, int sizeof_value) {
    redisContext *redis = NULL;
    redisReply *reply = NULL;
    /* Connecting with Redis */
    redis = connect_redis(REDIS_HOST, REDIS_PORT, db_dynamic);
    if (!redis) {
        const char m[] = "Redis server is not running";
        err(m);
        return KORE_RESULT_ERROR;
    }

    /* Print command on redis channel */
    reply = cmd_redis(redis, "GET %s", key);
    if (reply->len) {
        snprintf(value, sizeof_value, reply->str);
    } else {
        value[0] = 0;
    }
    /* Free resources */
    if (reply)
        freeReplyObject(reply);
    if (redis)
        redisFree(redis);
    return KORE_RESULT_OK;

}

int there_are_event_not_recognized() {
    char sql[] =
            "SELECT * FROM event WHERE not recognized and description='new_mac_address'";
    attributes_set_t dummy_attribute;
    dummy_attribute=attrinit();
    int rv = sql_select_into_attributes(sql, "dummy", &dummy_attribute);
    attrfree(dummy_attribute);
    func(" admin_should_handle_event = %d", rv);
    if (rv > 0) {
        return 1;
    } else {
        return 0;
    }
}
