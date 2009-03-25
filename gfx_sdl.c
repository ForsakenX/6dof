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

int nmaterials;
struct material *materials;
int last_material;

static struct gfx_scene scene;

static SDL_Surface *screen;

static int set_screen(int width, int height, int bpp);
static int prepare_material(const struct material *m);

static void init_materials(void)
{
	struct material m =
	{
		 .flags = 0
		,{
			.simple = {
				 .color = { 0.5f, 0.5f, 0.5f }
				,.alpha = 1.0f
				,.specular = 1.0f
			}
		}
	};

	nmaterials = 0;
	materials = NULL;
	prepare_material(&m);
	last_material = -1;
}

static void init_lighting(void)
{
	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
	glMateriali(GL_FRONT, GL_SHININESS, 10);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

static void do_lighting(void)
{
	vector *cam_pos = &scene.camera.pos;
	float ambient[] = {0.2f,0.2f,0.2f,1.0f};
	float lightpos[] = {cam_pos->x,cam_pos->y,cam_pos->z,1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
}

static int valid_normal(vector *v)
{
	if (isnan(v->x))
		return 0;
	if (vec_mag2(v) < 0.001f)
		return 0;
	return 1;
}

static void draw_basis(void)
{
	DEBUG(10, "Drawing basis\n");
	glBegin(GL_LINES);
	glVertex3i(-32768, 0, 0);
	glVertex3i(32767, 0, 0);
	glVertex3i(0, -32768, 0);
	glVertex3i(0, 32767, 0);
	glVertex3i(0, 0, -32768);
	glVertex3i(0, 0, 32767);
	glEnd();
}

/* TODO (low priority): improve */
static void draw_normals(const struct mesh *m)
{
	int i, j;

	DEBUG(10, "Drawing normals for mesh %p\n", m);
	for (i=0; i < m->nfaces; i++)
	{
		if (valid_normal(&m->faces[i].normal))
		{
			glBegin(GL_LINES);
			for (j=0; j < m->faces[i].nverts; j++)
			{
				glVertex3f(
					 m->verts[m->faces[i].verts[j].index].x
					,m->verts[m->faces[i].verts[j].index].y
					,m->verts[m->faces[i].verts[j].index].z
				);
				glVertex3f(
					 m->faces[i].normal.x*100.0f+m->verts[m->faces[i].verts[j].index].x
					,m->faces[i].normal.y*100.0f+m->verts[m->faces[i].verts[j].index].y
					,m->faces[i].normal.z*100.0f+m->verts[m->faces[i].verts[j].index].z
				);
			}
			glEnd();
		}
	}
}

static void render_setmaterial(const struct material *m)
{
	float f[4];

	DEBUG(7, "gfx_sdl: render_setmaterial(%p)\n", m);
	if (m->flags & MATERIAL_FULL)
	{
		f[3] = m->full.alpha;
		MEMCPY(f, m->full.ambient, 3);
		glMaterialfv(GL_FRONT, GL_AMBIENT, f);
		MEMCPY(f, m->full.diffuse, 3);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, f);
		MEMCPY(f, m->full.specular, 3);
		glMaterialfv(GL_FRONT, GL_SPECULAR, f);
		MEMCPY(f, m->full.emission, 3);
		glMaterialfv(GL_FRONT, GL_EMISSION, f);
		glMaterialf(GL_FRONT, GL_SHININESS, m->full.shininess * 128.0f);
	}
	else
	{
		MEMCPY(f, m->simple.color, 3);
		f[3] = m->simple.alpha;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, f);
		MEMCPY(f, m->simple.specular, 3);
		glMaterialfv(GL_FRONT, GL_SPECULAR, f);
		glMateriali(GL_FRONT, GL_SHININESS, 10);
	}
}

static void draw_mesh(const struct mesh *m)
{
	int i, j;

	DEBUG(9, "Drawing mesh %p\n", m);
	if (m->gfx_handle >= 0)
	{
		DEBUG(9, "Using display list %d for mesh %p\n", m->gfx_handle, m);
		glCallList(m->gfx_handle);
	}
	else
	{
		DEBUG(9, "Mesh %p has no display list, drawing\n", m);
		for (i=0; i < m->nfaces; i++)
		{
			if (m->faces[i].texture >= 0)
				glBindTexture(GL_TEXTURE_2D, m->faces[i].texture);
			glBegin(GL_POLYGON);
			for (j=0; j < m->faces[i].nverts; j++)
			{
				if (m->faces[i].verts[j].material != last_material)
				{
					render_setmaterial(
						&materials[m->faces[i].verts[j].material]
					);
					last_material = m->faces[i].verts[j].material;
				}
				if (valid_normal(&m->faces[i].verts[j].normal))
				{
					glNormal3f(
						 m->faces[i].verts[j].normal.x
						,m->faces[i].verts[j].normal.y
						,m->faces[i].verts[j].normal.z
					);
				}
				else
				{
					glNormal3f(
						 m->faces[i].normal.x
						,m->faces[i].normal.y
						,m->faces[i].normal.z
					);
				}
				glTexCoord2f(
					 m->faces[i].verts[j].texcoords.s
					,m->faces[i].verts[j].texcoords.t
				);
				glVertex3f(
					 m->verts[m->faces[i].verts[j].index].x
					,m->verts[m->faces[i].verts[j].index].y
					,m->verts[m->faces[i].verts[j].index].z
				);
			}
			glEnd();
		}
		DEBUG(9, "%d faces processed\n", i);
		if (config_get_int("drawnormals"))
			draw_normals(m);
	}
}

static void draw_model(const struct model *m)
{
	int isnull;
	int i;

	DEBUG(9, "Drawing model %p\n", m);
	for (i=0; i < m->ngroups; i++)
	{
		if (!vec_isnull(&m->offsets[i]))
			isnull = 1;
		else
			isnull = 0;
		if (isnull)
		{
			glPushMatrix();
			glTranslatef(
				 m->offsets[i].x
				,m->offsets[i].y
				,m->offsets[i].z
			);
			DEBUG(10, "Offset for model group %d is (%g, %g, %g)\n",
				i, m->offsets[i].x, m->offsets[i].y, m->offsets[i].z
			);
		}
		draw_mesh(&m->groups[i]);
		if (isnull)
			glPopMatrix();
	}
	DEBUG(9, "%d model groups processed\n", i);
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
	init_materials();
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
	glFrontFace(GL_CW);
	glPolygonMode(GL_BACK, GL_LINE);
	init_lighting();
	return 0;
}

static void set_title(const char *title, const char *icon)
{
	DEBUG(5, "gfx_sdl: set_title(\"%s\", \"%s\")\n", title, icon);
	SDL_WM_SetCaption(title, icon);
}

static int prepare_mesh(const struct mesh *m)
{
	int id;

	id = glGenLists(1);
	DEBUG(5, "gfx_sdl: prepare_mesh(%p) = %d\n", m, id);
	glNewList(id, GL_COMPILE);
	draw_mesh(m);
	glEndList();

	return id;
}

static void release_mesh(int id)
{
	DEBUG(5, "gfx_sdl: release_mesh(%d)\n", id);
	glDeleteLists(id, 1);
}

static int prepare_texture(const struct image *img)
{
	int id;

	glGenTextures(1, (GLuint *) &id);
	DEBUG(5, "gfx_sdl: prepare_texture(%p) = %d\n", img, id);
	DEBUG(5, "width = %d, height = %d\n", img->width, img->height);
	glBindTexture(GL_TEXTURE_2D, (GLuint) id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return id;
}

static void release_texture(int id)
{
	DEBUG(5, "gfx_sdl: release_texture(%d)\n", id);
	glDeleteTextures(1, (GLuint *) &id);
}

static int prepare_material(const struct material *m)
{
	DEBUG(5, "gfx_sdl: prepare_material(%p) = %d\n", m, nmaterials);
	REALLOC(materials, nmaterials + 1);
	MEMCPY(&materials[nmaterials], m, 1);
	nmaterials++;
	return nmaterials - 1;
}

static void release_material(int id)
{
	DEBUG(5, "gfx_sdl: release_material(%d)\n", id);
	nmaterials--;
	REALLOC(materials, nmaterials);
}

static void set_level(struct model *m)
{
	DEBUG(4, "gfx_sdl: set_level(%p)\n", m);
	scene.level = m;
}

static struct gfx_scene *get_scene(void)
{
	DEBUG(10, "gfx_sdl: get_scene() = %p\n", &scene);
	return &scene;
}

static int render(void)
{
	vector cam_pos;
	vector cam_fvec = { 0.0f, 0.0f, 1.0f };
	vector cam_uvec = { 0.0f, 1.0f, 0.0f };

	DEBUG(8, "gfx_sdl: render()\n");
	/* Clear the display. This may be removed when the camera
	 * is known to be within an enclosed model. */
	glClearColor(0.0f, 0.0f, 0.25f, 0.0f);
	glClearDepth(-1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	do_lighting();
	/* Set up the camera. */
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
	/* Make the Z axis point into the screen. */
	glScalef(1.0f, 1.0f, -1.0f);
	/* Draw the scene. */
	if (config_get_int("drawbasis"))
		draw_basis();
	draw_model(scene.level);
	/* for (i=0; i < scene.num_objects; i++)
		draw_model(&scene.objects[i].model); */
	glFlush();
	SDL_GL_SwapBuffers();
	glDisable(GL_TEXTURE_2D);
	return 0;
}

static void flip_vert(char *pixels, int width, int height)
{
	char row[width*3];
	int i;

	for (i=0; i < height/2; i++)
	{
		MEMCPY(row, &pixels[i*width*3], width*3);
		MEMCPY(&pixels[i*width*3], &pixels[(height-i-1)*width*3], width*3);
		MEMCPY(&pixels[(height-i-1)*width*3], row, width*3);
	}
}

static void read_pixels(char *data, int x, int y, int width, int height, int origin)
{
	glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	if (origin == UPPER_LEFT)
		flip_vert(data, width, height);
}

struct gfx_driver gfx_sdl =
{
	.init = init,
	.shutdown = shutdown,
	.set_screen = set_screen,
	.set_title = set_title,
	.prep_mesh = prepare_mesh,
	.release_mesh = release_mesh,
	.prep_texture = prepare_texture,
	.release_texture = release_texture,
	.prep_material = prepare_material,
	.release_material = release_material,
	.set_level = set_level,
	.get_scene = get_scene,
	.render = render,
	.read_pixels = read_pixels
};
