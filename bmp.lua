-- vim:set sw=4 ts=4:
--
-- This Lua script reads texture data from a .bmp file.
-- NOTE: only partial .bmp support is provided (enough for loading
--       Forsaken texture data).
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

local f = binfile(io.open(..., 'rb'))
if not f:binread('h') == 0x4d42 then
	error('not a BMP file')
end
local size = f:binread('i')
if size <= 0 then
	error(('invalid size specification (%d)'):format(size))
end
-- Ignore reserved value
f:binread('i')
local offset = f:binread('i')
if offset < 0 then
	error(('invalid offset (%d)'):format(offset))
end
-- Now read the DIB header
local hdrsize, width, height, bpp, format, size2 = f:binread('iii??hii')
if bpp ~= 8 and bpp ~= 24 then
	error(('%d bpp not supported'):format(bpp))
end
if format ~= 0 then
	error(('format %d not supported - must be 0 (RGB)'):format(format))
end
if size2 ~= 0 and size ~= size2 + offset then
	error(('invalid image size %d (must be equal to file size %d '
	    .. '+ bitmap data offset %d)'):format(size, size2, offset))
end
-- Skip resolution data
f:read(8)
-- Palette info
local ncolors = f:binread('i????')
if hdrsize > 40 then
	-- Skip additional header info and warn...
	f:read(hdrsize-40)
	warn(('BMP reader: warning: unknown header type (size %d) but '
	   .. 'trying to continue'):format(hdrsize))
end
local colors = {}
if bpp == 8 then
	-- Read the color palette
	for i = 1, ncolors do
		local b, g, r = f:binread('BBB?')
		table.insert(colors, string.char(r, g, b))
	end
end
f:seek("set", offset)
-- Now read the pixel data
local data = ''
local rows = {}
for _ = 1, height do
	local row = {}
	for _ = 1, width do
		table.insert(row, colors[f:read(1):byte()+1])
	end
	table.insert(rows, row)
end
f:close()
for i = height, 1, -1 do
	data = data .. table.concat(rows[i])
end

return {
	width = width;
	height = height;
	flags = 0;
	data = data;
}
