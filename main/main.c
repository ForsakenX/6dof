/* vim:set sw=4 ts=4:
 *
 * Copyright (C) 2010  Pim Goossens
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

static int lua_create(void)
{
	/* luaL_Reg x[] = { { NULL, NULL } }; */

	L1 = luaL_newstate();
	if (!L1)
	{
		ERROR("error while initializing Lua!");
		return -1;
	}
	luaL_openlibs(L1);
	return 0;
}

static int lua_init(int argc, char *argv[])
{
	int err;
	int i;

	LUA_RUN("main/init.lua");
	lua_settop(L1, 0);
	lua_getglobal(L1, "init");    /* [bottom] init [top] */
	lua_createtable(L1, argc, 0); /* init, args */
	for (i=0; i<argc; i++)
	{
		lua_pushinteger(L1, i+1);
		lua_pushstring(L1, argv[i]);
		lua_settable(L1, -3);
	}
	/* Call Lua function init(argv) */
	err = lua_pcall(L1, 1, 0, 0);
	if (err)
		ERROR("%s", lua_tostring(L1, -1));
	return err;
}

static int init(int argc, char *argv[])
{
	CHECK(lua_create());
	CHECK(luafuncs_register(L1));
	debug_init();
	CHECK(lua_init(argc, argv));
#ifdef SDL
	CHECK(SDL_Init(0));
#endif
	CHECK(audio_init());
	CHECK(gfx_init());
	CHECK(input_init());
	CHECK(game_init());
	/* TODO: move elsewhere... */
	LUA_RUN("main/bindings.lua");
	return 0;
}

static void shutdown(void)
{
	game_shutdown();
	input_shutdown();
	audio_shutdown();
	gfx_shutdown();
#ifdef SDL
	SDL_Quit();
#endif
	lua_close(L1);
}

int main(int argc, char *argv[])
{
	int err;

	err = init(argc, argv);
	if (err)
		return err;
	if (!load_level(config_get_str("level")))
	{
		while (!game_main());
	}
	shutdown();
	return 0;
}
