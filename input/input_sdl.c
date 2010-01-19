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

#define DEBUG(level, x...) DEBUGX(DBG_INPUT, level, x)

#define MAX_INPUTS 512
#define MAX_JOYSTICKS 4

static int num_inputs;

static const struct fixed_input
{
	int type;
	int id;
	const char *name;
} fixed_inputs[] =
{
	/* Keys */
	 { BUTTON, SDLK_UNKNOWN, "unknown" }
	,{ BUTTON, SDLK_FIRST, "first" }
	,{ BUTTON, SDLK_BACKSPACE, "backspace" }
	,{ BUTTON, SDLK_TAB, "tab" }
	,{ BUTTON, SDLK_CLEAR, "clear" }
	,{ BUTTON, SDLK_RETURN, "return" }
	,{ BUTTON, SDLK_PAUSE, "pause" }
	,{ BUTTON, SDLK_ESCAPE, "escape" }
	,{ BUTTON, SDLK_SPACE, "space" }
	,{ BUTTON, SDLK_EXCLAIM, "exclaim" }
	,{ BUTTON, SDLK_QUOTEDBL, "quotedbl" }
	,{ BUTTON, SDLK_HASH, "hash" }
	,{ BUTTON, SDLK_DOLLAR, "dollar" }
	,{ BUTTON, SDLK_AMPERSAND, "ampersand" }
	,{ BUTTON, SDLK_QUOTE, "quote" }
	,{ BUTTON, SDLK_LEFTPAREN, "leftparen" }
	,{ BUTTON, SDLK_RIGHTPAREN, "rightparen" }
	,{ BUTTON, SDLK_ASTERISK, "asterisk" }
	,{ BUTTON, SDLK_PLUS, "plus" }
	,{ BUTTON, SDLK_COMMA, "comma" }
	,{ BUTTON, SDLK_MINUS, "minus" }
	,{ BUTTON, SDLK_PERIOD, "period" }
	,{ BUTTON, SDLK_SLASH, "slash" }
	,{ BUTTON, SDLK_0, "0" }
	,{ BUTTON, SDLK_1, "1" }
	,{ BUTTON, SDLK_2, "2" }
	,{ BUTTON, SDLK_3, "3" }
	,{ BUTTON, SDLK_4, "4" }
	,{ BUTTON, SDLK_5, "5" }
	,{ BUTTON, SDLK_6, "6" }
	,{ BUTTON, SDLK_7, "7" }
	,{ BUTTON, SDLK_8, "8" }
	,{ BUTTON, SDLK_9, "9" }
	,{ BUTTON, SDLK_COLON, "colon" }
	,{ BUTTON, SDLK_SEMICOLON, "semicolon" }
	,{ BUTTON, SDLK_LESS, "less" }
	,{ BUTTON, SDLK_EQUALS, "equals" }
	,{ BUTTON, SDLK_GREATER, "greater" }
	,{ BUTTON, SDLK_QUESTION, "question" }
	,{ BUTTON, SDLK_AT, "at" }
	,{ BUTTON, SDLK_LEFTBRACKET, "leftbracket" }
	,{ BUTTON, SDLK_BACKSLASH, "backslash" }
	,{ BUTTON, SDLK_RIGHTBRACKET, "rightbracket" }
	,{ BUTTON, SDLK_CARET, "caret" }
	,{ BUTTON, SDLK_UNDERSCORE, "underscore" }
	,{ BUTTON, SDLK_BACKQUOTE, "backquote" }
	,{ BUTTON, SDLK_a, "a" }
	,{ BUTTON, SDLK_b, "b" }
	,{ BUTTON, SDLK_c, "c" }
	,{ BUTTON, SDLK_d, "d" }
	,{ BUTTON, SDLK_e, "e" }
	,{ BUTTON, SDLK_f, "f" }
	,{ BUTTON, SDLK_g, "g" }
	,{ BUTTON, SDLK_h, "h" }
	,{ BUTTON, SDLK_i, "i" }
	,{ BUTTON, SDLK_j, "j" }
	,{ BUTTON, SDLK_k, "k" }
	,{ BUTTON, SDLK_l, "l" }
	,{ BUTTON, SDLK_m, "m" }
	,{ BUTTON, SDLK_n, "n" }
	,{ BUTTON, SDLK_o, "o" }
	,{ BUTTON, SDLK_p, "p" }
	,{ BUTTON, SDLK_q, "q" }
	,{ BUTTON, SDLK_r, "r" }
	,{ BUTTON, SDLK_s, "s" }
	,{ BUTTON, SDLK_t, "t" }
	,{ BUTTON, SDLK_u, "u" }
	,{ BUTTON, SDLK_v, "v" }
	,{ BUTTON, SDLK_w, "w" }
	,{ BUTTON, SDLK_x, "x" }
	,{ BUTTON, SDLK_y, "y" }
	,{ BUTTON, SDLK_z, "z" }
	,{ BUTTON, SDLK_DELETE, "delete" }
	,{ BUTTON, SDLK_WORLD_0, "world_0" }
	,{ BUTTON, SDLK_WORLD_1, "world_1" }
	,{ BUTTON, SDLK_WORLD_2, "world_2" }
	,{ BUTTON, SDLK_WORLD_3, "world_3" }
	,{ BUTTON, SDLK_WORLD_4, "world_4" }
	,{ BUTTON, SDLK_WORLD_5, "world_5" }
	,{ BUTTON, SDLK_WORLD_6, "world_6" }
	,{ BUTTON, SDLK_WORLD_7, "world_7" }
	,{ BUTTON, SDLK_WORLD_8, "world_8" }
	,{ BUTTON, SDLK_WORLD_9, "world_9" }
	,{ BUTTON, SDLK_WORLD_10, "world_10" }
	,{ BUTTON, SDLK_WORLD_11, "world_11" }
	,{ BUTTON, SDLK_WORLD_12, "world_12" }
	,{ BUTTON, SDLK_WORLD_13, "world_13" }
	,{ BUTTON, SDLK_WORLD_14, "world_14" }
	,{ BUTTON, SDLK_WORLD_15, "world_15" }
	,{ BUTTON, SDLK_WORLD_16, "world_16" }
	,{ BUTTON, SDLK_WORLD_17, "world_17" }
	,{ BUTTON, SDLK_WORLD_18, "world_18" }
	,{ BUTTON, SDLK_WORLD_19, "world_19" }
	,{ BUTTON, SDLK_WORLD_20, "world_20" }
	,{ BUTTON, SDLK_WORLD_21, "world_21" }
	,{ BUTTON, SDLK_WORLD_22, "world_22" }
	,{ BUTTON, SDLK_WORLD_23, "world_23" }
	,{ BUTTON, SDLK_WORLD_24, "world_24" }
	,{ BUTTON, SDLK_WORLD_25, "world_25" }
	,{ BUTTON, SDLK_WORLD_26, "world_26" }
	,{ BUTTON, SDLK_WORLD_27, "world_27" }
	,{ BUTTON, SDLK_WORLD_28, "world_28" }
	,{ BUTTON, SDLK_WORLD_29, "world_29" }
	,{ BUTTON, SDLK_WORLD_30, "world_30" }
	,{ BUTTON, SDLK_WORLD_31, "world_31" }
	,{ BUTTON, SDLK_WORLD_32, "world_32" }
	,{ BUTTON, SDLK_WORLD_33, "world_33" }
	,{ BUTTON, SDLK_WORLD_34, "world_34" }
	,{ BUTTON, SDLK_WORLD_35, "world_35" }
	,{ BUTTON, SDLK_WORLD_36, "world_36" }
	,{ BUTTON, SDLK_WORLD_37, "world_37" }
	,{ BUTTON, SDLK_WORLD_38, "world_38" }
	,{ BUTTON, SDLK_WORLD_39, "world_39" }
	,{ BUTTON, SDLK_WORLD_40, "world_40" }
	,{ BUTTON, SDLK_WORLD_41, "world_41" }
	,{ BUTTON, SDLK_WORLD_42, "world_42" }
	,{ BUTTON, SDLK_WORLD_43, "world_43" }
	,{ BUTTON, SDLK_WORLD_44, "world_44" }
	,{ BUTTON, SDLK_WORLD_45, "world_45" }
	,{ BUTTON, SDLK_WORLD_46, "world_46" }
	,{ BUTTON, SDLK_WORLD_47, "world_47" }
	,{ BUTTON, SDLK_WORLD_48, "world_48" }
	,{ BUTTON, SDLK_WORLD_49, "world_49" }
	,{ BUTTON, SDLK_WORLD_50, "world_50" }
	,{ BUTTON, SDLK_WORLD_51, "world_51" }
	,{ BUTTON, SDLK_WORLD_52, "world_52" }
	,{ BUTTON, SDLK_WORLD_53, "world_53" }
	,{ BUTTON, SDLK_WORLD_54, "world_54" }
	,{ BUTTON, SDLK_WORLD_55, "world_55" }
	,{ BUTTON, SDLK_WORLD_56, "world_56" }
	,{ BUTTON, SDLK_WORLD_57, "world_57" }
	,{ BUTTON, SDLK_WORLD_58, "world_58" }
	,{ BUTTON, SDLK_WORLD_59, "world_59" }
	,{ BUTTON, SDLK_WORLD_60, "world_60" }
	,{ BUTTON, SDLK_WORLD_61, "world_61" }
	,{ BUTTON, SDLK_WORLD_62, "world_62" }
	,{ BUTTON, SDLK_WORLD_63, "world_63" }
	,{ BUTTON, SDLK_WORLD_64, "world_64" }
	,{ BUTTON, SDLK_WORLD_65, "world_65" }
	,{ BUTTON, SDLK_WORLD_66, "world_66" }
	,{ BUTTON, SDLK_WORLD_67, "world_67" }
	,{ BUTTON, SDLK_WORLD_68, "world_68" }
	,{ BUTTON, SDLK_WORLD_69, "world_69" }
	,{ BUTTON, SDLK_WORLD_70, "world_70" }
	,{ BUTTON, SDLK_WORLD_71, "world_71" }
	,{ BUTTON, SDLK_WORLD_72, "world_72" }
	,{ BUTTON, SDLK_WORLD_73, "world_73" }
	,{ BUTTON, SDLK_WORLD_74, "world_74" }
	,{ BUTTON, SDLK_WORLD_75, "world_75" }
	,{ BUTTON, SDLK_WORLD_76, "world_76" }
	,{ BUTTON, SDLK_WORLD_77, "world_77" }
	,{ BUTTON, SDLK_WORLD_78, "world_78" }
	,{ BUTTON, SDLK_WORLD_79, "world_79" }
	,{ BUTTON, SDLK_WORLD_80, "world_80" }
	,{ BUTTON, SDLK_WORLD_81, "world_81" }
	,{ BUTTON, SDLK_WORLD_82, "world_82" }
	,{ BUTTON, SDLK_WORLD_83, "world_83" }
	,{ BUTTON, SDLK_WORLD_84, "world_84" }
	,{ BUTTON, SDLK_WORLD_85, "world_85" }
	,{ BUTTON, SDLK_WORLD_86, "world_86" }
	,{ BUTTON, SDLK_WORLD_87, "world_87" }
	,{ BUTTON, SDLK_WORLD_88, "world_88" }
	,{ BUTTON, SDLK_WORLD_89, "world_89" }
	,{ BUTTON, SDLK_WORLD_90, "world_90" }
	,{ BUTTON, SDLK_WORLD_91, "world_91" }
	,{ BUTTON, SDLK_WORLD_92, "world_92" }
	,{ BUTTON, SDLK_WORLD_93, "world_93" }
	,{ BUTTON, SDLK_WORLD_94, "world_94" }
	,{ BUTTON, SDLK_WORLD_95, "world_95" }
	,{ BUTTON, SDLK_KP0, "kp0" }
	,{ BUTTON, SDLK_KP1, "kp1" }
	,{ BUTTON, SDLK_KP2, "kp2" }
	,{ BUTTON, SDLK_KP3, "kp3" }
	,{ BUTTON, SDLK_KP4, "kp4" }
	,{ BUTTON, SDLK_KP5, "kp5" }
	,{ BUTTON, SDLK_KP6, "kp6" }
	,{ BUTTON, SDLK_KP7, "kp7" }
	,{ BUTTON, SDLK_KP8, "kp8" }
	,{ BUTTON, SDLK_KP9, "kp9" }
	,{ BUTTON, SDLK_KP_PERIOD, "kp_period" }
	,{ BUTTON, SDLK_KP_DIVIDE, "kp_divide" }
	,{ BUTTON, SDLK_KP_MULTIPLY, "kp_multiply" }
	,{ BUTTON, SDLK_KP_MINUS, "kp_minus" }
	,{ BUTTON, SDLK_KP_PLUS, "kp_plus" }
	,{ BUTTON, SDLK_KP_ENTER, "kp_enter" }
	,{ BUTTON, SDLK_KP_EQUALS, "kp_equals" }
	,{ BUTTON, SDLK_UP, "up" }
	,{ BUTTON, SDLK_DOWN, "down" }
	,{ BUTTON, SDLK_RIGHT, "right" }
	,{ BUTTON, SDLK_LEFT, "left" }
	,{ BUTTON, SDLK_INSERT, "insert" }
	,{ BUTTON, SDLK_HOME, "home" }
	,{ BUTTON, SDLK_END, "end" }
	,{ BUTTON, SDLK_PAGEUP, "pageup" }
	,{ BUTTON, SDLK_PAGEDOWN, "pagedown" }
	,{ BUTTON, SDLK_F1, "f1" }
	,{ BUTTON, SDLK_F2, "f2" }
	,{ BUTTON, SDLK_F3, "f3" }
	,{ BUTTON, SDLK_F4, "f4" }
	,{ BUTTON, SDLK_F5, "f5" }
	,{ BUTTON, SDLK_F6, "f6" }
	,{ BUTTON, SDLK_F7, "f7" }
	,{ BUTTON, SDLK_F8, "f8" }
	,{ BUTTON, SDLK_F9, "f9" }
	,{ BUTTON, SDLK_F10, "f10" }
	,{ BUTTON, SDLK_F11, "f11" }
	,{ BUTTON, SDLK_F12, "f12" }
	,{ BUTTON, SDLK_F13, "f13" }
	,{ BUTTON, SDLK_F14, "f14" }
	,{ BUTTON, SDLK_F15, "f15" }
	,{ BUTTON, SDLK_NUMLOCK, "numlock" }
	,{ BUTTON, SDLK_CAPSLOCK, "capslock" }
	,{ BUTTON, SDLK_SCROLLOCK, "scrollock" }
	,{ BUTTON, SDLK_RSHIFT, "rshift" }
	,{ BUTTON, SDLK_LSHIFT, "lshift" }
	,{ BUTTON, SDLK_RCTRL, "rctrl" }
	,{ BUTTON, SDLK_LCTRL, "lctrl" }
	,{ BUTTON, SDLK_RALT, "ralt" }
	,{ BUTTON, SDLK_LALT, "lalt" }
	,{ BUTTON, SDLK_RMETA, "rmeta" }
	,{ BUTTON, SDLK_LMETA, "lmeta" }
	,{ BUTTON, SDLK_LSUPER, "lsuper" }
	,{ BUTTON, SDLK_RSUPER, "rsuper" }
	,{ BUTTON, SDLK_MODE, "mode" }
	,{ BUTTON, SDLK_COMPOSE, "compose" }
	,{ BUTTON, SDLK_HELP, "help" }
	,{ BUTTON, SDLK_PRINT, "print" }
	,{ BUTTON, SDLK_SYSREQ, "sysreq" }
	,{ BUTTON, SDLK_BREAK, "break" }
	,{ BUTTON, SDLK_MENU, "menu" }
	,{ BUTTON, SDLK_POWER, "power" }
	,{ BUTTON, SDLK_EURO, "euro" }
	,{ BUTTON, SDLK_UNDO, "undo" }

	/* Miscellaneous */
	,{ BUTTON, SDLK_LAST+1, "quit" }

	/* Mouse */
	,{ POINTER, SDLK_LAST+11, "mouseptr" }
	,{ BALL, SDLK_LAST+12, "mousemove" }
	,{ BUTTON, SDLK_LAST+13, "mouse1" }
	,{ BUTTON, SDLK_LAST+14, "mouse2" }
	,{ BUTTON, SDLK_LAST+15, "mouse3" }
	,{ WHEEL, SDLK_LAST+16, "mousewheel" }
	/* -- Alternatively... (needs changes in code too)
	,{ PULSE, SDLK_LAST+16, "mousewheelup" }
	,{ PULSE, SDLK_LAST+17, "mousewheeldown" }
	*/
	,{ BUTTON, SDLK_LAST+18, "mouse4" }
	,{ BUTTON, SDLK_LAST+19, "mouse5" }

	/* End of list. */
	,{ 0, 0, NULL }
};

static struct input inputs[MAX_INPUTS];

static int mouse_mode;

static int num_joysticks;

static struct joystick
{
	SDL_Joystick *handle;
	int id;
	const char *name;
	int axes;
	int buttons;
	int balls;
} joysticks[MAX_JOYSTICKS];

static struct joystick *joy_from_id(int id)
{
	int i;

	for (i=0; i<MAX_JOYSTICKS; i++)
	{
		if (id == joysticks[i].id)
			break;
	}
	if (i >= MAX_JOYSTICKS)
	{
		ERROR("input id %d does not correspond to a joystick input", id);
		return NULL;
	}

	return &joysticks[i];
}

static int joy_control_to_id(struct joystick *joy, int type, int n)
{
	return SDLK_LAST+30
		+ joy->id * 32
		+ (type==BUTTON?8:(type==AXIS?0:24))
		+ n;
}

/*
static void joy_id_to_control(int id, struct joystick **joy, int *type, int *n)
{
	struct joystick *j;
	int i;

	id -= SDLK_LAST+30;
	for (i=0; i<MAX_JOYSTICKS; i++)
	{
		if (id>>5 == joysticks[i].id)
			break;
	}
	if (i >= MAX_JOYSTICKS)
	{
		ERROR("input id %d does not correspond to a joystick input", id);
		*joy = NULL;
		return;
	}
	id &= 31;
	if (id >= 24)
	{
		*type = POINTER;
		id -= 24;
	}
	else if (id >= 8)
	{
		*type = BUTTON;
		id -= 8;
	}
	else
		*type = AXIS;
	*n = id;
}
*/

static void add_joystick_control(struct joystick *joy, int type, int n)
{
	char name[32];

	if (num_inputs >= MAX_INPUTS)
	{
		ERROR("MAX_INPUTS (%d) reached! Unable to add more.", MAX_INPUTS);
		return;
	}
	sprintf(name, "joy%d%s%d"
		,joy->id+1
		,(type==BUTTON?"btn":(type==AXIS?"axis":"ball"))
		,n+1
	);
	DEBUG(1, "Adding %s %d on joystick %s as input \"%s\" (%d)\n"
		,(type==BUTTON?"button":(type==AXIS?"axis":"trackball"))
		,n+1
		,joy->name
		,name
		,joy_control_to_id(joy, type, n)
	);
	inputs[num_inputs].type = type;
	inputs[num_inputs].id = joy_control_to_id(joy, type, n);
	strcpy(inputs[num_inputs].name, name);
	num_inputs++;
}

static void init_joysticks(void)
{
	int n, i, j, k;
	struct joystick *joy;

	DEBUG(1, "input_sdl: init_joysticks()\n");
	n = SDL_NumJoysticks();
	if (n <= 0)
	{
		DEBUG(1, "No joysticks found\n");
		return;
	}
	if (debug_level >= 2)
	{
		for (i=0; i<n; i++)
			DEBUG(1, "Found joystick %d: \"%s\"\n", i+1, SDL_JoystickName(i));
	}
	if (n > MAX_JOYSTICKS)
	{
		ERROR(
			"Note: %d joysticks were detected, but only %d are supported.\n"
			"The remaining ones will be ignored. In order to fix this,\n"
			"increase MAX_JOYSTICKS in " __FILE__ " and recompile the game.\n"
			, n, MAX_JOYSTICKS
		);
		n = MAX_JOYSTICKS;
	}
	for (i=0, j=0; i<n; i++)
	{
		joy = &joysticks[j];
		joy->handle = SDL_JoystickOpen(i);
		if (joy->handle)
		{
			joy->id = i;
			joy->name = SDL_JoystickName(i);
			joy->axes = SDL_JoystickNumAxes(joy->handle);
			joy->buttons = SDL_JoystickNumButtons(joy->handle);
			joy->balls = SDL_JoystickNumBalls(joy->handle);
			for (k=0; k < joy->axes; k++)
				add_joystick_control(joy, AXIS, k);
			for (k=0; k < joy->buttons; k++)
				add_joystick_control(joy, BUTTON, k);
			for (k=0; k < joy->balls; k++)
				add_joystick_control(joy, POINTER, k);
			j++;
			DEBUG(1, "Initialized joystick %s: %d axes, %d buttons"
				, joy->name, joy->axes, joy->buttons);
			if (joy->balls > 0)
				DEBUG(1, ", %d trackballs", joy->balls);
			DEBUG(1, "\n");
		}
		else
		{
			ERROR(
				"An error occurred while trying to trying to initialize\n"
				"the \"%s\" joystick for use: %s\n"
				, SDL_JoystickName(i), SDL_GetError()
			);
		}
	}
	num_joysticks = j;
}

static int init(void)
{
	int i;

	DEBUG(1, "input_sdl: init()\n");
	for (i=0; fixed_inputs[i].type; i++)
	{
		inputs[i].type = fixed_inputs[i].type;
		inputs[i].id = fixed_inputs[i].id;
		strncpy(inputs[i].name, fixed_inputs[i].name, sizeof(inputs[i].name));
	}
	inputs[i].type = 0;
	num_inputs = i;
	mouse_mode = MOUSE_POINTER;
	if (config_get_int("joystick"))
	{
		DEBUG(1, "input_sdl: initializing joystick subsystem\n");
		if (SDL_InitSubSystem(SDL_INIT_JOYSTICK))
		{
			ERROR("SDL_InitSubSystem(): %s", SDL_GetError());
			return 1;
		}
		init_joysticks();
	}

	return 0;
}

static void shutdown(void)
{
	DEBUG(1, "input_sdl: shutdown()\n");
	if (num_joysticks > 0)
	{
		DEBUG(1, "input_sdl: shutting down joystick subsystem\n");
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
	}
	num_inputs = 0;
	inputs[0].type = 0;
}

static const struct input *get_inputs(int *n)
{
	if (n)
	{
		DEBUG(3, "input_sdl: get_inputs(%p <- %d) = %p\n"
			, n, num_inputs, inputs);
		*n = num_inputs;
	}
	else
	{
		DEBUG(3, "input_sdl: get_inputs(NULL) = %p\n", inputs);
	}
	return inputs;
}

static int get_event(struct input_event *event, int wait)
{
	SDL_Event sdl_ev;
	int ret;

	DEBUG(3, "input_sdl: get_event(x, %s %d)\n"
		,wait?"/*wait*/":"/*poll*/"
		,wait);
	if (wait)
	{
		ret = SDL_WaitEvent(&sdl_ev);
		if (!ret)
			ERROR("error while polling/waiting for event!");
	}
	else
		ret = SDL_PollEvent(&sdl_ev);
	if (!ret)
		return 1;
	switch (sdl_ev.type)
	{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			event->type = BUTTON;
			event->id = sdl_ev.key.keysym.sym;
			event->pressed = sdl_ev.type == SDL_KEYDOWN;
			if (event->pressed)
			{
				DEBUG(3, "Key press event, id == %d\n", event->id);
			}
			else
			{
				DEBUG(3, "Key release event, id == %d\n", event->id);
			}
			break;
		case SDL_MOUSEMOTION:
			if (mouse_mode == MOUSE_POINTER)
			{
				event->type = POINTER;
				event->id = SDLK_LAST+11;
				event->x = sdl_ev.motion.x;
				event->y = sdl_ev.motion.y;
				DEBUG(3, "Mouse pointer event, id == %d, pos = (%g, %g)\n", event->id, event->x, event->y);
			}
			else if (mouse_mode == MOUSE_MOVE)
			{
				event->type = BALL;
				event->id = SDLK_LAST+12;
				event->x = sdl_ev.motion.xrel;
				event->y = sdl_ev.motion.yrel;
				DEBUG(3, "Mouse motion event, id == %d, movement = (%g, %g)\n", event->id, event->x, event->y);
			}
			else
			{
				ERROR("invalid mouse_mode value %d (!), discarding event", mouse_mode);
				return 1;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
#if SDL_VERSION_ATLEAST(1, 2, 5)
			/* Handle mouse wheel events specially. */
			if (sdl_ev.button.button == SDL_BUTTON_WHEELUP)
			{
				event->type = WHEEL;
				event->id = SDLK_LAST+16;
				event->value = -1;
				DEBUG(3, "Mouse wheel (up) event, id == %d\n", event->id);
				break;
			}
			else if (sdl_ev.button.button == SDL_BUTTON_WHEELDOWN)
			{
				event->type = WHEEL;
				event->id = SDLK_LAST+16;
				event->value = 1;
				DEBUG(3, "Mouse wheel (down) event, id == %d\n", event->id);
				break;
			}
			/* If neither, pass it through to the code below. */
#endif
		case SDL_MOUSEBUTTONUP:
			event->type = BUTTON;
			switch (sdl_ev.button.button)
			{
				case SDL_BUTTON_LEFT: event->id = SDLK_LAST+13; break;
				case SDL_BUTTON_MIDDLE: event->id = SDLK_LAST+14; break;
				case SDL_BUTTON_RIGHT: event->id = SDLK_LAST+15; break;
#if SDL_VERSION_ATLEAST(1, 2, 5)
				case SDL_BUTTON_WHEELUP:
				case SDL_BUTTON_WHEELDOWN:
					/* Ignore mouse wheel "release" events. */
					return 1;
#endif
#if SDL_VERSION_ATLEAST(1, 2, 13)
				case SDL_BUTTON_X1: event->id = SDLK_LAST+18; break;
				case SDL_BUTTON_X2: event->id = SDLK_LAST+19; break;
#endif
			}
			event->pressed = sdl_ev.type == SDL_MOUSEBUTTONDOWN;
			if (event->pressed)
			{
				DEBUG(3, "Mouse button press event, id == %d\n", event->id);
			}
			else
			{
				DEBUG(3, "Mouse button release event, id == %d\n", event->id);
			}
			break;
		case SDL_JOYAXISMOTION:
			event->type = AXIS;
			event->id = joy_control_to_id(
				 joy_from_id(sdl_ev.jaxis.which)
				,AXIS
				,sdl_ev.jaxis.axis
			);
			event->value = (float) sdl_ev.jaxis.value / 32768.0f;
			DEBUG(3, "Joystick axis event, id == %d, value = %g\n", event->id, event->value);
			break;
		case SDL_JOYBUTTONDOWN:
		case SDL_JOYBUTTONUP:
			event->type = BUTTON;
			event->id = joy_control_to_id(
				 joy_from_id(sdl_ev.jbutton.which)
				,BUTTON
				,sdl_ev.jbutton.button
			);
			event->pressed = sdl_ev.type == SDL_JOYBUTTONDOWN;
			DEBUG(3, "Joystick button event, id == %d, state = %s\n", event->id, event->pressed ? "pressed" : "released");
			break;
		case SDL_JOYBALLMOTION:
			event->type = POINTER;
			event->id = joy_control_to_id(
				 joy_from_id(sdl_ev.jball.which)
				,POINTER
				,sdl_ev.jball.ball
			);
			event->x = sdl_ev.jball.xrel;
			event->y = sdl_ev.jball.yrel;
			DEBUG(3, "Joystick trackball event, id == %d, movement = (%d, %d)\n", event->id, sdl_ev.jball.xrel, sdl_ev.jball.yrel);
			break;
		case SDL_QUIT:
			event->type = BUTTON;
			event->id = SDLK_LAST+1;
			event->pressed = 1;
			DEBUG(3, "Quit event, id == %d\n", event->id);
			break;
		default:
			DEBUG(3, "Unhandled event (discarded)\n");
			return 1;
	}

	return 0;
}

static void clear_queue(void)
{
	SDL_Event null;

	DEBUG(2, "input_sdl: clear_queue()\n");
	while (SDL_PollEvent(&null));
}

static void set_mouse_mode(int mode)
{
	switch (mode)
	{
		case MOUSE_POINTER:
			DEBUG(2, "input_sdl: set_mouse_mode(MOUSE_POINTER)\n");
			mouse_mode = MOUSE_POINTER;
			DEBUG(2, "input_sdl: releasing input\n");
			SDL_ShowCursor(SDL_ENABLE);
			SDL_WM_GrabInput(SDL_GRAB_OFF);
			break;
		case MOUSE_MOVE:
			DEBUG(2, "input_sdl: set_mouse_mode(MOUSE_MOVE)\n");
			mouse_mode = MOUSE_MOVE;
			DEBUG(2, "input_sdl: grabbing input\n");
			SDL_ShowCursor(SDL_DISABLE);
			SDL_WM_GrabInput(SDL_GRAB_ON);
			break;
		default:
			ERROR("invalid mouse mode %d\n", mode);
	}
}

struct input_driver input_sdl =
{
	.init = init,
	.shutdown = shutdown,
	.get_inputs = get_inputs,
	.get_event = get_event,
	.clear_queue = clear_queue,
	.set_mouse_mode = set_mouse_mode
};
