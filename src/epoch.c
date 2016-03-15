/*  Dowse - auxilary time functions
 *
 *  (c) Copyright 2016 Dyne.org foundation, Amsterdam
 *  Written by Denis Roio aka jaromil <jaromil@dyne.org>
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

 RFC3339 compliant conversion from and to epoch */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: #include <errno.h>

/* we are very faithful in incoming strings at the moment, relying on
 * correctness of upstream libraries as dnscap or pgl. Strict format
 * and error checking should be implemented soon. */

#include <time.h>

#define MAX 128

// assuming all buffers are allocated by the caller

int epoch2utc(char *epoch, char *utc) {
    struct tm  ts;
    time_t epoch_t;

    if( strncmp(epoch, "now", 3) == 0 ) {

        // Get current time
        time(&epoch_t);
        // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"

    } else {

        epoch_t = atoll(epoch);

    }

    if(utc) { // if not NULL render string
        ts = *gmtime(&epoch_t);
        strftime(utc, MAX, "%Y-%m-%dT%H:%M:%SZ", &ts);
    }

    // in any case return time_t value
    return(epoch_t);

}

int utc2epoch(char *utc, char *epoch) {
// TODO:
    return(0);
}

time_t ts2epoch(struct timeval *ts, char *epoch) {

    if(epoch) // if not NULL render string
        snprintf(epoch,MAX,"%lu",ts->tv_sec);
    // in any case return time_t value
    return(ts->tv_sec);
}
#ifdef TEST

int main(int argc, char **argv) {
    char epoch[128];
    char utc[128];

    if( strcmp(argv[1],"epoch2utc")==0 ) {
        epoch2utc(argv[2],utc);
        printf("UTC: %s\n",utc);
    }
    exit(0);
}

#endif
