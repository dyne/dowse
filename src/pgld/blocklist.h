// pgld - blocklist management
//
// Copyright (C) 2009 Cader <cade.robinson@gmail.com>
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


#ifndef INC_BLOCKLIST_H
#define INC_BLOCKLIST_H

#include <stdio.h>
#include <inttypes.h>
// #include <time.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <netinet/in.h>

/* iconv is not needed in LOWMEM mode (no strings handled) */
#ifndef LOWMEM
#include <iconv.h>
#else

#include "pgld.h"

typedef int iconv_t;
static inline iconv_t iconv_open(const char *tocode, const char *fromcode) {
    return 0;
}

static inline int iconv_close(iconv_t cd) {
    return 0;
}
#endif /* LOWMEM */

#define MAX_LINE_LENGTH 350
#define MAX_LABEL_LENGTH 300
#define MAX_INMEMLABEL_LENGTH 64
#define ALLOC_CHUNK 16384

typedef struct {
    uint32_t ip_min, ip_max;
#ifndef LOWMEM
    char *name;
#endif
    uint32_t hits;
} block_entry_t;

typedef struct {
    block_entry_t *entries;
    uint32_t count, size, numips;
} blocklist_t;

void blocklist_init();
void blocklist_append(uint32_t ip_min, uint32_t ip_max, const char *name, iconv_t ic);
void blocklist_clear(uint32_t start);
void blocklist_sort();
void blocklist_merge();
void blocklist_stats(int clearhits);
block_entry_t * blocklist_find(uint32_t ip);
void blocklist_dump();
blocklist_t blocklist;

#endif /* INC_BLOCKLIST_H */
