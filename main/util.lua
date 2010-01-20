-- vim:set sw=4 ts=4:
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

-- Keep these in sync with include/debug.h
debugchan = {
	audio = 0;
	general = 1;
	gfx = 2;
	input = 3;
	io = 4;
	level = 5;
	lua = 6;
	mem = 7;
}

DBG_AUDIO = debugchan.audio
DBG_GENERAL = debugchan.general
DBG_GFX = debugchan.gfx
DBG_INPUT = debugchan.input
DBG_IO = debugchan.io
DBG_LEVEL = debugchan.level
DBG_LUA = debugchan.lua
DBG_MEM = debugchan.mem

function warn(str)
	print(str)
end

-- This function adds a "binread" method to a filehandle, for
-- binary data reading and parsing.
function binfile(f)
	if type(f) ~= 'userdata' or tostring(f):sub(1,4) ~= 'file' then
		error('expected a file argument, got '..tostring(f))
	end
	local t = { fh = f }
	setmetatable(t, {
		__index = function(t, x)
			local func
			if x == 'binread' then
				func = binread
			else
				func = f[x]
			end
			return function(t, ...)
				return func(t.fh, ...)
			end
		end
	})
	return t
end

function screenshot()
	fname = 'scrn-' .. os.date('%Y%m%d-%H%M%S') .. '.ppm'
	DEBUG(4, 'Saving screenshot to ' .. fname)
	local image = gfx.readpixels(0, 0, config.width, config.height)
	local f = io.open(fname, 'w')
	f:write(("P6\n%d %d\n255\n"):format(image.width, image.height))
	f:write(image.data)
	f:close()
end
