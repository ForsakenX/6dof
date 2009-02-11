-- vim:set sw=4 ts=4:
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

function DEBUG(level, ...)
	if type(debug_level) == 'function' and debug_level() >= level then
		print(string.format(...))
	end
end

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
