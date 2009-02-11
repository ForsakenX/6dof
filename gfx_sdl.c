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

#define MAX_VERTS 16384
#define MAX_FACES 16384
#define MAX_VERTS_PER_FACE 10

typedef struct
{
	int type;
	short nverts;
	short verts[MAX_VERTS_PER_FACE];
	vector normal;
} face;

typedef struct
{
	int id;
	int nverts;
	vector verts[MAX_VERTS];
	int nfaces;
	face faces[MAX_FACES];
} model;

model level;

#if 0
int num_models = 0;
static model models[32];
#endif

static struct gfx_scene scene;

static SDL_Surface *screen;

static int set_screen(int width, int height, int bpp);

static void init_lighting(void)
{
	//float ambdiff[] = {0.8f,0.8f,0.8f,1.0f};
	float ambdiff[] = {0.5f,0.5f,0.5f,1.0f};
	float spec[] = {0.0f,0.0f,0.0f,1.0f};
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambdiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMateriali(GL_FRONT, GL_SHININESS, 10);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

static void do_lighting(void)
{
	vector *cam_pos = &scene.camera.pos;
	float ambient[] = {0.5f,0.5f,0.5f,1.0f};
	float lightpos[] = {cam_pos->x,cam_pos->y,cam_pos->z,1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
}

/* [-0, +0, -] */
static void add_verts_from_lua_array(model *m, lua_State *L, int index)
{
	int n;
	vector *v;

	DEBUG(6, "Adding vertices from Lua array at index %d\n", index);
	if (index < 0)
		/* Index counts from top of stack. */
		index--;
	for (n=1;;n++)
	{
		lua_pushinteger(L, n);
		lua_gettable(L, index);
		if (lua_istable(L, -1))
		{
			v = &m->verts[m->nverts];
			lua_pushinteger(L, 1);
			lua_gettable(L, -2);
			v->x = (scalar) lua_tonumber(L, -1);
			lua_pop(L, 1);
			lua_pushinteger(L, 2);
			lua_gettable(L, -2);
			v->y = (scalar) lua_tonumber(L, -1);
			lua_pop(L, 1);
			lua_pushinteger(L, 3);
			lua_gettable(L, -2);
			v->z = (scalar) lua_tonumber(L, -1);
			lua_pop(L, 2);
			DEBUG(6, "Added vertex (%g, %g, %g)\n", v->x, v->y, v->z);
			m->nverts++;
		}
		else
		{
			lua_pop(L, 1);
			break;
		}
	}
	DEBUG(6, "%d vertices added, current total is %d\n", n-1, m->nverts);
}

/* [-0, +0, -] */
static void add_faces_from_lua_array(model *m, lua_State *L, int index)
{
	int n, i;
	face *f;

	DEBUG(6, "Adding faces from Lua array at index %d\n", index);
	if (index < 0)
		/* Index counts from top of stack. */
		index--;
	for (n=1;;n++)
	{
		lua_pushinteger(L, n);
		lua_gettable(L, index);
		if (lua_istable(L, -1))
		{
			f = &m->faces[m->nfaces];
			f->normal.x = NAN;
			DEBUG(6, "Adding face [");
			f->nverts = 0;
			for (i=1;;i++)
			{
				lua_pushinteger(L, i);
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
				{
					f->verts[f->nverts] = ((short) lua_tointeger(L, -1)) - 1;
					lua_pop(L, 1);
					DEBUG(6, "%s(%g, %g, %g)", (f->nverts?", ":"")
						,m->verts[f->verts[f->nverts]].x
						,m->verts[f->verts[f->nverts]].y
						,m->verts[f->verts[f->nverts]].z
					);
					f->nverts++;
				}
				else if (lua_istable(L, -1) && lua_objlen(L, -1) == 3)
				{
					/* Surface normal. */
					lua_pushinteger(L, 1);
					lua_gettable(L, -2);
					f->normal.x = lua_tonumber(L, -1);
					lua_pop(L, 1);
					lua_pushinteger(L, 2);
					lua_gettable(L, -2);
					f->normal.y = lua_tonumber(L, -1);
					lua_pop(L, 1);
					lua_pushinteger(L, 3);
					lua_gettable(L, -2);
					f->normal.z = lua_tonumber(L, -1);
					lua_pop(L, 2);
					DEBUG(6, "%s:normal(%g, %g, %g)", (f->nverts?", ":"")
						,f->normal.x, f->normal.y, f->normal.z
					);
				}
				else
				{
					lua_pop(L, 1);
					break;
				}
			}
			DEBUG(6, "]\n");
			lua_pop(L, 1);
			m->nfaces++;
		}
		else
		{
			lua_pop(L, 1);
			break;
		}
	}
	DEBUG(6, "%d faces added, current total is %d\n", n-1, m->nfaces);
}

static int create_dl(model *m)
{
	int id;
	int i, j;

	id = glGenLists(1);
	glNewList(id, GL_COMPILE);
	for (i=0; i < m->nfaces; i++)
	{
		glBegin(GL_POLYGON);
		for (j=0; j < m->faces[i].nverts; j++)
		{
			if (!isnan(m->faces[i].normal.x))
			{
				glNormal3f(
					 m->faces[i].normal.x
					,m->faces[i].normal.y
					,m->faces[i].normal.z
				);
			}
			glVertex3f(
				 m->verts[m->faces[i].verts[j]].x
				,m->verts[m->faces[i].verts[j]].y
				,m->verts[m->faces[i].verts[j]].z
			);
		}
		glEnd();
	}
	glEndList();

	return id;
}

static int init(void)
{
	char drv_name[64];
	DEBUG(3, "gfx_sdl: init()\n");
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
	{
		ERROR("SDL_Init(): %s", SDL_GetError());
		return 1;
	}
	set_screen(0, 0, 0);
	if (!screen)
	{
		ERROR("SDL_SetVideoMode(): %s", SDL_GetError());
		return 1;
	}
	DEBUG(2, "gfx_sdl: using video driver \"%s\"\n",
		SDL_VideoDriverName(drv_name, 64));
	memset(&scene, 0, sizeof(struct gfx_scene));
	return 0;
}

static void shutdown(void)
{
	DEBUG(3, "gfx_sdl: shutdown()\n");
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

static int set_screen(int width, int height, int bpp)
{
	SDL_Surface *new_screen;
	DEBUG(3, "gfx_sdl: set_screen(%d, %d, %d)\n", width, height, bpp);
	if (width <= 0 || height <= 0 || bpp <= 0)
	{
		width = config_get_int("width");
		height = config_get_int("height");
		bpp = config_get_int("bpp");
		DEBUG(3, "gfx_sdl: setting screen mode to %dx%dx%d\n", width, height, bpp);
	}
	if (bpp == 16)
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	}
	else if (bpp == 24)
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	}
	else if (bpp == 32)
	{
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	}
	else
	{
		ERROR("invalid bpp value %d (must be 16, 24, or 32)", bpp);
		return 1;
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	new_screen = SDL_SetVideoMode(
		 width, height, bpp
		,SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_OPENGL
	);
	if (!new_screen)
	{
		ERROR("SDL_SetVideoMode(): %s", SDL_GetError());
		return 1;
	}
	SDL_WM_SetCaption("6dof", "6dof");
	screen = new_screen;
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, 4.0f/3.0f, -1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GEQUAL);
	glPolygonMode(GL_FRONT, GL_LINE);
	init_lighting();
	return 0;
}

static void set_window_title(const char *title, const char *icon)
{
	SDL_WM_SetCaption(title, icon);
}

static int load_level(char *filename)
{
	int n;

	DEBUG(3, "gfx_sdl: load_level(\"%s\")\n", filename);
	/*
	f = io_open_file(filename, "r");
	if (!f)
	{
		ERROR("couldn't open level file %s", filename);
		return 1;
	}
	*/
	n = lua_gettop(L1);
	if (luaL_dofile(L1, filename))
	{
		ERROR("couldn't load level - %s", lua_tostring(L1, -1));
		return 1;
	}
	if (lua_gettop(L1) != n+1)
	{
		ERROR("level file must provide one table containing level data");
		return 1;
	}
	lua_getfield(L1, -1, "vertices");
	if (!lua_istable(L1, -1))
	{
		ERROR("no vertex data provided by level file");
		return 1;
	}
	add_verts_from_lua_array(&level, L1, -1);
	lua_pop(L1, 1);
	lua_getfield(L1, -1, "faces");
	if (!lua_istable(L1, -1))
	{
		ERROR("no face (polygon) data provided by level file");
		return 1;
	}
	add_faces_from_lua_array(&level, L1, -1);
	lua_pop(L1, 2);
	level.id = create_dl(&level);
	return 0;
}

#if 0
static int load_model(char *filename)
{
	int dl;
	DEBUG(5, "gfx_sdl: load_model(\"%s\")\n", filename);
	if (num_models >= 32)
		return 1;
	dl = glGenLists(1);
	glNewList(dl, GL_COMPILE);
	/* ... */
	models[num_models].id = dl;
	num_models++;
	return 0;
}
#endif

static struct gfx_scene *get_scene(void)
{
	DEBUG(10, "gfx_sdl: get_scene() = %#010tx\n", &scene);
	return &scene;
}

static int render(void)
{
	int i, j;
	vector cam_pos;
	vector cam_fvec = { 0.0f, 0.0f, 1.0f };
	vector cam_uvec = { 0.0f, 1.0f, 0.0f };
	int draw_normals = config_get_int("drawnormals");

	DEBUG(8, "gfx_sdl: render()\n");
	glClearColor(0.0f, 0.0f, 0.25f, 0.0f);
	glClearDepth(-1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	do_lighting();
	cam_pos = scene.camera.pos;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	vec_rotate(&cam_fvec, &scene.camera.orient);
	vec_add(&cam_fvec, &cam_fvec, &cam_pos);
	vec_rotate(&cam_uvec, &scene.camera.orient);
	DEBUG(9, "cam_pos = (%g, %g, %g)\ncam_fvec = (%g, %g, %g)\ncam_uvec = (%g, %g, %g)\n"
		, cam_pos.x, cam_pos.y, cam_pos.z
		, cam_fvec.x, cam_fvec.y, cam_fvec.z
		, cam_uvec.x, cam_uvec.y, cam_uvec.z
	);
	gluLookAt(
		 cam_pos.x, cam_pos.y, -cam_pos.z
		,cam_fvec.x, cam_fvec.y, -cam_fvec.z
		,cam_uvec.x, cam_uvec.y, -cam_uvec.z
	);
	glColor3f(1.0f, 1.0f, 1.0f);
	glScalef(1.0f, 1.0f, -1.0f);
	glCallList(level.id);
	for (i=0; i<level.nfaces; i++)
	{
		if (draw_normals && !isnan(level.faces[i].normal.x))
		{
			glBegin(GL_LINES);
			for (j=0; j<level.faces[i].nverts; j++)
			{
				glVertex3f(
					 level.verts[level.faces[i].verts[j]].x
					,level.verts[level.faces[i].verts[j]].y
					,level.verts[level.faces[i].verts[j]].z
				);
				glVertex3f(
					 level.faces[i].normal.x*100.0f+level.verts[level.faces[i].verts[j]].x
					,level.faces[i].normal.y*100.0f+level.verts[level.faces[i].verts[j]].y
					,level.faces[i].normal.z*100.0f+level.verts[level.faces[i].verts[j]].z
				);
			}
			glEnd();
		}
	}
	glFlush();
	SDL_GL_SwapBuffers();
	return 0;
}


struct gfx_driver gfx_sdl =
{
	.init = init,
	.shutdown = shutdown,
	.set_screen = set_screen,
	.set_window_title = set_window_title,
	.load_level = load_level,
	/*.load_model = load_model,*/
	.get_scene = get_scene,
	.render = render
};
