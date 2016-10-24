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

#ifndef __EPOCH_H__
#define __EPOCH_H__

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


/* Get the system time into *TS.  */

void gettime (struct timespec *ts) {
#if HAVE_NANOTIME
	nanotime (ts);
#else

# if defined CLOCK_REALTIME && HAVE_CLOCK_GETTIME
	if (clock_gettime (CLOCK_REALTIME, ts) == 0)
		return;
# endif

	{
		struct timeval tv;
		gettimeofday (&tv, NULL);
		ts->tv_sec = tv.tv_sec;
		ts->tv_nsec = tv.tv_usec * 1000;
	}

#endif
}

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
        gmtime_r(&epoch_t,&ts);
        strftime(utc, MAX, "%Y-%m-%dT%H:%M:%SZ", &ts);
    }

    // in any case return time_t value
    return(epoch_t);

}

int utc2epoch(char *utc, char *epoch) {
// TODO:
    return(0);
}

time_t ts2epoch(struct timeval *tv, char *epoch) {

    if(epoch) // if not NULL render string
        snprintf(epoch,MAX,"%lu",tv->tv_sec);
    // in any case return time_t value
    return(tv->tv_sec);
}

#endif
