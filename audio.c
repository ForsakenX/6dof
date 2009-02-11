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

struct audio_driver *audio;

int audio_init(void)
{
	DEBUG(1, "Initializing audio subsystem\n");
#ifdef AUDIO_SDLMIXER
	DEBUG(2, "Using SDL_mixer for audio\n");
	audio = &audio_sdlmixer;
#else
	DEBUG(2, "Using dummy driver for audio (silence)\n");
	audio = &audio_null;
#endif
	return audio->init();
}

void audio_shutdown(void)
{
	DEBUG(1, "Shutting down audio subsystem\n");
	audio->shutdown();
}
