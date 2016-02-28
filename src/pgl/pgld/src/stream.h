// pgld - stream reader
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


#ifndef INC_STREAM_H
#define INC_STREAM_H

#include <stdio.h>
#include <string.h>

#ifdef HAVE_ZLIB
#include <zlib.h>
#endif

#include "pgld.h"

#define CHUNK 1024

typedef struct {
    FILE *f;
#ifdef HAVE_ZLIB
    int compressed;
    int eos;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
#endif
} stream_t;

int stream_open(stream_t *stream, const char *filename);
int stream_close(stream_t *stream);
char * stream_getline(char *buf, int max, stream_t *stream);

#endif /* INC_STREAM_H */
