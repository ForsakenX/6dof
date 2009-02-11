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

#ifndef INPUT_H
#define INPUT_H

#include "common.h"

struct control
{
	vector move;
	vector turn;
	int afterburn;
	int fire1;
	int fire2;
};

/* For on/off type inputs like keys and buttons. */
#define BUTTON 1
/* For bounded (absolute) single axis inputs such as a joystick axis. */
#define AXIS 2
/* For bounded double axis inputs such as a mouse pointer. */
#define POINTER 3
/* For wheel-like (unbounded single axis) inputs, yielding relative
 * values. */
#define WHEEL 4
/* For ball-like (unbounded double axis) inputs, yielding relative
 * values. */
#define BALL 5

/* Mouse generates POINTER events. */
#define MOUSE_POINTER 0
/* Mouse generates BALL events. */
#define MOUSE_MOVE 1

struct input
{
	/* One of the above input types. If type is 0, this marks the end
	 * of an array of this structure. */
	int type;
	/* A unique numeric identifier for this input that may differ
	 * from its array index. */
	int id;
	/* A unique textual identifier for the input. For use in scripts
	 * as parameter to a bind function or as human-readable output. */
	char name[32];
};

struct input_event
{
	int type;
	/* The unique numeric identifier for the input. */
	int id;
	union
	{
		/* For BUTTON type inputs. */
		int pressed;
		/* For AXIS and WHEEL type inputs. */
		scalar value;
		/* For POINTER and BALL type inputs. */
		struct
		{
			scalar x;
			scalar y;
		};
	};
};

struct input_driver
{
	/* Perform initialization. No parameters are given directly, but
	 * they can be retrieved through the global configuration system,
	 * using the config_get_*() functions.
	 *
	 * Returns 0 on success, non-zero otherwise. */
	int (*init)(void);

	/* Clean up and free resources held by this input module. This
	 * function never fails. Warnings may be printed if unexpected
	 * conditions occur. */
	void (*shutdown)(void);

	/* Return a list of inputs provided by this input driver.
	 * For convenience, if n is not NULL, then *n will be set to
	 * the length of the returned array. */
	const struct input *(*get_inputs)(int *n);

	/* Check for an input event. If/when there is one, it is written
	 * to event and the function returns zero. Otherwise, if wait is
	 * zero, the function will return immediately with a non-zero
	 * value. If wait is non-zero, it will wait for an event to
	 * happen. */
	int (*get_event)(struct input_event *event, int wait);

	/* Clear the input event queue. */
	void (*clear_queue)(void);

	/* Control what type of mouse events are generated. mode can be
	 * MOUSE_POINTER or MOUSE_MOVE. This function is not guaranteed
	 * to have any effect. */
	void (*set_mouse_mode)(int mode);
};

extern struct input_driver *input;

#ifdef INPUT_SDL
extern struct input_driver input_sdl;
#endif

typedef void (*input_func)(const struct input_event *event);

/* Initializes the default / currently selected input subsystem. */
int input_init(void);

/* Shuts down the currently initialized input subsystem. */
void input_shutdown(void);

/* Registers a C input function. Such a function has the prototype
 * (void)(*function)(const struct input_event *event) and can be
 * bound to an input from within a Lua script. */
void input_register_func(const char *name, input_func function);

/* Processes pending input events and performs the actions they
 * trigger. If wait is non-zero, the function will block until
 * at least one input event has been processed. */
void input_process_events(int wait);

#endif /* INPUT_H */
