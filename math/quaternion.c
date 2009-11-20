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

#include "common.h"

scalar quat_len2(const quat *q)
{
	return q->x*q->x + q->y*q->y
	     + q->z*q->z + q->w*q->w;
}

scalar quat_len(const quat *q)
{
	return sqrts(quat_len2(q));
}

void quat_norm(quat *q, const quat *q1)
{
	scalar c = quat_len2(q1);
	if (c < 0.000001f)
	{
		ERROR("trying to normalize (near) null quaternion (%g, %g, %g, %g)\n"
			, q1->x, q1->y, q1->z, q1->w
		);
		return;
	}
	if (ABS(c-1.0f) < 0.000001f)
		/* Normalized enough. */
		return;
	c = (scalar) 1.0f / sqrts(c);
	q->x = q1->x * c;
	q->y = q1->y * c;
	q->z = q1->z * c;
	q->w = q1->w * c;
}

void quat_conj(quat *q, const quat *q1)
{
	q->x = -q1->x;
	q->y = -q1->y;
	q->z = -q1->z;
}

void quat_mul(quat *q, const quat *q1, const quat *q2)
{
	quat tmp;

	tmp.x = q1->w*q2->x + q1->x*q2->w + q1->y*q2->z - q1->z*q2->y;
	tmp.y = q1->w*q2->y - q1->x*q2->z + q1->y*q2->w + q1->z*q2->x;
	tmp.z = q1->w*q2->z + q1->x*q2->y - q1->y*q2->x + q1->z*q2->w;
	tmp.w = q1->w*q2->w - q1->x*q2->x - q1->y*q2->y - q1->z*q2->z;

	*q = tmp;
}

void quat_mul_imm(quat *q, scalar x, scalar y, scalar z, scalar w)
{
	quat tmp;

	tmp.x = q->w*x + q->x*w + q->y*z - q->z*y;
	tmp.y = q->w*y - q->x*z + q->y*w + q->z*x;
	tmp.z = q->w*z + q->x*y - q->y*x + q->z*w;
	tmp.w = q->w*w - q->x*x - q->y*y - q->z*z;

	*q = tmp;
}

void quat_make_aa(quat *q, scalar angle, scalar x, scalar y, scalar z)
{
	angle /= (scalar) 2.0f;
	q->x = x*sins(angle);
	q->y = y*sins(angle);
	q->z = z*sins(angle);
	q->w = coss(angle);
}

void quat_make_euler(quat *q, scalar yaw, scalar pitch, scalar roll)
{
	scalar sin_y, sin_p, sin_r;
	scalar cos_y, cos_p, cos_r;

	yaw *= (scalar) M_PI / (scalar) 360.0f;
	pitch *= (scalar) M_PI / (scalar) 360.0f;
	roll *= (scalar) M_PI / (scalar) 360.0f;
	sin_y = sins(yaw);
	sin_p = sins(pitch);
	sin_r = sins(roll);
	cos_y = coss(yaw);
	cos_p = coss(pitch);
	cos_r = coss(roll);
	q->x = cos_r*sin_p*cos_y + sin_r*cos_p*sin_y;
	q->y = cos_r*cos_p*sin_y - sin_r*sin_p*cos_y;
	q->z = sin_r*cos_p*cos_y - cos_r*sin_p*sin_y;
	q->w = cos_r*cos_p*cos_y + sin_r*sin_p*sin_y;
	quat_norm(q, q);
}
