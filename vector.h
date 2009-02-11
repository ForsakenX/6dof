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

scalar vec_mag2(vector *v);
scalar vec_mag(vector *v);
void vec_norm(vector *v);
void vec_neg(vector *v);
void vec_add(vector *v, vector *v1, vector *v2);
scalar vec_dist(vector *v1, vector *v2);
scalar vec_dotprod(vector *v);
scalar vec_dotprod_imm(vector *v, scalar x, scalar y, scalar z);
void vec_crossprod(vector *v, vector *v1, vector *v2);
void vec_rotate(vector *v, quat *q);

#endif /* VECTOR_H */
