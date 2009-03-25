-- vim:set sw=4 ts=4:
--
-- This Lua script reads texture data from a .ppm file.
--
-- Copyright (C) 2009  Pim Goossens
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License along
-- with this program; if not, see <http://www.gnu.org/licenses/>, or write
-- to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
-- Boston, MA 02110-1301 USA.

local f = io.open(..., 'rb')
if not f then
	error('error while opening file ' .. (...))
end
-- Read the header
local magic = f:read()
if not magic == 'P6' then
	error('not a PPM file')
end
local width, height, maxval = f:read("*n", "*n", "*n")
if not maxval == 255 then
	error('expected maxval of 255')
end
-- Skip the single whitespace character following the maxval
f:read(1)
-- Now read the image data
data = f:read(width * height * 3)
if #data < width * height * 3 then
	error('insufficient data')
end
f:close()

return {
	width = width;
	height = height;
	flags = 0;
	data = data;
}
