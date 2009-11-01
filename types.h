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

#ifndef TYPES_H
#define TYPES_H

#if defined(SCALAR_FLOAT)
typedef float scalar;
#elif defined(SCALAR_DOUBLE)
typedef double scalar;
#elif defined(SCALAR_LONG_DOUBLE)
typedef long double scalar;
#else
#warning "no scalar type macro defined, defaulting to float"
#define SCALAR_FLOAT
typedef float scalar;
#endif

typedef struct
{
	scalar x, y, z;
} vector;

typedef struct
{
	union
	{
		struct { scalar x, y, z; };
		vector v;
	};
	scalar w;
} quat;

#define ROW_MAJOR 0
#define COL_MAJOR 1

typedef struct
{
	int type;
	scalar m[4][4];
} mat44;

/* Some macros that work with types. */

/* 1 if the type of the given expression is integral, 0 otherwise. */
#define isint(x) \
	( \
		   __builtin_types_compatible_p(typeof(x), char) \
		|| __builtin_types_compatible_p(typeof(x), short) \
		|| __builtin_types_compatible_p(typeof(x), int) \
		|| __builtin_types_compatible_p(typeof(x), long) \
		|| __builtin_types_compatible_p(typeof(x), long long) \
		|| __builtin_types_compatible_p(typeof(x), unsigned char) \
		|| __builtin_types_compatible_p(typeof(x), unsigned short) \
		|| __builtin_types_compatible_p(typeof(x), unsigned int) \
		|| __builtin_types_compatible_p(typeof(x), unsigned long) \
		|| __builtin_types_compatible_p(typeof(x), unsigned long long) \
	)

/* 1 if the type of the given expression is a floating point type,
 * 0 otherwise. */
#define isfloat(x) \
	( \
		   __builtin_types_compatible_p(typeof(x), float) \
		|| __builtin_types_compatible_p(typeof(x), double) \
		|| __builtin_types_compatible_p(typeof(x), long double) \
	)

#endif /* TYPES_H */
