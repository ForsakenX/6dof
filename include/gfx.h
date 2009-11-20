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

#ifndef GFX_H
#define GFX_H

#include "common.h"

/* Screen mode flags. */
#define FULLSCREEN 1

/* Screen/image origin specification. */
#define UPPER_LEFT 0
#define LOWER_LEFT 1

/* 0-255 */
#define RGBAi(r,g,b,a) (color) ((r) << 24 + (g) << 16 + (b) << 8 + (a))

/* 0.0 - 1.0 */
#define RGBAf(r,g,b,a) (color) ( \
	  (int) ((r)*255) << 24  \
	+ (int) ((g)*255) << 16  \
	+ (int) ((b)*255) << 8   \
	+ (int) ((a)*255)        \
)

#define RGBi(r,g,b) RGBAi(r,g,b,255)
#define RGBf(r,g,b) RGBAf(r,g,b,1.0f)

#define HSVAi(h,s,v,a) \
	({ \
		float _x = ((float) (h) / (256.0f/6.0f)); \
		int _i = (int) _x % 6; \
		float _f = _x - _i; \
		int _p = ((v) % 256) * (255 - ((s) % 256)) / 65536; \
		int _q = ((v) % 256) * (255 - _f * (float) ((s) % 256)) / 65536; \
		int _t = ((v) % 256) * \
			(255 - (1.0f - _f) * (float) ((s) % 256)) / 65536; \
		color _c; \
		switch (_i) \
		{ \
			case 0: _c = RGBAi(v,t,p,a); break; \
			case 1: _c = RGBAi(q,v,p,a); break; \
			case 2: _c = RGBAi(p,v,t,a); break; \
			case 3: _c = RGBAi(p,q,v,a); break; \
			case 4: _c = RGBAi(t,p,v,a); break; \
			case 5: _c = RGBAi(v,p,q,a); break; \
		} \
		_c; \
	})

#define HSVi(h,s,v) HSVAi(h,s,v,255)

#define RGB2GRAY(r,g,b)  \
	((float)(r) * 0.299f \
	+(float)(g) * 0.587f \
	+(float)(b) * 0.114f)

#define GRAYAi(v,a) RGBAi(v,v,v,a)
#define GRAYAf(v,a) RGBAf(v,v,v,a)
#define GRAYi(v) GRAYAi(v,255)
#define GRAYf(v) GRAYAf(v,1.0f)

/* RGBA */
typedef unsigned int color;

#if 0 /* FIXME: ? */
#define RGB 0
#define RGBA 1
#define BGR 2
#define BGRA 3
#define GRAYSCALE 4
#endif

struct image
{
	int width;
	int height;
	/* FIXME: ? */
	/* int format; */
	int flags;
	char *data;
};

#define MATERIAL_FULL 1

struct material
{
	int flags;
	union
	{
		/* Simple colored material with alpha and specular value. */
		struct
		{
			float color[3];
			float specular[3];
			float alpha;
		} simple;
		/* Detailed material specification. */
		struct
		{
			float ambient[3];
			float diffuse[3];
			float specular[3];
			float emission[3];
			float shininess;
			float alpha;
		} full;
	};
};

struct gfx_object
{
	struct model *model;
	vector pos;
	quat orient;
	/* lighting? */
};

struct camera
{
	vector pos;
	quat orient;
	float fov;
};

struct gfx_scene
{
	struct camera camera;
	struct model *level;
	int num_objects;
	struct gfx_object *objects;
};

struct gfx_driver
{
	/* Perform initialization. No parameters are given directly, but
	 * they can be retrieved through the global configuration system,
	 * using the config_get_*() functions.
	 *
	 * Returns 0 on success, non-zero otherwise. */
	int (*init)(void);

	/* Clean up and free resources held by this graphics module. This
	 * function never fails. Warnings may be printed if unexpected
	 * conditions occur. */
	void (*shutdown)(void);

	int (*set_screen)(int width, int height, int bpp, int flags);

	/* Set the window title and icon name. */
	void (*set_title)(const char *title, const char *icon);

	/* Prepare a mesh (basic 3D geometry data) for rendering.
	 *
	 * On success, returns a mesh ID for reference in gfx_object
	 * structures and release_mesh, or zero on failure. */
	int (*prep_mesh)(const struct mesh *m);

	/* Destroy rendering info for a model (which was created using
	 * prep_model). id is the number that was returned by prepmodel. */
	void (*release_mesh)(int id);

	/* Prepare a texture for rendering.
	 *
	 * On success, returns a texture ID for reference in model
	 * structures, or zero on failure. */
	int (*prep_texture)(const struct image *tex);

	/* Destroy rendering info for a texture (which was created using
	 * prep_texture). id is the number that was returned by
	 * prep_texture. */
	void (*release_texture)(int id);

	/* Prepare a material for rendering.
	 *
	 * On success, returns a material ID for reference in model
	 * structures, or zero on failure. */
	int (*prep_material)(const struct material *m);

	/* Destroy rendering info for a material (which was created using
	 * prep_material). id is the number that was returned by
	 * prep_material. */
	void (*release_material)(int id);

	/* Use the given model as the level. */
	void (*set_level)(struct model *m);

	/* Return a pointer to the scene data structure.
	 *
	 * Returns the pointer or NULL if no such structure has been
	 * initialized. */
	struct gfx_scene * (*get_scene)(void);

	/* Render the current scene.
	 *
	 * Returns 0 on success, non-zero otherwise. */
	int (*render)(void);

	/* Reads previously rendered pixel data from the screen. Pixels
	 * in the rectangle (x, y, x+width, x+height) are written to the
	 * memory pointed to by data in RGB format. */
	void (*read_pixels)
	(
		 char *data
		,int x
		,int y
		,int width
		,int height
		,int origin
	);
};

extern struct gfx_driver gfx_sdl;
extern struct gfx_driver *gfx;

int gfx_init(void);
void gfx_shutdown(void);

/* See prepmodel in struct driver. The ID is stored in a structure
 * inside the model itself, along with any driver-independent data. */
void gfx_prepmodel(const struct model *m);

/* See releasemodel in struct driver. */
void gfx_releasemodel(const struct model *m);

#endif /* GFX_H */
