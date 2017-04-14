/*

	ax_time - public domain
	Last update: 2015-10-01 Aaron Miller


	This library provides high-resolution timing functionality. Unlike some
	other libraries, we do not use lossy floating-point operations to calculate
	the current time (until a float is explicitly requested).


	USAGE
	=====

	Define AXTIME_IMPLEMENTATION in exactly one source file that includes this
	header, before including it.

	The following don't need to be defined, as default definitions will be
	provided, but can be defined if you want to alter default functionality
	without modifying this file.

	AXTIME_FUNC and AXTIME_CALL control the function declaration and calling
	conventions. Ensure that all source files including this use the same
	definitions for these. (That can be done in your Makefile or project
	settings.)

	AXTIME_CONVFUNC and AXTIME_CONVCALL are the same as AXTIME_FUNC and
	AXTIME_CALL, but for the time conversion functions here. These functions
	qualify as constexpr in C++11.

	AXTIME_WRAPFUNC and AXTIME_WRAPCALL are the same as AXTIME_FUNC and
	AXTIME_CALL, but for the C++ wrapper functions for the time querying
	functions. The default is to attempt to force inline them.

	AXTIME_TYPES_DEFINED should be set to 1 if you provide typedefs for
	axtm_u64_t, axtm_s64_t, axtm_u32_t, and axtm_s32_t.

	AXTIME_FPTYPE_DEFINED should be set to 1 if you provide a typedef for
	axtm_fp_t. By default this represents "double."

	AX_TARGET_FPS defines the target frame rate. This is for the frame-specific
	timing functions.


	CONFIGURATION MACROS
	====================

	Define any of these to either 1 or 0 prior to including this header, if you
	want to alter the default functionality.

		AXTIME_CXX_ENABLED
		------------------
		Enables the C++ parts of this. All C++ code is in the ax namespace. If
		C++ is enabled then the pointer size is guaranteed to be accurate.


		AXTIME_CONVFUNC_IMPL
		--------------------
		Only set this if *you* custom defined AXTIME_CONVFUNC.


	INTERACTIONS
	============

	This library will use ax_platform definitions if they are available. To use
	them include ax_platform.h before including this header.

	This library will use ax_types if it has been included prior to this header.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_TIME_H_
#define INCGUARD_AX_TIME_H_

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

#ifdef AXTIME_IMPLEMENTATION
# define AXTIME_IMPLEMENT           1
#else
# define AXTIME_IMPLEMENT           0
#endif

#ifndef AXTIME_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXTIME_CXX_ENABLED        AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXTIME_CXX_ENABLED        1
# else
#  define AXTIME_CXX_ENABLED        0
# endif
#endif

#ifndef AXTIME_EXTRNC_ENTER
# ifdef __cplusplus
#  define AXTIME_EXTRNC_ENTER       extern "C" {
#  define AXTIME_EXTRNC_LEAVE       }
# else
#  define AXTIME_EXTRNC_ENTER
#  define AXTIME_EXTRNC_LEAVE
# endif
#endif

#ifndef AXTIME_FUNC
# ifdef AX_FUNC
#  define AXTIME_FUNC               AX_FUNC
# else
#  define AXTIME_FUNC               extern
# endif
#endif
#ifndef AXTIME_CALL
# ifdef AX_CALL
#  define AXTIME_CALL               AX_CALL
# else
#  define AXTIME_CALL
# endif
#endif

#ifndef AXTIME_CONVFUNC
# if defined( AX_CONSTEXPR_INLINE ) && AX_HAS_CONSTEXPR_INLINE
#  define AXTIME_CONVFUNC           AX_CONSTEXPR_INLINE
#  define AXTIME_CONVFUNC_IMPL      1
#  ifndef AXTIME_CONVCALL
#   define AXTIME_CONVCALL
#  endif
# elif AXTIME_CXX_ENABLED
#  define AXTIME_CONVFUNC           inline
#  define AXTIME_CONVFUNC_IMPL      1
# else
#  define AXTIME_CONVFUNC           AXTIME_FUNC
#  define AXTIME_CONVFUNC_IMPL      AXTIME_IMPLEMENT
# endif
#endif

#ifndef AXTIME_CONVCALL
# define AXTIME_CONVCALL            AXTIME_CALL
#endif

#ifndef AXTIME_WRAPFUNC
# ifdef AX_FORCEINLINE
#  define AXTIME_WRAPFUNC           AX_FORCEINLINE
# elif AXTIME_CXX_ENABLED
#  define AXTIME_WRAPFUNC           inline
# else
#  define AXTIME_WRAPFUNC           AXTIME_FUNC
# endif
#endif

#ifndef AXTIME_WRAPCALL
# define AXTIME_WRAPCALL            AXTIME_CALL
#endif

#ifndef AXTIME_TYPES_DEFINED
# define AXTIME_TYPES_DEFINED       1
# if AX_TYPES_DEFINED
typedef ax_u64_t                    axtm_u64_t;
typedef ax_s64_t                    axtm_s64_t;
typedef ax_u32_t                    axtm_u32_t;
typedef ax_s32_t                    axtm_s32_t;
# elif defined( _MSC_VER )
typedef unsigned __int64            axtm_u64_t;
typedef signed __int64              axtm_s64_t;
typedef unsigned __int32            axtm_u32_t;
typedef signed __int32              axtm_s32_t;
# else
#  include <stdint.h>
typedef uint64_t                    axtm_u64_t;
typedef int64_t                     axtm_s64_t;
typedef uint32_t                    axtm_u32_t;
typedef int32_t                     axtm_s32_t;
# endif
#endif
#ifdef ax_static_assert
ax_static_assert( sizeof( axtm_s64_t ) == 8, "ax_time: size mismatch" );
ax_static_assert( sizeof( axtm_u64_t ) == 8, "ax_time: size mismatch" );
ax_static_assert( sizeof( axtm_s32_t ) == 4, "ax_time: size mismatch" );
ax_static_assert( sizeof( axtm_u32_t ) == 4, "ax_time: size mismatch" );
#endif
#ifndef AXTIME_FPTYPE_DEFINED
# define AXTIME_FPTYPE_DEFINED      1
typedef double                      axtm_fp_t;
#endif

#ifndef AX_TARGET_FPS
# define AX_TARGET_FPS              60
#endif

#define AXTIME_NANOSECS             1000000000
#define AXTIME_MICROSECS            1000000
#define AXTIME_MILLISECS            1000


/*
===============================================================================

	TIMING CONVERSION

===============================================================================
*/

AXTIME_EXTRNC_ENTER

AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL axtm_sec2mic( axtm_fp_t seconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( axtm_u64_t )( seconds*1000000.0 );
}
#else
;
#endif
AXTIME_CONVFUNC axtm_u32_t AXTIME_CONVCALL axtm_sec2mil( axtm_fp_t seconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( axtm_u32_t )( seconds*1000.0 );
}
#else
;
#endif

AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL axtm_mic2sec( axtm_u64_t microseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( ( axtm_fp_t )( microseconds ) )/1000000.0;
}
#else
;
#endif
AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL axtm_mil2sec( axtm_u32_t milliseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( ( axtm_fp_t )( milliseconds ) )/1000.0;
}
#else
;
#endif

AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL axtm_mil2mic( axtm_u32_t milliseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( ( axtm_u64_t )( milliseconds ) )*1000;
}
#else
;
#endif
AXTIME_CONVFUNC axtm_u32_t AXTIME_CONVCALL axtm_mic2mil( axtm_u64_t microseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( axtm_u32_t )( microseconds/1000 );
}
#else
;
#endif

AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL axtm_frame2sec( axtm_fp_t targetFPS )
#if AXTIME_CONVFUNC_IMPL
{
	return 1.0/targetFPS;
}
#else
;
#endif
AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL axtm_sec2frame( axtm_fp_t seconds, axtm_fp_t targetFPS )
#if AXTIME_CONVFUNC_IMPL
{
	return seconds/axtm_frame2sec( targetFPS );
}
#else
;
#endif
AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL axtm_mil2frame( axtm_u32_t milliseconds, axtm_fp_t targetFPS )
#if AXTIME_CONVFUNC_IMPL
{
	return axtm_sec2frame( axtm_mil2sec( milliseconds ), targetFPS );
}
#else
;
#endif
AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL axtm_mic2frame( axtm_u64_t microseconds, axtm_fp_t targetFPS )
#if AXTIME_CONVFUNC_IMPL
{
	return axtm_sec2frame( axtm_mic2sec( microseconds ), targetFPS );
}
#else
;
#endif

AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL axtm_mic2nan( axtm_u64_t microseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return microseconds*1000;
}
#else
;
#endif
AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL axtm_mil2nan( axtm_u32_t milliseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( ( axtm_u64_t )milliseconds )*1000000;
}
#else
;
#endif
AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL axtm_sec2nan( axtm_fp_t seconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( axtm_u64_t )( seconds*1000000000.0 );
}
#else
;
#endif
AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL axtm_nan2mic( axtm_u64_t nanoseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return nanoseconds/1000;
}
#else
;
#endif
AXTIME_CONVFUNC axtm_u32_t AXTIME_CONVCALL axtm_nan2mil( axtm_u64_t nanoseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( axtm_u32_t )( nanoseconds/1000000 );
}
#else
;
#endif
AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL axtm_nan2sec( axtm_u64_t nanoseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return ( axtm_fp_t )( nanoseconds )/1000000000.0;
}
#else
;
#endif

AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL axtm_mic2hnan( axtm_u64_t microseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return microseconds*10;
}
#else
;
#endif
AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL axtm_hnan2mic( axtm_u64_t hundrednanoseconds )
#if AXTIME_CONVFUNC_IMPL
{
	return hundrednanoseconds/10;
}
#else
;
#endif

AXTIME_EXTRNC_LEAVE

#if AXTIME_CXX_ENABLED
namespace ax
{

	AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL secondsToMicroseconds( axtm_fp_t seconds )
	{
		return axtm_u64_t( seconds*1000000.0 );
	}
	AXTIME_CONVFUNC axtm_u32_t AXTIME_CONVCALL secondsToMilliseconds( axtm_fp_t seconds )
	{
		return axtm_u32_t( seconds*1000.0 );
	}

	AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL microsecondsToSeconds( axtm_u64_t microseconds )
	{
		return axtm_fp_t( microseconds )/1000000.0;
	}
	AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL millisecondsToSeconds( axtm_u32_t milliseconds )
	{
		return axtm_fp_t( milliseconds )/1000.0;
	}

	AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL millisecondsToMicroseconds( axtm_u32_t milliseconds )
	{
		return axtm_u64_t( milliseconds )*1000;
	}
	AXTIME_CONVFUNC axtm_u32_t AXTIME_CONVCALL microsecondsToMilliseconds( axtm_u64_t microseconds )
	{
		return axtm_u32_t( microseconds/1000 );
	}

	AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL secondsPerFrame( axtm_fp_t targetFPS = AX_TARGET_FPS )
	{
		return 1.0/targetFPS;
	}
	AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL secondsToFrames( axtm_fp_t seconds, axtm_fp_t targetFPS = AX_TARGET_FPS )
	{
		return seconds/secondsPerFrame( targetFPS );
	}
	AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL millisecondsToFrames( axtm_u32_t milliseconds, axtm_fp_t targetFPS = AX_TARGET_FPS )
	{
		return secondsToFrames( millisecondsToSeconds( milliseconds ), targetFPS );
	}
	AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL microsecondsToFrames( axtm_u64_t microseconds, axtm_fp_t targetFPS = AX_TARGET_FPS )
	{
		return secondsToFrames( microsecondsToSeconds( microseconds ), targetFPS );
	}

	AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL microsecondsToNanoseconds( axtm_u64_t microseconds )
	{
		return microseconds*1000;
	}
	AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL millisecondsToNanoseconds( axtm_u32_t milliseconds )
	{
		return axtm_u64_t( milliseconds )*1000000;
	}
	AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL secondsToNanoseconds( axtm_fp_t seconds )
	{
		return axtm_u64_t( seconds*1000000000.0 );
	}
	AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL nanosecondsToMicroseconds( axtm_u64_t nanoseconds )
	{
		return nanoseconds/1000;
	}
	AXTIME_CONVFUNC axtm_u32_t AXTIME_CONVCALL nanosecondsToMilliseconds( axtm_u64_t nanoseconds )
	{
		return axtm_u32_t( nanoseconds/1000000 );
	}
	AXTIME_CONVFUNC axtm_fp_t AXTIME_CONVCALL nanosecondsToSeconds( axtm_u64_t nanoseconds )
	{
		return axtm_fp_t( nanoseconds )/1000000000.0;
	}

	AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL microsecondsToHundredNanoseconds( axtm_u64_t microseconds )
	{
		return microseconds*10;
	}
	AXTIME_CONVFUNC axtm_u64_t AXTIME_CONVCALL hundredNanosecondsToMicroseconds( axtm_u64_t hundrednanoseconds )
	{
		return hundrednanoseconds/10;
	}

}
#endif /*AXTIME_CXX_ENABLED*/


/*
===============================================================================

	TIME QUERIES

===============================================================================
*/

#if AXTIME_IMPLEMENT

# define axtm__convres(T_,F_,R_)\
	(((axtm_u64_t)(T_))*((axtm_u64_t)(R_))/((axtm_u64_t)(F_)))

# ifndef AXTIME_PLATFORM_DEFINED
#  define AXTIME_PLATFORM_WINDOWS   0
#  define AXTIME_PLATFORM_MACH      0
#  define AXTIME_PLATFORM_POSIX     0
#  ifdef _WIN32
#   undef AXTIME_PLATFORM_WINDOWS
#   define AXTIME_PLATFORM_WINDOWS  1
#  elif defined( __APPLE__ )
#   undef AXTIME_PLATFORM_MACH
#   define AXTIME_PLATFORM_MACH     1
#  else
#   undef AXTIME_PLATFORM_POSIX
#   define AXTIME_PLATFORM_POSIX    1
#  endif
#  define AXTIME_PLATFORM_DEFINED   1
# endif

# if AXTIME_PLATFORM_WINDOWS
#  undef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN       1
#  if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0600
#   undef _WIN32_WINNT
#   define _WIN32_WINNT             0x0603
#  endif
#  include <Windows.h>
#  undef min
#  undef max
# elif AXTIME_PLATFORM_MACH
#  include <time.h>
#  include <mach/mach.h>
#  include <mach/mach_time.h>
# elif AXTIME_PLATFORM_POSIX
#  include <time.h>
#  include <sys/time.h>
# endif

/* Convert a time (t) from the system frequency to the target frequency (tf) */
static axtm_u64_t AXTIME_CALL axtm__convfreq( axtm_u64_t t, axtm_u64_t tf )
{
# if AXTIME_PLATFORM_WINDOWS
	static LARGE_INTEGER f;
	static int didinit = 0;

	if( !didinit ) {
		if( !QueryPerformanceFrequency( &f ) ) {
			return 0;
		}
		didinit = 1;
	}

	return axtm__convres( t, f.QuadPart, tf );
# elif AXTIME_PLATFORM_MACH
	static axtm_u64_t rate = 0;

	if( __builtin_expect( rate == 0, 0 ) ) {
		static mach_timebase_info_data_t info;
		if( mach_timebase_info( &info ) != KERN_SUCCESS ) {
			return 0;
		}
		rate = ((axtm_u64_t)1000000000)*info.numer/info.denom;
	}

	return axtm__convres( t, rate, tf );
# elif AXTIME_PLATFORM_POSIX
	return axtm__convres( t, AXTIME_NANOSECS, tf );
# else
#  error axtm__freq() does not support this platform
# endif
}

# if AXTIME_PLATFORM_POSIX
#  ifdef CLOCK_MONOTONIC_PRECISE
#   define AXTIME_POSIX_CLOCK       CLOCK_MONOTONIC_PRECISE
#  elif defined( CLOCK_MONOTONIC_RAW )
#   define AXTIME_POSIX_CLOCK       CLOCK_MONOTONIC_RAW
#  elif defined( CLOCK_HIGHRES )
#   define AXTIME_POSIX_CLOCK       CLOCK_HIGHRES
#  elif defined( CLOCK_MONOTONIC )
#   define AXTIME_POSIX_CLOCK       CLOCK_MONOTONIC
#  elif defined( CLOCK_REALTIME )
#   define AXTIME_POSIX_CLOCK       CLOCK_REALTIME
#  else
#   define AXTIME_POSIX_CLOCK       (-1)
#  endif
# endif /*AXTIME_PLATFORM_POSIX*/

static axtm_u64_t axtm__read( void )
{
# if AXTIME_PLATFORM_WINDOWS
	LARGE_INTEGER t;

	if( !QueryPerformanceCounter( &t ) ) {
		return 0;
	}

	return t.QuadPart;
# elif AXTIME_PLATFORM_MACH
	static axtm_u64_t base = 0;
#  if defined(__GNUC__) || defined(__clang__)
	if( __builtin_expect( base == 0, 0 ) ) {
		base = mach_absolute_time();
	}
#  else
	if( !base ) {
		base = mach_absolute_time();
	}
#  endif
	return mach_absolute_time() - base;
# elif AXTIME_PLATFORM_POSIX
	struct timespec t;

#  if AXTIME_POSIX_CLOCK != -1
	if( clock_gettime( AXTIME_POSIX_CLOCK, &t ) == 0 )
	{
		return (((axtm_u64_t)t.tv_sec)*AXTIME_NANOSECS) + (axtm_u64_t)t.tv_nsec;
	}
	else
#  endif /*AXTIME_POSIX_CLOCK != -1*/
	{
		struct timeval tv;

		if( gettimeofday( &tv, NULL ) == -1 ) {
			return 0;
		}

		return (((axtm_u64_t)tv.tv_sec)*AXTIME_NANOSECS) +
			(axtm_u64_t)tv.tv_usec*1000;
	}
# endif /*AXTIME_PLATFORM...*/
}

#endif /*AXTIME_IMPLEMENT*/

AXTIME_EXTRNC_ENTER

/* Retrieve the number of nanoseconds that have elapsed */
AXTIME_FUNC int AXTIME_CALL axtm_nanoseconds( int *psec, int *pnanosec )
#if AXTIME_IMPLEMENT
{
	axtm_u64_t t;
	int dummy;

	if( !psec ) { psec = &dummy; }
	if( !pnanosec ) { pnanosec = &dummy; }

	t = axtm__read();
	if( !t ) {
		*psec = 0;
		*pnanosec = 0;

		return 0;
	}

	*psec = (int)axtm__convfreq( t, 1 );
	*pnanosec = (int)axtm__convfreq( t, AXTIME_NANOSECS )%AXTIME_NANOSECS;

	return 1;
}
#else
;
#endif

/* Retrieve the number of microseconds that have elapsed */
AXTIME_FUNC axtm_u64_t AXTIME_CALL axtm_microseconds( void )
#if AXTIME_IMPLEMENT
{
	return axtm__convfreq( axtm__read(), AXTIME_MICROSECS );
}
#else
;
#endif

/* Retrieve the number of seconds that have elapsed */
AXTIME_FUNC axtm_fp_t AXTIME_CALL axtm_seconds( void )
#if AXTIME_IMPLEMENT
{
	static axtm_u64_t base = 0;

	if( !base ) {
		base = axtm_microseconds();
	}

	return ( ( axtm_fp_t )( axtm_microseconds() - base ) )/AXTIME_MICROSECS;
}
#else
;
#endif

/* Retrieve the number of milliseconds that have elapsed (low latency ver.) */
AXTIME_FUNC axtm_u64_t AXTIME_CALL axtm_milliseconds_lowlatency( void )
#if AXTIME_IMPLEMENT
{
# if AXTIME_PLATFORM_WINDOWS
	static axtm_u64_t base = 0;

	if( !base ) {
		base = GetTickCount64();
	}

	return GetTickCount64() - base;
# else
	return ( axtm_u64_t )clock();
# endif
}
#else
;
#endif

AXTIME_EXTRNC_LEAVE

#if AXTIME_CXX_ENABLED
namespace ax
{

	AXTIME_WRAPFUNC bool AXTIME_WRAPCALL nanoseconds( int &seconds, int &nanoseconds )
	{
		return !!axtm_nanoseconds( &seconds, &nanoseconds );
	}
	AXTIME_WRAPFUNC axtm_u64_t AXTIME_WRAPCALL microseconds()
	{
		return axtm_microseconds();
	}
	AXTIME_WRAPFUNC axtm_fp_t AXTIME_WRAPCALL seconds()
	{
		return axtm_seconds();
	}
	AXTIME_WRAPFUNC axtm_u64_t AXTIME_WRAPCALL milliseconds_lowLatency()
	{
		return axtm_milliseconds_lowlatency();
	}

}
#endif /*AXTIME_CXX_ENABLED*/


/*
===============================================================================

	C++ TIMER CLASS

===============================================================================
*/
#if AXTIME_CXX_ENABLED
namespace ax
{

	// Basic timing information
	class CTimer
	{
	public:
		typedef axtm_u64_t  uint64;
		typedef axtm_fp_t   real;

							// Constructor
		inline              CTimer                      ()
							: m_uBase( axtm_microseconds() )
							, m_uFreq( 1 )
							{
							}
							// Destructor
		inline              ~CTimer                     ()
							{
							}

							// reset the timer to zero
		inline void         reset                       ()
							{
								m_uBase = axtm_microseconds();
							}
							// reset the timer to the nearest frequency boundary
		inline void         coalescedReset              ()
							{
								const uint64 t = axtm_microseconds();
								m_uBase = t - t%m_uFreq;
							}

							// Set the frequency in seconds
		inline void         setFrequencySeconds         ( real sec )
							{
								m_uFreq = uint64( sec*1000000.0 );
							}
							// Retrieve the frequency in seconds
		inline real         getFrequencySeconds         () const
							{
								return real( m_uFreq )/1000000.0;
							}
							// Set the frequency in hertz
		inline void         setFrequencyHertz           ( real hz )
							{
								m_uFreq = uint64( 1000000.0/hz );
							}
							// Retrieve the frequency in hertz
		inline real         getFrequencyHertz           () const
							{
								return 1000000.0/real( m_uFreq );
							}
							// Set the frequency in microseconds
		inline void         setFrequency                ( uint64 microseconds )
							{
								m_uFreq = microseconds > 0 ? microseconds : 1;
							}
							// Retrieve the frequency in microseconds
		inline uint64       getFrequency                () const
							{
								return m_uFreq;
							}

							// Retrieve the total elapsed time in seconds since
							// construction or the last reset().
		inline real         getElapsedSeconds           () const
							{
								return real( getElapsed() )/1000000.0;
							}
							// Retrieve the total elapsed time in microseconds
		inline uint64       getElapsed                  () const
							{
								return axtm_microseconds() - m_uBase;
							}
							// Retrieve the total number of triggers since the
							// last reset
		inline uint64       getCount                    () const
							{
								return getElapsed()/m_uFreq;
							}

							// Retrieve the current progress
		inline real         getProgress                 () const
							{
								return real( getElapsed() )/real( m_uFreq );
							}

	protected:
							// Base from which the timer counts (microseconds)
		uint64              m_uBase;
							// Amount of time per period (microseconds)
		uint64              m_uFreq;
	};

}
#endif /*AXTIME_CXX_ENABLED*/

#endif
