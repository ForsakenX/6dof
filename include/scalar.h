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

#ifndef SCALAR_H
#define SCALAR_H

#include "common.h"

#if defined(SCALAR_FLOAT)
#define SCALAR_SUFFIXED(x) x ## f
#elif defined(SCALAR_DOUBLE)
#define SCALAR_SUFFIXED(x) x
#elif defined(SCALAR_LONG_DOUBLE)
#define SCALAR_SUFFIXED(x) x ## l
#endif

#define acoss SCALAR_SUFFIXED(acos)
#define acoshs SCALAR_SUFFIXED(acosh)
#define asinhs SCALAR_SUFFIXED(asinh)
#define asins SCALAR_SUFFIXED(asin)
#define atans SCALAR_SUFFIXED(atan)
#define atan2s SCALAR_SUFFIXED(atan2)
#define atanhs SCALAR_SUFFIXED(atanh)
#define cbrts SCALAR_SUFFIXED(cbrt)
#define ceils SCALAR_SUFFIXED(ceil)
#define copysigns SCALAR_SUFFIXED(copysign)
#define coss SCALAR_SUFFIXED(cos)
#define erfs SCALAR_SUFFIXED(erf)
#define erfcs SCALAR_SUFFIXED(erfc)
#define exps SCALAR_SUFFIXED(exp)
#define expm1s SCALAR_SUFFIXED(expm1)
#define fabss SCALAR_SUFFIXED(fabs)
#define fdims SCALAR_SUFFIXED(fdim)
#define floors SCALAR_SUFFIXED(floor)
#define fmas SCALAR_SUFFIXED(fma)
#define fmaxs SCALAR_SUFFIXED(fmax)
#define fmins SCALAR_SUFFIXED(fmin)
#define fmods SCALAR_SUFFIXED(fmod)
#define frexps SCALAR_SUFFIXED(frexp)
#define hypots SCALAR_SUFFIXED(hypot)
#define ilogbs SCALAR_SUFFIXED(ilogb)
#define ldexps SCALAR_SUFFIXED(ldexp)
#define lgammas SCALAR_SUFFIXED(lgamma)
#define llrints SCALAR_SUFFIXED(llrint)
#define llrounds SCALAR_SUFFIXED(llround)
#define logs SCALAR_SUFFIXED(log)
#define log10s SCALAR_SUFFIXED(log10)
#define log1ps SCALAR_SUFFIXED(log1p)
#define log2s SCALAR_SUFFIXED(log2)
#define logbs SCALAR_SUFFIXED(logb)
#define lrints SCALAR_SUFFIXED(lrint)
#define lrounds SCALAR_SUFFIXED(lround)
#define mods SCALAR_SUFFIXED(mod)
#define nans SCALAR_SUFFIXED(nan)
#define nearbyints SCALAR_SUFFIXED(nearbyint)
#define nextafters SCALAR_SUFFIXED(nextafter)
#define nexttowards SCALAR_SUFFIXED(nexttoward)
#define pows SCALAR_SUFFIXED(pow)
#define remainders SCALAR_SUFFIXED(remainder)
#define remquos SCALAR_SUFFIXED(remquo)
#define rints SCALAR_SUFFIXED(rint)
#define rounds SCALAR_SUFFIXED(round)
#define scalblns SCALAR_SUFFIXED(scalbln)
#define scalbns SCALAR_SUFFIXED(scalbn)
#define sins SCALAR_SUFFIXED(sin)
#define sqrts SCALAR_SUFFIXED(sqrt)
#define tans SCALAR_SUFFIXED(tan)
#define tanhs SCALAR_SUFFIXED(tanh)
#define tgammas SCALAR_SUFFIXED(tgamma)
#define truncs SCALAR_SUFFIXED(trunc)

#endif /* SCALAR_H */
