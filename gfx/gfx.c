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

#define DEBUG(level, x...) DEBUGX(DBG_GFX, level, x)

struct gfx_driver *gfx;

int lua_istexture(lua_State *L, int index)
{
	if (!lua_istable(L, index))
		return 0;
	if (!LUA_ISFIELDTYPE(L, index, "width", number))
		return 0;
	if (!LUA_ISFIELDTYPE(L, index, "height", number))
		return 0;
	if (!LUA_ISFIELDTYPE(L, index, "flags", number))
		return 0;
	if (!LUA_ISFIELDTYPE(L, index, "data", string))
		return 0;
	return 1;
}

int lua_ismaterial(lua_State *L, int index)
{
	if (!lua_istable(L, index))
		return 0;
	if (!LUA_ISFIELDTYPE(L, index, "flags", number))
		return 0;
	/* ... */
	return 1;
}

static int gfx_luanewtexture(lua_State *L)
{
	struct image tex;
	const char *data;
	int n;
	size_t len;
	int *handle;

	n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "invalid number of arguments (expected 1, got %d)\n", n);
	luaL_argcheck(L, lua_istexture(L, 1), 1, "not a texture");
	lua_getfield(L, 1, "width");
	tex.width = luaL_checkint(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 1, "height");
	tex.height = luaL_checkint(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 1, "flags");
	tex.flags = luaL_checkint(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, 1, "data");
	data = lua_tolstring(L, -1, &len);
	if (len != tex.width * tex.height * 3)
	{
		luaL_error(L,
			"texture data length (%d) does not correspond to "
			"the dimensions (%dx%d = %d) given",
			len, tex.width, tex.height, tex.width * tex.height
		);
	}
	tex.data = (char *) data;
	lua_settop(L, 0);
	n = gfx->prep_texture(&tex);
	if (n > 0)
	{
		handle = lua_newuserdata(L, sizeof(n));
		*handle = n;
		luaL_getmetatable(L, "texture");
		lua_setmetatable(L, 1);
	}
	else
		lua_pushnil(L);

	return 1;
}

void lua_tocolor(lua_State *L, int index, float *color)
{
	lua_getfieldi(L, index, 1);
	color[0] = (float) lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfieldi(L, index, 2);
	color[1] = (float) lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfieldi(L, index, 3);
	color[2] = (float) lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_getfieldi(L, index, 4);
	if (lua_isnumber(L, -1))
		color[3] = (float) lua_tonumber(L, -1);
	else
		color[3] = 1.0f;
	lua_pop(L, 1);
}

int lua_iscolor(lua_State *L, int index)
{
	int i;

	if (!lua_istable(L, index))
		return 0;
	for (i=1; i<=3; i++)
	{
		lua_getfieldi(L, index, i);
		if (!lua_isnumber(L, -1))
			goto no;
		lua_pop(L, 1);
	}
	lua_getfieldi(L, index, 4);
	if (!lua_isnil(L, -1) && !lua_isnumber(L, -1))
		goto no;

	lua_pop(L, 1);
	return 1;
no:
	lua_pop(L, 1);
	return 0;
}

void luaL_checkcolor(lua_State *L, int index, float *color)
{
	luaL_argcheck(L,
		 lua_istable(L, index)
		 && (lua_objlen(L, index) == 3 || lua_objlen(L, index) == 4)
		,index
		,"not a color"
	);
	lua_getfieldi(L, index, 1);
	color[0] = (float) luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_getfieldi(L, index, 2);
	color[1] = (float) luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_getfieldi(L, index, 3);
	color[2] = (float) luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_getfieldi(L, index, 4);
	if (lua_isnil(L, -1))
		color[3] = 1.0f;
	else
		color[3] = (float) luaL_checknumber(L, -1);
	lua_pop(L, 1);
}

static int gfx_luanewmaterial(lua_State *L)
{
	struct material mat;
	int n;
	int *handle;

	n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "invalid number of arguments (expected 1, got %d)\n", n);
	luaL_argcheck(L, lua_ismaterial(L, 1), 1, "not a material");
	lua_getfield(L, 1, "flags");
	mat.flags = luaL_checkint(L, -1);
	lua_pop(L, 1);
	if (mat.flags & MATERIAL_FULL)
	{
		lua_getfield(L, 1, "ambient");
		luaL_checkcolor(L, -1, mat.full.ambient);
		lua_pop(L, 1);
		lua_getfield(L, 1, "diffuse");
		luaL_checkcolor(L, -1, mat.full.diffuse);
		lua_pop(L, 1);
		lua_getfield(L, 1, "specular");
		luaL_checkcolor(L, -1, mat.full.specular);
		lua_pop(L, 1);
		lua_getfield(L, 1, "emission");
		luaL_checkcolor(L, -1, mat.full.emission);
		lua_pop(L, 1);
		lua_getfield(L, 1, "shininess");
		mat.full.shininess = luaL_checknumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, 1, "alpha");
		if (lua_isnumber(L, -1))
			mat.full.alpha = lua_tonumber(L, -1);
		else
			mat.full.alpha = 1.0f;
		lua_pop(L, 1);
	}
	else
	{
		lua_getfield(L, 1, "color");
		luaL_checkcolor(L, -1, mat.simple.color);
		lua_pop(L, 1);
		lua_getfield(L, 1, "specular");
		luaL_checkcolor(L, -1, mat.simple.specular);
		lua_pop(L, 1);
	}
	lua_settop(L, 0);
	n = gfx->prep_material(&mat);
	if (n > 0)
	{
		handle = lua_newuserdata(L, sizeof(n));
		*handle = n;
		luaL_getmetatable(L, "material");
		lua_setmetatable(L, 1);
	}
	else
		lua_pushnil(L);

	return 1;
}

static int gfx_luarmtexture(lua_State *L)
{
	int n;
	int *handle;

	n = lua_gettop(L);
	if (n != 1)
		luaL_error(L, "invalid number of arguments (expected 1, got %d)\n", n);
	handle = luaL_checkudata(L, 1, "texture");
	if (handle)
	{
		gfx->release_texture(*handle);
		*handle = 0;
	}
	else
	{
		ERROR("attempt to finalize texture multiple times");
	}

	return 0;
}

static int gfx_luarmmaterial(lua_State *L)
{
	int n;
	int *handle;

	n = lua_gettop(L);
	if (n != 1)
		luaL_error(L, "invalid number of arguments (expected 1, got %d)\n", n);
	handle = luaL_checkudata(L, 1, "material");
	if (handle)
	{
		gfx->release_material(*handle);
		*handle = 0;
	}
	else
	{
		ERROR("attempt to finalize material multiple times");
	}

	return 0;
}

/*
void lua_totexture(lua_State *L, int index, struct image *img)
{
	const char *data;
	int len;

	luaL_argcheck(L, lua_istexture(L, index), index, "not a texture");
	lua_getfield(L, index, "width");
	img->width = lua_tointeger(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "height");
	img->height = lua_tointeger(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "flags");
	img->flags = lua_tointeger(L, -1);
	lua_pop(L, 1);
	lua_getfield(L, index, "data");
	data = lua_tolstring(L, -1, &len);
	if (len != width * height)
	{
		luaL_error(
			"texture data length (%d) does not correspond to "
			"the dimensions (%dx%d) given",
			len, width, height
		);
	}
	img->data = MALLOC(char, len);
	memcpy(img->data, data, len);
	lua_pop(L, 1);
}
*/

int gfx_luareadpixels(lua_State *L)
{
	int n;
	int x, y, width, height;
	char *data;

	n = lua_gettop(L);
	if (n != 4)
		luaL_error(L, "invalid number of arguments (expected 2, got %d)\n", n);
	x = luaL_checkint(L, 1);
	y = luaL_checkint(L, 2);
	width = luaL_checkint(L, 3);
	height = luaL_checkint(L, 4);
	data = MALLOC(char, width * height * 3);
	gfx->read_pixels(data, x, y, width, height, UPPER_LEFT);
	lua_createtable(L1, 4, 0);
	lua_pushinteger(L1, width);
	lua_setfield(L1, -2, "width");
	lua_pushinteger(L1, height);
	lua_setfield(L1, -2, "height");
	lua_pushinteger(L1, 0);
	lua_setfield(L1, -2, "flags");
	lua_pushlstring(L1, data, width * height * 3);
	lua_setfield(L1, -2, "data");
	FREE(data);

	return 1;
}

int gfx_init(void)
{
	const luaL_Reg funcs[] =
	{
		 { "texture", gfx_luanewtexture }
		,{ "material", gfx_luanewmaterial }
		,{ "readpixels", gfx_luareadpixels }
		,{ NULL, NULL }
	};

	DEBUG(1, "Initializing graphics subsystem\n");
	DEBUG(1, "Using SDL for graphics\n");
	gfx = &gfx_sdl;
	CHECK(gfx->init());
	luaL_register(L1, "gfx", funcs);
	lua_pushinteger(L1, MATERIAL_FULL);
	lua_setfield(L1, -2, "MATERIAL_FULL");
	lua_pop(L1, 1);
	/* Build the metatable for textures. */
	luaL_newmetatable(L1, "texture");
	lua_pushcfunction(L1, gfx_luarmtexture);
	lua_setfield(L1, -2, "__gc");
	lua_pop(L1, 1);
	/* Build the metatable for materials. */
	luaL_newmetatable(L1, "material");
	lua_pushcfunction(L1, gfx_luarmmaterial);
	lua_setfield(L1, -2, "__gc");
	lua_pop(L1, 1);

	return 0;
}

void gfx_shutdown(void)
{
	DEBUG(1, "Shutting down graphics subsystem\n");
	gfx->shutdown();
}

void gfx_prepmodel(const struct model *m)
{
	int i;

	DEBUG(2, "Preparing model %p for rendering\n", m);
	for (i=0; i < m->ngroups; i++)
	{
		if (m->groups[i].gfx_handle < 0)
			m->groups[i].gfx_handle = gfx->prep_mesh(&m->groups[i]);
	}
}

void gfx_releasemodel(const struct model *m)
{
	int i;

	DEBUG(2, "Releasing model %p from gfx system\n", m);
	for (i=0; i < m->ngroups; i++)
	{
		if (m->groups[i].gfx_handle >= 0)
			gfx->release_mesh(m->groups[i].gfx_handle);
		m->groups[i].gfx_handle = -1;
	}
}

#if 0
/* FIXME: ? */
void gfx_swapredblue(struct image *img)
{
	char *c;
	int i;
	char tmp;

	c = img->data;
	for (i=0; i < img->width*img->height; i++)
	{
		if (img->format == RGB || img->format == BGR)
		{
			tmp = c[0];
			c[0] = c[2];
			c[2] = tmp;
			c = &c[3];
		}
		else if (img->format == RGBA || img->format == BGRA)
		{
			tmp = c[0];
			c[0] = c[2];
			c[2] = tmp;
			c = &c[4];
		}
		else if (img->format == GRAYSCALE)
		{
			ERROR("attempt to swap red and value components of a "
			      "grayscale texture\n");
		}
		else
		{
			ERROR("invalid texture format (%d)\n", img->format);
		}
	}
}
#endif
