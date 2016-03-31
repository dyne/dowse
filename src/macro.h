/*
 * kmod - interface to kernel module operations
 *
 * Copyright (C) 2011-2013  ProFUSION embedded systems
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <stddef.h>

#if defined(HAVE_STATIC_ASSERT)
#define assert_cc(expr) \
	_Static_assert((expr), #expr)
#else
#define assert_cc(expr) \
       do { (void) sizeof(char [1 - 2*!(expr)]); } while(0)
#endif

#define check_types_match(expr1, expr2)		\
	((typeof(expr1) *)0 != (typeof(expr2) *)0)

#define container_of(member_ptr, containing_type, member)		\
	((containing_type *)						\
	 ((char *)(member_ptr) - offsetof(containing_type, member))	\
	 - check_types_match(*(member_ptr), ((containing_type *)0)->member))


/* Two gcc extensions.
 * &a[0] degrades to a pointer: a different type from an array */
#define _array_size_chk(arr) ({ \
	assert_cc(!__builtin_types_compatible_p(typeof(arr), typeof(&(arr)[0]))); \
	0; \
	})

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + _array_size_chk(arr))
#define XSTRINGIFY(x) #x
#define STRINGIFY(x) XSTRINGIFY(x)

/* Temporaries for importing index handling */
#define NOFAIL(x) (x)
#define fatal(x...) do { } while (0)

/* Attributes */

#define _must_check_ __attribute__((warn_unused_result))
#define _printf_format_(a,b) __attribute__((format (printf, a, b)))
#define _unused_ __attribute__((unused))
#define _always_inline_ __inline__ __attribute__((always_inline))
#define _cleanup_(x) __attribute__((cleanup(x)))

/* Define C11 noreturn without <stdnoreturn.h> and even on older gcc
 * compiler versions */
#ifndef noreturn
#if defined(HAVE_NORETURN)
#define noreturn _Noreturn
#else
#define noreturn __attribute__((noreturn))
#endif
#endif

#define UNIQ __COUNTER__
