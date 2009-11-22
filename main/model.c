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

int lua_ismesh(lua_State *L, int index)
{
	if (!lua_istable(L, index))
		return 0;
	lua_getfield(L, index, "vertices");
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return 0;
	}
	lua_getfield(L, index, "faces");
	if (!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		return 0;
	}
	lua_pop(L, 1);
	return 1;
}

static void mesh_clear(struct mesh *m)
{
	int i;

	DEBUG(9, "Destroying %d verts, %d faces\n",
		m->nverts, m->nfaces);
	if (m->nverts && m->verts)
	{
		FREE(m->verts);
	}
	if (m->nfaces && m->faces)
	{
		for (i=0; i < m->nfaces; i++)
		{
			FREE(m->faces[i].verts);
		}
		FREE(m->faces);
	}
}

struct mesh *mesh_create(void)
{
	struct mesh *m;
	m = CALLOC1(struct mesh);
	DEBUG(10, "mesh_create() = %p\n", m);
	m->gfx_handle = -1;
	return m;
}

void mesh_destroy(struct mesh *m)
{
	DEBUG(7, "Destroying mesh at %p\n", m);
	mesh_clear(m);
	if (m->gfx_handle >= 0)
		gfx->release_mesh(m->gfx_handle);
	FREE(m);
}

void mesh_copy(struct mesh *to, const struct mesh *from)
{
	int i;

	DEBUG(7, "Copying mesh at %p (%d verts, %d faces) to %p\n"
		, from, from->nverts, from->nfaces, to);
	if (to->nverts || to->nfaces)
		mesh_clear(to);
	mesh_addverts(to, from->verts, from->nverts);
	REALLOC(to->faces, from->nfaces);
	for (i=0; i < from->nfaces; i++)
	{
		int nverts = from->faces[i].nverts;
		if (nverts <= 0)
		{
			luaL_error(L1, "face %d (%p) has %d vertices",
				i, &from->faces[i], nverts
			);
			continue;
		}
		to->faces[i].nverts = nverts;
		to->faces[i].verts = NULL;
		to->faces[i].normal = from->faces[i].normal;
		to->faces[i].texture = from->faces[i].texture;
		REALLOC(to->faces[i].verts, nverts);
		MEMCPY(to->faces[i].verts, from->faces[i].verts, nverts);
	}
	to->nfaces = from->nfaces;
	DEBUG(7, "%d verts, %d faces copied\n", to->nverts, to->nfaces);
}

void mesh_addverts(struct mesh *m, const vertex *verts, int nverts)
{
	DEBUG(7, "Adding %d vertices to mesh\n", nverts);
	/* Allocate space for the updated list. */
	REALLOC(m->verts, m->nverts + nverts);
	/* Insert the new vertices. */
	MEMCPY(&m->verts[m->nverts], verts, nverts);
	/* Update vertex count. */
	m->nverts += nverts;
}

void mesh_addluaverts(struct mesh *m, lua_State *L, int index)
{
	int n;
	int i;
	vertex *v;

	DEBUG(6, "Adding vertices from Lua array at index %d\n", index);
	n = lua_objlen(L, index);
	DEBUG(6, "Length of array reported as %d\n", n);
	REALLOC(m->verts, m->nverts + n);
	for (i=0; i<n; i++)
	{
		/* Retrieve the (i+1)'th vertex from the array. */
		lua_getfieldi(L, index, i+1);
		if (lua_istable(L, -1))
		{
			v = &m->verts[m->nverts + i];
			lua_tovertex(L, -1, v);
			DEBUG(6, "Added vertex (%g, %g, %g)\n", v->x, v->y, v->z);
		}
		lua_pop(L, 1);
	}
	m->nverts += i;
	DEBUG(6, "%d vertices added, current total is %d\n", i, m->nverts);
}

void mesh_addface(struct mesh *m, const struct polygon *face)
{
	struct polygon *to;

	DEBUG(7, "Adding polygon with %d vertices\n", face->nverts);
	REALLOC(m->faces, m->nfaces + 1);
	to = &m->faces[m->nfaces];
	to->nverts = face->nverts;
	to->verts = NULL;
	REALLOC(to->verts, face->nverts);
	MEMCPY(to->verts, face->verts, face->nverts);
	to->normal = face->normal;
	to->texture = face->texture;
	m->nfaces++;
}

static void cornerfromlua(struct corner *corner, lua_State *L, int index)
{
	lua_getfield(L, index, "index");
	if (!lua_isnil(L, -1))
		corner->index = luaL_checkint(L, -1) - 1;
	lua_pop(L, 1);
	lua_getfield(L, index, "normal");
	if (!lua_isnil(L, -1))
		luaL_checkvector(L, -1, &corner->normal);
	lua_pop(L, 1);
	lua_getfield(L, index, "texcoords");
	if (!lua_istable(L, -1))
		luaL_error(L, "expected {s, t} texcoord table");
	lua_getfieldi(L, index, 1);
	corner->texcoords.s = luaL_checknumber(L, -1);
	lua_pop(L, 1);
	lua_getfieldi(L, index, 2);
	corner->texcoords.t = luaL_checknumber(L, -1);
	lua_pop(L, 2);
	lua_getfield(L, index, "material");
	if (!lua_isnil(L, -1))
		corner->material = *((int *) luaL_checkudata(L, -1, "material"));
	lua_pop(L, 1);
}

static void facefromlua(struct polygon *face, lua_State *L, int index)
{
	int n, i;

	n = lua_objlen(L, index);
	if (!n)
	{
		ERROR("face %p has no vertices\n", face);
		return;
	}
	face->nverts = 0;
	REALLOC(face->verts, n);
	MEMSET(face->verts, 0, n);
	lua_getfield(L, index, "normal");
	if (!lua_isnil(L, -1))
		luaL_checkvector(L, -1, &face->normal);
	lua_pop(L, 1);
	lua_getfield(L, index, "texture");
	if (!lua_isnil(L, -1))
		face->texture = *((int *) luaL_checkudata(L, -1, "texture"));
	lua_pop(L, 1);
	for (i=0; i<n; i++)
	{
		lua_getfieldi(L, index, i+1);
		if (!lua_istable(L, -1))
			luaL_error(L, "table expected, got %s", luaL_typename(L, -1));
		cornerfromlua(&face->verts[i], L, -1);
		lua_pop(L, 1);
	}
	face->nverts = n;
}

void mesh_addluafaces(struct mesh *m, lua_State *L, int index)
{
	int n, i;

	DEBUG(6, "Adding faces from Lua array at index %d\n", index);
	n = lua_objlen(L, index);
	DEBUG(6, "Length of array reported as %d\n", n);
	REALLOC(m->faces, m->nfaces + n);
	MEMSET(&m->faces[m->nfaces], 0, n);
	for (i=1; i<=n; i++)
	{
		lua_getfieldi(L, index, i);
		if (!lua_istable(L, -1))
			luaL_error(L, "index %d: table expected, got %s",
				i, luaL_typename(L, -1));
		facefromlua(&m->faces[m->nfaces], L, -1);
		lua_pop(L, 1);
		m->nfaces++;
	}
	DEBUG(6, "%d faces added, current total is %d\n", i-1, m->nfaces);
}

struct mesh *luaL_checkmesh(lua_State *L, int index)
{
	struct mesh *m;

	luaL_argcheck(L, lua_ismesh(L, index), index, "not a mesh");
	m = mesh_create();
	lua_getfield(L, -1, "vertices");
	mesh_addluaverts(m, L, -1);
	lua_pop(L, 1);
	lua_getfield(L, -1, "faces");
	mesh_addluafaces(m, L, -1);
	lua_pop(L, 1);

	return m;
}

struct model *luaL_checkmodel(lua_State *L, int index)
{
	int n, i;
	struct mesh *group;
	vector offset;
	struct model *m;

	n = lua_objlen(L, index);
	if (n <= 0)
	{
		luaL_error(L, "invalid table length at index %d", index);
	}
	m = model_create(NULL);
	for (i=0; i<n; i++)
	{
		lua_getfieldi(L, index, i+1);
		group = luaL_checkmesh(L, -1);
		lua_getfield(L, -1, "offset");
		if (lua_isvector(L, -1))
			lua_tovector(L, -1, &offset);
		else
			offset.x = offset.y = offset.z = 0.0f;
		lua_pop(L, 2);
		model_addgroup(m, group, &offset);
		mesh_destroy(group);
	}

	return m;
}

struct model *model_create(const struct mesh *data)
{
	struct model *m;

	m = CALLOC1(struct model);
	if (data)
	{
		DEBUG(10, "model_create(%p) = %p\n", data, m);
		DEBUG(7, "Created new model\n");
		m->ngroups = 1;
		m->groups = mesh_create();
		mesh_copy(m->groups, data);
		m->offsets = CALLOC1(vector);
	}
	else
	{
		DEBUG(10, "model_create(NULL) = %p\n", m);
		DEBUG(7, "Created new empty model\n");
	}

	return m;
}

void model_destroy(struct model *m)
{
	int i;

	DEBUG(7, "Destroying model %p\n", m);
	for (i=0; i < m->ngroups; i++)
		mesh_clear(&m->groups[i]);
	FREE(m->groups);
	FREE(m->offsets);
	FREE(m);
}

void model_addgroup(struct model *m, const struct mesh *g, const vector *offset)
{
	DEBUG(7, "Adding group to model, total = %d\n", m->ngroups + 1);
	REALLOC(m->groups, m->ngroups + 1);
	REALLOC(m->offsets, m->ngroups + 1);
	mesh_copy(&m->groups[m->ngroups], g);
	m->offsets[m->ngroups] = *offset;
	m->ngroups++;
}

void model_rmgroups(struct model *m, int from, int to)
{
	int i;

	DEBUG(7, "Removing groups %d to %d from model\n", from, to - 1);
	if (from < 0 || to > m->ngroups)
	{
		ERROR("tried to remove non-existent sub-model");
		return;
	}
	else if (to < from+1)
	{
		DEBUG(7, "No groups to remove\n");
		return;
	}
	for (i=from; i<to; i++)
		mesh_destroy(&m->groups[i]);
	for (i=to; i < m->ngroups; i++)
		m->groups[from+i-to] = m->groups[i];
	m->ngroups -= to-from;
	if (m->ngroups == 0)
	{
		FREE(m->groups);
		m->groups = NULL;
	}
	else
	{
		REALLOC(m->groups, m->ngroups);
	}
}

void model_movegroup(struct model *m, int index, const vector *offset)
{
	if (index < 0 || index >= m->ngroups)
	{
		ERROR("invalid index %d", index);
	}
	m->offsets[index] = *offset;
}
