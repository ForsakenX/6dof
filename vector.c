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

scalar vec_mag2(vector *v)
{
	return v->x*v->x
	     + v->y*v->y
	     + v->z*v->z;
}

scalar vec_mag(vector *v)
{
	return sqrt(vec_mag2(v));
}

void vec_norm(vector *v)
{
	scalar mag = vec_mag2(v);
	if (ABS(mag-1.0f) < 0.000001f)
		/* Normalized enough. */
		return;
	mag = sqrt(mag);
	v->x /= mag;
	v->y /= mag;
	v->z /= mag;
}

void vec_neg(vector *v)
{
	v->x = -v->x;
	v->y = -v->y;
	v->z = -v->z;
}

void vec_add(vector *v, vector *v1, vector *v2)
{
	vector tmp;

	tmp.x = v1->x + v2->x;
	tmp.y = v1->y + v2->y;
	tmp.z = v1->z + v2->z;

	*v = tmp;
}

scalar vec_dist(vector *v1, vector *v2)
{
	return sqrts(
		  SQ(v1->x - v2->x)
		+ SQ(v1->y - v2->y)
		+ SQ(v1->z - v2->z)
	);
}

scalar vec_dotprod(vector *v1, vector *v2)
{
	return v1->x*v2->x
	     + v1->y*v2->y
	     + v1->z*v2->z;
}

scalar vec_dotprod_imm(vector *v, scalar x, scalar y, scalar z)
{
	return v->x*x
	     + v->y*y
	     + v->z*z;
}

void vec_crossprod(vector *v, vector *v1, vector *v2)
{
	vector tmp;

	tmp.x = v1->y*v2->z - v1->z*v2->y;
	tmp.y = v1->z*v2->x - v1->x*v2->z;
	tmp.z = v1->x*v2->y - v1->y*v2->x;

	*v = tmp;
}

void vec_rotate(vector *v, quat *q)
{
	vector tmpv;
	quat vecq, q_inv;

	/* v = q * v * q' */
	tmpv = *v;
	vecq.x = tmpv.x;
	vecq.y = tmpv.y;
	vecq.z = tmpv.z;
	vecq.w = 0.0f;
	q_inv = *q;
	quat_conj(&q_inv);
	quat_mul(&vecq, &vecq, &q_inv);
	quat_mul(&vecq, q, &vecq);
	v->x = vecq.x;
	v->y = vecq.y;
	v->z = vecq.z;
}

#endif /* VECTOR_H */
