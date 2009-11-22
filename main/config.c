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

#include "../include/common.h"

int config_get_int(const char *opt)
{
	int i;
	lua_getglobal(L1, "config");
	lua_getfield(L1, -1, opt);
	if (lua_isboolean(L1, -1))
		i = lua_toboolean(L1, -1);
	else
		i = luaL_checkint(L1, -1);
	lua_pop(L1, 2);
	return i;
}

float config_get_float(const char *opt)
{
	float f;
	lua_getglobal(L1, "config");
	lua_getfield(L1, -1, opt);
	f = (float) luaL_checknumber(L1, -1);
	lua_pop(L1, 2);
	return f;
}

/* NOTE: static storage of at most 8 option values. */
char *config_get_str(const char *opt)
{
	static char str[256*8];
	static int rot = 0;
	lua_getglobal(L1, "config");
	lua_getfield(L1, -1, opt);
	strncpy(&str[256*rot], luaL_checkstring(L1, -1), 256);
	str[256*rot+255] = '\0';
	lua_pop(L1, 2);
	rot = (rot + 1) % 8;
	return str;
}
