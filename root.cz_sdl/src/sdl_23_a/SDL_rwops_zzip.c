/*
 *      Copyright (c) 2001 Guido Draheim <guidod@gmx.de>
 *      Use freely under the restrictions of the ZLIB License
 */

#include "SDL_rwops_zzip.h"

#define SDL_RWOPS_ZZIP_FILE(_context) \
             ((ZZIP_FILE*) (_context)->hidden.unknown.data1)

static int _zzip_seek(SDL_RWops *context, int offset, int whence)
{
    return zzip_seek(SDL_RWOPS_ZZIP_FILE(context), offset, whence);
}

static int _zzip_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
    return zzip_read(SDL_RWOPS_ZZIP_FILE(context), (char *)ptr, size*maxnum);
}

static int _zzip_write(SDL_RWops *context, const void *ptr, int size, int num)
{
    return 0; /* ignored */
}

static int _zzip_close(SDL_RWops *context)
{
    zzip_close (SDL_RWOPS_ZZIP_FILE(context));

    if ( context )
	SDL_FreeRW (context);

    return 0;
}

#ifndef O_BINARY
#define O_BINARY 0
#endif

SDL_RWops *SDL_RWFromZZIP(const char* file, const char* mode)
{
    register int mo = 0;
    register SDL_RWops* rwops;
    register ZZIP_FILE* zzip_file;

    for (; *mode; ++mode)
	switch (*mode)
	{
	case 'r': mo |= O_RDONLY;  break;
	case '+': mo |= ZZIP_CASEINSENSITIVE; break; /* == O_APPEND */
	case 'b': mo |= O_BINARY; break;
	case 'w': /* ouch! */  return 0;
	    /* default, 't', 'a', etc, just ignore */
	}

    zzip_file = zzip_open (file, mo);
    if (! zzip_file) return 0;

    rwops = SDL_AllocRW ();
    if (! rwops) { zzip_close (zzip_file); return 0; }

    SDL_RWOPS_ZZIP_FILE(rwops) = zzip_file;
    rwops->read = _zzip_read;
    rwops->write = _zzip_write;
    rwops->seek = _zzip_seek;
    rwops->close = _zzip_close;
    return rwops;
}

