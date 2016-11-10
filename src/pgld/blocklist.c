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


#include "blocklist.h"
#include "pgld.h"

void blocklist_init() {
    blocklist.entries = NULL;
    blocklist.count = 0;
    blocklist.size = 0;
    blocklist.numips = 0;
}

void blocklist_append(uint32_t ip_min, uint32_t ip_max, const char *name, iconv_t ic) {
    block_entry_t *e;
    // if blocklist is full add 1024 more entries - used to be 16384
    // lowered because you could be possibly using (X-1)*20 bytes extra mem
    // where X is the blocklist->size += X; below
    if (blocklist.size == blocklist.count) {
        blocklist.size += ALLOC_CHUNK;
        blocklist.entries = realloc(blocklist.entries, sizeof(block_entry_t) * blocklist.size);
        CHECK_OOM(blocklist.entries);
    }
    e = blocklist.entries + blocklist.count;
    e->ip_min = ip_min;
    e->ip_max = ip_max;
#ifndef LOWMEM
    if (ic >= 0) {
        char buf2[MAX_LABEL_LENGTH];
        size_t insize, outsize;
        char *inb, *outb;
        int ret;

        insize = strlen(name);
        inb = (char *)name;
        outsize = MAX_LABEL_LENGTH - 1;
        outb = buf2;
        memset(buf2, 0, MAX_LABEL_LENGTH);
        ret = iconv(ic, &inb, &insize, &outb, &outsize);
        if (ret >= 0) {
            e->name = strdup(buf2);
        } else {
            do_log(LOG_ERR, "ERROR: Cannot convert string: %s", strerror(errno));
            e->name = strdup("(conversion error)");
        }
    } else {
        e->name = strdup(name);
    }
#endif
    e->hits = 0;
    blocklist.count++;
}

void blocklist_clear(uint32_t start) {
#ifndef LOWMEM
    uint32_t i;
    for (i = start; i < blocklist.count; i++) {
        if (blocklist.entries[i].name) {
            free(blocklist.entries[i].name);
        }
    }
#endif
    if (start == 0) {
        free(blocklist.entries);
        blocklist_init();
//         blocklist.entries = NULL;
//         blocklist.count = 0;
//         blocklist.size = 0;
    } else {
        blocklist.size = blocklist.count = start;
        blocklist.entries = realloc(blocklist.entries, sizeof(block_entry_t) * blocklist.size);
        CHECK_OOM(blocklist.entries);
    }
}

static int block_entry_compare(const void *a, const void *b) {
    const block_entry_t *e1 = a;
    const block_entry_t *e2 = b;
    if (e1->ip_min < e2->ip_min) return -1;
    if (e1->ip_min > e2->ip_min) return 1;
    return 0;
}

static int block_key_compare(const void *a, const void *b) {
    const block_entry_t *key = a;
    const block_entry_t *entry = b;
    if (key->ip_max < entry->ip_min) return -1;
    if (key->ip_min > entry->ip_max) return 1;
    return 0;
}

void blocklist_sort() {
    qsort(blocklist.entries, blocklist.count, sizeof(block_entry_t), block_entry_compare);
}

void blocklist_merge () {
    uint32_t i, j, k, merged=0, ip_max=0;
    blocklist.numips = 0;

    if (blocklist.count == 0) {
        return;
    }

    for (i = 0; i < blocklist.count; i++) {
#ifndef LOWMEM
        //truncate name to MAX_INMEMLABEL_LENGTH
        if ( strlen(blocklist.entries[i].name) > MAX_INMEMLABEL_LENGTH) {
            blocklist.entries[i].name=realloc(blocklist.entries[i].name, MAX_INMEMLABEL_LENGTH);
            blocklist.entries[i].name[MAX_INMEMLABEL_LENGTH-1]='\0';
        }
#endif
        ip_max = blocklist.entries[i].ip_max;

        //look at the next entries to see if they can merge or are the same
        for (j = i + 1; j < blocklist.count; j++) {
            if (ip_max == 4294967295UL ) {
                j=blocklist.count;
                break;
            }
            if (blocklist.entries[j].ip_min > ip_max + 1) {
                break;
            }
            if (blocklist.entries[j].ip_max > ip_max) {
                ip_max = blocklist.entries[j].ip_max;
            }

        }
        if (j > i + 1) {
            // set i max to new max
            blocklist.entries[i].ip_max = ip_max;
            // go through merged elements and blank them
            for (k = i + 1; k < j; k++) {
#ifndef LOWMEM
                if ( (strlen(blocklist.entries[i].name) + strlen(blocklist.entries[k].name)) <  MAX_INMEMLABEL_LENGTH && !strstr(blocklist.entries[i].name, blocklist.entries[k].name) ) {
                    blocklist.entries[i].name = realloc(blocklist.entries[i].name, strlen(blocklist.entries[i].name) + strlen(blocklist.entries[k].name) +4);
                    strcat(blocklist.entries[i].name, " | ");
                    strcat(blocklist.entries[i].name, blocklist.entries[k].name);
                }
                free(blocklist.entries[k].name);
                blocklist.entries[k].name = '\0';
#endif
                blocklist.entries[k].ip_min=4294967295UL; //set to 255.255.255.255 so sort push this element to bottom
                blocklist.entries[k].ip_max=4294967295UL; //set to 255.255.255.255 so sort push this element to bottom
                blocklist.entries[k].hits=0;
                merged++;
            } // end for k
#ifndef LOWMEM
            //truncate merged name to MAX_INMEMLABEL_LENGTH
            if ( strlen(blocklist.entries[i].name) > MAX_INMEMLABEL_LENGTH) {
                blocklist.entries[i].name=realloc(blocklist.entries[i].name, MAX_INMEMLABEL_LENGTH);
                blocklist.entries[i].name[MAX_INMEMLABEL_LENGTH-1]='\0';
            }
#endif
            i = j - 1;
        } //end if j
        blocklist.numips += (blocklist.entries[i].ip_max - blocklist.entries[i].ip_min);
        if (blocklist.numips < 4294967295UL) {
            blocklist.numips++;
        }
    } //end for i
    if (merged) {
        do_log(LOG_INFO, "INFO: Merged %u of %u entries.", merged, blocklist.count);
        blocklist_sort();
        blocklist.count -= merged;
        blocklist.entries = realloc(blocklist.entries, sizeof(block_entry_t) * blocklist.count );
        blocklist_merge();
    }
}

void blocklist_stats(int clearhits) {
    uint32_t i, total = 0;
    char pip_min[INET_ADDRSTRLEN],pip_max[INET_ADDRSTRLEN];
    do_log(LOG_INFO, "STATS: Blocked hit statistics:");
    for (i = 0; i < blocklist.count; i++) {
        if (blocklist.entries[i].hits >= 1) {
            int2ip(blocklist.entries[i].ip_min, pip_min);
            int2ip(blocklist.entries[i].ip_max, pip_max);
#ifndef LOWMEM
            do_log(LOG_INFO, "STATS: %s-%s: %s - %u hit(s)", pip_min, pip_max, blocklist.entries[i].name, blocklist.entries[i].hits);
#else
            do_log(LOG_INFO, "STATS: %s-%s: %u hit(s)", pip_min, pip_max, blocklist.entries[i].hits);
#endif
            total += blocklist.entries[i].hits;
            if (clearhits) {
                blocklist.entries[i].hits=0;
            }
        }
    }
    do_log(LOG_INFO, "STATS: %u hits total", total);
}

block_entry_t *blocklist_find(uint32_t ip) {
    block_entry_t e;
    e.ip_min = e.ip_max = ip;
    return bsearch(&e, blocklist.entries, blocklist.count, sizeof(block_entry_t), block_key_compare);
}

void blocklist_dump() {
    uint32_t i;
    char pip_min[INET_ADDRSTRLEN],pip_max[INET_ADDRSTRLEN];
    for (i = 0; i < blocklist.count; i++) {
        int2ip(blocklist.entries[i].ip_min,pip_min);
        int2ip(blocklist.entries[i].ip_max,pip_max);
#ifndef LOWMEM
        printf("%s:%s-%s\n", blocklist.entries[i].name, pip_min, pip_max);
#else
        printf("%s-%s\n", pip_min, pip_max);
#endif
    }
}
