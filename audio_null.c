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

static int init(void)
{
	DEBUG(3, "audio_null: init()\n");
	return 0;
}

static void shutdown(void)
{
	DEBUG(3, "audio_null: shutdown()\n");
}

static int load_sound(const char *filename)
{
	DEBUG(4, "audio_null: load_sound(\"%s\")\n", filename);
	return 0;
}

static void free_sound(int soundno)
{
	DEBUG(5, "audio_null: free_sound(%d)\n", soundno);
}

static int play_sound(int soundno, int volume, float panning, int flags)
{
	DEBUG(5, "audio_null: play_sound(%d, %d, %.3f, %#x)\n",
		soundno, volume, panning, flags);
	return 0;
}

static int play_sound_at(int soundno, scalar x, scalar y, scalar z, int volume, int flags)
{
	DEBUG(5, "audio_null: play_sound_at(%d, %.3f, %.3f, %.3f, %d, %#x)\n",
		soundno, x, y, z, volume, flags);
	return 0;
}

static int stop_sound(int handle)
{
	DEBUG(5, "audio_null: stop_sound(%d)\n", handle);
	return 0;
}

struct audio_driver audio_null =
{
	.init = init,
	.shutdown = shutdown,
	.load = load_sound,
	.free = free_sound,
	.play = play_sound,
	.play_at = play_sound_at,
	.stop = stop_sound
};
