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

#ifndef GLOBAL_H
#define GLOBAL_H

extern lua_State *L1;
extern int debug_level;

/* Checks whether a field has the given type. */
#define LUA_ISFIELDTYPE(L, index, name, type) \
	({ \
		int _b; \
		lua_getfield(L, index, name); \
		_b = lua_is ## type(L, -1); \
		lua_pop(L, 1); \
		_b; \
	})

/* Integer version of lua_getfield(). */
void lua_getfieldi(lua_State *L, int index, int n);

#endif /* GLOBAL_H */
