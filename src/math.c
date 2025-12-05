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

#include "math.h"

INT32 _fltused = 0;

#define DIV_MOD_IMPL(size)                                                               \
	static UINT##size __udivmod##size(UINT##size a, UINT##size b, UINT##size *remainder) \
	{                                                                                    \
		UINT##size quotient = 0;                                                         \
		UINT##size bit = 1;                                                              \
                                                                                         \
		if (b == 0)                                                                      \
			while (1)                                                                    \
			{                                                                            \
			};                                                                           \
                                                                                         \
		while ((INT##size)b >= 0)                                                        \
		{                                                                                \
			b <<= 1;                                                                     \
			bit <<= 1;                                                                   \
		}                                                                                \
                                                                                         \
		while (bit)                                                                      \
		{                                                                                \
			if (b <= a)                                                                  \
			{                                                                            \
				a -= b;                                                                  \
				quotient += bit;                                                         \
			}                                                                            \
			b >>= 1;                                                                     \
			bit >>= 1;                                                                   \
		}                                                                                \
                                                                                         \
		*remainder = a;                                                                  \
		return quotient;                                                                 \
	}                                                                                    \
                                                                                         \
	UINT##size __udiv##size(UINT##size a, UINT##size b)                                  \
	{                                                                                    \
		UINT##size remainder;                                                            \
		return __udivmod##size(a, b, &remainder);                                        \
	}                                                                                    \
                                                                                         \
	UINT##size __umod##size(UINT##size a, UINT##size b)                                  \
	{                                                                                    \
		UINT##size remainder;                                                            \
		__udivmod##size(a, b, &remainder);                                               \
		return remainder;                                                                \
	}                                                                                    \
                                                                                         \
	INT##size __sdiv##size(INT##size a, INT##size b)                                     \
	{                                                                                    \
		BOOL negative = FALSE;                                                           \
		if (a < 0)                                                                       \
		{                                                                                \
			a = -a;                                                                      \
			negative = TRUE;                                                             \
		}                                                                                \
		if (b < 0)                                                                       \
		{                                                                                \
			b = -b;                                                                      \
			negative = !negative;                                                        \
		}                                                                                \
                                                                                         \
		INT##size result = __udiv##size(a, b);                                           \
		if (negative)                                                                    \
			result = -result;                                                            \
		return result;                                                                   \
	}                                                                                    \
                                                                                         \
	INT##size __smod##size(INT##size a, INT##size b)                                     \
	{                                                                                    \
		BOOL negative = FALSE;                                                           \
		if (a < 0)                                                                       \
		{                                                                                \
			a = -a;                                                                      \
			negative = TRUE;                                                             \
		}                                                                                \
		if (b < 0)                                                                       \
		{                                                                                \
			b = -b;                                                                      \
			negative = !negative;                                                        \
		}                                                                                \
                                                                                         \
		INT##size result = __umod##size(a, b);                                           \
		if (negative)                                                                    \
			result = -result;                                                            \
		return result;                                                                   \
	}

// UINT64 __shl64(UINT64 a, UINT8 count)
// {
// 	UINT32 *parts = (UINT32 *)&a;
// 	count &= 63;
// 	if (count >= 32)
// 	{
// 		parts[HIGH_PART] = parts[LOW_PART] << (count - 32);
// 		parts[LOW_PART] = 0;
// 	}
// 	else
// 	{
// 		parts[HIGH_PART] = (parts[HIGH_PART] << count) | (parts[LOW_PART] >> (32 - count));
// 		parts[LOW_PART] <<= count;
// 	}
// 	return a;
// }

// UINT64 __shr64(UINT64 a, UINT8 count)
// {
// 	UINT32 *parts = (UINT32 *)&a;
// 	count &= 63;
// 	if (count >= 32)
// 	{
// 		parts[LOW_PART] = parts[HIGH_PART] >> (count - 32);
// 		parts[HIGH_PART] = 0;
// 	}
// 	else
// 	{
// 		parts[LOW_PART] = (parts[LOW_PART] >> count) | (parts[HIGH_PART] << (32 - count));
// 		parts[HIGH_PART] >>= count;
// 	}
// 	return a;
// }

// INT64 __sar64(INT64 a, UINT8 count)
// {
// 	INT32 *parts = (INT32 *)&a;
// 	count &= 63;
// 	if (count >= 32)
// 	{
// 		parts[LOW_PART] = parts[HIGH_PART] >> (count - 32);
// 		parts[HIGH_PART] >>= 31;
// 	}
// 	else
// 	{
// 		parts[LOW_PART] = (((UINT32)parts[LOW_PART]) >> count) | (((UINT32)parts[HIGH_PART]) << (32 - count));
// 		parts[HIGH_PART] >>= count;
// 	}
// 	return a;
// }

// UINT16 __byteswap16(UINT16 a)
// {
// 	return (a >> 8) | (a << 8);
// }

// UINT32 __byteswap32(UINT32 a)
// {
// 	return (a >> 24) | ((a >> 8) & 0xff00) | ((a << 8) & 0xff0000) | (a << 24);
// }

// UINT64 __byteswap64(UINT64 a)
// {
// 	UINT32 *parts = (UINT32 *)&a;
// 	UINT32 s0 = __byteswap32(parts[1]);
// 	UINT32 s1 = __byteswap32(parts[0]);
// 	parts[0] = s0;
// 	parts[1] = s1;
// 	return a;
// }
// ARCHITECTURE_I386 specific implementations
UINT64 __udivdi3(UINT64 a, UINT64 b) { return __udiv64(a, b); }
UINT64 __umoddi3(UINT64 a, UINT64 b) { return __umod64(a, b); }
INT64 __divdi3(INT64 a, INT64 b) { return __sdiv64(a, b); }
INT64 __moddi3(INT64 a, INT64 b) { return __smod64(a, b); }

// ARCHITECTURE_ARM7A specific implementations
UINT64 __rt_udiv64(UINT64 a, UINT64 b) { return __udiv64(a, b); }
UINT64 __rt_umod64(UINT64 a, UINT64 b) { return __umod64(a, b); }
INT64 __rt_sdiv64(INT64 a, INT64 b) { return __sdiv64(a, b); }
INT64 __rt_smod64(INT64 a, INT64 b) { return __smod64(a, b); }

INT64 __dtoi64(DOUBLE x) { return (INT64)x; }
DOUBLE __i64tod(INT64 x) { return (DOUBLE)x; }

// UINT64 __aeabi_uldivmod(UINT64 a, UINT64 b, UINT64 *rem)
// {
// 	UINT64 quotient = __udiv64(a, b);
// 	*rem = __umod64(a, b);
// 	return quotient;
// }

// // INT64 __aeabi_d2lz(DOUBLE d) { return (INT64)d; }
// // DOUBLE __aeabi_l2d(INT64 l) { return (DOUBLE)l; }

DOUBLE __trunctfdf2(long double a)
{
	return (DOUBLE)a;
}

INT64 _allrem(INT64 a, INT64 b) { return __smod64(a, b); }

// DIV_MOD_IMPL(8)
// DIV_MOD_IMPL(16)
// DIV_MOD_IMPL(32)
DIV_MOD_IMPL(64)