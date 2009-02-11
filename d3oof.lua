-- vim:set sw=4 ts=4:
--
-- This Lua script reads vertex/face data from a Descent 3 .oof object
-- polymodel file.
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

local vert_offset = 1
local attach = { [-1]=vec(0,0,0) }

DEBUG(4, "Loading "..config.lvlfile)
local f = binfile(io.open(config.lvlfile, 'rb'))
local hdrID, ver = f:binread('ii')
if hdrID ~= 0x4f505350 then
	error('not an OOF file')
end
DEBUG(4, "OOF version: "..ver)
if ver ~= 2200 and ver ~= 2300 then
	error('invalid OOF version number (expected 2200 (0x898) or 2300 (0x8fc))')
end
local chunktype = f:read(4)
while chunktype ~= nil and chunktype:len() == 4 do
	local chunklen = f:binread('i')
	if chunktype == 'SOBJ' then
		local verts_in_this_group = 0
		local sobj_num = f:binread('i')
		DEBUG(6, 'Reading subobject '..sobj_num)
		local parent_idx = f:binread('i')
		DEBUG(6, 'Parent object index is '..parent_idx)
		f:read(28)
		-- Attachpoint
		local offset = f:binread('v')
		DEBUG(6, 'Attachpoint relative to parent is '..tostring(offset))
		offset = offset + attach[parent_idx]
		attach[sobj_num] = offset
		DEBUG(6, "This sobj's attachpoint is "..tostring(attach[sobj_num]))
		f:read(24)
		DEBUG(6, ('Subobject name is "%s"'):format(f:binread('t')))
		DEBUG(6, ('Subobject properties: "%s"'):format(f:binread('t')))
		f:read(12)
		local nverts = f:binread('i')
		for i = 1, nverts do
			local v = f:binread('v')
			table.insert(verts, v + offset)
		end
		-- Ignore vertex normals for now
		f:read(12*nverts)
		verts_in_this_group = verts_in_this_group + nverts
		if ver == 2300 then
			f:read(4*nverts)
		end
		-- Read faces
		for i = 1, f:binread('i') do
			-- Face normal
			local face = {f:binread('v')}
			local nverts = f:binread('i')
			-- Ignore texture indices
			local num_tidx = f:binread('i')
			f:read(4*num_tidx)
			-- Face vertices
			for i = 1, nverts do
				table.insert(face, f:binread('i')+vert_offset)
				-- Ignore texcoords for now
				f:read(8)
			end
			table.insert(faces, face)
			-- Some more data to ignore
			f:read(8)
		end
		vert_offset = vert_offset + verts_in_this_group
	else
		-- Unknown or unused chunk - ignore
		f:read(chunklen)
	end
	chunktype = f:read(4)
end
f:close()

DEBUG(4, ("%s: %d vertices, %d faces"):format(config.lvlfile, #verts, #faces))
return { vertices = verts, faces = faces }
