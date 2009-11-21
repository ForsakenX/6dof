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

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include "../include/common.h"

#define MAX_SOUNDS 32

int num_sounds;
Mix_Chunk *sounds[MAX_SOUNDS];

/* Returns index, or -1 if no slots are available. */
static int find_empty_slot(void)
{
	int i;

	for (i=0; i<MAX_SOUNDS; i++)
	{
		if (sounds[i] == NULL)
			return i;
	}
	return -1;
}

static void stop_all_sounds(void)
{
}

static int init(void)
{
	int i;

	DEBUG(3, "audio_sdlmixer: init()\n");
	if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
	{
		ERROR("SDL_InitSubsystem(): %s\n", SDL_GetError());
		return 1;
	}
	if (Mix_OpenAudio(config_get_int("audio_frequency"), MIX_DEFAULT_FORMAT, 2,
		config_get_int("audio_buffer_size")) == -1)
	{
		ERROR("Mix_OpenAudio(): %s\n", SDL_GetError());
		return 1;
	}
	for (i=0; i<MAX_SOUNDS; i++)
		sounds[i] = NULL;
	return 0;
}

static void shutdown(void)
{
	DEBUG(3, "audio_sdlmixer: shutdown()\n");
	stop_all_sounds();
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

static int load_sound(char *filename)
{
	Mix_Chunk *s;
	int index;

	DEBUG(4, "audio_sdlmixer: load_sound(%s)\n", filename);
	index = find_empty_slot();
	if (index == -1)
	{
		ERROR("no free sound slots left, increase MAX_SOUNDS and recompile");
		return -1;
	}
	s = Mix_LoadWAV(filename);
	if (s)
	{
		sounds[index] = s;
	}
	else
	{
		ERROR("Failed to load sound %s: %s\n", filename, SDL_GetError());
		return -1;
	}
	return index;
}

static void free_sound(int soundno)
{
	DEBUG(5, "audio_sdlmixer: free_sound(%d)\n", soundno);
	if (sounds[soundno] == NULL)
	{
		ERROR("attempt to free unused sound slot");
		return;
	}
	Mix_FreeChunk(sounds[soundno]);
	sounds[soundno] = NULL;
}

static int play_sound(int soundno, int volume, float panning, int flags)
{
	DEBUG(5, "audio_sdlmixer: play_sound(%d, %d, %.3f, %#x)\n",
		soundno, volume, panning, flags);
	return 0;
}

static int play_sound_at(int soundno, scalar x, scalar y, scalar z, int volume, int flags)
{
	DEBUG(5, "audio_sdlmixer: play_sound_at(%d, %.3f, %.3f, %.3f, %d, %#x)\n",
		soundno, x, y, z, volume, flags);
	return 0;
}

static int stop_sound(int handle)
{
	DEBUG(5, "audio_sdlmixer: stop_sound(%d)\n", handle);
	return 0;
}

struct audio_driver audio_sdlmixer =
{
	.init = init,
	.shutdown = shutdown,
	.load = load_sound,
	.free = free_sound,
	.play = play_sound,
	.play_at = play_sound_at,
	.stop = stop_sound
};
