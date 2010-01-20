/* vim:set sw=4 ts=4:
 *
 * Copyright (C) 2010  Pim Goossens
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

/* INFO and ERROR are used like printf(). */

#define INFO(x...) fprintf(stderr, x)

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

#include "debug.h"
#include "mem.h"
#include "types.h"
#include "endian.h"
#include "global.h"
#include "scalar.h"
#include "vector.h"
#include "quaternion.h"
#include "config.h"
#include "lua_binread.h"
#include "file.h"
#include "time.h"
#include "model.h"
#include "audio.h"
#include "gfx.h"
#include "input.h"
#include "game.h"

#endif /* COMMON_H */
