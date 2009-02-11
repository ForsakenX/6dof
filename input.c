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

struct input_driver *input;

int num_inputs;
const struct input *inputs;

static void init_lua_table(lua_State *L)
{
	int i;

	lua_createtable(L, 0, 2);
	lua_createtable(L, 0, num_inputs);
	for (i=0; i<num_inputs; i++)
	{
		lua_pushinteger(L, inputs[i].id);
		lua_setfield(L, -2, inputs[i].name);
	}
	lua_setfield(L, -2, "id");
	lua_newtable(L);
	lua_setfield(L, -2, "bindings");
	lua_newtable(L);
	lua_setfield(L, -2, "funcs");
	lua_setglobal(L, "input");
	DEBUG(6, "init_lua_table(): input module initialized (%d inputs)\n"
		, num_inputs);
}

/* Returns -1 if the name doesn't exist. */
static int input_name_to_id(const char *name)
{
	int id;

	lua_getglobal(L1, "input");
	lua_getfield(L1, -1, "id");
	lua_getfield(L1, -1, name);
	if (lua_isnumber(L1, -1))
		id = lua_tointeger(L1, -1);
	else
		id = -1;
	lua_pop(L1, 3);

	return id;
}

/* A C closure bound to a C based input event handler (input_func).
 * It takes a struct input_event light userdata as argument. */
static int input_from_lua(lua_State *L)
{
	int u;
	input_func f;
	struct input_event *event;

	u = lua_upvalueindex(1);
	if (!lua_isuserdata(L, u))
	{
		ERROR("expected input_func as upvalue 1 - event ignored");
		return 0;
	}
	f = lua_touserdata(L, u);
	if (!lua_isuserdata(L, 1))
	{
		ERROR("expected struct input_event as argument 1 - event ignored");
		return 0;
	}
	event = lua_touserdata(L, 1);
	DEBUG(8, "C function (%#tx) triggered by input event with id %d\n"
		, f, event->id
	);
	lua_pop(L, 1);
	f(event);
	return 0;
}

/* [-0, +1, e] */
static void push_lua_event_data(lua_State *L, const struct input_event *event)
{
	lua_createtable(L, 0, 3);
	lua_pushinteger(L, event->id);
	lua_setfield(L, -2, "id");
	switch (event->type)
	{
		case BUTTON:
			lua_pushliteral(L, "button");
			lua_setfield(L, -2, "type");
			lua_pushboolean(L, event->pressed != 0);
			lua_setfield(L, -2, "pressed");
			break;
		case AXIS:
			lua_pushliteral(L, "axis");
			lua_setfield(L, -2, "type");
			lua_pushnumber(L, event->value);
			lua_setfield(L, -2, "value");
			break;
		case POINTER:
			lua_pushliteral(L, "pointer");
			lua_setfield(L, -2, "type");
			lua_pushnumber(L, event->x);
			lua_setfield(L, -2, "x");
			lua_pushnumber(L, event->y);
			lua_setfield(L, -2, "y");
			break;
		default:
			/* Trouble. The event shouldn't have made it this far. */
			luaL_error(L, "invalid event type %d", event->type);
	}
}

/* [-0, +?, e] */
static int push_lua_table(lua_State *L, int index)
{
	/* TODO: move this function somewhere else! */
	int i, len;

	if (!lua_istable(L, index))
	{
		luaL_error(L, "push_lua_table called on non-table at index %d"
			, index);
	}
	len = lua_objlen(L, index);
	for (i=1; i<=len; i++)
	{
		lua_pushinteger(L, i);
		if (index < 0)
			lua_gettable(L, --index);
		else
			lua_gettable(L, index);
	}

	return len;
}

static int set_mouse_move_func(lua_State *L)
{
	int n;

	n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "bad number of arguments (expected 1, got %d)", n);
	luaL_checktype(L, 1, LUA_TBOOLEAN);
	if (lua_toboolean(L, 1))
		input->set_mouse_mode(MOUSE_MOVE);
	else
		input->set_mouse_mode(MOUSE_POINTER);

	return 0;
}

static const luaL_Reg lua_funcs[] =
{
	{ "setmousecontrol", set_mouse_move_func },
	{ NULL, NULL }
};

static void register_lua_funcs(lua_State *L)
{
	lua_getglobal(L, "input");
	luaL_register(L, NULL, lua_funcs);
	lua_setglobal(L, "input");
}

int input_init(void)
{
	int ret;

	DEBUG(1, "Initializing input subsystem\n");
	DEBUG(2, "Using SDL for input\n");
	input = &input_sdl;
	ret = input->init();
	if (ret)
		return ret;
	inputs = input->get_inputs(&num_inputs);
	init_lua_table(L1);
	register_lua_funcs(L1);
	LUA_RUN("input.lua");

	return 0;
}

void input_shutdown(void)
{
	DEBUG(1, "Shutting down input subsystem\n");
	input->shutdown();
}

void input_register_func(const char *name, input_func function)
{
	lua_getglobal(L1, "input");
	lua_getfield(L1, -1, "funcs");
	lua_pushstring(L1, name);
	lua_pushlightuserdata(L1, function);
	lua_pushcclosure(L1, input_from_lua, 1);
	lua_settable(L1, -3);
	lua_pop(L1, 2);
}

void input_process_events(int wait)
{
	struct input_event event;
	int i;

	DEBUG(8, "Processing input events\n");
	lua_getglobal(L1, "input");
	lua_getfield(L1, -1, "bindings");
	while (!input->get_event(&event, wait))
	{
		lua_pushinteger(L1, event.id);
		lua_gettable(L1, -2);
		if (lua_isnil(L1, -1))
		{
			DEBUG(6, "Unbound input event (id == %d)\n", event.id);
			/* Pop the function from the stack. */
			lua_pop(L1, 1);
		}
		else if (lua_iscfunction(L1, -1))
		{
			DEBUG(5, "Calling C function for input event %d\n", event.id);
			lua_pushlightuserdata(L1, &event);
			if (lua_pcall(L1, 1, 0, 0))
			{
				ERROR(lua_tostring(L1, -1));
			}
		}
		else if (lua_isfunction(L1, -1))
		{
			DEBUG(5, "Calling Lua function for input event %d\n", event.id);
			push_lua_event_data(L1, &event);
			if (lua_pcall(L1, 1, 0, 0))
			{
				ERROR(lua_tostring(L1, -1));
			}
		}
		else if (lua_istable(L1, -1))
		{
			lua_getfield(L1, -1, "func");
			if (!lua_isfunction(L1, -1))
			{
				ERROR("binding table (%d) must contain function to call"
					, event.id);
				lua_pop(L1, 2);
				continue;
			}
			push_lua_event_data(L1, &event);
			i = push_lua_table(L1, -3);
			DEBUG(5, "Calling function for input event %d (with %d arguments)\n"
				, event.id, i);
			if (lua_pcall(L1, i+1, 0, 0))
			{
				ERROR(lua_tostring(L1, -1));
			}
		}
		else
		{
			ERROR("invalid binding for event %d (must be a function)"
				, event.id);
		}
	}
	/* Pop input.bindings and input tables from the stack. */
	lua_pop(L1, 2);
}
