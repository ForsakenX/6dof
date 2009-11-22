/* vim:set sw=4 ts=4:
 *
 * Copyright (C) 2009  Pim Goossens
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>, or write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */

#ifdef STANDALONE

#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#define DEBUG(x...)
#define luafuncs_register luaopen_luafuncs

#else /* !STANDALONE */

#include "../include/common.h"

#endif /* !STANDALONE */

/* Integer / bit operations. */
static int shiftleft(lua_State *L)
{
	int n;
	lua_Integer a;
	int b;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "expected 2 arguments for shift left function (got %d)", n);
	a = luaL_checkinteger(L, 1);
	b = luaL_checkint(L, 2);
	lua_settop(L, 0);
	lua_pushinteger(L, a << b);

	return 1;
}

static int shiftright(lua_State *L)
{
	int n;
	lua_Integer a;
	int b;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "expected 2 arguments for shift right function (got %d)", n);
	a = luaL_checkinteger(L, 1);
	b = luaL_checkint(L, 2);
	lua_settop(L, 0);
	lua_pushinteger(L, a >> b);

	return 1;
}

static int testbit(lua_State *L)
{
	int n;
	lua_Integer a;
	int b;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "expected 2 arguments for shift right function (got %d)", n);
	a = luaL_checkinteger(L, 1);
	b = luaL_checkint(L, 2);
	lua_settop(L, 0);
	lua_pushboolean(L, a & (1 << (b-1)));

	return 1;
}

static int bitand(lua_State *L)
{
	int n;
	lua_Integer a;
	lua_Integer b;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "expected 2 arguments for bitwise AND function (got %d)", n);
	a = luaL_checkinteger(L, 1);
	b = luaL_checkinteger(L, 2);
	lua_settop(L, 0);
	lua_pushinteger(L, a & b);

	return 1;
}

static int bitor(lua_State *L)
{
	int n;
	lua_Integer a;
	lua_Integer b;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "expected 2 arguments for bitwise OR function (got %d)", n);
	a = luaL_checkinteger(L, 1);
	b = luaL_checkinteger(L, 2);
	lua_settop(L, 0);
	lua_pushinteger(L, a | b);

	return 1;
}

static int bitxor(lua_State *L)
{
	int n;
	lua_Integer a;
	lua_Integer b;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "expected 2 arguments for bitwise XOR function (got %d)", n);
	a = luaL_checkinteger(L, 1);
	b = luaL_checkinteger(L, 2);
	lua_settop(L, 0);
	lua_pushinteger(L, a ^ b);

	return 1;
}

static int bitnot(lua_State *L)
{
	int n;
	lua_Integer a;

	n = lua_gettop(L);
	if (n != 1)
		return luaL_error(L, "expected 1 argument for bitwise NOT function (got %d)", n);
	a = luaL_checkinteger(L, 1);
	lua_settop(L, 0);
	lua_pushinteger(L, ~a);

	return 1;
}

#if 0
/* -1 = unknown, -2 = invalid format string. */
static int calcsize(const char *fmt)
{
	int len;

	for (len=0; *fmt; fmt++)
	{
		switch (*fmt)
		{
			case '?':
			case 'b':
			case 'B':
			case 'c':
				len++; break;
			case 'h':
			case 'H':
				len += 2; break;
			case 'i':
			case 'I':
			case 'f':
				len += 4; break;
			case 'l':
			case 'L':
				len += 8; break;
			case 'v':
				len += 12; break;
			case 's':
			case 'S':
			case 't':
			case 'z':
				return -1;
			default:
				return -2;
		}
	}

	return len;
}
#endif

/* Utility macro for the unpack() function. */
#define CHECKLEN(n) \
	if (len < n) \
		return luaL_error(L, "%s:%d: string too short (%d left, needed %d)" \
			, __FILE__, __LINE__, len, n);

/* Takes a format argument and a string argument. Interprets data
 * in the string according to the format specification and returns
 * that data.
 *
 * Format characters:
 *   ? = 1 byte of data to be discarded
 *   b = signed byte value
 *   B = unsigned byte value
 *   c = 1-character string
 *   f = 4-byte floating-point value (float)
 *   h = signed 2-byte value (short)
 *   H = unsigned 2-byte value (short)
 *   i = signed 4-byte value (int)
 *   I = unsigned 4-byte value (int)
 *   l = signed 8-byte value (long long)
 *   L = unsigned 8-byte value (long long)
 *   s = string preceded by length (1 byte)
 *   S = string preceded by length (2 bytes)
 *   t = string preceded by length (4 bytes)
 *   v = vector (3 floats totaling 12 bytes)
 *   z = null-terminated string
 */
static int unpack(lua_State *L)
{
	int n;
	size_t len;
	const char *str;
	const char *fmt;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "expected 2 arguments for binread function (got %d)", n);
	fmt = luaL_checkstring(L, 1);
	str = luaL_checklstring(L, 2, &len);
	DEBUG(9, "unpack(\"%s\", ...)\n", fmt);
	lua_settop(L, 0);
	n = 0;
	for (; *fmt; fmt++)
	{
		switch (*fmt)
		{
			case '?':
				CHECKLEN(1);
				str = &str[1];
				n--;
				len--;
				break;
			case 'b':
				CHECKLEN(1);
				lua_pushinteger(L, *((signed char *) str));
				str = &str[1];
				len--;
				break;
			case 'B':
				CHECKLEN(1);
				lua_pushinteger(L, *((unsigned char *) str));
				str = &str[1];
				len--;
				break;
			case 'c':
				CHECKLEN(1);
				{
					char c[2] = { str[0], '\0' };
					lua_pushstring(L, c);
				}
				str = &str[1];
				len--;
				break;
			case 'f':
				CHECKLEN(4);
				lua_pushnumber(L, *((float *) str));
				str = &str[4];
				len -= 4;
				break;
			case 'h':
				CHECKLEN(2);
				lua_pushinteger(L, *((short *) str));
				str = &str[2];
				len -= 2;
				break;
			case 'H':
				CHECKLEN(2);
				lua_pushinteger(L, *((unsigned short *) str));
				str = &str[2];
				len -= 2;
				break;
			case 'i':
				CHECKLEN(4);
				lua_pushinteger(L, *((int *) str));
				str = &str[4];
				len -= 4;
				break;
			case 'I':
				CHECKLEN(4);
				lua_pushinteger(L, *((unsigned int *) str));
				str = &str[4];
				len -= 4;
				break;
			case 'l':
				CHECKLEN(8);
				lua_pushnumber(L, *((long long *) str));
				str = &str[8];
				len -= 8;
				break;
			case 'L':
				CHECKLEN(8);
				lua_pushnumber(L, *((unsigned long long *) str));
				str = &str[8];
				len -= 8;
				break;
			case 's':
				CHECKLEN(1);
				{
					unsigned char l = str[0];
					str = &str[1];
					len -= 1;
					CHECKLEN(l);
					lua_pushlstring(L, str, l);
					str = &str[l];
					len -= l;
				}
				break;
			case 'S':
				CHECKLEN(2);
				{
					unsigned short l = *((unsigned short *) str);
					str = &str[2];
					len -= 2;
					CHECKLEN(l);
					lua_pushlstring(L, str, l);
					str = &str[l];
					len -= l;
				}
				break;
			case 't':
				CHECKLEN(4);
				{
					unsigned int l = *((unsigned int *) str);
					str = &str[4];
					len -= 4;
					CHECKLEN(l);
					lua_pushlstring(L, str, l);
					str = &str[l];
					len -= l;
				}
				break;
			case 'v':
				CHECKLEN(12);
				lua_getglobal(L, "vec");
				lua_pushnumber(L, *((float *) str));
				str = &str[4];
				lua_pushnumber(L, *((float *) str));
				str = &str[4];
				lua_pushnumber(L, *((float *) str));
				str = &str[4];
				lua_call(L, 3, 1);
				len -= 12;
				break;
			case 'z':
				CHECKLEN(1);
				{
					int l = strlen(str);
					lua_pushstring(L, str);
					str = &str[l];
					len -= l;
				}
				break;
			default:
				luaL_error(L, "invalid character '%c' in format string", *fmt);
		}
		n++;
	}

	return n;
}
#undef CHECKLEN

#define GETCHAR ({ \
	int _c = getc(f); \
	if (_c < 0) \
		return luaL_error(L, "unexpected EOF"); \
	_c; \
})
#define READ(n) \
	DEBUG(7, "%s:%d: reading %d bytes\n", __FILE__, __LINE__, n); \
	if (fread(buf, 1, n, f) < n) \
		return luaL_error(L, "unexpected EOF"); \
	luaL_addlstring(&lbuf, buf, n);
static int binread(lua_State *L)
{
	int n;
	FILE *f;
	FILE **fp;
	const char *fmt;
	const char *fmt_save;
	char buf[256];
	luaL_Buffer lbuf;

	n = lua_gettop(L);
	if (n != 2)
		return luaL_error(L, "expected 2 arguments for binread function (got %d)", n);
	fp = luaL_checkudata(L, 1, LUA_FILEHANDLE);
	f = *fp;
	fmt = luaL_checkstring(L, 2);
	fmt_save = fmt;
	DEBUG(9, "binread(\"%s\", ...)\n", fmt);
	luaL_buffinit(L, &lbuf);
	for (; *fmt; fmt++)
	{
		switch (*fmt)
		{
			case '?':
			case 'b':
			case 'B':
			case 'c':
				luaL_addchar(&lbuf, GETCHAR);
				break;
			case 'h':
			case 'H':
				READ(2); break;
			case 'f':
			case 'i':
			case 'I':
				READ(4); break;
			case 'l':
			case 'L':
				READ(8); break;
			case 's':
				{
					int len = GETCHAR;
					luaL_addchar(&lbuf, len);
					READ(len);
				}
				break;
			case 'S':
				{
					unsigned short len;
					if (fread(&len, 2, 1, f) < 1)
						return luaL_error(L, "unexpected EOF");
					luaL_addlstring(&lbuf, (char *) &len, 2);
					READ(len);
				}
				break;
			case 't':
				{
					unsigned int len;
					if (fread(&len, 4, 1, f) < 1)
						return luaL_error(L, "unexpected EOF");
					luaL_addlstring(&lbuf, (char *) &len, 4);
					READ(len);
				}
				break;
			case 'v':
				READ(12); break;
			case 'z':
				{
					int c;
					while ((c = GETCHAR))
					{
						if (c < 0)
							return luaL_error(L, "unexpected EOF");
						luaL_addchar(&lbuf, c);
					}
				}
				break;
			default:
				return luaL_error(L, "invalid format string");
		}
	}
	lua_settop(L, 0);
	lua_pushcfunction(L, unpack);
	lua_pushstring(L, fmt_save);
	luaL_pushresult(&lbuf);
	lua_call(L, 2, LUA_MULTRET);

	return lua_gettop(L);
}
#undef GETCHAR
#undef READ

static const struct luaL_Reg funcs[] =
{
	 { "shl", shiftleft }
	,{ "shr", shiftright }
	,{ "test", testbit }
	,{ "bitand", bitand }
	,{ "bitor", bitor }
	,{ "bitxor", bitxor }
	,{ "bitnot", bitnot }
	,{ "unpack", unpack }
	,{ "binread", binread }
	,{ NULL, NULL }
};

int luafuncs_register(lua_State *L)
{
	const struct luaL_Reg *f;

	for (f=funcs; f->name; f++)
	{
		lua_pushcfunction(L, f->func);
		lua_setglobal(L, f->name);
	}

	return 0;
}
