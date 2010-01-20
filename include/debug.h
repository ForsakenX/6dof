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

#ifndef DEBUG_H
#define DEBUG_H

#define DBG_LEVELMASK 3
#define DBG_CHANMASK ~DBG_LEVELMASK
#define DBG_CHAN(x) (1<<(x+2))

/* Debug types. Keep in sync with main/util.lua */
#define DBG_AUDIO   0
#define DBG_GENERAL 1
#define DBG_GFX     2
#define DBG_INPUT   3
#define DBG_IO      4
#define DBG_LEVEL   5
#define DBG_LUA     6 /* C <-> Lua interface - not Lua scripts themselves. */
#define DBG_MEM     7 /* Memory management. */

/* DEBUGX is used like printf() but with two extra arguments before
 * the format string:
 *
 *   - The first one specifies what this debug message is about;
 *     see the above list of debug types.
 *
 *   - The second one specifies the minimum debug level needed for
 *     the message to be printed.
 *
 *   Typically, a module will define a DEBUG macro to call DEBUGX
 *   with a default type for that module.
 */

#ifdef NDEBUG
#define DEBUGX(x...)
#else
/* Trying to split the definition of this macro across multiple lines
 * just makes it uglier. */
#define DEBUGX(type, level, x...) \
	{ if ((debug_level & DBG_LEVELMASK) >= level && (debug_level & DBG_CHANMASK & DBG_CHAN(type))) fprintf(stderr, x); }
#endif

/* Sets initial debug level and binds debugging-related functions
 * into the Lua environment. */
void debug_init(void);

#endif /* DEBUG_H */
