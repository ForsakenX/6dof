-- vim:set sw=4 ts=4:
--
-- This Lua script reads vertex/face data from a Forsaken .mx/.mxa/.mxv
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
local triangles = {}
local vert_offset = 1

local f = binfile(io.open(config.lvlfile, 'rb'))
local magic, ver = f:binread('ii')
if magic ~= 0x584a5250 then
	error('not an MX file')
end
-- Read and forget texture file names
for i = 1, f:binread('h') do f:binread('z') end
-- For each group...
for i = 1, f:binread('h') do
	local verts_in_this_group = 0
	-- For each execlist...
	for i = 1, f:binread('h') do
		-- Read and forget exec_type
		f:binread('i')
		-- For each vertex...
		for i = 1, f:binread('h') do
			local v,blah,color,specular,tu,tv = f:binread('vIIIII')
			table.insert(verts, v)
			verts_in_this_group = verts_in_this_group + 1
		end
		-- For each texture group...
		for i = 1, f:binread('h') do
			local textype,sv,nv,tnum = f:binread('hhhh')
			-- For each triangle...
			for i = 1, f:binread('h') do
				local v0,v1,v2,pad16,normal = f:binread('hhhhv')
				table.insert(triangles, {v0+vert_offset,v1+vert_offset,v2+vert_offset,normal})
			end
		end
	end
	vert_offset = vert_offset + verts_in_this_group
end
f:close()

DEBUG(4, ("%s: %d vertices, %d triangles"):format(config.lvlfile, #verts, #triangles))
return { vertices = verts, faces = triangles }
