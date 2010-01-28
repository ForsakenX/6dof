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
		void *_p; \
		DEBUGX(DBG_MEM, 3, "malloc(%zd * %zd /* %s */) = ", \
			(size_t) (num), sizeof(type), #type \
		); \
		_p = malloc((size_t) (num) * sizeof(type)); \
		if (!_p) \
		{ \
			ERROR("malloc(%zd * sizeof(%s)) failed", (size_t) (num), #type); \
			luaL_error(L1, "malloc() failed"); \
		} \
		DEBUGX(DBG_MEM, 3, "%p\n", _p); \
		_p; \
	})

#define CALLOC(type, num) \
	({ \
		void *_p; \
		DEBUGX(DBG_MEM, 3, "calloc(%zd, %zd /* %s */) = ", \
			(size_t) (num), sizeof(type), #type \
		); \
		_p = calloc((size_t) (num), sizeof(type)); \
		if (!_p) \
		{ \
			ERROR("calloc(%zd, sizeof(%s)) failed", (size_t) (num), #type); \
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
	DEBUGX(DBG_MEM, 3, "realloc(%s /* %p */, %zd * %zd) = ", \
		#list, list, (size_t) (num), sizeof(*(list)) \
	); \
	if ((num) > 0) \
	{ \
		list = realloc(list, (size_t) (num) * sizeof(*(list))); \
		DEBUGX(DBG_MEM, 3, "%p\n", list); \
		if (!list) \
		{ \
			ERROR("realloc(%s, %zd * %zd) failed", \
				#list, (size_t) (num), sizeof(*(list)) \
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
		void *_p; \
		DEBUGX(DBG_MEM, 3, "memcpy(%p, %p, %zd * %zd)\n", \
			dest, src, (size_t) (nelem), sizeof(*(dest)) \
		); \
		_p = \
			__builtin_choose_expr( \
				__builtin_types_compatible_p(typeof(*(dest)), typeof(*(src))), \
				memcpy(dest, src, (size_t) (nelem) * sizeof(*(dest))), \
				(void) 0 \
			); \
		_p; \
	})

/* memset() with type inference (takes element count instead of
 * byte count. */
#define MEMSET(mem, value, n) \
	DEBUGX(DBG_MEM, 3, "memset(%p, %d, %zd * %zd)\n", \
		mem, value, (size_t) (n), sizeof(*(mem)) \
	); \
	memset(mem, value, (size_t) (n) * sizeof(*(mem)));

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
