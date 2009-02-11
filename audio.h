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

#ifndef AUDIO_H
#define AUDIO_H

#include "common.h"

/* The interface for audio subsystems. */
struct audio_driver
{
	/* Perform initialization. No parameters are given directly, but
	 * they can be retrieved through the global configuration system,
	 * using the config_get_*() functions.
	 *
	 * Returns 0 on success, non-zero otherwise. */
	int (*init)(void);

	/* Clean up and free resources held by this audio module. This
	 * function never fails. Warnings may be printed if unexpected
	 * conditions occur. */
	void (*shutdown)(void);

	/* Load a sound from a file.
	 *
	 * Returns a sound ID (which can be supplied to the play functions)
	 * which is zero or positive, or a negative value on failure. */
	int (*load)(char *filename);

	/* Unload the given sound from memory. This function can't fail,
	 * but may print a warning if e.g. an invalid soundno is given. */
	void (*free)(int soundno);

	/* Start playing the sound specified by soundno. The volume should
	 * be in the range 0-65535. The panning is specified as a floating
	 * point number in the range -1.0 (left) to 1.0 (right), with 0.0
	 * being in the center.
	 *
	 * Returns a handle that can be used to modify or stop the sound. */
	int (*play)(int soundno, int volume, float panning, int flags);

	/* Start playing the sound specified by soundno and located at the
	 * given offset to the center (0, 0, 0), specified by the x, y, and
	 * z parameters. This offset will determine the sound volume and
	 * panning. The volume parameter indicates how loud the sound
	 * would be played if it were at the center (zero distance).
	 *
	 * Returns a handle that can be used to modify or stop the sound. */
	int (*play_at)(int soundno, scalar x, scalar y, scalar z, int volume, int flags);

	/* Stop the given sound. */
	int (*stop)(int handle);
};

extern struct audio_driver audio_null;
#ifdef AUDIO_SDLMIXER
extern struct audio_driver audio_sdlmixer;
#endif

extern struct audio_driver *audio;

/* Initializes the default / currently selected audio subsystem. */
int audio_init(void);

/* Shuts down the currently initialized audio subsystem. */
void audio_shutdown(void);

#endif /* AUDIO_H */
