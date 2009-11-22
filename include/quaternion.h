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

#ifndef QUATERNION_H
#define QUATERNION_H

#include "common.h"

/* The void functions all write their results to the quaternion
 * pointed to by their first parameter. Such `destination' parameters
 * may equal any of their source parameters and the function will
 * still produce correct results. */

/* Return the squared length (magnitude) of a quaternion. */
scalar quat_len2(const quat *q);

/* Return the length of a quaternion. */
scalar quat_len(const quat *q);

/* q <- q1 normalized. If q1 is the null quaternion, an error is
 * printed and the functions returns without modifying q. */
void quat_norm(quat *q, const quat *q1);

/* q <- q1 reversed, such that q will have the same angle but rotate
 * in the opposite direction. */
void quat_conj(quat *q, const quat *q1);

/* q <- q1 * q2, combining their rotations. */
void quat_mul(quat *q, const quat *q1, const quat *q2);

/* q <- q * (x, y, z, w). */
void quat_mul_imm(quat *q, scalar x, scalar y, scalar z, scalar w);

/* q <- the quaternion that represents the given axis-angle rotation. */
void quat_make_aa(quat *q, scalar angle, scalar x, scalar y, scalar z);

/* q <- the quaternion that represents the given Euler angle rotation. */
void quat_make_euler(quat *q, scalar yaw, scalar pitch, scalar roll);

#endif /* QUATERNION_H */
