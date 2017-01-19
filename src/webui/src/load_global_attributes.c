
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

int load_global_attributes() {
    log_entering();
    int rv;

    WEBUI_DEBUG
    /**/
    if (check_if_reset_admin_device()) {
        WEBUI_DEBUG
        rv=reset_admin_device();
    } else {
        func("load admin device into global attributes")
        rv = sql_select_into_attributes( "SELECT macaddr,ip4,ip6 FROM found WHERE admin='yes'",
            "admin_device",
            &global_attributes);
    }
    global_attributes=attrcat(global_attributes,"dowse_network_name","not yet available");

    return rv;
}
