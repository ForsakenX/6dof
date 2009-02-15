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

#ifndef VECTOR_H
#define VECTOR_H

#include "common.h"

/* The void functions all write their results to the vector pointed
 * to by their first parameter. Such `destination' parameters may
 * equal any of their source parameters and the function will still
 * produce correct results. */

/* Return the squared length (magnitude) of a vector. */
scalar vec_mag2(vector *v);

/* Return the length of a vector. */
scalar vec_mag(vector *v);

/* v <- v1 normalized. If v1 is the null vector, an error is
 * printed and the functions returns without modifying v. */
void vec_norm(vector *v, vector *v1);

/* v <- v1 negated, such that v will have the same length but point
 * in the opposite direction. */
void vec_neg(vector *v, vector *v1);

/* v <- v1 + v2 */
void vec_add(vector *v, vector *v1, vector *v2);

/* v <- v1 - v2 */
void vec_sub(vector *v, vector *v1, vector *v2);

/* Returns the squared distance between v1 and v2. */
scalar vec_dist2(vector *v1, vector *v2);

/* Returns the distance between v1 and v2. */
scalar vec_dist(vector *v1, vector *v2);

/* Returns the dot product of v1 and v2. */
scalar vec_dotprod(vector *v1, vector *v2);

/* Returns the dot product of v and the vector (x, y, z). */
scalar vec_dotprod_imm(vector *v, scalar x, scalar y, scalar z);

/* v <- v1 x v2 (the cross product of v1 and v2). */
void vec_crossprod(vector *v, vector *v1, vector *v2);

/* Rotates v as indicated by the quaternion q. */
void vec_rotate(vector *v, quat *q);

/* [-0, +1, -]
 *
 * Pushes the vector v onto the Lua stack. */
void lua_pushvector(lua_State *L, vector *v);

/* [-0, +1, -]
 *
 * Pushes the vector (x, y, z) onto the Lua stack. */
void lua_pushvector_imm(lua_State *L, scalar x, scalar y, scalar z);

/* [-0, +0, -]
 *
 * Writes the vector (3-number table) value at index to v. */
void lua_tovector(lua_State *L, int index, vector *v);

/* [-0, +0, v]
 *
 * Like lua_tovector but checks that the value is a vector. */
void luaL_checkvector(lua_State *L, int index, vector *v);

#endif /* VECTOR_H */
