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

void vec_norm(vector *v, vector *v1)
{
	scalar c = vec_mag2(v1);
	if (c < 0.000001f)
	{
		ERROR("trying to normalize (near) null vector (%g, %g, %g)\n"
			, v1->x, v1->y, v1->z
		);
		return;
	}
	if (ABS(c-1.0f) < 0.000001f)
		/* Normalized enough. */
		return;
	c = (scalar) 1.0f / sqrts(c);
	v->x = v1->x * c;
	v->y = v1->y * c;
	v->z = v1->z * c;
}

void vec_neg(vector *v, vector *v1)
{
	v->x = -v1->x;
	v->y = -v1->y;
	v->z = -v1->z;
}

void vec_add(vector *v, vector *v1, vector *v2)
{
	v->x = v1->x + v2->x;
	v->y = v1->y + v2->y;
	v->z = v1->z + v2->z;
}

void vec_sub(vector *v, vector *v1, vector *v2)
{
	v->x = v1->x - v2->x;
	v->y = v1->y - v2->y;
	v->z = v1->z - v2->z;
}

scalar vec_dist2(vector *v1, vector *v2)
{
	return SQ(v1->x - v2->x) + SQ(v1->y - v2->y) + SQ(v1->z - v2->z);
}

scalar vec_dist(vector *v1, vector *v2)
{
	return sqrts(vec_dist2(v1, v2));
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
	quat_conj(&q_inv, &q_inv);
	quat_mul(&vecq, &vecq, &q_inv);
	quat_mul(&vecq, q, &vecq);
	v->x = vecq.x;
	v->y = vecq.y;
	v->z = vecq.z;
}

void lua_pushvector(lua_State *L, vector *v)
{
	lua_getglobal(L, "vec");
	lua_pushnumber(L, v->x);
	lua_pushnumber(L, v->y);
	lua_pushnumber(L, v->z);
	lua_call(L, 3, 1);
}

void lua_pushvector_imm(lua_State *L, scalar x, scalar y, scalar z)
{
	lua_getglobal(L, "vec");
	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	lua_pushnumber(L, z);
	lua_call(L, 3, 1);
}

void lua_tovector(lua_State *L, int index, vector *v)
{
	if (index < 0)
		/* Index counts from top of stack. */
		index--;
	lua_pushinteger(L, 1);
	lua_gettable(L, index);
	v->x = (scalar) lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushinteger(L, 2);
	lua_gettable(L, index);
	v->y = (scalar) lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushinteger(L, 3);
	lua_gettable(L, index);
	v->z = (scalar) lua_tonumber(L, -1);
	lua_pop(L, 1);
}

void luaL_checkvector(lua_State *L, int index, vector *v)
{
	luaL_argcheck(L,
		 lua_istable(L, index) && lua_objlen(L, index) != 3
		,index
		,"not a vector"
	);
	if (index < 0)
		/* Index counts from top of stack. */
		index--;
	lua_pushinteger(L, 1);
	lua_gettable(L, index);
	v->x = (scalar) luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_pushinteger(L, 2);
	lua_gettable(L, index);
	v->y = (scalar) luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_pushinteger(L, 3);
	lua_gettable(L, index);
	v->z = (scalar) luaL_checknumber(L, -1);
	lua_pop(L, 1);
}

#endif /* VECTOR_H */
