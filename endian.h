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

#ifndef ENDIAN_H
#define ENDIAN_H

#include "common.h"

#define SWAB(x) \
	( \
		__builtin_choose_expr( \
			isint(x), \
			( \
				__builtin_choose_expr( \
					sizeof(x) == 1, (x) \
					, \
					__builtin_choose_expr( \
						sizeof(x) == 2, bswap_16(x) \
						, \
						__builtin_choose_expr( \
							sizeof(x) == 4, bswap_32(x) \
							, \
							__builtin_choose_expr( \
								sizeof(x) == 8, bswap_64(x) \
								, \
								(void) 0 \
							) \
						) \
					) \
				) \
			) \
			, \
			(void) 0 \
		) \
	)

/* TODO: handle floating-point types.
 *
 * This is trickier since simply using the method above would cause
 * floating-point <-> integer conversions. There's also the question
 * of what *type* of value to return; if the swapped value is
 * returned as a float it could cause an unintended conversion later
 * on.
 *
 * So, floating-point byte-swapping support will be added later,
 * if/when it's needed.
 */

#if BYTE_ORDER == BIG_ENDIAN

#warning "This project has not yet been tested on big-endian systems."
#define BE(x) x
#define LE(x) SWAB(x)

#elif BYTE_ORDER == LITTLE_ENDIAN

#define BE(x) SWAB(x)
#define LE(x) x

#else
#error "Unknown or unsupported byte order"
#endif

#endif /* ENDIAN_H */
