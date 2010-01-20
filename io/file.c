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

#define DEBUG(level, x...) DEBUGX(DBG_IO, level, x)

#define VALID    0x01010101
#define READABLE 0x02020202
#define WRITABLE 0x04040404

struct _io
{
	char desc[256];
	FILE *fh;
	int flags;
};

io *io_open_file(const char *name, const char *mode)
{
	io *f;
	const char *c;

	DEBUG(2, "Opening file \"%s\" with mode \"%s\"\n", name, mode);
	f = malloc(sizeof(io));
	if (!f)
		return NULL;
	f->fh = fopen(name, mode);
	if (!f->fh)
	{
		free(f);
		return NULL;
	}
	f->flags = VALID;
	for (c=mode; *c; c++)
	{
		switch (*c)
		{
			case 'r':
				f->flags |= READABLE;
			case 'w':
				f->flags |= WRITABLE;
			case '+':
				f->flags |= READABLE | WRITABLE;
		}
	}
	strcpy(f->desc, "file \"");
	strncat(f->desc, name, 248);
	strcat(f->desc, "\"");
	return f;
}

io *io_open_fd(int fd, const char *mode)
{
	io *f;

	DEBUG(2, "Opening fd %d with mode \"%s\"\n", fd, mode);
	f = malloc(sizeof(io));
	if (!f)
		return NULL;
	f->fh = fdopen(fd, mode);
	if (!f->fh)
	{
		free(f);
		return NULL;
	}
	sprintf(f->desc, "fd %d", fd);
	return f;
}

int io_valid(io *f)
{
	return f
		&& (f->flags & VALID)
		&& ((f->flags & 0x000000ff) == (f->flags & 0xff000000))
		&& ((f->flags & 0x000000ff) == (f->flags & 0x00ff0000))
		&& ((f->flags & 0x000000ff) == (f->flags & 0x0000ff00));
}

char *io_type_desc(io *f)
{
	if (!io_valid(f))
	{
		ERROR("io_type_desc() called on corrupted/uninitialized I/O handle");
		return "corrupted/uninitialized I/O handle";
	}
	return f->desc;
}

void io_close(io *f)
{
	if (!io_valid(f))
	{
		ERROR("io_close() called on corrupted/uninitialized I/O handle");
		return;
	}
	if (fclose(f->fh))
		ERROR("error while closing stream!");
	memset(f, 0, sizeof(io));
	free(f);
}

int io_read(void *buf, size_t len, io *f)
{
	int n;

	if (!io_valid(f))
	{
		ERROR("io_read() called on corrupted/uninitialized I/O handle");
		return INT_MIN;
	}
	if ((f->flags & READABLE) != READABLE)
	{
		ERROR("io_read() called on unreadable I/O handle");
		return INT_MIN;
	}
	DEBUG(3, "Reading %zd bytes from %s\n", len, f->desc);
	n = fread(buf, 1, len, f->fh);
	if (n <= 0)
	{
		if (feof(f->fh))
			return 0;
		else if (ferror(f->fh))
			return -errno;
		else
		{
			ERROR("WTF - read failed but no EOF/error reported");
			return INT_MIN;
		}
	}
	return n;
}

int io_write(const void *buf, size_t len, io *f)
{
	int n;

	if (!io_valid(f))
	{
		ERROR("io_write() called on corrupted/uninitialized I/O handle");
		return INT_MIN;
	}
	if ((f->flags & WRITABLE) != WRITABLE)
	{
		ERROR("io_write() called on unwritable I/O handle");
		return INT_MIN;
	}
	DEBUG(3, "Writing %zd bytes from %s\n", len, f->desc);
	n = fwrite(buf, 1, len, f->fh);
	if (n <= 0)
	{
		if (feof(f->fh))
			return 0;
		else if (ferror(f->fh))
			return -errno;
		else
		{
			ERROR("WTF - read failed but no EOF/error reported");
			return INT_MIN;
		}
	}
	return n;
}
