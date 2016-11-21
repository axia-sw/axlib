/*

	ax_printf - public domain
	Last update: 2015-10-01 Aaron Miller


	TODO

	- Add proper %g and %e support.
	- Test %f more, and add support for #INF, #NAN, etc.


	See: http://www.cplusplus.com/reference/cstdio/printf/
	See: http://www.unix.com/man-page/FreeBSD/9/printf/
	See: http://www.unix.com/man-page/freebsd/3/syslog/


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_PRINTF_H_
#define INCGUARD_AX_PRINTF_H_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

#if !defined( AX_NO_INCLUDES ) && defined( __has_include )
# if __has_include( "ax_platform.h" )
#  include "ax_platform.h"
# endif
# if __has_include( "ax_types.h" )
#  include "ax_types.h"
# endif
#endif

#ifdef AXPRINTF_IMPLEMENTATION
# define AXPF_IMPLEMENT             1
#else
# define AXPF_IMPLEMENT             0
#endif

#ifndef AXPF_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXPF_CXX_ENABLED          AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXPF_CXX_ENABLED          1
# else
#  define AXPF_CXX_ENABLED          1
# endif
#endif

#ifndef AXPF_CXX_OVERLOADS_ENABLED
# define AXPF_CXX_OVERLOADS_ENABLED 1
#endif

#ifndef AXPF_STDFILE_ENABLED
# define AXPF_STDFILE_ENABLED       1
#endif

#ifndef AXPF_UTF8CONV_ENABLED
# define AXPF_UTF8CONV_ENABLED      1
#endif

#ifndef AXPF_PARM_ANNO
# ifdef AX_PRINTF_PARM
#  define AXPF_PARM_ANNO            AX_PRINTF_PARM
# else
#  define AXPF_PARM_ANNO
# endif
#endif

#ifndef AXPF_FUNC_ANNO
# ifdef AX_PRINTF_ATTR
#  define AXPF_FUNC_ANNO(Fmt_,AL_)  AX_PRINTF_ATTR(Fmt_,AL_)
# else
#  define AXPF_FUNC_ANNO(Fmt_,AL_)
# endif
#endif

/* function declaration specifier */
#ifndef AXPF_FUNC
# ifdef AX_FUNC
#  define AXPF_FUNC                 AX_FUNC
# else
#  define AXPF_FUNC                 extern
# endif
#endif

/* function calling convention */
#ifndef AXPF_CALL
# ifdef AX_CALL
#  define AXPF_CALL                 AX_CALL
# else
#  define AXPF_CALL
# endif
#endif

#ifndef axpf_alloc
# include <stdlib.h>
# define axpf_alloc(N_)             (malloc((N_)))
# define axpf_realloc(P_,N_)        (realloc((P_),(N_)))
# define axpf_free(P_)              (free((P_)))
#endif

#if defined( _WIN32 ) && AXPF_UTF8CONV_ENABLED
# define AXPF__WINCON 1
#else
# define AXPF__WINCON 0
#endif

#if AXPF_STDFILE_ENABLED
# include <stdio.h> /* Needed for FILE ;; TODO: Make inclusion optional */
#endif
#include <stdarg.h>
#include <wchar.h> /* jimmio92 - needed for wcschr */
#if AXPF_IMPLEMENT
# include <errno.h>
# include <string.h> /* TODO: Don't rely on this */
# if AXPF__WINCON
#  include <Windows.h>
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
# undef AXPF_CXX_ENABLED
# define AXPF_CXX_ENABLED           0
#endif

#if !AXPF_CXX_ENABLED
# undef AXPF_CXX_OVERLOADS_ENABLED
# define AXPF_CXX_OVERLOADS_ENABLED 0
#endif

#ifdef AX_TYPES_DEFINED
typedef ax_s8_t                     axpf_s8_t;
typedef ax_s16_t                    axpf_s16_t;
typedef ax_s32_t                    axpf_s32_t;
typedef ax_s64_t                    axpf_s64_t;
typedef ax_u8_t                     axpf_u8_t;
typedef ax_u16_t                    axpf_u16_t;
typedef ax_u32_t                    axpf_u32_t;
typedef ax_u64_t                    axpf_u64_t;
typedef ax_sptr_t                   axpf_sptr_t;
typedef ax_uptr_t                   axpf_uptr_t;
typedef ax_smax_t                   axpf_smax_t;
typedef ax_umax_t                   axpf_umax_t;
#elif defined( _MSC_VER )
# include <stddef.h>
typedef signed __int8               axpf_s8_t;
typedef signed __int16              axpf_s16_t;
typedef signed __int32              axpf_s32_t;
typedef signed __int64              axpf_s64_t;
typedef unsigned __int8             axpf_u8_t;
typedef unsigned __int16            axpf_u16_t;
typedef unsigned __int32            axpf_u32_t;
typedef unsigned __int64            axpf_u64_t;
typedef ptrdiff_t                   axpf_sptr_t;
typedef size_t                      axpf_uptr_t;
typedef ax_s64_t                    axpf_smax_t;
typedef ax_u64_t                    axpf_umax_t;
#else
# include <stdint.h>
# include <stddef.h>
typedef int8_t                      axpf_s8_t;
typedef int16_t                     axpf_s16_t;
typedef int32_t                     axpf_s32_t;
typedef int64_t                     axpf_s64_t;
typedef uint8_t                     axpf_u8_t;
typedef uint16_t                    axpf_u16_t;
typedef uint32_t                    axpf_u32_t;
typedef uint64_t                    axpf_u64_t;
typedef ptrdiff_t                   axpf_sptr_t;
typedef size_t                      axpf_uptr_t;
typedef intmax_t                    axpf_smax_t;
typedef uintmax_t                   axpf_umax_t;
#endif
typedef axpf_sptr_t                 axpf_ptrdiff_t;
typedef axpf_uptr_t                 axpf_size_t;

#ifdef _MSC_VER
typedef axpf_s64_t                  axpf_longlong_t;
typedef axpf_u64_t                  axpf_ulonglong_t;
#else
typedef signed long long int        axpf_longlong_t;
typedef unsigned long long int      axpf_ulonglong_t;
#endif

typedef axpf_ptrdiff_t( AXPF_CALL *axpf_write_fn_t )( void *, const char *, const char * );

struct axpf__state_;

struct axpf__write_mem_data_
{
	char *p;
	axpf_size_t i;
	axpf_size_t n;
};

AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpf__write_mem_f
(
	void *data,
	const char *s,
	const char *e
)
#if AXPF_IMPLEMENT
{
	struct axpf__write_mem_data_ *md;
	axpf_size_t n;

	md = ( struct axpf__write_mem_data_ * )data;

	n = ( axpf_size_t )( e - s );
	if( !n ) {
		return 0;
	}

	if( md->i + n > md->n ) {
		n = md->n - md->i;
	}

	memcpy( ( void * )&md->p[ md->i ], ( const void * )s, n );
	md->i += n;
	md->p[ md->i ] = '\0';

	return n;
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpf__write_memdup_f
(
	void *data,
	const char *s,
	const char *e
)
#if AXPF_IMPLEMENT
{
	struct axpf__write_mem_data_ *md;
	axpf_size_t n;
	char *p;

	md = ( struct axpf__write_mem_data_ * )data;

	n = ( axpf_size_t )( e - s );
	if( !n ) {
		return 0;
	}

	if( md->i + n > md->n ) {
		axpf_size_t capacity;

		capacity = md->i + n + 1;
		capacity += ( capacity - capacity%32 ) + 32;

		p = ( char * )( !md->p ? axpf_alloc( capacity ) : axpf_realloc( ( void * )md->p, capacity ) );
		if( !p ) {
			return ( axpf_ptrdiff_t )-1;
		}

		md->p = p;
		md->n = capacity;
	}

	memcpy( ( void * )&md->p[ md->i ], ( const void * )s, n );
	md->i += n;
	md->p[ md->i ] = '\0';

	return n;
}
#else
;
#endif
#if AXPF_IMPLEMENT && AXPF_UTF8CONV_ENABLED
static axpf_size_t AXPF_CALL axpf__utf8towchar
(
	wchar_t *pwszDstBuf,
	wchar_t *pwszDstBufEnd,
	const char **ps,
	const char *e,
	axpf_size_t *pcDstLen
)
{
# define AXPF__UTF8_INVALCP 0xFFFD
	axpf_size_t r;
	const char *s;
	wchar_t *d;

	d =  pwszDstBuf;
	s = *ps;

	while( s != e && d + 2 < pwszDstBufEnd ) {
		unsigned advance;
		unsigned ch, cp;

		ch = +( *( const unsigned char * )s++ );
		cp = 0;
		if( ( ch & 0xF8 ) == 0xF0 ) {
			advance = 4;
			cp = ch & ~0xF8;
		} else if( ( ch & 0xF0 ) == 0xE0 ) {
			advance = 3;
			cp = ch & ~0xE0;
		} else if( ( ch & 0xE0 ) == 0xC0 ) {
			advance = 2;
			cp = ch & ~0xC0;
		} else if( ( ch & 0x80 ) == 0x00 ) {
			advance = 1;
			cp = ch & 0x7F;
		} else {
			advance = 0;
			cp = AXPF__UTF8_INVALCP;
		}

		while( advance-- > 1 ) {
			if( s == e ) {
				cp = AXPF__UTF8_INVALCP;
				break;
			}

			ch = +( *( const unsigned char * )s++ );
			if( ( ch & 0xC0 ) != 0x80 ) {
				cp = AXPF__UTF8_INVALCP;
				break;
			}

			cp <<= 6;
			cp  |= ch & 0x3F;
		}

		if( cp > 0x10FFFF || ( cp >= 0xD800 && cp <= 0xDFFF ) ) {
			cp = AXPF__UTF8_INVALCP;
		}

		if( sizeof( wchar_t ) == 2 ) {
			if( cp < 0x010000 ) {
				*d++ = cp;
			} else {
				*d++ = 0xD800 + ( ( cp - 0x010000 ) & 0x03FF );
				*d++ = cp & 0xFFFF;
			}
		} else {
			*d++ = cp;
		}
	}
	if( pcDstLen != ( axpf_size_t * )0 ) {
		*pcDstLen = d - pwszDstBuf;
	}
	*d = L'\0';

	r = ( axpf_size_t )( s - *ps );
	*ps = s;

	return r;
}
#endif
#if AXPF__WINCON
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpf__write_con_f
(
	void *data,
	const char *s,
	const char *e
	)
# if AXPF_IMPLEMENT
{
	axpf_ptrdiff_t r;
	axpf_size_t t, dt;
	wchar_t wszBuf[ 512 ];
	DWORD x;

	r = 0;
	while( ( t = axpf__utf8towchar( wszBuf, &wszBuf[ 511 ], &s, e, &dt ) ) != 0 ) {
		if( !WriteConsoleW( ( HANDLE )data, ( const void * )wszBuf, ( DWORD )dt, &x, ( LPVOID )0 ) ) {
			return -1;
		}

		r += t;
	}

	( ( void )x );
	return r;
}
# else
	;
# endif
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpf__write_fp_f
(
	void *data,
	const char *s,
	const char *e
)
#if AXPF_IMPLEMENT
{
# if AXPF_UTF8CONV_ENABLED
	static axpf_size_t *const pfsztnil = ( axpf_size_t * )0;
	axpf_ptrdiff_t r;
	axpf_size_t t;
	wchar_t wszBuf[ 512 ];
	int didfail;

	r = 0;
	didfail = 0;
	while( ( t = axpf__utf8towchar( wszBuf, &wszBuf[ 511 ], &s, e, pfsztnil ) ) != 0 ) {
		if( fputws( wszBuf, ( FILE * )data ) >= 0 ) {
			r += t;
		} else if( data != ( void * )stdout || data != ( void * )stderr ) {
			didfail = 1;
		}
	}

	return didfail ? -1 : r;
# else
	if( !fwrite( ( const void * )s, ( axpf_size_t )( e - s ), 1, ( FILE * )data ) ) {
		return ( axpf_ptrdiff_t )-1;
	}
#endif

	return ( axpf_ptrdiff_t )( e - s );
}
#else
;
#endif

enum
{
	/* "-" Left-justify within the given field width */
	kAxPF_Left = 1<<0,
	/* "+" Result will be preceeded by either a - or + */
	kAxPF_Sign = 1<<1,
	/* " " Result will be preceeded by either a - or a space */
	kAxPF_Space = 1<<2,
	/* "#" Insert an appropriate radix (0, 0x, 0X) */
	kAxPF_Radix = 1<<3,
	/* "0" Left-pad the number with zeroes instead of spaces */
	kAxPF_Zero = 1<<4,
	/* "'" Use a digit separator */
	kAxPF_Group = 1<<5,

	/* [Internal] Printing an array */
	kAxPF_Array = 1<<10,
	/* [Internal] Uppercase */
	kAxPF_Upper = 1<<11,
	/* [Internal] File path (replace '\' with '/' on Windows) */
	kAxPF_FSPath = 1<<12,
	/* [Internal] Width directive was specified */
	kAxPF_Width = 1<<13,
	/* [Internal] Precision directive was specified */
	kAxPF_Precision = 1<<14,
	/* [Internal] Use a lower-case radix (for pointer printing) */
	kAxPF_Pointer = 1<<15
};
typedef enum
{
	kAxLS_None,
	kAxLS_hh,
	kAxLS_h,
	kAxLS_l,
	kAxLS_ll,
	kAxLS_j,
	kAxLS_z,
	kAxLS_t,
	kAxLS_L,

	kAxLS_I,
	kAxLS_I32,
	kAxLS_I64
} axpf__lengthSpecifier_t;

#define AXPF__MAX_ARRAY_PRINT 4
struct axpf__state_
{
	axpf_write_fn_t pfn_write;
	void *write_data;
	axpf_size_t num_written;
	int diderror;

	unsigned repeats;
	axpf_size_t arraysize;
	char arrayprint[ AXPF__MAX_ARRAY_PRINT ];
	unsigned flags;
	int width;
	int precision;
	axpf__lengthSpecifier_t lenspec;
	unsigned radix;
	va_list args;
	const char *s;
	const char *e;
	const char *p;
};

#if AXPF_IMPLEMENT

static char axpf__read( struct axpf__state_ *s )
{
	return s->p < s->e ? *s->p++ : '\0';
}
static char axpf__look( struct axpf__state_ *s )
{
	return s->p < s->e ? *s->p : '\0';
}
static void axpf__skip( struct axpf__state_ *s )
{
	if( s->p < s->e ) {
		s->p++;
	}
}

static int axpf__check( struct axpf__state_ *s, int ch )
{
	if( s->p < s->e && *s->p == ch ) {
		++s->p;
		return 1;
	}

	return 0;
}
static int axpf__checks( struct axpf__state_ *s, const char *p )
{
	if( s->p < s->e && *s->p == *p ) {
		const char *q;

		q = s->p + 1;
		while( *q++ == *++p ) {
			if( !*p ) {
				break;
			}
		}
		if( !*p ) {
			s->p = q;
			return 1;
		}
	}

	return 0;
}

static int axpf__write( struct axpf__state_ *s, const char *p, const char *e )
{
	axpf_ptrdiff_t r;

	if( p == e ) {
		return 1;
	}

	if( s->flags & kAxPF_FSPath ) {
		const char *q, *b;
		char chslash = '/';

		b = p;
		for( q = p; q < e; ++q ) {
			if( *q != '\\' ) {
				continue;
			}

			r = 0;
			if( b != q && ( r = s->pfn_write( s->write_data, b, q ) ) == -1 ) {
				s->diderror = 1;
				return 0;
			}

			b = q + 1;
			if( ( s->pfn_write( s->write_data, &chslash, &chslash + 1 ) ) == -1 ) {
				s->diderror = 1;
				return 0;
			}

			if( s->num_written + 1 + ( axpf_size_t )r >= s->num_written ) {
				s->num_written += 1 + ( axpf_size_t )r;
			} else {
				s->num_written = ( ~( axpf_size_t )0 ) - 1;
			}
		}

		p = b;
	}

	r = s->pfn_write( s->write_data, p, e );
	if( r == -1 ) {
		s->diderror = 1;
		return 0;
	}

	if( s->num_written + ( axpf_size_t )r >= s->num_written ) {
		s->num_written += ( axpf_size_t )r;
	} else {
		s->num_written = ( ~( axpf_size_t )0 ) - 1;
	}

	return 1;
}
static int axpf__writech( struct axpf__state_ *s, char ch )
{
	return axpf__write( s, &ch, &ch + 1 );
}

static char *axpf__utoa( char *end, axpf_umax_t i, unsigned radix, int flags )
{
	static const char *lower = "0123456789abcdefghijklmnopqrstuvwxyz";
	static const char *upper = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	const char *digits;
	unsigned groupingdist, groupingbase;
	char groupingchar;
	char *p;

	if( radix < 2 ) { radix = 2; }
	if( radix > 36 ) { radix = 36; }

	digits = ( flags & 1 ) ? upper : lower;

	/* TODO: Grab information from locale */
	groupingbase = ( flags & 4 ) ? 3 : 9999;
	groupingdist = groupingbase + 1;
	groupingchar = ',';

	p = end - 1;

	*p = '\0';

	while( i > 0 ) {
		if( !--groupingdist ) {
			groupingdist = groupingbase;
			*--p = groupingchar;
		}
		*--p = digits[ i%radix ];
		i /= radix;
	}
	if( *p == '\0' && ( ~flags & 2 ) ) {
		*--p = '0';
	}

	return p;
}
static int axpf__dtoax( char *buf, char *outsign, double f, int usescinot, unsigned maxdigits, unsigned radix )
{
	/*
		ORIGINAL: http://www.edaboard.com/thread5585.html (by cb30)
	*/
	union {
		axpf_s64_t i;
		double f;
	} x;
	axpf_s64_t mantissa, whole, fract;
	axpf_s16_t exp2;

	( void )usescinot;

	if( !f ) {
		*buf++ = '0';
		*buf++ = '.';
		*buf++ = '0';
		*buf   = '\0';

		return 0;
	}

	x.f = f;
	if( x.i < 0 ) {
		x.i &= ~( 1ULL<<63 );
		*outsign = '-';
	} else {
		*outsign = '+';
	}

	exp2 = ( ( axpf_s16_t )( ( x.i>>52 ) & 0x7FF ) ) - 1023;
	mantissa = ( x.i & 0x1FFFFFFFFFFFFF ) | 0x10000000000000;
	whole = 0;
	fract = 0;

	if( exp2 >= 63 ) {
		*buf = '\0';
		return 1;
	}

	if( exp2 < -52 ) {
		*buf = '\0';
		return -1;
	}

	if( exp2 >= 52 ) {
		whole = mantissa << ( exp2 - 52 );
	} else if( exp2 >= 0 ) {
		whole = mantissa >> ( 52 - exp2 );
		fract = ( mantissa << ( exp2 + 1 ) ) & 0x1FFFFFFFFFFFFF;
	} else {
		fract = ( mantissa & 0x1FFFFFFFFFFFFF ) >> -( exp2 + 1 );
	}

	if( !whole ) {
		*buf++ = '0';
	} else {
		char tmp[ 32 ];
		char *p;

		p = axpf__utoa( &tmp[ sizeof( tmp ) - 1 ], whole, radix, 0 );
		while( *p != '\0' ) {
			*buf++ = *p++;
		}
	}

	*buf++ = '.';

	if( !fract ) {
		*buf++ = '0';
	} else {
		unsigned numdigits = 0;

		if( maxdigits > 32 ) {
			maxdigits = 32;
		}

		for( numdigits = 0; numdigits < maxdigits; ++numdigits ) {
			fract *= 10;
			*buf++ = ( char )( ( fract >> 53 ) + '0' );
			fract &= 0x1FFFFFFFFFFFFF;
		}
	}

	*buf = '\0';
	return 0;
}

static int axpf__pad( struct axpf__state_ *s, unsigned num, char ch )
{
	static const char spaces[] = "                ";
	static const char zeroes[] = "0000000000000000";
	const char *p;

	p = ( ch == ' ' ? &spaces[0] : ( ch == '0' ? &zeroes[0] : NULL ) );

	if( !p ) {
		while( num-- > 0 ) {
			if( !axpf__writech( s, ch ) ) {
				return 0;
			}
		}

		return 1;
	}

	while( num > 0 ) {
		unsigned n = num;

		if( n > sizeof( spaces ) - 1 ) {
			n = sizeof( spaces ) - 1;
		}

		if( !axpf__write( s, p, p + n ) ) {
			return 0;
		}

		num -= n;
	}

	return 1;
}
static int axpf__write_uintx( struct axpf__state_ *s, axpf_umax_t i, char sign )
{
	const char *prefix;
	unsigned prefixlen;
	unsigned numberlen, numbersignlen;
	unsigned numpadzeroes;
	unsigned numpadspaces;
	unsigned padinfringe;
	char buf[ 128 ];
	char *p;
	int f;

	f = 0;
	if( s->flags & kAxPF_Upper ) { f |= 1; }
	if( ( s->flags & kAxPF_Precision ) && s->precision == 0 ) { f |= 2; }
	if( s->flags & kAxPF_Group ) { f |= 4; }

	p = axpf__utoa( &buf[ sizeof( buf ) ], i, s->radix, f );

	prefix = NULL;
	prefixlen = 0;
	if( s->flags & ( kAxPF_Radix | kAxPF_Pointer ) ) {
		int useupper;

		if( ( s->flags & kAxPF_Pointer ) || ( ~s->flags & kAxPF_Upper ) ) {
			useupper = 0;
		} else {
			useupper = 1;
		}

		switch( s->radix ) {
		case 2:
			prefixlen = 2;
			prefix = useupper ? "0B" : "0b";
			break;
		case 8:
			prefixlen = 1;
			prefix = "0";
			break;
		case 16:
			prefixlen = 2;
			prefix = useupper ? "0X" : "0x";
			break;
		default:
			break;
		}
	}
	numberlen = ( unsigned )( axpf_size_t )( &buf[ sizeof( buf ) - 1 ] - p );

	if( !sign ) {
		if( s->flags & kAxPF_Sign ) {
			sign = '+';
		} else if( s->flags & kAxPF_Space ) {
			sign = ' ';
		}
	}
	numbersignlen = numberlen + +( sign != '\0' );

	numpadzeroes = 0;
	if( ( s->flags & kAxPF_Precision ) && ( unsigned )s->precision > numbersignlen ) {
		numpadzeroes = s->precision - numbersignlen;
	}

	numpadspaces = 0;
	padinfringe = numbersignlen + prefixlen + numpadzeroes;
	if( ( s->flags & kAxPF_Width ) && ( unsigned )s->width > padinfringe ) {
		if( s->flags & kAxPF_Zero ) {
			numpadzeroes += s->width - padinfringe;
		} else {
			numpadspaces += s->width - padinfringe;
		}
	}

	if( ~s->flags & kAxPF_Left ) {
		if( !axpf__pad( s, numpadspaces, ' ' ) ) {
			return 0;
		}
	}

	if( sign ) {
		if( !axpf__writech( s, sign ) ) {
			return 0;
		}
	}

	if( prefix != NULL && prefixlen > 0 ) {
		if( !axpf__write( s, prefix, prefix + prefixlen ) ) {
			return 0;
		}
	}
	axpf__pad( s, numpadzeroes, '0' );

	if( !axpf__write( s, p, &buf[ sizeof( buf ) - 1 ] ) ) {
		return 0;
	}

	if( s->flags & kAxPF_Left ) {
		if( !axpf__pad( s, numpadspaces, ' ' ) ) {
			return 0;
		}
	}

	return 1;
}
static int axpf__write_int( struct axpf__state_ *s, axpf_smax_t i )
{
	while( s->repeats-- > 0 ) {
		if( i < 0 ) {
			if( !axpf__write_uintx( s, ( axpf_umax_t )-i, '-' ) ) {
				return 0;
			}
		} else if( !axpf__write_uintx( s, ( axpf_umax_t )i, '\0' ) ) {
			return 0;
		}
	}

	return 1;
}
static int axpf__write_uint( struct axpf__state_ *s, axpf_umax_t i )
{
	while( s->repeats-- > 0 ) {
		if( !axpf__write_uintx( s, i, '\0' ) ) {
			return 0;
		}
	}

	return 1;
}

static int axpf__write_floatd( struct axpf__state_ *s, double f, char spec )
{
	unsigned maxdigits;
	char buf[ 128 ], *p, *end, *dec;
	char sign;
	int r;

	maxdigits = ( s->flags & kAxPF_Precision ) ? s->precision : 6;
	sign = '\0';
	p = &buf[ sizeof( buf )/2 ];
	r = axpf__dtoax( p, &sign, ( double )f, 0, maxdigits, s->radix );
	if( r < 0 ) {
		buf[ 0 ] = '#'; buf[ 1 ] = 'S'; buf[ 2 ] = '\0';
		p = &buf[ 0 ];
	} else if( r > 0 ) {
		buf[ 0 ] = '#'; buf[ 1 ] = 'L'; buf[ 2 ] = '\0';
		p = &buf[ 0 ];
	}

	if( sign != '-' && ( ~s->flags & kAxPF_Sign ) ) {
		sign = '\0';
	}

	end = strchr( p, '\0' );

	if( spec == 'g' ) {
		while( end > &buf[ 0 ] && *( end - 1 ) == '0' ) {
			*--end = '\0';
		}

		if( end > &buf[ 0 ] && *( end - 1 ) == '.' ) {
			*--end = '\0';
		}
	}

	dec = strchr( p, '.' );
	if( !dec ) {
		dec = end;
	}

	if( ( s->flags & kAxPF_Precision ) && s->precision > 0 && r == 0 ) {
		unsigned numdigits;

		if( *dec != '.' ) {
			dec[ 0 ] = '.';
			dec[ 1 ] = '\0';
			end = &dec[ 1 ];
		}

		numdigits = ( unsigned )( axpf_size_t )( end - dec ) - 1;
		while( ( int )numdigits < s->precision && numdigits < 32 ) {
			*end++ = '0';
			++numdigits;
		}

		*end = '\0';
	}

	if( ( s->flags & kAxPF_Width ) && s->width > 0 && r == 0 ) {
		unsigned numwidth;
		char ch;

		ch = ( s->flags & kAxPF_Zero ) ? '0' : ' ';

		numwidth = ( unsigned )( axpf_size_t )( dec - p );
		if( ch != '0' && sign != '\0' ) {
			*--p = sign;
			sign = '\0';
			++numwidth;
		}

		while( ( int )numwidth < s->width && numwidth < 32 ) {
			*--p = ch;
			++numwidth;
		}
	}

	if( sign != '\0' ) {
		*--p = sign;
	}

	while( s->repeats-- > 0 ) {
		if( !axpf__write( s, p, end ) ) {
			return 0;
		}
	}

	return 1;
}

static int axpf__write_char( struct axpf__state_ *s, int ch )
{
	while( s->repeats-- > 0 ) {
		if( !axpf__writech( s, ch ) ) {
			return 0;
		}
	}

	return 1;
}
static int axpf__writewch( struct axpf__state_ *s, wchar_t ch )
{
	( void )s;
	( void )ch;

	return 0;
}
static int axpf__write_wchar( struct axpf__state_ *s, wchar_t ch )
{
	while( s->repeats-- > 0 ) {
		if( !axpf__writewch( s, ch ) ) {
			return 0;
		}
	}

	return 1;
}

static int axpf__write_str( struct axpf__state_ *s, const char *p )
{
	const char *e;

	if( !p ) {
		p = "(null)";
		e = p + 6;
		if( s->flags & kAxPF_Precision ) {
			if( s->precision <= 6 ) {
				e = &p[ s->precision ];
			}
		}
	} else if( s->flags & kAxPF_Precision ) {
		e = &p[ s->precision ];
	} else {
		e = strchr( p, '\0' );
	}

	while( s->repeats-- > 0 ) {
		if( !axpf__write( s, p, e ) ) {
			return 0;
		}
	}

	return 1;
}
static int axpf__write_wstr( struct axpf__state_ *s, const wchar_t *p )
{
	const wchar_t *e;

	if( !p ) {
		p = L"(null)";
		e = p + 6;
		if( s->flags & kAxPF_Precision ) {
			if( s->precision <= 6 ) {
				e = &p[ s->precision ];
			}
		}
	} else if( s->flags & kAxPF_Precision ) {
		e = &p[ s->precision ];
	} else {
		e = wcschr( p, L'\0' );
	}

	while( s->repeats-- > 0 ) {
		/* FIXME: If `AXPF_UTF8CONV_ENABLED == 1` then just forward to the writer */
		while( p < e ) {
			if( !axpf__writewch( s, *p++ ) ) {
				return 0;
			}
		}
	}

	return 1;
}

static int axpf__write_syserr( struct axpf__state_ *s, int err )
{
	char errbuf[ 128 ];

#if defined( _MSC_VER ) && defined( __STDC_WANT_SECURE_LIB__ )
	if( strerror_s( errbuf, sizeof( errbuf ), err ) != 0 ) {
		errbuf[ 0 ] = '('; errbuf[ 1 ] = 'n'; errbuf[ 2 ] = 'u';
		errbuf[ 3 ] = 'l'; errbuf[ 4 ] = 'l'; errbuf[ 5 ] = ')';
		errbuf[ 6 ] = '\0';
	}
#else
	strncpy( errbuf, strerror( err ), sizeof( errbuf ) - 1 );
#endif
	errbuf[ sizeof( errbuf ) - 1 ] = '\0';

	while( s->repeats-- > 0 ) {
		if( !axpf__write( s, errbuf, strchr( errbuf, '\0' ) ) ) {
			return 0;
		}
	}

	return 1;
}
static int axpf__write_bitfield( struct axpf__state_ *s, int bits,
const char *names )
{
	int needcomma;

	if( !names || !*names ) {
		return 0;
	}

	if( !s->repeats ) {
		return 1;
	}

	s->radix = *( const unsigned char * )names;
	if( s->radix == 1 ) {
		s->radix = 10;
	}

	++names;

	if( !axpf__write_uint( s, ( unsigned )bits ) ) {
		return 0;
	}

	if( !axpf__writech( s, ' ' ) || !axpf__writech( s, '<' ) ) {
		return 0;
	}

	needcomma = 0;
	while( *names && bits ) {
		unsigned bit;

		bit = ( ( unsigned )*names++ ) - 1;
		if( bits & ( 1<<bit ) ) {
			const char *p;

			bits &= ~( 1<<bit );

			if( needcomma ) {
				axpf__writech( s, ',' );
			}

			p = names;
			while( *names > ' ' ) {
				++names;
			}

			if( !axpf__write( s, p, names ) ) {
				return 0;
			}

			needcomma = 1;
		}
	}

	if( !axpf__writech( s, '>' ) ) {
		return 0;
	}

	return 1;
}
static int axpf__write_hex_dump( struct axpf__state_ *s, const unsigned char *p,
const char *delimiter )
{
	const char *delimend;
	unsigned count = 16;
	unsigned n;
	unsigned i;

	if( !s->repeats ) {
		return 1;
	}

	if( !delimiter || !*delimiter ) {
		delimiter = " ";
	}

	delimend = strchr( delimiter, '\0' );
	n = ( unsigned )( axpf_size_t )( delimend - delimiter );

	s->radix = 16;

	if( ( s->flags & kAxPF_Width ) && s->width > 0 ) {
		count = s->width;
	}

	s->flags = kAxPF_Precision;
	s->precision = 2;
	for( i = 0; i < count; ++i ) {
		if( i > 0 && !axpf__writech( s, delimiter[ ( i - 1 )%n ] ) ) {
			return 0;
		}

		if( !axpf__write_uintx( s, p[ i ], '\0' ) ) {
			return 0;
		}
	}

	return 1;
}

static int axpf__find( struct axpf__state_ *s, int ch, int doflush )
{
	const char *p;
	axpf_size_t n;

	n = ( axpf_size_t )( s->e - s->p );
	if( !n ) {
		return 0;
	}

	p = ( const char * )memchr( ( const void * )s->p, ch, n );
	if( !p ) {
		return 0;
	}

	if( doflush && s->p != p ) {
		if( !axpf__write( s, s->p, p ) ) {
			return 0;
		}
	}

	s->p = p;
	return 1;
}

static int axpf__getdigit( int ch, unsigned radix )
{
	unsigned rlo, rhi;
	unsigned c;

	rlo = radix < 10 ? radix : 10;
	rhi = radix > 10 ? radix - 10 : 0;

	c = ( unsigned )ch;
	if( c >= '0' && c < '0' + rlo ) {
		return ch - '0';
	}

	if( c >= 'a' && c < 'a' + rhi ) {
		return ch - 'a' + 10;
	}
	if( c >= 'A' && c < 'A' + rhi ) {
		return ch - 'A' + 10;
	}

	return -1;
}

static unsigned axpf__step_uint( struct axpf__state_ *s )
{
	unsigned r = 0;

	for(;;) {
		int n;

		n = axpf__getdigit( axpf__look( s ), 10 );
		if( n < 0 ) {
			break;
		}

		if( r < ( 1L<<( sizeof( int )*8 - 2 ) ) ) {
			r *= 10;
			r += n;
		}
		axpf__skip( s );
	}

	return r;
}
static int axpf__step_int( struct axpf__state_ *s )
{
	int m = 1;

	if( axpf__check( s, '-' ) ) {
		m = -1;
	}

	return m*( int )axpf__step_uint( s );
}
static int axpf__step( struct axpf__state_ *s )
{
	char spec;

	if( !axpf__find( s, '%', 1 ) ) {
		axpf__write( s, s->p, s->e );
		return 0;
	}

	axpf__skip( s );

	if( axpf__check( s, '%' ) ) {
		axpf__writech( s, '%' );
		return 1;
	}

	s->repeats = 1;
	s->arraysize = 0;
	s->arrayprint[ 0 ] = '{';
	s->arrayprint[ 1 ] = ',';
	s->arrayprint[ 2 ] = ' ';
	s->arrayprint[ 3 ] = '}';
	s->flags = 0;
	s->width = 0;
	s->precision = 0;
	s->lenspec = kAxLS_None;
	s->radix = 10;

	/* repeats */
	if( axpf__check( s, '{' ) ) {
		if( axpf__check( s, '*' ) ) {
			s->repeats = va_arg( s->args, unsigned );
		} else {
			s->repeats = axpf__step_uint( s );
		}

		( void )axpf__check( s, '}' );
	}

	/* array */
	if( axpf__check( s, '[' ) ) {
		unsigned n = 0;

		if( axpf__check( s, '*' ) ) {
			s->arraysize = va_arg( s->args, axpf_size_t );
		} else {
			s->arraysize = axpf__step_uint( s );
		}
		s->flags |= kAxPF_Array;

		while( n < AXPF__MAX_ARRAY_PRINT ) {
			if( axpf__check( s, ']' ) ) {
				break;
			}

			s->arrayprint[ n++ ] = axpf__read( s );
		}

		if( n == AXPF__MAX_ARRAY_PRINT ) {
			( void )axpf__check( s, ']' );
		} else if( n > 0 ) {
			/* the last character specified is the closing value */
			s->arrayprint[ AXPF__MAX_ARRAY_PRINT - 1 ] = s->arrayprint[ n - 1 ];

			/* if a space was omitted then don't space elements out */
			if( n < 4 ) {
				s->arrayprint[ 2 ] = '\0';
			}

			/* if a delimiter was omitted then assume a comma */
			if( n < 3 ) {
				s->arrayprint[ 1 ] = ',';
			}
		}
	}

	/* flags */
	unsigned fcount;
	do {
		fcount = 0;

		if( axpf__check( s, '-' ) ) {
			s->flags |= kAxPF_Left;
			++fcount;
		}
		if( axpf__check( s, '+' ) ) {
			s->flags |= kAxPF_Sign;
			++fcount;
		}
		if( axpf__check( s, ' ' ) ) {
			s->flags |= kAxPF_Space;
			++fcount;
		}
		if( axpf__check( s, '#' ) ) {
			s->flags |= kAxPF_Radix;
			++fcount;
		}
		if( axpf__check( s, '0' ) ) {
			s->flags |= kAxPF_Zero;
			++fcount;
		}
		if( axpf__check( s, '\'' ) ) {
			s->flags |= kAxPF_Group;
			++fcount;
		}
		if( axpf__check( s, '/' ) ) {
			s->flags |= kAxPF_FSPath;
			++fcount;
		}
	} while( fcount > 0 );

	/* width */
	if( axpf__check( s, '*' ) ) {
		s->width = va_arg( s->args, int );
		s->flags |= kAxPF_Width;
	} else if( axpf__getdigit( axpf__look( s ), 10 ) >= 0 ) {
		s->width = axpf__step_int( s );
		s->flags |= kAxPF_Width;
	}

	/* precision */
	if( axpf__check( s, '.' ) ) {
		if( axpf__check( s, '*' ) ) {
			s->precision = va_arg( s->args, int );
			s->flags |= kAxPF_Precision;
		} else if( axpf__getdigit( axpf__look( s ), 10 ) >= 0 ) {
			s->precision = axpf__step_int( s );
			s->flags |= kAxPF_Precision;
		}
	}

	/* length specifier */
	if( axpf__check( s, 'h' ) ) {
		if( axpf__check( s, 'h' ) ) {
			s->lenspec = kAxLS_hh;
		} else {
			s->lenspec = kAxLS_h;
		}
	} else if( axpf__check( s, 'l' ) ) {
		if( axpf__check( s, 'l' ) ) {
			s->lenspec = kAxLS_ll;
		} else {
			s->lenspec = kAxLS_l;
		}
	} else if( axpf__check( s, 'j' ) ) {
		s->lenspec = kAxLS_j;
	} else if( axpf__check( s, 'z' ) ) {
		s->lenspec = kAxLS_z;
	} else if( axpf__check( s, 't' ) ) {
		s->lenspec = kAxLS_t;
	} else if( axpf__check( s, 'L' ) ) {
		s->lenspec = kAxLS_L;
	} else if( axpf__check( s, 'q' ) ) {
		s->lenspec = kAxLS_I64;
	} else if( axpf__check( s, 'w' ) ) {
		s->lenspec = kAxLS_l;
	} else if( axpf__check( s, 'I' ) ) {
		if( axpf__checks( s, "32" ) ) {
			s->lenspec = kAxLS_I32;
		} else if( axpf__checks( s, "64" ) ) {
			s->lenspec = kAxLS_I64;
		} else {
			s->lenspec = kAxLS_I;
		}
	}

	spec = axpf__read( s );
	if( spec >= 'A' && spec <= 'Z' && spec!='C' && spec!='D' && spec!='S' ) {
		s->flags |= kAxPF_Upper;
		spec = spec - 'A' + 'a';
	}

	/* arrays require special handling */
	if( s->flags & kAxPF_Array ) {
		const void *ptrbase;
		unsigned repeats;

		if( spec == 'r' ) {
			s->radix = va_arg( s->args, unsigned int );
		}
		ptrbase = va_arg( s->args, const void * );

		if( spec == 'a' ) {
			spec = 'f';
			s->radix = 16;
		}

		repeats = s->repeats;
		while( repeats-- > 0 ) {
			union {
				const void *p;
				const int *i; const unsigned int *u;
				const signed char *sc; const unsigned char *usc;
				const short int *si; const unsigned short int *usi;
				const long int *li; const unsigned long int *uli;
#ifdef _MSC_VER
				const __int64 *lli; const unsigned __int64 *ulli;
#else
				const long long int *lli; const unsigned long long int *ulli;
#endif
				const axpf_smax_t *im; const axpf_umax_t *uim;
				const axpf_size_t *sz;
				const axpf_ptrdiff_t *pd;
				const axpf_s32_t *i32; const axpf_u32_t *ui32;
				const axpf_s64_t *i64; const axpf_u64_t *ui64;
				const float *f;
				const double *d;
				const long double *ld;
				const char *c;
				const wchar_t *wc;
				const char *const *s;
				const wchar_t *const *ws;
			} x;
			axpf_size_t i;

			if( !ptrbase ) {
				const char buf[] = "(null)";
				axpf__write( s, buf, buf + sizeof(buf) - 1 );
				continue;
			}

			axpf__writech( s, s->arrayprint[ 0 ] );

			x.p = ptrbase;
			for( i = 0; i < s->arraysize; ++i ) {
				if( i > 0 ) {
					axpf__writech( s, s->arrayprint[ 1 ] );
				}
				if( s->arrayprint[ 2 ] == ' ' ) {
					axpf__writech( s, ' ' );
				}

				s->repeats = 1;
				switch( spec ) {
				case 'd':
				case 'i':
					switch( s->lenspec ) {
#define P_(F_) axpf__write_int( s, x.F_[i] ); break
					case kAxLS_None: P_( i );
					case kAxLS_hh:   P_( sc );
					case kAxLS_h:    P_( si );
					case kAxLS_l:    /*fallthrough*/
					case kAxLS_L:    P_( li );
					case kAxLS_ll:   P_( lli );
					case kAxLS_j:    P_( im );
					case kAxLS_z:    P_( sz );
					case kAxLS_I:    /*fallthrough*/
					case kAxLS_t:    P_( pd );
					case kAxLS_I32:  P_( i32 );
					case kAxLS_I64:  P_( i64 );
#undef P_
					}
					break;

				case 'u':
				case 'o':
				case 'x':
				case 'r':
					if( spec == 'o' ) {
						s->radix = 8;
					} else if( spec == 'x' ) {
						s->radix = 16;
					} else if( spec == 'r' ) {
						s->radix = va_arg( s->args, unsigned int );
					}

					switch( s->lenspec ) {
#define P_(F_) axpf__write_uint( s, x.F_[i] ); break
					case kAxLS_None: P_( u );
					case kAxLS_hh:   P_( usc );
					case kAxLS_h:    P_( usi );
					case kAxLS_l:    /*fallthrough*/
					case kAxLS_L:    P_( uli );
					case kAxLS_ll:   P_( ulli );
					case kAxLS_j:    P_( uim );
					case kAxLS_I:    /*fallthrough*/
					case kAxLS_z:    P_( sz );
					case kAxLS_t:    P_( pd );
					case kAxLS_I32:  P_( ui32 );
					case kAxLS_I64:  P_( ui64 );
#undef P_
					}
					break;

				case 'f':
				case 'e':
				case 'g':
					if( s->lenspec == kAxLS_None ) {
						axpf__write_floatd( s, ( double )x.f[ i ], spec );
					} else if( s->lenspec == kAxLS_l ) {
						axpf__write_floatd( s, x.d[ i ], spec );
					} else if( s->lenspec == kAxLS_L ) {
						axpf__write_floatd( s, ( double )x.ld[ i ], spec );
					}
					break;

				case 'c':
				case 'C':
					if( s->lenspec == kAxLS_None && spec != 'C' ) {
						axpf__write_char( s, x.c[ i ] );
					} else if( s->lenspec == kAxLS_l || spec == 'C' ) {
						axpf__write_wchar( s, x.wc[ i ] );
					}
					break;

				case 's':
				case 'S':
					if( s->lenspec == kAxLS_None && spec != 'S' ) {
						axpf__write_str( s, x.s[ i ] );
					} else if( s->lenspec == kAxLS_l || spec == 'S' ) {
						axpf__write_wstr( s, x.ws[ i ] );
					}
					break;

				case 'p':
					s->radix = 16;
					s->flags |= kAxPF_Radix | kAxPF_Pointer | kAxPF_Precision;
					s->precision = sizeof( void * )*2;
					axpf__write_uint( s, x.sz[ i ] );
					break;
				}
			}

			if( s->arrayprint[ 2 ] == ' ' ) {
				axpf__writech( s, ' ' );
			}

			axpf__writech( s, s->arrayprint[ 3 ] );
		}

		return !s->diderror;
	}

	/* check the specifier */
	switch( spec ) {
	case 'd':
	case 'i':
		if( s->lenspec == kAxLS_l ) {
			axpf__write_int( s, va_arg( s->args, long int ) );
		} else if( s->lenspec == kAxLS_ll || s->lenspec == kAxLS_L ) {
			axpf__write_int( s, va_arg( s->args, axpf_longlong_t ) );
		} else if( s->lenspec == kAxLS_t || s->lenspec == kAxLS_I ) {
			axpf__write_int( s, va_arg( s->args, axpf_ptrdiff_t ) );
		} else if( s->lenspec == kAxLS_z ) {
			axpf__write_int( s, va_arg( s->args, axpf_size_t ) );
		} else if( s->lenspec == kAxLS_j ) {
			axpf__write_int( s, va_arg( s->args, axpf_smax_t ) );
		} else if( s->lenspec == kAxLS_I32 ) {
			axpf__write_int( s, va_arg( s->args, axpf_s32_t ) );
		} else if( s->lenspec == kAxLS_I64 ) {
			axpf__write_int( s, va_arg( s->args, axpf_s64_t ) );
		} else {
			axpf__write_int( s, va_arg( s->args, int ) );
		}
		break;

	case 'u':
	case 'o':
	case 'x':
	case 'r':
		if( spec == 'o' ) {
			s->radix = 8;
		} else if( spec == 'x' ) {
			s->radix = 16;
		} else if( spec == 'r' ) {
			s->radix = va_arg( s->args, unsigned int );
		}

		if( s->lenspec == kAxLS_l ) {
			axpf__write_uint( s, va_arg( s->args, unsigned long int ) );
		} else if( s->lenspec == kAxLS_ll || s->lenspec == kAxLS_L ) {
			axpf__write_uint( s, va_arg( s->args, axpf_ulonglong_t ) );
		} else if( s->lenspec == kAxLS_t ) {
			axpf__write_uint( s, va_arg( s->args, axpf_ptrdiff_t ) );
		} else if( s->lenspec == kAxLS_z || s->lenspec == kAxLS_I ) {
			axpf__write_uint( s, va_arg( s->args, axpf_size_t ) );
		} else if( s->lenspec == kAxLS_j ) {
			axpf__write_uint( s, va_arg( s->args, axpf_umax_t ) );
		} else if( s->lenspec == kAxLS_I32 ) {
			axpf__write_uint( s, va_arg( s->args, axpf_u32_t ) );
		} else if( s->lenspec == kAxLS_I64 ) {
			axpf__write_uint( s, va_arg( s->args, axpf_u64_t ) );
		} else {
			axpf__write_uint( s, va_arg( s->args, unsigned int ) );
		}
		break;

	case 'f':
	case 'e':
	case 'g':
	case 'a':
		if( spec == 'a' ) {
			spec = 'f';
			s->radix = 16;
		}

		if( s->lenspec == kAxLS_None ) {
			axpf__write_floatd( s, va_arg( s->args, double ), spec );
		} else if( s->lenspec == kAxLS_l || s->lenspec == kAxLS_L ) {
			axpf__write_floatd( s, ( double )va_arg( s->args, long double ),
				spec );
		}
		break;

	case 'c':
	case 'C':
		if( s->lenspec == kAxLS_None && spec != 'C' ) {
			axpf__write_char( s, va_arg( s->args, int ) );
		} else if( s->lenspec == kAxLS_l || spec == 'C' ) {
			axpf__write_wchar( s, va_arg( s->args, int/*wint_t*/ ) );
		}
		break;

	case 's':
	case 'S':
		if( s->lenspec == kAxLS_None && spec != 'S' ) {
			axpf__write_str( s, va_arg( s->args, const char * ) );
		} else if( s->lenspec == kAxLS_l || spec == 'S' ) {
			axpf__write_wstr( s, va_arg( s->args, const wchar_t * ) );
		}
		break;

	case 'p':
		s->radix = 16;
		s->flags |= kAxPF_Radix | kAxPF_Pointer | kAxPF_Precision;
		s->precision = sizeof( void * )*2;
		axpf__write_uint( s, va_arg( s->args, axpf_size_t ) );
		break;

	case 'n':
		switch( s->lenspec ) {
#define P_(Ty_) *va_arg( s->args, Ty_ * ) = ( Ty_ )s->num_written; break
		case kAxLS_None: P_( int );
		case kAxLS_hh:   P_( signed char );
		case kAxLS_h:    P_( short int );
		case kAxLS_l:    /*fallthrough*/
		case kAxLS_L:    P_( long int );
#ifdef _MSC_VER
		case kAxLS_ll:   P_( __int64 );
#else
		case kAxLS_ll:   P_( long long int );
#endif
		case kAxLS_j:    P_( axpf_smax_t );
		case kAxLS_z:    P_( axpf_size_t );
		case kAxLS_I:    /*fallthrough*/
		case kAxLS_t:    P_( axpf_ptrdiff_t );
		case kAxLS_I32:  P_( axpf_s32_t );
		case kAxLS_I64:  P_( axpf_s64_t );
#undef P_
		}
		break;

	case 'm':
		axpf__write_syserr( s, s->width != 0 ? s->width : ( int )errno );
		break;

	case 'b':
		{
			int bits;
			const char *names;

			bits = va_arg( s->args, int );
			names = va_arg( s->args, const char * );

			axpf__write_bitfield( s, bits, names );
		}
		break;
	case 'D':
		{
			const unsigned char *data;
			const char *delm;

			data = va_arg( s->args, const unsigned char * );
			delm = va_arg( s->args, const char * );

			axpf__write_hex_dump( s, data, delm );
		}
		break;
	}

	return !s->diderror;
}

#endif /* AXPF_IMPLEMENT */

AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpf__main
(
	struct axpf__state_ *s,
	const char *fmt,
	const char *fmte,
	va_list args
)
#if AXPF_IMPLEMENT
{
# if AXPF__WINCON
	if( s->pfn_write == &axpf__write_fp_f ) {
		HANDLE h;
		DWORD dwMode;

		h = INVALID_HANDLE_VALUE;
		if( s->write_data == ( void * )stdout ) {
			h = GetStdHandle( STD_OUTPUT_HANDLE );
		} else if( s->write_data == ( void * )stderr ) {
			h = GetStdHandle( STD_ERROR_HANDLE );
		}

		if( h != INVALID_HANDLE_VALUE && GetConsoleMode( h, &dwMode ) ) {
			s->pfn_write  = &axpf__write_con_f;
			s->write_data = ( void * )h;
		}
	}
# endif
	s->s = fmt;
	s->p = fmt;
	s->e = !fmte ? strchr( fmt, '\0' ) : fmte;

	s->num_written = 0;
# if defined( va_copy )
#  define axpf__va_copy va_copy
# elif defined( __va_copy )
#  define axpf__va_copy __va_copy
# elif defined(__has_builtin) && __has_builtin(__builtin_va_copy)
#  define axpf__va_copy __builtin_va_copy
# else
#  error ax_printf: `va_copy` not defined.
# endif
	axpf__va_copy( s->args, args );
# undef axpf__va_copy

	s->diderror = 0;

	while( axpf__step( s ) ) {
	}

	if( s->diderror ) {
		return -1;
	}

	return ( axpf_ptrdiff_t )s->num_written;
}
#else
;
#endif

#if AXPF_STDFILE_ENABLED

AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axfpfev
(
	FILE *fp,
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	va_list args
)
AXPF_FUNC_ANNO(2,4)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )fp;

	return axpf__main( &s, fmt, fmte, args );
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axfpfv
(
	FILE *fp,
	AXPF_PARM_ANNO
	const char *fmt,
	va_list args
)
AXPF_FUNC_ANNO(2,3)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )fp;

	return axpf__main( &s, fmt, ( const char * )0, args );
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axfpfe
(
	FILE *fp,
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	...
)
AXPF_FUNC_ANNO(2,4)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;
	axpf_ptrdiff_t r;
	va_list args;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )fp;

	va_start( args, fmte );
	r = axpf__main( &s, fmt, fmte, args );
	va_end( args );

	return r;
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axfpf
(
	FILE *fp,
	AXPF_PARM_ANNO
	const char *fmt,
	...
)
AXPF_FUNC_ANNO(2,3)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;
	axpf_ptrdiff_t r;
	va_list args;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )fp;

	va_start( args, fmt );
	r = axpf__main( &s, fmt, ( const char * )0, args );
	va_end( args );

	return r;
}
#else
;
#endif

AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axerrfev
(
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	va_list args
)
AXPF_FUNC_ANNO(1,3)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )stderr;

	return axpf__main( &s, fmt, fmte, args );
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axerrfv
(
	AXPF_PARM_ANNO
	const char *fmt,
	va_list args
)
AXPF_FUNC_ANNO(1,2)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )stderr;

	return axpf__main( &s, fmt, ( const char * )0, args );
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axerrfe
(
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	...
)
AXPF_FUNC_ANNO(1,3)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;
	axpf_ptrdiff_t r;
	va_list args;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )stderr;

	va_start( args, fmte );
	r = axpf__main( &s, fmt, fmte, args );
	va_end( args );

	return r;
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axerrf
(
	AXPF_PARM_ANNO
	const char *fmt,
	...
)
AXPF_FUNC_ANNO(1,2)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;
	axpf_ptrdiff_t r;
	va_list args;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )stderr;

	va_start( args, fmt );
	r = axpf__main( &s, fmt, ( const char * )0, args );
	va_end( args );

	return r;
}
#else
;
#endif

AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpfev
(
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	va_list args
)
AXPF_FUNC_ANNO(1,3)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )stdout;

	return axpf__main( &s, fmt, fmte, args );
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpfv
(
	AXPF_PARM_ANNO
	const char *fmt,
	va_list args
)
AXPF_FUNC_ANNO(1,2)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )stdout;

	return axpf__main( &s, fmt, ( const char * )0, args );
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpfe
(
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	...
)
AXPF_FUNC_ANNO(1,3)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;
	axpf_ptrdiff_t r;
	va_list args;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )stdout;

	va_start( args, fmte );
	r = axpf__main( &s, fmt, fmte, args );
	va_end( args );

	return r;
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axpf
(
	AXPF_PARM_ANNO
	const char *fmt,
	...
)
AXPF_FUNC_ANNO(1,2)
#if AXPF_IMPLEMENT
{
	struct axpf__state_ s;
	axpf_ptrdiff_t r;
	va_list args;

	s.pfn_write = &axpf__write_fp_f;
	s.write_data = ( void * )stdout;

	va_start( args, fmt );
	r = axpf__main( &s, fmt, ( const char * )0, args );
	va_end( args );

	return r;
}
#else
;
#endif

#endif /*AXPF_STDFILE_ENABLED*/

AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axspfev
(
	char *buf,
	axpf_size_t nbuf,
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	va_list args
)
AXPF_FUNC_ANNO(3,5)
#if AXPF_IMPLEMENT
{
	struct axpf__write_mem_data_ m;
	struct axpf__state_ s;
	axpf_ptrdiff_t r;

	m.p = buf;
	m.i = 0;
	m.n = nbuf;

	s.pfn_write = &axpf__write_mem_f;
	s.write_data = ( void * )&m;

	r = axpf__main( &s, fmt, fmte, args );
	if( m.n > 0 ) {
		m.p[ m.i < m.n ? m.i : m.n - 1 ] = '\0';
	}

	return r;
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axspfv
(
	char *buf,
	axpf_size_t nbuf,
	AXPF_PARM_ANNO
	const char *fmt,
	va_list args
)
AXPF_FUNC_ANNO(3,4)
#if AXPF_IMPLEMENT
{
	return axspfev( buf, nbuf, fmt, ( const char * )0, args );
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axspfe
(
	char *buf,
	axpf_size_t nbuf,
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	...
)
AXPF_FUNC_ANNO(3,5)
#if AXPF_IMPLEMENT
{
	axpf_ptrdiff_t r;
	va_list args;

	va_start( args, fmte );
	r = axspfev( buf, nbuf, fmt, fmte, args );
	va_end( args );

	return r;
}
#else
;
#endif
AXPF_FUNC axpf_ptrdiff_t
AXPF_CALL axspf
(
	char *buf,
	axpf_size_t nbuf,
	AXPF_PARM_ANNO
	const char *fmt,
	...
)
AXPF_FUNC_ANNO(3,4)
#if AXPF_IMPLEMENT
{
	axpf_ptrdiff_t r;
	va_list args;

	va_start( args, fmt );
	r = axspfev( buf, nbuf, fmt, ( const char * )0, args );
	va_end( args );

	return r;
}
#else
;
#endif

AXPF_FUNC const char *
AXPF_CALL axffev
(
	char *buf,
	axpf_size_t nbuf,
	AXPF_PARM_ANNO
	const char *fmts,
	const char *fmte,
	va_list args
)
AXPF_FUNC_ANNO(3,5)
#if AXPF_IMPLEMENT
{
	( void )axspfev( buf, nbuf, fmts, fmte, args );
	return buf;
}
#else
;
#endif
AXPF_FUNC const char *
AXPF_CALL axffe
(
	char *buf,
	axpf_size_t nbuf,
	AXPF_PARM_ANNO
	const char *fmts,
	const char *fmte,
	...
)
AXPF_FUNC_ANNO(3,5)
#if AXPF_IMPLEMENT
{
	va_list args;

	va_start( args, fmte );
	( void )axspfev( buf, nbuf, fmts, fmte, args );
	va_end( args );

	return buf;
}
#else
;
#endif
AXPF_FUNC const char *
AXPF_CALL axffv
(
	char *buf,
	axpf_size_t nbuf,
	AXPF_PARM_ANNO
	const char *fmt,
	va_list args
)
AXPF_FUNC_ANNO(3,4)
#if AXPF_IMPLEMENT
{
	( void )axspfev( buf, nbuf, fmt, ( const char * )0, args );
	return buf;
}
#else
;
#endif
AXPF_FUNC const char *
AXPF_CALL axff
(
	char *buf,
	axpf_size_t nbuf,
	AXPF_PARM_ANNO
	const char *fmt,
	...
)
AXPF_FUNC_ANNO(3,4)
#if AXPF_IMPLEMENT
{
	va_list args;

	va_start( args, fmt );
	( void )axspfev( buf, nbuf, fmt, ( const char * )0, args );
	va_end( args );

	return buf;
}
#else
;
#endif

AXPF_FUNC char *
AXPF_CALL axdupfev
(
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	va_list args
)
AXPF_FUNC_ANNO(1,3)
#if AXPF_IMPLEMENT
{
	struct axpf__write_mem_data_ m;
	struct axpf__state_ s;
	axpf_ptrdiff_t r;

	m.p = ( char * )0;
	m.i = 0;
	m.n = 0;

	s.pfn_write = &axpf__write_memdup_f;
	s.write_data = ( void * )&m;

	r = axpf__main( &s, fmt, fmte, args );
	if( r < 0 ) {
		axpf_free( ( void * )m.p );
		return ( char * )0;
	}

	return m.p;
}
#else
;
#endif
AXPF_FUNC char *
AXPF_CALL axdupfe
(
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	...
)
AXPF_FUNC_ANNO(1,3)
#if AXPF_IMPLEMENT
{
	va_list args;
	char *p;

	va_start( args, fmte );
	p = axdupfev( fmt, fmte, args );
	va_end( args );

	return p;
}
#else
;
#endif
AXPF_FUNC char *
AXPF_CALL axdupfv
(
	AXPF_PARM_ANNO
	const char *fmt,
	va_list args
)
AXPF_FUNC_ANNO(1,2)
#if AXPF_IMPLEMENT
{
	return axdupfev( fmt, ( const char * )0, args );
}
#else
;
#endif
AXPF_FUNC char *
AXPF_CALL axdupf
(
	AXPF_PARM_ANNO
	const char *fmt,
	...
)
AXPF_FUNC_ANNO(1,2)
#if AXPF_IMPLEMENT
{
	va_list args;
	char *p;

	va_start( args, fmt );
	p = axdupfev( fmt, ( const char * )0, args );
	va_end( args );

	return p;
}
#else
;
#endif

AXPF_FUNC const char *
AXPF_CALL axfev
(
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	va_list args
)
AXPF_FUNC_ANNO(1,3)
#if AXPF_IMPLEMENT
{
	static axpf_size_t bufi = 0;
	static char buf[ 0x10000 ];

	struct axpf__write_mem_data_ m;
	struct axpf__state_ s;
	axpf_ptrdiff_t r;

	m.p = &buf[ bufi ];
	m.i = 0;
	m.n = sizeof( buf ) - bufi;

	s.pfn_write = &axpf__write_mem_f;
	s.write_data = ( void * )&m;

	r = axpf__main( &s, fmt, fmte, args );
	if( r < 0 ) {
		return ( char * )0;
	}

	bufi += ( axpf_size_t )r;
	if( bufi + 0x200 >= sizeof( buf ) ) {
		bufi = 0;
	}

	return m.p;
}
#else
;
#endif
AXPF_FUNC const char *
AXPF_CALL axfe
(
	AXPF_PARM_ANNO
	const char *fmt,
	const char *fmte,
	...
)
AXPF_FUNC_ANNO(1,3)
#if AXPF_IMPLEMENT
{
	va_list args;
	const char *p;

	va_start( args, fmte );
	p = axfev( fmt, fmte, args );
	va_end( args );

	return p;
}
#else
;
#endif
AXPF_FUNC const char *
AXPF_CALL axfv
(
	AXPF_PARM_ANNO
	const char *fmt,
	va_list args
)
AXPF_FUNC_ANNO(1,2)
#if AXPF_IMPLEMENT
{
	return axfev( fmt, ( const char * )0, args );
}
#else
;
#endif
AXPF_FUNC const char *
AXPF_CALL axf
(
	AXPF_PARM_ANNO
	const char *fmt,
	...
)
AXPF_FUNC_ANNO(1,2)
#if AXPF_IMPLEMENT
{
	const char *p;
	va_list args;

	va_start( args, fmt );
	p = axfev( fmt, ( const char * )0, args );
	va_end( args );

	return p;
}
#else
;
#endif

#ifdef __cplusplus
}
#endif

#if AXPF_CXX_OVERLOADS_ENABLED
template< axpf_size_t tMaxBuf >
inline axpf_ptrdiff_t axspfev( char( &buf )[ tMaxBuf ], AXPF_PARM_ANNO const char *fmt,
const char *fmte, va_list args ) AXPF_FUNC_ANNO(2,4)
{
	return axspfev( buf, tMaxBuf, fmt, fmte, args );
}
template< axpf_size_t tMaxBuf >
inline axpf_ptrdiff_t axspfv( char( &buf )[ tMaxBuf ], AXPF_PARM_ANNO const char *fmt,
va_list args ) AXPF_FUNC_ANNO(2,3)
{
	return axspfev( buf, tMaxBuf, fmt, ( const char * )0, args );
}
template< axpf_size_t tMaxBuf >
inline axpf_ptrdiff_t axspfe( char( &buf )[ tMaxBuf ], AXPF_PARM_ANNO const char *fmt,
const char *fmte, ... ) AXPF_FUNC_ANNO(2,4)
{
	axpf_ptrdiff_t r;
	va_list args;

	va_start( args, fmte );
	r = axspfev( buf, tMaxBuf, fmt, fmte, args );
	va_end( args );

	return r;
}
template< axpf_size_t tMaxBuf >
inline axpf_ptrdiff_t axspf( char( &buf )[ tMaxBuf ], AXPF_PARM_ANNO const char *fmt, ... )
AXPF_FUNC_ANNO(2,3)
{
	axpf_ptrdiff_t r;
	va_list args;

	va_start( args, fmt );
	r = axspfev( buf, tMaxBuf, fmt, ( const char * )0, args );
	va_end( args );

	return r;
}

template< axpf_size_t tMaxBuf >
inline const char *axffev( char( &buf )[ tMaxBuf ], AXPF_PARM_ANNO const char *fmts, const char *fmte, va_list args ) AXPF_FUNC_ANNO(2,4)
{
	( void )axspfev( buf, tMaxBuf, fmts, fmte, args );
	return buf;
}
template< axpf_size_t tMaxBuf >
inline const char *axffe( char( &buf )[ tMaxBuf ], AXPF_PARM_ANNO const char *fmts, const char *fmte, ... ) AXPF_FUNC_ANNO(2,4)
{
	va_list args;

	va_start( args, fmte );
	( void )axspfev( buf, tMaxBuf, fmts, fmte, args );
	va_end( args );

	return buf;
}
template< axpf_size_t tMaxBuf >
inline const char *axffv( char( &buf )[ tMaxBuf ], AXPF_PARM_ANNO const char *fmt, va_list args ) AXPF_FUNC_ANNO(2,3)
{
	( void )axspfev( buf, tMaxBuf, fmt, ( const char * )0, args );
	return buf;
}
template< axpf_size_t tMaxBuf >
inline const char *axff( char( &buf )[ tMaxBuf ], AXPF_PARM_ANNO const char *fmt, ... ) AXPF_FUNC_ANNO(2,3)
{
	va_list args;

	va_start( args, fmt );
	( void )axspfev( buf, tMaxBuf, fmt, ( const char * )0, args );
	va_end( args );

	return buf;
}
#endif /*AXPF_CXX_OVERLOADS_ENABLED*/

#endif
