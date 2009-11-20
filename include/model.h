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

#ifndef GEOM_H
#define GEOM_H

#include "common.h"

typedef vector vertex;

struct texcoords
{
	float s;
	float t;
};

/* An individual vertex of a polygon. */
struct corner
{
	/* Index into the array of vertices. */
	int index;
	/* Per vertex-normal (will override face normal). */
	vector normal;
	struct texcoords texcoords;
	int material;
};

struct polygon
{
	int nverts;
	struct corner *verts;
	/* Face normal, may be overridden by vertex-specific ones. */
	vector normal;
	int texture;
};

struct mesh
{
	int nverts;
	vertex *verts;
	int nfaces;
	struct polygon *faces;
	int gfx_handle;
	/* int physics_handle; */
};

struct model
{
	int ngroups;
	struct mesh *groups;
	vector *offsets;
};

/* All functions that add data copy it over from their source, which
 * may therefore be deallocated afterwards, or be specified as a
 * constant. */

/* Allocate space for a mesh and return a pointer to it. */
struct mesh *mesh_create(void);

/* Destroy a mesh (free the memory used by it). */
void mesh_destroy(struct mesh *m);

/* Make the destination mesh a copy of the source, which can then
 * be safely modified or destroyed without affecting the copy.
 *
 * to must not point to uninitialized data - the function will attempt
 * to destroy any mesh data the destination seems to contain, so zero
 * it first if there is none. */
void mesh_copy(struct mesh *to, const struct mesh *from);

/* Add vertices to a mesh. */
void mesh_addverts(struct mesh *m, const vertex *verts, int nverts);

/* Add vertices from a Lua array at the given index to a mesh. */
void mesh_addluaverts(struct mesh *m, lua_State *L, int index);

/* Add a face to a mesh. */
void mesh_addface(struct mesh *m, const struct polygon *face);

/* Add faces from a Lua array at the given index to a mesh. */
void mesh_addluafaces(struct mesh *m, lua_State *L, int index);

/* Create a new model.
 *
 * If data is non-NULL, the model will contain a single group with
 * data from specified mesh. Otherwise, the model will be empty. */
struct model *model_create(const struct mesh *data);

/* Destroy a model and all data it contains. */
void model_destroy(struct model *m);

/* Add a group ("mesh" / "sub-model") to a model at the given offset. */
void model_addgroup(struct model *m, const struct mesh *g, const vector *offset);

/* Remove groups from a model. to is exclusive; the index specified
 * by it is not included in the list of groups removed. */
void model_rmgroup(struct model *m, int from, int to);

/* Change the offset for a given group. */
void model_movegroup(struct model *m, int index, const vector *offset);

#define lua_pushvertex lua_pushvector
#define lua_tovertex lua_tovector

#endif /* GEOM_H */
