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

#ifndef FILE_H
#define FILE_H

#include "common.h"

typedef struct _io io;

/* Open a file given its name and return an opaque handle that can
 * be passed to the other io_* functions.
 *
 * Returns NULL on failure. */
io *io_open_file(const char *name, const char *mode);

/* Open a stream given its file descriptor and return an opaque
 * handle that can be passed to the other io_* functions.
 *
 * Returns NULL on failure. */
io *io_open_fd(int fd, const char *mode);

/* Close the given stream. */
void io_close(io *f);

/* Check whether f points to a valid I/O handle.
 *
 * Returns non-zero if and only if it does. */
int io_valid(io *f);

/* Returns a textual description of the given I/O handle. */
char *io_type_desc(io *f);

/* Read len bytes from the given I/O handle into buf.
 *
 * Returns the number of bytes actually read, 0 on EOF, or <0
 * on failure. */
int io_read(void *buf, size_t len, io *f);

/* Write len bytes from buf to the given I/O handle.
 *
 * Returns the number of bytes actually written, or <0 on
 * failure. */
int io_write(const void *buf, size_t len, io *f);

#endif /* FILE_H */
