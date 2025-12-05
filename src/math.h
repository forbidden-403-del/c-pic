// Copyright (c) 2012 Rusty Wagner
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef __LIBC__MATH_H__
#define __LIBC__MATH_H__

#include "primitives.h"

#define DIV_MOD_DECL(size)                               \
    UINT##size __udiv##size(UINT##size a, UINT##size b); \
    UINT##size __umod##size(UINT##size a, UINT##size b); \
    INT##size __sdiv##size(INT##size a, INT##size b);    \
    INT##size __smod##size(INT##size a, INT##size b);

// DIV_MOD_DECL(8)
// DIV_MOD_DECL(16)
// DIV_MOD_DECL(32)
DIV_MOD_DECL(64)

UINT64 __shl64(UINT64 a, UINT8 count);
UINT64 __shr64(UINT64 a, UINT8 count);
INT64 __sar64(INT64 a, UINT8 count);
UINT16 __byteswap16(UINT16 a);
UINT32 __byteswap32(UINT32 a);
UINT64 __byteswap64(UINT64 a);

UINT64 __udivdi3(UINT64 a, UINT64 b);
UINT64 __umoddi3(UINT64 a, UINT64 b);
INT64 __divdi3(INT64 a, INT64 b);
INT64 __moddi3(INT64 a, INT64 b);

UINT64 __rt_udiv64(UINT64 a, UINT64 b);
UINT64 __rt_umod64(UINT64 a, UINT64 b);
INT64 __rt_sdiv64(INT64 a, INT64 b);
INT64 __rt_smod64(INT64 a, INT64 b);

UINT64 __aeabi_uldivmod(UINT64 a, UINT64 b, UINT64 *rem);
INT64 _allrem(INT64 a, INT64 b);

INT64 __dtoi64(DOUBLE x);
DOUBLE __i64tod(INT64 x);

#endif