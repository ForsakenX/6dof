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

#endif /* TYPES_H */
