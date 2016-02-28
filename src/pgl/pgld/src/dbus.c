// pgld - D-Bus messaging interface
//
// Copyright (C) 2011 Dimitris Palyvos-Giannas <jimaras@gmail.com>
// Copyright (C) 2009 Cader <cade.robinson@gmail.com>
// Copyright (C) 2008 jpv <jpv950@gmail.com>
// Copyright (C) 2008 Jindrich Makovicka <makovick@gmail.com>
//
// This file is part of pgl.
//
// pgl is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pgl is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pgl.  If not, see <http://www.gnu.org/licenses/>.


#include "dbus.h"

static DBusConnection *dbconn = NULL;

// use_dbus is still 0, so in do_log calls dbus is still disabled.
int pgl_dbus_init() {

    // Object representing an exception.
    DBusError dberr;

    int req;

    // Initializes a DBusError structure.
    dbus_error_init (&dberr);

    dbconn = dbus_bus_get (DBUS_BUS_SYSTEM, &dberr);
    if (dbus_error_is_set (&dberr)) {
        do_log(LOG_ERR, "ERROR: Connection Error (%s)\n", dberr.message);
        dbus_error_free(&dberr);
    }
    if (dbconn == NULL) {
        do_log(LOG_ERR, "ERROR: dbconn is NULL.\n");
        return -1;
    }

    req = dbus_bus_request_name (dbconn, PGL_DBUS_PUBLIC_NAME,
        DBUS_NAME_FLAG_DO_NOT_QUEUE, &dberr); /* TODO: DBUS_NAME_FLAG_ALLOW_REPLACEMENT goes here */
    if (dbus_error_is_set (&dberr)) {
        do_log(LOG_ERR, "ERROR: Error requesting name %s.\n", dberr.message);
        dbus_error_free(&dberr);
        return -1;
    }
    if (req == DBUS_REQUEST_NAME_REPLY_EXISTS) {
        /*TODO: req = dbus_bus_request_name (dbconn, NFB_DBUS_PUBLIC_NAME,
            DBUS_NAME_FLAG_REPLACE_EXISTING, &dberr); */
        do_log(LOG_WARNING, "WARN: pgld dbus is already initialized.\n");
        return -1;
    }

    do_log(LOG_INFO, "INFO: Connected to dbus system bus.");

    return 0;
}

// use_dbus now is 1, so in do_log dbus is enabled.
// Therefore use do_log_xdbus for error messages.
void pgl_dbus_send(const char *format, va_list ap) {

    if (dbconn == NULL) {
        do_log_xdbus(LOG_ERR, "ERROR: dbus_send() called with NULL connection.\n");
        exit(1);
    }

    dbus_uint32_t serial = 0; // unique number to associate replies with requests
    DBusMessage *dbmsg = NULL;
    DBusMessageIter dbiter;
    char *msgPtr;
    char msg[MSG_SIZE];
    vsnprintf(msg, sizeof msg, format, ap);
    msgPtr = msg;
    /* create dbus signal */
    dbmsg = dbus_message_new_signal ("/org/netfilter/pgl",
                                     "org.netfilter.pgl",
                                     "pgld_message");

    if (dbmsg == NULL) {
        do_log_xdbus(LOG_ERR, "ERROR: NULL dbus message.\n");
        exit(1);
    }

    dbus_message_iter_init_append(dbmsg, &dbiter);
    if (!dbus_message_iter_append_basic(&dbiter, DBUS_TYPE_STRING, &msgPtr)) {
        /*The API needs a double pointer, otherwise causes segfault*/
        do_log_xdbus(LOG_ERR, "ERROR: dbus_message_iter_append_basic - out of memory.\n");
    }
    if (!dbus_connection_send (dbconn, dbmsg, &serial)) {
        do_log_xdbus(LOG_ERR, "ERROR: dbus_connection_send - out of memory.\n");
    }
    dbus_connection_flush(dbconn);
    dbus_message_unref(dbmsg);

}
