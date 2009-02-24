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

-- Thanks to Methods for the setfenv suggestion
function load_config(name)
	DEBUG(3, "Loading configuration from "..name)
	local rawcfg = {}
	rawset(config, '_config', rawcfg)
	setfenv(loadfile(name), rawcfg)()
	debuglevel(rawcfg.debug)
end

function write_config(name)
	DEBUG(3, "Writing configuration to "..name)
	local f = file.open(name, 'wb')
	for key, value in pairs(config._config) do
		f:write(key.." = "..tostring(value).."\n")
	end
	f:close()
end

local function config_checkvalue(k, v)
	if k == 'debug' then
		v = tonumber(v)
		if v < -10 or v > 10 then return "value must be in range -10 to 10" end
	elseif k == 'dummyint' then
		v = tonumber(v)
		if v == nil then return "value must be a number" end
		if v < 0 or v > 4 then return "value must be in range 0 to 4" end
	elseif k == 'dummystr' then
		if v == '' then return "string may not be empty" end
	elseif k == 'width' then
		v = tonumber(v)
		if v < 1 then return "minimum screen width is 1" end
	elseif k == 'height' then
		v = tonumber(v)
		if v < 1 then return "minimum screen height is 1" end
	elseif k == 'bpp' then
		v = tonumber(v)
		if v ~= 16 and v ~= 24 and v ~= 32 then
			return "bpp must be 16, 24, or 32"
		end
	end
	return nil
end

setmetatable(config, {
	__index = function(t, key)
		if key == 'debug' then
			return debuglevel()
		end
		local value = rawget(t, '_config')[key]
		if value == nil then
			warn("unknown option name (`"..key.."') or nil value")
		else
			return value
		end
	end,
	__newindex = function(t, key, value)
		if rawget(t, '_config')[key] == nil then
			warn("tried to set unknown option `"..key.."' to `"..tostring(value).."'")
		else
			local err = config_checkvalue(key, value)
			if err == nil then
				if key == 'debug' then
					debuglevel(value)
				else
					DEBUG(2, "Setting option `"..key.."' to `"..tostring(value).."'")
					rawset(rawget(t, '_config'), key, value)
				end
			else
				warn("ignoring invalid value `"..tostring(value)..
					"' for option `"..key.."': "..err)
			end
		end
	end
})

load_config('defaults.lua')
