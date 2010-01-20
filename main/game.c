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

#define DEBUG(level, x...) DEBUGX(DBG_GENERAL, level, x)

static struct model *level;

float accel, drag, turnaccel, turndrag;

int move_forward, move_backward, slide_left, slide_right, slide_up, slide_down;
int turn_left, turn_right, pitch_up, pitch_down, roll_left, roll_right;
scalar move_x, move_y, move_z, turn, pitch, roll;
scalar mouseturn_x, mouseturn_y;
scalar mousesens_x, mousesens_y;

struct control camera;
vector cam_vel;
vector cam_turn_vel;
int quit;

static void move_forward_func(const struct input_event *event)
{
	move_forward = event->pressed?1:0;
}

static void move_backward_func(const struct input_event *event)
{
	move_backward = event->pressed?1:0;
}

static void slide_left_func(const struct input_event *event)
{
	slide_left = event->pressed?1:0;
}

static void slide_right_func(const struct input_event *event)
{
	slide_right = event->pressed?1:0;
}

static void slide_up_func(const struct input_event *event)
{
	slide_up = event->pressed?1:0;
}

static void slide_down_func(const struct input_event *event)
{
	slide_down = event->pressed?1:0;
}

static void move_x_func(const struct input_event *event)
{
	move_x += event->value;
}

static void move_y_func(const struct input_event *event)
{
	move_y += -event->value;
}

static void move_z_func(const struct input_event *event)
{
	move_z += -event->value;
}

static void turn_left_func(const struct input_event *event)
{
	turn_left = event->pressed?1:0;
}

static void turn_right_func(const struct input_event *event)
{
	turn_right = event->pressed?1:0;
}

static void pitch_up_func(const struct input_event *event)
{
	pitch_up = event->pressed?1:0;
}

static void pitch_down_func(const struct input_event *event)
{
	pitch_down = event->pressed?1:0;
}

static void roll_left_func(const struct input_event *event)
{
	roll_left = event->pressed?1:0;
}

static void roll_right_func(const struct input_event *event)
{
	roll_right = event->pressed?1:0;
}

static void turn_x_func(const struct input_event *event)
{
	turn = event->value;
}

static void turn_y_func(const struct input_event *event)
{
	pitch = -event->value;
}

static void turn_z_func(const struct input_event *event)
{
	roll = -event->value;
}

static void turn_xy_func(const struct input_event *event)
{
	mouseturn_x = event->x * config_get_float("mousesensx");
	mouseturn_y = event->y * config_get_float("mousesensy");
}

static void quit_func(const struct input_event *event)
{
	quit = 1;
}

#define CAP(x, limit) ((x)>(limit)?(limit):((x)<-(limit)?-(limit):(x)))

static void do_camera_physics(vector *cam_pos, quat *cam_orient)
{
	vector move;
	quat qturn;

	/* Rotation */
	camera.turn.x = CAP(turn + mouseturn_x + turn_right-turn_left, 1.0f);
	camera.turn.y = CAP(pitch + mouseturn_y + pitch_up-pitch_down, 1.0f);
	camera.turn.z = CAP(roll + roll_left-roll_right, 1.0f);
	cam_turn_vel.x += camera.turn.x * time_diff * turnaccel;
	cam_turn_vel.y += camera.turn.y * time_diff * turnaccel;
	cam_turn_vel.z += camera.turn.z * time_diff * turnaccel;
	cam_turn_vel.x *= powf(turndrag, -time_diff);
	cam_turn_vel.y *= powf(turndrag, -time_diff);
	cam_turn_vel.z *= powf(turndrag, -time_diff);
	quat_make_euler(&qturn, cam_turn_vel.x*time_diff, cam_turn_vel.y*time_diff*0.75, cam_turn_vel.z*time_diff*0.75);
	quat_mul(cam_orient, cam_orient, &qturn);
	quat_norm(cam_orient, cam_orient);

	/* Movement */
	camera.move.x = CAP(move_x + slide_right-slide_left, 1.0f);
	camera.move.y = CAP(move_y + slide_up-slide_down, 1.0f);
	camera.move.z = CAP(move_z + move_forward-move_backward, 1.0f);
	move = camera.move;
	if (vec_mag(&move))
		vec_rotate(&move, cam_orient);
	cam_vel.x += move.x * time_diff * accel;
	cam_vel.y += move.y * time_diff * accel;
	cam_vel.z += move.z * time_diff * accel;
	cam_vel.x *= powf(drag, -time_diff);
	cam_vel.y *= powf(drag, -time_diff);
	cam_vel.z *= powf(drag, -time_diff);
	cam_pos->x += cam_vel.x * time_diff;
	cam_pos->y += cam_vel.y * time_diff;
	cam_pos->z += cam_vel.z * time_diff;
}

static void clear_level(void)
{
	DEBUG(2, "game: clear_level()\n");
	if (level)
	{
		DEBUG(1, "Unloading level\n");
		model_destroy(level);
		lua_pushnil(L1);
		lua_setglobal(L1, "level");
		lua_gc(L1, LUA_GCCOLLECT, 0);
		level = NULL;
	}
}

int game_init(void)
{
	DEBUG(1, "Initializing game core\n");
	level = NULL;
	quit = 0;
	accel = config_get_float("accel");
	drag = config_get_float("drag");
	turnaccel = config_get_float("turnaccel");
	turndrag = config_get_float("turndrag");
	move_forward = move_backward = slide_left =
		slide_right = slide_up = slide_down =
		turn_left = turn_right = pitch_up =
		pitch_down = roll_left = roll_right = 0;
	input_register_func("forward", move_forward_func);
	input_register_func("reverse", move_backward_func);
	input_register_func("slide_left", slide_left_func);
	input_register_func("slide_right", slide_right_func);
	input_register_func("slide_up", slide_up_func);
	input_register_func("slide_down", slide_down_func);
	input_register_func("move_x", move_x_func);
	input_register_func("move_y", move_y_func);
	input_register_func("move_z", move_z_func);
	input_register_func("turn_left", turn_left_func);
	input_register_func("turn_right", turn_right_func);
	input_register_func("pitch_up", pitch_up_func);
	input_register_func("pitch_down", pitch_down_func);
	input_register_func("roll_left", roll_left_func);
	input_register_func("roll_right", roll_right_func);
	input_register_func("turn", turn_x_func);
	input_register_func("pitch", turn_y_func);
	input_register_func("roll", turn_z_func);
	input_register_func("rotatexy", turn_xy_func);
	input_register_func("exit", quit_func);
	input->set_mouse_mode(MOUSE_POINTER);
	cam_vel.x = 0.0f;
	cam_vel.y = 0.0f;
	cam_vel.z = 0.0f;
	quat_make_aa(&gfx->get_scene()->camera.orient, 1.0f, 0.0f, 1.0f, 0.0f);
	return 0;
}

void game_shutdown(void)
{
	DEBUG(1, "Shutting down game core\n");
	clear_level();
}

int load_level(const char *filename)
{
	struct mesh *m;
	const char *err;
	int ret;

	DEBUG(1, "game: load_level(\"%s\")\n", filename);
	ret = luaL_dofile(L1, filename);
	if (ret)
	{
		err = lua_tostring(L1, -1);
		ERROR("error while loading level: %s", err);
		lua_pop(L1, 1);
		return 1;
	}
	lua_getfield(L1, -1, "vertices");
	if (!lua_istable(L1, -1))
	{
		ERROR("expected `vertices' field in Lua level data table");
		lua_pop(L1, 2);
		return 1;
	}
	clear_level();
	m = mesh_create();
	mesh_addluaverts(m, L1, -1);
	lua_pop(L1, 1);
	lua_getfield(L1, -1, "faces");
	if (!lua_istable(L1, -1))
	{
		mesh_destroy(m);
		ERROR("expected `faces' field in Lua level data table");
		return 1;
	}
	mesh_addluafaces(m, L1, -1);
	lua_pop(L1, 1);
	lua_setglobal(L1, "level");
	level = model_create(m);
	/* m is no longer needed. */
	mesh_destroy(m);
	gfx_prepmodel(level);
	gfx->set_level(level);

	return 0;
}

int game_main(void)
{
	struct gfx_scene *scene;
	vector *cam_pos;
	quat *cam_orient;
	static scalar last_title_update_time = 0;
	static int frames = 0;
	char title[20];

	time_update();
	if (time - last_title_update_time > 1)
	{
		sprintf(title, "6dof (%.2f FPS)"
			, frames / (time - last_title_update_time)
		);
		gfx->set_title(title, "6dof");
		last_title_update_time = time;
		frames = 0;
		lua_gc(L1, LUA_GCCOLLECT, 0);
	}
	mouseturn_x = mouseturn_y = 0;
	input_process_events(0);
	scene = gfx->get_scene();
	cam_pos = &scene->camera.pos;
	cam_orient = &scene->camera.orient;
	do_camera_physics(cam_pos, cam_orient);
	DEBUG(3, "Camera position = (%g, %g, %g)\n", cam_pos->x, cam_pos->y, cam_pos->z);
	gfx->render();
	frames++;
	return quit;
}
