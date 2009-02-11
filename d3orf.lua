-- vim:set sw=4 ts=4:
--
-- This Lua script reads vertex/face data from a Descent 3 .orf level
-- room file.
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

local verts = {}
local faces = {}

DEBUG(4, "Loading "..config.lvlfile)
local f = binfile(io.open(config.lvlfile, 'rb'))
local hdrID, size, ver, nverts, nfaces = f:binread('iiiii')
DEBUG(4, ("Header says %d verts, %d faces"):format(nverts, nfaces))
local chunktype = f:binread('i')
while chunktype ~= 3 do
	local chunksize = f:binread('i')
	DEBUG(5, ("Chunk type: %d, size: %d"):format(chunktype,chunksize))
	if chunktype == 1 then
		-- For each vertex...
		for i = 1, nverts do
			local v = f:binread('v')
			table.insert(verts, v)
		end
	elseif chunktype == 2 then
		-- For each face...
		for i = 1, nfaces do
			local numverts, normal = f:binread('?iv??')
			local face = {normal}
			for i = 1, numverts do
				-- Read index and discard the 28 bytes following it
				local vidx = f:binread('H')
				f:read(28)
				table.insert(face, vidx+1)
			end
			table.insert(faces, face)
		end
	elseif chunktype == 4 then
		-- Ignore texture data
		f:read(chunksize)
	end
	chunktype = f:binread('i')
end
f:close()

DEBUG(4, ("%s: %d vertices, %d faces"):format(config.lvlfile, #verts, #faces))
return { vertices = verts, faces = faces }
