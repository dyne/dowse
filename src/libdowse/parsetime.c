/*  Dowse - embedded WebUI based on Kore.io
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
 *
 */

#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <string.h>

int relative_time(char *local_time, char *out) {
    time_t nowutc;
    time_t now;
    time_t then;
    long int deltaSeconds;
    long int deltaMinutes;

    struct tm tt;
    // parse utc into a tm struct
    memset(&tt,0,sizeof(struct tm));
    if( ! strptime(local_time, "%Y-%m-%dT%H:%M:%SZ", &tt) ) {
        // kore_log(LOG_ERR,"relative_time failed parsing UTC string: %s",utc);
        if( ! strptime(local_time, "%Y-%m-%d %H:%M:%S", &tt) ) {
            if( ! strptime(local_time, "%s", &tt) ) {
                sprintf(out,"n/a");
                return 1;
            }
        }
    }
    // gets what UTC time is now
    time(&nowutc);
    struct tm *now_tt = localtime(&nowutc);

    now = mktime(now_tt);

    tt.tm_isdst = now_tt->tm_isdst;
    then = mktime(&tt);

    // localtime_r( &rnow, &now );
    deltaSeconds = nowutc - then;
    deltaMinutes = (deltaSeconds / 60);


    if(deltaSeconds < 60)
        snprintf(out,256,"%ld seconds ago", deltaSeconds);
    else if (deltaMinutes < 60)
        snprintf(out,256,"%ld minutes ago", deltaMinutes);
    else if (deltaMinutes < (24 * 60))
        snprintf(out,256,"%d hours ago",(int)floor(deltaMinutes/60));
    else if (deltaMinutes < (24 * 60 * 7))
        snprintf(out,256,"%d days ago",(int)floor(deltaMinutes/(60 * 24)));
    else if (deltaMinutes < (24 * 60 * 31))
        snprintf(out,256,"%d weeks ago",(int)floor(deltaMinutes/(60 * 24 * 7)));
    else if (deltaMinutes < (24 * 60 * 365.25))
        snprintf(out,256,"%d months ago",(int)floor(deltaMinutes/(60 * 24 * 30)));
    else
        snprintf(out,256,"%d years ago",(int)floor(deltaMinutes/(60 * 24 * 365)));

    return 0;
}

