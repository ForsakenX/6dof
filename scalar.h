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

#ifndef SCALAR_H
#define SCALAR_H

#include "common.h"

#if defined(SCALAR_FLOAT)

#define sins sinf
#define coss cosf
#define sqrts sqrtf

#elif defined(SCALAR_DOUBLE)

#define sins sin
#define coss cos
#define sqrts sqrt

#elif defined(SCALAR_LONG_DOUBLE)

#define sins sinl
#define coss cosl
#define sqrts sqrtl

#endif

#endif /* SCALAR_H */
