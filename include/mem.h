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

#ifndef MEM_H
#define MEM_H

/* Alloc + check macros. */

#define MALLOC(type, num) \
	({ \
		DEBUGX(DBG_MEM, 3, "malloc(%d, %zd /* %s */) = ", \
			num, sizeof(type), #type \
		); \
		void *_p = malloc(num * sizeof(type)); \
		if (!_p) \
		{ \
			ERROR("malloc(%d * sizeof(%s)) failed", num, #type); \
			luaL_error(L1, "malloc() failed"); \
		} \
		DEBUGX(DBG_MEM, 3, "%p\n", _p); \
		_p; \
	})

#define CALLOC(type, num) \
	({ \
		DEBUGX(DBG_MEM, 3, "calloc(%d, %zd /* %s */) = ", \
			num, sizeof(type), #type \
		); \
		void *_p = calloc(num, sizeof(type)); \
		if (!_p) \
		{ \
			ERROR("calloc(%d, sizeof(%s)) failed", num, #type); \
			luaL_error(L1, "calloc() failed"); \
		} \
		DEBUGX(DBG_MEM, 3, "%p\n", _p); \
		_p; \
	})

/* malloc() and calloc() are typically used to allocate space for
 * a single new object, so here are some separate macros for it. */
#define MALLOC1(type) MALLOC(type, 1)
#define CALLOC1(type) CALLOC(type, 1)

/* realloc() is typically used to allocate space for a list or
 * resize a list to some number of elements. */
#define REALLOC(list, num) \
	DEBUGX(DBG_MEM, 3, "realloc(%s /* %p */, %d * %zd) = ", \
		#list, list, (int) (num), sizeof(*(list)) \
	); \
	if ((num) > 0) \
	{ \
		list = realloc(list, (num) * sizeof(*(list))); \
		DEBUGX(DBG_MEM, 3, "%p\n", list); \
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
		DEBUGX(DBG_MEM, 3, "memcpy(%p, %p, %d * %zd)\n", \
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
	DEBUGX(DBG_MEM, 3, "memset(%p, %d, %d * %zd)\n", \
		mem, value, n, sizeof(*(mem)) \
	); \
	memset(mem, value, (n) * sizeof(*(mem)));

#define FREE(x) \
	if (x) \
	{ \
		DEBUGX(DBG_MEM, 3, "free(%p)\n", x); \
		free(x); \
		(x) = NULL; \
	} \
	else \
	{ \
		ERROR("attempted to free null pointer " #x); \
	}

#endif /* MEM_H */
