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

scalar quat_len2(quat *q);
scalar quat_len(quat *q);
void quat_norm(quat *q);
void quat_conj(quat *q);
void quat_mul(quat *q, quat *q1, quat *q2);
void quat_mul_imm(quat *q, scalar x, scalar y, scalar z, scalar w);
void quat_make_aa(quat *q, scalar angle, scalar x, scalar y, scalar z);
void quat_make_euler(quat *q, scalar yaw, scalar pitch, scalar roll);

#endif /* QUATERNION_H */
