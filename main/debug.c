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

#define DEBUG(level, x...) DEBUGX(DBG_GENERAL, level, x)

#ifdef NDEBUG
static int lua_debug_print(lua_State *L)
{
	return 0;
}

static int lua_debug_level(lua_State *L)
{
	if (lua_gettop(L) == 0) /* Query debug level - return 0. */
	{
		lua_pushinteger(L, 0);
		return 1;
	}
	else /* Attempt to set debug level - no-op. */
		return 0;
}
#else /* !NDEBUG */
static int lua_debug_print(lua_State *L)
{
	int type;
	int level;
	int n;

	n = lua_gettop(L);

	if (n != 3)
		luaL_error(L, "invalid number of arguments (expected 3, got %d)\n", n);
	type = luaL_checkint(L, 1);
	level = luaL_checkint(L, 2);
	DEBUGX(type, level, "%s\n", luaL_checkstring(L, 3));

	return 0;
}

static int lua_debug_level(lua_State *L)
{
	int argc;
	int n;

	argc = lua_gettop(L);
	/* No arguments -> query debug level. */
	if (argc == 0)
	{
		lua_pushinteger(L, debug_level);
		return 1;
	}
	else if (argc == 1)
	{
		/* One argument -> change debug level. */
		n = lua_tointeger(L, 1);
		/* Output debug message if either old or new level is > 0. */
		if (((n + debug_level) & DBG_LEVELMASK) > 0)
		{
			DEBUG(0, "Debug level set to %d\n", n);
		}
		debug_level = n;
	}
	else
	{
		luaL_error(L, "too many arguments (expected 1, got %d)", argc);
	}
	return 0;
}
#endif /* !NDEBUG */

static const struct luaL_Reg funcs[] =
{
	 { "DEBUGX", lua_debug_print }
	,{ "debuglevel", lua_debug_level }
	,{ NULL, NULL }
};

void debug_init(void)
{
	const struct luaL_Reg *f;

	debug_level = 0;

	/* Export functions to Lua environment. */
	for (f=funcs; f->name; f++)
	{
		lua_pushcfunction(L1, f->func);
		lua_setglobal(L1, f->name);
	}
}
