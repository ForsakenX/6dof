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

struct audio_driver *audio;

static int audio_luaload(lua_State *L)
{
	int n;
	const char *soundname;
	const char *filename;
	int id;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "invalid number of arguments (expected 2, got %d)\n", n);
	soundname = luaL_checkstring(L, 1);
	filename = luaL_checkstring(L, 2);
	lua_pop(L, 1);
	id = audio->load(filename);
	if (id >= 0)
	{
		lua_getglobal(L, "audio");
		lua_getfield(L, -1, "sounds");
		lua_pushinteger(L, id);
		lua_setfield(L, -2, soundname);
		lua_pop(L, 2);
		lua_pushboolean(L, ~0);
	}
	else
		lua_pushnil(L);

	return 1;
}

static int audio_luafree(lua_State *L)
{
	int n;
	const char *soundname;
	int soundno;

	n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "invalid number of arguments (expected 1, got %d)\n", n);
	soundname = luaL_checkstring(L, 1);
	lua_pop(L, 1);
	lua_getglobal(L, "audio");
	lua_getfield(L, -1, "sounds");
	lua_getfield(L, -1, soundname);
	soundno = luaL_checkint(L, -1);
	audio->free(soundno);
	lua_pop(L, 3);

	return 0;
}

static int audio_luaplay(lua_State *L)
{
	int n;
	const char *soundname;
	int soundno;
	int volume;
	float panning;
	int flags;
	int handle;

	n = lua_gettop(L);
	if (n != 4)
		return luaL_error(L, "invalid number of arguments (expected 1, got %d)\n", n);
	soundname = luaL_checkstring(L, 1);
	volume = luaL_checkint(L, 2);
	panning = luaL_checknumber(L, 3);
	flags = luaL_checkint(L, 4);
	lua_pop(L, 4);
	lua_getglobal(L, "audio");
	lua_getfield(L, -1, "sounds");
	lua_getfield(L, -1, soundname);
	soundno = luaL_checkint(L, -1);
	lua_pop(L, 3);
	handle = audio->play(soundno, volume, panning, flags);
	if (handle >= 0)
		lua_pushinteger(L, handle);
	else
		lua_pushnil(L);

	return 1;
}

static int audio_luaplay3d(lua_State *L)
{
	int n;
	const char *soundname;
	int soundno;
	vector pos;
	int volume;
	int flags;
	int handle;

	n = lua_gettop(L);
	if (n != 4)
		return luaL_error(L, "invalid number of arguments (expected 1, got %d)\n", n);
	soundname = luaL_checkstring(L, 1);
	luaL_checkvector(L, 2, &pos);
	volume = luaL_checkint(L, 3);
	flags = luaL_checkint(L, 4);
	lua_pop(L, 4);
	lua_getglobal(L, "audio");
	lua_getfield(L, -1, "sounds");
	lua_getfield(L, -1, soundname);
	soundno = luaL_checkint(L, -1);
	lua_pop(L, 3);
	handle = audio->play_at(soundno, pos.x, pos.y, pos.z, volume, flags);
	if (handle >= 0)
		lua_pushinteger(L, handle);
	else
		lua_pushnil(L);

	return 1;
}

static int audio_luastop(lua_State *L)
{
	int n;
	int handle;

	n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "invalid number of arguments (expected 1, got %d)\n", n);
	handle = luaL_checkint(L, 1);
	lua_pop(L, 1);
	audio->stop(handle);

	return 0;
}

void audio_luainit(void)
{
	const luaL_Reg funcs[] =
	{
		 { "load", audio_luaload }
		,{ "free", audio_luafree }
		,{ "play", audio_luaplay }
		,{ "play3d", audio_luaplay3d }
		,{ "stop", audio_luastop }
		,{ NULL, NULL }
	};
	luaL_register(L1, "audio", funcs);
	lua_newtable(L1);
	lua_setfield(L1, -2, "sounds");
	lua_pop(L1, 1);
}

int audio_init(void)
{
	int ret;

	DEBUG(1, "Initializing audio subsystem\n");
#ifdef AUDIO_SDLMIXER
	DEBUG(2, "Using SDL_mixer for audio\n");
	audio = &audio_sdlmixer;
#else
	DEBUG(2, "Using dummy driver for audio (silence)\n");
	audio = &audio_null;
#endif
	ret = audio->init();
	if (ret)
		return ret;
	audio_luainit();

	return 0;
}

void audio_shutdown(void)
{
	DEBUG(1, "Shutting down audio subsystem\n");
	audio->shutdown();
}
