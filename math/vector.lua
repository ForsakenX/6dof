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

local function vec_tostring(v)
	return ("(%g, %g, %g)"):format(v[1], v[2], v[3])
end

local function add(v1, v2)
	return vec(v1[1]+v2[1], v1[2]+v2[2], v1[3]+v2[3])
end

local function sub(v1, v2)
	return vec(v1[1]-v2[1], v1[2]-v2[2], v1[3]-v2[3])
end

local function mul(v1, v2)
	if type(v1) == 'number' then
		return vec(v1*v2[1], v1*v2[2], v1*v2[3])
	elseif type(v2) == 'number' then
		return vec(v2*v1[1], v2*v1[2], v2*v1[3])
	else
		return v1[1]*v2[1] + v1[2]*v2[2] + v1[3]*v2[3]
	end
end

local function unm(v)
	return vec(-v[1], -v[2], -v[3])
end

local function length(v)
	return math.sqrt(v[1]^2 + v[2]^2 + v[3]^2)
end

local function eq(v1, v2)
	return v1[1] == v2[1] and v1[2] == v2[2] and v1[3] == v2[3]
end

local function normalize(v)
	local len = v:len()
	return vec(v[1]/len, v[2]/len, v[3]/len)
end

local function cross_prod(v1, v2)
	return vec(
		 v1[2]*v2[3] - v1[3]*v2[2]
		,v1[3]*v2[1] - v1[1]*v2[3]
		,v1[1]*v2[2] - v1[2]*v2[1]
	)
end

local function index(v, key)
	return ({
		x = v[1];
		y = v[2];
		z = v[3];
		len = length;
		norm = normalize;
		cross = cross_prod;
	})[key]
end

local vec_mt = {
	__tostring = vec_tostring;
	__add = add;
	__sub = sub;
	__mul = mul;
	__unm = unm;
	__eq = eq;
	__index = index;
}

function vec(...)
	local args = {...}
	local v
	if type(args[1]) == 'table' and #(args[1]) == 3 then
		v = {args[1][1], args[1][2], args[1][3]}
	elseif type(args[1]) == 'number' and #args == 3 then
		v = {args[1], args[2], args[3]}
	else
		error('invalid argument list, expected table or 3 numbers')
	end
	setmetatable(v, vec_mt)
	return v
end
