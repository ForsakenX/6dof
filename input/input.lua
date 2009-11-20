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

function input.bind(name, func)
	local id = input.id[name]
	if id ~= nil then
		if func ~= nil then
			DEBUG(5, 'Input "'..name..'" ('..id..') bound to function')
		else
			DEBUG(5, 'Input "'..name..'" ('..id..') unbound')
		end
		input.bindings[id] = func
	else
		warn('No such input "'..name..'"')
	end
end

function input.unbind(name)
	input.bind(name, nil)
end

-- FIXME: environment for bound functions...?
function input.load(name)
	DEBUG(1, 'Loading configuration from "'..name..'"')
	setfenv(loadfile('bindings.lua'), input)()
end
