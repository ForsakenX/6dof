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

struct gfx_object
{
	int model;
	vector pos;
	quat orient;
	/* lighting? */
};

struct gfx_camera
{
	vector pos;
	quat orient;
};

struct gfx_scene
{
	int num_objects;
	struct gfx_object *objects;
	struct gfx_camera camera;
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

	/* Set a new screen resolution / bit depth. If any of the
	 * parameters are zero or negative, the values should be taken from
	 * the configuration system (using the config_get_*() functions).
	 *
	 * Returns 0 on success, non-zero otherwise. */
	int (*set_screen)(int width, int height, int bpp);

	/* Set the window title and icon name. */
	void (*set_window_title)(const char *title, const char *icon);

	/* Load a level and prepare it for rendering. Any previously
	 * loaded level will be unloaded and its resources freed.
	 *
	 * Returns 0 on success, non-zero otherwise. */
	int (*load_level)(char *filename);

	/* Load a 3D model.
	 *
	 * On success, returns a model ID for reference in gfx_object
	 * structures, or zero on failure. */
	int (*load_model)(char *filename);

	/* Return a pointer to the scene data structure.
	 *
	 * Returns the pointer or NULL if no such structure has been
	 * initialized. */
	struct gfx_scene * (*get_scene)(void);

	/* Render the current scene.
	 *
	 * Returns 0 on success, non-zero otherwise. */
	int (*render)(void);
};

int gfx_init(void);
void gfx_shutdown(void);

extern struct gfx_driver gfx_sdl;
extern struct gfx_driver *gfx;

#endif /* GFX_H */
