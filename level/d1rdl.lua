-- vim:set sw=4 ts=4:
--
-- This Lua script reads vertex/face from a Descent 1 .rdl level file.
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

-- Convert an 8-bit integer (0 <= n <= 255) to a list of bits
local function bits(n)
	b = {}
	for i = 1, 8 do
		table.insert(b, test(n, i))
	end
	return b
end

-- Compute a face normal given three of its vertices
local function normal(v1,v2,v3)
	local n = (v2-v1):cross(v3-v2):norm()
	return n
end

local function face(v)
	return {
		{ index = v[1], texcoords = {0,0}, material = nil };
		{ index = v[2], texcoords = {0,1}, material = nil };
		{ index = v[3], texcoords = {1,0}, material = nil };
		{ index = v[4], texcoords = {1,1}, material = nil };
		normal = normal(verts[v[1]], verts[v[2]], verts[v[3]]);
		texture = nil;
	}
end

local f = binfile(io.open(config.lvlfile, 'rb'))
local hdrID, ver, offset = f:binread('iii????????')
if hdrID ~= 0x504c564c or ver ~= 1 then
	error('not an RDL file')
end
offset = offset + 1
DEBUG(4, 'Offset to mine geometry data is '..offset)
if offset < 20 then
	error('invalid mine geometry data offset')
end
-- Skip to mine geometry data
if offset > 20 then
	f:read(offset-20)
end
local nverts, ncubes = f:binread('hh')
DEBUG(4, ('Header says %d verts, %d cubes'):format(nverts, ncubes))
-- Read vertices
for i = 1, nverts do
	-- Coordinates in D1/D2's "fix" (fixed-point) format
	local vx, vy, vz = f:binread('iii')
	-- Convert to floating-point vector
	local v = vec(vx/0x10000, vy/0x10000, vz/0x10000)
	DEBUG(6, 'Adding vertex '..tostring(v))
	table.insert(verts, v)
end
-- Now for the hardest part... read the cubes section which will
-- provide us with the face (polygon) data
for i = 1, ncubes do
	DEBUG(6, '--- Start of next cube, file position = '..f:seek())
	local sidemask = bits(f:binread('B'))
	local n = 0
	for i, s in ipairs(sidemask) do
		if i <= 6 and s then
			n = n + 2
		end
	end
	-- Check the `attached cube ID' data for negative cube IDs
	DEBUG(6, ('Reading %d bytes of attached cube ID data'):format(n))
	local neighbors = {f:binread(('h'):rep(n/2))}
	for i, n in ipairs(neighbors) do
		if n < 0 then
			sidemask[i] = false
		end
	end
	-- Now read the face data, processing only those that are not
	-- connected to other cubes
	local doface = true
	local vLFT, vLFB, vRFB, vRFT, vLBT, vLBB, vRBB, vRBT = f:binread('HHHHHHHH')
	-- Check vertex indices for validity
	for i, v in ipairs({vLFT, vLFB, vRFB, vRFT, vLBT, vLBB, vRBB, vRBT}) do
		if v > nverts then
			error('invalid vertex index '..v)
		end
	end
	DEBUG(6, ('Cube vertex indices: %d, %d, %d, %d, %d, %d, %d, %d'):format(vLFT, vLFB, vRFB, vRFT, vLBT, vLBB, vRBB, vRBT))
	if not sidemask[3] then
		-- Left and right are swapped it seems... oh well.
		table.insert(faces, face({vLBT+1, vLFT+1, vLFB+1, vLBB+1}))
	end
	if not sidemask[2] then
		table.insert(faces, face({vLBT+1, vRBT+1, vRFT+1, vLFT+1}))
	end
	if not sidemask[1] then
		table.insert(faces, face({vRFT+1, vRBT+1, vRBB+1, vRFB+1}))
	end
	if not sidemask[4] then
		table.insert(faces, face({vRBB+1, vLBB+1, vLFB+1, vRFB+1}))
	end
	if not sidemask[5] then
		table.insert(faces, face({vRBT+1, vLBT+1, vLBB+1, vRBB+1}))
	end
	if not sidemask[6] then
		table.insert(faces, face({vLFT+1, vRFT+1, vRFB+1, vLFB+1}))
	end
	if sidemask[7] then
		-- Discard special data
		DEBUG(6, 'Discarding 4 bytes of special data')
		f:read(4)
	end
	-- Discard cube light value
	DEBUG(6, 'Discarding 2 bytes of cube light data')
	f:read(2)
	local wallmask = bits(f:binread('B'))
	for i = 1, 6 do
		if wallmask[i] then
			-- Wall ID, check for special value 255
			DEBUG(6, 'Reading 1 byte of wall ID data')
			local wallnum = f:binread('B')
			if wallnum == 255 then
				wallmask[i] = false
			end
		end
	end
	-- For each closed side of the cube, and those with a wall...
	for i = 1, 6 do
		if wallmask[i] or not sidemask[i] then
			-- Primary texture data
			local primtex = f:binread('h')
			DEBUG(6, 'Discarding 2 bytes of primary texture data')
			if primtex < 0 then
				-- Discard secondary texture info
				DEBUG(6, 'Discarding 2 bytes of secondary texture data')
				f:read(2)
			end
			-- Discard UVL info
			DEBUG(6, 'Discarding 24 bytes of UVL data')
			f:read(24)
		end
	end
end
f:close()

DEBUG(4, ("%s: %d vertices, %d faces"):format(config.lvlfile, #verts, #faces))
return { vertices = verts, faces = faces }
