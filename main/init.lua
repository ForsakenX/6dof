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

config = {}

function init(args)
	local function do_arg(str)
		local opt, value = string.match(str, '^%-%-(%w+)=(.+)$')
		if opt and value then
			config[opt] = value
			return
		end
		opt = string.match(str, '^%-%-no%-(%w+)$')
		if opt then
			config[opt] = false
			return
		end
		opt = string.match(str, '^%-%-(%w+)$')
		if opt then
			config[opt] = true
		else
			error("invalid argument '" .. str .. "'")
		end
	end
	program_name = args[1]
	table.remove(args, 1)
	dofile("main/util.lua")
	dofile("math/vector.lua")
	dofile("main/config.lua")
	for i, arg in ipairs(args) do
		do_arg(arg)
	end
end
