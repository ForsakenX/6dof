/* vim:set sw=4 ts=4:
 *
 * Copyright (C) 2009  Pim Goossens
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>, or write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */

#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <byteswap.h>

#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifdef SDL
#include <SDL/SDL.h>
#endif

#ifdef AUDIO_SDLMIXER
#include <SDL/SDL_mixer.h>
#endif

#ifdef OPENGL
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/* To check functions that return 0 on success, non-zero on failure. */
#define CHECK(x) \
	{ \
		int _err = x; \
		if (_err) return _err; \
	}

/* DEBUG is used like printf() but with an additional first
 * argument indicating the minimum debug level for the message. */
#ifdef NDEBUG
#define DEBUG(x...)
#else
#define DEBUG(level, x...) { if (debug_level >= level) fprintf(stderr, x); }
#endif

/* Used like printf(). */
#ifdef NDEBUG
#define ERROR(x...) \
	{ \
		fputs("Error: ", stderr); \
		fprintf(stderr, x); \
		fputs("\n", stderr); \
	}
#else /* !NDEBUG */
#define ERROR(x...) \
	{ \
		if (debug_level > 0) \
		{ \
			fprintf(stderr, "Error at %s:%s:%d: " \
				, __FILE__, __FUNCTION__, __LINE__ \
			); \
		} \
		else \
		{ \
			fputs("Error: ", stderr); \
		} \
		fprintf(stderr, x); \
		fputs("\n", stderr); \
	}
#endif /* !NDEBUG */

/* Load and run a Lua script, returning an error code if it failed. */
#define LUA_RUN(f) \
	{ \
		int _err = luaL_dofile(L1, f); \
		if (_err) \
		{ \
			fprintf(stderr, "%s(): Lua: %s\n", \
				__FUNCTION__, lua_tostring(L1, -1)); \
			return _err; \
		} \
	}

/* Some math macros usable for any numerical type. */
#define ABS(x) ((x)>=0?(x):-(x))
#define SQ(x) ((x)*(x))

/* Alloc + check macros. */

#define MALLOC(type, num) \
	({ \
		DEBUG(10, "malloc(%d, %zd /* %s */) = ", \
			num, sizeof(type), #type \
		); \
		void *_p = malloc(num * sizeof(type)); \
		if (!_p) \
		{ \
			ERROR("malloc(%d * sizeof(%s)) failed", num, #type); \
			luaL_error(L1, "malloc() failed"); \
		} \
		DEBUG(10, "%p\n", _p); \
		_p; \
	})

#define CALLOC(type, num) \
	({ \
		DEBUG(10, "calloc(%d, %zd /* %s */) = ", \
			num, sizeof(type), #type \
		); \
		void *_p = calloc(num, sizeof(type)); \
		if (!_p) \
		{ \
			ERROR("calloc(%d, sizeof(%s)) failed", num, #type); \
			luaL_error(L1, "calloc() failed"); \
		} \
		DEBUG(10, "%p\n", _p); \
		_p; \
	})

/* malloc() and calloc() are typically used to allocate space for
 * a single new object, so here are some separate macros for it. */
#define MALLOC1(type) MALLOC(type, 1)
#define CALLOC1(type) CALLOC(type, 1)

/* realloc() is typically used to allocate space for a list or
 * resize a list to some number of elements. */
#define REALLOC(list, num) \
	DEBUG(10, "realloc(%s /* %p */, %d * %zd) = ", \
		#list, list, (int) (num), sizeof(*(list)) \
	); \
	if ((num) > 0) \
	{ \
		list = realloc(list, (num) * sizeof(*(list))); \
		DEBUG(10, "%p\n", list); \
		if (!list) \
		{ \
			ERROR("realloc(%s, %d * %zd) failed", \
				#list, num, sizeof(*(list)) \
			); \
			luaL_error(L1, "realloc() failed"); \
		} \
	} \
	else \
	{ \
		ERROR("REALLOC invoked with num == %d\n", num); \
	}

/* memcpy() with type checking and inference. */
#define MEMCPY(dest, src, nelem) \
	({ \
		DEBUG(10, "memcpy(%p, %p, %d * %zd)\n", \
			dest, src, nelem, sizeof(*(dest)) \
		); \
		void *_p = \
			__builtin_choose_expr( \
				__builtin_types_compatible_p(typeof(*(dest)), typeof(*(src))), \
				memcpy(dest, src, nelem * sizeof(*(dest))), \
				(void) 0 \
			); \
		_p; \
	})

/* memset() with type inference (takes element count instead of
 * byte count. */
#define MEMSET(mem, value, n) \
	DEBUG(10, "memset(%p, %d, %d * %zd)\n", \
		mem, value, n, sizeof(*(mem)) \
	); \
	memset(mem, value, (n) * sizeof(*(mem)));

#define FREE(x) \
	if (x) \
	{ \
		DEBUG(10, "free(%p)\n", x); \
		free(x); \
		(x) = NULL; \
	} \
	else \
	{ \
		ERROR("attempted to free null pointer " #x); \
	}

#include "types.h"
#include "endian.h"
#include "global.h"
#include "scalar.h"
#include "vector.h"
#include "quaternion.h"
#include "config.h"
#include "lua_funcs.h"
#include "file.h"
#include "time.h"
#include "model.h"
#include "audio.h"
#include "gfx.h"
#include "input.h"
#include "game.h"

#endif /* COMMON_H */
