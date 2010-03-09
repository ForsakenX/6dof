-- vim:set sw=4 ts=4:
--
-- This Lua script reads vertex/face data from a Forsaken .mx/.mxa/.mxv
-- polymodel file.
--
-- Copyright (C) 2010  Pim Goossens
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

local function DEBUG(level, str)
	DEBUGX(DBG_LEVEL, level, str)
end

local textures = {}
local matlist = {}
local materials = {}
local verts = {}
local texcoords = {}
local triangles = {}
local vert_offset = 1

-- Functions for finding forsaken texture files which tend to differ in case

local function capitalize(str)
	return str:sub(1, 1):upper() .. str:sub(2):lower()
end

local function tryname(name)
	-- TODO: portable path separator?
	local searchdirs = {
		 config.lvltexdir
		,config.texdir
	}
	for _, dir in ipairs(searchdirs) do
		local f = io.open(dir .. '/' .. name, 'rb')
		if f then
			f:close()
			return dir
		end
	end
	return nil
end

local function findtex(name)
	local f
	local t = { name:lower(), name:upper(), capitalize(name) }
	for _, fname in ipairs(t) do
		local dir = tryname(fname)
		if dir then
			-- TODO: portable path separator?
			return dir .. '/' .. fname
		end
	end
	error("couldn't find texture " .. name)
end

-- Convert a 32-bit Forsaken color value to an (R,G,B,A) quadruple.
local function rgba(n)
	local b = bitand(n, 0xff) / 255
	n = shr(n, 8)
	local g = bitand(n, 0xff) / 255
	n = shr(n, 8)
	local r = bitand(n, 0xff) / 255
	n = shr(n, 8)
	local a = bitand(n, 0xff) / 255
	return r,g,b,a
end

local function getmat(color, specular)
	for c, x in pairs(matlist) do
		if c == color then
			for s, m in pairs(x) do
				if s == specular then
					return m
				end
			end
		end
	end
	return nil
end

local function addmat(color, specular, m)
	if not matlist[color] then
		matlist[color] = {}
	end
	matlist[color][specular] = m
end

local function material(color, specular)
	local r,g,b,a = rgba(color)
	local sr,sg,sb,sa = rgba(bitor(specular, shl(255, 24)))
	local m = getmat(color, specular)
	if m then
		return m
	else
		c = {r,g,b,a}
		s = {sr,sg,sb,sa}
		m = gfx.material({
			flags = 0;
			color = c;
			specular = s;
			alpha = 1;
		})
		addmat(color, specular, m)
	end
	return m
end

local function face(verts, normal, texture)
	local v0 = verts[1] + vert_offset
	local v1 = verts[2] + vert_offset
	local v2 = verts[3] + vert_offset
	return {
		{ index = v0, texcoords = texcoords[v0], material = materials[v0] };
		{ index = v1, texcoords = texcoords[v1], material = materials[v1] };
		{ index = v2, texcoords = texcoords[v2], material = materials[v2] };
		normal = normal;
		texture = texture;
	}
end

local f = binfile(io.open(config.lvlfile, 'rb'))
local magic, ver = f:binread('ii')
if magic ~= 0x584a5250 then
	error('not an MX file')
end
-- Read texture file names and prepare the textures for rendering
for i = 1, f:binread('h') do
	local texname = f:binread('z')
	table.insert(textures,
		gfx.texture(loadfile('level/bmp.lua')(findtex(texname)))
	)
end
-- For each group...
for i = 1, f:binread('h') do
	-- For each execlist...
	for i = 1, f:binread('h') do
		local verts_in_this_group = 0
		-- Read and forget exec_type
		f:binread('i')
		-- For each vertex...
		for i = 1, f:binread('h') do
			local v,blah,color,specular,tu,tv = f:binread('vIIIff')
			table.insert(verts, v)
			table.insert(materials, material(color, specular))
			table.insert(texcoords, {tu, tv})
			verts_in_this_group = verts_in_this_group + 1
		end
		-- For each texture group...
		for i = 1, f:binread('h') do
			local textype,sv,nv,tnum = f:binread('hhhh')
			-- For each triangle...
			for i = 1, f:binread('h') do
				local v0,v1,v2,pad16,normal = f:binread('hhhhv')
				table.insert(triangles, face({v0, v1, v2}, normal, textures[tnum+1]))
			end
		end
		vert_offset = vert_offset + verts_in_this_group
	end
end
f:close()

DEBUG(1, ("%s: %d vertices, %d triangles"):format(config.lvlfile, #verts, #triangles))
return { vertices = verts, faces = triangles }
