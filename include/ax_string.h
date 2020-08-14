/*

	ax_string - public domain
	Last update: 2015-09-30 Aaron Miller


	Contains various string routines.

	- Unicode encoding and methods.
	- File system path functions.
	- String formatting (e.g., axstrf( "Number: %i", 1 ))
	- More...


	USAGE
	=====

	Define AXSTR_IMPLEMENTATION in exactly one source file that includes this
	header, before including it.

	AXSTR_FUNC and AXSTR_CALL control the function declaration and calling
	conventions. Ensure that all source files including this use the same
	definitions for these. (That can be done in your Makefile or project
	settings.)

	AXSTR_STDSTR_ENABLED can be defined to 1 or 0 prior to including the header
	for implementation. If set to 1 then then the standard c string routines
	will be used internally. Otherwise, a naive implementation will be used.

	AXSTR_WINSTR_ENABLED works like AXSTR_STDSTR_ENABLED. If set to 1 it will
	use certain Windows SafeString functions in places where standard C does not
	provide the same functionality. This is ignored on non-Windows platforms.

	AXSTR_ABSPATH_ENABLED controls whether axstr_abspath() is declared/defined.
	Currently, axstr_abspath() relies on underlying OS functionality that might
	not be available on your target platform. It should work with Windows and
	POSIX-compatible operating systems (GNU/Linux, Mac OS X, *BSD), but has not
	been thoroughly tested yet.

	AXSTR_CXX_ENABLED controls whether any ax_string C++ feature will be used.
	If this is set to 0 (or __cplusplus is not defined) then all AXSTR_CXX_*
	macros will be redefined to 0 and no C++ feature will be used. See the
	following AXSTR_CXX_* macros for more information.

	AXSTR_CXX_OVERLOADS_ENABLED will enable or disable templated overloads of
	the string functions provided here, for automatic buffer size information.
	This allows, e.g., { char buf[20]; axstr_cpy( buf, "Hello!" ); } Whereas
	normally these functions require an explicit buffer size, this information
	can be inferred in C++.

	AXSTR_CXX_CLASSES_ENABLED, if set to 1, will define the ax::MutStr class,
	based on the functionality here. All functions in the class are inlined, but
	the functions they reference (such as axstr_to_uint) aren't.


	REPLACE STRING ALLOCATORS
	=========================

	You can specify your own allocator to use with this library by defining the
	axstr_alloc and axstr_free macros. By default they are defined to the
	standard C library's malloc() and free().


	INTERACTIONS
	============

	ax_types
	--------
	Uses the basic types defined here if AXSTR_TYPES_DEFINED has not been
	defined yet.

	ax_platform
	-----------
	Calls ax_static_assert to verify compile-time assumptions, such as the size
	of axstr_utf8_t, but only if ax_static_assert is defined.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

/*! \file  ax_string.h
 *  \brief String library.
 *
 *  This exposes various secure string manipulation functions, utility
 *  functions (ranging from very special purpose to extremely common), as well
 *  as the classes `ax::Str` and `ax::MutStr`. The former being used for
 *  efficiently passing strings (which do not need to be `NUL`-terminated), with
 *  the latter being useful for storing and manipulating strings.
 */

#ifndef INCGUARD_AX_STRING_H_
#define INCGUARD_AX_STRING_H_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

#define AXSTR_VERSION_MAJOR 0
#define AXSTR_VERSION_MINOR 1
#define AXSTR_VERSION_PATCH 0

#define AXSTR_VERSION \
	( AXSTR_VERSION_MAJOR*10000 + \
	  AXSTR_VERSION_MINOR*100   + \
	  AXSTR_VERSION_PATCH )

#if !defined( AX_NO_INCLUDES ) && defined( __has_include )
# if __has_include( "ax_platform.h" )
#  include "ax_platform.h"
# endif
# if __has_include( "ax_types.h" )
#  include "ax_types.h"
# endif
#endif

#ifndef AXSTR_OUT_Z
# ifdef AX_OUT_Z
#  define AXSTR_OUT_Z(MaxChars_) AX_OUT_Z(MaxChars_)
# else
#  define AXSTR_OUT_Z(MaxChars_)
# endif
#endif
#ifndef AXSTR_IN_Z
# ifdef AX_IN_Z
#  define AXSTR_IN_Z AX_IN_Z
# else
#  define AXSTR_IN_Z
# endif
#endif
#ifndef AXSTR_INOUT_Z
# ifdef AX_INOUT_Z
#  define AXSTR_INOUT_Z(MaxChars_) AX_INOUT_Z(MaxChars_)
# else
#  define AXSTR_INOUT_Z(MaxChars_)
# endif
#endif

#ifndef AXSTR_COUNTOF
# ifdef AX_COUNTOF
#  define AXSTR_COUNTOF(Arr1D_) AX_COUNTOF(Arr1D_)
# else
#  define AXSTR_COUNTOF(Arr1D_) (sizeof((Arr1D_))/sizeof((Arr1D_)[0]))
# endif
#endif

#ifndef AXSTR_TYPES_DEFINED
# define AXSTR_TYPES_DEFINED        1
# ifdef AX_TYPES_DEFINED
typedef ax_uptr_t                   axstr_size_t;
typedef ax_sptr_t                   axstr_ptrdiff_t;
typedef ax_u8_t                     axstr_utf8_t;
typedef ax_u16_t                    axstr_utf16_t;
typedef ax_u32_t                    axstr_utf32_t;
typedef ax_bool_t                   axstr_bool_t;
typedef ax_s64_t                    axstr_sint_t;
typedef ax_u64_t                    axstr_uint_t;
typedef double                      axstr_real_t;
# else
#  include <stddef.h>
typedef size_t                      axstr_size_t;
typedef ptrdiff_t                   axstr_ptrdiff_t;
typedef unsigned char               axstr_utf8_t;
typedef unsigned short              axstr_utf16_t;
typedef unsigned int                axstr_utf32_t;
typedef unsigned char               axstr_bool_t;
typedef signed int                  axstr_sint_t;
typedef unsigned int                axstr_uint_t;
typedef double                      axstr_real_t;
# endif
#endif

#ifdef ax_static_assert
ax_static_assert( sizeof( axstr_utf8_t ) == 1, "ax_string: axstr_utf8_t has invalid size" );
ax_static_assert( sizeof( axstr_utf16_t ) == 2, "ax_string: axstr_utf16_t has invalid size" );
ax_static_assert( sizeof( axstr_utf32_t ) == 4, "ax_string: axstr_utf32_t has invalid size" );
#endif

#define AXSTR_UNKNOWN_LENGTH        ( ~( axstr_size_t )0 )

#ifndef AXSTR_STDSTR_ENABLED
# define AXSTR_STDSTR_ENABLED       1
#endif

#ifndef AXSTR_WINSTR_ENABLED
# ifndef __GNUC__
#  define AXSTR_WINSTR_ENABLED      1
# else
#  define AXSTR_WINSTR_ENABLED      0
# endif
#endif

#ifndef _WIN32
# undef AXSTR_WINSTR_ENABLED
# define AXSTR_WINSTR_ENABLED       0
#endif

#ifndef AXSTR_ABSPATH_ENABLED
# if defined( _WIN32 ) || defined( __APPLE__ )
#  define AXSTR_ABSPATH_ENABLED     1
# elif defined( linux ) || defined( __linux ) || defined( __linux__ )
#  define AXSTR_ABSPATH_ENABLED     1
# else
#  define AXSTR_ABSPATH_ENABLED     0
# endif
#endif

#ifndef AXSTR_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXSTR_CXX_ENABLED         AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXSTR_CXX_ENABLED         1
# else
#  define AXSTR_CXX_ENABLED         0
# endif
#endif

#ifndef AXSTR_CXX_OVERLOADS_ENABLED
# define AXSTR_CXX_OVERLOADS_ENABLED 1
#endif
#ifndef AXSTR_CXX_CLASSES_ENABLED
# define AXSTR_CXX_CLASSES_ENABLED  1
#endif

/* function declaration specifier */
#ifndef AXSTR_FUNC
# ifdef AX_FUNC
#  define AXSTR_FUNC                AX_FUNC
# else
#  define AXSTR_FUNC                extern
# endif
#endif

/* function calling convention */
#ifndef AXSTR_CALL
# ifdef AX_CALL
#  define AXSTR_CALL                AX_CALL
# else
#  define AXSTR_CALL
# endif
#endif
#ifndef AXSTR_CALLF
# ifdef AX_CALLF
#  define AXSTR_CALLF               AX_CALLF
# elif defined( _WIN32 )
#  define AXSTR_CALLF               __cdecl
# else
#  define AXSTR_CALLF
# endif
#endif

#ifdef AXSTR_IMPLEMENTATION
# define AXSTR_IMPLEMENT            1
#else
# define AXSTR_IMPLEMENT            0
#endif

#if AXSTR_ABSPATH_ENABLED
# if AXSTR_IMPLEMENT
#  ifdef _WIN32
#   undef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN      1
#   if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0600
#    undef _WIN32_WINNT
#    define _WIN32_WINNT            0x0603
#   endif
#   include <Windows.h>
#  else
	/* TODO: Where was "realpath" declared? */
#  endif
# endif
#endif
#if AXSTR_STDSTR_ENABLED && AXSTR_IMPLEMENT
# include <stdio.h>
#endif

#ifndef AXSTR_DIRSEP
# if defined( _WIN32 )
#  define AXSTR_DIRSEP              "\\"
#  define AXSTR_DIRSEP_CH           '\\'
# else
#  define AXSTR_DIRSEP              "/"
#  define AXSTR_DIRSEP_CH           '/'
# endif
#endif

#ifndef AXSTR_DEFAULT_RADIX
# define AXSTR_DEFAULT_RADIX        10
#endif
#ifndef AXSTR_DEFPARSE_RADIX
# define AXSTR_DEFPARSE_RADIX       axstr_radix_c
#endif
#ifndef AXSTR_DIGITSEP_CH
# define AXSTR_DIGITSEP_CH          '\''
#endif
#ifndef AXSTR_DIGITSEP_SPACING
# define AXSTR_DIGITSEP_SPACING     3
#endif
#ifndef AXSTR_FLOAT_TRAIL_DIGITS
# define AXSTR_FLOAT_TRAIL_DIGITS   6
#endif

#ifndef AXSTR_ASSERT
# if defined( AX_ASSERT )
#  define AXSTR_ASSERT(Expr_)       AX_ASSERT(Expr_)
# elif defined( assert )
#  define AXSTR_ASSERT(Expr_)       assert(Expr_)
# else
#  define AXSTR_ASSERT(Expr_)       ((void)0)
# endif
#endif

#include <stdarg.h>                 /* needed for va_list */

#ifndef AXSTR_MAX_PATH
# if defined( _WIN32 )
#  define AXSTR_MAX_PATH            32768
# elif defined( linux ) || defined(__linux__)
#  define AXSTR_MAX_PATH            4096
# elif defined( __APPLE__ )
#  define AXSTR_MAX_PATH            1016
# elif defined( PATH_MAX )
#  define AXSTR_MAX_PATH            PATH_MAX
# elif defined( _PATH_MAX )
#  define AXSTR_MAX_PATH            _PATH_MAX
# elif defined( MAX_PATH )
#  define AXSTR_MAX_PATH            MAX_PATH
# elif defined( _MAX_PATH )
#  define AXSTR_MAX_PATH            _MAX_PATH
# else
#  define AXSTR_MAX_PATH            250
# endif
#endif

#ifndef AXSTR_MAX_NAME
# if defined( NAME_MAX )
#  define AXSTR_MAX_NAME            NAME_MAX
# elif defined( _WIN32 )
#  define AXSTR_MAX_NAME            248
# elif defined( __APPLE__ )
#  define AXSTR_MAX_NAME            250
# elif defined( linux ) || defined( __linux__ )
#  define AXSTR_MAX_NAME            255
# else
#  define AXSTR_MAX_NAME            128
# endif
#endif

#ifndef axstr_alloc
# if AXSTR_IMPLEMENT
#  include <string.h>
#  include <stdlib.h>
# endif
# define axstr_alloc                malloc
# define axstr_free                 free
#endif

#undef min
#undef max

/* cannot use c++ features in c */
#ifndef __cplusplus
# undef AXSTR_CXX_ENABLED
# define AXSTR_CXX_ENABLED          0
#endif
/* if c++ features in general aren't enabled then disable all individual features */
#if !AXSTR_CXX_ENABLED
# undef  AXSTR_CXX_OVERLOADS_ENABLED
# undef  AXSTR_CXX_CLASSES_ENABLED
# define AXSTR_CXX_OVERLOADS_ENABLED 0
# define AXSTR_CXX_CLASSES_ENABLED   0
#endif

/*!
 *  Passed to number conversion functions to determine the radix to use.
 *  e.g., base-10 (decimal), base-16 (hexadecimal), etc.
 *
 *  The special members axstr_radix_c and axstr_radix_basic allow for special
 *  processing of numbers based on a prefix. (e.g., 0x for hexadecimal.) Neither
 *  of these modes are case sensitive.
 */
typedef enum
{
	/*! `0b<n>` = bin, `0<n>` = oct, `0x<n>` = hex; else dec */
	axstr_radix_c                   = -1,
	/*! `%<n>`, `0b<n>` = bin; `0c<n>` = oct; `0x<n>` = hex; `<r>x<n>` = <r>; else dec */
	axstr_radix_basic               = -2,

	/*! base-2 (e.g., 11010101011110) */
	axstr_radix_binary              = 2,
	/*! base-8 (e.g., 777) */
	axstr_radix_octal               = 8,
	/*! base-10 (e.g., 90210) */
	axstr_radix_decimal             = 10,
	/*! base-16 (e.g., FACEBEEF) */
	axstr_radix_hexadecimal         = 16
} axstr_radix_t;

/*!
 * \brief Encoding of the string.
 *
 * Strings are internally encoded with UTF-8, always. There are routines to
 * convert to an alternate Unicode encoding, however.
 */
typedef enum
{
	/*! Unknown encoding */
	axstr_enc_unknown,

	/*! UTF-8 encoding */
	axstr_enc_utf8,
	/*! UTF-16 little endian encoding */
	axstr_enc_utf16_le,
	/*! UTF-16 big endian encoding */
	axstr_enc_utf16_be,
	/*! UTF-32 little endian encoding */
	axstr_enc_utf32_le,
	/*! UTF-32 big endian encoding */
	axstr_enc_utf32_be
} axstr_encoding_t;

/*!
 * \brief Byte-Order-Mark mode for Unicode data.
 *
 * Indicates whether a byte-order-mark (BOM) should be (or is) present.
 */
typedef enum
{
	/*! The byte-order-mark is disabled. It won't be written. */
	axstr_bom_disabled,
	/*! The byte-order-mark is enabled. It will be written. */
	axstr_bom_enabled
} axstr_byteordermark_t;

/*!
 * \brief Whether to keep quotes in tokens from quoted string split operations.
 */
typedef enum
{
	/*! If the string `"a"` is encountered, then the enclosing quotes are
	 *  removed and `a` is returned. */
	axstr_strip_quotes,
	/*! If the string `"a"` is encountered, then those three bytes, `"a"`, are
	 *  returned as opposed to just the byte `a`. */
	axstr_keep_quotes
} axstr_keepquotes_t;

#if AXSTR_IMPLEMENT
# if AXSTR_WINSTR_ENABLED
#  include <strsafe.h>
# endif
# if AXSTR_STDSTR_ENABLED
#  include <string.h>
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*! \internal
 *  \brief Perform a memcpy operation
 */
AXSTR_FUNC void AXSTR_CALL axstr__memcpy( void *pDst, const void *pSrc, axstr_size_t cBytes )
#if AXSTR_IMPLEMENT
{
# if AXSTR_STDSTR_ENABLED
	memcpy( pDst, pSrc, cBytes );
# else
	union {
		unsigned char *p8;
		axstr_size_t * psz;
		axstr_size_t   n;
		void *         p;
	} dst;
	union {
		const unsigned char *p8;
		const axstr_size_t * psz;
		axstr_size_t         n;
		const void *         p;
	} src;

	/* FIXME: If the operation is sufficiently large, use SIMD loads/stores */

	/* prepare the copy operation */
	dst.p = pDst;
	src.p = pSrc;

	/* align to `axstr_size_t` if possible */
	if( dst.n%sizeof(axstr_size_t) == src.n%sizeof(axstr_size_t) && cBytes>=sizeof(axstr_size_t) ) {
		cBytes -= dst.n%sizeof(axstr_size_t);
		while( dst.n%sizeof(axstr_size_t) != 0 ) {
			*dst.p8++ = *src.p8++;
		}
	}

	/* copy in `axstr_size_t`-sized chunks (should be 4 or 8 bytes at a time) */
	while( cBytes >= sizeof(axstr_size_t) ) {
		*dst.psz++ = *src.psz++;
		cBytes -= sizeof(axstr_size_t);
	}

	/* copy the remaining bytes */
	while( cBytes > 0 ) {
		*dst.p8++ = *src.p8++;
		--cBytes;
	}
# endif
}
#else
;
#endif
/*! \internal
 *  \brief Perform a memmove operation
 */
AXSTR_FUNC void AXSTR_CALL axstr__memmove( void *dst, axstr_size_t cMaxDst, const void *src, axstr_size_t cBytes )
#if AXSTR_IMPLEMENT
{
	if( cBytes > cMaxDst ) {
		cBytes = cMaxDst;
	}

# if AXSTR_STDSTR_ENABLED
#  if defined( _MSC_VER ) && defined( __STDC_WANT_SECURE_LIB__ )
	memmove_s( dst, cMaxDst, src, cBytes );
#  else
	memmove( dst, src, cBytes );
#  endif
# else
	do {
		unsigned char *d, *de;
		const unsigned char *s, *se;

		d = ( unsigned char * )dst;
		s = ( const unsigned char * )src;

		de = d + cBytes;
		se = s + cBytes;

		/* check for no overlap (can do simple memcpy) */
		if( !( ( s >= d && s <= de ) || ( se >= d && se <= de ) ) ) {
			axstr__memcpy( dst, src, cBytes );
			return;
		}

		/* copying downward? */
		if( d < s ) {
			while( cBytes-- > 0 ) {
				*d++ = *s++;
			}
		} else {
			while( cBytes > 0 ) {
				d[ cBytes - 1 ] = s[ cBytes - 1 ];
				--cBytes;
			}
		}
	} while(0);
# endif
}
#else
;
#endif

/*!
 * \brief  Given a buffer (of at least four bytes in size), check for a byte
 *         order mark and determine the encoding of the string from that.
 *
 * \param  pBuf       A buffer containing the BOM (e.g., start of file) that is
 *                    *at least* four bytes in size.
 * \param  pOutLength Optional pointer that receives the length of the BOM.
 * \return Encoding of the buffer or `axstr_enc_unknown` if not detected.
 */
AXSTR_FUNC axstr_encoding_t AXSTR_CALL axstr_detect_encoding( const char *pBuf, axstr_size_t *pOutLength )
#if AXSTR_IMPLEMENT
{
	const unsigned char *p;

	p = ( const unsigned char * )pBuf;

	if( p[0]==0x00 && p[1]==0x00 && p[2]==0xFE && p[3]==0xFF ) {
		if( pOutLength != ( axstr_size_t * )0 ) {
			*pOutLength = 4;
		}

		return axstr_enc_utf32_be;
	}

	if( p[0]==0xEF && p[1]==0xBB && p[2]==0xBF ) {
		if( pOutLength != ( axstr_size_t * )0 ) {
			*pOutLength = 3;
		}

		return axstr_enc_utf8;
	}

	if( p[0]==0xFE && p[1]==0xFF ) {
		if( pOutLength != ( axstr_size_t * )0 ) {
			*pOutLength = 2;
		}

		return axstr_enc_utf16_be;
	}

	if( p[0]==0xFF && p[1]==0xFE ) {
		if( p[2]==0x00 && p[3]==0x00 ) {
			if( pOutLength != ( axstr_size_t * )0 ) {
				*pOutLength = 4;
			}

			return axstr_enc_utf32_le;
		}

		if( pOutLength != ( axstr_size_t * )0 ) {
			*pOutLength = 2;
		}

		return axstr_enc_utf16_le;
	}

	if( pOutLength != ( axstr_size_t * )0 ) {
		*pOutLength = 0;
	}

	return axstr_enc_unknown;
}
#else
;
#endif

/*!
 * \brief  Compare two strings for equality, ignoring casing.
 *
 * This comparison cannot be used for sorting.
 *
 * \param  pszFirst  First `NUL`-terminated string to compare.
 * \param  pszSecond Second `NUL`-terminated string to compare.
 * \param  cBytes    Maximum number of bytes from either string to test.
 * \return nonzero if the strings match, or `0` if not.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_casecmpn( const char *pszFirst, const char *pszSecond, axstr_size_t cBytes )
#if AXSTR_IMPLEMENT
{
# if AXSTR_STDSTR_ENABLED
	if( cBytes == AXSTR_UNKNOWN_LENGTH ) {
#  ifdef _WIN32
		return ( axstr_bool_t )( _stricmp( pszFirst, pszSecond ) == 0 );
#  else
		return ( axstr_bool_t )( strcasecmp( pszFirst, pszSecond ) == 0 );
#  endif
	}

#  ifdef _WIN32
	return ( axstr_bool_t )( _strnicmp( pszFirst, pszSecond, cBytes ) == 0 );
#  else
	return ( axstr_bool_t )( strncasecmp( pszFirst, pszSecond, cBytes ) == 0 );
#  endif
# else
	axstr_size_t i;

	i = 0;

	while( i < cBytes ) {
		if( pszFirst[ i ] != pszSecond[ i ] ) {
			char c, d;

			c = pszFirst [i]>='A' && pszFirst [i]<='Z' ? pszFirst [i] - 'A' + 'a' : pszFirst [i];
			d = pszSecond[i]>='A' && pszSecond[i]<='Z' ? pszSecond[i] - 'A' + 'a' : pszSecond[i];

			if( c != d ) {
				return 0;
			}
		}

		if( pszFirst[i]=='\0' ) {
			break;
		}
		++i;
	}

	return 1;
# endif
}
#else
;
#endif
/*!
 * \brief  Compare two strings for equality, ignoring casing.
 *
 * This comparison cannot be used for sorting.
 *
 * \param  pszFirst  First `NUL`-terminated string to compare.
 * \param  pszSecond Second `NUL`-terminated string to compare.
 * \return nonzero if the strings match, or `0` if not.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_casecmp( const char *pszFirst, const char *pszSecond )
#if AXSTR_IMPLEMENT
{
	return axstr_casecmpn( pszFirst, pszSecond, AXSTR_UNKNOWN_LENGTH );
}
#else
;
#endif

/*!
 * \brief  Compare two strings for equality, case-sensitive.
 *
 * This comparison cannot be used for sorting.
 *
 * \param  pszFirst  First `NUL`-terminated string to compare.
 * \param  pszSecond Second `NUL`-terminated string to compare.
 * \param  cBytes    Maximum number of bytes from either string to test.
 * \return nonzero if the strings match, or `0` if not.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_cmpn( const char *pszFirst, const char *pszSecond, axstr_size_t cBytes )
#if AXSTR_IMPLEMENT
{
# if AXSTR_STDSTR_ENABLED
	if( cBytes == AXSTR_UNKNOWN_LENGTH ) {
		return ( axstr_bool_t )( strcmp( pszFirst, pszSecond ) == 0 );
	}

	return ( axstr_bool_t )( strncmp( pszFirst, pszSecond, cBytes ) == 0 );
# else
	axstr_size_t i;

	i = 0;

	while( i < cBytes ) {
		if( pszFirst[ i ] != pszSecond[ i ] ) {
			return 0;
		}

		if( pszFirst[ i ] == '\0' ) {
			break;
		}
		++i;
	}

	return 1;
# endif
}
#else
;
#endif
/*!
 * \brief  Compare two strings for equality, case-sensitive.
 *
 * This comparison cannot be used for sorting.
 *
 * \param  pszFirst  First `NUL`-terminated string to compare.
 * \param  pszSecond Second `NUL`-terminated string to compare.
 * \return nonzero if the strings match, or `0` if not.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_cmp( const char *pszFirst, const char *pszSecond )
#if AXSTR_IMPLEMENT
{
	return axstr_cmpn( pszFirst, pszSecond, AXSTR_UNKNOWN_LENGTH );
}
#else
;
#endif

/*!
 * \brief  Compare two strings for sorting purposes, ignoring casing.
 *
 * \param  pszFirst  First `NUL`-terminated string to compare.
 * \param  pszSecond Second `NUL`-terminated string to compare.
 * \param  cBytes    Maximum number of bytes from either string to test.
 * \return Negative if `pszFirst` should come before `pszSecond`, zero if both
 *         are equivalent, or positive if `pszFirst` should go after `pszSecond`
 *         in a sorted list.
 */
AXSTR_FUNC int AXSTR_CALL axstr_sortcasecmpn( const char *pszFirst, const char *pszSecond, axstr_size_t cBytes )
#if AXSTR_IMPLEMENT
{
# if AXSTR_STDSTR_ENABLED
	if( cBytes == AXSTR_UNKNOWN_LENGTH ) {
#  ifdef _WIN32
		return _stricmp( pszFirst, pszSecond );
#  else
		return strcasecmp( pszFirst, pszSecond );
#  endif
	}

#  ifdef _WIN32
	return _strnicmp( pszFirst, pszSecond, cBytes );
#  else
	return strncasecmp( pszFirst, pszSecond, cBytes );
#  endif
# else
	axstr_size_t i;
	char a, b;
	int d;

	i = 0;

	while( i < cBytes ) {
		a = pszFirst [i]>='A' && pszFirst [i]<='Z' ? pszFirst [i] - 'A' + 'a' : pszFirst [i];
		b = pszSecond[i]>='A' && pszSecond[i]<='Z' ? pszSecond[i] - 'A' + 'a' : pszSecond[i];

		d = +( a - b );
		if( d != 0 ) {
			return d;
		}

		if( pszFirst[i]=='\0' ) {
			break;
		}
		++i;
	}

	return 0;
# endif
}
#else
;
#endif
/*!
 * \brief  Compare two strings for sorting purposes, ignoring casing.
 *
 * \param  pszFirst  First `NUL`-terminated string to compare.
 * \param  pszSecond Second `NUL`-terminated string to compare.
 * \return Negative if `pszFirst` should come before `pszSecond`, zero if both
 *         are equivalent, or positive if `pszFirst` should go after `pszSecond`
 *         in a sorted list.
 */
AXSTR_FUNC int AXSTR_CALL axstr_sortcasecmp( const char *pszFirst, const char *pszSecond )
#if AXSTR_IMPLEMENT
{
	return axstr_sortcasecmpn( pszFirst, pszSecond, AXSTR_UNKNOWN_LENGTH );
}
#else
;
#endif

/*!
 * \brief  Compare two strings for sorting purposes, case-sensitive.
 *
 * \param  pszFirst  First `NUL`-terminated string to compare.
 * \param  pszSecond Second `NUL`-terminated string to compare.
 * \param  cBytes    Maximum number of bytes from either string to test.
 * \return Negative if `pszFirst` should come before `pszSecond`, zero if both
 *         are equivalent, or positive if `pszFirst` should go after `pszSecond`
 *         in a sorted list.
 */
AXSTR_FUNC int AXSTR_CALL axstr_sortcmpn( const char *pszFirst, const char *pszSecond, axstr_size_t cBytes )
#if AXSTR_IMPLEMENT
{
# if AXSTR_STDSTR_ENABLED
	if( cBytes == AXSTR_UNKNOWN_LENGTH ) {
		return strcmp( pszFirst, pszSecond );
	}

	return strncmp( pszFirst, pszSecond, cBytes );
# else
	axstr_size_t i;
	int d;

	i = 0;

	while( i < cBytes ) {
		d = +( pszFirst[ i ] - pszSecond[ i ] );
		if( d != 0 ) {
			return d;
		}

		if( pszFirst[ i ] == '\0' ) {
			break;
		}
		++i;
	}

	return 0;
# endif
}
#else
;
#endif
/*!
 * \brief  Compare two strings for sorting purposes, case-sensitive.
 *
 * \param  pszFirst  First `NUL`-terminated string to compare.
 * \param  pszSecond Second `NUL`-terminated string to compare.
 * \return Negative if `pszFirst` should come before `pszSecond`, zero if both
 *         are equivalent, or positive if `pszFirst` should go after `pszSecond`
 *         in a sorted list.
 */
AXSTR_FUNC int AXSTR_CALL axstr_sortcmp( const char *pszFirst, const char *pszSecond )
#if AXSTR_IMPLEMENT
{
	return axstr_sortcmpn( pszFirst, pszSecond, AXSTR_UNKNOWN_LENGTH );
}
#else
;
#endif

/*!
 * \brief  Determine whether two bytes are equal as far as file system paths are
 *         concerned.
 *
 * On Windows, this treats `\` the same as `/` and `A-Z` the same as `a-z`. On
 * other platforms, this is a straight byte equality comparison.
 *
 * \param  a First byte to compare.
 * \param  b Second byte to compare.
 * \return `true` if the bytes are equal as far as paths are concerned. `false`
 *         otherwise.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_cmppathchar( char a, char b )
#if AXSTR_IMPLEMENT
{
# ifdef _WIN32
	a = ( a >= 'A' && a <= 'Z' ) ? a - 'A' + 'a' : ( a == '\\' ? '/' : a );
	b = ( b >= 'A' && b <= 'Z' ) ? b - 'A' + 'a' : ( b == '\\' ? '/' : b );
# endif

	return ( axstr_bool_t )( a == b );
}
#else
;
#endif
/*!
 * \brief  Determine whether a given byte is a path separator.
 *
 * On all platforms, `/` is a path separator. On Windows, `\` is also a path
 * separator.
 *
 * \param  a Byte to test.
 * \return `true` if `a` is a path separator.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_ispathsep( char a )
#if AXSTR_IMPLEMENT
{
# ifdef _WIN32
	if( a == '\\' ) {
		return ( axstr_bool_t )1;
	}
# endif

	return ( axstr_bool_t )( a == '/' );
}
#else
;
#endif

/*!
 * \brief Determine whether a string is empty or not.
 *
 * \param pszSource Pointer to an ANSI or UTF-8 encoded `NUL`-terminated string.
 *                  Can be `nullptr`.
 * \return `true` if `pszSource` is empty.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_empty( const char *pszSource )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( !pszSource || *pszSource == '\0' );
}
#else
;
#endif
/*!
 * \brief Determine whether a string is used (not empty).
 *
 * \param pszSource Pointer to an ANSI or UTF-8 encoded `NUL`-terminated string.
 *                  Can be `nullptr`.
 * \return `true` if `pszSource` is used (not empty).
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_used( const char *pszSource )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( pszSource != ( const char * )0 && *pszSource != '\0' );
}
#else
;
#endif
/*!
 * \brief Retrieve the length (in bytes) of a string, not counting its final
 *        `NUL` terminator.
 *
 * \param pszSource Pointer to an ANSI or UTF-8 encoded `NUL`-terminated string.
 *                  Can be `nullptr`.
 * \return Length of the string in bytes prior to the `NUL` terminator.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_len( const char *pszSource )
#if AXSTR_IMPLEMENT
{
# if AXSTR_STDSTR_ENABLED
	return !pszSource ? 0 : strlen( pszSource );
# else
	const char *p;

	if( !pszSource ) {
		return 0;
	}

	for( p=pszSource; *p!='\0'; ++p ) {
	}

	return ( axstr_size_t )( p - pszSource );
# endif
}
#else
;
#endif
/*!
 * \brief Retrieve the length (in characters) of a string, not counting its
 *        final `NUL` terminator.
 *
 * More precisely, this returns the number of codepoints in the string.
 *
 * \note All strings are treated as UTF-8 encodings. UTF-8 characters can be
 *       between one and four bytes long.
 *
 * \return Number of codepoints in the string prior to the `NUL` terminator.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_chlen( const char *pszSource )
#if AXSTR_IMPLEMENT
{
	const axstr_utf8_t *p;
	axstr_size_t n;

	if( !( p = ( const axstr_utf8_t * )pszSource ) ) {
		return 0;
	}

	n = 0;
	while( *p != '\0' ) {
		const axstr_utf8_t *q;

		++n;
		if( ( *p & 0xF0 ) == 0xF0 ) {
			q = p + 4;
		} else if( ( *p & 0xE0 ) == 0xE0 ) {
			q = p + 3;
		} else if( ( *p & 0xC0 ) == 0xC0 ) {
			q = p + 2;
		} else {
			q = p + 1;
		}

		while( *p != '\0' && p < q ) {
			++p;
		}
	}

	return n;
}
#else
;
#endif

/*!
 * \brief Create a duplicate of a `NUL`-terminated string.
 *
 * If this fails to allocate memory then `ppszOutDst` will *not* be modified.
 *
 * \param ppszOutDst Pointer to the string pointer to modify. If *ppszOutDst is
 *                   not `nullptr` then it will be deallocated first (with
 *                   `axstr_free`).
 * \param pszSrc Source string to duplicate. If `nullptr` then no duplication
 *               occurs, but `ppszOutDst` will still be processed.
 * \param cSrc Number of bytes from `pszSrc` to copy. (A `NUL` terminator byte
 *             will be appended to the duplicated string.)
 *
 * \return Valid pointer on successful allocation; `nullptr` on deallocation
 *         *or* allocation failure.
 *
 * \note Remember to call `axstr_free()` on the returned memory when you're no
 *       longer using it.
 */
AXSTR_FUNC char *AXSTR_CALL axstr_dupton( char **ppszOutDst, const char *pszSrc, axstr_size_t cSrc )
#if AXSTR_IMPLEMENT
{
	char *tmp;

	if( pszSrc != ( const char * )0 ) {
		tmp = ( char * )axstr_alloc( cSrc + 1 );
		if( !tmp ) {
			return NULL;
		}

		axstr__memcpy( ( void * )tmp, ( const void * )pszSrc, cSrc );
		tmp[ cSrc ] = '\0';
	} else {
		tmp = ( char * )0;
	}

	if( ppszOutDst != ( char ** )0 ) {
		free( *ppszOutDst );
		*ppszOutDst = tmp;
	}

	return tmp;
}
#else
;
#endif
/*!
 * \brief Create a duplicate of a `NUL`-terminated string.
 *
 * If this fails to allocate memory then `ppszOutDst` will *not* be modified.
 *
 * \param ppszOutDst Pointer to the string pointer to modify. If *ppszOutDst is
 *                   not `nullptr` then it will be deallocated first (with
 *                   `axstr_free`).
 * \param pszSrc Source string to duplicate. If `nullptr` then no duplication
 *               occurs, but `ppszOutDst` will still be processed.
 *
 * \return Valid pointer on successful allocation; `nullptr` on deallocation
 *         *or* allocation failure.
 *
 * \note Remember to call `axstr_free()` on the returned memory when you're no
 *       longer using it.
 */
AXSTR_FUNC char *AXSTR_CALL axstr_dupto( char **ppszOutDst, const char *pszSrc )
#if AXSTR_IMPLEMENT
{
	return axstr_dupton( ppszOutDst, pszSrc, pszSrc != ( const char * )0 ? axstr_len( pszSrc ) : 0 );
}
#else
;
#endif
/*!
 * \brief Create a duplicate of a `NUL`-terminated string.
 *
 * \param pszSrc Source string to duplicate. If `nullptr` then no duplication
 *               occurs.
 * \param cSrc Number of bytes from `pszSrc` to copy. (A `NUL` terminator byte
 *             will be appended to the duplicated string.)
 *
 * \return Valid pointer on successful allocation; `nullptr` on deallocation
 *         *or* allocation failure.
 *
 * \note Remember to call `axstr_free()` on the returned memory when you're no
 *       longer using it.
 */
AXSTR_FUNC char *AXSTR_CALL axstr_dupn( const char *pszSrc, axstr_size_t cSrc )
#if AXSTR_IMPLEMENT
{
	return axstr_dupton( ( char ** )0, pszSrc, cSrc );
}
#else
;
#endif
/*!
 * \brief Create a duplicate of a `NUL`-terminated string.
 *
 * \param pszSrc Source string to duplicate. If `nullptr` then no duplication
 *               occurs.
 *
 * \return Valid pointer on successful allocation; `nullptr` on deallocation
 *         *or* allocation failure.
 *
 * \note Remember to call `axstr_free()` on the returned memory when you're no
 *       longer using it.
 */
AXSTR_FUNC char *AXSTR_CALL axstr_dup( const char *pszSrc )
#if AXSTR_IMPLEMENT
{
	return axstr_dupton( ( char ** )0, pszSrc, pszSrc != ( const char * )0 ? axstr_len( pszSrc ) : 0 );
}
#else
;
#endif

/*!
 * \brief  Stream data into a string buffer.
 *
 * This is an efficient approach for rapidly filling a string buffer with data
 * that you get bit-by-bit. Instead of the usual `strcat` approach, which would
 * require searching through the destination buffer until the `NUL`-terminator
 * is found for *each* operation, this manages state to keep track of where to
 * write to.
 *
 * \code{.c}
	static const char *const pszStrings[] = { "Welcome ", "gypsy." };
	size_t i;
	char szBuf[256];

	// state for streaming
	char *p = szBuf;
	axstr_size_t n = sizeof(szBuf);

	for( i=0; i<sizeof(pszStrings)/sizeof(pszStrings[0]); ++i ) {
		axstr_streamn( &p, &n, pszStrings[i], AXSTR_UNKNOWN_LENGTH );
	}

	printf( "%s\n", szBuf );
 * \endcode
 *
 * If the number of bytes to be written would exceed the capacity of the buffer
 * then the output will be truncated. A `NUL`-terminator will always be written
 * within the capacity of the destination buffer.
 *
 * \param  ppszDstBuf Pointer to a pointer of the destination buffer. The inner
 *                    pointer will be incremented by the number of bytes
 *                    written, such that it always points to the next location
 *                    to be written to.
 * \param  pcDstBytes Pointer to the number of bytes remaining in `ppszDstBuf`.
 *                    The value this points to will be decremented by the number
 *                    of bytes written.
 * \param  pszSrc     Source string to copy from.
 * \param  cSrcBytes  Number of bytes in the source string to copy from, *or*
 *                    `AXSTR_UNKNOWN_LENGTH` to copy everything until the
 *                    `NUL`-terminator of `pszSrc`.
 * \return Number of bytes written to `ppszDstBuf`.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_streamn( char **ppszDstBuf, axstr_size_t *pcDstBytes, const char *pszSrc, axstr_size_t cSrcBytes )
#if AXSTR_IMPLEMENT
{
# if AXSTR_WINSTR_ENABLED
	char *pszDstBase;
	char *pszDstEnd;
	size_t cRemaining;
	HRESULT r;

	if( cSrcBytes == AXSTR_UNKNOWN_LENGTH ) {
		cSrcBytes = pszSrc != ( const char * )0 ? axstr_len( pszSrc ) : 0;
	}

	r = StringCbCopyNExA( *ppszDstBuf, *pcDstBytes, pszSrc, cSrcBytes, &pszDstEnd, &cRemaining, STRSAFE_IGNORE_NULLS );
	if( r != S_OK ) {
		return 0;
	}

	pszDstBase = *ppszDstBuf;
	*ppszDstBuf = pszDstEnd;
	*pcDstBytes = cRemaining;

	return ( axstr_size_t )( pszDstEnd - pszDstBase );
# else
	char *d, *de;
	const char *s, *se;
	axstr_size_t cWritten;

	d = *ppszDstBuf;
	de = d + *pcDstBytes - 1;

	s = pszSrc;
	se = cSrcBytes == AXSTR_UNKNOWN_LENGTH ? ( const char * )AXSTR_UNKNOWN_LENGTH : s + cSrcBytes;

	while( d < de && s < se ) {
		if( *s == '\0' ) {
			break;
		}

		*d++ = *s++;
	}

	*d = '\0';

	cWritten = d - *ppszDstBuf;

	*ppszDstBuf = d;
	*pcDstBytes -= cWritten;

	return cWritten;
# endif
}
#else
;
#endif
/*!
 * \brief  Stream data into a string buffer.
 *
 * This is an efficient approach for rapidly filling a string buffer with data
 * that you get bit-by-bit. Instead of the usual `strcat` approach, which would
 * require searching through the destination buffer until the `NUL`-terminator
 * is found for *each* operation, this manages state to keep track of where to
 * write to.
 *
 * \code{.c}
	static const char *const pszStrings[] = { "Welcome ", "gypsy." };
	size_t i;
	char szBuf[256];

	// state for streaming
	char *p = szBuf;
	axstr_size_t n = sizeof(szBuf);

	for( i=0; i<sizeof(pszStrings)/sizeof(pszStrings[0]); ++i ) {
		axstr_stream( &p, &n, pszStrings[i] );
	}

	printf( "%s\n", szBuf );
 * \endcode
 *
 * If the number of bytes to be written would exceed the capacity of the buffer
 * then the output will be truncated. A `NUL`-terminator will always be written
 * within the capacity of the destination buffer.
 *
 * \param  ppszDstBuf Pointer to a pointer of the destination buffer. The inner
 *                    pointer will be incremented by the number of bytes
 *                    written, such that it always points to the next location
 *                    to be written to.
 * \param  pcDstBytes Pointer to the number of bytes remaining in `ppszDstBuf`.
 *                    The value this points to will be decremented by the number
 *                    of bytes written.
 * \param  pszSrc     `NUL`-terminated source string to copy from.
 * \return Number of bytes written to `ppszDstBuf`.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_stream( char **ppszDstBuf, axstr_size_t *pcDstBytes, const char *pszSrc )
#if AXSTR_IMPLEMENT
{
	return axstr_streamn( ppszDstBuf, pcDstBytes, pszSrc, AXSTR_UNKNOWN_LENGTH );
}
#else
;
#endif

/*!
 * \brief Copy one string to another.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_cpyn( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszSrc, axstr_size_t cSrcBytes )
#if AXSTR_IMPLEMENT
{
	return axstr_streamn( &pszDstBuf, &cDstBytes, pszSrc, cSrcBytes );
}
#else
;
#endif
/*!
 * \brief Copy one string to another.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_cpy( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszSrc )
#if AXSTR_IMPLEMENT
{
	return axstr_streamn( &pszDstBuf, &cDstBytes, pszSrc, AXSTR_UNKNOWN_LENGTH );
}
#else
;
#endif

/*!
 * \brief Append one string to another.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_catn( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszSrc, axstr_size_t cSrcBytes )
#if AXSTR_IMPLEMENT
{
	char *p;
	axstr_size_t n;

	p = strchr( pszDstBuf, '\0' );
	n = cDstBytes - ( axstr_size_t )( p - pszDstBuf );
	return axstr_streamn( &p, &n, pszSrc, cSrcBytes );
}
#else
;
#endif
/*!
 * \brief Append one string to another.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_cat( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszSrc )
#if AXSTR_IMPLEMENT
{
	char *p;
	axstr_size_t n;

	p = strchr( pszDstBuf, '\0' );
	n = cDstBytes - ( axstr_size_t )( p - pszDstBuf );
	return axstr_streamn( &p, &n, pszSrc, AXSTR_UNKNOWN_LENGTH );
}
#else
;
#endif

/*!
 * \brief Determine whether the test string starts with the given start string.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_starts( const char *pszTest, const char *pszStart )
#if AXSTR_IMPLEMENT
{
	return axstr_cmpn( pszTest, pszStart, axstr_len( pszStart ) );
}
#else
;
#endif
/*!
 * \brief Determine whether the test string ends with the given end string.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_ends( const char *pszTest, const char *pszEnd )
#if AXSTR_IMPLEMENT
{
	axstr_size_t testlen;
	axstr_size_t endlen;

	testlen = axstr_len( pszTest );
	endlen = axstr_len( pszEnd );

	if( endlen > testlen ) {
		return ( axstr_bool_t )0;
	}

	return axstr_cmpn( &pszTest[ testlen - endlen ], pszEnd, endlen );
}
#else
;
#endif

/*!
 * \brief Append a string to a path.
 *
 * If last character (before the `NUL`-terminator) in `pszDstBuf` is not a path
 * separator (`/` on all platforms; `/` or `\` for MS) then the platform's
 * preferred path separator (`\` on Windows, `/` everywhere else) will be
 * appended first.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_catpathn( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszSrc, axstr_size_t cSrcBytes )
#if AXSTR_IMPLEMENT
{
	axstr_size_t dstlen;
	axstr_size_t n;
	char *p;

	dstlen = axstr_len( pszDstBuf );
	if( dstlen + 2 >= cDstBytes ) {
		return 0;
	}

	if( dstlen > 0 && !axstr_ispathsep( pszDstBuf[ dstlen - 1 ] ) ) {
		pszDstBuf[ dstlen++ ] = AXSTR_DIRSEP_CH;
		pszDstBuf[ dstlen ] = '\0';
	}

	p = &pszDstBuf[ dstlen ];
	n = cDstBytes - dstlen;
	return axstr_streamn( &p, &n, pszSrc, cSrcBytes );
}
#else
;
#endif
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_catpath( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszSrc )
#if AXSTR_IMPLEMENT
{
	return axstr_catpathn( pszDstBuf, cDstBytes, pszSrc, AXSTR_UNKNOWN_LENGTH );
}
#else
;
#endif

/*!
 * \brief Move a set of bytes within a string
 */
AXSTR_FUNC void AXSTR_CALL axstr_move( char *pDstBuf, axstr_size_t cDstBytes, axstr_size_t uTargetPos, axstr_size_t uSourcePos, axstr_size_t cBytes )
#if AXSTR_IMPLEMENT
{
	if( !pDstBuf || !cDstBytes || uTargetPos >= cDstBytes || uSourcePos >= cDstBytes || !cBytes ) {
		return;
	}

# if AXSTR_STDSTR_ENABLED
#  if defined( _MSC_VER ) && defined( __STDC_WANT_SECURE_LIB__ )
	memmove_s( &pDstBuf[ uTargetPos ], cDstBytes - uTargetPos, &pDstBuf[ uSourcePos ], cBytes );
#  else
	memmove( &pDstBuf[ uTargetPos ], &pDstBuf[ uSourcePos ], cBytes );
#  endif
# else
	{
		char *dst = &pDstBuf[ uTargetPos ];
		const char *src = &pDstBuf[ uSourcePos ];
		axstr_size_t n = cBytes;

		if( dst < src ) {
			while( n > 0 ) {
				--n;
				*dst++ = *src++;
			}
		} else if( dst > src ) {
			while( n > 0 ) {
				--n;
				dst[ n ] = src[ n ];
			}
		}
	}
# endif
}
#else
;
#endif

/*!
 * \brief  Remove a range of bytes from a given string.
 *
 * \param  pszDstBuf Pointer to the string to be manipulated.
 * \param  cDstBytes Current length of the string (not the maximum size).
 * \param  uOffset   Start of the range to remove.
 * \param  cBytes    Number of bytes to remove.
 * \return New length of the string (in bytes).
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_remove( char *pszDstBuf, axstr_size_t cDstBytes, axstr_size_t uOffset, axstr_size_t cBytes )
#if AXSTR_IMPLEMENT
{
	axstr_size_t uEnd;
	axstr_size_t cLen;

	if( !pszDstBuf || !cDstBytes || !cBytes || uOffset >= cDstBytes ) {
		return cDstBytes;
	}

	uEnd = uOffset + cBytes < cDstBytes ? uOffset + cBytes : cDstBytes;
	cLen = uEnd - uOffset;

	axstr_move( pszDstBuf, cDstBytes, uOffset, uEnd, cDstBytes - uEnd );
	pszDstBuf[ cDstBytes - cLen ] = '\0';

	return cDstBytes - cLen;
}
#else
;
#endif

/*!
 * \brief Format a string, with the given varargs list.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_printfv( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszFormat, va_list vlArgs )
#if AXSTR_IMPLEMENT
{
	axstr_ptrdiff_t r;

# if AXSTR_STDSTR_ENABLED
#  if defined( _MSC_VER ) && defined( __STDC_WANT_SECURE_LIB__ )
	r = vsprintf_s( pszDstBuf, cDstBytes, pszFormat, vlArgs );
#  else
	r = vsnprintf( pszDstBuf, cDstBytes, pszFormat, vlArgs );
	pszDstBuf[ cDstBytes - 1 ] = '\0';
#  endif
# else
	/* TODO */
	r = -1;
# endif

	return ( axstr_size_t )( r < 0 ? 0 : r );
}
#else
;
#endif
/*!
 * \brief Format a string.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALLF axstr_printf( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszFormat, ... )
#if AXSTR_IMPLEMENT
{
	va_list vlArgs;
	axstr_size_t r;

	va_start( vlArgs, pszFormat );
	r = axstr_printfv( pszDstBuf, cDstBytes, pszFormat, vlArgs );
	va_end( vlArgs );

	return r;
}
#else
;
#endif

/*!
 * \brief Create a temporary formatted string, with variable arguments list.
 *
 * \warning Not thread safe!
 */
AXSTR_FUNC const char *AXSTR_CALL axstrfv( const char *pszFormat, va_list vlArgs )
#if AXSTR_IMPLEMENT
{
	static char buf[ 65536 ];
	static axstr_size_t curbuf = 0;
	static const axstr_size_t minbuf = sizeof( buf )/6 + 1;

	axstr_size_t n;
	char *p;

	if( curbuf + minbuf > sizeof( buf ) ) {
		curbuf = 0;
	}

	n = sizeof( buf ) - curbuf;
	p = &buf[ curbuf ];

	curbuf += axstr_printfv( p, n, pszFormat, vlArgs );

	return p;
}
#else
;
#endif
/*!
 * \brief Create a temporary formatted string.
 *
 * \warning Not thread safe!
 */
AXSTR_FUNC const char *AXSTR_CALLF axstrf( const char *pszFormat, ... )
#if AXSTR_IMPLEMENT
{
	va_list vlArgs;
	const char *p;

	va_start( vlArgs, pszFormat );
	p = axstrfv( pszFormat, vlArgs );
	va_end( vlArgs );

	return p;
}
#else
;
#endif

/*!
 * \brief  Perform one step of a UTF-8 to UTF-32 decoding.
 *
 * \param  ppUTF8Src   Points to the pointer of what will be read from. That
 *                     pointer is incremented by this function upon completion.
 * \param  pUTF8SrcEnd One element past the end of the buffer referenced by
 *                     `*ppUTF8Src`. No data will be read from this point or
 *                     beyond.
 * \return UTF-32 encoding of the character.
 */
AXSTR_FUNC axstr_utf32_t AXSTR_CALL axstr_step_utf8_decode( const axstr_utf8_t **ppUTF8Src, const axstr_utf8_t *pUTF8SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf32_t uCodepoint;
	const axstr_utf8_t *p;

	p = *ppUTF8Src;

	if( p >= pUTF8SrcEnd ) {
		return 0;
	}

	if( ( p[0] & 0xF0 ) == 0xF0 ) {
		if( p + 4 > pUTF8SrcEnd ) {
			*ppUTF8Src = pUTF8SrcEnd;
			return 0xFFFD;
		}

		uCodepoint  = ( ( axstr_utf32_t )( p[0] & 0x07 ) ) << 18;
		uCodepoint |= ( ( axstr_utf32_t )( p[1] & 0x3F ) ) << 12;
		uCodepoint |= ( ( axstr_utf32_t )( p[2] & 0x3F ) ) <<  6;
		uCodepoint |= ( ( axstr_utf32_t )( p[3] & 0x3F ) ) <<  0;

		*ppUTF8Src += 4;
		return uCodepoint;
	}

	if( ( p[0] & 0xE0 ) == 0xE0 ) {
		if( p + 3 > pUTF8SrcEnd ) {
			*ppUTF8Src = pUTF8SrcEnd;
			return 0xFFFD;
		}

		uCodepoint  = ( ( axstr_utf32_t )( p[0] & 0x0F ) ) << 12;
		uCodepoint |= ( ( axstr_utf32_t )( p[1] & 0x3F ) ) <<  6;
		uCodepoint |= ( ( axstr_utf32_t )( p[2] & 0x3F ) ) <<  0;

		*ppUTF8Src += 3;
		return uCodepoint;
	}

	if( ( p[0] & 0xC0 ) == 0xC0 ) {
		if( p + 2 > pUTF8SrcEnd ) {
			*ppUTF8Src = pUTF8SrcEnd;
			return 0xFFFD;
		}

		uCodepoint  = ( ( axstr_utf32_t )( p[0] & 0x1F ) ) <<  6;
		uCodepoint |= ( ( axstr_utf32_t )( p[1] & 0x3F ) ) <<  0;

		*ppUTF8Src += 2;
		return uCodepoint;
	}

	uCodepoint = ( axstr_utf32_t )( p[0] & 0x7F );
	*ppUTF8Src += 1;

	return uCodepoint;
}
#else
;
#endif
/*!
 * \brief  Perform one step of a UTF-16 to UTF-32 decoding.
 *
 * \param  ppUTF16Src   Points to the pointer of what will be read from. That
 *                      pointer is incremented by this function upon completion.
 * \param  pUTF16SrcEnd One element past the end of the buffer referenced by
 *                      `*ppUTF16Src`. No data will be read from this point or
 *                      beyond.
 * \return UTF-32 encoding of the character.
 */
AXSTR_FUNC axstr_utf32_t AXSTR_CALL axstr_step_utf16_decode( const axstr_utf16_t **ppUTF16Src, const axstr_utf16_t *pUTF16SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf32_t uCodepoint;

	if( *ppUTF16Src >= pUTF16SrcEnd ) {
		return 0;
	}

	/* if encoded as one codepoint then return that codepoint */
	if( **ppUTF16Src < 0xD800 || **ppUTF16Src > 0xDFFF ) {
		return ( axstr_utf32_t )*( *ppUTF16Src )++;
	}

	/* two codepoints */
	if( *ppUTF16Src + 1 >= pUTF16SrcEnd ) {
		*ppUTF16Src = pUTF16SrcEnd;
		return 0xFFFD;
	}

	/* check for invalid first codepoint */
	if( **ppUTF16Src > 0xDBFF ) {
		( *ppUTF16Src ) += 2;
		return 0xFFFD;
	}

	/* check for invalid second codepoint */
	if( ( *ppUTF16Src )[1] < 0xDC00 || ( *ppUTF16Src )[1] > 0xDFFF ) {
		( *ppUTF16Src ) += 2;
		return 0xFFFD;
	}

	/* encode */
	uCodepoint = 0x10000 + ( ( ( ( axstr_utf32_t )( ( *ppUTF16Src )[0] & 0x3FF ) )<<10 ) | ( ( ( *ppUTF16Src )[1] ) & 0x3FF ) );
	( *ppUTF16Src ) += 2;

	return uCodepoint;
}
#else
;
#endif

/*!
 * \brief  Perform one step of a UTF-32 to UTF-8 encoding.
 *
 * \param  ppUTF8Dst   Points to the pointer of what will be written to. That
 *                     pointer is incremented by this function upon completion.
 * \param  pUTF8DstEnd One element past the end of the buffer referenced by
 *                     `*ppUTF8Dst`. No data will be written to or beyond this
 *                     point.
 * \param  uCodepoint  Unicode character to encode.
 * \return `1` on success; `0` on failure.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_step_utf8_encode( axstr_utf8_t **ppUTF8Dst, axstr_utf8_t *pUTF8DstEnd, axstr_utf32_t uCodepoint )
#if AXSTR_IMPLEMENT
{
	if( uCodepoint > 0x10000 ) {
		if( *ppUTF8Dst + 4 >= pUTF8DstEnd ) {
			return 0;
		}

		( *ppUTF8Dst )[ 0 ] = ( axstr_utf8_t )( 0xF0 | ( ( uCodepoint>>18 ) & 0x07 ) );
		( *ppUTF8Dst )[ 1 ] = ( axstr_utf8_t )( 0x80 | ( ( uCodepoint>>12 ) & 0x3F ) );
		( *ppUTF8Dst )[ 2 ] = ( axstr_utf8_t )( 0x80 | ( ( uCodepoint>> 6 ) & 0x3F ) );
		( *ppUTF8Dst )[ 3 ] = ( axstr_utf8_t )( 0x80 | ( ( uCodepoint>> 0 ) & 0x3F ) );

		*ppUTF8Dst += 4;
	} else if( uCodepoint > 0x7FF ) {
		if( *ppUTF8Dst + 3 >= pUTF8DstEnd ) {
			return 0;
		}

		( *ppUTF8Dst )[ 0 ] = ( axstr_utf8_t )( 0xE0 | ( ( uCodepoint>>12 ) & 0x0F ) );
		( *ppUTF8Dst )[ 1 ] = ( axstr_utf8_t )( 0x80 | ( ( uCodepoint>> 6 ) & 0x3F ) );
		( *ppUTF8Dst )[ 2 ] = ( axstr_utf8_t )( 0x80 | ( ( uCodepoint>> 0 ) & 0x3F ) );

		*ppUTF8Dst += 3;
	} else if( uCodepoint > 0x7F ) {
		if( *ppUTF8Dst + 2 >= pUTF8DstEnd ) {
			return 0;
		}

		( *ppUTF8Dst )[ 0 ] = ( axstr_utf8_t )( 0xC0 | ( ( uCodepoint>> 6 ) & 0x1F ) );
		( *ppUTF8Dst )[ 1 ] = ( axstr_utf8_t )( 0x80 | ( ( uCodepoint>> 0 ) & 0x3F ) );

		*ppUTF8Dst += 2;
	} else {
		if( *ppUTF8Dst + 1 >= pUTF8DstEnd ) {
			return 0;
		}

		( *ppUTF8Dst )[ 0 ] = ( axstr_utf8_t )uCodepoint;

		*ppUTF8Dst += 1;
	}

	return 1;
}
#else
;
#endif
/*!
 * \brief  Perform one step of a UTF-32 to UTF-16 encoding.
 *
 * \param  ppUTF16Dst   Points to the pointer of what will be written to. That
 *                      pointer is incremented by this function upon completion.
 * \param  pUTF16DstEnd One element past the end of the buffer referenced by
 *                      `*ppUTF16Dst`. No data will be written to or beyond this
 *                      point.
 * \param  uCodepoint   Unicode character to encode.
 *
 * \return `1` on success; `0` on failure.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_step_utf16_encode( axstr_utf16_t **ppUTF16Dst, axstr_utf16_t *pUTF16DstEnd, axstr_utf32_t uCodepoint )
#if AXSTR_IMPLEMENT
{
	if( uCodepoint >= 0x10000 ) {
		if( *ppUTF16Dst + 2 >= pUTF16DstEnd ) {
			return 0;
		}

		( *ppUTF16Dst )[0] = 0xD800 | ( axstr_utf16_t )( ( uCodepoint >> 10 ) & 0x3FF );
		( *ppUTF16Dst )[1] = 0xDC00 | ( axstr_utf16_t )( ( uCodepoint >>  0 ) & 0x3FF );

		*ppUTF16Dst += 2;
	} else {
		if( *ppUTF16Dst + 1 >= pUTF16DstEnd ) {
			return 0;
		}

		( *ppUTF16Dst )[0] = ( axstr_utf16_t )uCodepoint;

		*ppUTF16Dst += 1;
	}

	return 1;
}
#else
;
#endif

/*!
 * \brief Convert a chunk of UTF-8 data to UTF-16 data, returning the number of
 *        codepoints written.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_counted_utf8_to_utf16( axstr_utf16_t *pUTF16Dst, axstr_size_t cMaxDstUTF16Chars, const axstr_utf8_t *pUTF8Src )
#if AXSTR_IMPLEMENT
{
	axstr_utf16_t *dstp;
	axstr_utf16_t *dste;
	const axstr_utf8_t *srcp;
	const axstr_utf8_t *srce;

	dstp = pUTF16Dst;
	dste = dstp + cMaxDstUTF16Chars;
	srcp = pUTF8Src;
	srce = ( const axstr_utf8_t * )( ~( axstr_size_t )0 );

	while( *srcp != '\0' ) {
		axstr_utf32_t uCodepoint;

		uCodepoint = axstr_step_utf8_decode( &srcp, srce );
		if( !axstr_step_utf16_encode( &dstp, dste, uCodepoint ) ) {
			return 0;
		}
	}

	*dstp = ( axstr_utf16_t )'\0';
	return ( axstr_size_t )( dstp - pUTF16Dst );
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-8 data to UTF-16 data, returning the number of
 *        codepoints written.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_counted_utf8_to_utf16_n( axstr_utf16_t *pUTF16Dst, axstr_size_t cMaxDstUTF16Chars, const axstr_utf8_t *pUTF8Src, const axstr_utf8_t *pUTF8SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf16_t *dstp;
	axstr_utf16_t *dste;
	const axstr_utf8_t *srcp;
	const axstr_utf8_t *srce;

	dstp = pUTF16Dst;
	dste = dstp + cMaxDstUTF16Chars;
	srcp = pUTF8Src;
	srce = pUTF8SrcEnd;

	while( srcp < srce ) {
		axstr_utf32_t uCodepoint;

		uCodepoint = axstr_step_utf8_decode( &srcp, srce );
		if( !axstr_step_utf16_encode( &dstp, dste, uCodepoint ) ) {
			return 0;
		}
	}

	*dstp = ( axstr_utf16_t )'\0';
	return ( axstr_size_t )( dstp - pUTF16Dst );
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-8 data to UTF-32 data, returning the number of
 *        codepoints written.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_counted_utf8_to_utf32( axstr_utf32_t *pUTF32Dst, axstr_size_t cMaxDstUTF32Chars, const axstr_utf8_t *pUTF8Src )
#if AXSTR_IMPLEMENT
{
	axstr_utf32_t *dstp;
	axstr_utf32_t *dste;
	const axstr_utf8_t *srcp;
	const axstr_utf8_t *srce;

	dstp = pUTF32Dst;
	dste = pUTF32Dst + cMaxDstUTF32Chars;
	srcp = pUTF8Src;
	srce = ( const axstr_utf8_t * )( ~( axstr_size_t )0 );

	while( *srcp != '\0' ) {
		if( dstp + 1 >= dste ) {
			break;
		}

		*dstp++ = axstr_step_utf8_decode( &srcp, srce );
	}

	*dstp = 0;
	return ( axstr_size_t )( dstp - pUTF32Dst );
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-8 data to UTF-32 data, returning the number of
 *        codepoints written.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_counted_utf8_to_utf32_n( axstr_utf32_t *pUTF32Dst, axstr_size_t cMaxDstUTF32Chars, const axstr_utf8_t *pUTF8Src, const axstr_utf8_t *pUTF8SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf32_t *dstp;
	axstr_utf32_t *dste;
	const axstr_utf8_t *srcp;
	const axstr_utf8_t *srce;

	dstp = pUTF32Dst;
	dste = pUTF32Dst + cMaxDstUTF32Chars;
	srcp = pUTF8Src;
	srce = pUTF8SrcEnd;

	while( srcp < srce ) {
		if( dstp + 1 >= dste ) {
			break;
		}

		*dstp++ = axstr_step_utf8_decode( &srcp, srce );
	}

	*dstp = 0;
	return ( axstr_size_t )( dstp - pUTF32Dst );
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-16 data to UTF-8 data, returning the number of
 *        codepoints written.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_counted_utf16_to_utf8( axstr_utf8_t *pUTF8Dst, axstr_size_t cMaxDstUTF8Bytes, const axstr_utf16_t *pUTF16Src )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;
	const axstr_utf16_t *srcp;
	const axstr_utf16_t *srce;

	dstp = pUTF8Dst;
	dste = dstp + cMaxDstUTF8Bytes;
	srcp = pUTF16Src;
	srce = ( const axstr_utf16_t * )( ~( axstr_size_t )0 );

	while( *srcp != '\0' ) {
		axstr_utf32_t uCodepoint;

		uCodepoint = axstr_step_utf16_decode( &srcp, srce );
		if( !axstr_step_utf8_encode( &dstp, dste, uCodepoint ) ) {
			return 0;
		}
	}

	*dstp = '\0';
	return ( axstr_size_t )( dstp - pUTF8Dst );
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-16 data to UTF-8 data, returning the number of
 *        codepoints written.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_counted_utf16_to_utf8_n( axstr_utf8_t *pUTF8Dst, axstr_size_t cMaxDstUTF8Bytes, const axstr_utf16_t *pUTF16Src, const axstr_utf16_t *pUTF16SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;
	const axstr_utf16_t *srcp;
	const axstr_utf16_t *srce;

	dstp = pUTF8Dst;
	dste = dstp + cMaxDstUTF8Bytes;
	srcp = pUTF16Src;
	srce = pUTF16SrcEnd;

	while( srcp < srce ) {
		axstr_utf32_t uCodepoint;

		uCodepoint = axstr_step_utf16_decode( &srcp, srce );
		if( !axstr_step_utf8_encode( &dstp, dste, uCodepoint ) ) {
			return 0;
		}
	}

	*dstp = '\0';
	return ( axstr_size_t )( dstp - pUTF8Dst );
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-32 data to UTF-8 data, returning the number of
 *        codepoints written.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_counted_utf32_to_utf8( axstr_utf8_t *pUTF8Dst, axstr_size_t cMaxDstUTF8Bytes, const axstr_utf32_t *pUTF32Src )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;
	const axstr_utf32_t *srcp;

	dstp = pUTF8Dst;
	dste = pUTF8Dst + cMaxDstUTF8Bytes;
	srcp = pUTF32Src;

	while( *srcp != 0 ) {
		if( !axstr_step_utf8_encode( &dstp, dste, *srcp++ ) ) {
			return 0;
		}
	}

	*dstp = '\0';
	return ( axstr_size_t )( dstp - pUTF8Dst );
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-32 data to UTF-8 data, returning the number of
 *        codepoints written.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_counted_utf32_to_utf8_n( axstr_utf8_t *pUTF8Dst, axstr_size_t cMaxDstUTF8Bytes, const axstr_utf32_t *pUTF32Src, const axstr_utf32_t *pUTF32SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;
	const axstr_utf32_t *srcp;
	const axstr_utf32_t *srce;

	dstp = pUTF8Dst;
	dste = pUTF8Dst + cMaxDstUTF8Bytes;
	srcp = pUTF32Src;
	srce = pUTF32SrcEnd;

	while( srcp < srce ) {
		if( !axstr_step_utf8_encode( &dstp, dste, *srcp++ ) ) {
			return 0;
		}
	}

	*dstp = '\0';
	return ( axstr_size_t )( dstp - pUTF8Dst );
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-8 data to UTF-16 data.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_utf8_to_utf16( axstr_utf16_t *pUTF16Dst, axstr_size_t cMaxDstUTF16Chars, const axstr_utf8_t *pUTF8Src )
#if AXSTR_IMPLEMENT
{
	axstr_utf16_t *dstp;
	axstr_utf16_t *dste;
	const axstr_utf8_t *srcp;
	const axstr_utf8_t *srce;

	dstp = pUTF16Dst;
	dste = dstp + cMaxDstUTF16Chars;
	srcp = pUTF8Src;
	srce = ( const axstr_utf8_t * )( ~( axstr_size_t )0 );

	while( *srcp != '\0' ) {
		axstr_utf32_t uCodepoint;

		uCodepoint = axstr_step_utf8_decode( &srcp, srce );
		if( !axstr_step_utf16_encode( &dstp, dste, uCodepoint ) ) {
			return 0;
		}
	}

	*dstp = ( axstr_utf16_t )'\0';
	return 1;
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-8 data to UTF-16 data.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_utf8_to_utf16_n( axstr_utf16_t *pUTF16Dst, axstr_size_t cMaxDstUTF16Chars, const axstr_utf8_t *pUTF8Src, const axstr_utf8_t *pUTF8SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf16_t *dstp;
	axstr_utf16_t *dste;
	const axstr_utf8_t *srcp;
	const axstr_utf8_t *srce;

	dstp = pUTF16Dst;
	dste = dstp + cMaxDstUTF16Chars;
	srcp = pUTF8Src;
	srce = pUTF8SrcEnd;

	while( srcp < srce ) {
		axstr_utf32_t uCodepoint;

		uCodepoint = axstr_step_utf8_decode( &srcp, srce );
		if( !axstr_step_utf16_encode( &dstp, dste, uCodepoint ) ) {
			return 0;
		}
	}

	*dstp = ( axstr_utf16_t )'\0';
	return 1;
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-8 data to UTF-32 data.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_utf8_to_utf32( axstr_utf32_t *pUTF32Dst, axstr_size_t cMaxDstUTF32Chars, const axstr_utf8_t *pUTF8Src )
#if AXSTR_IMPLEMENT
{
	axstr_utf32_t *dstp;
	axstr_utf32_t *dste;
	const axstr_utf8_t *srcp;
	const axstr_utf8_t *srce;

	dstp = pUTF32Dst;
	dste = pUTF32Dst + cMaxDstUTF32Chars;
	srcp = pUTF8Src;
	srce = ( const axstr_utf8_t * )( ~( axstr_size_t )0 );

	while( *srcp != '\0' ) {
		if( dstp + 1 >= dste ) {
			break;
		}

		*dstp++ = axstr_step_utf8_decode( &srcp, srce );
	}

	*dstp = 0;
	return 1;
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-8 data to UTF-32 data.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_utf8_to_utf32_n( axstr_utf32_t *pUTF32Dst, axstr_size_t cMaxDstUTF32Chars, const axstr_utf8_t *pUTF8Src, const axstr_utf8_t *pUTF8SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf32_t *dstp;
	axstr_utf32_t *dste;
	const axstr_utf8_t *srcp;
	const axstr_utf8_t *srce;

	dstp = pUTF32Dst;
	dste = pUTF32Dst + cMaxDstUTF32Chars;
	srcp = pUTF8Src;
	srce = pUTF8SrcEnd;

	while( srcp < srce ) {
		if( dstp + 1 >= dste ) {
			break;
		}

		*dstp++ = axstr_step_utf8_decode( &srcp, srce );
	}

	*dstp = 0;
	return 1;
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-16 data to UTF-8 data.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_utf16_to_utf8( axstr_utf8_t *pUTF8Dst, axstr_size_t cMaxDstUTF8Bytes, const axstr_utf16_t *pUTF16Src )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;
	const axstr_utf16_t *srcp;
	const axstr_utf16_t *srce;

	dstp = pUTF8Dst;
	dste = dstp + cMaxDstUTF8Bytes;
	srcp = pUTF16Src;
	srce = ( const axstr_utf16_t * )( ~( axstr_size_t )0 );

	while( *srcp != '\0' ) {
		axstr_utf32_t uCodepoint;

		uCodepoint = axstr_step_utf16_decode( &srcp, srce );
		if( !axstr_step_utf8_encode( &dstp, dste, uCodepoint ) ) {
			return 0;
		}
	}

	*dstp = '\0';
	return 1;
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-16 data to UTF-8 data.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_utf16_to_utf8_n( axstr_utf8_t *pUTF8Dst, axstr_size_t cMaxDstUTF8Bytes, const axstr_utf16_t *pUTF16Src, const axstr_utf16_t *pUTF16SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;
	const axstr_utf16_t *srcp;
	const axstr_utf16_t *srce;

	dstp = pUTF8Dst;
	dste = dstp + cMaxDstUTF8Bytes;
	srcp = pUTF16Src;
	srce = pUTF16SrcEnd;

	while( srcp < srce ) {
		axstr_utf32_t uCodepoint;

		uCodepoint = axstr_step_utf16_decode( &srcp, srce );
		if( !axstr_step_utf8_encode( &dstp, dste, uCodepoint ) ) {
			return 0;
		}
	}

	*dstp = '\0';
	return 1;
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-32 data to UTF-8 data.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_utf32_to_utf8( axstr_utf8_t *pUTF8Dst, axstr_size_t cMaxDstUTF8Bytes, const axstr_utf32_t *pUTF32Src )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;
	const axstr_utf32_t *srcp;

	dstp = pUTF8Dst;
	dste = pUTF8Dst + cMaxDstUTF8Bytes;
	srcp = pUTF32Src;

	while( *srcp != 0 ) {
		if( !axstr_step_utf8_encode( &dstp, dste, *srcp++ ) ) {
			return 0;
		}
	}

	*dstp = '\0';
	return 1;
}
#else
;
#endif
/*!
 * \brief Convert a chunk of UTF-32 data to UTF-8 data.
 *
 * This assumes the byte-order of the host machine.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_utf32_to_utf8_n( axstr_utf8_t *pUTF8Dst, axstr_size_t cMaxDstUTF8Bytes, const axstr_utf32_t *pUTF32Src, const axstr_utf32_t *pUTF32SrcEnd )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;
	const axstr_utf32_t *srcp;
	const axstr_utf32_t *srce;

	dstp = pUTF8Dst;
	dste = pUTF8Dst + cMaxDstUTF8Bytes;
	srcp = pUTF32Src;
	srce = pUTF32SrcEnd;

	while( srcp < srce ) {
		if( !axstr_step_utf8_encode( &dstp, dste, *srcp++ ) ) {
			return 0;
		}
	}

	*dstp = '\0';
	return 1;
}
#else
;
#endif
/*!
 * \brief Write a byte-order-mark (BOM) to a buffer
 *
 * If `bom` is set to `axstr_bom_enabled` then the appropriate byte-order-mark
 * for the encoding `enc` is used.
 *
 * \return Number of bytes written to `pDstBuf`. (Will not exceed `cDstBytes`.)
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_write_bom( void *pDstBuf, axstr_size_t cDstBytes, axstr_encoding_t enc, axstr_byteordermark_t bom )
#if AXSTR_IMPLEMENT
{
	static const unsigned char UTF8_BOM[] = { 0xEF, 0xBB, 0xBF };
	static const unsigned char UTF16_BE_BOM[] = { 0xFE, 0xFF };
	static const unsigned char UTF16_LE_BOM[] = { 0xFF, 0xFE };
	static const unsigned char UTF32_BE_BOM[] = { 0x00, 0x00, 0xFE, 0xFF };
	static const unsigned char UTF32_LE_BOM[] = { 0xFF, 0xFE, 0x00, 0x00 };

	if( cDstBytes < 4 ) {
		return 0;
	}

	if( bom == axstr_bom_enabled ) {
		const unsigned char *p;
		axstr_size_t n;

		p = ( const unsigned char * )0;
		n = 0;

		switch( enc )
		{
		case axstr_enc_unknown:
			break;

		case axstr_enc_utf8:
			p = &UTF8_BOM[0];
			n = sizeof( UTF8_BOM );
			break;
		case axstr_enc_utf16_be:
			p = &UTF16_BE_BOM[0];
			n = sizeof( UTF16_BE_BOM );
			break;
		case axstr_enc_utf16_le:
			p = &UTF16_LE_BOM[0];
			n = sizeof( UTF16_LE_BOM );
			break;
		case axstr_enc_utf32_be:
			p = &UTF32_BE_BOM[0];
			n = sizeof( UTF32_BE_BOM );
			break;
		case axstr_enc_utf32_le:
			p = &UTF32_LE_BOM[0];
			n = sizeof( UTF32_LE_BOM );
			break;
		}

		axstr__memcpy( pDstBuf, ( const void * )p, n );
		return n;
	}

	return 0;
}
#else
;
#endif

#if AXSTR_IMPLEMENT
/*! \internal */
static axstr_bool_t AXSTR_CALL axstr_stream_bytes_( void **ppDstBuf, axstr_size_t *pcDstBytes, const void *pSrc, axstr_size_t cSrcBytes )
{
	axstr_size_t i;
	unsigned char *d;
	const unsigned char *s;

	if( cSrcBytes > *pcDstBytes ) {
		return ( axstr_bool_t )0;
	}

	d = ( unsigned char * )*ppDstBuf;
	s = ( const unsigned char * )pSrc;
	for( i = 0; i < cSrcBytes; ++i ) {
		*d++ = *s++;
	}

	*pcDstBytes -= cSrcBytes;
	*ppDstBuf = ( void * )d;

	return ( axstr_bool_t )1;
}
#endif

/*!
 * \brief Convert a UTF-8 stream to an arbitrary binary encoding with an
 *        optional byte-order-mark.
 *
 * Does not explicitly write a `NUL`-terminator. (Useful when writing to a
 * (potentially memory mapped) file.)
 *
 * \return Number of bytes written.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_to_encoding_n( void *pDstBuf, axstr_size_t cDstBytes, const char *pszSrc, axstr_size_t cSrcBytes, axstr_encoding_t enc, axstr_byteordermark_t bom )
#if AXSTR_IMPLEMENT
{
	const axstr_utf8_t *s;
	const axstr_utf8_t *e;
	axstr_size_t n;
	void *d;

	n = axstr_write_bom( pDstBuf, cDstBytes, enc, bom );

	d = ( void * )( ( unsigned char * )pDstBuf + n );
	n = cDstBytes - n;

	s = ( const axstr_utf8_t * )pszSrc;
	e = s + ( cSrcBytes == AXSTR_UNKNOWN_LENGTH ? axstr_len( pszSrc ) : cSrcBytes );

	if( enc == axstr_enc_utf8 ) {
		if( !axstr_stream_bytes_( &d, &n, ( const void * )s, ( axstr_size_t )( e - s ) ) ) {
			return 0;
		}

		return cDstBytes - n;
	}

	do {
		axstr_utf32_t cp;
		unsigned char bytes[ 4 ];
		axstr_size_t nbytes;

		cp = axstr_step_utf8_decode( &s, e );

		nbytes = 0;

		if( enc == axstr_enc_utf16_be || enc == axstr_enc_utf16_le ) {
			axstr_utf16_t utf16[ 2 ];

			if( cp < 0x10000 ) {
				utf16[ 0 ] = ( axstr_utf16_t )cp;
				utf16[ 1 ] = 0;
				nbytes = 2;
			} else {
				utf16[ 0 ] = 0xD800 | ( axstr_utf16_t )( ( cp >> 10 ) & 0x3FF );
				utf16[ 1 ] = 0xDC00 | ( axstr_utf16_t )( ( cp >>  0 ) & 0x3FF );
				nbytes = 4;
			}

			if( enc == axstr_enc_utf16_be ) {
				bytes[ 0 ] = ( unsigned char )( ( utf16[0] & 0xFF00 )>>8 );
				bytes[ 1 ] = ( unsigned char )( ( utf16[0] & 0x00FF )>>0 );
				bytes[ 2 ] = ( unsigned char )( ( utf16[1] & 0xFF00 )>>8 );
				bytes[ 3 ] = ( unsigned char )( ( utf16[1] & 0x00FF )>>0 );
			} else {
				bytes[ 0 ] = ( unsigned char )( ( utf16[0] & 0x00FF )>>0 );
				bytes[ 1 ] = ( unsigned char )( ( utf16[0] & 0xFF00 )>>8 );
				bytes[ 2 ] = ( unsigned char )( ( utf16[1] & 0x00FF )>>0 );
				bytes[ 3 ] = ( unsigned char )( ( utf16[1] & 0xFF00 )>>8 );
			}
		} else if( enc == axstr_enc_utf32_be ) {
			bytes[ 0 ] = ( unsigned char )( ( cp & 0xFF000000 )>>24 );
			bytes[ 1 ] = ( unsigned char )( ( cp & 0x00FF0000 )>>16 );
			bytes[ 2 ] = ( unsigned char )( ( cp & 0x0000FF00 )>> 8 );
			bytes[ 3 ] = ( unsigned char )( ( cp & 0x000000FF )>> 0 );
			nbytes = 4;
		} else {
			bytes[ 0 ] = ( unsigned char )( ( cp & 0x000000FF )>> 0 );
			bytes[ 1 ] = ( unsigned char )( ( cp & 0x0000FF00 )>> 8 );
			bytes[ 2 ] = ( unsigned char )( ( cp & 0x00FF0000 )>>16 );
			bytes[ 3 ] = ( unsigned char )( ( cp & 0xFF000000 )>>24 );
			nbytes = 4;
		}

		if( !axstr_stream_bytes_( &d, &n, &bytes[0], nbytes ) ) {
			return 0;
		}
	} while( s < e );

	return cDstBytes - n;
}
#else
;
#endif
/*!
 * \brief Convert a UTF-8 stream to an arbitrary binary encoding with an
 *        optional byte-order-mark.
 *
 * Does not explicitly write a `NUL`-terminator. (Useful when writing to a
 * (potentially memory mapped) file.)
 *
 * \return Number of bytes written.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_to_encoding( void *pDstBuf, axstr_size_t cDstBytes, const char *pszSrc, axstr_encoding_t enc, axstr_byteordermark_t bom )
#if AXSTR_IMPLEMENT
{
	return axstr_to_encoding_n( pDstBuf, cDstBytes, pszSrc, AXSTR_UNKNOWN_LENGTH, enc, bom );
}
#else
;
#endif

#if AXSTR_IMPLEMENT
/*! \internal
 *  \brief Returns nonzero if this machine is little-endian
 */
static int AXSTR_CALL axstr__is_le( void )
{
	union
	{
		unsigned char  x[ 2 ];
		unsigned short y;
	} v;

	v.y = 0x1122;
	return v.x[ 0 ] == 0x22;
}
/*! \internal
 *  \brief Returns true if the given encoding is not in native byte-order
 */
static int AXSTR_CALL axstr__is_native_byteorder( axstr_encoding_t enc )
{
	if( axstr__is_le() ) {
		return enc == axstr_enc_utf16_le || enc == axstr_enc_utf32_le;
	}

	return enc == axstr_enc_utf16_be || enc == axstr_enc_utf32_be;
}
/*! \internal
 *  \brief Perform a 32-bit byte-swap
 */
static axstr_utf32_t AXSTR_CALL axstr__bswap32( axstr_utf32_t v )
{
	return
		(
			( ( v&0x000000FF )<<24 ) |
			( ( v&0x0000FF00 )<< 8 ) |
			( ( v&0x00FF0000 )>> 8 ) |
			( ( v&0xFF000000 )>>24 )
		);
}
/*! \internal
 *  \brief Perform a 16-bit byte-swap
 */
static axstr_utf16_t AXSTR_CALL axstr__bswap16( axstr_utf16_t v )
{
	return
		(
			( ( v&0x00FF )<<8 ) |
			( ( v&0xFF00 )>>8 )
		);
}
#endif

/*!
 * \brief Convert a stream in an arbitrary binary encoding into a UTF-8 stream.
 *
 * Does not explicitly write a `NUL`-terminator. (Useful when writing to a
 * file.)
 *
 * If `enc` is `axstr_enc_unknown` then a BOM will be checked for. A BOM will
 * not be written out to the destination buffer.
 *
 * \return Number of bytes written, or the number of bytes that would have been written if `pDstBuf` weren't `nullptr`.
 */
AXSTR_FUNC axstr_size_t AXSTR_CALL axstr_from_encoding( void *pDstBuf, axstr_size_t cDstBytes, const void *pSrcBuf, axstr_size_t cSrcBytes, axstr_encoding_t enc )
#if AXSTR_IMPLEMENT
{
	axstr_size_t cBytesWritten = 0;
	axstr_utf8_t Temp[ 4 ];
	axstr_utf8_t *dstp;
	axstr_utf8_t *dste;

	/* Detect the encoding if not given */
	if( enc == axstr_enc_unknown && cSrcBytes >= 4 ) {
		axstr_size_t cBOMBytes;

		enc = axstr_detect_encoding( ( const char * )pSrcBuf, &cBOMBytes );

		pSrcBuf = ( const void * )( ( const char * )pSrcBuf + cBOMBytes );
		cSrcBytes -= cBOMBytes;
	}

	/* Do a straight copy if it's UTF-8 */
	if( enc == axstr_enc_utf8 || enc == axstr_enc_unknown ) {
		if( !pDstBuf ) {
			return cSrcBytes;
		}

		if( cSrcBytes > cDstBytes ) {
			return 0;
		}

		axstr__memcpy( pDstBuf, pSrcBuf, cSrcBytes );
		return cSrcBytes;
	}

	dstp = ( axstr_utf8_t * )pDstBuf;
	dste = ( axstr_utf8_t * )pDstBuf + cDstBytes;

	/* Handle 32-bit encoding */
	if( enc == axstr_enc_utf32_le || enc == axstr_enc_utf32_be ) {
		const axstr_utf32_t *p;
		const axstr_utf32_t *e;

		p = ( const axstr_utf32_t * )pSrcBuf;
		e = ( const axstr_utf32_t * )( ( const char * )pSrcBuf + cSrcBytes );

		/* Do a byte-swapped decode if needed */
		if( axstr__is_native_byteorder( enc ) ) {
			if( !pDstBuf ) {
				while( p < e ) {
					dstp = &Temp[ 0 ];
					dste = &Temp[ sizeof( Temp ) ];
					if( !axstr_step_utf8_encode( &dstp, dste, *p++ ) ) {
						break;
					}

					cBytesWritten += ( axstr_size_t )( dstp - &Temp[ 0 ] );
				}

				return cBytesWritten;
			}

			while( p < e ) {
				if( !axstr_step_utf8_encode( &dstp, dste, *p++ ) ) {
					break;
				}
			}

			return ( axstr_size_t )( dstp - ( axstr_utf8_t * )pDstBuf );
		} else {
			if( !pDstBuf ) {
				while( p < e ) {
					dstp = &Temp[ 0 ];
					dste = &Temp[ sizeof( Temp ) ];
					if( !axstr_step_utf8_encode( &dstp, dste, axstr__bswap32( *p++ ) ) ) {
						break;
					}

					cBytesWritten += ( axstr_size_t )( dstp - &Temp[ 0 ] );
				}

				return cBytesWritten;
			}

			while( p < e ) {
				if( !axstr_step_utf8_encode( &dstp, dste, axstr__bswap32( *p++ ) ) ) {
					break;
				}
			}

			return ( axstr_size_t )( dstp - ( axstr_utf8_t * )pDstBuf );
		}
	}

	/* Handle 16-bit encodings */
	if( enc == axstr_enc_utf16_le || enc == axstr_enc_utf16_be ) {
		const axstr_utf16_t *p;
		const axstr_utf16_t *e;
		axstr_utf32_t v;

		p = ( const axstr_utf16_t * )pSrcBuf;
		e = ( const axstr_utf16_t * )( ( const char * )pSrcBuf + cSrcBytes );

		/* Do a byte-swapped decode if needed */
		if( axstr__is_native_byteorder( enc ) ) {
			if( !pDstBuf ) {
				while( p < e ) {
					dstp = &Temp[ 0 ];
					dste = &Temp[ sizeof( Temp ) ];

					v = axstr_step_utf16_decode( &p, e );
					if( !axstr_step_utf8_encode( &dstp, dste, axstr_step_utf16_decode( &p, e ) ) ) {
						break;
					}

					cBytesWritten += ( axstr_size_t )( dstp - &Temp[ 0 ] );
				}

				return cBytesWritten;
			}

			while( p < e ) {
				v = axstr_step_utf16_decode( &p, e );
				if( !axstr_step_utf8_encode( &dstp, dste, v ) ) {
					break;
				}
			}

			return ( axstr_size_t )( dstp - ( axstr_utf8_t * )pDstBuf );
		} else {
			axstr_utf16_t SrcTemp[ 2 ];
			axstr_utf16_t *pSrcTemp;
			axstr_utf16_t *pSrcTempE;

#ifdef AX_STATIC_SUPPRESS
			AX_STATIC_SUPPRESS(6201)
			AX_STATIC_SUPPRESS(6305)
#endif
			pSrcTempE = &SrcTemp[ AXSTR_COUNTOF( SrcTemp ) ];

			if( !pDstBuf ) {
				while( p < e ) {
					dstp = &Temp[ 0 ];
					dste = &Temp[ sizeof( Temp ) ];

					SrcTemp[ 0 ] = axstr__bswap16( *p++ );
					SrcTemp[ 1 ] = p < e ? axstr__bswap16( *p ) : 0;
					pSrcTemp = &SrcTemp[ 0 ];

					v = axstr_step_utf16_decode( ( const axstr_utf16_t ** )&pSrcTemp, pSrcTempE );
					if( pSrcTemp == pSrcTempE ) {
						++p;
					}

					if( !axstr_step_utf8_encode( &dstp, dste, v ) ) {
						break;
					}

					cBytesWritten += ( axstr_size_t )( dstp - &Temp[ 0 ] );
				}

				return cBytesWritten;
			}

			while( p < e ) {
				SrcTemp[ 0 ] = axstr__bswap16( *p++ );
				SrcTemp[ 1 ] = p < e ? axstr__bswap16( *p ) : 0;
				pSrcTemp = &SrcTemp[ 0 ];

				v = axstr_step_utf16_decode( ( const axstr_utf16_t ** )&pSrcTemp, pSrcTempE );
				if( pSrcTemp == pSrcTempE ) {
					++p;
				}

				if( !axstr_step_utf8_encode( &dstp, dste, v ) ) {
					break;
				}
			}

			return ( axstr_size_t )( dstp - ( axstr_utf8_t * )pDstBuf );
		}
	}

	/* Unreachable */
	return 0;
}
#else
;
#endif

/*!
 * \brief  Retrieve the relative path from one file to another
 *
 * In the event of truncation a terminating `NUL` character will be placed as
 * the last byte within the buffer (within the bounds dictated by `cDstBytes`).
 *
 * \param  pszDstBuf  Destination buffer
 * \param  cDstBytes  Size of destination buffer in bytes, including NUL terminator
 * \param  pszSrcFrom The file with the base path
 * \param  pszSrcTo   The file you are getting to from pszSrcFrom
 * \return `pszDstBuf` on success; `nullptr` otherwise.
 */
AXSTR_FUNC const char *AXSTR_CALL axstr_relpath( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszSrcFrom, const char *pszSrcTo )
#if AXSTR_IMPLEMENT
{
	const char *srcfrom_p, *srcto_p;
	const char *srcfrom_q, *srcto_q; /* last matching directory */
	char *dstp;
	axstr_size_t dstn;

	/* ignore bad inputs */
	if( !pszDstBuf || cDstBytes <= 1 || !pszSrcFrom || !pszSrcTo ) {
		return ( const char * )0;
	}

	/* skip all matching characters */
	srcfrom_p = pszSrcFrom;
	srcfrom_q = pszSrcFrom;
	srcto_p = pszSrcTo;
	srcto_q = pszSrcTo;
	while( axstr_cmppathchar( *srcfrom_p, *srcto_p ) ) {
		if( axstr_ispathsep( *srcfrom_p ) ) {
			srcfrom_q = srcfrom_p + 1;
			srcto_q = srcto_p + 1;
		}

		++srcfrom_p;
		++srcto_p;
	}

	/* respect directories passed in without a trailing '/' */
	if( *srcfrom_p == '\0' && axstr_ispathsep( *srcto_p ) ) {
		/* we're at a directory */
		++srcto_p;
	} else {
		/* reset to last directory */
		srcfrom_p = srcfrom_q;
		srcto_p = srcto_q;
	}

	/* no matching characters means "to" is absolute */
	if( srcfrom_p == pszSrcFrom ) {
		axstr_cpy( pszDstBuf, cDstBytes, pszSrcTo );
		return pszDstBuf;
	}

	dstp = pszDstBuf;
	dstn = cDstBytes;

	*dstp = '\0';

	/* find how many directories up "from" is from "to" */
	while( *srcfrom_p != '\0' ) {
		if( axstr_ispathsep( *srcfrom_p ) ) {
			while( axstr_ispathsep( *( srcfrom_p + 1 ) ) ) {
				++srcfrom_p;
			}

			axstr_stream( &dstp, &dstn, ".." AXSTR_DIRSEP );
		}

		++srcfrom_p;
	}

	/* append the rest of the "to" string */
	axstr_stream( &dstp, &dstn, srcto_p );
	return pszDstBuf;
}
#else
;
#endif

#if AXSTR_ABSPATH_ENABLED

/*!
 * \brief Retrieve the absolute path of a file
 */
AXSTR_FUNC const char *AXSTR_CALL axstr_abspath( char *pszDstBuf, axstr_size_t cDstBytes, const char *pszRelPath )
#if AXSTR_IMPLEMENT
{
	if( !pszDstBuf || cDstBytes <= 1 || !pszRelPath ) {
		return ( const char * )0;
	}

	*pszDstBuf = '\0';
# ifdef _WIN32
	static axstr_utf16_t wszDstBuf[ AXSTR_MAX_PATH ];
	static axstr_utf16_t wszSrcBuf[ AXSTR_MAX_PATH ];

	if( !axstr_utf8_to_utf16( &wszSrcBuf[ 0 ], AXSTR_MAX_PATH, ( const axstr_utf8_t * )pszRelPath ) ) {
		return ( const char * )0;
	}
	GetFullPathNameW( ( LPCWSTR )wszSrcBuf, ( DWORD )AXSTR_MAX_PATH, ( LPWSTR )wszDstBuf, ( LPWSTR * )0 );
	if( !axstr_utf16_to_utf8( ( axstr_utf8_t * )pszDstBuf, cDstBytes, wszDstBuf ) ) {
		return ( const char * )0;
	}

	return pszDstBuf;
# else
	if( cDstBytes < AXSTR_MAX_PATH ) {
		char szTmpBuf[ AXSTR_MAX_PATH ];
		realpath( pszRelPath, szTmpBuf );
		axstr_cpy( pszDstBuf, cDstBytes, szTmpBuf );
	} else {
		realpath( pszRelPath, pszDstBuf );
	}

	return pszDstBuf;
# endif
}
#else
;
#endif
#endif /* AXSTR_ABSPATH_ENABLED */

/*!
 * \brief Search for a single character in a string
 */
AXSTR_FUNC char *AXSTR_CALL axstr_findchr( const char *pszText, axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t utf8[ 5 ];
	axstr_utf8_t *utf8p = &utf8[ 0 ];
	axstr_utf8_t *utf8e = &utf8[ 5 ];

# if AXSTR_STDSTR_ENABLED
	if( ch <= 0x7F ) {
		return ( char * )strchr( pszText, ch );
	}

	if( !axstr_step_utf8_encode( &utf8p, utf8e, ch ) ) {
		return ( char * )0;
	}

	*utf8p = '\0';

	return ( char * )strstr( pszText, ( const char * )utf8 );
# else
	const axstr_utf8_t *p;
	const axstr_utf8_t *q;

	if( !axstr_step_utf8_encode( &utf8p, utf8e, ch ) ) {
		return ( char * )0;
	}

	*utf8p = '\0';
	utf8e = utf8p;
	utf8p = &utf8[0];

	p = pszText;
	while( *p != '\0' ) {
		/* find the first matching UTF-8 byte */
		while( *p != *utf8p ) {
			/* found end of input string before match */
			if( *p == '\0' ) {
				return ( char * )0;
			}

			++p;
		}

		/* if this is a match then this is the value we need to return */
		q = p;

		/* compare p to utf8 */
		do {
			/* if incrementing would bring us to the end of the utf-8 string then we found a match */
			if( utf8p + 1 == utf8e ) {
				return ( char * )q;
			}

			++p;
			++utf8p;
		} while( *p == *utf8p );

		/* reset utf8p to base of array (no match found) */
		utf8p = &utf8[0];
	}

	return ( char * )0;
# endif
}
#else
;
#endif

/*!
 * \brief Search for a string of a specific length
 */
AXSTR_FUNC char *AXSTR_CALL axstr_findstrn( const char *pszText, const char *pszFind, axstr_size_t cFindBytes )
#if AXSTR_IMPLEMENT
{
	const char *p;
	const char *q;

	q = pszText;

# if AXSTR_STDSTR_ENABLED
	do {
		p = strchr( q, *pszFind );
		if( !p ) {
			break;
		}

		q = p + 1;
	} while( strncmp( pszFind, p, cFindBytes ) != 0 );
# else
	do {
		p = axstr_findchr( q, *pszFind );
		if( !p ) {
			break;
		}

		q = p + 1;
	} while( !axstr_cmpn( pszFind, p, cFindBytes ) );
# endif

	return ( char * )p;
}
#else
;
#endif
/*!
 * \brief Search for a substring within a string
 */
AXSTR_FUNC char *AXSTR_CALL axstr_findstr( const char *pszText, const char *pszFind )
#if AXSTR_IMPLEMENT
{
# if AXSTR_STDSTR_ENABLED
	return ( char * )strstr( pszText, pszFind );
# else
	return ( char * )axstr_findstrn( pszText, pszFind, axstr_len( pszFind ) );
# endif
}
#else
;
#endif

/*!
 * \brief Search for the longest match of a string
 */
AXSTR_FUNC const char *AXSTR_CALL axstr_longest_match( const char *pszText, const char *pszFind, axstr_size_t cFindBytes, axstr_size_t *pcOutMatchLen )
#if AXSTR_IMPLEMENT
{
	/*
	 *  NOTE: This can be significantly optimized but should be fine for small
	 *  -     one-off tools
	 */

	axstr_size_t cTestBytes;
	const char *p;

	cTestBytes = cFindBytes;
	while( cTestBytes > 0 ) {
		p = axstr_findstrn( pszText, pszFind, cTestBytes );
		if( p != ( const char * )0 ) {
			if( pcOutMatchLen != ( axstr_size_t * )0 ) {
				*pcOutMatchLen = cTestBytes;
			}

			return p;
		}

		--cTestBytes;
	}

	return ( const char * )0;
}
#else
;
#endif

/*!
 * \brief Search for the longest common contiguous text chunk in a set of
 *        strings.
 *
 * e.g., "`CEntity`," "`IEntity`," "`CEntity_Pivot`" would yield "`Entity`".
 *
 * \note "`CEntity_Local`," "`CEntity_Vehicle`," "`CEntity_Player`" would yield
 *       "`CEntity_`" as the longest match.
 */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_find_common( char *pszDstBuf, axstr_size_t cDstBytes, const char *const *ppszTexts, axstr_size_t cTexts )
#if AXSTR_IMPLEMENT
{
	axstr_size_t cBestMatch;
	axstr_size_t uBestMatch;
	axstr_bool_t bMatchFound;
	axstr_size_t cLen;
	axstr_size_t cTestMatch;
	axstr_size_t cMatchLen;
	axstr_size_t i, j;
	const char *p;

	cBestMatch = 0;
	uBestMatch = ~( axstr_size_t )0;
	bMatchFound = 0;

	cLen = axstr_len( ppszTexts[ 0 ] );
	for( i = 0; ppszTexts[ 0 ][ i ] != '\0'; ++i ) {
		cTestMatch = ~( axstr_size_t )0;

		for( j = 1; j < cTexts; ++j ) {
			p = axstr_longest_match( &ppszTexts[ 0 ][ i ], ppszTexts[ j ], cLen - i, &cMatchLen );
			if( !p ) {
				break;
			}

			if( cMatchLen < cTestMatch ) {
				cTestMatch = cMatchLen;
			}
		}

		if( j == cTexts ) {
			if( cTestMatch > cBestMatch ) {
				cBestMatch = cTestMatch;
				uBestMatch = i;
			}

			bMatchFound = 1;
		}
	}

	if( !bMatchFound ) {
		*pszDstBuf = '\0';
		return 0;
	}

	axstr_cpyn( pszDstBuf, cDstBytes, &ppszTexts[ 0 ][ uBestMatch ], cBestMatch );
	return 1;
}
#else
;
#endif

/*
 *  FIXME: These should check Unicode characters outside of the ASCII range too
 */

/*! \brief Check if `ch` is a whitespace character. */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_iswhite( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( ch <= ' ' );
}
#else
;
#endif
/*! \brief Check if `ch` is a lowercase letter (`^a-z$`). */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_islower( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( ch >= 'a' && ch <= 'z' );
}
#else
;
#endif
/*! \brief Check if `ch` is an uppercase letter (`^A-Z$`). */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isupper( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( ch >= 'A' && ch <= 'Z' );
}
#else
;
#endif
/*! \brief Check if `ch` is an English alphabet letter (`^a-zA-Z$`). */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isalpha( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( axstr_islower( ch ) || axstr_isupper( ch ) );
}
#else
;
#endif
/*! \brief Check if `ch` is an English alphabet letter or digit
 *         (`^a-zA-Z0-9$`). */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isalnum( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( axstr_isalpha( ch ) || ( ch>='0' && ch<='9' ) );
}
#else
;
#endif
/*! \brief Check if `ch` is a miscellaneous ASCII identifier character. */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isnamemisc( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( ch == '_' );
}
#else
;
#endif
/*! \brief Check if `ch` is a valid starting character for an ASCII
 *         identifier. */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isnamestart( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( axstr_isalpha( ch ) || axstr_isnamemisc( ch ) );
}
#else
;
#endif
/*! \brief Check if `ch` is a valid continuation character for an ASCII
 *         identifier. */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isname( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( axstr_isalnum( ch ) || axstr_isnamemisc( ch ) );
}
#else
;
#endif

#if AXSTR_IMPLEMENT
/*! \internal */
struct axstr__unicoderange_s
{
	axstr_utf32_t s;
	axstr_utf32_t e;
};
#endif

/*! \brief Check if `ch` is a valid starting character for an Apple Swift-style
 *         identifier. */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isunicodenamestart( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	static const struct axstr__unicoderange_s ranges[] = {
		{ 0x00B2, 0x00B5 }, { 0x00B7, 0x00BA },
		{ 0x00BC, 0x00BE }, { 0x00C0, 0x00D6 }, { 0x00D8, 0x00F6 }, { 0x00F8, 0x00FF },
		{ 0x0100, 0x02FF }, { 0x0370, 0x167F }, { 0x1681, 0x180D }, { 0x180F, 0x1DBF },
		{ 0x1E00, 0x1FFF },
		{ 0x200B, 0x200D }, { 0x202A, 0x202E }, { 0x203F, 0x2040 }, { 0x2060, 0x20CF },
		{ 0x2100, 0x218F }, { 0x2460, 0x24FF }, { 0x2776, 0x2793 },
		{ 0x2C00, 0x2DFF }, { 0x2E80, 0x2FFF },
		{ 0x3004, 0x3007 }, { 0x3021, 0x302F }, { 0x3031, 0xD7FF },
		{ 0xF900, 0xFD3D }, { 0xFD40, 0xFDCF }, { 0xFDF0, 0xFE1F }, { 0xFE30, 0xFE44 },
		{ 0xFE47, 0xFFFD },
		{ 0x10000, 0x1FFFD }, { 0x20000, 0x2FFFD }, { 0x30000, 0x3FFFD }, { 0x40000, 0x4FFFD },
		{ 0x50000, 0x5FFFD }, { 0x60000, 0x6FFFD }, { 0x70000, 0x7FFFD }, { 0x80000, 0x8FFFD },
		{ 0x90000, 0x9000D }, { 0xA0000, 0xAFFFD }, { 0xB0000, 0xBFFFD }, { 0xC0000, 0xCFFFD },
		{ 0xD0000, 0xDFFFD }, { 0xE0000, 0xEFFFD }
	};
	axstr_size_t i;

	if( ch == 0x00A8 || ch == 0x00AA || ch == 0x00AD || ch == 0x00AF || ch == 0x2054 ) {
		return 1;
	}

	for( i = 0; i < sizeof(ranges)/sizeof(ranges[0]); ++i ) {
		if( ch >= ranges[i].s && ch <= ranges[i].e ) {
			return 1;
		}
	}

	return 0;
}
#else
;
#endif
/*! \brief Check if `ch` is a valid continuation character for an Apple
 *         Swift-style identifier. */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isunicodename( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	static const struct axstr__unicoderange_s ranges[] = {
		{ 0x0300, 0x036F }, { 0x1DC0, 0x1DFF }, { 0x20D0, 0x20FF }, { 0xFE20, 0xFE2F }
	};
	axstr_size_t i;

	if( axstr_isunicodenamestart( ch ) ) {
		return 1;
	}

	for( i = 0; i < sizeof(ranges)/sizeof(ranges[0]); ++i ) {
		if( ch >= ranges[i].s && ch <= ranges[i].e ) {
			return 1;
		}
	}

	return 0;
}
#else
;
#endif

/*! \brief Convert an uppercase letter to a lowercase letter. */
AXSTR_FUNC axstr_utf32_t AXSTR_CALL axstr_tolower( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( ch >= 'A' && ch <= 'Z' ) ? ch - 'A' + 'a' : ch;
}
#else
;
#endif
/*! \brief Convert a lowercase letter to an uppercase letter. */
AXSTR_FUNC axstr_utf32_t AXSTR_CALL axstr_toupper( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return ( ch >= 'a' && ch <= 'z' ) ? ch - 'a' + 'A' : ch;
}
#else
;
#endif

#if AXSTR_IMPLEMENT
/*! \internal */
static const char *axstr__plus( const char *p, const char *e, axstr_size_t n )
{
	if( e != ( const char * )0 && ( p + n ) >= e ) {
		return ( const char * )0;
	}

	return p + n;
}
/*! \internal */
static char axstr__deref( const char *p )
{
	if( !p ) {
		return '\0';
	}

	return *p;
}
#endif

/*! \brief Get a pointer to the first non-whitespace character in a range. */
AXSTR_FUNC const char *AXSTR_CALL axstr_skip_whitespace_e( const char *p, const char *e )
#if AXSTR_IMPLEMENT
{
	axstr_utf8_t ch;
	for(;;) {
		ch = ( axstr_utf8_t )axstr__deref( p );
		if( ch > ' ' || !ch ) {
			break;
		}

		p = axstr__plus( p, e, 1 );
	}

	return p;
}
#else
;
#endif
/*! \brief Get a pointer to the first non-whitespace character in a
 *         `NUL`-terminated string. */
AXSTR_FUNC const char *AXSTR_CALL axstr_skip_whitespace( const char *p )
#if AXSTR_IMPLEMENT
{
	while( axstr_iswhite( *( const axstr_utf8_t * )p ) && *p != '\0' ) {
		++p;
	}

	return p;
}
#else
;
#endif

/*! \brief Get a pointer to the first byte of the next line in a range. */
AXSTR_FUNC const char *AXSTR_CALL axstr_skip_line_e( const char *p, const char *e )
#if AXSTR_IMPLEMENT
{
	while( p < e ) {
		if( *p == '\r' ) {
			++p;

			return p < e && *p == '\n' ? ++p : p;
		}

		if( *p++ == '\n' ) {
			return p;
		}
	}

	return p;
}
#else
;
#endif
/*! \brief Get a pointer to the first byte of the next line in a
 *         `NUL`-terminated string. */
AXSTR_FUNC const char *AXSTR_CALL axstr_skip_line( const char *p )
#if AXSTR_IMPLEMENT
{
	while( *p != '\0' ) {
		if( *p == '\r' ) {
			++p;
			return *p == '\n' ? ++p : p;
		}

		if( *p++ == '\n' ) {
			return p;
		}
	}

	return p;
}
#else
;
#endif

/*! \brief Convert a character representing a given digit to a number, or -1 if
 *         the character is not part of the base specified by `radix`. */
AXSTR_FUNC int AXSTR_CALL axstr_digit( char ch, unsigned int radix )
#if AXSTR_IMPLEMENT
{
	int radixlo, radixhi;

	if( radix < 2 || radix > 36 ) {
		return -1;
	}

	radixlo = radix > 10 ? 10 : radix;
	radixhi = radix > 10 ? radix - 10 : 0;

	if( ch >= '0' && ch < '0' + radixlo ) {
		return ( int )( ch - '0' );
	}
	if( ch >= 'a' && ch < 'a' + radixhi ) {
		return 10 + ( int )( ch - 'a' );
	}
	if( ch >= 'A' && ch < 'A' + radixhi ) {
		return 10 + ( int )( ch - 'A' );
	}

	return -1;
}
#else
;
#endif
/*! \brief Determine whether a character represents a valid digit of the given
 *         base. */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isdigit_x( char ch, unsigned int radix )
#if AXSTR_IMPLEMENT
{
	return ( axstr_bool_t )( axstr_digit( ch, radix ) != -1 );
}
#else
;
#endif
/*! \brief Determine whether a character represents a valid base-10 digit. */
AXSTR_FUNC axstr_bool_t AXSTR_CALL axstr_isdigit( axstr_utf32_t ch )
#if AXSTR_IMPLEMENT
{
	return axstr_isdigit_x( ch, AXSTR_DEFAULT_RADIX );
}
#else
;
#endif

AXSTR_FUNC unsigned int AXSTR_CALL axstr_decode_radix_e( const char **ppszNumber, const char *pEnd, int defaultRadix )
#if AXSTR_IMPLEMENT
{
	const char *p;
	unsigned int radix = 10;

	p = axstr__plus( *ppszNumber, pEnd, 0 );
	if( !p ) {
		return 0;
	}

	if( defaultRadix == axstr_radix_c ) {
		if( axstr__deref( p ) == '0' ) {
			const char ch = axstr__deref( axstr__plus( p, pEnd, 1 ) );

			if( ch == 'x' || ch == 'X' ) {
				radix = 16;
				p = axstr__plus( p, pEnd, 2 );
			} else if( ch == 'b' || ch == 'B' ) {
				radix = 2;
				p = axstr__plus( p, pEnd, 2 );
			} else if( ch != '.' ) {
				radix = 8;
				p = axstr__plus( p, pEnd, 1 );
			}
		}
	} else if( defaultRadix == axstr_radix_basic ) {
		if( axstr__deref( p ) == '%' ) {
			radix = 2;
			p = axstr__plus( p, pEnd, 1 );
		} else if( axstr__deref( p ) == '$' ) {
			radix = 16;
			p = axstr__plus( p, pEnd, 1 );
		} else if( axstr__deref( p ) == '0' ) {
			const char ch = axstr__deref( axstr__plus( p, pEnd, 1 ) );

			if( ch == 'b' || ch == 'B' ) {
				radix = 2;
				p = axstr__plus( p, pEnd, 2 );
			} else if( ch == 'c' || ch == 'C' ) {
				radix = 8;
				p = axstr__plus( p, pEnd, 2 );
			} else if( ch == 'd' || ch == 'D' ) {
				radix = 10;
				p = axstr__plus( p, pEnd, 2 );
			} else if( ch == 'h' || ch == 'H' || ch == 'x' || ch == 'X' ) {
				radix = 16;
				p = axstr__plus( p, pEnd, 2 );
			}
		} else {
			unsigned int testradix = 0;
			const char *check;
			char checkch;

			check = p;
			while( check != ( const char * )0 && *check >= '0' && *check <= '9' ) {
				testradix *= 10;
				testradix += +( *check - '0' );
				check = axstr__plus( check, pEnd, 1 );
			}

			checkch = axstr__deref( check );
			if( ( checkch == 'x' || checkch == 'X' ) && testradix >= 2 && testradix <= 36 ) {
				radix = testradix;
				p = axstr__plus( check, pEnd, 1 );
			}
		}
	} else {
		radix = defaultRadix;
	}

	*ppszNumber = ( const char * )p;
	return radix;
}
#else
;
#endif
AXSTR_FUNC unsigned int AXSTR_CALL axstr_decode_radix( const char **ppszNumber, int defaultRadix )
#if AXSTR_IMPLEMENT
{
	return axstr_decode_radix_e( ppszNumber, ( const char * )0, defaultRadix );
}
#else
;
#endif

#if AXSTR_IMPLEMENT
static axstr_sint_t AXSTR_CALL axstr_parse_sign_part_( const char **ppszNumber, const char *pEnd )
{
	char ch;

	ch = axstr__deref( *ppszNumber );
	if( ch == '-' ) {
		*ppszNumber = axstr__plus( *ppszNumber, pEnd, 1 );
		return ( axstr_sint_t )-1;
	} else if( ch == '+' ) {
		*ppszNumber = axstr__plus( *ppszNumber, pEnd, 1 );
	}

	return ( axstr_sint_t )1;
}

static axstr_uint_t AXSTR_CALL axstr_parse_uint_part_( const char **ppszNumber, const char *pEnd, unsigned int radix, char chDigitSep )
{
	axstr_uint_t n;
	const char *p;

	n = 0;
	p = *ppszNumber;

	while( axstr__deref( p ) != '\0' ) {
		const int digit = axstr_digit( *p == chDigitSep && *p != '\0' ? *( p + 1 ) : *p, radix );
		if( digit < 0 ) {
			break;
		}

		if( *p == chDigitSep ) {
			p = axstr__plus( p, pEnd, 2 );
		} else {
			p = axstr__plus( p, pEnd, 1 );
		}

		n *= radix;
		n += ( axstr_uint_t )digit;
	}

	*ppszNumber = ( const char * )p;

	return n;
}
#endif

AXSTR_FUNC axstr_uint_t AXSTR_CALL axstr_to_uint_xe( const char **ppszNumber, const char *pEnd, int defaultRadix, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	unsigned int radix;

	*ppszNumber = axstr_skip_whitespace_e( *ppszNumber, pEnd );
	radix = axstr_decode_radix_e( ppszNumber, pEnd, defaultRadix );

	return axstr_parse_uint_part_( ppszNumber, pEnd, radix, chDigitSep );
}
#else
;
#endif
AXSTR_FUNC axstr_uint_t AXSTR_CALL axstr_to_uint_x( const char **ppszNumber, int defaultRadix, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	return axstr_to_uint_xe( ppszNumber, ( const char * )0, defaultRadix, chDigitSep );
}
#else
;
#endif
AXSTR_FUNC axstr_sint_t AXSTR_CALL axstr_to_int_xe( const char **ppszNumber, const char *pEnd, int defaultRadix, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	unsigned int radix;
	axstr_sint_t m;

	*ppszNumber = axstr_skip_whitespace_e( *ppszNumber, pEnd );
	radix = axstr_decode_radix_e( ppszNumber, pEnd, defaultRadix );

	m = axstr_parse_sign_part_( ppszNumber, pEnd );
	*ppszNumber = axstr_skip_whitespace_e( *ppszNumber, pEnd );

	return m*( axstr_sint_t )axstr_parse_uint_part_( ppszNumber, pEnd, radix, chDigitSep );
}
#else
;
#endif
AXSTR_FUNC axstr_sint_t AXSTR_CALL axstr_to_int_x( const char **ppszNumber, int defaultRadix, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	return axstr_to_int_xe( ppszNumber, ( const char * )0, defaultRadix, chDigitSep );
}
#else
;
#endif
AXSTR_FUNC axstr_real_t AXSTR_CALL axstr_to_float_xe( const char **ppszNumber, const char *pEnd, int defaultRadix, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	const char *p;
	unsigned int radix;
	axstr_sint_t m;
	axstr_uint_t whole;
	axstr_uint_t fract;
	axstr_sint_t esign;
	axstr_uint_t e;
	axstr_real_t f;
	axstr_uint_t fractmag;
	axstr_uint_t tmpfract;
	char ch;

	p = axstr_skip_whitespace_e( *ppszNumber, pEnd );
	radix = axstr_decode_radix_e( &p, pEnd, defaultRadix );

	m = axstr_parse_sign_part_( &p, pEnd );
	p = axstr_skip_whitespace_e( p, pEnd );

	if( !p || ( *p == '.' && axstr_digit( axstr__deref( axstr__plus( p, pEnd, 1 ) ), radix ) == -1 ) ) {
		return ( axstr_real_t )0;
	}

	whole = axstr_parse_uint_part_( &p, pEnd, radix, chDigitSep );
	if( axstr__deref( p ) == '.' ) {
		p = axstr__plus( p, pEnd, 1 );
		fract = axstr_parse_uint_part_( &p, pEnd, radix, chDigitSep );
	} else {
		fract = 0;
	}

	esign = 1;
	e = 0;
	ch = axstr__deref( p );
	if( ch == 'e' || ch == 'p' || ch == 'E' || ch == 'P' ) {
		p = axstr__plus( p, pEnd, 1 );
		esign = axstr_parse_sign_part_( &p, pEnd );
		e = axstr_parse_uint_part_( &p, pEnd, radix, chDigitSep );
	}

	*ppszNumber = p;

	f = ( axstr_real_t )whole;

	fractmag = 1;
	tmpfract = fract;
	while( tmpfract > 0 ) {
		fractmag *= radix;
		tmpfract /= radix;
	}

	f += ( ( axstr_real_t )fract )/( axstr_real_t )fractmag;
	if( esign > 0 ) {
		while( e > 0 ) {
			f *= 10;
			--e;
		}
	} else {
		while( e > 0 ) {
			f /= 10;
			--e;
		}
	}

	return f*( axstr_real_t )m;
}
#else
;
#endif
AXSTR_FUNC axstr_real_t AXSTR_CALL axstr_to_float_x( const char **ppszNumber, int defaultRadix, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	return axstr_to_float_xe( ppszNumber, ( const char * )0, defaultRadix, chDigitSep );
}
#else
;
#endif

AXSTR_FUNC axstr_uint_t AXSTR_CALL axstr_to_uint_e( const char *pszNumber, const char *pEnd )
#if AXSTR_IMPLEMENT
{
	return axstr_to_uint_xe( &pszNumber, pEnd, AXSTR_DEFPARSE_RADIX, AXSTR_DIGITSEP_CH );
}
#else
;
#endif
AXSTR_FUNC axstr_uint_t AXSTR_CALL axstr_to_uint( const char *pszNumber )
#if AXSTR_IMPLEMENT
{
	return axstr_to_uint_x( &pszNumber, AXSTR_DEFPARSE_RADIX, AXSTR_DIGITSEP_CH );
}
#else
;
#endif
AXSTR_FUNC axstr_sint_t AXSTR_CALL axstr_to_int_e( const char *pszNumber, const char *pEnd )
#if AXSTR_IMPLEMENT
{
	return axstr_to_int_xe( &pszNumber, pEnd, AXSTR_DEFPARSE_RADIX, AXSTR_DIGITSEP_CH );
}
#else
;
#endif
AXSTR_FUNC axstr_sint_t AXSTR_CALL axstr_to_int( const char *pszNumber )
#if AXSTR_IMPLEMENT
{
	return axstr_to_int_x( &pszNumber, AXSTR_DEFPARSE_RADIX, AXSTR_DIGITSEP_CH );
}
#else
;
#endif
AXSTR_FUNC axstr_real_t AXSTR_CALL axstr_to_float_e( const char *pszNumber, const char *pEnd )
#if AXSTR_IMPLEMENT
{
	return axstr_to_float_xe( &pszNumber, pEnd, AXSTR_DEFPARSE_RADIX, AXSTR_DIGITSEP_CH );
}
#else
;
#endif
AXSTR_FUNC axstr_real_t AXSTR_CALL axstr_to_float( const char *pszNumber )
#if AXSTR_IMPLEMENT
{
	return axstr_to_float_x( &pszNumber, AXSTR_DEFPARSE_RADIX, AXSTR_DIGITSEP_CH );
}
#else
;
#endif

#if AXSTR_IMPLEMENT
static const char *const g_axstr_pszUpperDigits_ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
# if 0
static const char *const g_axstr_pszLowerDigits_ = "0123456789abcdefghijklmnopqrstuvwxyz";
# endif
#endif

AXSTR_FUNC char *AXSTR_CALL axstr_from_uint_x( char *pszDstBuf, axstr_size_t cDstBytes, axstr_uint_t value, unsigned int radix, unsigned int numDigitsForSep, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	char szTmpBuf[ 64 ];
	char *p;
	unsigned int cRemainingDigitsForSep;
	axstr_uint_t n;

	if( cDstBytes < 2 ) {
		return ( char * )0;
	}

	if( value == 0 ) {
		axstr_cpy( pszDstBuf, cDstBytes, "0" );
		return pszDstBuf;
	}

	if( radix < 2 || radix > 36 ) {
		return ( char * )0;
	}

	cRemainingDigitsForSep = numDigitsForSep > 0 ? numDigitsForSep : ~( unsigned int )0;
	p = &szTmpBuf[ sizeof( szTmpBuf ) - 1 ];
	*p = '\0';

	n = value;
	while( n > 0 ) {
		if( cRemainingDigitsForSep-- == 0 ) {
			cRemainingDigitsForSep = numDigitsForSep;
			*--p = chDigitSep;
		}

		*--p = g_axstr_pszUpperDigits_[ n%radix ];
		n /= radix;
	}

	axstr_cpy( pszDstBuf, cDstBytes, p );
	return pszDstBuf;
}
#else
;
#endif

AXSTR_FUNC char *AXSTR_CALL axstr_from_int_x( char *pszDstBuf, axstr_size_t cDstBytes, axstr_sint_t value, unsigned int radix, unsigned int numDigitsForSep, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	axstr_size_t n;
	axstr_uint_t v;
	char *p;

	if( cDstBytes < 3 ) {
		return ( char * )0;
	}

	p = pszDstBuf;
	n = cDstBytes;
	if( value < 0 ) {
		v = ( axstr_uint_t )-value;
		*p++ = '-';
		--n;
	} else {
		v = ( axstr_uint_t )value;
	}

	if( !axstr_from_uint_x( p, n, v, radix, numDigitsForSep, chDigitSep ) ) {
		return ( char * )0;
	}

	return pszDstBuf;
}
#else
;
#endif
AXSTR_FUNC char *AXSTR_CALL axstr_from_float_x( char *pszDstBuf, axstr_size_t cDstBytes, axstr_real_t value, unsigned int radix, unsigned int maxTrailingDigits, unsigned int numDigitsForSep, char chDigitSep )
#if AXSTR_IMPLEMENT
{
	static const unsigned int kMaxDigitsBuf = 255;
	unsigned int maxDigits;
	axstr_sint_t whole;
	axstr_uint_t fract;
	axstr_real_t f;
	char digitsBuf[ kMaxDigitsBuf + 1 ];

	if( cDstBytes < 4 ) {
		return ( char * )0;
	}
	if( radix < 2 || radix > 36 ) {
		return ( char * )0;
	}

	maxDigits = maxTrailingDigits > kMaxDigitsBuf ? kMaxDigitsBuf : maxTrailingDigits;

	whole = (axstr_sint_t)value;
	fract = 0;

	f = value - (axstr_real_t)whole;
	while( f > (axstr_real_t)0.0 ) {
		fract *= radix;

		f *= radix;
		fract += (ax_uint_t)( f*radix );

		f = f - (ax_uint_t)f;
	}

	if( !axstr_from_int_x( pszDstBuf, cDstBytes, whole, radix, numDigitsForSep, chDigitSep ) ) {
		return ( char * )0;
	}
	if( !axstr_from_uint_x( digitsBuf, sizeof( digitsBuf ), fract, radix, 0, '\0' ) ) {
		*pszDstBuf = '\0';
		return ( char * )0;
	}
	digitsBuf[ maxDigits ] = '\0';

	if( maxDigits > 0 ) {
		axstr_cat( pszDstBuf, cDstBytes, "." );
		axstr_cat( pszDstBuf, cDstBytes, digitsBuf );
	}

	return pszDstBuf;
}
#else
;
#endif

AXSTR_FUNC char *AXSTR_CALL axstr_from_uint( char *pszDstBuf, axstr_size_t cDstBytes, axstr_uint_t value )
#if AXSTR_IMPLEMENT
{
	return axstr_from_uint_x( pszDstBuf, cDstBytes, value, AXSTR_DEFAULT_RADIX, AXSTR_DIGITSEP_SPACING, AXSTR_DIGITSEP_CH );
}
#else
;
#endif
AXSTR_FUNC char *AXSTR_CALL axstr_from_int( char *pszDstBuf, axstr_size_t cDstBytes, axstr_sint_t value )
#if AXSTR_IMPLEMENT
{
	return axstr_from_int_x( pszDstBuf, cDstBytes, value, AXSTR_DEFAULT_RADIX, AXSTR_DIGITSEP_SPACING, AXSTR_DIGITSEP_CH );
}
#else
;
#endif
AXSTR_FUNC char *AXSTR_CALL axstr_from_float( char *pszDstBuf, axstr_size_t cDstBytes, axstr_real_t value )
#if AXSTR_IMPLEMENT
{
	return axstr_from_float_x( pszDstBuf, cDstBytes, value, AXSTR_DEFAULT_RADIX, AXSTR_FLOAT_TRAIL_DIGITS, AXSTR_DIGITSEP_SPACING, AXSTR_DIGITSEP_CH );
}
#else
;
#endif

#if AXSTR_IMPLEMENT
static const axstr_utf32_t g_axstr_katakana_conv_[] = {
	0x0000FF67, // ァ 30A1
	0x0000FF71, // ア 30A2
	0x0000FF68, // ィ 30A3
	0x0000FF72, // イ 30A4
	0x0000FF69, // ゥ 30A5
	0x0000FF73, // ウ 30A6
	0x0000FF6A, // ェ 30A7
	0x0000FF74, // エ 30A8
	0x0000FF6B, // ォ 30A9
	0x0000FF75, // オ 30AA
	0x0000FF76, // カ 30AB
	0xFF9EFF76, // ガ 30AC
	0x0000FF77, // キ 30AD
	0xFF9EFF77, // ギ 30AE
	0x0000FF78, // ク 30AF
	0xFF9EFF78, // グ 30B0
	0x0000FF79, // ケ 30B1
	0xFF9EFF79, // ゲ 30B2
	0x0000FF7A, // コ 30B3
	0xFF9EFF7A, // ゴ 30B4
	0x0000FF7B, // サ 30B5
	0xFF9EFF7B, // ザ 30B6
	0x0000FF7C, // シ 30B7
	0xFF9EFF7C, // ジ 30B8
	0x0000FF7D, // ス 30B9
	0xFF9EFF7D, // ズ 30BA
	0x0000FF7E, // セ 30BB
	0xFF9EFF7E, // ゼ 30BC
	0x0000FF7F, // ソ 30BD
	0xFF9EFF7F, // ゾ 30BE
	0x0000FF80, // タ 30BF
	0xFF9EFF80, // ダ 30C0
	0x0000FF81, // チ 30C1
	0xFF9EFF81, // ヂ 30C2
	0x0000FF6F, // ッ 30C3
	0x0000FF82, // ツ 30C4
	0xFF9EFF82, // ヅ 30C5
	0x0000FF83, // テ 30C6
	0xFF9EFF83, // デ 30C7
	0x0000FF84, // ト 30C8
	0xFF9EFF84, // ド 30C9
	0x0000FF85, // ナ 30CA
	0x0000FF86, // ニ 30CB
	0x0000FF87, // ヌ 30CC
	0x0000FF88, // ネ 30CD
	0x0000FF89, // ノ 30CE
	0x0000FF8A, // ハ 30CF
	0xFF9EFF8A, // バ 30D0
	0xFF9FFF8A, // パ 30D1
	0x0000FF8B, // ヒ 30D2
	0xFF9EFF8B, // ビ 30D3
	0xFF9FFF8B, // ピ 30D4
	0x0000FF8C, // フ 30D5
	0xFF9EFF8C, // ブ 30D6
	0xFF9FFF8C, // プ 30D7
	0x0000FF8D, // ヘ 30D8
	0xFF9EFF8D, // ベ 30D9
	0xFF9FFF8D, // ペ 30DA
	0x0000FF8E, // ホ 30DB
	0xFF9EFF8E, // ボ 30DC
	0xFF9FFF8E, // ポ 30DD
	0x0000FF8F, // マ 30DE
	0x0000FF90, // ミ 30DF
	0x0000FF91, // ム 30E0
	0x0000FF92, // メ 30E1
	0x0000FF93, // モ 30E2
	0x0000FF6C, // ャ 30E3
	0x0000FF94, // ヤ 30E4
	0x0000FF6D, // ュ 30E5
	0x0000FF95, // ユ 30E6
	0x0000FF6E, // ョ 30E7
	0x0000FF96, // ヨ 30E8
	0x0000FF97, // ラ 30E9
	0x0000FF98, // リ 30EA
	0x0000FF99, // ル 30EB
	0x0000FF9A, // レ 30EC
	0x0000FF9B, // ロ 30ED
	0x0000FF9C, // ヮ 30EE    NOTE: No half-width small wa? This uses the half-width wa.
	0x0000FF9C, // ワ 30EF
	0x00000000, // ヰ 30F0    NOTE: No half-width version (uncommon; wi)
	0x00000000, // ヱ 30F1    NOTE: No half-width version (uncommon; we)
	0x0000FF66, // ヲ 30F2
	0x0000FF9D, // ン 30F3
	0x0000FF73, // ヴ 30F4    NOTE: vu = u + diacritic
	0x00000000, // ヵ 30F5    NOTE: No half-width version (small ka)
	0x00000000, // ヶ 30F6    NOTE: No half-width version (small ke)
	0xFF9EFF73, // ヷ 30F7    NOTE: va = wa + diacritic
	0x00000000, // ヸ 30F8    NOTE: No half-width version (vi = wi + diacritic; no wi)
	0x00000000, // ヹ 30F9    NOTE: No half-width version (ve = we + diacritic; no we)
	0xFF9EFF66, // ヺ 30FA    NOTE: vo = wo + diacritic
	0x0000FF65  // ・ 30FB    NOTE: This is a middle dot (half-width) not a period
};

static axstr_utf32_t AXSTR_CALL axstr_halfwidth_katakana_to_fullwidth_katakana_( axstr_utf32_t UTF32Char, axstr_utf32_t *pDstMark )
{
	axstr_utf32_t Data;

	if( UTF32Char >= 0x30A1 && UTF32Char <= 0x30FB ) {
		Data = g_axstr_katakana_conv_[ UTF32Char - 0x30A1 ];

		if( pDstMark != ( axstr_utf32_t * )0 ) {
			*pDstMark = ( Data & 0xFFFF0000 ) >> 16;
		}

		return Data & 0x0000FFFF;
	}

	return UTF32Char;
}
static axstr_utf32_t AXSTR_CALL axstr_fullwidth_katakana_to_halfwidth_katakana_( axstr_utf32_t UTF32Char, axstr_utf32_t UTF32DiacriticMark )
{
	axstr_utf32_t TestValue;
	axstr_size_t i;

	if( UTF32Char < 0x0000FF65 || UTF32Char > 0x0000FF9D ) {
		return UTF32Char;
	}

	TestValue = UTF32Char | ( UTF32DiacriticMark << 16 );
	for( i = 0; i < sizeof( g_axstr_katakana_conv_ )/sizeof( g_axstr_katakana_conv_[ 0 ] ); ++i ) {
		if( g_axstr_katakana_conv_[ i ] == TestValue ) {
			return ( axstr_utf32_t )( i + 0x30A1 );
		}
	}

	return UTF32Char;
}

#endif

/*!
 * \brief Convert a half-width character to a full-width character.
 *
 * Supports English letters and Japanese katakana.
 * e.g., "CLANNAD" to "ＣＬＡＮＮＡＤ"
 */
AXSTR_FUNC axstr_utf32_t AXSTR_CALL axstr_hantozen_ch( axstr_utf32_t UTF32Char )
#if AXSTR_IMPLEMENT
{
	if( UTF32Char >= 0x0021 && UTF32Char <= 0x007E ) {
		return UTF32Char - 0x0021 + 0xFF01;
	}

	return axstr_halfwidth_katakana_to_fullwidth_katakana_( UTF32Char , ( axstr_utf32_t * )0 );
}
#else
;
#endif
/*!
 * \brief Convert a full-width character to a half-width character.
 *
 * Supports English letters and Japanese katakana.
 * e.g., "ＣＬＡＮＮＡＤ" to "CLANNAD"
 */
AXSTR_FUNC axstr_utf32_t AXSTR_CALL axstr_zentohan_ch( axstr_utf32_t UTF32Char )
#if AXSTR_IMPLEMENT
{
	if( UTF32Char >= 0xFF01 && UTF32Char <= 0xFF5E ) {
		return UTF32Char - 0xFF01 + 0x0021;
	}

	return axstr_fullwidth_katakana_to_halfwidth_katakana_( UTF32Char, 0 );
}
#else
;
#endif

/*
===============================================================================

	STRING OBJECT

===============================================================================
*/

/*! \internal */
typedef struct axstr__obj_s
{
	axstr_size_t cLen;
	axstr_size_t cMax;
} axstr__obj_t;

#ifndef AXSTR__STROBJ_WANTED
# ifdef __cplusplus
#  define AXSTR__STROBJ_WANTED      1
# else
#  define AXSTR__STROBJ_WANTED      0
# endif
#endif

#ifndef AXSTR__OBJSTR_WANTED
# if AXSTR_IMPLEMENT || defined( __cplusplus )
#  define AXSTR__OBJSTR_WANTED      1
# else
#  define AXSTR__OBJSTR_WANTED      0
# endif
#endif

#ifndef AXSTR__INLINESTATIC
# ifdef __cplusplus
#  define AXSTR__INLINESTATIC       inline
# else
#  define AXSTR__INLINESTATIC       static
# endif
#endif

#if AXSTR__STROBJ_WANTED
/*! \internal */
AXSTR__INLINESTATIC axstr__obj_t *AXSTR_CALL axstr__strobj( const char *p )
{
	return p != ( const char * )0 ? ( ( ( axstr__obj_t * )p ) - 1 ) : ( axstr__obj_t * )0;
}
#endif
#if AXSTR__OBJSTR_WANTED
/*! \internal */
AXSTR__INLINESTATIC char *AXSTR_CALL axstr__objstr( const axstr__obj_t *p )
{
	return p != ( const axstr__obj_t * )0 ? ( char * )( p + 1 ) : ( char * )0;
}
#endif

/*! \interal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_alloc( axstr_size_t cMaxLen )
#if AXSTR_IMPLEMENT
{
	axstr__obj_t *p;

	p = ( axstr__obj_t * )axstr_alloc( cMaxLen + sizeof( *p ) );
	if( !p ) {
		return ( axstr__obj_t * )0;
	}

	p->cLen = 0;
	p->cMax = cMaxLen;

	return p;
}
#else
;
#endif
/*! \internal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_free( axstr__obj_t *p )
#if  AXSTR_IMPLEMENT
{
	axstr_free( ( void * )p );
	return ( axstr__obj_t * )0;
}
#else
;
#endif

/*! \internal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_setlen( axstr__obj_t *dst, axstr_size_t cLen )
#if AXSTR_IMPLEMENT
{
	if( !dst || cLen >= dst->cMax ) {
		return ( axstr__obj_t * )0;
	}

	dst->cLen = cLen;
	return dst;
}
#else
;
#endif

/*! \internal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_reserve( axstr__obj_t *src, axstr_size_t cLen )
#if AXSTR_IMPLEMENT
{
	static const axstr_size_t gran = 64;
	static const axstr_size_t minlen = 16;
	axstr__obj_t *dst;

	if( !src || cLen >= src->cMax ) {
		const axstr_size_t n = cLen + 1;
		const axstr_size_t maxlen = n > minlen ? n + ( n%gran != 0 ? gran - n%gran : 0 ) : minlen;

		dst = axstr__obj_alloc( maxlen );
		if( !dst ) {
			return ( axstr__obj_t * )0;
		}

		if( src != ( axstr__obj_t * )0 ) {
			dst->cLen = axstr_cpyn( axstr__objstr( dst ), maxlen, axstr__objstr( src ), src->cLen );
			axstr_free( ( void * )src );
		} else {
			*axstr__objstr( dst ) = '\0';
		}
	} else {
		dst = src;
		if( cLen < dst->cLen ) {
			axstr__objstr( src )[ cLen ] = '\0';
			dst->cLen = cLen;
		}
	}

	return dst;
}
#else
;
#endif

/*! \internal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_clear( axstr__obj_t *src )
#if AXSTR_IMPLEMENT
{
	if( !src ) {
		return ( axstr__obj_t * )0;
	}

	if( src->cMax > 0 ) {
		*( ( char * )( src + 1 ) ) = '\0';
	}
	src->cLen = 0;

	return src;
}
#else
;
#endif
/*! \internal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_remove( axstr__obj_t *src, axstr_size_t uOffset, axstr_size_t cLen )
#if AXSTR_IMPLEMENT
{
	if( !src ) {
		return ( axstr__obj_t * )0;
	}

	src->cLen = axstr_remove( axstr__objstr( src ), src->cLen, uOffset, cLen );
	return src;
}
#else
;
#endif

/*! \internal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_assign( axstr__obj_t *obj, const char *src, axstr_size_t cLen )
#if AXSTR_IMPLEMENT
{
	axstr_size_t n;
	axstr__obj_t *dstobj;
	char *dst;

	if( !src ) {
		return ( axstr__obj_t * )0;
	}

	n = cLen == ~( axstr_size_t )0 ? axstr_len( src ) : cLen;

	dstobj = axstr__obj_reserve( obj, n );
	if( !dstobj ) {
		return ( axstr__obj_t * )0;
	}

	dst = axstr__objstr( dstobj );

	axstr_cpyn( dst, dstobj->cMax, src, n );
	dstobj->cLen = n;

	return dstobj;
}
#else
;
#endif

/*! \internal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_prepend( axstr__obj_t *obj, const char *src, axstr_size_t cLen )
#if AXSTR_IMPLEMENT
{
	axstr_size_t n;
	axstr__obj_t *dstobj;
	char *dst;

	n = cLen == ~( axstr_size_t )0 ? axstr_len( src ) : cLen;

	dstobj = axstr__obj_reserve( obj, ( !obj ? 0 : obj->cLen ) + n );
	if( !dstobj ) {
		return ( axstr__obj_t * )0;
	}

	dst = axstr__objstr( dstobj );

	axstr_move( dst, dstobj->cMax, n, 0, dstobj->cLen );
	axstr__memcpy( ( void * )dst, ( const void * )src, n );

	dstobj->cLen += n;
	return dstobj;
}
#else
;
#endif
/*! \internal */
AXSTR_FUNC axstr__obj_t *AXSTR_CALL axstr__obj_append( axstr__obj_t *obj, const char *src, axstr_size_t cLen )
#if AXSTR_IMPLEMENT
{
	axstr_size_t n;
	axstr__obj_t *dstobj;
	char *dst;

	n = cLen == ~( axstr_size_t )0 ? axstr_len( src ) : cLen;

	dstobj = axstr__obj_reserve( obj, ( !obj ? 0 : obj->cLen ) + n );
	if( !dstobj ) {
		return ( axstr__obj_t * )0;
	}

	dst = axstr__objstr( dstobj );
	axstr_cpyn( dst + dstobj->cLen, dstobj->cMax - dstobj->cLen, src, n );

	dstobj->cLen += n;
	return dstobj;
}
#else
;
#endif

#if AXSTR_IMPLEMENT
# define AXSTR__ROW(N_)\
	N_+0x0, N_+0x1, N_+0x2, N_+0x3, N_+0x4, N_+0x5, N_+0x6, N_+0x7,\
	N_+0x8, N_+0x9, N_+0xA, N_+0xB, N_+0xC, N_+0xD, N_+0xE, N_+0xF
/*! \internal */
unsigned char axstr__g_chars[256] = {
	AXSTR__ROW(0x00), AXSTR__ROW(0x10), AXSTR__ROW(0x20), AXSTR__ROW(0x30),
	AXSTR__ROW(0x40), AXSTR__ROW(0x50), AXSTR__ROW(0x60), AXSTR__ROW(0x70),
	AXSTR__ROW(0x80), AXSTR__ROW(0x90), AXSTR__ROW(0xA0), AXSTR__ROW(0xB0),
	AXSTR__ROW(0xC0), AXSTR__ROW(0xD0), AXSTR__ROW(0xE0), AXSTR__ROW(0xF0)
};
# undef AXSTR__ROW
#else
/*! \internal */
extern unsigned char axstr__g_chars[256];
#endif

AXSTR_FUNC const char *AXSTR_CALL axstr__getCharPtr( unsigned char x )
#if AXSTR_IMPLEMENT
{
	return ( const char * )&axstr__g_chars[ x ];
}
#else
;
#endif

#ifdef __cplusplus
}
#endif

#if AXSTR_CXX_OVERLOADS_ENABLED
template< axstr_size_t tDstSize >
inline axstr_size_t axstr_cpyn( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszSrc, axstr_size_t cSrcBytes )
{
	return axstr_cpyn( pszDstBuf, tDstSize, pszSrc, cSrcBytes );
}
template< axstr_size_t tDstSize >
inline axstr_size_t axstr_cpy( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszSrc )
{
	return axstr_cpy( pszDstBuf, tDstSize, pszSrc );
}

template< axstr_size_t tDstSize >
inline axstr_size_t axstr_catn( AXSTR_INOUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszSrc, axstr_size_t cSrcBytes )
{
	return axstr_catn( pszDstBuf, tDstSize, pszSrc, cSrcBytes );
}
template< axstr_size_t tDstSize >
inline axstr_size_t axstr_cat( AXSTR_INOUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszSrc )
{
	return axstr_cat( pszDstBuf, tDstSize, pszSrc );
}

template< axstr_size_t tDstSize >
inline axstr_size_t axstr_catpathn( AXSTR_INOUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszSrc, axstr_size_t cSrcBytes )
{
	return axstr_catpathn( pszDstBuf, tDstSize, pszSrc, cSrcBytes );
}
template< axstr_size_t tDstSize >
inline axstr_size_t axstr_catpath( AXSTR_INOUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszSrc )
{
	return axstr_catpath( pszDstBuf, tDstSize, pszSrc );
}

template< axstr_size_t tDstSize >
inline axstr_size_t axstr_printfv( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszFormat, va_list vlArgs )
{
	return axstr_printfv( pszDstBuf, tDstSize, pszFormat, vlArgs );
}
template< axstr_size_t tDstSize >
inline axstr_size_t axstr_printf( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszFormat, ... )
{
	va_list vlArgs;

	va_start( vlArgs, pszFormat );
	const axstr_size_t r = axstr_printfv( pszDstBuf, tDstSize, pszFormat, vlArgs );
	va_end( vlArgs );

	return r;
}

template< axstr_size_t tDstSize >
inline axstr_bool_t axstr_utf8_to_utf16( axstr_utf16_t( &pUTF16Dst )[ tDstSize ], const axstr_utf8_t *pUTF8Src )
{
	return axstr_utf8_to_utf16( pUTF16Dst, tDstSize, pUTF8Src );
}
template< axstr_size_t tDstSize >
inline axstr_bool_t axstr_utf16_to_utf8( axstr_utf8_t( &pUTF8Dst )[ tDstSize ], const axstr_utf16_t *pUTF16Src )
{
	return axstr_utf16_to_utf8( pUTF8Dst, tDstSize, pUTF16Src );
}

template< axstr_size_t tDstSize >
inline const char *axstr_relpath( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszSrcFrom, const char *pszSrcTo )
{
	return axstr_relpath( pszDstBuf, tDstSize, pszSrcFrom, pszSrcTo );
}
# if AXSTR_ABSPATH_ENABLED
template< axstr_size_t tDstSize >
inline const char *axstr_abspath( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *pszSrc )
{
	return axstr_abspath( pszDstBuf, tDstSize, pszSrc );
}
# endif

template< axstr_size_t tDstSize >
inline axstr_bool_t axstr_find_common( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *const *ppszTexts, axstr_size_t cTexts )
{
	return axstr_find_common( pszDstBuf, tDstSize, ppszTexts, cTexts );
}
template< axstr_size_t tNumTexts >
inline axstr_bool_t axstr_find_common( AXSTR_OUT_Z(cDstBytes) char *pszDstBuf, axstr_size_t cDstBytes, const char *const( &ppszTexts )[ tNumTexts ] )
{
	return axstr_find_common( pszDstBuf, cDstBytes, ppszTexts, tNumTexts );
}
template< axstr_size_t tDstSize, axstr_size_t tNumTexts >
inline axstr_bool_t axstr_find_common( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], const char *const( &ppszTexts )[ tNumTexts ] )
{
	return axstr_find_common( pszDstBuf, tDstSize, ppszTexts, tNumTexts );
}

template< axstr_size_t tDstSize >
inline char *axstr_from_uint_x( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], axstr_uint_t value, unsigned int radix = AXSTR_DEFAULT_RADIX, unsigned int numDigitsForSep = AXSTR_DIGITSEP_SPACING, char chDigitSep = AXSTR_DIGITSEP_CH )
{
	return axstr_from_uint_x( pszDstBuf, tDstSize, value, radix, numDigitsForSep, chDigitSep );
}
template< axstr_size_t tDstSize >
inline char *axstr_from_int_x( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], axstr_sint_t value, unsigned int radix = AXSTR_DEFAULT_RADIX, unsigned int numDigitsForSep = AXSTR_DIGITSEP_SPACING, char chDigitSep = AXSTR_DIGITSEP_CH )
{
	return axstr_from_int_x( pszDstBuf, tDstSize, value, radix, numDigitsForSep, chDigitSep );
}
template< axstr_size_t tDstSize >
inline char *axstr_from_float_x( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], axstr_real_t value, unsigned int radix = AXSTR_DEFAULT_RADIX, unsigned int maxTrailingDigits = AXSTR_FLOAT_TRAIL_DIGITS, unsigned int numDigitsForSep = AXSTR_DIGITSEP_SPACING, char chDigitSep = AXSTR_DIGITSEP_CH )
{
	return axstr_from_float_x( pszDstBuf, tDstSize, value, radix, maxTrailingDigits, numDigitsForSep, chDigitSep );
}

template< axstr_size_t tDstSize >
inline char *axstr_from_uint( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], axstr_uint_t value )
{
	return axstr_from_uint( pszDstBuf, tDstSize, value );
}
template< axstr_size_t tDstSize >
inline char *axstr_from_int( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], axstr_sint_t value )
{
	return axstr_from_int( pszDstBuf, tDstSize, value );
}
template< axstr_size_t tDstSize >
inline char *axstr_from_float( AXSTR_OUT_Z(tDstSize) char( &pszDstBuf )[ tDstSize ], axstr_real_t value )
{
	return axstr_from_float( pszDstBuf, tDstSize, value );
}
#endif /* AXSTR_CXX_OVERLOADS_ENABLED */

#if AXSTR_CXX_CLASSES_ENABLED
# if AXSTR_STDSTR_ENABLED
#  include <string.h>
#  include <wchar.h>
# endif
namespace ax
{

#ifndef AXSTR_CXX11_MOVE_ENABLED
# ifdef AX_CXX_RVALUE_REFS_ENABLED
#  define AXSTR_CXX11_MOVE_ENABLED AX_CXX_RVALUE_REFS_ENABLED
# elif defined( _MSC_VER ) && _MSC_VER >= 1600
#  define AXSTR_CXX11_MOVE_ENABLED 1
# elif defined( __GNUC__ ) && __GNUC__ >= 4
#  define AXSTR_CXX11_MOVE_ENABLED 1
# elif defined( __clang__ )
#  define AXSTR_CXX11_MOVE_ENABLED 1
# else
#  define AXSTR_CXX11_MOVE_ENABLED 0
# endif
#endif

#if AXSTR_CXX11_MOVE_ENABLED
	namespace str_detail
	{

		template< typename T > struct RemoveRef { typedef T Type; };
		template< typename T > struct RemoveRef< T & > { typedef T Type; };
		template< typename T > struct RemoveRef< T && > { typedef T Type; };

		template< class T >
		inline typename RemoveRef< T >::Type &&move( T &&x )
		{
			return static_cast< typename RemoveRef< T >::Type && >( x );
		}

	}
# define axstr_cxx11_move(X_) ax::str_detail::move(X_)
#else
# define axstr_cxx11_move(X_) (X_)
#endif

#ifndef AXSTR_OBJMUTSTR_ENABLED
# define AXSTR_OBJMUTSTR_ENABLED 0
#endif

	template< typename T > class TArr;
	template< typename T, typename A > class TMutArr;

	namespace policy { struct DefaultStringAllocator; }
	namespace detail { class  MutStrCore; }

	template< typename Allocator = policy::DefaultStringAllocator >
	class TMutStr;

#if AXSTR_OBJMUTSTR_ENABLED
	class ObjMutStr;
#endif
	class Str;

	namespace EKeepEmpty  { enum Type { No, Yes }; }
	namespace EKeepQuotes { enum Type { No, Yes }; }
	namespace ECase       { enum Type { Sensitive, Insensitive }; }


	namespace policy
	{

		/*!
		 * \brief Templated type that allows you to define how certain array
		 *        container types should be manipulated. (Coding conventions may
		 *        differ between our types and your (or third party) types,
		 *        after all.)
		 */
		template< typename TArrayContainer >
		struct StringArrayManipulation
		{
			/*! The string type the array container holds. */
			typedef typename TArrayContainer::Type     StringType;
			/*! The size-type the array container uses. Might not be size_t. */
			typedef typename TArrayContainer::SizeType SizeType;

			/*!
			 * \brief Remove all elements from the container.
			 */
			static inline Void clear( TArrayContainer &container )
			{
				container.clear();
			}
			/*!
			 * \brief Resize the given container to the number of elements
			 *        given, performing default constructions for new elements.
			 *
			 * \note The container's implementation is allowed to throw on error
			 *       instead of returning `false`.
			 *
			 * \return `true` on success; `false` otherwise.
			 */
			static inline Bool resize( TArrayContainer &container, SizeType cElements )
			{
				return container.resize( cElements );
			}
			/*!
			 * \brief Reserve a specific amount of space in the container such
			 *        that subsequent resize operations <= to the given number
			 *        will succeed if this succeeds.
			 *
			 * \note The container's implementation is allowed to throw on error
			 *       instead of returning `false`.
			 *
			 * \return `true` on success; `false` otherwise.
			 */
			static inline Bool reserve( TArrayContainer &container, SizeType cElements )
			{
				return container.reserve( cElements );
			}

			/*!
			 * \brief Add a string to the end of the container.
			 *
			 * \note The container's implementation is allowed to throw on error
			 *       instead of returning `false`.
			 *
			 * \return `true` on success; `false` otherwise.
			 */
			static inline Bool pushBack( TArrayContainer &container, const StringType &x )
			{
				return container.append( x );
			}
		};

	}




	/*!
	 * \brief Holds an immutable reference to an existing string.
	 *
	 * This is great for lightweight string parsing operations, passing strings
	 * to functions, etc. The cost of this class is just the cost of a
	 * pointer/size pair.
	 *
	 * \note This is *not* `NUL`-terminated.
	 */
	class Str
	{
#if AXSTR_OBJMUTSTR_ENABLED
	friend class ObjMutStr;
#endif
	friend class detail::MutStrCore;
	public:
		typedef axstr_ptrdiff_t DiffType;
		typedef axstr_size_t    SizeType;
		typedef axstr_size_t    AllocSizeType;

		/*! \brief Empty constructor. */
		inline Str()
		: m_pStr( "" )
		, m_cStr( 0 )
		{
		}
		/*! \brief Copy contructor. */
		inline Str( const Str &src )
		: m_pStr( src.m_pStr )
		, m_cStr( src.m_cStr )
		{
		}
		/*! \brief Construct from a specific character. */
		inline Str( const char chMyChar )
		: m_pStr( axstr__getCharPtr( ( unsigned char )chMyChar ) )
		, m_cStr( 1 )
		{
		}
		/*! \brief Construct from a `NUL`-terminated string. */
		inline Str( const char *pszSource )
		: m_pStr( pszSource )
		, m_cStr( !pszSource ? 0 : axstr_len( pszSource ) )
		{
		}
		/*! \brief Construct from a string range. */
		inline Str( const char *s, const char *e )
		: m_pStr( s )
		, m_cStr( e - s )
		{
		}
		/*! \brief Construct from a base string pointer and a byte count. */
		inline Str( const char *pSource, SizeType cBytes )
		: m_pStr( pSource )
		, m_cStr( cBytes )
		{
		}
		/*! \brief Construct from a mutable string. */
#if AXSTR_OBJMUTSTR_ENABLED
		explicit Str( const ObjMutStr &s );
#endif
		explicit Str( const detail::MutStrCore &s );
		template< typename Allocator >
		explicit Str( const TMutStr< Allocator > &s );
		/*! \brief Construct from a mutable string and a byte count. */
#if AXSTR_OBJMUTSTR_ENABLED
		Str( const ObjMutStr &s, SizeType cBytes );
#endif
		Str( const detail::MutStrCore &s, SizeType cBytes );
		template< typename Allocator >
		Str( const TMutStr< Allocator > &s, SizeType cBytes );
		/*! \brief Construct from a mutable string, offset, and byte count. */
#if AXSTR_OBJMUTSTR_ENABLED
		Str( const ObjMutStr &s, SizeType uOffset, SizeType cBytes );
#endif
		Str( const detail::MutStrCore &s, SizeType uOffset, SizeType cBytes );
		template< typename Allocator >
		Str( const TMutStr< Allocator > &s, SizeType uOffset, SizeType cBytes );

		/*! \brief   Retrieve the base pointer to this string.
		 *  \warning This is *not* `NUL`-terminated. */
		inline const char *get() const
		{
			return m_pStr;
		}
		/*! \brief Retrieve the end pointer to this string. */
		inline const char *getEnd() const
		{
			return m_pStr + m_cStr;
		}

		/*! \brief  Find a given byte within the string.
		 *  \return `-1` if not found, or the zero-based index to the byte. */
		inline DiffType find( char ch, DiffType iAfter = -1 ) const
		{
			if( !m_pStr || !m_cStr || iAfter < -1 || iAfter >= (DiffType)m_cStr ) {
				return -1;
			}

			if( !ch ) {
				return m_cStr;
			}

			const char *const e = m_pStr + m_cStr;
			const char *const s = iAfter >= 0 ? m_pStr + iAfter + 1 : m_pStr;
# if AXSTR_STDSTR_ENABLED
			const void *const p = memchr( ( const void * )s, +ch, e - s );
			if( !p ) {
				return -1;
			}

			return DiffType( ( ( const char * )p ) - m_pStr );
# else
			for( const char *p = s; p < e; ++p ) {
				if( *p == ch ) {
					return DiffType( p - m_pStr );
				}
			}

			return -1;
# endif
		}
		/*! \brief  Find a given substring within the string.
		 *  \return `-1` if not found, or the zero-based index to the first byte
		 *          of the substring otherwise. */
		inline DiffType find( Str inStr, DiffType iAfter = -1 ) const
		{
			if( !m_pStr || !m_cStr || iAfter < -1 || inStr.isEmpty() ) {
				return -1;
			}

			const char *const e = m_pStr + m_cStr;
			const char *const s = iAfter >= 0 ? m_pStr + iAfter + 1 : m_pStr;
# if AXSTR_STDSTR_ENABLED
			const char *p = s;
			for(;;) {
				p = ( const char * )memchr( ( const void * )p, +*inStr.get(), e - p );
				if( !p ) {
					break;
				}

				if( Str( p, e ).startsWith( inStr ) ) {
					return DiffType( p - m_pStr );
				}

				++p;
				if( p == e ) {
					break;
				}
			}
# else
			for( const char *p = s; p < e; ++p ) {
				if( Str( p, e ).startsWith( inStr ) ) {
					return DiffType( p - m_pStr );
				}
			}
# endif

			return -1;
		}
		/*! \brief  Find the first occurrence of any of the given characters
		 *          within the string.
		 *  \return `-1` if not found, or the zero-based index to the first
		 *          byte matching the given characters that was found. */
		inline DiffType findAny( const char *pszChars, DiffType iAfter = -1 ) const
		{
			if( !m_pStr || !m_cStr || iAfter < -1 || !pszChars || !*pszChars ) {
				return -1;
			}

			const char *const s = iAfter >= 0 ? m_pStr + iAfter + 1 : m_pStr;
			const char *const e = m_pStr + m_cStr;
			for( const char *p = s; p < e; ++p ) {
# if AXSTR_STDSTR_ENABLED
				if( *p == '\0' ) {
					continue;
				}

				if( strchr( pszChars, +*p ) != ( const char * )0 ) {
					return DiffType( p - m_pStr );
				}
# else
				for( const char *q = pszChars; *q != '\0'; ++q ) {
					if( *p == *q ) {
						return DiffType( p - m_pStr );
					}
				}
# endif
			}

			return -1;
		}

		/*! \brief  Find a given byte within the string. Case insensitive.
		 *  \return `-1` if not found, or the zero-based index to the byte. */
		inline DiffType caseFind( char ch, DiffType iAfter = -1 ) const
		{
			if( !m_pStr || !m_cStr || iAfter < -1 ) {
				return -1;
			}

			if( !ch ) {
				return m_cStr;
			}

			const char *const e = m_pStr + m_cStr;
			const char *const s = iAfter >= 0 ? m_pStr + iAfter + 1 : m_pStr;

			for( const char *p = s; p < e; ++p ) {
				if( axstr_tolower(*p) == axstr_tolower(ch) ) {
					return DiffType( p - m_pStr );
				}
			}

			return -1;
		}
		/*! \brief  Find a given substring within the string. Case insensitive.
		 *  \return `-1` if not found, or the zero-based index to the first byte
		 *          of the substring otherwise. */
		inline DiffType caseFind( Str inStr, DiffType iAfter = -1 ) const
		{
			if( !m_pStr || !m_cStr || iAfter < -1 || inStr.isEmpty() ) {
				return -1;
			}

			const char *const e = m_pStr + m_cStr;
			const char *const s = iAfter >= 0 ? m_pStr + iAfter + 1 : m_pStr;

			for( const char *p = s; p < e; ++p ) {
				if( Str( p, e ).caseStartsWith( inStr ) ) {
					return DiffType( p - m_pStr );
				}
			}

			return -1;
		}
		/*! \brief  Find the first occurrence of any of the given characters
		 *          within the string. Case insensitive.
		 *  \return `-1` if not found, or the zero-based index to the first
		 *          byte matching the given characters that was found. */
		inline DiffType caseFindAny( const char *pszChars, DiffType iAfter = -1 ) const
		{
			if( !m_pStr || !m_cStr || iAfter < -1 || !pszChars || !*pszChars ) {
				return -1;
			}

			const char *const s = iAfter >= 0 ? m_pStr + iAfter + 1 : m_pStr;
			const char *const e = m_pStr + m_cStr;

			for( const char *p = s; p < e; ++p ) {
				for( const char *q = pszChars; *q != '\0'; ++q ) {
					if( axstr_tolower(*p) == axstr_tolower(*q) ) {
						return DiffType( p - m_pStr );
					}
				}
			}

			return -1;
		}

		/*! \brief  Find the last occurrence of the given byte within the
		 *          string.
		 *  \return `-1` if not found, or the zero-based index to the byte. */
		inline DiffType findLast( char c ) const
		{
			const char *p = m_pStr + m_cStr;
			while( p-- > m_pStr ) {
				if( *p == c ) {
					return DiffType( p - m_pStr );
				}
			}
			return -1;
		}
		/*! \brief  Find the last occurrence of the given substring within this
		 *          string.
		 *  \return `-1` if not found, or the zero-based index to the first byte
		 *          of the substring. */
		inline DiffType findLast( Str s ) const
		{
			const SizeType n = s.len();
			if( s.isEmpty() || m_cStr < n ) {
				return -1;
			}

			for( DiffType i = m_cStr - n; i >= 0; --i ) {
				if( mid( i, n ) == s ) {
					return i;
				}
			}

			return -1;
		}

		/*! \brief Get the byte index for a given column index within the string. */
		inline DiffType columnToByteIndex( SizeType columnIndex ) const
		{
			SizeType byteIndex = 0, currentColumn = 0;

			if( !m_pStr ) {
				return columnIndex == 0 ? 0 : -1;
			}

			while( byteIndex < m_cStr ) {
				if( currentColumn++ == columnIndex ) {
					return DiffType( byteIndex );
				}

				const unsigned char *p = ( const unsigned char * )m_pStr + byteIndex;

				if( ( *p & 0xF0 ) == 0xF0 ) {
					byteIndex += 4;
				} else if( ( *p & 0xE0 ) == 0xE0 ) {
					byteIndex += 3;
				} else if( ( *p & 0xC0 ) == 0xC0 ) {
					byteIndex += 2;
				} else {
					byteIndex += 1;
				}
			}

			return DiffType( -1 );
		}
		/*! \brief Get the column index from a given byte index within the string. */
		inline DiffType byteToColumnIndex( SizeType byteIndex ) const
		{
			SizeType currentByteIndex = 0, currentColumn = 0;

			if( !m_pStr ) {
				return byteIndex == 0 ? 0 : -1;
			}

			if( byteIndex > m_cStr ) {
				byteIndex = m_cStr;
			}

			while( currentByteIndex < byteIndex ) {
				const unsigned char *p = ( const unsigned char * )m_pStr + byteIndex;

				if( ( *p & 0xF0 ) == 0xF0 ) {
					currentByteIndex += 4;
				} else if( ( *p & 0xE0 ) == 0xE0 ) {
					currentByteIndex += 3;
				} else if( ( *p & 0xC0 ) == 0xC0 ) {
					currentByteIndex += 2;
				} else {
					currentByteIndex += 1;
				}

				++currentColumn;
			}

			return DiffType( currentColumn );
		}
		/*! \brief Return the length of this string in columns. */
		inline SizeType numColumns() const
		{
			return SizeType( byteToColumnIndex( m_cStr ) );
		}

		/*! \brief Same as `find(char,DiffType)` but returns a pointer. */
		inline const char *findPtr( char ch, DiffType iAfter = -1 ) const
		{
			return pointer( find( ch, iAfter ) );
		}
		/*! \brief Same as `find(Str,DiffType)` but returns a pointer. */
		inline const char *findPtr( Str inStr, DiffType iAfter = -1 ) const
		{
			return pointer( find( inStr, iAfter ) );
		}
		/*! \brief Same as `findAny(const char*,DiffType)` but returns a
		 *         pointer. */
		inline const char *findAnyPtr( const char *pszChars, DiffType iAfter = -1 ) const
		{
			return pointer( findAny( pszChars, iAfter ) );
		}

		/*! \brief Same as `caseFind(char,DiffType)` but returns a pointer. */
		inline const char *caseFindPtr( char ch, DiffType iAfter = -1 ) const
		{
			return pointer( caseFind( ch, iAfter ) );
		}
		/*! \brief Same as `caseFind(Str,DiffType)` but returns a pointer. */
		inline const char *caseFindPtr( Str inStr, DiffType iAfter = -1 ) const
		{
			return pointer( caseFind( inStr, iAfter ) );
		}
		/*! \brief Same as `caseFindAny(const char*,DiffType)` but returns a
		 *         pointer. */
		inline const char *caseFindAnyPtr( const char *pszChars, DiffType iAfter = -1 ) const
		{
			return pointer( caseFindAny( pszChars, iAfter ) );
		}

		/*! \brief Same as `findLast(char)` but returns a pointer. */
		inline const char *findLastPtr( char c ) const
		{
			return pointer( findLast( c ) );
		}
		/*! \brief Same as `findLast(Str)` but returns a pointer. */
		inline const char *findLastPtr( Str s ) const
		{
			return pointer( findLast( s ) );
		}

		/*! \brief Split this string based on a delimiter to an array. */
		template< typename TArrayContainer >
		inline bool splitTo( TArrayContainer &outArray, Str inSeparator, EKeepEmpty::Type = EKeepEmpty::No ) const;
		template< typename TArrayContainer >
		inline TArrayContainer split( Str inSeparator, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
			TArrayContainer container;
			splitTo< TArrayContainer >( container, inSeparator, keepEmpty );
			return container;
		}

		/*! \brief Split this string based on a delimiter, but treats quoted
		 *         items as separate tokens. */
		template< typename TArrayContainer >
		inline bool splitUnquotedTo( TArrayContainer &outArray, Str inSeparator, Str inEscape = '\\', EKeepQuotes::Type = EKeepQuotes::No, EKeepEmpty::Type = EKeepEmpty::No ) const;
		template< typename TArrayContainer >
		inline TArrayContainer splitUnquoted( Str inSeparator, Str inEscape = '\\', EKeepQuotes::Type keepQuotes = EKeepQuotes::No, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
			TArrayContainer container;
			splitUnquotedTo< TArrayContainer >( container, inSeparator, inEscape, keepQuotes, keepEmpty );
			return container;
		}

		/*! \brief Split this string by lines. */
		template< typename TArrayContainer >
		inline bool splitLinesTo( TArrayContainer &outArr, EKeepEmpty::Type = EKeepEmpty::No ) const;
		template< typename TArrayContainer >
		inline TArrayContainer splitLines( EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
			TArrayContainer container;
			splitLinesTo< TArrayContainer >( container, keepEmpty );
			return container;
		}

		/*! \brief Find the index of the extension part of a filename within
		 *         this string. */
		inline DiffType findExtension() const
		{
			const DiffType i = findLast( '.' );
			if( i == -1 ) {
				return -1;
			}

			if( find( '/', i + 1 ) != -1 ) {
				return -1;
			}
#ifdef _WIN32
			if( find( '\\', i + 1 ) != -1 ) {
				return -1;
			}
#endif

			return i;
		}

		/*! \brief Find the index of the first directory separator within this
		 *         string. */
		inline DiffType findDirSep( const DiffType iAfter = -1 ) const
		{
#ifdef _WIN32
			const DiffType x = find( '/', iAfter );
			const DiffType y = find( '\\', iAfter );

			return x < y && x != -1 ? x : y;
#else
			return find( '/', iAfter );
#endif
		}
		/*! \brief Find the index of the last directory separator within this
		 *         string. */
		inline DiffType findLastDirSep() const
		{
#ifdef _WIN32
			const DiffType x = findLast( '/' );
			const DiffType y = findLast( '\\' );
			return x > y ? x : y;
#else
			return findLast( '/' );
#endif
		}

		/*! \brief Check whether this string starts with a directory
		 *         separator. */
		inline bool startsWithDirSep() const
		{
#ifdef _WIN32
			if( startsWith( '\\' ) ) {
				return true;
			}
#endif

			return startsWith( '/' );
		}
		/*! \brief Check whether this string ends with a directory separator. */
		inline bool endsWithDirSep() const
		{
#ifdef _WIN32
			if( endsWith( '\\' ) ) {
				return true;
			}
#endif

			return endsWith( '/' );
		}

		/*! \brief Retrieve the extension of this string as a separate string
		 *         view. */
		inline Str getExtension() const
		{
			const DiffType i = findExtension();
			if( i == -1 ) {
				return Str();
			}

			return Str( m_pStr + i, m_cStr - i );
		}
		/*! \brief Retrieve the filename part of this string as a separate
		 *         string view. */
		inline Str getFilename() const
		{
			const DiffType i = findLastDirSep() + 1;
			if( !i ) {
				return *this;
			}

			return Str( m_pStr + i, m_cStr - i );
		}
		/*! \brief Retrieve the directory part of this string as a separate
		 *         string view. */
		inline Str getDirectory() const
		{
			const DiffType i = findLastDirSep() + 1;
			if( !i ) {
				return Str();
			}

			return Str( m_pStr, i );
		}
		/*! \brief Retrieve the basename part of this string as a separate
		 *         string view. */
		inline Str getBasename() const
		{
			const DiffType sep = findLastDirSep();

			const DiffType ext = find( '.', sep );

			const char *const s = m_pStr + ( sep + 1 );
			const char *const e = m_pStr + ( ext == -1 ? m_cStr : ext );

			return Str( s, e );
		}
		/*! \brief Retrieve the root part of this string as a separate string
		 *         view. */
		inline Str getRoot() const
		{
#ifdef _WIN32
			if( startsWith( "\\\\" ) ) {
				const DiffType sep = find( '\\', 2 );
				if( sep == -1 ) {
					return Str( *this );
				}

				return Str( m_pStr, &m_pStr[ sep + 1 ] );
			}

			if( m_cStr >= 2 && m_pStr[ 1 ] == ':' ) {
				if( m_cStr > 2 && ( m_pStr[ 2 ] == '/' || m_pStr[ 2 ] == '\\' ) ) {
					return Str( m_pStr, m_pStr + 3 );
				}

				return Str( m_pStr, m_pStr + 2 );
			}

			return Str();
#else
			if( !startsWith( '/' ) ) {
				return Str();
			}

			return Str( m_pStr, m_pStr + 1 );
#endif
		}

#if 0
		/*! \brief Concatenate this string with `path` with path semantics into
		 *         `outStr`. */
#if AXSTR_OBJMUTSTR_ENABLED
		bool tryAppendPathTo( ObjMutStr &outStr, Str path ) const;
#endif
		template< typename Allocator >
		bool tryAppendPathTo( TMutStr< Allocator > &outStr, Str path ) const;
#endif

		/*! \brief Check whether this string is equal to another,
		 *         case-insensitive. */
		inline bool caseCmp( Str x ) const
		{
			if( m_cStr != x.m_cStr ) {
				return false;
			}

			return axstr_casecmpn( m_pStr, x.m_pStr, m_cStr );
		}
		/*! \brief Check whether this string is equal to another,
		 *         case-sensitive. */
		inline bool cmp( Str x ) const
		{
			if( m_cStr != x.m_cStr ) {
				return false;
			}

			return axstr_cmpn( m_pStr, x.m_pStr, m_cStr );
		}

		/*! \brief Determine whether this string should come before or after
		 *         another for sorting purposes, case-insensitive. */
		inline int sortCaseCmp( Str x ) const
		{
			return axstr_sortcasecmpn( m_pStr, x.m_pStr, m_cStr <= x.m_cStr ? m_cStr : x.m_cStr );
		}
		/*! \brief Determine whether this string should come before or after
		 *         another for sorting purposes, case-sensitive. */
		inline int sortCmp( Str x ) const
		{
			return axstr_sortcmpn( m_pStr, x.m_pStr, m_cStr <= x.m_cStr ? m_cStr : x.m_cStr );
		}

		/*! \brief Determine whether this string starts with another. */
		inline bool startsWith( Str x ) const
		{
			if( m_cStr < x.m_cStr ) {
				return false;
			}

			return axstr_cmpn( m_pStr, x.m_pStr, x.m_cStr );
		}
		/*! \brief Determine whether this string ends with another. */
		inline bool endsWith( Str x ) const
		{
			if( m_cStr < x.m_cStr ) {
				return false;
			}

			return axstr_cmpn( m_pStr + ( m_cStr - x.m_cStr ), x.m_pStr, x.m_cStr );
		}
		/*! \brief Determine whether this string contains another. */
		inline bool contains( Str x ) const
		{
			if( m_cStr < x.m_cStr ) {
				return false;
			}

			return find( x ) != -1;
		}

		/*! \brief Determine whether this string starts with a given byte. */
		inline bool startsWith( char x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			return *m_pStr == x;
		}
		/*! \brief Determine whether this string ends with a given byte. */
		inline bool endsWith( char x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			return m_pStr[ m_cStr - 1 ] == x;
		}
		/*! \brief Determine whether this string contains a given byte. */
		inline bool contains( char x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

#if AXSTR_STDSTR_ENABLED
			return memchr( ( const void * )m_pStr, +x, m_cStr ) != ( const void * )0;
#else
			return find( x, -1 ) != -1;
#endif
		}

		/*! \brief Determine whether this string starts with any of the bytes
		 *         of another string. */
		inline bool startsWithAny( Str x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			const char c = *m_pStr;
			while( x.isUsed() ) {
				if( c == *x.get() ) {
					return true;
				}

				x = x.skip();
			}

			return false;
		}
		/*! \brief Determine whether this string ends with any of the bytes of
		 *         another string. */
		inline bool endsWithAny( Str x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			const char c = m_pStr[ m_cStr - 1 ];
			while( x.isUsed() ) {
				if( c == *x.get() ) {
					return true;
				}

				x = x.skip();
			}

			return false;
		}
		/*! \brief Determine whether this string contains any of the bytes of
		 *         another string. */
		inline bool containsAny( Str x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			while( x.isUsed() ) {
				if( find( x.readByte() ) != -1 ) {
					return true;
				}
			}

			return false;
		}

		/*! \brief Determine whether this string starts with another string,
		 *         case-insensitive. */
		inline bool caseStartsWith( Str x ) const
		{
			if( m_cStr < x.m_cStr ) {
				return false;
			}

			return axstr_casecmpn( m_pStr, x.m_pStr, x.m_cStr );
		}
		/*! \brief Determine whether this string ends with another string,
		 *         case-insensitive. */
		inline bool caseEndsWith( Str x ) const
		{
			if( m_cStr < x.m_cStr ) {
				return false;
			}

			return axstr_casecmpn( m_pStr + ( m_cStr - x.m_cStr ), x.m_pStr, x.m_cStr );
		}
		/*! \brief Determine whether this string contains another string,
		 *         case-insensitive. */
		inline bool caseContains( Str x ) const
		{
			if( m_cStr < x.m_cStr ) {
				return false;
			}

			return caseFind( x ) != -1;
		}

		/*! \brief Determine whether this string starts with a given byte,
		 *         case-insensitive. */
		inline bool caseStartsWith( char x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			return axstr_tolower( *m_pStr ) == axstr_tolower( x );
		}
		/*! \brief Determine whether this string ends with a given byte,
		 *         case-insensitive. */
		inline bool caseEndsWith( char x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			return axstr_tolower( m_pStr[ m_cStr - 1 ] ) == axstr_tolower( x );
		}
		/*! \brief Determine whether this string contains a given byte,
		 *         case-insensitive. */
		inline bool caseContains( char x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			return caseFind( x ) != -1;
		}

		/*! \brief Determine whether this string starts with any of the bytes of
		 *         another string, case-insensitive. */
		inline bool caseStartsWithAny( Str x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			const axstr_utf32_t c = axstr_tolower( *( const unsigned char * )m_pStr );
			while( x.isUsed() ) {
				const axstr_utf32_t d = axstr_tolower( *( const unsigned char * )x.get() );
				if( c == d ) {
					return true;
				}

				x = x.skip();
			}

			return false;
		}
		/*! \brief Determine whether this string ends with any of the bytes of
		 *         another string, case-insensitive. */
		inline bool caseEndsWithAny( Str x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			const axstr_utf32_t c = axstr_tolower( ( ( const unsigned char * )m_pStr )[ m_cStr - 1 ] );
			while( x.isUsed() ) {
				const axstr_utf32_t d = axstr_tolower( *( const unsigned char * )x.get() );
				if( c == d ) {
					return true;
				}

				x = x.skip();
			}

			return false;
		}
		/*! \brief Determine whether this string contains any of the bytes of
		 *         another string, case-insensitive. */
		inline bool caseContainsAny( Str x ) const
		{
			if( m_cStr < 1 ) {
				return false;
			}

			while( x.isUsed() ) {
				if( caseFind( x.readByte() ) != -1 ) {
					return true;
				}
			}

			return false;
		}

		/*! \brief Advance if this string starts with the other string.
		 *  \return `true` if it started with `x` prior to advancing. */
		inline bool skipIfStartsWith( Str x )
		{
			if( !startsWith( x ) ) {
				return false;
			}

			*this = skip( x.len() );
			return true;
		}
		/*! \brief Chop if this string ends with the other string.
		 *  \return `true` if it ended with `x` prior to chopping. */
		inline bool dropIfEndsWith( Str x )
		{
			if( !endsWith( x ) ) {
				return false;
			}

			*this = drop( x.len() );
			return true;
		}
		/*! \brief Advance if starts with the other. Case insensitive.
		 *  \return `true` if it started with `x` prior to advancing. */
		inline bool skipIfCaseStartsWith( Str x )
		{
			if( !caseStartsWith( x ) ) {
				return false;
			}

			*this = skip( x.len() );
			return true;
		}
		/*! \brief Chop if ends with the other. Case insensitive.
		 *  \return `true` if it ended with `x` prior to chopping. */
		inline bool dropIfCaseEndsWith( Str x )
		{
			if( !caseEndsWith( x ) ) {
				return false;
			}

			*this = drop( x.len() );
			return true;
		}

		/*! \brief Advance if this string starts with the other string.
		 *  Also skips whitespace if it started with the other string.
		 *  \return `true` if it started with `x` prior to advancing. */
		inline bool skipAndTrimIfStartsWith( Str x )
		{
			if( !startsWith( x ) ) {
				return false;
			}

			*this = skip( x.len() ).trimLeft();
			return true;
		}
		/*! \brief Chop if this string ends with the other string.
		 *  Also drops whitespace if it ended with the other string.
		 *  \return `true` if it ended with `x` prior to chopping. */
		inline bool dropAndTrimIfEndsWith( Str x )
		{
			if( !endsWith( x ) ) {
				return false;
			}

			*this = drop( x.len() ).trimRight();
			return true;
		}
		/*! \brief Advance if starts with the other. Case insensitive.
		 *  Also skips whitespace if it started with the other string.
		 *  \return `true` if it started with `x` prior to advancing. */
		inline bool skipAndTrimIfCaseStartsWith( Str x )
		{
			if( !caseStartsWith( x ) ) {
				return false;
			}

			*this = skip( x.len() ).trimLeft();
			return true;
		}
		/*! \brief Chop if ends with the other. Case insensitive.
		 *  Also drops whitespace if it ended with the other string.
		 *  \return `true` if it ended with `x` prior to chopping. */
		inline bool dropAndTrimIfCaseEndsWith( Str x )
		{
			if( !caseEndsWith( x ) ) {
				return false;
			}

			*this = drop( x.len() ).trimRight();
			return true;
		}

		/*! \brief Retrieve the left `n`-bytes of this string. */
		inline Str left( DiffType n ) const
		{
			if( !m_pStr ) { return Str(); }
			return Str( m_pStr, calcBytes_( n ) );
		}
		/*! \brief Retrieve the right `n`-bytes of this string. */
		inline Str right( DiffType n ) const
		{
			if( !m_pStr ) { return Str(); }

			const SizeType cBytes = calcBytes_( n );

			return Str( m_pStr + ( m_cStr - cBytes ), cBytes );
		}
		/*! \brief Retrieve `num`-bytes of this string starting from `pos`. */
		inline Str mid( DiffType pos, SizeType num = 1 ) const
		{
			if( !m_pStr ) { return Str(); }

			const SizeType cOffBytes = calcBytes_( pos );
			const SizeType cMovBytes = cOffBytes + num <= m_cStr ? num : m_cStr - cOffBytes;

			return Str( m_pStr + cOffBytes, cMovBytes );
		}
		/*! \brief Retrieve the substring of this string from `startPos` to
		 *         `endPos`. */
		inline Str substr( DiffType startPos, DiffType endPos ) const
		{
			if( !m_pStr ) { return Str(); }

			const SizeType cOffBytesLT = calcBytes_( startPos );
			const SizeType cOffBytesRT = calcBytes_( endPos );

			const SizeType cOffBytesL = cOffBytesLT < cOffBytesRT ? cOffBytesLT : cOffBytesRT;
			const SizeType cOffBytesR = cOffBytesLT < cOffBytesRT ? cOffBytesRT : cOffBytesLT;

			return Str( m_pStr + cOffBytesL, m_pStr + cOffBytesR );
		}

		/*! \brief Retrieve the left `n`-bytes of this string. */
		inline Str extractLeft( DiffType n )
		{
			if( !m_pStr ) { return Str(); }
			Str r( m_pStr, calcBytes_( n ) );

			m_pStr += r.len();
			m_cStr -= r.len();

			if( !m_cStr ) {
				m_pStr = ( const char * )0;
			}

			return r;
		}
		/*! \brief Retrieve the right `n`-bytes of this string. */
		inline Str extractRight( DiffType n )
		{
			if( !m_pStr ) { return Str(); }

			const SizeType cBytes = calcBytes_( n );

			Str r( m_pStr + ( m_cStr - cBytes ), cBytes );

			m_cStr -= r.len();
			if( !m_cStr ) {
				m_pStr = ( const char * )0;
			}

			return r;
		}

		/*! \brief Remove the first `pos`-bytes in the string. */
		inline Str skip( DiffType pos = 1 ) const
		{
			if( !m_pStr ) { return Str(); }

			const SizeType cOffBytes = calcBytes_( pos );

			return Str( m_pStr + cOffBytes, m_cStr - cOffBytes );
		}
		/*! \brief Remove the last `num`-bytes in the string. */
		inline Str drop( DiffType num = 1 ) const
		{
			if( !m_pStr ) { return Str(); }

			const SizeType cBytes = calcBytes_( num );

			return Str( m_pStr, m_cStr - cBytes );
		}

		/*! \brief Trim leading whitespace. */
		inline Str skipWhitespace() const
		{
			if( !m_pStr ) { return Str(); }

			const char *const e = m_pStr + m_cStr;
			const char *s = m_pStr;
			while( s < e && *( const unsigned char * )s <= ' ' ) {
				++s;
			}

			return Str( s, e );
		}
		/*! \brief Remove the current line from the string. */
		inline Str skipLine() const
		{
			if( !m_pStr ) { return Str(); }

			const char *const e = m_pStr + m_cStr;
			const char *const s = axstr_skip_line_e( m_pStr, e );

			return Str( s, e );
		}
		/*! \brief Remove the current line from the string, returning it. */
		inline Str readLine()
		{
			if( !m_pStr ) { return Str(); }

			const char *const p = axstr_skip_line_e( m_pStr, m_pStr + m_cStr );
			Str line( m_pStr, p );

			m_cStr -= line.len();
			m_pStr = p;

			line.dropIfEndsWith( '\n' );
			line.dropIfEndsWith( '\r' );

			return line;
		}

		/*! \brief Retrieve the first character and skip forward. */
		inline axstr_utf32_t readChar()
		{
			const axstr_utf8_t *p = ( const axstr_utf8_t * )m_pStr;
			const axstr_utf8_t *const e = p + m_cStr;

			const axstr_utf32_t ch = axstr_step_utf8_decode( &p, e );

			m_cStr -= SizeType( p - ( const axstr_utf8_t * )m_pStr );
			m_pStr = ( const char * )p;

			return ch;
		}
		/*! \brief Retrieve the first byte and skip forward. */
		inline char readByte()
		{
			if( !m_cStr ) {
				return '\0';
			}

			--m_cStr;
			return *m_pStr++;
		}
		/*! \brief Check for a specific byte; if there then skip forward. */
		inline bool checkByte( char ch )
		{
			if( !m_cStr ) {
				return ch == '\0';
			}

			if( *m_pStr != ch ) {
				return false;
			}

			--m_cStr;
			++m_pStr;

			return true;
		}
		/*! \brief Check for a specific byte range; if there then skip forward. */
		inline bool checkByteRange( char chStart, char chEnd )
		{
			const unsigned char s = ( unsigned char )chStart;
			const unsigned char e = ( unsigned char )chEnd;
			const unsigned char c = ( unsigned char )( !m_cStr ? '\0' : *m_pStr );

			if( !( s <= c && c <= e ) ) {
				return false;
			}

			if( m_cStr > 0 ) {
				++m_pStr;
				--m_cStr;
			}

			return true;
		}
		/*! \brief Check for a specific character; if there then skip forward. */
		inline Bool checkChar( axstr_utf32_t codepoint )
		{
			if( !m_cStr ) {
				return codepoint == 0;
			}

			if( axstr_utf32_t( *m_pStr ) == codepoint ) {
				--m_cStr;
				++m_pStr;

				return true;
			}

			Str t( *this );
			if( t.readChar() == codepoint ) {
				const UPtr cDiff = m_cStr - t.m_cStr;

				m_cStr -= cDiff;
				m_pStr += cDiff;

				return true;
			}

			return false;
		}

		/*! \brief Read a "token" (non-whitespace character sequence) */
		inline Str readToken()
		{
			*this = skipWhitespace();

			SizeType cTokenChars;
			for( cTokenChars = 0; cTokenChars < m_cStr; ++cTokenChars ) {
				if( ( ( const unsigned char * )m_pStr )[ cTokenChars ] <= ' ' ) {
					break;
				}
			}

			if( !cTokenChars ) {
				return Str();
			}

			Str r( m_pStr, cTokenChars );
			*this = skip( cTokenChars ).skipWhitespace();

			return r;
		}
		/*! \brief Read a potentially quoted token */
		inline Str readQuotedToken( EKeepQuotes::Type keepQuotes = EKeepQuotes::No )
		{
			*this = skipWhitespace();

			SizeType cTokenChars = 0;
			Str r;

			if( firstByte() == '\"' ) {
				cTokenChars = 1;
				while( cTokenChars < m_cStr ) {
					if( m_pStr[ cTokenChars++ ] == '\"' ) {
						break;
					}
				}

				if( keepQuotes == EKeepQuotes::No ) {
					r = Str( m_pStr + 1, cTokenChars -2 );
				}
			} else {
				while( cTokenChars < m_cStr ) {
					if( ( ( const unsigned char * )m_pStr )[ cTokenChars ] <= ' ' ) {
						break;
					}

					++cTokenChars;
				}
			}

			if( r.isEmpty() ) {
				r = Str( m_pStr, cTokenChars );
			}

			*this = skip( cTokenChars ).skipWhitespace();
			return r;
		}

		/*! \brief Retrieve the first byte of the string. */
		inline char firstByte() const
		{
			if( !m_cStr ) {
				return '\0';
			}

			return m_pStr[ 0 ];
		}
		/*! \brief Retrieve the last byte of the string. */
		inline char lastByte() const
		{
			if( !m_cStr ) {
				return '\0';
			}

			return m_pStr[ m_cStr - 1 ];
		}

		/*! \brief Skip whitespace from the left. */
		inline Str trimLeft() const
		{
			return skipWhitespace();
		}
		/*! \brief Drop whitespace from the right. */
		inline Str trimRight() const
		{
			axstr_size_t i = m_cStr;

			while( i > 0 ) {
				if( *( ( const unsigned char * )m_pStr + ( i - 1 ) ) > ' ' ) {
					break;
				}

				--i;
			}

			return Str( m_pStr, i );
		}
		/*! \brief Remove whitespace from both sides. */
		inline Str trim() const
		{
			return trimLeft().trimRight();
		}

		/*! \brief Get the remainder of this string starting with the first
		 *         instance of `ch`. */
		inline Str search( char ch, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = find( ch, iAfter );
			if( iFound != -1 ) {
				return skip( iFound );
			}

			return Str();
		}
		/*! \brief Get the remainder of this string starting with the first
		 *         instance of `inStr`. */
		inline Str search( Str inStr, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = find( inStr, iAfter );
			if( iFound != -1 ) {
				return skip( iFound );
			}

			return Str();
		}
		/*! \brief Get the remainder of this string starting with the first
		 *         instance of any byte in `pszChars`. */
		inline Str searchAny( const char *pszChars, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = findAny( pszChars, iAfter );
			if( iFound != -1 ) {
				return skip( iFound );
			}

			return Str();
		}

		/*! \brief Get the remainder of this string starting with the first
		 *         instance of `ch`. Case insensitive. */
		inline Str caseSearch( char ch, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = caseFind( ch, iAfter );
			if( iFound != -1 ) {
				return skip( iFound );
			}

			return Str();
		}
		/*! \brief Get the remainder of this string starting with the first
		 *         instance of `inStr`. Case insensitive. */
		inline Str caseSearch( Str inStr, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = caseFind( inStr, iAfter );
			if( iFound != -1 ) {
				return skip( iFound );
			}

			return Str();
		}
		/*! \brief Get the remainder of this string starting with the first
		 *         instance of any byte in `pszChars`. Case insensitive. */
		inline Str caseSearchAny( const char *pszChars, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = caseFindAny( pszChars, iAfter );
			if( iFound != -1 ) {
				return skip( iFound );
			}

			return Str();
		}

		/*! \brief As with `search(char,DiffType)` but also retrieves the skipped
		 *         part of the string in `outSkipped`. */
		inline Str search( Str &outSkipped, char ch, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = find( ch, iAfter );
			if( iFound != -1 ) {
				outSkipped = left( iFound );
				return skip( iFound );
			}

			return Str();
		}
		/*! \brief As with `search(Str,DiffType)` but also retrieves the skipped
		 *         part of the string in `outSkipped`. */
		inline Str search( Str &outSkipped, Str inStr, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = find( inStr, iAfter );
			if( iFound != -1 ) {
				outSkipped = left( iFound );
				return skip( iFound );
			}

			return Str();
		}
		/*! \brief As with `searchAny(const char*,DiffType)` but also retrieves
		 *         the skipped part of the string in `outSkipped`. */
		inline Str searchAny( Str &outSkipped, const char *pszChars, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = findAny( pszChars, iAfter );
			if( iFound != -1 ) {
				outSkipped = left( iFound );
				return skip( iFound );
			}

			return Str();
		}

		/*! \brief As with `caseSearch(char,DiffType)` but also retrieves the
		 *         skipped part of the string in `outSkipped`. */
		inline Str caseSearch( Str &outSkipped, char ch, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = caseFind( ch, iAfter );
			if( iFound != -1 ) {
				outSkipped = left( iFound );
				return skip( iFound );
			}

			return Str();
		}
		/*! \brief As with `caseSearch(Str,DiffType)` but also retrieves the
		 *         skipped part of the string in `outSkipped`. */
		inline Str caseSearch( Str &outSkipped, Str inStr, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = caseFind( inStr, iAfter );
			if( iFound != -1 ) {
				outSkipped = left( iFound );
				return skip( iFound );
			}

			return Str();
		}
		/*! \brief As with `caseSearchAny(const char*,DiffType)` but also
		 *         retrieves the skipped part of the string in `outSkipped`. */
		inline Str caseSearchAny( Str &outSkipped, const char *pszChars, DiffType iAfter = -1 ) const
		{
			const DiffType iFound = caseFindAny( pszChars, iAfter );
			if( iFound != -1 ) {
				outSkipped = left( iFound );
				return skip( iFound );
			}

			return Str();
		}

		/*! \brief Determine whether `subtext` is a substring (in the same
		 *         memory range) of this string. */
		inline bool hasSubstring( Str subtext ) const
		{
			return subtext.get() >= get() && subtext.getEnd() <= getEnd();
		}
		/*! \brief Determine whether this string is a substring of `supertext`
		 *         (is in the same memory range as `supertext`). */
		inline bool isSubstring( Str supertext ) const
		{
			return supertext.hasSubstring( *this );
		}

		/*! \brief Convert a string to a signed integer. */
		inline axstr_sint_t toInteger( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
		{
			if( !m_pStr ) { return 0; }

			const char *p = m_pStr;
			return axstr_to_int_xe( &p, m_pStr + m_cStr, defaultRadix, chDigitSep );
		}
		/*! \brief Convert a string to an unsigned integer. */
		inline axstr_uint_t toUnsignedInteger( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
		{
			if( !m_pStr ) { return 0; }

			const char *p = m_pStr;
			return axstr_to_uint_xe( &p, m_pStr + m_cStr, defaultRadix, chDigitSep );
		}
		/*! \brief Convert a string to a float. */
		inline axstr_real_t toFloat( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
		{
			if( !m_pStr ) { return 0; }

			const char *p = m_pStr;
			return axstr_to_float_xe( &p, m_pStr + m_cStr, defaultRadix, chDigitSep );
		}

		/*! \brief Convert a string to a boolean. */
		inline bool toBool() const
		{
			return
				caseCmp( "true" )    ||
				caseCmp( "yes" )     ||
				caseCmp( "on" )      ||
				caseCmp( "enabled" ) ||
				caseCmp( "enable" )  ||
				cmp( "1" );
		}

		inline SizeType toEncoding( void *pDstBuf, SizeType cDstBytes, axstr_encoding_t enc, axstr_byteordermark_t bom = axstr_bom_disabled ) const
		{
			return axstr_to_encoding_n( pDstBuf, cDstBytes, m_pStr, m_cStr, enc, bom );
		}
		inline wchar_t *toWStr( AXSTR_OUT_Z(cDstChars) wchar_t *pDstBuf, SizeType cDstChars ) const
		{
			pDstBuf[ 0 ] = L'\0';

			const axstr_utf16_t x = 0xABCD;
			const axstr_encoding_t enc =
				( *( const axstr_utf8_t * )&x == 0xAB )
				? (
					( sizeof( wchar_t ) == 2 ) ? (
						axstr_enc_utf16_be
					) : (
						axstr_enc_utf32_be
					)
				) : (
					( sizeof( wchar_t ) == 2 ) ? (
						axstr_enc_utf16_le
					) : (
						axstr_enc_utf32_le
					)
				);

			const axstr_size_t cBytesWritten = axstr_to_encoding_n( pDstBuf, cDstChars*sizeof( wchar_t ), m_pStr, m_cStr, enc, axstr_bom_disabled );

			if( cBytesWritten + sizeof( wchar_t ) > cDstChars*sizeof( wchar_t ) ) {
				return ( wchar_t * )0;
			}

			pDstBuf[ cBytesWritten/sizeof( wchar_t ) ] = L'\0';

			return pDstBuf;
		}
		template< axstr_size_t tNumWChars >
		inline wchar_t *toWStr( AXSTR_OUT_Z(tNumWChars) wchar_t( &dstBuf )[ tNumWChars ] ) const
		{
			return toWStr( &dstBuf[0], tNumWChars );
		}

		/*! \brief Determine whether a string is enclosed in quotes. */
		inline bool isQuoted() const
		{
			return startsWith( '\"' ) && endsWith( '\"' ) && len() >= 2;
		}

#if AXSTR_OBJMUTSTR_ENABLED
		bool quoteTo( ObjMutStr &dst ) const;
		bool unquoteTo( ObjMutStr &dst ) const;
		bool tabTo( ObjMutStr &dst, SizeType levels = 1 ) const;
		bool untabTo( ObjMutStr &dst, SizeType levels = 1 ) const;
#endif

		template< typename Allocator >
		bool quoteTo( TMutStr< Allocator > &dst ) const;
		template< typename Allocator >
		bool unquoteTo( TMutStr< Allocator > &dst ) const;
		template< typename Allocator >
		bool tabTo( TMutStr< Allocator > &dst, SizeType levels = 1 ) const;
		template< typename Allocator >
		bool untabTo( TMutStr< Allocator > &dst, SizeType levels = 1 ) const;

		TMutStr<> quoted() const;
		Str unquoted() const;
		TMutStr<> tabbed( SizeType levels = 1 ) const;
		TMutStr<> untabbed( SizeType levels = 1 ) const;

		template< typename Allocator >
		bool escapeTo( TMutStr< Allocator > &dst ) const;
		template< typename Allocator >
		bool unescapeTo( TMutStr< Allocator > &dst ) const;

		TMutStr<> escaped() const;
		TMutStr<> unescaped() const;

		template< typename Allocator >
		bool replaceTo( TMutStr< Allocator > &dst, Str searchFor, Str replaceWith ) const;
		TMutStr<> replaced( Str searchFor, Str replaceWith ) const;

		inline const char *pointer( DiffType index = 0 ) const
		{
			return SizeType( index ) < m_cStr ? &m_pStr[ index ] : 0;
		}

		/*! \brief Total amount of memory this object is using. */
		inline AllocSizeType memSize() const
		{
			return sizeof( *this );
		}
		/*! \brief Maximum capacity of the current object (before
		 *         reallocation). */
		inline SizeType max() const
		{
			return 0;
		}
		/*! \brief Current length of the string in bytes (not including the
		 *         `NUL`-terminator). */
		inline SizeType len() const
		{
			return m_cStr;
		}
		/*! \brief Same as `len()` but returned as an `int` type for `printf()`
		 *         compatibility. */
		inline int lenInt() const
		{
			return (int)m_cStr;
		}
		/*! \brief Current length of the string in bytes, including the
		 *         `NUL`-terminator. */
		inline SizeType num() const
		{
			return m_cStr;
		}

		/*! \brief Determine whether this string is empty. */
		inline bool isEmpty() const
		{
			return !m_cStr;
		}
		/*! \brief Determine whether this string is not empty. */
		inline bool isUsed() const
		{
			return m_cStr > 0;
		}

		/*! \brief Get a byte index clamped into bounds. */
		inline SizeType clampIndex( SizeType uOffset ) const
		{
			const SizeType n = m_cStr;
			return uOffset < n ? uOffset : n;
		}
		/*! \brief Retrieve a pointer based on an index, clamped if
		 *         necessary. */
		inline const char *clampIndexToPointer( SizeType uOffset ) const
		{
			return m_pStr + clampIndex( uOffset );
		}
		/*! \brief Get a byte length clamped into bounds. */
		inline SizeType clampLength( SizeType uOffset, SizeType cLen ) const
		{
			const SizeType s = clampIndex( uOffset );
			const SizeType e = clampIndex( uOffset + cLen );
			return e - s;
		}

		inline bool operator==( Str x ) const
		{
			return cmp( x );
		}
		inline bool operator!=( Str x ) const
		{
			return !cmp( x );
		}
		inline bool operator<( Str x ) const
		{
			return sortCmp( x ) < 0;
		}
		inline bool operator>( Str x ) const
		{
			return sortCmp( x ) > 0;
		}
		inline bool operator<=( Str x ) const
		{
			return sortCmp( x ) <= 0;
		}
		inline bool operator>=( Str x ) const
		{
			return sortCmp( x ) >= 0;
		}

		inline bool operator!() const
		{
			return !m_cStr;
		}
		inline char operator*() const
		{
			return m_cStr > 0 ? *m_pStr : '\0';
		}

		inline Str &operator++()
		{
			if( m_cStr > 0 ) {
				--m_cStr;
				++m_pStr;
			}

			return *this;
		}
		inline Str operator++( int )
		{
			Str temp( *this );
			++*this;
			return temp;
		}

		TMutStr<> operator+( Str x ) const;

		inline char operator[]( SizeType i ) const
		{
			return i < m_cStr ? m_pStr[ i ] : '\0';
		}

	private:
		const char *m_pStr;
		SizeType    m_cStr;

		/*! \internal */
		inline SizeType calcBytes_( DiffType n ) const
		{
			const SizeType cUnclampedBytes = n < 0 ? m_cStr + 1 + n : n;
			return cUnclampedBytes < m_cStr ? cUnclampedBytes : m_cStr;
		}
	};




	/*
	===========================================================================

		BASE MUTABLE STRING

		Modifiable/dynamic string class base.

	===========================================================================
	*/

	namespace policy
	{

		/*!
		 * \brief Default heap-based allocator, using whatever `axstr_alloc` and
		 *        `axstr_free` are defined to.
		 */
		struct DefaultStringAllocator
		{
			typedef axstr_size_t AllocSizeType;

			inline void *allocate( AllocSizeType cBytes, AllocSizeType &cAllocedBytes )
			{
				void *const p = axstr_alloc( cBytes );
				cAllocedBytes = !p ? 0 : cBytes;
				return p;
			}
			inline void deallocate( void *pBytes, AllocSizeType cBytes )
			{
				((void)cBytes);
				axstr_free( pBytes );
			}

			inline void swap( DefaultStringAllocator &, char *&a, char *&b )
			{
				char *const temp = a;
				a = b;
				b = temp;
			}
		};

		/*!
		 * \brief Null allocator, which always acts as if it's out-of-memory.
		 */
		struct NullStringAllocator
		{
			typedef axstr_size_t AllocSizeType;

			inline void *allocate( AllocSizeType, AllocSizeType &cAllocedBytes )
			{
				cAllocedBytes = 0;
				return ( void * )0;
			}
			inline void deallocate( void *, AllocSizeType )
			{
			}

			inline void swap( NullStringAllocator &, char *&a, char *&b )
			{
				char *const temp = a;
				a = b;
				b = temp;
			}
		};

		/*!
		 * \brief Small string allocator, which uses a base-size of some kind
		 *        for embedding the string until it reaches some limit.
		 */
		template< axstr_size_t tBaseSize, typename OverflowAllocator = DefaultStringAllocator >
		struct SmallStringAllocator: private OverflowAllocator
		{
			static_assert( tBaseSize > 0, "Must specify a valid initial size." );

			typedef axstr_size_t AllocSizeType;

			inline SmallStringAllocator()
			: OverflowAllocator()
			, m_usingSelf( false )
			{
			}

			inline void *allocate( AllocSizeType cBytes, AllocSizeType &cAllocedBytes )
			{
				if( !m_usingSelf && cBytes <= tBaseSize ) {
					m_usingSelf = true;
					cAllocedBytes = tBaseSize;
					return reinterpret_cast< void * >( &m_allocMem[ 0 ] );
				}

				return OverflowAllocator::allocate( cBytes, cAllocedBytes );
			}
			inline void deallocate( void *pBytes, AllocSizeType cBytes )
			{
				if( pBytes == reinterpret_cast< void * >( &m_allocMem[ 0 ] ) ) {
					AXSTR_ASSERT( m_usingSelf == true );
					AXSTR_ASSERT( cBytes == tBaseSize );

					m_usingSelf = false;
					return;
				}

				OverflowAllocator::deallocate( pBytes, cBytes );
			}

			inline void swap( SmallStringAllocator &x, char *&a, char *&b )
			{
				if( !ownsPointer( a ) && !x.ownsPointer( b ) ) {
					OverflowAllocator::swap( x, a, b );
					return;
				}

				if( ownsPointer( a ) && !x.ownsPointer( b ) ) {
					axstr__memcpy( x.m_allocMem, m_allocMem, tBaseSize );
					x.m_usingSelf = true;
					m_usingSelf = false;
					a = b;
					b = x.m_allocMem;
					return;
				}

				if( x.ownsPointer( b ) && !ownsPointer( a ) ) {
					axstr__memcpy( m_allocMem, x.m_allocMem, tBaseSize );
					m_usingSelf = true;
					x.m_usingSelf = false;
					b = a;
					a = x.m_allocMem;
					return;
				}

				char tempBuf[ tBaseSize ];
				axstr__memcpy( tempBuf, m_allocMem, tBaseSize );
				axstr__memcpy( m_allocMem, x.m_allocMem, tBaseSize );
				axstr__memcpy( x.m_allocMem, tempBuf, tBaseSize );
				a = m_allocMem;
				b = x.m_allocMem;
			}

		private:
			char          m_allocMem[ tBaseSize ];
			bool          m_usingSelf;

			inline bool ownsPointer( char *p ) const
			{
				return p >= &m_allocMem[0] && p <= &m_allocMem[ tBaseSize ];
			}
		};

	}

	namespace detail
	{

		class MutStrCore
		{
		template< typename Allocator >
		friend class TMutStr;
		public:
			typedef Str::DiffType DiffType;
			typedef Str::SizeType SizeType;

			/// \brief Retrieve an immutable view of this string.
			inline Str view() const
			{
				return Str( m_data, m_cLen );
			}

			/// \brief  Retrieve this as a C-style string.
			/// \return A valid pointer, always.
			inline const char *c_str() const
			{
				return m_data != ( char * )0 ? m_data : "";
			}

			/// \brief  Retrieve the base pointer to this string.
			inline const char *get() const
			{
				return m_data;
			}
			/// \brief Retrieve the end pointer to this string.
			inline const char *getEnd() const
			{
				return m_data + m_cLen;
			}

			/// \brief Retrieve the base pointer to this string for modification purposes
			inline char *data()
			{
				return m_data;
			}

			/// \brief  Find a given byte within the string.
			/// \return `-1` if not found, or the zero-based index to the byte.
			inline DiffType find( char ch, DiffType iAfter = -1 ) const
			{
				return view().find( ch, iAfter );
			}
			/// \brief  Find a given substring within the string.
			/// \return `-1` if not found, or the zero-based index to the first
			///         byte of the substring otherwise.
			inline DiffType find( Str inStr, DiffType iAfter = -1 ) const
			{
				return view().find( inStr, iAfter );
			}
			/// \brief  Find the first occurrence of any of the given characters
			///         within the string.
			/// \return `-1` if not found, or the zero-based index to the first
			///         byte matching the given characters that was found.
			inline DiffType findAny( const char *pszChars, DiffType iAfter = -1 ) const
			{
				return view().findAny( pszChars, iAfter );
			}

			/// \brief  Find a given byte within the string. Case insensitive.
			/// \return `-1` if not found, or the zero-based index to the
			///         byte.
			inline DiffType caseFind( char ch, DiffType iAfter = -1 ) const
			{
				return view().caseFind( ch, iAfter );
			}
			/// \brief  Find a given substring within the string. Case
			///         insensitive.
			/// \return `-1` if not found, or the zero-based index to the first
			///         byte of the substring otherwise.
			inline DiffType caseFind( Str inStr, DiffType iAfter = -1 ) const
			{
				return view().caseFind( inStr, iAfter );
			}
			/// \brief  Find the first occurrence of any of the given characters
			///         within the string. Case insensitive.
			/// \return `-1` if not found, or the zero-based index to the first
			///         byte matching the given characters that was found.
			inline DiffType caseFindAny( const char *pszChars, DiffType iAfter = -1 ) const
			{
				return view().caseFindAny( pszChars, iAfter );
			}

			/// \brief  Find the last occurrence of the given byte within the
			///         string.
			/// \return `-1` if not found, or the zero-based index to the byte.
			inline DiffType findLast( char c ) const
			{
				return view().findLast( c );
			}
			/// \brief  Find the last occurrence of the given substring within
			///         this string.
			/// \return `-1` if not found, or the zero-based index to the first
			///         byte of the substring.
			inline DiffType findLast( Str s ) const
			{
				return view().findLast( s );
			}

			/// \brief Get the byte index for a given column index within the
			///        string.
			inline DiffType columnToByteIndex( SizeType columnIndex ) const
			{
				return view().columnToByteIndex( columnIndex );
			}
			/// \brief Get the column index from a given byte index within the
			///        string.
			inline DiffType byteToColumnIndex( SizeType byteIndex ) const
			{
				return view().byteToColumnIndex( byteIndex );
			}
			/// \brief Return the length of this string in columns.
			inline SizeType numColumns() const
			{
				return SizeType( byteToColumnIndex( m_cLen ) );
			}

			/// \brief Same as `find(char,DiffType)` but returns a pointer.
			inline const char *findPtr( char ch, DiffType iAfter = -1 ) const
			{
				return pointer( find( ch, iAfter ) );
			}
			/// \brief Same as `find(Str,DiffType)` but returns a pointer.
			inline const char *findPtr( Str inStr, DiffType iAfter = -1 ) const
			{
				return pointer( find( inStr, iAfter ) );
			}
			/// \brief Same as `findAny(const char*,DiffType)` but returns a
			///        pointer.
			inline const char *findAnyPtr( const char *pszChars, DiffType iAfter = -1 ) const
			{
				return pointer( findAny( pszChars, iAfter ) );
			}

			/// \brief Same as `caseFind(char,DiffType)` but returns a pointer.
			inline const char *caseFindPtr( char ch, DiffType iAfter = -1 ) const
			{
				return pointer( caseFind( ch, iAfter ) );
			}
			/// \brief Same as `caseFind(Str,DiffType)` but returns a pointer.
			inline const char *caseFindPtr( Str inStr, DiffType iAfter = -1 ) const
			{
				return pointer( caseFind( inStr, iAfter ) );
			}
			/// \brief Same as `caseFindAny(const char*,DiffType)` but returns a
			///        pointer.
			inline const char *caseFindAnyPtr( const char *pszChars, DiffType iAfter = -1 ) const
			{
				return pointer( caseFindAny( pszChars, iAfter ) );
			}

			/// \brief Same as `findLast(char)` but returns a pointer.
			inline const char *findLastPtr( char c ) const
			{
				return pointer( findLast( c ) );
			}
			/// \brief Same as `findLast(Str)` but returns a pointer.
			inline const char *findLastPtr( Str s ) const
			{
				return pointer( findLast( s ) );
			}

			/// \brief Split this string based on a delimiter to an array.
			template< typename TArrayContainer >
			inline bool splitTo( TArrayContainer &outArray, Str inSeparator, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const
			{
				return view().splitTo< TArrayContainer >( outArray, inSeparator, keepEmpty );
			}
			template< typename TArrayContainer >
			inline TArrayContainer split( Str inSeparator, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
				TArrayContainer container;
				splitTo< TArrayContainer >( container, inSeparator, keepEmpty );
				return container;
			}

			/// \brief Split this string based on a delimiter, but treats quoted
			///        items as separate tokens.
			template< typename TArrayContainer >
			inline bool splitUnquotedTo( TArrayContainer &outArray, Str inSeparator, Str inEscape = '\\', EKeepQuotes::Type keepQuotes = EKeepQuotes::No, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const
			{
				return view().splitUnquotedTo< TArrayContainer >( outArray, inSeparator, inEscape, keepQuotes, keepEmpty );
			}
			template< typename TArrayContainer >
			inline TArrayContainer splitUnquoted( Str inSeparator, Str inEscape = '\\', EKeepQuotes::Type keepQuotes = EKeepQuotes::No, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
				TArrayContainer container;
				splitUnquotedTo< TArrayContainer >( container, inSeparator, inEscape, keepQuotes, keepEmpty );
				return container;
			}

			/// \brief Split this string by lines.
			template< typename TArrayContainer >
			inline bool splitLinesTo( TArrayContainer &outArr, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const
			{
				return view().splitLinesTo< TArrayContainer >( outArr, keepEmpty );
			}
			template< typename TArrayContainer >
			inline TArrayContainer splitLines( EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
				TArrayContainer container;
				splitLinesTo< TArrayContainer >( container, keepEmpty );
				return container;
			}

			/// \brief Find the index of the extension part of a filename within
			///        this string.
			inline DiffType findExtension() const
			{
				return view().findExtension();
			}

			/// \brief Find the index of the first directory separator within
			///        this string.
			inline DiffType findDirSep( const DiffType iAfter = -1 ) const
			{
				return view().findDirSep( iAfter );
			}
			/// \brief Find the index of the last directory separator within
			///        this string.
			inline DiffType findLastDirSep() const
			{
				return view().findLastDirSep();
			}

			/// \brief Check whether this string starts with a directory
			///        separator.
			inline bool startsWithDirSep() const
			{
				return view().startsWithDirSep();
			}
			/// \brief Check whether this string ends with a directory
			///        separator.
			inline bool endsWithDirSep() const
			{
				return view().endsWithDirSep();
			}

			/// \brief Retrieve the extension of this string as a separate
			///        string view.
			inline Str getExtension() const
			{
				return view().getExtension();
			}
			/// \brief Retrieve the filename part of this string as a separate
			///        string view.
			inline Str getFilename() const
			{
				return view().getFilename();
			}
			/// \brief Retrieve the directory part of this string as a separate
			///        string view.
			inline Str getDirectory() const
			{
				return view().getDirectory();
			}
			/// \brief Retrieve the basename part of this string as a separate
			///        string view.
			inline Str getBasename() const
			{
				return view().getBasename();
			}
			/// \brief Retrieve the root part of this string as a separate
			///        string view.
			inline Str getRoot() const
			{
				return view().getRoot();
			}

#if 0
			/// \brief Concatenate this string with `path` with path semantics
			///        into `outStr`.
			template< typename OtherAllocator >
			bool tryAppendPathTo( TMutStr< OtherAllocator > &outStr, Str path ) const
			{
				return view().tryAppendPathTo( outStr, path );
			}
#endif

			/// \brief Check whether this string is equal to another,
			///        case-insensitive.
			inline bool caseCmp( Str x ) const
			{
				return view().caseCmp( x );
			}
			/// \brief Check whether this string is equal to another,
			///        case-sensitive.
			inline bool cmp( Str x ) const
			{
				return view().cmp( x );
			}

			/// \brief Determine whether this string should come before or after
			///        another for sorting purposes, case-insensitive.
			inline int sortCaseCmp( Str x ) const
			{
				return view().sortCaseCmp( x );
			}
			/// \brief Determine whether this string should come before or after
			///        another for sorting purposes, case-sensitive.
			inline int sortCmp( Str x ) const
			{
				return view().sortCmp( x );
			}

			/// \brief Determine whether this string starts with another.
			inline bool startsWith( Str x ) const
			{
				return view().startsWith( x );
			}
			/// \brief Determine whether this string ends with another.
			inline bool endsWith( Str x ) const
			{
				return view().endsWith( x );
			}
			/// \brief Determine whether this string contains another.
			inline bool contains( Str x ) const
			{
				return view().contains( x );
			}

			/// \brief Determine whether this string starts with a given byte.
			inline bool startsWith( char x ) const
			{
				return m_cLen > 0 && *m_data == x;
			}
			/// \brief Determine whether this string ends with a given byte.
			inline bool endsWith( char x ) const
			{
				return m_cLen > 0 && m_data[ m_cLen - 1 ] == x;
			}
			/// \brief Determine whether this string contains a given byte.
			inline bool contains( char x ) const
			{
				return view().contains( x );
			}

			/// \brief Determine whether this string starts with any of the
			///        bytes of another string.
			inline bool startsWithAny( Str x ) const
			{
				return view().startsWithAny( x );
			}
			/// \brief Determine whether this string ends with any of the bytes
			///        of another string.
			inline bool endsWithAny( Str x ) const
			{
				return view().endsWithAny( x );
			}
			/// \brief Determine whether this string contains any of the bytes
			///        of another string.
			inline bool containsAny( Str x ) const
			{
				return view().containsAny( x );
			}

			/// \brief Determine whether this string starts with another string,
			///        case-insensitive.
			inline bool caseStartsWith( Str x ) const
			{
				return view().caseStartsWith( x );
			}
			/// \brief Determine whether this string ends with another string,
			///        case-insensitive.
			inline bool caseEndsWith( Str x ) const
			{
				return view().caseEndsWith( x );
			}
			/// \brief Determine whether this string contains another string,
			///        case-insensitive.
			inline bool caseContains( Str x ) const
			{
				return view().caseContains( x );
			}

			/// \brief Determine whether this string starts with a given byte,
			///        case-insensitive.
			inline bool caseStartsWith( char x ) const
			{
				return view().caseStartsWith( x );
			}
			/// \brief Determine whether this string ends with a given byte,
			///        case-insensitive.
			inline bool caseEndsWith( char x ) const
			{
				return view().caseEndsWith( x );
			}
			/// \brief Determine whether this string contains a given byte,
			///        case-insensitive.
			inline bool caseContains( char x ) const
			{
				return view().caseContains( x );
			}

			/// \brief Determine whether this string starts with any of the
			///        bytes of another string, case-insensitive.
			inline bool caseStartsWithAny( Str x ) const
			{
				return view().caseStartsWithAny( x );
			}
			/// \brief Determine whether this string ends with any of the bytes
			///        of another string, case-insensitive.
			inline bool caseEndsWithAny( Str x ) const
			{
				return view().caseEndsWithAny( x );
			}
			/// \brief Determine whether this string contains any of the bytes
			///        of another string, case-insensitive.
			inline bool caseContainsAny( Str x ) const
			{
				return view().caseContainsAny( x );
			}

			/// \brief Retrieve the left `n`-bytes of this string.
			inline Str left( DiffType n ) const
			{
				return view().left( n );
			}
			/// \brief Retrieve the right `n`-bytes of this string.
			inline Str right( DiffType n ) const
			{
				return view().right( n );
			}
			/// \brief Retrieve `num`-bytes of this string starting from `pos`.
			inline Str mid( DiffType pos, SizeType num = 1 ) const
			{
				return view().mid( pos, num );
			}
			/// \brief Retrieve the substring of this string from `startPos` to
			///        `endPos`.
			inline Str substr( DiffType startPos, DiffType endPos ) const
			{
				return view().substr( startPos, endPos );
			}

			/// \brief Retrieve the first byte of the string.
			inline char firstByte() const
			{
				return m_cLen > 0 ? *m_data : '\0';
			}
			/// \brief Retrieve the last byte of the string.
			inline char lastByte() const
			{
				return m_cLen > 0 ? m_data[ m_cLen - 1 ] : '\0';
			}

			/// \brief Retrieve a view of this string with left-hand whitespace
			///        trimmed.
			inline Str trimmedLeft() const
			{
				return view().trimLeft();
			}
			/// \brief Retrieve a view of this string with right-hand whitespace
			///        trimmed.
			inline Str trimmedRight() const
			{
				return view().trimRight();
			}
			/// \brief Retrieve a view of this string with whitespace trimmed.
			inline Str trimmed() const
			{
				return view().trim();
			}

			/// \brief Determine whether `subtext` is a substring (in the same
			///        memory range) of this string.
			inline bool hasSubstring( Str subtext ) const
			{
				return view().hasSubstring( subtext );
			}
			/// \brief Determine whether this string is a substring of
			/// `supertext` (is in the same memory range as `supertext`).
			inline bool isSubstring( Str supertext ) const
			{
				return view().isSubstring( supertext );
			}

			/// \brief Convert a string to a signed integer.
			inline axstr_sint_t toInteger( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
			{
				return view().toInteger( defaultRadix, chDigitSep );
			}
			/// \brief Convert a string to an unsigned integer.
			inline axstr_uint_t toUnsignedInteger( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
			{
				return view().toUnsignedInteger( defaultRadix, chDigitSep );
			}
			/// \brief Convert a string to a float.
			inline axstr_real_t toFloat( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
			{
				return view().toFloat( defaultRadix, chDigitSep );
			}

			/// \brief Convert a string to a boolean.
			inline bool toBool() const
			{
				return view().toBool();
			}

			inline SizeType toEncoding( void *pDstBuf, SizeType cDstBytes, axstr_encoding_t enc, axstr_byteordermark_t bom = axstr_bom_disabled ) const
			{
				return view().toEncoding( pDstBuf, cDstBytes, enc, bom );
			}
			inline wchar_t *toWStr( AXSTR_OUT_Z(cDstChars) wchar_t *pDstBuf, SizeType cDstChars ) const
			{
				return view().toWStr( pDstBuf, cDstChars );
			}
			template< axstr_size_t tNumWChars >
			inline wchar_t *toWStr( AXSTR_OUT_Z(tNumWChars) wchar_t( &dstBuf )[ tNumWChars ] ) const
			{
				return view().toWStr( &dstBuf[0], tNumWChars );
			}

			/// \brief Determine whether a string is enclosed in quotes.
			inline bool isQuoted() const
			{
				return startsWith( '\"' ) && endsWith( '\"' ) && len() >= 2;
			}

			template< typename DestStringType >
			inline bool quoteTo( DestStringType &dst ) const
			{
				return view().quoteTo( dst );
			}
			template< typename DestStringType >
			inline bool unquoteTo( DestStringType &dst ) const
			{
				return view().unquoteTo( dst );
			}
			template< typename DestStringType >
			inline bool tabTo( DestStringType &dst, unsigned levels = 1 ) const
			{
				return view().tabTo( dst, levels );
			}
			template< typename DestStringType >
			inline bool untabTo( DestStringType &dst, unsigned levels = 1 ) const
			{
				return view().untabTo( dst, levels );
			}

			template< typename Allocator >
			inline bool escapeTo( TMutStr< Allocator > &dst ) const
			{
				return view().escapeTo( dst );
			}
			template< typename Allocator >
			inline bool unescapeTo( TMutStr< Allocator > &dst ) const
			{
				return view().unescapeTo( dst );
			}

			template< typename Allocator >
			bool replaceTo( TMutStr< Allocator > &dst, Str searchFor, Str replaceWith ) const
			{
				return view().replaceTo( dst, searchFor, replaceWith );
			}

			/// \brief Retrieve a pointer within the string from an index
			inline const char *pointer( DiffType index = 0 ) const
			{
				return SizeType( index ) <= m_cLen ? &m_data[ index ] : 0;
			}
			/// \brief Retrieve the internal pointer
			inline const char *ptr() const
			{
				return m_data;
			}

			/// \brief Current length of the string in bytes (not including the
			///        `NUL`-terminator).
			inline SizeType len() const
			{
				return m_cLen;
			}
			/// \brief Same as `len()` but returned as an `int` type for
			///        `printf()` compatibility.
			inline int lenInt() const
			{
				return int( m_cLen );
			}
			/// \brief Current length of the string in bytes, including the
			///        `NUL`-terminator.
			inline SizeType num() const
			{
				return m_cLen > 0 ? m_cLen + 1 : 0;
			}

			/// \brief Determine whether this string is empty.
			inline bool isEmpty() const
			{
				return !m_cLen;
			}
			/// \brief Determine whether this string is not empty.
			inline bool isUsed() const
			{
				return m_cLen > 0;
			}

			/// \brief Get a byte index clamped into bounds.
			inline SizeType clampIndex( SizeType uOffset ) const
			{
				return uOffset < m_cLen ? uOffset : m_cLen;
			}
			/// \brief Retrieve a pointer based on an index, clamped if
			///        necessary.
			inline const char *clampIndexToPointer( SizeType uOffset ) const
			{
				return m_data + clampIndex( uOffset );
			}
			/// \brief Get a byte length clamped into bounds.
			inline SizeType clampLength( SizeType uOffset, SizeType cLen ) const
			{
				// Handle overflow
				if( uOffset + cLen < uOffset ) {
					return m_cLen;
				}

				const SizeType s = clampIndex( uOffset );
				const SizeType e = clampIndex( uOffset + cLen );

				return e - s;
			}
			/// \brief Convert a position into an offset
			inline SizeType positionToOffset( DiffType pos ) const
			{
				if( pos < 0 ) {
					pos = m_cLen + 1 + pos;
					if( pos < 0 ) {
						pos = 0;
					}
				}

				return clampIndex( SizeType( pos ) );
			}
			/// \brief Convert a position range into starting and ending offsets
			inline void convertPositionRangeToOffsets( SizeType &outStartOff, SizeType &outEndOff, DiffType startPos, DiffType endPos ) const
			{
				const SizeType startOff = positionToOffset( startPos );
				const SizeType endOff = positionToOffset( endPos );

				const SizeType minOff = startOff < endOff ? startOff : endOff;
				const SizeType maxOff = startOff < endOff ? endOff : startOff;

				outStartOff = minOff;
				outEndOff = maxOff;
			}

			/// \brief Clear the string.
			inline void clear()
			{
				if( m_cLen > 0 ) {
					m_cLen = 0;
					*m_data = '\0';
				}
			}

			/// \brief Replace all backslashes with forward slashes.
			inline void backToForwardSlashes()
			{
				Str x( m_data, m_data + m_cLen );
				DiffType iLast = -1;
				for(;;) {
					iLast = x.find( '\\', iLast );
					if( iLast < 0 ) {
						break;
					}

					m_data[ iLast ] = '/';
				}
			}
			/// \brief Replace all forward slashes with backslashes.
			inline void forwardToBackSlashes()
			{
				Str x( m_data, m_data + m_cLen );
				DiffType iLast = -1;
				for(;;) {
					iLast = x.find( '/', iLast );
					if( iLast < 0 ) {
						break;
					}

					m_data[ iLast ] = '\\';
				}
			}

			/// \brief Sanitize for use as a file name.
			inline void sanitizeFilename( char chReplacement = '-' )
			{
				// FIXME: Probably faster to just use a small table.
				//   "  U+0022
				//   *  U+002A
				//   /  U+002F
				//   :  U+003A
				//   <  U+003C
				//   >  U+003E
				//   ?  U+003F
				//   \  U+005C
				//   |  U+007C
				static const char *const pszSpecialChars = "\"*/:<>?\\|";

				char *const e = m_data + m_cLen;
				for( char *p = m_data; p < e; ++p ) {
					if( *( unsigned char * )p < 0x20 ) {
						*p = chReplacement;
						continue;
					}

#if AXSTR_STDSTR_ENABLED
					if( !strchr( pszSpecialChars, *p ) ) {
						continue;
					}
#else
					const char *q;
					for( q = pszSpecialChars; *q != '\0'; ++q ) {
						if( *p == *q ) {
							q = ( const char * )0;
							break;
						}
					}

					if( q != ( const char * )0 ) {
						continue;
					}
#endif

					*p = chReplacement;
				}
			}

			/// \brief Remove whitespace from the left.
			inline void trimLeft()
			{
				const SizeType cLen = m_cLen;
				unsigned char *const p = reinterpret_cast< unsigned char * >( m_data );

				if( !cLen ) {
					return;
				}

				SizeType cWhite = 0;

				while( cWhite < cLen ) {
					if( p[ cWhite ] > ' ' ) {
						break;
					}

					++cWhite;
				}

				if( !cWhite ) {
					return;
				}

				const SizeType n = cLen - cWhite;

				axstr__memmove( ( void * )p, m_cLen, ( const void * )( p + cWhite ), n );

				m_cLen = n;
				p[ m_cLen ] = '\0';
			}
			/// \brief Drop whitespace from the right.
			inline void trimRight()
			{
				const SizeType cLen = m_cLen;
				unsigned char *const p = reinterpret_cast< unsigned char * >( m_data );

				if( !cLen ) {
					return;
				}

				SizeType cWhite = 0;

				while( cWhite < cLen ) {
					if( p[ cLen - cWhite - 1 ] > ' ' ) {
						break;
					}

					++cWhite;
				}

				m_cLen = cLen - cWhite;
				p[ m_cLen ] = '\0';
			}
			/// \brief Remove whitespace from both sides.
			inline void trim()
			{
				trimRight();
				trimLeft();
			}

			/// \brief Remove a set of bytes from the string.
			inline void removeByPositions( DiffType startPos, DiffType endPos )
			{
				SizeType startOff, endOff;
				convertPositionRangeToOffsets( startOff,endOff, startPos,endPos );

				const SizeType cLen = endOff - startOff;
				removeByOffsetAndSize( startOff, cLen );
			}
			template< typename OtherAllocator >
			inline bool removeByPositionsTo( TMutStr< OtherAllocator > &dst, DiffType startPos, DiffType endPos ) const
			{
				SizeType startOff, endOff;
				convertPositionRangeToOffsets( startOff,endOff, startPos,endPos );

				const SizeType cLen = endOff - startOff;
				return removeByOffsetAndSizeTo( dst, startOff, cLen );
			}

			/// \brief Remove a set of bytes from the string.
			inline void removeByOffsetAndSize( SizeType uOffset, SizeType cLen )
			{
				const SizeType clampedOffset = clampIndex( uOffset );
				const SizeType clampedLength = clampLength( clampedOffset, cLen );

				if( !clampedLength ) {
					return;
				}

				const SizeType cLeftBytes = clampedOffset;
				const SizeType cRightBytes = m_cLen - clampedLength - clampedOffset;

				const SizeType cRemaining = cLeftBytes + cRightBytes;

				axstr__memmove( m_data + cLeftBytes, m_cLen - cLeftBytes, m_data + ( m_cLen - cRightBytes ), cRightBytes );
				m_cLen = cRemaining;
				m_data[ m_cLen ] = '\0';
			}

			template< typename OtherAllocator >
			inline bool removeByOffsetAndSizeTo( TMutStr< OtherAllocator > &dst, SizeType uOffset, SizeType cLen ) const
			{
				const SizeType clampedOffset = clampIndex( uOffset );
				const SizeType clampedLength = clampLength( clampedOffset, cLen );

				dst.clear();
				if( !clampedLength ) {
					return true;
				}

				const SizeType cLeftBytes = clampedOffset;
				const SizeType cRightBytes = m_cLen - clampedLength - clampedOffset;

				const SizeType cRemaining = cLeftBytes + cRightBytes;

				if( !dst.reserve( cRemaining ) ) {
					return false;
				}

				bool r = true;
				r = r && dst.tryAssign( view().left( cLeftBytes ) );
				r = r && dst.tryAppend( view().right( cRightBytes ) );

				if( !r ) {
					dst.clear();
					return false;
				}

				return true;
			}

			/// \brief Skip past a section of bytes in this string
			inline void skipMe( SizeType cBytes = 1 )
			{
				if( !m_cLen ) {
					return;
				}

				if( cBytes > m_cLen ) {
					cBytes = m_cLen;
				}

				axstr__memmove( m_data, m_cLen, m_data + cBytes, cBytes );
				m_cLen -= cBytes;
				m_data[ m_cLen ] = '\0';
			}
			/// \brief Drop ending section of bytes from this string
			inline void dropMe( SizeType cBytes = 1 )
			{
				if( !m_cLen ) {
					return;
				}

				if( cBytes > m_cLen ) {
					cBytes = m_cLen;
				}

				m_cLen -= cBytes;
				m_data[ m_cLen ] = '\0';
			}

			/// \brief Truncate this string such that it fits within the given range
			inline void truncate( SizeType cBytes )
			{
				if( m_cLen <= cBytes ) {
					return;
				}

				m_cLen = cBytes;
				m_data[ m_cLen ] = '\0';
			}

			inline bool operator==( Str x ) const
			{
				return view().cmp( x );
			}
			inline bool operator!=( Str x ) const
			{
				return !view().cmp( x );
			}
			inline bool operator<( Str x ) const
			{
				return view().sortCmp( x ) < 0;
			}
			inline bool operator>( Str x ) const
			{
				return view().sortCmp( x ) > 0;
			}
			inline bool operator<=( Str x ) const
			{
				return view().sortCmp( x ) <= 0;
			}
			inline bool operator>=( Str x ) const
			{
				return view().sortCmp( x ) >= 0;
			}

			inline bool operator!() const
			{
				return !m_cLen;
			}
			inline char operator*() const
			{
				return firstByte();
			}

			inline char operator[]( SizeType i ) const
			{
				return i < m_cLen ? m_data[ i ] : '\0';
			}

		protected:
			char *   m_data;
			SizeType m_cLen;

			MutStrCore()
			: m_data( ( char * )0 )
			, m_cLen( 0 )
			{
			}
			MutStrCore( const MutStrCore &x )
			: m_data( x.m_data )
			, m_cLen( x.m_cLen )
			{
			}
#if AXSTR_CXX11_MOVE_ENABLED
			MutStrCore( MutStrCore &&x )
			: m_data( x.m_data )
			, m_cLen( x.m_cLen )
			{
				x.m_data = ( char * )0;
				x.m_cLen = 0;
			}
#endif
			MutStrCore( char *p, SizeType cLen )
			: m_data( p )
			, m_cLen( cLen )
			{
			}
		};

	}

	template< typename Allocator >
	class TMutStr: private Allocator, public detail::MutStrCore
	{
	public:
		typedef TMutStr< Allocator >              Self;
		typedef detail::MutStrCore::DiffType      DiffType;
		typedef detail::MutStrCore::SizeType      SizeType;
		typedef typename Allocator::AllocSizeType AllocSizeType;

		TMutStr()
		: Allocator()
		, detail::MutStrCore()
		, m_cAllocedBytes( 0 )
		{
		}
		TMutStr( const TMutStr &other )
		: Allocator( other )
		, detail::MutStrCore()
		, m_cAllocedBytes( 0 )
		{
			assign( other );
		}
		TMutStr( Str other )
		: Allocator()
		, detail::MutStrCore()
		, m_cAllocedBytes( 0 )
		{
			assign( other );
		}
#if AXSTR_CXX11_MOVE_ENABLED
		TMutStr( TMutStr &&x )
		: Allocator( static_cast< Allocator && >( x ) )
		, detail::MutStrCore( static_cast< detail::MutStrCore && >( x ) )
		, m_cAllocedBytes( x.m_cAllocedBytes )
		{
			x.m_cAllocedBytes = 0;
		}
		template< typename A >
		TMutStr( TMutStr< A > &&x )
		: Allocator( static_cast< Allocator && >( x ) )
		, detail::MutStrCore( static_cast< detail::MutStrCore && >( x ) )
		, m_cAllocedBytes( x.m_cAllocedBytes )
		{
			x.m_cAllocedBytes = 0;
		}
#endif
		~TMutStr()
		{
		}

		/// \brief Total amount of memory this object is using
		inline AllocSizeType memSize() const
		{
			return sizeof( *this ) + m_cAllocedBytes;
		}
		/// \brief Maximum capacity of the current object (before reallocation)
		inline SizeType max() const
		{
			return SizeType( m_cAllocedBytes > 0 ? m_cAllocedBytes - 1 : 0 );
		}

		/// \brief  Prepare this string to hold enough space for a string of
		///         `cLen`-bytes.
		///
		/// If this fails, this object is *still* valid.
		///
		/// \return `true` if this succceeded; `false` otherwise.
		inline bool reserve( SizeType cLen )
		{
			if( m_cAllocedBytes >= cLen + 1 ) {
				return true;
			}

			const SizeType n = cLen + ( 16 - ( cLen + 15 )%16 );

			SizeType cAllocedBytes = 0;
			char *const p = ( char * )Allocator::allocate( n, cAllocedBytes );
			if( !p ) {
				return false;
			}

			if( m_cLen > cLen ) {
				m_cLen = cLen;
			}

			if( m_cLen > 0 ) {
				axstr__memcpy( ( void * )p, ( const void * )m_data, m_cLen + 1 );
			} else {
				*p = '\0';
			}

			Allocator::deallocate( ( void * )m_data, m_cAllocedBytes );
			m_data = p;

			m_cAllocedBytes = cAllocedBytes;
			return true;
		}
		/// \brief  As `reserve()` but also sets the length of the string, event
		///         if there would be uninitialized memory.
		///
		/// This is primarily intended for use in operations that will
		/// immediately fill the buffer with data, such as reading directly from
		/// a file into this string's memory buffer.
		inline bool reserveAndSetLen( SizeType cLen )
		{
			if( !reserve( cLen ) ) {
				return false;
			}

			m_cLen = cLen;
			m_data[ m_cLen ] = '\0';

			return true;
		}
		/// Remove all dynamic memory associated with this string.
		inline TMutStr &purge()
		{
			Allocator::deallocate( m_data, m_cAllocedBytes );
			m_cAllocedBytes = 0;
			m_data = ( char * )0;

			return *this;
		}

		inline TMutStr removedByPositions( DiffType startPos, DiffType endPos ) const
		{
			TMutStr< Allocator > r;

			if( !removeByPositionsTo( r, startPos, endPos ) ) {
				// FIXME: Signal error (axstr_cxx_error or something)
				((void)0);
			}

			return r;
		}
		inline TMutStr removedByOffsetAndSize( SizeType uOffset, SizeType cLen ) const
		{
			TMutStr< Allocator > r;

			if( !removeByOffsetAndSizeTo( r, uOffset, cLen ) ) {
				// FIXME: Signal error (axstr_cxx_error or something)
				((void)0);
			}

			return r;
		}

		/// \brief Extract a substring, removing it from this string.
		template< typename OtherAllocator >
		inline bool extractByPositionsTo( TMutStr< OtherAllocator > &dst, DiffType startPos, DiffType endPos )
		{
			SizeType startOff, endOff;
			convertPositionRangeToOffsets( startOff, endOff, startPos, endPos );

			const SizeType cLen = endOff - startOff;
			return extractByOffsetAndSizeTo( dst, startOff, cLen );
		}
		/// \brief Extract a substring, removing it from this string.
		inline TMutStr extractByPositions( DiffType startPos, DiffType endPos )
		{
			TMutStr dst;
			extractByPositionsTo( dst, startPos, endPos );
			return dst;
		}

		/// \brief Extract a substring, removing it from this string.
		template< typename OtherAllocator >
		inline bool extractByOffsetAndSizeTo( TMutStr< OtherAllocator > &dst, SizeType uOffset, SizeType cLength )
		{
			if( uOffset + cLength < uOffset ) {
				return false;
			}

			if( uOffset > m_cLen ) {
				uOffset = m_cLen;
			}
			if( uOffset + cLength > m_cLen ) {
				cLength = m_cLen - uOffset;
			}

			if( !cLength ) {
				dst.clear();
				return true;
			}

			const char *const s = m_data + uOffset;
			const char *const e = s + cLength;

			if( !dst.tryAssign( Str( s, e ) ) ) {
				return false;
			}

			removeByOffsetAndSize( uOffset, cLength );
			return true;
		}
		/// \brief Extract a substring, removing it from this string.
		inline TMutStr extractByOffsetAndSize( DiffType startPos, DiffType endPos )
		{
			TMutStr dst;
			extractByOffsetAndSizeTo( dst, startPos, endPos );
			return dst;
		}

		/// \brief Assign text to this string.
		inline bool tryAssign( Str s )
		{
			if( m_cAllocedBytes < s.len() + 1 && !reserve( s.len() ) ) {
				return false;
			}

			axstr__memcpy( ( void * )m_data, ( const void * )s.get(), s.len() );

			m_cLen = s.len();
			m_data[ m_cLen ] = '\0';

			return true;
		}
		inline TMutStr &assign( Str s )
		{
			( void )tryAssign( s ); // FIXME: Error handler on fail
			return *this;
		}

		// Insert text at the beginning of the string
		inline bool tryPrepend( Str s )
		{
			if( s.isEmpty() ) {
				return true;
			}

			if( m_cLen + s.len() < m_cLen || !reserve( m_cLen + s.len() ) ) {
				return false;
			}

			axstr__memmove( m_data + s.len(), m_cLen, m_data, m_cLen );
			axstr__memcpy( m_data, reinterpret_cast< const void * >( s.get() ), s.len() );

			m_cLen += s.len();
			return true;
		}
		inline TMutStr &prepend( Str s )
		{
			( void )tryPrepend( s ); // FIXME: Error handler on fail
			return *this;
		}
		inline TMutStr prepended( Str s ) const
		{
			TMutStr<> dst;

			if( m_cLen + s.len() < m_cLen ) {
				// FIXME: Error handler on fail
				return TMutStr<>();
			}

			if( !dst.reserve( m_cLen + s.len() ) ) {
				// FIXME: Error handler on fail
				return TMutStr<>();
			}

			dst.assign( s );
			dst.append( view() );

			return dst;
		}

		/// \brief Insert text at the end of the string.
		inline bool tryAppend( Str s )
		{
			if( m_cLen + s.len() < m_cLen || !reserve( m_cLen + s.len() ) ) {
				return false;
			}

			axstr__memcpy( m_data + m_cLen, reinterpret_cast< const void * >( s.get() ), s.len() );
			m_cLen += s.len();

			m_data[ m_cLen ] = '\0';
			return true;
		}
		inline bool tryAppend( const char *s, const char *e )
		{
			return tryAppend( Str( s, e ) );
		}
		inline TMutStr &append( Str s )
		{
			if( !tryAppend( s ) ) {
				// FIXME: Signal error!
			}

			return *this;
		}
		inline TMutStr &append( const char *s, const char *e )
		{
			return append( Str( s, e ) );
		}
		inline TMutStr appended( Str s ) const
		{
			TMutStr<> dst;

			if( m_cLen + s.len() < m_cLen ) {
				// FIXME: Signal error
				return TMutStr<>();
			}

			if( !dst.reserve( m_cLen + s.len() ) ) {
				// FIXME: Signal error
				return TMutStr<>();
			}

			dst.assign( view() );
			dst.append( s );

			return dst;
		}
		inline TMutStr appended( const char *s, const char *e ) const
		{
			return appended( s, e );
		}

		inline bool tryAppendUTF32Char( axstr_utf32_t utf32Char )
		{
			char buf[ 5 ] = { '\0', '\0', '\0', '\0', '\0' };
			{
				axstr_utf8_t *bp = (axstr_utf8_t*)&buf[0];
				axstr_utf8_t *const be = (axstr_utf8_t*)&buf[sizeof(buf)-1];

				if( !axstr_step_utf8_encode( &bp, be, utf32Char ) ) {
					return false;
				}
			}

			return tryAppend( Str( buf ) );
		}
		inline TMutStr &appendUTF32Char( axstr_utf32_t utf32Char )
		{
			if( !tryAppendUTF32Char( utf32Char ) ) {
				// FIXME: Signal an error
			}

			return *this;
		}
		inline TMutStr appendedUTF32Char( axstr_utf32_t utf32Char ) const
		{
			return TMutStr(*this).appendUTF32Char( utf32Char );
		}

#ifdef INCGUARD_AX_PRINTF_H_
	private:
		static inline axpf_ptrdiff_t
		AXPF_CALL axpfWrite_f
		(
			void *data,
			const char *s,
			const char *e
		)
		{
			Self *md;

			md = reinterpret_cast< Self * >( data );
			if( !md ) {
				return axpf_ptrdiff_t( -1 );
			}

			const SizeType oldLen = md->len();
			if( !md->tryAppend( Str( s, e ) ) ) {
				return axpf_ptrdiff_t( -1 );
			}

			const SizeType newLen = md->len();

			const axpf_ptrdiff_t cWritten = axpf_ptrdiff_t( axpf_size_t( newLen - oldLen ) );
			return cWritten;
		}

	public:
		inline bool appendfv( AXPF_PARM_ANNO Str formatStr, va_list args ) AXPF_FUNC_ANNO(1,2)
		{
			struct axpf__state_ s;

			s.pfn_write  = &axpfWrite_f;
			s.write_data = reinterpret_cast< void * >( this );

			const SizeType cLen = m_cLen;
			const axpf_ptrdiff_t r = axpf__main( &s, formatStr.get(), formatStr.getEnd(), args );
			if( r < 0 ) {
				m_cLen = cLen;
				if( m_data != ( char * )0 ) {
					m_data[ m_cLen ] = '\0';
				}

				return false;
			}

			return true;
		}

		inline bool appendf( AXPF_PARM_ANNO Str formatStr, ... ) AXPF_FUNC_ANNO( 1, 2 )
		{
			va_list args;

			va_start( args, formatStr );
			const bool r = appendfv( formatStr, args );
			va_end( args );

			return r;
		}

		inline bool formatv( AXPF_PARM_ANNO Str formatStr, va_list args ) AXPF_FUNC_ANNO( 1, 2 )
		{
			clear();
			return appendfv( formatStr, args );
		}
		inline bool format( AXPF_PARM_ANNO Str formatStr, ... ) AXPF_FUNC_ANNO( 1, 2 )
		{
			va_list args;

			va_start( args, formatStr );
			const bool r = formatv( formatStr, args );
			va_end( args );

			return r;
		}

		static inline TMutStr formattedv( AXPF_PARM_ANNO Str formatStr, va_list args ) AXPF_FUNC_ANNO( 1, 2 )
		{
			Self dst;

			if( !dst.appendfv( formatStr, args ) ) {
				return Self();
			}

			return dst;
		}
		static inline TMutStr formatted( AXPF_PARM_ANNO Str formatStr, ... ) AXPF_FUNC_ANNO( 1, 2 )
		{
			va_list args;

			va_start( args, formatStr );
			TMutStr<> r = formattedv( formatStr, args );
			va_end( args );

			return r;
		}
#endif

		/// \brief Concatenate a path to the string.
		inline bool tryAppendPath( Str s, char chPathSep = AXSTR_DIRSEP_CH )
		{
			if( !endsWithDirSep() && !s.startsWithDirSep() ) {
				if( m_cLen + s.len() + 1 <= m_cLen || !reserve( m_cLen + s.len() + 1 ) ) {
					return false;
				}

				const SizeType cLen = m_cLen;
				if( !tryAppend( chPathSep ) || !tryAppend( s ) ) {
					truncate( cLen );
					return false;
				}

				return true;
			}

			return tryAppend( s );
		}
		inline TMutStr &appendPath( Str s, char chPathSep = AXSTR_DIRSEP_CH )
		{
			if( !tryAppendPath( s, chPathSep ) ) {
				// FIXME: Signal error
			}

			return *this;
		}

		/// \brief Replace the extension (e.g., ".txt") in this string with
		///        another.
		inline bool tryReplaceExtension( Str newExt )
		{
			const DiffType extPos = findExtension();
			if( extPos == -1 ) {
				return tryAppend( newExt );
			}

			const SizeType extOff = SizeType( extPos );
			if( extOff + newExt.len() < extOff || !reserve( extOff + newExt.len() ) ) {
				return false;
			}

			m_data[ extOff ] = '\0';

			axstr__memcpy( m_data + extOff, reinterpret_cast< const void * >( newExt.get() ), newExt.len() );
			m_cLen = extOff + newExt.len();

			return true;
		}
		inline TMutStr &replaceExtension( Str newExt )
		{
			( void )tryReplaceExtension( newExt );
			return *this;
		}

		static inline Self fromWStr( const wchar_t *pwsSrc, SizeType cSrcChars )
		{
			Self r;

			SizeType uIndex = 0;
			while( uIndex < cSrcChars ) {
				axstr_utf8_t szBuf[ 256 ] = { 0 };

				if( sizeof( wchar_t ) == 4 ) {
					if( !axstr_utf32_to_utf8_n( szBuf, sizeof( szBuf ), ( const axstr_utf32_t * )&pwsSrc[ uIndex ], ( const axstr_utf32_t * )&pwsSrc[ cSrcChars ] ) ) {
						break;
					}
				} else {
					if( !axstr_utf16_to_utf8_n( szBuf, sizeof( szBuf ), ( const axstr_utf16_t * )&pwsSrc[ uIndex ], ( const axstr_utf16_t * )&pwsSrc[ cSrcChars ] ) ) {
						break;
					}
				}
				uIndex += sizeof( szBuf ) - 1;

				if( !r.tryAppend( ( const char * )szBuf ) ) {
					return Self();
				}
			}

			return r;
		}
		static inline Self fromWStr( const wchar_t *pwszSrc )
		{
			SizeType n = 0;
#if AXSTR_STDSTR_ENABLED
			while( pwszSrc[ n ] != L'\0' ) {
				++n;
			}
#else
			n = SizeType( wcslen( pwszSrc ) );
#endif

			return fromWStr( pwszSrc, n );
		}

		static inline Self fromEncoding( void *pSrc, axstr_size_t cSrcLen, axstr_encoding_t enc = axstr_enc_unknown )
		{
			Self r;

			// check for overflow
			if( cSrcLen*4 + 1 < cSrcLen ) {
				return Self();
			}

			if( !r.reserve( cSrcLen*4 ) ) {
				return Self();
			}

			const axstr_size_t cDstLen =
				axstr_from_encoding(
					(void*)r.m_data, r.m_cAllocedBytes,
					pSrc, cSrcLen,
					enc
				);

			r.m_cLen = cDstLen;
			r.m_data[ r.m_cLen ] = '\0';

			return r;
		}
		template< typename TArrayContainer >
		static inline Self fromEncoding( const TArrayContainer &src, axstr_encoding_t enc = axstr_enc_unknown )
		{
			return fromEncoding( (void*)src.pointer(), (axstr_size_t)src.len(), enc );
		}

		template< typename OtherAllocator >
		inline bool quoteTo( TMutStr< OtherAllocator > &dst ) const
		{
			return view().quoteTo( dst );
		}
		template< typename OtherAllocator >
		inline bool unquoteTo( TMutStr< OtherAllocator > &dst ) const
		{
			return view().unquoteTo( dst );
		}
		template< typename OtherAllocator >
		inline bool tabTo( TMutStr< OtherAllocator > &dst, SizeType levels = 1 ) const
		{
			return view().tabTo( dst, levels );
		}
		template< typename OtherAllocator >
		inline bool untabTo( TMutStr< OtherAllocator > &dst, SizeType levels = 1 ) const
		{
			return view().untabTo( dst, levels );
		}

		template< typename OtherAllocator >
		bool escapeTo( TMutStr< OtherAllocator > &dst ) const
		{
			return view().escapeTo( dst );
		}
		template< typename OtherAllocator >
		bool unescapeTo( TMutStr< OtherAllocator > &dst ) const
		{
			return view().unescapeTo( dst );
		}

		template< typename OtherAllocator >
		bool replaceTo( TMutStr< OtherAllocator > &dst, Str searchFor, Str replaceWith ) const
		{
			return view().replaceTo( dst, searchFor, replaceWith );
		}

		inline Self quoted() const
		{
			Self dst;
			if( !quoteTo( dst ) ) {
				// FIXME: Signal error
				return Self();
			}
			return dst;
		}
		inline Str unquoted() const
		{
			if( startsWith( '\"' ) && endsWith( '\"' ) ) {
				return view().skip().drop();
			}

			return view();
		}
		inline Self tabbed( SizeType levels = 1 ) const
		{
			Self dst;
			if( !tabTo( dst, levels ) ) {
				// FIXME: Signal error
				return Self();
			}
			return dst;
		}
		inline Self untabbed( SizeType levels = 1 ) const
		{
			Self dst;
			if( !untabTo( dst, levels ) ) {
				// FIXME: Signal error
				return Self();
			}
			return dst;
		}
		inline Self escaped() const
		{
			Self dst;
			if( !escapeTo( dst ) ) {
				return Self();
			}
			return dst;
		}
		inline Self unescaped() const
		{
			Self dst;
			if( !unescapeTo( dst ) ) {
				return Self();
			}
			return dst;
		}
		inline Self replaced( Str searchFor, Str replaceWith ) const
		{
			Self dst;
			if( !replaceTo( dst, searchFor, replaceWith ) ) {
				return Self();
			}
			return dst;
		}

		inline TMutStr &quoteMe()
		{
			if( !tryAppend( '\"' ) ) {
				return *this;
			}
			if( !tryPrepend( '\"' ) ) {
				m_data[ --m_cLen ] = '\0';
				return *this;
			}

			return *this;
		}
		inline TMutStr &unquoteMe()
		{
			if( view().endsWith( '\"' ) ) {
				m_data[ --m_cLen ] = '\0';
			}
			if( view().startsWith( '\"' ) ) {
				removeByOffsetAndSize( 0, 1 );
			}

			return *this;
		}
		inline TMutStr &tabMe( SizeType levels = 1 )
		{
			Self tmp;

			if( !tabTo( tmp, levels ) ) {
				return *this;
			}

			return swap( tmp );
		}
		inline TMutStr &untabMe( SizeType levels = 1 )
		{
			Self tmp;

			if( !untabTo( tmp, levels ) ) {
				return *this;
			}

			return swap( tmp );
		}

		inline Self &escapeMe()
		{
			Self tmp;

			if( !escapeTo( tmp ) ) {
				return *this;
			}

			return swap( tmp );
		}
		inline Self &unescapeMe()
		{
			Self tmp;

			if( !unescapeTo( tmp ) ) {
				return *this;
			}

			return swap( tmp );
		}

		inline Self &replaceMe( Str searchFor, Str replaceWith )
		{
			Self tmp;

			if( !replaceTo( tmp, searchFor, replaceWith ) ) {
				return *this;
			}

			return swap( tmp );
		}

		static inline Self fromInteger( axstr_sint_t value, unsigned int radix = 10, unsigned int numDigitsForSep = AXSTR_DIGITSEP_SPACING, char chDigitSep = AXSTR_DIGITSEP_CH ) {
			char buf[ 128 ];
			if( !axstr_from_int_x( buf, sizeof( buf ), value, radix, numDigitsForSep, chDigitSep ) ) {
				return Self();
			}

			Self r;
			r.assign( buf );

			return r;
		}
		static inline Self fromUnsignedInteger( axstr_uint_t value, unsigned int radix = 10, unsigned int numDigitsForSep = AXSTR_DIGITSEP_SPACING, char chDigitSep = AXSTR_DIGITSEP_CH ) {
			char buf[ 128 ];
			if( !axstr_from_uint_x( buf, sizeof( buf ), value, radix, numDigitsForSep, chDigitSep ) ) {
				return Self();
			}

			Self r;
			r.assign( buf );

			return r;
		}
		static inline Self fromFloat( axstr_real_t value, unsigned int radix = 10, unsigned int maxTrailingDigits = AXSTR_FLOAT_TRAIL_DIGITS, unsigned int numDigitsForSep = AXSTR_DIGITSEP_SPACING, char chDigitSep = AXSTR_DIGITSEP_CH ) {
			char buf[ 256 ];
			if( !axstr_from_float_x( buf, sizeof( buf ), value, radix, maxTrailingDigits, numDigitsForSep, chDigitSep ) ) {
				return Self();
			}

			Self r;
			r.assign( buf );

			return r;
		}

		inline TMutStr &operator=( Str s )
		{
			return assign( s );
		}
		inline TMutStr &operator=( const Self &s )
		{
			return assign( s );
		}
		template< typename OtherAllocator >
		inline TMutStr &operator=( const TMutStr< OtherAllocator > &s )
		{
			return assign( s );
		}
#if AXSTR_CXX11_MOVE_ENABLED
		inline TMutStr &operator=( TMutStr &&x )
		{
			return swap( x );
		}
#endif
		inline TMutStr &operator+=( Str s )
		{
			return append( s );
		}
		inline TMutStr &operator<<( Str s )
		{
			return append( s );
		}

		inline operator Str() const
		{
			return Str( m_data, m_cLen );
		}

		inline char &at( SizeType i )
		{
#ifdef AX_ASSERT
			AX_ASSERT( i < len() );
#elif defined( AX_TRAP )
			if( i >= len() ) {
				AX_TRAP();
			}
#endif

			return m_data[ i ];
		}
		inline char at( SizeType i ) const
		{
			if( i >= len() ) {
				return '\0';
			}

			return m_data[ i ];
		}

		inline char &operator[]( SizeType i )
		{
			return at( i );
		}
		inline char operator[]( SizeType i ) const
		{
			return at( i );
		}

		inline TMutStr &swap( TMutStr &other )
		{
			Allocator::swap( other, m_data, other.m_data );

			const SizeType cLen = m_cLen;
			m_cLen = other.m_cLen;
			other.m_cLen = cLen;

			const SizeType cAllocedBytes = m_cAllocedBytes;
			m_cAllocedBytes = other.m_cAllocedBytes;
			other.m_cAllocedBytes = cAllocedBytes;

			return *this;
		}

	private:
		AllocSizeType m_cAllocedBytes;
	};

	typedef TMutStr< policy::DefaultStringAllocator > MutStr;

	template< axstr_size_t tBufSize, typename OverflowAllocator = policy::DefaultStringAllocator >
	using TSmallStr = TMutStr< policy::SmallStringAllocator< tBufSize, OverflowAllocator > >;




	/*
	===========================================================================

		STRING

		Modifiable/dynamic string class.

	===========================================================================
	*/

#if AXSTR_OBJMUTSTR_ENABLED
	class ObjMutStr
	{
	public:
		typedef Str::SizeType SizeType;
		typedef Str::DiffType DiffType;
		typedef axstr_size_t  AllocSizeType;

		inline ObjMutStr()
		: m_pObj( ( char * )0 )
		{
		}
		explicit inline ObjMutStr( Str x )
		: m_pObj( ( char * )0 )
		{
			assign( x );
		}
		inline ObjMutStr( const ObjMutStr &x )
		: m_pObj( ( char * )0 )
		{
			assign( x );
		}
#if AXSTR_CXX11_MOVE_ENABLED
		inline ObjMutStr( ObjMutStr &&x )
		: m_pObj( x.m_pObj )
		{
			x.m_pObj = ( char * )0;
		}
#endif
		inline ~ObjMutStr()
		{
			purge();
		}

		inline const axstr__obj_t *getObjectPointer() const
		{
			return m_pObj != ( char * )0 ? ( ( const axstr__obj_t * )m_pObj ) - 1 : 0;
		}
		inline axstr__obj_t *getObjectPointer()
		{
			return m_pObj != ( char * )0 ? ( ( axstr__obj_t * )m_pObj ) - 1 : 0;
		}
		inline const char *getStringPointer() const
		{
			return m_pObj;
		}
		inline char *getStringPointer()
		{
			return m_pObj;
		}

		inline const char *cstring() const
		{
			return m_pObj;
		}
		inline const char *c_str() const
		{
			return m_pObj;
		}

		inline char *get()
		{
			return m_pObj;
		}
		inline const char *get() const
		{
			return m_pObj;
		}

		inline char *getEnd()
		{
			return m_pObj + len();
		}
		inline const char *getEnd() const
		{
			return m_pObj + len();
		}

		inline const char *pointer( SizeType index = 0 ) const
		{
			return index < len() ? &m_pObj[ index ] : 0;
		}
		inline char *pointer( SizeType index = 0 )
		{
			return index < len() ? &m_pObj[ index ] : 0;
		}

		inline Str ref() const
		{
			return Str( *this );
		}
		inline Str view() const
		{
			return Str( *this );
		}

		// Total amount of memory this object is using
		inline AllocSizeType memSize() const
		{
			return sizeof( *this ) + ( m_pObj != ( char * )0 ? getObjectPointer()->cMax + sizeof( sizeof( axstr__obj_t ) ) : 0 );
		}
		// Maximum capacity of the current object (before reallocation)
		inline SizeType max() const
		{
			return m_pObj != ( char * )0 ? getObjectPointer()->cMax : 0;
		}
		// Current length of the string in bytes (not including the null terminator)
		inline SizeType len() const
		{
			return m_pObj != ( char * )0 ? getObjectPointer()->cLen : 0;
		}
		// Current length of the string in bytes, including the null terminator
		inline SizeType num() const
		{
			return m_pObj != ( char * )0 ? getObjectPointer()->cLen + 1 : 0;
		}

		inline bool isEmpty() const
		{
			return !m_pObj || getObjectPointer()->cLen == 0;
		}
		inline bool isUsed() const
		{
			return m_pObj != ( char * )0 && getObjectPointer()->cLen > 0;
		}

		// get a byte index clamped into bounds
		inline SizeType clampIndex( SizeType uOffset ) const
		{
			const SizeType n = len();
			return uOffset < n ? uOffset : n;
		}
		// Retrieve a pointer based on an index, clamped if necessary
		inline const char *clampIndexToPointer( SizeType uOffset ) const
		{
			return cstring() + clampIndex( uOffset );
		}
		// get a byte length clamped into bounds
		inline SizeType clampLength( SizeType uOffset, SizeType cLen ) const
		{
			const SizeType s = clampIndex( uOffset );
			const SizeType e = clampIndex( uOffset + cLen );
			return e - s;
		}

		// Prepare memory
		inline bool reserve( SizeType cLen )
		{
			axstr__obj_t *const pDst = axstr__obj_reserve( getObjectPointer(), cLen );
			if( !pDst ) {
				return false;
			}

			m_pObj = ( char * )( pDst + 1 );
			return true;
		}
		inline bool reserveAndSetLen( SizeType cLen )
		{
			if( !reserve( cLen ) ) {
				return false;
			}

			if( !axstr__obj_setlen( getObjectPointer(), cLen ) ) {
				return false;
			}

			m_pObj[ cLen ] = '\0';
			return true;
		}

		inline DiffType find( char ch, DiffType iAfter = -1 ) const
		{
			return Str( *this ).find( ch, iAfter );
		}
		inline DiffType findAny( const char *pszChars, DiffType iAfter = -1 ) const
		{
			return Str( *this ).findAny( pszChars, iAfter );
		}
		inline DiffType findLast( char ch ) const
		{
			return Str( *this ).findLast( ch );
		}
		inline DiffType findLast( Str s ) const
		{
			return Str( *this ).findLast( s );
		}

		// Clear the string
		inline ObjMutStr &clear()
		{
			axstr__obj_clear( axstr__strobj( m_pObj ) );
			return *this;
		}
		// Remove all dynamic memory associated with this string
		inline ObjMutStr &purge()
		{
			axstr__obj_free( axstr__strobj( m_pObj ) );
			m_pObj = ( char * )0;
			return *this;
		}

		// Remove a set of bytes from the string
		inline ObjMutStr &remove( SizeType uOffset, SizeType cLen )
		{
			axstr__obj_remove( axstr__strobj( m_pObj ), uOffset, cLen );
			return *this;
		}
		inline ObjMutStr removed( SizeType uOffset, SizeType cLen ) const
		{
			return ObjMutStr( *this ).remove( uOffset, cLen );
		}

		// Extract a substring from this, removing it from this string
		inline ObjMutStr extract( DiffType startPos, DiffType endPos )
		{
			if( !m_pObj ) { return ObjMutStr(); }

			const SizeType cOffBytesLT = Str( *this ).calcBytes_( startPos );
			const SizeType cOffBytesRT = Str( *this ).calcBytes_( endPos );

			const SizeType cOffBytesL = cOffBytesLT < cOffBytesRT ? cOffBytesLT : cOffBytesRT;
			const SizeType cOffBytesR = cOffBytesLT < cOffBytesRT ? cOffBytesRT : cOffBytesLT;

			ObjMutStr r( Str( m_pObj + cOffBytesL, m_pObj + cOffBytesR ) );
			remove( cOffBytesL, cOffBytesR - cOffBytesL );

			return r;
		}

		// Remove whitespace from the left
		inline ObjMutStr &trimLeft()
		{
			SizeType i = 0;

			const SizeType n = len();
			while( i < n ) {
				if( *( ( const unsigned char * )m_pObj + i ) > ' ' ) {
					break;
				}

				++i;
			}

			return remove( 0, i );
		}
		// drop whitespace from the right
		inline ObjMutStr &trimRight()
		{
			const SizeType n = len();
			SizeType i = n;

			while( i > 0 ) {
				if( *( ( const unsigned char * )m_pObj + ( i - 1 ) ) > ' ' ) {
					break;
				}

				--i;
			}

			return remove( i, n - i );
		}
		// Remove whitespace from both sides
		inline ObjMutStr &trim()
		{
			return trimLeft().trimRight();
		}

		inline Str trimmedLeft() const
		{
			return Str( *this ).trimLeft();
		}
		inline Str trimmedRight() const
		{
			return Str( *this ).trimRight();
		}
		inline Str trimmed() const
		{
			return Str( *this ).trimLeft().trimRight();
		}

		// assign text to this string
		inline bool tryAssign( Str s )
		{
			axstr__obj_t *const p = axstr__obj_assign( axstr__strobj( m_pObj ), s.get(), s.len() );
			if( !p ) {
				return false;
			}

			setObject_( p );
			return true;
		}
		inline ObjMutStr &assign( Str s )
		{
			return setObject_( axstr__obj_assign( axstr__strobj( m_pObj ), s.get(), s.len() ) );
		}

		// Insert text at the beginning of the string
		inline bool tryPrepend( Str s )
		{
			axstr__obj_t *const p = axstr__obj_prepend( axstr__strobj( m_pObj ), s.get(), s.len() );
			if( !p ) {
				return false;
			}

			setObject_( p );
			return true;
		}
		inline ObjMutStr &prepend( Str s )
		{
			return setObject_( axstr__obj_prepend( axstr__strobj( m_pObj ), s.get(), s.len() ) );
		}
		inline ObjMutStr prepended( Str s ) const
		{
			return ObjMutStr( *this ).prepend( s );
		}

		// Insert text at the end of the string
		inline bool tryAppend( Str s )
		{
			axstr__obj_t *const p = axstr__obj_append( axstr__strobj( m_pObj ), s.get(), s.len() );
			if( !p ) {
				return false;
			}

			setObject_( p );
			return true;
		}
		inline bool tryAppend( const char *s, const char *e )
		{
			return tryAppend( Str( s, e ) );
		}
		inline ObjMutStr &append( Str s )
		{
			return setObject_( axstr__obj_append( axstr__strobj( m_pObj ), s.get(), s.len() ) );
		}
		inline ObjMutStr &append( const char *s, const char *e )
		{
			return append( Str( s, e ) );
		}
		inline ObjMutStr appended( Str s ) const
		{
			return ObjMutStr( *this ).append( s );
		}
		inline ObjMutStr appended( const char *s, const char *e ) const
		{
			return ObjMutStr( *this ).append( Str( s, e ) );
		}

#ifdef INCGUARD_AX_PRINTF_H_
		inline bool appendfv( Str formatStr, va_list args )
		{
			char *const p = axdupfev( formatStr.get(), formatStr.getEnd(), args );
			if( !p ) {
				return false;
			}

			axstr__obj_t *const pObj = axstr__obj_append( axstr__strobj( m_pObj ), p, ~( axstr_size_t )0 );
			axpf_free( ( void * )p );

			if( !pObj ) {
				return false;
			}

			setObject_( pObj );
			return true;
		}

		inline bool appendf( Str formatStr, ... )
		{
			va_list args;

			va_start( args, formatStr );
			const bool r = appendfv( formatStr, args );
			va_end( args );

			return r;
		}

		inline bool formatv( Str formatStr, va_list args )
		{
			char *const p = axdupfev( formatStr.get(), formatStr.getEnd(), args );
			if( !p ) {
				return false;
			}

			axstr__obj_t *const pObj = axstr__obj_assign( axstr__strobj( m_pObj ), p, ~SizeType( 0 ) );
			if( !pObj ) {
				return false;
			}

			setObject_( pObj );
			return true;
		}
		inline bool format( Str formatStr, ... )
		{
			va_list args;

			va_start( args, formatStr );
			const bool r = formatv( formatStr, args );
			va_end( args );

			return r;
		}

		static inline ObjMutStr formattedv( Str formatStr, va_list args )
		{
			ObjMutStr Temp;
			Temp.formatv( formatStr, args );
			return Temp;
		}
		static inline ObjMutStr formatted( Str formatStr, ... )
		{
			va_list args;
			ObjMutStr Temp;

			va_start( args, formatStr );
			Temp.formatv( formatStr, args );
			va_end( args );

			return Temp;
		}
#endif

		// Concatenate a path to the string
		inline bool tryAppendPath( Str s )
		{
			return Str( *this ).tryAppendPathTo( *this, s );
		}
		inline ObjMutStr &appendPath( Str s )
		{
			( void )Str( *this ).tryAppendPathTo( *this, s );
			return *this;
		}

		template< typename TArrayContainer >
		inline bool splitTo( TArrayContainer &outArray, Str inSeparator, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const;
		template< typename TArrayContainer >
		inline TArrayContainer split( Str inSeparator, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
			TArrayContainer container;
			splitTo< TArrayContainer >( container, inSeparator, keepEmpty );
			return container;
		}

		template< typename TArrayContainer >
		inline bool splitUnquotedTo( TArrayContainer &outArray, Str inSeparator, Str inEscape = '\\', EKeepQuotes::Type keepQuotes = EKeepQuotes::No, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const;
		template< typename TArrayContainer >
		inline TArrayContainer splitUnquoted( Str inSeparator, Str inEscape = '\\', EKeepQuotes::Type keepQuotes = EKeepQuotes::No, EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
			TArrayContainer container;
			splitUnquotedTo< TArrayContainer >( container, inSeparator, inEscape, keepQuotes, keepEmpty );
			return container;
		}


		/*! \brief Split this string by lines. */
		template< typename TArrayContainer >
		inline bool splitLinesTo( TArrayContainer &outArr ) const
		{
			return Str( *this ).splitLinesTo< TArrayContainer >( outArr );
		}
		template< typename TArrayContainer >
		inline TArrayContainer splitLines( EKeepEmpty::Type keepEmpty = EKeepEmpty::No ) const {
			TArrayContainer container;
			splitLinesTo< TArrayContainer >( container, keepEmpty );
			return container;
		}

		template< typename TString >
		static inline bool mergeTo( ObjMutStr &outStr, const TArr< TString > &inArr, Str inGlue = "" );
		template< typename TString >
		static inline ObjMutStr merge( const TArr< TString > &inArr, Str inGlue = "" );

		inline DiffType findExtension() const
		{
			return Str( *this ).findExtension();
		}

		inline Str getExtension() const
		{
			return Str( *this ).getExtension();
		}
		inline Str getFilename() const
		{
			return Str( *this ).getFilename();
		}
		inline Str getDirectory() const
		{
			return Str( *this ).getDirectory();
		}
		inline Str getBasename() const
		{
			return Str( *this ).getBasename();
		}
		inline Str getRoot() const
		{
			return Str( *this ).getRoot();
		}

		inline bool tryReplaceExtension( Str NewExt )
		{
			const DiffType i = findExtension();
			if( i == -1 ) {
				return tryAppend( NewExt );
			}

			const char ch = m_pObj[ i ];
			SizeType cLen = axstr__strobj( m_pObj )->cLen;

			m_pObj[ i ] = '\0';
			axstr__strobj( m_pObj )->cLen = i;

			if( !tryAppend( NewExt ) ) {
				m_pObj[ i ] = ch;
				axstr__strobj( m_pObj )->cLen = cLen;

				return false;
			}

			return true;
		}
		inline ObjMutStr &replaceExtension( Str NewExt )
		{
			( void )tryReplaceExtension( NewExt );
			return *this;
		}

		inline bool caseCmp( Str x ) const
		{
			return Str( *this ).caseCmp( x );
		}
		inline bool cmp( Str x ) const
		{
			return Str( *this ).cmp( x );
		}
		inline int sortCaseCmp( Str x ) const
		{
			return Str( *this ).sortCaseCmp( x );
		}
		inline int sortCmp( Str x ) const
		{
			return Str( *this ).sortCmp( x );
		}

		inline bool startsWith( Str x ) const
		{
			return Str( *this ).startsWith( x );
		}
		inline bool endsWith( Str x ) const
		{
			return Str( *this ).endsWith( x );
		}
		inline bool caseStartsWith( Str x ) const
		{
			return Str( *this ).caseStartsWith( x );
		}
		inline bool caseEndsWith( Str x ) const
		{
			return Str( *this ).caseEndsWith( x );
		}

		inline bool startsWithDirSep() const
		{
			return Str( *this ).startsWithDirSep();
		}
		inline bool endsWithDirSep() const
		{
			return Str( *this ).endsWithDirSep();
		}

		inline Str left( DiffType n ) const
		{
			return Str( *this ).left( n );
		}
		inline Str right( DiffType n ) const
		{
			return Str( *this ).right( n );
		}
		inline Str mid( DiffType pos, SizeType num ) const
		{
			return Str( *this ).mid( pos, num );
		}
		inline Str substr( DiffType startPos, DiffType endPos ) const
		{
			return Str( *this ).substr( startPos, endPos );
		}

		// Remove the first 'n'-bytes in the string.
		inline Str skip( DiffType pos = 1 ) const
		{
			return Str( *this ).skip( pos );
		}
		inline ObjMutStr &skipMe( SizeType num = 1 )
		{
			return remove( 0, num );
		}
		// Remove the last 'n'-bytes in the string.
		inline Str drop( DiffType num = 1 ) const
		{
			return Str( *this ).drop( num );
		}
		inline ObjMutStr &dropMe( SizeType num = 1 )
		{
			const axstr_size_t cLen = len();
			if( num > cLen ) {
				num = cLen;
			}

			return remove( cLen - num, num );
		}

		inline char firstByte() const
		{
			return m_pObj != ( char * )0 ? *m_pObj : '\0';
		}
		inline char lastByte() const
		{
			return Str( *this ).lastByte();
		}

		inline axstr_sint_t toInteger( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
		{
			return Str( *this ).toInteger( defaultRadix, chDigitSep );
		}
		inline axstr_uint_t toUnsignedInteger( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
		{
			return Str( *this ).toUnsignedInteger( defaultRadix, chDigitSep );
		}
		inline axstr_real_t toFloat( int defaultRadix = AXSTR_DEFPARSE_RADIX, char chDigitSep = AXSTR_DIGITSEP_CH ) const
		{
			return Str( *this ).toFloat( defaultRadix, chDigitSep );
		}

		inline SizeType toEncoding( void *pDstBuf, SizeType cDstBytes, axstr_encoding_t enc, axstr_byteordermark_t bom = axstr_bom_disabled ) const
		{
			return Str( *this ).toEncoding( pDstBuf, cDstBytes, enc, bom );
		}
		inline wchar_t *toWStr( AXSTR_OUT_Z(cDstChars) wchar_t *pDstBuf, SizeType cDstChars ) const
		{
			return Str( *this ).toWStr( pDstBuf, cDstChars );
		}
		template< SizeType tNumWChars >
		inline wchar_t *toWStr( AXSTR_OUT_Z(tNumWChars) wchar_t( &dstBuf )[ tNumWChars ] ) const
		{
			return Str( *this ).toWStr( &dstBuf[0], tNumWChars );
		}

		static inline ObjMutStr fromWStr( const wchar_t *pwsSrc, SizeType cSrcChars )
		{
			ObjMutStr r;

			SizeType uIndex = 0;
			while( uIndex < cSrcChars ) {
				axstr_utf8_t szBuf[ 256 ] = { 0 };

				if( sizeof( wchar_t ) == 4 ) {
					if( !axstr_utf32_to_utf8_n( szBuf, sizeof( szBuf ), ( const axstr_utf32_t * )&pwsSrc[ uIndex ], ( const axstr_utf32_t * )&pwsSrc[ cSrcChars ] ) ) {
						break;
					}
				} else {
					if( !axstr_utf16_to_utf8_n( szBuf, sizeof( szBuf ), ( const axstr_utf16_t * )&pwsSrc[ uIndex ], ( const axstr_utf16_t * )&pwsSrc[ cSrcChars ] ) ) {
						break;
					}
				}
				uIndex += sizeof( szBuf ) - 1;

				if( !r.tryAppend( ( const char * )szBuf ) ) {
					return ObjMutStr();
				}
			}

			return r;
		}
		static inline ObjMutStr fromWStr( const wchar_t *pwszSrc )
		{
			SizeType n = 0;
			while( pwszSrc[ n ] != L'\0' ) {
				++n;
			}

			return fromWStr( pwszSrc, n );
		}

		inline bool isQuoted() const
		{
			return Str( *this ).isQuoted();
		}

		inline bool quoteTo( ObjMutStr &dst ) const
		{
			return Str( *this ).quoteTo( dst );
		}
		inline bool unquoteTo( ObjMutStr &dst ) const
		{
			return Str( *this ).unquoteTo( dst );
		}
		inline bool tabTo( ObjMutStr &dst, unsigned levels = 1 ) const
		{
			return Str( *this ).tabTo( dst, levels );
		}
		inline bool untabTo( ObjMutStr &dst, unsigned levels = 1 ) const
		{
			return Str( *this ).untabTo( dst, levels );
		}

		inline ObjMutStr quoted() const
		{
			return Str( *this ).quoted();
		}
		inline Str unquoted() const
		{
			return Str( *this ).unquoted();
		}
		inline ObjMutStr tabbed( SizeType levels = 1 ) const
		{
			return Str( *this ).tabbed( levels );
		}
		inline ObjMutStr untabbed( SizeType levels = 1 ) const
		{
			return Str( *this ).untabbed( levels );
		}

		inline ObjMutStr &quoteMe()
		{
			return assign( Str( *this ).quoted() );
		}
		inline ObjMutStr &unquoteMe()
		{
			return assign( Str( *this ).unquoted() );
		}
		inline ObjMutStr &tabMe( SizeType levels = 1 )
		{
			return assign( Str( *this ).tabbed( levels ) );
		}
		inline ObjMutStr &untabMe( SizeType levels = 1 )
		{
			return assign( Str( *this ).untabbed( levels ) );
		}

		// Replace all backslashes with forward slashes
		inline ObjMutStr &backToForwardSlashes()
		{
			Str x( *this );
			DiffType iLast = -1;
			for(;;) {
				iLast = x.find( '\\', iLast );
				if( iLast < 0 ) {
					break;
				}

				m_pObj[ iLast ] = '/';
			}

			return *this;
		}
		// Replace all forward slashes with backslashes
		inline ObjMutStr &forwardToBackSlashes()
		{
			Str x( *this );
			DiffType iLast = -1;
			for(;;) {
				iLast = x.find( '/', iLast );
				if( iLast < 0 ) {
					break;
				}

				m_pObj[ iLast ] = '\\';
			}

			return *this;
		}

		// Sanitize for use as a file name
		inline ObjMutStr &sanitizeFilename( char chReplacement = '-' )
		{
			static const char *const pszSpecialChars = "\\/:*?\"<>|";
			static const char chSanitize = '-';

			char *const e = m_pObj + len();
			for( char *p = m_pObj; p < e; ++p ) {
#if AXSTR_STDSTR_ENABLED
				if( !strchr( pszSpecialChars, *p ) ) {
					continue;
				}
#else
				const char *q;
				for( q = pszSpecialChars; *q != '\0'; ++q ) {
					if( *p == *q ) {
						q = ( const char * )0;
						break;
					}
				}

				if( q != ( const char * )0 ) {
					continue;
				}
#endif

				*p = chReplacement;
			}

			return *this;
		}

		inline ObjMutStr &operator=( Str s )
		{
			return assign( s );
		}
		inline ObjMutStr &operator=( const ObjMutStr &s )
		{
			return assign( s );
		}
#if AXSTR_CXX11_MOVE_ENABLED
		inline ObjMutStr &operator=( ObjMutStr &&x )
		{
			char *const p = m_pObj;
			m_pObj = x.m_pObj;
			x.m_pObj = p;

			return *this;
		}
#endif
		inline ObjMutStr &operator+=( Str s )
		{
			return append( s );
		}
		inline ObjMutStr &operator<<( Str s )
		{
			return append( s );
		}

		inline operator const axstr__obj_t *() const
		{
			return getObjectPointer();
		}
		inline operator axstr__obj_t *()
		{
			return getObjectPointer();
		}

		inline operator Str() const
		{
			return Str( *this );
		}

		inline bool operator==( Str x ) const
		{
			return Str( *this ).cmp( x );
		}
		inline bool operator!=( Str x ) const
		{
			return !Str( *this ).cmp( x );
		}
		inline bool operator<( Str x ) const
		{
			return Str( *this ).sortCmp( x ) < 0;
		}
		inline bool operator>( Str x ) const
		{
			return Str( *this ).sortCmp( x ) > 0;
		}
		inline bool operator<=( Str x ) const
		{
			return Str( *this ).sortCmp( x ) <= 0;
		}
		inline bool operator>=( Str x ) const
		{
			return Str( *this ).sortCmp( x ) >= 0;
		}

		inline bool operator!() const
		{
			return isEmpty();
		}

		inline char at( axstr_size_t i ) const
		{
			return i < len() ? m_pObj[ i ] : '\0';
		}
		inline char &at( axstr_size_t i )
		{
#ifdef AX_ASSERT
			AX_ASSERT( i < len() );
#elif defined( AX_TRAP )
			if( i >= len() ) {
				AX_TRAP();
			}
#endif

			return m_pObj[ i ];
		}

		inline char operator[]( axstr_size_t i ) const
		{
			return at( i );
		}
		inline char operator[]( axstr_ptrdiff_t i ) const
		{
			return at( axstr_size_t( i ) );
		}

		inline char &operator[]( axstr_size_t i )
		{
			return at( i );
		}
		inline char &operator[]( axstr_ptrdiff_t i )
		{
			return at( axstr_size_t( i ) );
		}

		inline ObjMutStr &swap( ObjMutStr &other )
		{
			char *const pTmp = m_pObj;
			m_pObj = other.m_pObj;
			other.m_pObj = pTmp;
			return *this;
		}

	private:
		char *                      m_pObj;

		inline ObjMutStr &setObject_( axstr__obj_t *pNewObj )
		{
			if( pNewObj != ( axstr__obj_t * )0 ) {
				m_pObj = axstr__objstr( pNewObj );
			}

			return *this;
		}
	};
#endif

#if AXSTR_OBJMUTSTR_ENABLED
	inline Str::Str( const ObjMutStr &s )
	: m_pStr( s.cstring() )
	, m_cStr( s.len() )
	{
	}
#endif
	inline Str::Str( const detail::MutStrCore &s )
	: m_pStr( s.get() )
	, m_cStr( s.len() )
	{
	}
	template< typename Allocator >
	inline Str::Str( const TMutStr< Allocator > &s )
	: m_pStr( s.get() )
	, m_cStr( s.len() )
	{
	}
#if AXSTR_OBJMUTSTR_ENABLED
	inline Str::Str( const ObjMutStr &s, SizeType cBytes )
	: m_pStr( s.cstring() )
	, m_cStr( cBytes < s.len() ? cBytes : s.len() )
	{
	}
#endif
	inline Str::Str( const detail::MutStrCore &s, SizeType cBytes )
	: m_pStr( s.get() )
	, m_cStr( cBytes < s.len() ? cBytes : s.len() )
	{
	}
	template< typename Allocator >
	inline Str::Str( const TMutStr< Allocator > &s, SizeType cBytes )
	: m_pStr( s.get() )
	, m_cStr( cBytes < s.len() ? cBytes : s.len() )
	{
	}
#if AXSTR_OBJMUTSTR_ENABLED
	inline Str::Str( const ObjMutStr &s, SizeType uOffset, SizeType cBytes )
	: m_pStr( s.clampIndexToPointer( uOffset ) )
	, m_cStr( s.clampIndex( uOffset + cBytes ) ) // FIXME: This looks incorrect
	{
	}
#endif
	inline Str::Str( const detail::MutStrCore &s, SizeType uOffset, SizeType cBytes )
	: m_pStr( s.get() + ( uOffset < s.len() ? uOffset : s.len() ) )
	, m_cStr( uOffset + cBytes < s.len() ? cBytes : s.len() - uOffset )
	{
	}
	template< typename Allocator >
	inline Str::Str( const TMutStr< Allocator > &s, SizeType uOffset, SizeType cBytes )
	: m_pStr( s.get() + ( uOffset < s.len() ? uOffset : s.len() ) )
	, m_cStr( uOffset + cBytes < s.len() ? cBytes : s.len() - uOffset )
	{
	}

	inline TMutStr<> Str::operator+( Str x ) const
	{
		TMutStr<> dst;
		if( m_cStr + x.len() < m_cStr || !dst.reserve( m_cStr + x.len() ) ) {
			return TMutStr<>();
		}

		dst.assign( *this );
		dst.append( *this );

		return dst;
	}

#if AXSTR_OBJMUTSTR_ENABLED
	inline bool Str::quoteTo( ObjMutStr &dst ) const
	{
		if( isQuoted() ) {
			dst.clear();
			dst.assign( *this );
			return dst.len() == len();
		}

		dst.clear();
		dst.assign( "\"" );
		dst.append( *this );
		dst.append( "\"" );

		return dst.len() == len() + 2;
	}
	inline bool Str::unquoteTo( ObjMutStr &dst ) const
	{
		dst.clear();
		const Str tmp = unquoted();
		dst.assign( tmp );
		return dst.len() == tmp.len();
	}

#if 0
	inline bool Str::tryAppendPathTo( ObjMutStr &outStr, Str path ) const
	{
#ifdef _WIN32
		const char chPathSep = '\\';
#else
		const char chPathSep = '/';
#endif
		const bool bEndsWithPathSep = endsWithDirSep();
		const bool bStartsWithPathSep = startsWithDirSep();

		if( !outStr.tryAssign( *this ) ) {
			return false;
		}

		if( !bEndsWithPathSep && !outStr.tryAppend( chPathSep ) ) {
			return false;
		}

		// Ignore a leading path separator for path if this string is not empty
		if( bStartsWithPathSep && isUsed() ) {
			path.skip();
		}

		if( !outStr.tryAppend( path ) ) {
			return false;
		}

		return true;
	}
#endif // if 0
#endif

	template< typename Allocator >
	inline bool Str::quoteTo( TMutStr<Allocator> &dst ) const
	{
		if( isQuoted() ) {
			dst.clear();
			dst.assign( *this );
			return dst.len() == len();
		}

		dst.clear();
		dst.assign( "\"" );
		dst.append( *this );
		dst.append( "\"" );

		return dst.len() == len() + 2;
	}
	template< typename Allocator >
	inline bool Str::unquoteTo( TMutStr<Allocator> &dst ) const
	{
		dst.clear();
		const Str tmp = unquoted();
		dst.assign( tmp );
		return dst.len() == tmp.len();
	}

#if 0
	template< typename Allocator >
	inline bool Str::tryAppendPathTo( TMutStr<Allocator> &outStr, Str path ) const
	{
#ifdef _WIN32
		const char chPathSep = '\\';
#else
		const char chPathSep = '/';
#endif
		const bool bEndsWithPathSep = endsWithDirSep();
		const bool bStartsWithPathSep = startsWithDirSep();

		if( m_cStr + path.len() + 1 <= m_cStr || !outStr.reserve( m_cStr + path.len() + 1 ) ) {
			return false;
		}

		if( !outStr.tryAssign( *this ) ) {
			return false;
		}

		if( !bEndsWithPathSep && !outStr.tryAppend( chPathSep ) ) {
			return false;
		}

		// Ignore a leading path separator for path if this string is not empty
		if( bStartsWithPathSep && isUsed() ) {
			path.skip();
		}

		if( !outStr.tryAppend( path ) ) {
			return false;
		}

		return true;
	}
#endif // if 0

	inline MutStr operator+( Str a, Str b )
	{
		MutStr dst;

		if( a.len() + b.len() < a.len() || !dst.reserve( a.len() + b.len() ) ) {
			return MutStr();
		}

		if( !dst.tryAssign( a ) || !dst.tryAppend( b ) ) {
			return MutStr();
		}

		return dst;
	}
	inline MutStr operator/( Str a, Str b )
	{
		MutStr dst;

		if( !dst.reserve( a.len() + b.len() ) ) {
			return MutStr();
		}

		dst.assign( a );
		dst.appendPath( b );

		return dst;
	}

	namespace detail
	{

#if AXSTR_OBJMUTSTR_ENABLED
		inline bool addTabs( ObjMutStr &dst, axstr_size_t levels )
		{
			const char tabs[] = "\t\t\t\t\t\t\t\t\t\t";

			const axstr_size_t cOldLen = dst.len();
			axstr_size_t cRemaining = levels;
			while( cRemaining > 0 ) {
				const axstr_size_t cTabs = cRemaining < sizeof( tabs ) ? cRemaining : sizeof( tabs );
				dst.append( Str( tabs, cRemaining ) );
				cRemaining -= cTabs;
			}

			return dst.len() == cOldLen + levels;
		}
#endif
		template< typename Allocator >
		inline bool addTabs( TMutStr<Allocator> &dst, axstr_size_t levels )
		{
			const char tabs[] = "\t\t\t\t\t\t\t\t\t\t";

			const axstr_size_t cOldLen = dst.len();
			axstr_size_t cRemaining = levels;
			while( cRemaining > 0 ) {
				const axstr_size_t cTabs = cRemaining < sizeof( tabs ) ? cRemaining : sizeof( tabs );
				dst.append( Str( tabs, cRemaining ) );
				cRemaining -= cTabs;
			}

			return dst.len() == cOldLen + levels;
		}

	}

#if AXSTR_OBJMUTSTR_ENABLED
	inline bool Str::tabTo( ObjMutStr &dst, SizeType levels ) const
	{
		dst.clear();

		if( !levels ) {
			dst.assign( *this );
			return dst.len() == len();
		}

		axstr_size_t cLines = 0;
		DiffType LastIndex = -1;
		for(;;) {
			++cLines;

			if( !detail::addTabs( dst, levels ) ) {
				return false;
			}

			const DiffType CheckIndex = find( '\n', LastIndex + 1 );
			if( CheckIndex < 0 ) {
				dst.append( skip( LastIndex + 1 ) );
				break;
			}

			dst.append( skip( LastIndex + 1 ).left( CheckIndex - LastIndex ) );

			LastIndex = CheckIndex + 1;
		}

		return dst.len() == len() + cLines*levels;
	}
	inline bool Str::untabTo( ObjMutStr &dst, SizeType levels ) const
	{
		dst.clear();

		if( !levels ) {
			dst.assign( *this );
			return dst.len() == len();
		}

		Str Cur = *this;
		axstr_size_t cExpected = 0;

		for(;;) {
			for( unsigned n = 0; n < levels && Cur.startsWith( '\t' ); ++n ) {
				Cur = Cur.skip();
			}

			const DiffType CheckIndex = Cur.find( '\n' );
			if( CheckIndex < 0 ) {
				cExpected += Cur.len();
				dst.append( Cur );
				break;
			}

			const axstr_size_t cLen = CheckIndex + 1;

			dst.append( Cur.left( cLen ) );
			Cur = Cur.skip( cLen );

			cExpected += cLen;
		}

		return dst.len() == cExpected;
	}
#endif

	template< typename Allocator >
	inline bool Str::tabTo( TMutStr<Allocator> &dst, SizeType levels ) const
	{
		dst.clear();

		if( !levels ) {
			dst.assign( *this );
			return dst.len() == len();
		}

		axstr_size_t cLines = 0;
		DiffType LastIndex = -1;
		for(;;) {
			++cLines;

			if( !detail::addTabs( dst, levels ) ) {
				return false;
			}

			const DiffType CheckIndex = find( '\n', LastIndex + 1 );
			if( CheckIndex < 0 ) {
				dst.append( skip( LastIndex + 1 ) );
				break;
			}

			dst.append( skip( LastIndex + 1 ).left( CheckIndex - LastIndex ) );

			LastIndex = CheckIndex + 1;
		}

		return dst.len() == len() + cLines*levels;
	}
	template< typename Allocator >
	inline bool Str::untabTo( TMutStr<Allocator> &dst, SizeType levels ) const
	{
		dst.clear();

		if( !levels ) {
			dst.assign( *this );
			return dst.len() == len();
		}

		Str Cur = *this;
		axstr_size_t cExpected = 0;

		for(;;) {
			for( unsigned n = 0; n < levels && Cur.startsWith( '\t' ); ++n ) {
				Cur = Cur.skip();
			}

			const DiffType CheckIndex = Cur.find( '\n' );
			if( CheckIndex < 0 ) {
				cExpected += Cur.len();
				dst.append( Cur );
				break;
			}

			const axstr_size_t cLen = CheckIndex + 1;

			dst.append( Cur.left( cLen ) );
			Cur = Cur.skip( cLen );

			cExpected += cLen;
		}

		return dst.len() == cExpected;
	}

#define AXSTR_ESCAPE_LIST_() \
	AXSTR_ESCAPE_( '\\', '\\' ) \
	AXSTR_ESCAPE_( '\'', '\'' ) \
	AXSTR_ESCAPE_( '\"', '\"' ) \
	AXSTR_ESCAPE_( '\?', '\?' ) \
	AXSTR_ESCAPE_( '\a', 'a' ) \
	AXSTR_ESCAPE_( '\b', 'b' ) \
	AXSTR_ESCAPE_( '\x1B', 'e' ) \
	AXSTR_ESCAPE_( '\f', 'f' ) \
	AXSTR_ESCAPE_( '\n', 'n' ) \
	AXSTR_ESCAPE_( '\r', 'r' ) \
	AXSTR_ESCAPE_( '\t', 't' ) \
	AXSTR_ESCAPE_( '\v', 'v' )

	template< typename Allocator >
	inline bool Str::escapeTo( TMutStr< Allocator > &dst ) const
	{
		dst.clear();

		const char *s = get();
		const char *const e = getEnd();
		const char *p = s;
		while( p != e ) {
			char chReplace = '\0';

			switch( *p ) {
#define AXSTR_ESCAPE_(SrcCh_,DstCh_) case SrcCh_: chReplace = DstCh_; break;
			AXSTR_ESCAPE_LIST_()
#undef AXSTR_ESCAPE_
			case '\0':
				chReplace = '0';
				break;
			default:
				break;
			}

			const Str plainRange( s, p );
			++p;

			if( !chReplace ) {
				continue;
			}

			s = p;

			if( plainRange.isUsed() && !dst.tryAppend( plainRange ) ) {
				goto fail;
			}

			if( !dst.tryAppend( '\\' ) ) {
				goto fail;
			}
			if( !dst.tryAppend( chReplace ) ) {
				goto fail;
			}
		}

		if( !dst.tryAppend( Str( s, p ) ) ) {
			goto fail;
		}

		return true;

	fail:
		dst.clear();
		return false;
	}
	template< typename Allocator >
	inline bool Str::unescapeTo( TMutStr< Allocator > &dst ) const
	{
		dst.clear();

		const char *s = get();
		const char *const e = getEnd();
		const char *p = s;
		while( p != e ) {
			const char *const q = p;
			if( *p++ != '\\' ) {
				continue;
			}

			// Break in the case we hit the end of the string on an incomplete
			// escape sequence.
			if( p == e ) {
				break;
			}

			switch( *p ) {
#define AXSTR_ESCAPE_(SrcCh_,DstCh_) \
	case DstCh_: \
		if( !dst.tryAppend( Str( s, q ) ) ) { goto fail; } \
		if( !dst.tryAppend( SrcCh_ ) ) { goto fail; } \
		break;
			AXSTR_ESCAPE_LIST_()
#undef AXSTR_ESCAPE_
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				if( p + 2 <= e ) {
					const char *os = p + 1;
					const char *op = os;
					while( op < e && axstr_isdigit_x( *op, 8 ) ) {
						++op;
						if( op - os > 3 ) {
							os = op - 3;
						}
					}
					const axstr_uint_t v = Str( os, op ).toUnsignedInteger( 8, '\0' );
					if( !dst.tryAppend( char((unsigned char)v) ) ) {
						goto fail;
					}
					p = op - 1; // p will be incremented later
				}
				break;
			case 'x':
				if( p + 2 <= e ) {
					const char *hs = p + 1;
					const char *hp = hs;
					while( hp < e && axstr_isdigit_x( *hp, 16 ) ) {
						++hp;
						if( hp - hs > 2 ) {
							hs = hp - 2;
						}
					}
					const axstr_uint_t v = Str( hs, hp ).toUnsignedInteger( 16, '\0' );
					if( !dst.tryAppend( char((unsigned char)v) ) ) {
						goto fail;
					}
					p = hp - 1; // p will be incremented later
				}
				break;
			case 'u':
				if( p + 5 <= e ) {
					const axstr_uint_t v = Str( p + 1, p + 5 ).toUnsignedInteger( 16, '\0' );
					char buf[ 5 ] = { '\0', '\0', '\0', '\0', '\0' };
					{
						axstr_utf8_t *up = (axstr_utf8_t*)&buf[0];
						axstr_utf8_t *const ue = (axstr_utf8_t*)&buf[sizeof(buf)-1];

						if( !axstr_step_utf8_encode( &up, ue, axstr_utf32_t(v) ) ) {
							buf[0] = '\?';
						}
					}
					if( !dst.tryAppend( Str( buf ) ) ) {
						goto fail;
					}
					p += 4;
				}
				break;
			case 'U':
				if( p + 9 <= e ) {
					const axstr_uint_t v = Str( p + 1, p + 9 ).toUnsignedInteger( 16, '\0' );
					char buf[ 5 ] = { '\0', '\0', '\0', '\0', '\0' };
					{
						axstr_utf8_t *up = (axstr_utf8_t*)&buf[0];
						axstr_utf8_t *const ue = (axstr_utf8_t*)&buf[sizeof(buf)-1];

						if( !axstr_step_utf8_encode( &up, ue, axstr_utf32_t(v) ) ) {
							buf[0] = '\?';
						}
					}
					if( !dst.tryAppend( Str( buf ) ) ) {
						goto fail;
					}
					p += 8;
				}
				break;
			default:
				break;
			}

			++p;
			s = p;
		}

		if( !dst.tryAppend( Str( s, p ) ) ) {
			goto fail;
		}

		return true;

	fail:
		dst.clear();
		return false;
	}

	template< typename Allocator >
	inline bool Str::replaceTo( TMutStr< Allocator > &dst, Str searchFor, Str replaceWith ) const
	{
		dst.clear();

		Str src( *this );
		while( src.isUsed() ) {
			const DiffType foundPos = src.find( searchFor );
			if( foundPos != -1 ) {
				if( !dst.tryAppend( src.left( foundPos ) ) ) {
					goto fail;
				}

				if( !dst.tryAppend( replaceWith ) ) {
					goto fail;
				}

				src = src.skip( foundPos );
				src = src.skip( searchFor.len() );
			} else {
				if( !dst.tryAppend( src ) ) {
					goto fail;
				}

				src = Str();
			}
		}

		return true;

	fail:
		dst.clear();
		return false;
	}

	inline TMutStr<> Str::quoted() const
	{
		TMutStr<> tmp;

		if( !quoteTo( tmp ) ) {
			return TMutStr<>();
		}

		return tmp;
	}
	inline Str Str::unquoted() const
	{
		if( startsWith( '\"' ) && endsWith( '\"' ) ) {
			return skip().drop();
		}

		return *this;
	}

	inline TMutStr<> Str::tabbed( SizeType levels ) const
	{
		TMutStr<> tmp;

		if( !tabTo( tmp, levels ) ) {
			return TMutStr<>();
		}

		return tmp;
	}
	inline TMutStr<> Str::untabbed( SizeType levels ) const
	{
		TMutStr<> tmp;

		if( !untabTo( tmp, levels ) ) {
			return TMutStr<>();
		}

		return tmp;
	}

	inline TMutStr<> Str::escaped() const
	{
		TMutStr<> tmp;

		if( !escapeTo( tmp ) ) {
			return TMutStr<>();
		}

		return tmp;
	}
	inline TMutStr<> Str::unescaped() const
	{
		TMutStr<> tmp;

		if( !unescapeTo( tmp ) ) {
			return TMutStr<>();
		}

		return tmp;
	}

	inline TMutStr<> Str::replaced( Str searchFor, Str replaceWith ) const
	{
		TMutStr<> tmp;

		if( !replaceTo( tmp, searchFor, replaceWith ) ) {
			return TMutStr<>();
		}

		return tmp;
	}

}

# if AXSTR_CXX_OVERLOADS_ENABLED
inline char *axstr_dupto( char **ppszOutDst, const ax::Str &src )
{
	return axstr_dupton( ppszOutDst, src.get(), src.len() );
}
inline char *axstr_dup( const ax::Str &src )
{
	return axstr_dupn( src.get(), src.len() );
}
inline axstr_size_t axstr_stream( char **ppszDstBuf, axstr_size_t *pcDstBytes, const ax::Str &src )
{
	return axstr_streamn( ppszDstBuf, pcDstBytes, src.get(), src.len() );
}
inline axstr_size_t axstr_cpy( char *pszDstBuf, axstr_size_t cDstBytes, const ax::Str &src )
{
	return axstr_cpyn( pszDstBuf, cDstBytes, src.get(), src.len() );
}
inline axstr_size_t axstr_cat( char *pszDstBuf, axstr_size_t cDstBytes, const ax::Str &src )
{
	return axstr_catn( pszDstBuf, cDstBytes, src.get(), src.len() );
}
inline axstr_size_t axstr_catpath( char *pszDstBuf, axstr_size_t cDstBytes, const ax::Str &src )
{
	return axstr_catpathn( pszDstBuf, cDstBytes, src.get(), src.len() );
}

template< axstr_size_t tDstSize >
inline axstr_size_t axstr_cpy( AXSTR_OUT_Z( tDstSize ) char( &pszDstBuf )[ tDstSize ], const ax::Str &src )
{
	return axstr_cpyn( pszDstBuf, tDstSize, src.get(), src.len() );
}
template< axstr_size_t tDstSize >
inline axstr_size_t axstr_cat( AXSTR_OUT_Z( tDstSize ) char( &pszDstBuf )[ tDstSize ], const ax::Str &src )
{
	return axstr_catn( pszDstBuf, tDstSize, src.get(), src.len() );
}
template< axstr_size_t tDstSize >
inline axstr_size_t axstr_catpath( AXSTR_OUT_Z( tDstSize ) char( &pszDstBuf )[ tDstSize ], const ax::Str &src )
{
	return axstr_catpathn( pszDstBuf, tDstSize, src.get(), src.len() );
}
# endif /*AXSTR_CXX_OVERLOADS_ENABLED*/

# if AXSTR_CXX_ENABLED
namespace ax
{

	template< typename TArrayContainer >
	inline bool Str::splitTo( TArrayContainer &outArray, Str inSeparator, EKeepEmpty::Type keepEmpty ) const
	{
		typedef policy::StringArrayManipulation<TArrayContainer> ArrayOps;

		ArrayOps::clear( outArray );

		if( inSeparator.isEmpty() ) {
			return false;
		}

		const axstr_size_t myLen = len();
		const axstr_size_t sepLen = inSeparator.len();

		DiffType index = 0;
		do {
			const  DiffType newIndex = find( inSeparator, index );
			const SizeType range = newIndex == -1 ? myLen : newIndex;
			const SizeType next = newIndex == -1 ? range : newIndex + sepLen;
			const SizeType count = range - SizeType( index );

			if( count > 0 || keepEmpty == EKeepEmpty::Yes ) {
				if( !ArrayOps::pushBack( outArray, mid( index, count ) ) ) {
					return false;
				}
			}

			index = next;
		} while( ( SizeType )index != myLen );

		return true;
	}

	template< typename TArrayContainer >
	inline bool Str::splitUnquotedTo( TArrayContainer &outArray, Str inSeparator, Str inEscape, EKeepQuotes::Type keepQuotes, EKeepEmpty::Type keepEmpty ) const
	{
		typedef policy::StringArrayManipulation<TArrayContainer> ArrayOps;

		ArrayOps::clear( outArray );

		if( inSeparator.isEmpty() ) {
			return false;
		}

		const SizeType myLen = len();
		if( !myLen ) {
			return true;
		}

		const SizeType sepLen = inSeparator.len();
		const SizeType escLen = inEscape.len();

		SizeType index = 0;
		bool bInQuote = false;
		const SizeType n = keepQuotes == EKeepQuotes::No ? 1 : 0;
		for( SizeType p = 0; p <= myLen; ++p ) {
			// Handle escapes
			if( escLen > 0 && mid( p, escLen ) == inEscape ) {
				p += escLen;
				/* TODO: Increment by character length (minus one) */
				continue;
			}

			// Handle the separator
			if( ( mid( p, sepLen ) == inSeparator && !bInQuote ) || p == myLen ) {
				if( keepEmpty == EKeepEmpty::Yes || p != index ) {
					const SizeType nn = bInQuote ? n : 0;
					if( !ArrayOps::pushBack( outArray, substr( index + nn, p ) ) ) {
						return false;
					}
				}

				p += sepLen - 1;
				index = p + 1;
				continue;
			}

			// Handle quotes
			if( mid( p, 1 ) == '\"' ) {
				bInQuote = !bInQuote;
				if( bInQuote ) {
					continue;
				}

				if( !ArrayOps::pushBack( outArray, substr( index + n, p + 1 - n ) ) ) {
					return false;
				}

				if( mid( p + 1, sepLen ) == inSeparator ) {
					p += sepLen;
				}

				index = p + 1;
			}
		}

		return true;
	}

	template< typename TArrayContainer >
	inline bool Str::splitLinesTo( TArrayContainer &outArr, EKeepEmpty::Type keepEmpty ) const
	{
		typedef policy::StringArrayManipulation<TArrayContainer> ArrayOps;

		ArrayOps::clear( outArr );

		const DiffType myLen = DiffType( len() );

		DiffType next = myLen;
		for( DiffType index = 0; index < myLen; index = next ) {
			DiffType nl = findAny( "\r\n", index - 1 );
			if( nl == -1 ) {
				nl = myLen;
			}

			const SizeType count = SizeType( nl - index );
			const Str part = mid( index, count );

			next = nl + ( nl + 1 < myLen && m_pStr[ nl ] == '\r' && m_pStr[ nl + 1 ] == '\n' ? 2 : 1 );

			if( keepEmpty == EKeepEmpty::No && part.isEmpty() ) {
				continue;
			}

			if( !ArrayOps::pushBack( outArr, part ) ) {
				return false;
			}
		}

		return true;
	}

#if AXSTR_OBJMUTSTR_ENABLED
	template< typename TArrayContainer >
	inline bool ObjMutStr::splitTo( TArrayContainer &outArray, Str inSeparator, EKeepEmpty::Type keepEmpty ) const
	{
		return Str( *this ).splitTo< TArrayContainer >( outArray, inSeparator, keepEmpty );
	}

	template< typename TArrayContainer >
	inline bool ObjMutStr::splitUnquotedTo( TArrayContainer &outArray, Str inSeparator, Str inEscape, EKeepQuotes::Type keepQuotes, EKeepEmpty::Type keepEmpty ) const
	{
		return Str( *this ).splitUnquotedTo< TArrayContainer >( outArray, inSeparator, inEscape, keepQuotes, keepEmpty );
	}
#endif

}
# endif

# if !defined( AX_NO_INCLUDES ) && defined( __has_include )
#  if AXSTR_CXX_ENABLED && __has_include( "ax_array.hpp" )
#   include "ax_array.hpp"
#  endif
# endif

# if defined( INCGUARD_AX_ARRAY_HPP_ ) && AXSTR_CXX_ENABLED
namespace ax
{

#if 0
	inline TMutArr< Str > Str::split( Str inSeparator, EKeepEmpty::Type keepEmpty ) const
	{
		TMutArr< Str > Arr;
		return splitTo( Arr, inSeparator, keepEmpty ) ? Arr : TMutArr< Str >();
	}
#endif
#if 0
	inline TMutArr< Str > Str::splitUnquoted( Str inSeparator, Str inEscape, EKeepQuotes::Type keepQuotes, EKeepEmpty::Type keepEmpty ) const
	{
		TMutArr< Str > Arr;
		return splitUnquotedTo( Arr, inSeparator, inEscape, keepQuotes, keepEmpty ) ? Arr : TMutArr< Str >();
	}
#endif
#if 0
	/*! \brief As with `splitLinesTo()` but returns the array directly. */
	inline TMutArr< Str > Str::splitLines() const
	{
		TMutArr< Str > arr;
		return splitLinesTo( arr ) ? arr : TMutArr< Str >();
	}
#endif
#if 0
	inline TMutArr< Str > MutStr::split( Str inSeparator, EKeepEmpty::Type keepEmpty ) const
	{
		return Str( *this ).split( inSeparator, keepEmpty );
	}
#endif
#if 0
	inline TMutArr< Str > MutStr::splitUnquoted( Str inSeparator, Str inEscape, EKeepQuotes::Type keepQuotes, EKeepEmpty::Type keepEmpty ) const
	{
		return Str( *this ).splitUnquoted( inSeparator, inEscape, keepQuotes, keepEmpty );
	}
#endif

#if AXSTR_OBJMUTSTR_ENABLED
	template< typename TString >
	inline bool ObjMutStr::mergeTo( ObjMutStr &outStr, const TArr< TString > &inArr, Str inGlue )
	{
		outStr.clear();

		const bool bAppendGlue = inGlue.isUsed();
		for( SizeType i = 0; i < inArr.num(); ++i ) {
			if( !outStr.tryAppend( inArr[ i ] ) ) {
				return false;
			}

			if( bAppendGlue && !outStr.tryAppend( inGlue ) ) {
				return false;
			}
		}

		return true;
	}
	template< typename TString >
	inline ObjMutStr ObjMutStr::merge( const TArr< TString > &inArr, Str inGlue )
	{
		ObjMutStr Str;
		return mergeTo( Str, inArr, inGlue ) ? Str : ObjMutStr();
	}
#endif

}
# endif /*INCGUARD_AX_ARRAY_HPP_*/

#endif

#endif
