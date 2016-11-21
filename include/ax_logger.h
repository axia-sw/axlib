/*

	ax_logger - public domain
	Last update: 2015-10-01 Aaron Miller


	Manages logging of application events.


	USAGE
	=====

	Define AXLOG_IMPLEMENTATION in exactly one source file that includes this
	header, before including it.


	CONFIGURATION
	=============

	AXLOG_FUNC and AXLOG_CALL control the function declaration and calling
	conventions. Ensure that all source files including this use the same
	definitions for these. (That can be done in your Makefile or project
	settings.)

	AXLOG_DEFAULT_FACILITY can optionally be defined prior to including this to
	set the default facility used in logging messages, where possible.

		Default: 0

	AXLOG_DEFAULT_FLAGS can optionally be defined prior to including this to set
	the default logging flags (not counting the facility) to be used when
	logging messages, where possible.

		Default: 0

	AXLOG_CXX_ENABLED controls whether any ax_logger C++ feature will be used.
	If this is set to 0 (or __cplusplus is not defined) then all AXLOG_CXX_*
	macros will be redefined to 0 and no C++ feature will be used. See the
	following AXLOG_CXX_* macros for more information.

		Default: 1 (unless __cplusplus isn't defined)

	AXLOG_CXX_OVERLOADS_ENABLED will enable or disable additional C++ overload
	functions that make life easier.

		Default: 1 (will be 0 if AXLOG_CXX_ENABLED is 0)

	AXLOG_CXX_STRUCT_METHODS_ENABLED will enable or disable additional C++
	methods added to the plain C structures.

		Default: 1 (will be 0 if AXLOG_CXX_ENABLED is 0)

	AXLOG_CXX_CLASSES_ENABLED, if set to 1, will define various helper classes.
	All C++ procedures will use `inline` as they are very lightweight and mostly
	only reference the C functions defined here.

		Default: 1 (will be 0 if AXLOG_CXX_ENABLED is 0)

	AXLOG_NO_PF can be defined to 1 to disable use of ax_printf. Naturally, it's
	preferred that ax_printf be used, but it has some minor differences from
	standard printf which may be undesired in some instances.

		Default: 0

	AXLOG_PRINTOUTF can be defined to the name of a function that will do a
	formatted text print to "standard output." Unless AXLOG_NO_PF is 1, this
	defaults to `axpf`.

	AXLOG_PRINTERRF is the same as AXLOG_PRINTOUTF, but sends the formatted text
	to "standard error." Unless AXLOG_NO_PF is 1, this defaults to `axerrf`.

	AXLOG_SNPRINTF is the name of the function to be used in place of snprintf.
	Unless AXLOG_NO_PF is 1, this defaults to `axspf`.

	AXLOG_SNPRINTFV is the name of the function to be used in place of
	vsnprintf. Unless AXLOG_NO_PF is 1, this defaults to `axspfv`.

	AXLOG_CUSTOM_VARARGS can be defined to 1 to indicate that <stdarg.h> should
	not be included and to indicate that the following are all defined:

		AXLOG_VA_T: `va_list` by default.
		AXLOG_VA_S: `va_start` by default.
		AXLOG_VA_E: `va_end` by default.

	AXLOG_VS_STYLE can be defined to 1 to indicate that the default logging
	function should format messages in a mostly Visual Studio compatible way.
	For example: "myfile.ext(13): error: Some issue." If defined to 0 then that
	would instead be: "myfile.ext:13: error: Some issue." This defaults to 1
	when building on Windows, and 0 otherwise. It should only be defined when
	AX_IMPLEMENTATION is defined.


	LOG FLAGS FORMAT
	================

	The logging flags integer is designed to fit within 16-bits while giving you
	6-bits (64 distinct values) to represent a "facility" for the report. (The
	"facility" being "where this report came from." e.g., "OpenGL API driver" or
	"input manager.")

	ax_logger's built-in flags encoding occupy the upper 10-bits of this 16-bit
	integer. This makes it easy for you to encode the facility by simply 'or'ing
	an index between 0 and 63 with the flags.

		```c
		#define FC_GENERAL  0
		#define FC_RENDERER 1
		#define FC_ENTITY   2

		// ...

		axlogf( axlogp_error | FC_RENDERER, "GL error: %s", glerrstr() );

		// ...
		```

	Strings can be specified to the logger by stating the


	FILTERS
	=======

	Filters allow for customized handling of reports. A filter can implement an
	actual logging process (e.g., to a file, over the network, etc), or it can
	suppress a report from going through. A filter can even modify a log such
	that its priority is altered (e.g., from "warning" to "error").


	THREAD SAFETY
	=============

	Adding and removing filters is *NOT* thread safe. This should be done prior
	to any multi-threaded logging operations.

	Logging *is* thread safe (as long as your filters are thread safe).


	INTERACTIONS
	============

	ax_types
	--------
	Uses the basic types defined there if AXLOG_TYPES_DEFINED has not been
	defined yet.

	ax_platform
	-----------
	Calls ax_static_assert to verify compile-time assumptions, such as the sizes
	of the types defined, but only if ax_static_assert is defined.

	ax_string
	---------
	When C++ features are used here, ax::Str will be used to make string
	management easier.

	ax_printf
	---------
	Used by axlogf() if available. This can be disabled by defining
	`AXLOG_NO_AXPF` prior to including this file.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_LOGGER_H_
#define INCGUARD_AX_LOGGER_H_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

#ifndef AXLOG_NO_PF
# define AXLOG_NO_PF 0
#endif

#if !defined( AX_NO_INCLUDES ) && defined( __has_include )
# if __has_include( "ax_platform.h" )
#  include "ax_platform.h"
# endif
# if __has_include( "ax_types.h" )
#  include "ax_types.h"
# endif
# if __has_include( "ax_string.h" )
#  include "ax_string.h"
# endif
# if !AXLOG_NO_PF && __has_include( "ax_printf.h" )
#  include "ax_printf.h"
# endif
#endif

#ifndef AXLOG_TYPES_DEFINED
# define AXLOG_TYPES_DEFINED 1
# ifdef AX_TYPES_DEFINED
typedef ax_uptr_t      axlog_uptr_t;
typedef ax_sptr_t      axlog_sptr_t;
typedef ax_u8_t        axlog_u8_t;
typedef ax_u16_t       axlog_u16_t;
typedef ax_u32_t       axlog_u32_t;
typedef ax_bool_t      axlog_bool_t;
# else
#  include <stddef.h>
typedef size_t         axlog_uptr_t;
typedef ptrdiff_t      axlog_sptr_t;
typedef unsigned char  axlog_u8_t;
typedef unsigned short axlog_u16_t;
typedef unsigned int   axlog_u32_t;
typedef unsigned char  axlog_bool_t;
# endif
#endif

#ifdef ax_static_assert
ax_static_assert( sizeof( axlog_u8_t  ) == 1, "ax_logger: axlog_u8_t has invalid size" );
ax_static_assert( sizeof( axlog_u16_t ) == 2, "ax_logger: axlog_u16_t has invalid size" );
ax_static_assert( sizeof( axlog_u32_t ) == 4, "ax_logger: axlog_u32_t has invalid size" );
#endif

#ifndef AXLOG_DEFAULT_FACILITY
# define AXLOG_DEFAULT_FACILITY 0
#endif
#ifndef AXLOG_DEFAULT_FLAGS
# define AXLOG_DEFAULT_FLAGS    0
#endif

#ifndef AXLOG_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXLOG_CXX_ENABLED     AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXLOG_CXX_ENABLED     1
# else
#  define AXLOG_CXX_ENABLED     0
# endif
#endif

#ifndef AXLOG_CXX_OVERLOADS_ENABLED
# define AXLOG_CXX_OVERLOADS_ENABLED      1
#endif
#ifndef AXLOG_CXX_STRUCT_METHODS_ENABLED
# define AXLOG_CXX_STRUCT_METHODS_ENABLED 1
#endif
#ifndef AXLOG_CXX_CLASSES_ENABLED
# define AXLOG_CXX_CLASSES_ENABLED        1
#endif

/* function declaration specifier */
#ifndef AXLOG_FUNC
# ifdef AX_FUNC
#  define AXLOG_FUNC AX_FUNC
# else
#  define AXLOG_FUNC extern
# endif
#endif

/* function calling convention */
#ifndef AXLOG_CALL
# ifdef AX_CALL
#  define AXLOG_CALL AX_CALL
# else
#  define AXLOG_CALL
# endif
#endif

#ifndef AXLOG_CALLF
# ifdef AX_CALLF
#  define AXLOG_CALLF               AX_CALLF
# elif defined( _WIN32 )
#  define AXLOG_CALLF               __cdecl
# else
#  define AXLOG_CALLF
# endif
#endif

#ifdef AXLOG_IMPLEMENTATION
# define AXLOG_IMPLEMENT 1
#else
# define AXLOG_IMPLEMENT 0
#endif

/* cannot use c++ features in c */
#ifndef __cplusplus
# undef  AXLOG_CXX_ENABLED
# define AXLOG_CXX_ENABLED 0
#endif
/* if c++ features in general aren't enabled then disable all individual features */
#if !AXLOG_CXX_ENABLED
# undef  AXLOG_CXX_OVERLOADS_ENABLED
# undef  AXLOG_CXX_STRUCT_METHODS_ENABLED
# undef  AXLOG_CXX_CLASSES_ENABLED
# define AXLOG_CXX_OVERLOADS_ENABLED      0
# define AXLOG_CXX_STRUCT_METHODS_ENABLED 0
# define AXLOG_CXX_CLASSES_ENABLED        0
#endif

/* if ax_printf wasn't found then it can't be used */
#if !AXLOG_NO_PF && !defined( INCGUARD_AX_PRINTF_H_ )
# undef  AXLOG_NO_PF
# define AXLOG_NO_PF 1
#endif
#ifndef AXLOG_PF_FSPATH_SPEC
# if !AXLOG_NO_PF
#  define AXLOG_PF_FSPATH_SPEC "/"
# else
#  define AXLOG_PF_FSPATH_SPEC ""
# endif
#endif

/* determine whether ax::Str is available */
#ifndef AXLOG_HAS_CXX_AXSTR
# if defined(INCGUARD_AX_STRING_H_) && defined(AXSTR_CXX_CLASSES_ENABLED)
#  define AXLOG_HAS_CXX_AXSTR 1
# else
#  define AXLOG_HAS_CXX_AXSTR 0
# endif
#endif

/* determine the operating system */
#ifndef AXLOG_OS_DEFINED
# ifdef INCGUARD_AX_PLATFORM_H_
#  define AXLOG_OS_WINDOWS  AX_OS_WINDOWS
#  define AXLOG_OS_LINUX    AX_OS_LINUX
#  define AXLOG_OS_MACOSX   AX_OS_MACOSX
# else
#  define AXLOG_OS_WINDOWS  0
#  define AXLOG_OS_LINUX    0
#  define AXLOG_OS_MACOSX   0
#  if defined( _WIN32 )
#   undef  AXLOG_OS_WINDOWS
#   define AXLOG_OS_WINDOWS 1
#  elif defined( linux ) || defined( __linux ) | defined( __linux__ )
#   undef  AXLOG_OS_LINUX
#   define AXLOG_OS_LINUX   1
#  elif defined( __APPLE__ )
#   undef  AXLOG_OS_MACOSX
#   define AXLOG_OS_MACOSX  1
#  endif
# endif
# define AXLOG_OS_DEFINED   1
#endif
#if !(AXLOG_OS_WINDOWS|AXLOG_OS_LINUX|AXLOG_OS_MACOSX)
# error ax_logger: AXLOG_OS not determined!
#endif

#if AXLOG_IMPLEMENT
# if AXLOG_OS_WINDOWS
#  undef  WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN 1
#  if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0600
#   undef _WIN32_WINNT
#   define _WIN32_WINNT       0x0603
#  endif
#  include <Windows.h>
#  undef min
#  undef max
#  undef AddJob
#  undef Yield
# endif
# if AXLOG_OS_LINUX || AXLOG_OS_MACOSX
#  undef  _GNU_SOURCE
#  define _GNU_SOURCE
#  include <unistd.h>
#  include <sys/types.h>
#  if AXLOG_OS_LINUX
#   include <sys/syscall.h>
#  endif
# endif
# if AXLOG_OS_MACOSX
#  include <pthread.h>
# endif
#endif

/* printf()-like routines */
#ifndef AXLOG_PRINTOUTF
# if AXLOG_NO_PF
#  define AXLOG_PRINTOUTF printf
# else
#  define AXLOG_PRINTOUTF axpf
# endif
#endif
#ifndef AXLOG_PRINTERRF
# if AXLOG_NO_PF
#  define AXLOG_PRINTERRF(...) fprintf(stderr, __VA_ARGS__)
# else
#  define AXLOG_PRINTERRF axerrf
# endif
#endif
#ifndef AXLOG_SNPRINTF
# if AXLOG_NO_PF
#  ifdef _MSC_VER
#   define AXLOG_SNPRINTF sprintf_s
#  else
#   define AXLOG_SNPRINTF snprintf
#  endif
# else
#  define AXLOG_SNPRINTF  axspf
# endif
#endif
#ifndef AXLOG_SNPRINTFV
# if AXLOG_NO_PF
#  ifdef _MSC_VER
#   define AXLOG_SNPRINTFV vsprintf_s
#  else
#   define AXLOG_SNPRINTFV vsnprintf
#  endif
# else
#  define AXLOG_SNPRINTFV  axspfv
# endif
#endif

/* varargs */
#ifndef AXLOG_CUSTOM_VARARGS
# define AXLOG_CUSTOM_VARARGS 0
#endif
#if !AXLOG_CUSTOM_VARARGS
# include <stdarg.h>
# define AXLOG_VA_T va_list
# define AXLOG_VA_S va_start
# define AXLOG_VA_E va_end
#else
# if !defined(AXLOG_VA_T) || !defined(AXLOG_VA_S) || !defined(AXLOG_VA_E)
#  error ax_logger: AXLOG_CUSTOM_VARARGS is defined, but not expected macros.
# endif
#endif

/* VS style */
#ifndef AXLOG_VS_STYLE
# if AXLOG_OS_WINDOWS
#  define AXLOG_VS_STYLE 1
# else
#  define AXLOG_VS_STYLE 0
# endif
#endif

/* current function */
#ifndef AXLOG_FUNCTION
# if defined( AX_PRETTY_FUNCTION )
#  define AXLOG_FUNCTION AX_PRETTY_FUNCTION
# elif defined( AX_FUNCTION )
#  define AXLOG_FUNCTION AX_FUNCTION
# elif defined( _MSC_VER )
#  define AXLOG_FUNCTION __FUNCTION__
# else
#  define AXLOG_FUNCTION __func__
# endif
#endif

/* C helper */
#ifdef __cplusplus
# define AXLOG__EXTRNC_ENTER extern "C" {
# define AXLOG__EXTRNC_LEAVE }
#else
# define AXLOG__EXTRNC_ENTER
# define AXLOG__EXTRNC_LEAVE
#endif

/* program-specific "facility" / "categorization" bits */
#define AXLOG_FACILITY_MASK 0x003F /* 0000 0000 0011 1111 */
/* priority of report; see axlog_priority_t */
#define AXLOG_PRIORITY_MASK 0x01C0 /* 0000 0001 1100 0000 */
/* cause of report; see axlog_cause_t */
#define AXLOG_CAUSE_MASK    0x1E00 /* 0001 1110 0000 0000 */
/* general flags for report; see axlog_flag_t */
#define AXLOG_FLAGS_MASK    0xE000 /* 1110 0000 0000 0000 */

/* specifies the priority for a given report */
typedef enum axlog_priority_e
{
	/* information typically only useful to programmers */
	axlogp_debug    = 0x0000U,
	/* general informational message */
	axlogp_info     = 0x0040U,
	/* non-error conditions where handling them should be considered */
	axlogp_notice   = 0x0080U,
	/* something that is technically valid but might be undesired */
	axlogp_warning  = 0x00C0U,
	/* something that is invalid or wrong, but can be recovered from */
	axlogp_error    = 0x0100U,
	/* errors that cannot be recovered from within their context */
	axlogp_critical = 0x0140U,
	/* condition that requires user correction (e.g., corrupt database) */
	axlogp_alert    = 0x0180U,
	/* unrecoverable condition in which application termination is imminent */
	axlogp_panic    = 0x01C0U,

	/* invalid priority (not a real priority) */
	axlogp_invalid  = 0xFFFFU
} axlog_priority_t;

/* specifies what caused a report to be generated */
typedef enum axlog_cause_e
{
	/* report generated wilfully, by design or requirement of the program */
	axlogc_prog     = 0x0000U,
	/* status report generated during initialization */
	axlogc_init     = 0x0200U,
	/* status report generated during finishing phases of the program */
	axlogc_fini     = 0x0400U,

	/* report generated from processing an internal file */
	axlogc_intfile  = 0x0600U,
	/* report generated from processing a user-supplied (external) file */
	axlogc_extfile  = 0x0800U,

	/* report generated for tracing purposes */
	axlogc_trace    = 0x0A00U,
	/* report generated for some other development/debug purpose */
	axlogc_devel    = 0x0C00U,
	/* report generated for statistics tracking */
	axlogc_stats    = 0x0E00U,

	/* extra details for a prior report (e.g., "included from here") */
	axlogc_details  = 0x1000U,

	/* ran out of memory */
	axlogc_nomem    = 0x1200U,
	/* a buffer overflow was detected */
	axlogc_bufover  = 0x1400U,
	/* a buffer underflow was detected */
	axlogc_bufunder = 0x1600U,
	/* a bounds check failed (index is out of range) */
	axlogc_bounds   = 0x1800U,
	/* a valid pointer was expected */
	axlogc_nullptr  = 0x1C00U,
	/* an internal check (e.g., invalid parameter) in the code failed */
	axlogc_check    = 0x1A00U,
	/* a runtime failure occurred */
	axlogc_runtime  = 0x1E00U,

	/* invalid cause (not a real cause) */
	axlogc_invalid  = 0xFFFFU
} axlog_cause_t;

/* flags used during the logging process */
typedef enum axlog_flag_e
{
	/* log system information (process ID, thread ID, error code(s), time) */
	axlogf_sysinfo  = 0x2000U,

	/* do not display this log event to the end-user */
	axlogf_internal = 0x4000U,

	/*
	axlogf_reserved = 0x8000U,
	*/

	/* invalid flags (not a real flag) */
	axlogf_invalid  = 0xFFFFU
} axlog_flag_t;

/* string reference */
typedef struct axlog_str_s
{
	/* pointer to the start of the string */
	const char *s;
	/* pointer to the end of the string; if NULL then string has terminator */
	const char *e;

#if AXLOG_CXX_STRUCT_METHODS_ENABLED
	inline axlog_str_s()
	: s( ( const char * )0 )
	, e( ( const char * )0 )
	{
	}
	inline axlog_str_s( const char *p )
	: s( p )
	, e( ( const char * )0 )
	{
	}
	inline axlog_str_s( const char *s, const char *e )
	: s( s )
	, e( e )
	{
	}
	inline axlog_str_s( const axlog_str_s &x )
	: s( x.s )
	, e( x.e )
	{
	}
# if AXLOG_HAS_CXX_AXSTR
	inline axlog_str_s( const ax::Str &x )
	: s( x.get() )
	, e( x.getEnd() )
	{
	}
	inline axlog_str_s( const ax::MutStr &x )
	: s( x.get() )
	, e( x.get() + x.len() )
	{
	}
# endif
	inline ~axlog_str_s()
	{
	}

	inline axlog_str_s &operator=( const char *p )
	{
		s = p;
		e = ( const char * )0;
		return *this;
	}
	inline axlog_str_s &operator=( const axlog_str_s &x )
	{
		s = x.s;
		e = x.e;
		return *this;
	}
# if AXLOG_HAS_CXX_AXSTR
	inline axlog_str_s &operator=( const ax::Str &x )
	{
		s = x.get();
		e = x.getEnd();
		return *this;
	}
	inline axlog_str_s &operator=( const ax::MutStr &x )
	{
		s = x.get();
		e = x.get() + x.len();
		return *this;
	}
# endif
#endif
} axlog_str_t;

/* range reference (for pointing out a specific place in a file) */
typedef struct axlog_range_s
{
	/* text of the line */
	axlog_str_t    linetext;

	/* offset on the line to the start of the range */
	axlog_u32_t    start;
	/* offset from the start of the range to the end */
	axlog_u32_t    count;

	/* offset from the start of the range to a point of interest */
	axlog_u32_t    point;

#if AXLOG_CXX_STRUCT_METHODS_ENABLED
	inline axlog_range_s()
	: linetext()
	, start(0)
	, count(0)
	, point(0)
	{
	}
	inline ~axlog_range_s()
	{
	}
#endif
} axlog_range_t;

/* basic file/line information details for a report */
typedef struct axlog_lineinfo_s
{
	/* name of the file */
	axlog_str_t   file;
	/* line number within the file (0 means no particular line) */
	axlog_u32_t   line;
	/* column within the line (0 means no particular column) */
	axlog_u32_t   column;

	/* function the report occurred within */
	axlog_str_t   func;
	/* expression that caused the report to be generated */
	axlog_str_t   expr;

	/* range of text to display for the given line */
	axlog_range_t range;

#if AXLOG_CXX_STRUCT_METHODS_ENABLED
	inline axlog_lineinfo_s()
	: file()
	, line(0)
	, column(0)
	, func()
	, expr()
	, range()
	{
	}
	inline ~axlog_lineinfo_s()
	{
	}
#endif
} axlog_lineinfo_t;

/* details for a report */
typedef struct axlog_report_s
{
	/* logging flags for the report (includes priority, facility, cause...) */
	axlog_u16_t      flags;

	/* module which generated this report (e.g., "basic3d.dll" or "-Wall") */
	axlog_str_t      mod;
	/* actual message of the report */
	axlog_str_t      msg;

	/* line information for the report */
	axlog_lineinfo_t info;

#if AXLOG_CXX_STRUCT_METHODS_ENABLED
	inline axlog_report_s()
	: flags(AXLOG_DEFAULT_FLAGS)
	, mod()
	, msg()
	, info()
	{
	}
	inline ~axlog_report_s()
	{
	}

	inline axlog_u16_t getFacility() const
	{
		return flags & AXLOG_FACILITY_MASK;
	}
	inline axlog_priority_t getPriority() const
	{
		return axlog_priority_t( flags & AXLOG_PRIORITY_MASK );
	}
	inline axlog_cause_t getCause() const
	{
		return axlog_cause_t( flags & AXLOG_CAUSE_MASK );
	}

	inline bool any( axlog_u16_t inFlags ) const
	{
		return ( ( flags & AXLOG_FLAGS_MASK ) & inFlags ) != 0;
	}
	inline bool all( axlog_u16_t inFlags ) const
	{
		return ( ( flags & AXLOG_FLAGS_MASK ) & inFlags ) == inFlags;
	}
#endif
} axlog_report_t;

/* system details for a report */
typedef struct axlog_sysinfo_s
{
	axlog_u32_t processId;
	axlog_u32_t threadId;
	int         posixErr;
	axlog_u32_t systemErr;
} axlog_sysinfo_t;

/* result of an attempted operation (generic) */
typedef enum axlog_result_e
{
	/* operation completed successfully */
	axlog_result_ok   = 1,

	/* operation failed for an unknown reason */
	axlog_result_fail = 0,

	/* operation failed because we ran out of memory */
	axlog_result_nomem = -10000,
	/* operation failed because of an invalid parameter */
	axlog_result_badarg,

	/* operation failed because there are too many filters */
	axlog_result_toomanyfilters,
	/* operation failed because the filter already exists */
	axlog_result_filterexists,
	/* operation failed because the filter does not exist */
	axlog_result_filternotfound,

	/* operation was cancelled by a filter */
	axlog_result_rejected
} axlog_result_t;
#define AXLOG_SUCCEEDED(X_)\
	(((axlog_result_t)(X_))==axlog_result_ok)
#define AXLOG_FAILED(X_)\
	(((axlog_result_t)(X_))!=axlog_result_ok)

/* type of a given filter (used for sorting purposes) */
typedef enum axlog_filter_type_e
{
	/* filter is primarily an endpoint (outputs logs) */
	axlog_filter_endpoint,
	/* filter is primarily a manager (alters or rejects logs) */
	axlog_filter_manager
} axlog_filter_type_t;

/* result of a filtering operation */
typedef enum axlog_send_e
{
	/* cancel the report; do not send any further */
	axlog_cancel,
	/* pass the report along to the next filter */
	axlog_forward
} axlog_send_t;

/* filter used for handling reports - 99% of the time, return `axlog_forward` */
typedef axlog_send_t( AXLOG_CALL *axlog_filter_t )( void *, axlog_report_t *, const axlog_sysinfo_t * );




AXLOG__EXTRNC_ENTER




/* convert an axlog result code into a human readable zero-terminated string */
AXLOG_FUNC const char *AXLOG_CALL axlog_result_to_string( axlog_result_t r )
#if AXLOG_IMPLEMENT
{
	switch( r ) {
	case axlog_result_ok:             return "ok";

	case axlog_result_fail:           return "fail";

	case axlog_result_nomem:          return "nomem";
	case axlog_result_badarg:         return "badarg";

	case axlog_result_toomanyfilters: return "toomanyfilters";
	case axlog_result_filterexists:   return "filterexists";
	case axlog_result_filternotfound: return "filternotfound";

	case axlog_result_rejected:       return "rejected";
	}

	return "(unknown)";
}
#else
;
#endif

#if AXLOG_IMPLEMENT

# ifndef AXLOG_MAX_FILTERS
#  define AXLOG_MAX_FILTERS 16
# endif
# define AXLOG__MASK_BITS 32
# define AXLOG__NUM_MASKS ((AXLOG_MAX_FILTERS+(AXLOG__MASK_BITS-1))/AXLOG__MASK_BITS)
# define AXLOG__FULL_MASK 0xFFFFFFFFU

# define AXLOG__MAX_FACILITIES (AXLOG_FACILITY_MASK+1)

struct axlog__filter_item_s;
typedef struct axlog__filter_item_s axlog__filter_item_t;

struct axlog__filter_item_s
{
	axlog_filter_t        pfnFilter;
	void *                pUserData;
	axlog__filter_item_t *pNext;
};

static axlog__filter_item_t  axlog__g_filters[ AXLOG_MAX_FILTERS ];
static axlog_u32_t           axlog__g_usedFilters[ AXLOG__NUM_MASKS ];
static axlog_u32_t           axlog__g_cFilters = 0;

static axlog__filter_item_t *axlog__g_pEndpointHead = (axlog__filter_item_t*)0;
static axlog__filter_item_t *axlog__g_pPassthruHead = (axlog__filter_item_t*)0;

static const char *          axlog__g_pszFacilities[ AXLOG__MAX_FACILITIES ];
static axlog_u32_t           axlog__g_cFacilities = 0;

static axlog__filter_item_t **axlog__get_filter_list( axlog_filter_type_t t )
{
	switch( t ) {
	case axlog_filter_endpoint: return &axlog__g_pEndpointHead;
	case axlog_filter_manager:  return &axlog__g_pPassthruHead;
	}

	return ( axlog__filter_item_t ** )0;
}

static void AXLOG_CALL axlog__set_used_filter( axlog_u32_t i )
{
	axlog__g_usedFilters[ i/AXLOG__MASK_BITS ] |= 1U<<(i%AXLOG__MASK_BITS);
}
static void AXLOG_CALL axlog__clear_used_filter( axlog_u32_t i )
{
	axlog__g_usedFilters[ i/AXLOG__MASK_BITS ] &= ~(1U<<(i%AXLOG__MASK_BITS));
}
static axlog_u32_t AXLOG_CALL axlog__find_free_filter( void )
{
	axlog_u32_t i, j;

	for( i = 0; i < AXLOG__NUM_MASKS; ++i ) {
		if( axlog__g_usedFilters[i] == AXLOG__FULL_MASK ) {
			continue;
		}

		/* FIXME: Use the faster bit-search method */
		for( j = 0; j < AXLOG__MASK_BITS; ++j ) {
			if( ~axlog__g_usedFilters[i] & (1U<<j) ) {
				continue;
			}

			return i*AXLOG__MASK_BITS + j;
		}
	}

	return ~0U;
}

#endif

/* possible results from trying to add a filter */
typedef enum axlog_add_filter_result_e
{
	axlog_add_filter_result_ok             = axlog_result_ok,

	axlog_add_filter_result_badarg         = axlog_result_badarg,
	axlog_add_filter_result_toomanyfilters = axlog_result_toomanyfilters,
	axlog_add_filter_result_filterexists   = axlog_result_filterexists
} axlog_add_filter_result_t;

/* add a filter */
AXLOG_FUNC axlog_add_filter_result_t AXLOG_CALL axlog_add_filter( axlog_filter_type_t type, axlog_filter_t pfnFilter, void *pUserParm )
#if AXLOG_IMPLEMENT
{
	axlog__filter_item_t *p;
	axlog__filter_item_t **ppList;
	axlog_u32_t i;

	if( !pfnFilter ) {
		return axlog_add_filter_result_badarg;
	}

	if( !( ppList = axlog__get_filter_list( type ) ) ) {
		return axlog_add_filter_result_badarg;
	}

	if( axlog__g_cFilters == AXLOG_MAX_FILTERS ) {
		return axlog_add_filter_result_toomanyfilters;
	}
	if( ( i = axlog__find_free_filter() ) == ~0U ) {
		return axlog_add_filter_result_toomanyfilters;
	}

	for( p = *ppList; p != ( axlog__filter_item_t * )0; p = p->pNext ) {
		if( p->pfnFilter == pfnFilter && p->pUserData == pUserParm ) {
			return axlog_add_filter_result_filterexists;
		}
	}

	p = &axlog__g_filters[ i ];
	p->pfnFilter = pfnFilter;
	p->pUserData = pUserParm;
	p->pNext     = *ppList;
	*ppList = p;

	axlog__set_used_filter( i );
	++axlog__g_cFilters;

	return axlog_add_filter_result_ok;
}
#else
;
#endif

/* possible results from trying to remove a filter */
typedef enum axlog_remove_filter_result_e
{
	axlog_remove_filter_result_ok             = axlog_result_ok,

	axlog_remove_filter_result_badarg         = axlog_result_badarg,
	axlog_remove_filter_result_filternotfound = axlog_result_filternotfound
} axlog_remove_filter_result_t;

/* remove a filter */
AXLOG_FUNC axlog_remove_filter_result_t AXLOG_CALL axlog_remove_filter( axlog_filter_type_t type, axlog_filter_t pfnFilter, void *pUserParm )
#if AXLOG_IMPLEMENT
{
	axlog__filter_item_t *p, *prior;
	axlog__filter_item_t **ppList;
	axlog_u32_t i;

	if( !pfnFilter || !( ppList = axlog__get_filter_list( type ) ) ) {
		return axlog_remove_filter_result_badarg;
	}

	prior = ( axlog__filter_item_t * )0;
	for( p = *ppList; p != ( axlog__filter_item_t * )0; prior=p, p=p->pNext ) {
		if( p->pfnFilter != pfnFilter || p->pUserData != pUserParm ) {
			continue;
		}

		if( prior != ( axlog__filter_item_t * )0 ) {
			prior->pNext = p->pNext;
		}

		i = ( axlog_u32_t )( axlog_uptr_t )( p - &axlog__g_filters[0] );
		axlog__clear_used_filter( i );
		--axlog__g_cFilters;
		return axlog_remove_filter_result_ok;
	}

	return axlog_remove_filter_result_filternotfound;
}
#else
;
#endif

/* possible results from trying to set a range of facilties */
typedef enum axlog_set_facilities_result_e
{
	axlog_set_facilities_result_ok     = axlog_result_ok,

	axlog_set_facilities_result_badarg = axlog_result_badarg
} axlog_set_facilities_result_t;

/* set a range of facility names */
AXLOG_FUNC axlog_set_facilities_result_t AXLOG_CALL axlog_set_facilities( axlog_u32_t uBase, axlog_u32_t cFacilities, const char *const *ppszFacilities )
#if AXLOG_IMPLEMENT
{
	axlog_u32_t i;

	if( uBase + cFacilities > AXLOG__MAX_FACILITIES || !ppszFacilities ) {
		return axlog_set_facilities_result_badarg;
	}

	/* clear any unset facilities */
	for( i = axlog__g_cFacilities; i < uBase; ++i ) {
		axlog__g_pszFacilities[ i ] = ( const char * )0;
	}

	/* set our range */
	for( i = 0; i < cFacilities; ++i ) {
		axlog__g_pszFacilities[ uBase + i ] = ppszFacilities[ i ];
	}

	/* update the count */
	if( axlog__g_cFacilities < uBase + cFacilities ) {
		axlog__g_cFacilities = uBase + cFacilities;
	}

	return axlog_set_facilities_result_ok;
}
#else
;
#endif
/* set a specific facility name */
AXLOG_FUNC axlog_set_facilities_result_t AXLOG_CALL axlog_set_facility( axlog_u32_t i, const char *pszFacility )
#if AXLOG_IMPLEMENT
{
	return axlog_set_facilities( i, 1, &pszFacility );
}
#else
;
#endif

/* retrieve the facility of a given report (returns 0xFFFF if invalid) */
AXLOG_FUNC axlog_u16_t AXLOG_CALL axlog_get_facility( const axlog_report_t *p )
#if AXLOG_IMPLEMENT
{
	if( !p ) {
		return 0xFFFF;
	}

	return p->flags & AXLOG_FACILITY_MASK;
}
#else
;
#endif
/* retrieve the priority of a given report */
AXLOG_FUNC axlog_priority_t AXLOG_CALL axlog_get_priority( const axlog_report_t *p )
#if AXLOG_IMPLEMENT
{
	if( !p ) {
		return axlogp_invalid;
	}

	return (axlog_priority_t)( p->flags & AXLOG_PRIORITY_MASK );
}
#else
;
#endif
/* retrieve the cause of a given report */
AXLOG_FUNC axlog_cause_t AXLOG_CALL axlog_get_cause( const axlog_report_t *p )
#if AXLOG_IMPLEMENT
{
	if( !p ) {
		return axlogc_invalid;
	}

	return (axlog_cause_t)( p->flags & AXLOG_CAUSE_MASK );
}
#else
;
#endif
/* retrieve the flags of a given report */
AXLOG_FUNC axlog_u16_t AXLOG_CALL axlog_get_flags( const axlog_report_t *p )
#if AXLOG_IMPLEMENT
{
	if( !p ) {
		return ( axlog_u16_t )axlogf_invalid;
	}

	return p->flags & AXLOG_FLAGS_MASK;
}
#else
;
#endif

/* convert a facility to a zero-terminated string */
AXLOG_FUNC const char *AXLOG_CALL axlog_facility_to_string( axlog_u32_t x )
#if AXLOG_IMPLEMENT
{
	const char *p;

	if( x >= axlog__g_cFacilities ) {
		if( !x ) {
			return "default";
		}

		return "(invalid)";
	}

	p = axlog__g_pszFacilities[ x ];
	if( !p ) {
		return "(unknown)";
	}

	return p;
}
#else
;
#endif
/* convert a priority to a zero-terminated string */
AXLOG_FUNC const char *AXLOG_CALL axlog_priority_to_string( axlog_priority_t x )
#if AXLOG_IMPLEMENT
{
	switch( x ) {
	case axlogp_debug:    return "debug";
	case axlogp_info:     return "info";
	case axlogp_notice:   return "notice";
	case axlogp_warning:  return "warning";
	case axlogp_error:    return "error";
	case axlogp_critical: return "critical";
	case axlogp_alert:    return "alert";
	case axlogp_panic:    return "panic";

	case axlogp_invalid:  return "(invalid)";
	}

	return "(unknown)";
}
#else
;
#endif

/* convert a cause to a zero-terminated string */
AXLOG_FUNC const char *AXLOG_CALL axlog_cause_to_string( axlog_cause_t x )
#if AXLOG_IMPLEMENT
{
	switch( x ) {
	case axlogc_prog:     return "prog";
	case axlogc_init:     return "init";
	case axlogc_fini:     return "fini";

	case axlogc_intfile:  return "intfile";
	case axlogc_extfile:  return "extfile";

	case axlogc_trace:    return "trace";
	case axlogc_devel:    return "devel";
	case axlogc_stats:    return "stats";

	case axlogc_details:  return "details";

	case axlogc_nomem:    return "nomem";
	case axlogc_bufover:  return "bufover";
	case axlogc_bufunder: return "bufunder";
	case axlogc_bounds:   return "bounds";
	case axlogc_nullptr:  return "nullptr";
	case axlogc_check:    return "check";
	case axlogc_runtime:  return "runtime";

	case axlogc_invalid:  return "(invalid)";
	}

	return "(unknown)";
}
#else
;
#endif

#if AXLOG_IMPLEMENT
static axlog_send_t AXLOG_CALL axlog__default_endpoint_filter( void *, axlog_report_t *p, const axlog_sysinfo_t *q )
{
# define AXLOG__P(X_) (!(X_)?"(null)":(X_))
# if AXLOG_VS_STYLE
#  define AXLOG__LS "("
#  define AXLOG__LE "): "
#  define AXLOG__FN " in "
# else
#  define AXLOG__LS ":"
#  define AXLOG__LE ": "
#  define AXLOG__FN ": "
# endif
# define AXLOG__FS AXLOG_PF_FSPATH_SPEC

	char szFile[ 512 ];
	char szFunc[ 256 ];
	const char *pszPrompt;
	const char *pszFacL, *pszFac;
	const char *pszCauL, *pszCauR, *pszCause;

	( void )q;

	/* file/line/column */
	if( p->info.file.s != ( const char * )0 ) {
		if( p->info.file.e != ( const char * )0 ) {
			if( p->info.line != 0 ) {
				if( p->info.column != 0 ) {
					AXLOG_SNPRINTF( szFile, sizeof(szFile), "%" AXLOG__FS ".*s" AXLOG__LS "%u:%u" AXLOG__LE,
						(int)(p->info.file.e - p->info.file.s), p->info.file.s,
						p->info.line, p->info.column );
				} else {
					AXLOG_SNPRINTF( szFile, sizeof(szFile), "%" AXLOG__FS ".*s" AXLOG__LS "%u" AXLOG__LE,
						(int)(p->info.file.e - p->info.file.s), p->info.file.s,
						p->info.line );
				}
			} else {
				AXLOG_SNPRINTF( szFile, sizeof(szFile), "%" AXLOG__FS ".*s: ",
					(int)(p->info.file.e - p->info.file.s), p->info.file.s );
			}
		} else {
			if( p->info.line != 0 ) {
				if( p->info.column != 0 ) {
					AXLOG_SNPRINTF( szFile, sizeof(szFile), "%" AXLOG__FS "s" AXLOG__LS "%u:%u" AXLOG__LE,
						p->info.file.s, p->info.line, p->info.column );
				} else {
					AXLOG_SNPRINTF( szFile, sizeof(szFile), "%" AXLOG__FS "s" AXLOG__LS "%u" AXLOG__LE,
						p->info.file.s, p->info.line );
				}
			} else {
				AXLOG_SNPRINTF( szFile, sizeof(szFile), "%" AXLOG__FS "s: ", p->info.file.s );
			}
		}

		szFile[ sizeof(szFile) - 1 ] = '\0';
	} else {
		szFile[ 0 ] = '\0';
	}

	/* function/expression */
	if( p->info.func.s != ( const char * )0 ) {
		if( p->info.func.e != ( const char * )0 ) {
			if( p->info.expr.s != ( const char * )0 ) {
				if( p->info.expr.e != ( const char * )0 ) {
					AXLOG_SNPRINTF( szFunc, sizeof(szFunc), AXLOG__FN "`%.*s`, `%.*s`: ",
						(int)(p->info.func.e - p->info.func.s), p->info.func.s,
						(int)(p->info.expr.e - p->info.expr.s), p->info.expr.s
						);
				} else {
					AXLOG_SNPRINTF( szFunc, sizeof(szFunc), AXLOG__FN "`%.*s`, `%s`: ",
						(int)(p->info.func.e - p->info.func.s), p->info.func.s,
						p->info.expr.s );
				}
			} else {
				AXLOG_SNPRINTF( szFunc, sizeof(szFunc), AXLOG__FN "`%.*s`: ",
					(int)(p->info.func.e - p->info.func.s), p->info.func.s );
			}
		} else {
			if( p->info.expr.s != ( const char * )0 ) {
				if( p->info.expr.e != ( const char * )0 ) {
					AXLOG_SNPRINTF( szFunc, sizeof(szFunc), AXLOG__FN "`%s`, `%.*s`: ",
						p->info.func.s,
						(int)(p->info.expr.e - p->info.expr.s), p->info.expr.s
						);
				} else {
					AXLOG_SNPRINTF( szFunc, sizeof(szFunc), AXLOG__FN "`%s`, `%s`: ",
						p->info.func.s, p->info.expr.s );
				}
			} else {
				AXLOG_SNPRINTF( szFunc, sizeof(szFunc), AXLOG__FN "`%s`: ",
					p->info.func.s );
			}
		}

		szFunc[ sizeof(szFunc) - 1 ] = '\0';
	} else {
		szFunc[ 0 ] = ':';
		szFunc[ 1 ] = ' ';
		szFunc[ 2 ] = '\0';
	}

	/* prompt */
	switch( axlog_get_priority( p ) ) {
	case axlogp_critical:
		pszPrompt = "error: **1:CRITICAL**";
		break;

	case axlogp_alert:
		pszPrompt = "error: **2:ALERT**";
		break;

	case axlogp_panic:
		pszPrompt = "error: **3:FATAL**";
		break;

	default:
		pszPrompt = axlog_priority_to_string( axlog_get_priority( p ) );
		break;
	}

	/* cause */
	if( axlog_get_cause( p ) == axlogc_extfile ) {
		pszFacL  = "";
		pszFac   = "";

		pszCauL  = "";
		pszCause = "";
		pszCauR  = "";
	} else {
		pszFacL  = "[";
		pszFac   = axlog_facility_to_string( axlog_get_facility( p ) );

		pszCauL  = ":";
		pszCause = axlog_cause_to_string( axlog_get_cause( p ) );
		pszCauR  = "] ";
	}

	/* log it */
	if( axlog_get_priority( p ) < axlogp_notice ) {
		if( p->msg.e != ( const char * )0 ) {
			AXLOG_PRINTOUTF( "%s%s%s%s%s%s%s%s%.*s\n",
				pszFacL,pszFac,pszCauL,pszCause,pszCauR,
				szFile,pszPrompt,szFunc,
				(int)(p->msg.e - p->msg.s), AXLOG__P(p->msg.s) );
		} else {
			AXLOG_PRINTOUTF( "%s%s%s%s%s%s%s%s%s\n",
				pszFacL,pszFac,pszCauL,pszCause,pszCauR,
				szFile,pszPrompt,szFunc,
				AXLOG__P(p->msg.s) );
		}
	} else {
		if( p->msg.e != ( const char * )0 ) {
			AXLOG_PRINTERRF( "%s%s%s%s%s%s%s%s%.*s\n",
				pszFacL,pszFac,pszCauL,pszCause,pszCauR,
				szFile,pszPrompt,szFunc,
				(int)(p->msg.e - p->msg.s), AXLOG__P(p->msg.s) );
		} else {
			AXLOG_PRINTERRF( "%s%s%s%s%s%s%s%s%s\n",
				pszFacL,pszFac,pszCauL,pszCause,pszCauR,
				szFile,pszPrompt,szFunc,
				AXLOG__P(p->msg.s) );
		}
	}

	return axlog_forward;
# undef AXLOG__FS
# undef AXLOG__FN
# undef AXLOG__LE
# undef AXLOG__LS
# undef AXLOG__P
}
#endif

#if AXLOG_IMPLEMENT
static void AXLOG_CALL axlog__capture_sysinfo( axlog_sysinfo_t *p )
{
# if AXLOG_OS_WINDOWS
	p->processId = ( axlog_u32_t )GetCurrentProcessId();
	p->threadId  = ( axlog_u32_t )GetCurrentThreadId();
	p->posixErr  = errno;
	p->systemErr = ( axlog_u32_t )GetLastError();
# elif AXLOG_OS_LINUX
	p->processId = ( axlog_u32_t )getpid();
	/* not defined?
	p->threadId  = ( axlog_u32_t )gettid();
	*/
	p->threadId  = ( axlog_u32_t )syscall( SYS_gettid );
	p->posixErr  = errno;
	p->systemErr = errno;
# elif AXLOG_OS_MACOSX
	p->processId = ( axlog_u32_t )getpid();
	p->threadId  = ( axlog_u32_t )pthread_mach_thread_np( pthread_self() );
	p->posixErr  = errno;
	p->systemErr = errno;
# else
#  error ax_logger: OS not implemented for axlog__capture_sysinfo
# endif
}
#endif

/* possible results from trying to submit a report */
typedef enum axlog_submit_report_result_e
{
	axlog_submit_report_result_ok       = axlog_result_ok,

	axlog_submit_report_result_badarg   = axlog_result_badarg,
	axlog_submit_report_result_rejected = axlog_result_rejected
} axlog_submit_report_result_t;

/* submit a report */
AXLOG_FUNC axlog_submit_report_result_t AXLOG_CALL axlog_submit_report( axlog_report_t *pInoutReport )
#if AXLOG_IMPLEMENT
{
	axlog__filter_item_t *p;
	axlog_sysinfo_t si, *q;

	if( !pInoutReport || ( !pInoutReport->msg.s && axlog_get_cause( pInoutReport ) < axlogc_nomem ) ) {
		return axlog_submit_report_result_badarg;
	}

	q = ( axlog_sysinfo_t * )0;
	if( pInoutReport->flags & axlogf_sysinfo ) {
		axlog__capture_sysinfo( &si );
		q = &si;
	}

	for( p = axlog__g_pPassthruHead; p != ( axlog__filter_item_t * )0; p = p->pNext ) {
		if( p->pfnFilter( p->pUserData, pInoutReport, q ) == axlog_cancel ) {
			return axlog_submit_report_result_rejected;
		}
	}

	if( !axlog__g_pEndpointHead ) {
		if( axlog__default_endpoint_filter( ( void * )0, pInoutReport, q ) == axlog_cancel ) {
			return axlog_submit_report_result_rejected;
		}

		return axlog_submit_report_result_ok;
	}

	for( p = axlog__g_pEndpointHead; p != ( axlog__filter_item_t * )0; p = p->pNext ) {
		if( p->pfnFilter( p->pUserData, pInoutReport, q ) == axlog_cancel ) {
			return axlog_submit_report_result_rejected;
		}
	}

	return axlog_submit_report_result_ok;
}
#else
;
#endif

/* possible results from trying to initialize a report */
typedef enum axlog_init_report_result_e
{
	axlog_init_report_result_ok     = axlog_result_ok,

	axlog_init_report_result_badarg = axlog_result_badarg
} axlog_init_report_result_t;

/* initialize a report */
AXLOG_FUNC axlog_init_report_result_t
AXLOG_CALL axlog_init_reportexv
(
	char *             pszDstMsg,
	axlog_uptr_t       cDstMsg,
	axlog_report_t *   pDstReport,
	axlog_u16_t        flags,
	const axlog_str_t *pFile,
	axlog_u32_t        line,
	const axlog_str_t *pFunc,
	const axlog_str_t *pExpr,
	const char *       pszFmt,
	AXLOG_VA_T         fmtArgs
)
#if AXLOG_IMPLEMENT
{
	if( !pszDstMsg || !cDstMsg || !pDstReport || !pszFmt ) {
		return axlog_init_report_result_badarg;
	}

	AXLOG_SNPRINTFV( pszDstMsg, cDstMsg, pszFmt, fmtArgs );

	pDstReport->flags = flags;

	pDstReport->mod.s = ( const char * )0;
	pDstReport->mod.e = ( const char * )0;
	pDstReport->msg.s = pszDstMsg;
	pDstReport->msg.e = ( const char * )0;

	pDstReport->info.file.s = !pFile ? ( const char * )0 : pFile->s;
	pDstReport->info.file.e = !pFile ? ( const char * )0 : pFile->e;
	pDstReport->info.line   = line;
	pDstReport->info.column = 0;

	pDstReport->info.func.s = !pFunc ? ( const char * )0 : pFunc->s;
	pDstReport->info.func.e = !pFunc ? ( const char * )0 : pFunc->e;
	pDstReport->info.expr.s = !pExpr ? ( const char * )0 : pExpr->s;
	pDstReport->info.expr.e = !pExpr ? ( const char * )0 : pExpr->e;

	pDstReport->info.range.linetext.s = ( const char * )0;
	pDstReport->info.range.linetext.e = ( const char * )0;

	pDstReport->info.range.start = 0;
	pDstReport->info.range.count = 0;
	pDstReport->info.range.point = 0;

	return axlog_init_report_result_ok;
}
#else
;
#endif
AXLOG_FUNC  axlog_init_report_result_t
AXLOG_CALLF axlog_init_reportex
(
	char *             pszDstMsg,
	axlog_uptr_t       cDstMsg,
	axlog_report_t *   pDstReport,
	axlog_u16_t        flags,
	const axlog_str_t *pFile,
	axlog_u32_t        line,
	const axlog_str_t *pFunc,
	const axlog_str_t *pExpr,
	const char *       pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_init_report_result_t r;
	AXLOG_VA_T                 fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r =
		axlog_init_reportexv
		(
			pszDstMsg,
			cDstMsg,
			pDstReport,
			flags,
			pFile,
			line,
			pFunc,
			pExpr,
			pszFmt,
			fmtArgs
		);
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif
AXLOG_FUNC axlog_init_report_result_t
AXLOG_CALL axlog_init_reportv
(
	char *          pszDstMsg,
	axlog_uptr_t    cDstMsg,
	axlog_report_t *pDstReport,
	axlog_u16_t     flags,
	const char *    pszFile,
	axlog_u32_t     line,
	const char *    pszFunc,
	const char *    pszExpr,
	const char *    pszFmt,
	AXLOG_VA_T      fmtArgs
)
#if AXLOG_IMPLEMENT
{
	if( !pszDstMsg || !cDstMsg || !pDstReport || !pszFmt ) {
		return axlog_init_report_result_badarg;
	}

	AXLOG_SNPRINTFV( pszDstMsg, cDstMsg, pszFmt, fmtArgs );

	pDstReport->flags = flags;

	pDstReport->mod.s = ( const char * )0;
	pDstReport->mod.e = ( const char * )0;
	pDstReport->msg.s = pszDstMsg;
	pDstReport->msg.e = ( const char * )0;

	pDstReport->info.file.s = pszFile;
	pDstReport->info.file.e = ( const char * )0;
	pDstReport->info.line   = line;
	pDstReport->info.column = 0;

	pDstReport->info.func.s = pszFunc;
	pDstReport->info.func.e = ( const char * )0;
	pDstReport->info.expr.s = pszExpr;
	pDstReport->info.expr.e = ( const char * )0;

	pDstReport->info.range.linetext.s = ( const char * )0;
	pDstReport->info.range.linetext.e = ( const char * )0;

	pDstReport->info.range.start = 0;
	pDstReport->info.range.count = 0;
	pDstReport->info.range.point = 0;

	return axlog_init_report_result_ok;
}
#else
;
#endif
AXLOG_FUNC  axlog_init_report_result_t
AXLOG_CALLF axlog_init_report
(
	char *          pszDstMsg,
	axlog_uptr_t    cDstMsg,
	axlog_report_t *pDstReport,
	axlog_u16_t     flags,
	const char *    pszFile,
	axlog_u32_t     line,
	const char *    pszFunc,
	const char *    pszExpr,
	const char *    pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_init_report_result_t r;
	AXLOG_VA_T                 fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r =
		axlog_init_reportv
		(
			pszDstMsg,
			cDstMsg,
			pDstReport,
			flags,
			pszFile,
			line,
			pszFunc,
			pszExpr,
			pszFmt,
			fmtArgs
		);
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

#ifndef AXLOG_FMTBUF_SIZE
# define AXLOG_FMTBUF_SIZE 1024
#endif

/* log a message (with an expression) */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_submitexprfv
(
	axlog_u16_t flags,
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszExpr,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	axlog_report_t rep;
	char           szBuf[ AXLOG_FMTBUF_SIZE ];

	if
	(
		axlog_init_reportv
		(
			szBuf, AXLOG_FMTBUF_SIZE,
			&rep,
			flags,
			pszFile, line, pszFunc, pszExpr,
			pszFmt, fmtArgs
		) != axlog_init_report_result_ok
	) {
		return axlog_submit_report_result_badarg;
	}

	return axlog_submit_report( &rep );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_submitexprf
(
	axlog_u16_t flags,
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszExpr,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_submitexprfv( flags, pszFile, line, pszFunc, pszExpr, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log a message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_submitfv
(
	axlog_u16_t flags,
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	axlog_report_t rep;
	char           szBuf[ AXLOG_FMTBUF_SIZE ];

	if
	(
		axlog_init_reportv
		(
			szBuf, AXLOG_FMTBUF_SIZE,
			&rep,
			flags,
			pszFile, line, pszFunc, ( const char * )0,
			pszFmt, fmtArgs
		) != axlog_init_report_result_ok
	) {
		return axlog_submit_report_result_badarg;
	}

	return axlog_submit_report( &rep );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_submitf
(
	axlog_u16_t flags,
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_submitfv( flags, pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log a debug (axlogp_debug) message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_debugfv
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( axlogp_debug|AXLOG_DEFAULT_FACILITY, pszFile, line, pszFunc, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_debugf
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_debugfv( pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log an informational (axlogp_info) message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_infofv
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( axlogp_info|AXLOG_DEFAULT_FACILITY, pszFile, line, pszFunc, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_infof
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_infofv( pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log a notice (axlogp_notice) message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_notefv
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( axlogp_notice|AXLOG_DEFAULT_FACILITY, pszFile, line, pszFunc, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_notef
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_notefv( pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log a warning (axlogp_warning) message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_warnfv
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( axlogp_warning|AXLOG_DEFAULT_FACILITY, pszFile, line, pszFunc, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_warnf
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_warnfv( pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log an error (axlogp_error) message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_errorfv
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( axlogp_error|AXLOG_DEFAULT_FACILITY, pszFile, line, pszFunc, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_errorf
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_errorfv( pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log a critical error (axlogp_critical) message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_criticalfv
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( axlogp_critical|AXLOG_DEFAULT_FACILITY, pszFile, line, pszFunc, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_criticalf
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_criticalfv( pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log an error alert (axlogp_alert) message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_alertfv
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( axlogp_alert|AXLOG_DEFAULT_FACILITY, pszFile, line, pszFunc, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_alertf
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_alertfv( pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log a fatal error (axlogp_panic) message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlog_panicfv
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	AXLOG_VA_T  fmtArgs
)
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( axlogp_panic|AXLOG_DEFAULT_FACILITY, pszFile, line, pszFunc, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlog_panicf
(
	const char *pszFile,
	axlog_u32_t line,
	const char *pszFunc,
	const char *pszFmt,
	...
)
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_panicfv( pszFile, line, pszFunc, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif

/* log a general message */
AXLOG_FUNC axlog_submit_report_result_t
AXLOG_CALL axlogfv( axlog_u16_t flags, const char *pszFmt, AXLOG_VA_T fmtArgs )
#if AXLOG_IMPLEMENT
{
	return axlog_submitfv( flags, (const char *)0, 0, (const char *)0, pszFmt, fmtArgs );
}
#else
;
#endif
AXLOG_FUNC  axlog_submit_report_result_t
AXLOG_CALLF axlogf( axlog_u16_t flags, const char *pszFmt, ... )
#if AXLOG_IMPLEMENT
{
	axlog_submit_report_result_t r;
	AXLOG_VA_T                   fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r = axlog_submitfv( flags, (const char *)0, 0, (const char *)0, pszFmt, fmtArgs );
	AXLOG_VA_E( fmtArgs );

	return r;
}
#else
;
#endif




AXLOG__EXTRNC_LEAVE




#if AXLOG_CXX_OVERLOADS_ENABLED

// convert an axlog result code into a human readable zero-terminated string
inline const char *AXLOG_CALL axlog_result_to_string( axlog_add_filter_result_t r )
{
	return axlog_result_to_string( axlog_result_t( r ) );
}
inline const char *AXLOG_CALL axlog_result_to_string( axlog_remove_filter_result_t r )
{
	return axlog_result_to_string( axlog_result_t( r ) );
}
inline const char *AXLOG_CALL axlog_result_to_string( axlog_set_facilities_result_t r )
{
	return axlog_result_to_string( axlog_result_t( r ) );
}
inline const char *AXLOG_CALL axlog_result_to_string( axlog_submit_report_result_t r )
{
	return axlog_result_to_string( axlog_result_t( r ) );
}
inline const char *AXLOG_CALL axlog_result_to_string( axlog_init_report_result_t r )
{
	return axlog_result_to_string( axlog_result_t( r ) );
}

// set a range of facility names
template< axlog_u32_t tFacilities >
inline     axlog_set_facilities_result_t
AXLOG_CALL axlog_set_facilities
(
	axlog_u32_t uBase,
	const char *const( &pszFacilities )[ tFacilities ]
)
{
	return axlog_set_facilities( uBase, tFacilities, pszFacilities );
}
template< axlog_u32_t tFacilities >
inline     axlog_set_facilities_result_t
AXLOG_CALL axlog_set_facilities
(
	const char *const( &pszFacilities )[ tFacilities ]
)
{
	return axlog_set_facilities( 0, tFacilities, pszFacilities );
}

// initialize a report
template< axlog_uptr_t tDstMsg >
inline     axlog_init_report_result_t
AXLOG_CALL axlog_init_reportexv
(
	char( &szDstMsg )[ tDstMsg ],
	axlog_report_t *   pDstReport,
	axlog_u16_t        flags,
	const axlog_str_t &file,
	axlog_u32_t        line,
	const axlog_str_t &func,
	const axlog_str_t &expr,
	const char *       pszFmt,
	AXLOG_VA_T         fmtArgs
)
{
	return
		axlog_init_reportexv
		(
			szDstMsg,
			tDstMsg,
			pDstReport,
			flags,
			&file,
			line,
			&func,
			&expr,
			pszFmt,
			fmtArgs
		);
}
template< axlog_uptr_t tDstMsg >
inline      axlog_init_report_result_t
AXLOG_CALLF axlog_init_reportex
(
	char( &szDstMsg )[ tDstMsg ],
	axlog_report_t *   pDstReport,
	axlog_u16_t        flags,
	const axlog_str_t &file,
	axlog_u32_t        line,
	const axlog_str_t &func,
	const axlog_str_t &expr,
	const char *       pszFmt,
	...
)
{
	axlog_init_report_result_t r;
	AXLOG_VA_T                 fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r =
		axlog_init_reportexv
		(
			szDstMsg,
			tDstMsg,
			pDstReport,
			flags,
			&file,
			line,
			&func,
			&expr,
			pszFmt,
			fmtArgs
		);
	AXLOG_VA_E( fmtArgs );

	return r;
}
template< axlog_uptr_t tDstMsg >
inline     axlog_init_report_result_t
AXLOG_CALL axlog_init_reportv
(
	char( &szDstMsg )[ tDstMsg ],
	axlog_report_t *pDstReport,
	axlog_u16_t     flags,
	const char *    pszFile,
	axlog_u32_t     line,
	const char *    pszFunc,
	const char *    pszExpr,
	const char *    pszFmt,
	AXLOG_VA_T      fmtArgs
)
{
	return
		axlog_init_reportv
		(
			szDstMsg,
			tDstMsg,
			pDstReport,
			flags,
			pszFile,
			line,
			pszFunc,
			pszExpr,
			pszFmt,
			fmtArgs
		);
}
template< axlog_uptr_t tDstMsg >
inline      axlog_init_report_result_t
AXLOG_CALLF axlog_init_report
(
	char( &szDstMsg )[ tDstMsg ],
	axlog_report_t *pDstReport,
	axlog_u16_t     flags,
	const char *    pszFile,
	axlog_u32_t     line,
	const char *    pszFunc,
	const char *    pszExpr,
	const char *    pszFmt,
	...
)
{
	axlog_init_report_result_t r;
	AXLOG_VA_T                 fmtArgs;

	AXLOG_VA_S( fmtArgs, pszFmt );
	r =
		axlog_init_reportv
		(
			szDstMsg,
			tDstMsg,
			pDstReport,
			flags,
			pszFile,
			line,
			pszFunc,
			pszExpr,
			pszFmt,
			fmtArgs
		);
	AXLOG_VA_E( fmtArgs );

	return r;
}

#endif

#if AXLOG_CXX_ENABLED
namespace ax
{

	namespace detail
	{

		class CReportProxy
		{
		public:
			inline CReportProxy( axlog_u16_t flags )
			: m_info()
			, m_flags( flags )
			{
				resetInfo();
			}
			inline CReportProxy( axlog_u16_t flags, const axlog_lineinfo_t &info )
			: m_info( info )
			, m_flags( flags )
			{
			}
			inline CReportProxy( axlog_u16_t flags, const axlog_str_t &file, axlog_u32_t line, axlog_u32_t column, const axlog_str_t &func )
			: m_info()
			, m_flags( flags )
			{
				resetInfo();

				m_info.file.s = file.s;
				m_info.file.e = file.e;
				m_info.line   = line;
				m_info.column = column;
				m_info.func.s = func.s;
				m_info.func.e = func.e;
			}
			inline ~CReportProxy()
			{
			}

			inline CReportProxy operator[]( axlog_u16_t facility ) const
			{
				return CReportProxy( ( m_flags & ~AXLOG_FACILITY_MASK ) | facility, m_info );
			}
			inline CReportProxy operator()( const axlog_str_t &file ) const
			{
				return CReportProxy( m_flags, file, m_info.line, m_info.column, m_info.func );
			}
			inline CReportProxy operator()( const axlog_str_t &file, axlog_u32_t line ) const
			{
				return CReportProxy( m_flags, file, line, 0, m_info.func );
			}
			inline CReportProxy operator()( const axlog_str_t &file, axlog_u32_t line, axlog_u32_t column ) const
			{
				return CReportProxy( m_flags, file, line, column, m_info.func );
			}
			inline CReportProxy operator()( const axlog_str_t &file, axlog_u32_t line, const axlog_str_t &function ) const
			{
				return CReportProxy( m_flags, file, line, 0, function );
			}
			inline CReportProxy operator()( const axlog_str_t &file, axlog_u32_t line, axlog_u32_t column, const axlog_str_t &function ) const
			{
				return CReportProxy( m_flags, file, line, column, function );
			}

			inline const CReportProxy &submit( const axlog_str_t &message ) const
			{
				axlog_report_t rep;

				rep.flags = m_flags;
				rep.mod.s = ( const char * )0;
				rep.mod.e = ( const char * )0;
				rep.msg.s = message.s;
				rep.msg.e = message.e;
				rep.info  = m_info;

				axlog_submit_report( &rep );
				return *this;
			}

			inline const CReportProxy &operator<<( const axlog_str_t &message ) const
			{
				return submit( message );
			}
			inline CReportProxy &operator+=( const axlog_str_t &message )
			{
				submit( message );
				return *this;
			}

		private:
			axlog_lineinfo_t m_info;
			axlog_u16_t      m_flags;

			inline void resetInfo()
			{
				m_info.file.s = ( const char * )0;
				m_info.file.e = ( const char * )0;
				m_info.line   = 0;
				m_info.column = 0;
				m_info.func.s = ( const char * )0;
				m_info.func.e = ( const char * )0;
				m_info.expr.s = ( const char * )0;
				m_info.expr.e = ( const char * )0;

				m_info.range.linetext.s = ( const char * )0;
				m_info.range.linetext.e = ( const char * )0;
				m_info.range.start      = 0;
				m_info.range.count      = 0;
				m_info.range.point      = 0;
			}
		};

	}

	static detail::CReportProxy debugLog   ( axlogp_debug    | AXLOG_DEFAULT_FACILITY );
	static detail::CReportProxy infoLog    ( axlogp_info     | AXLOG_DEFAULT_FACILITY );
	static detail::CReportProxy noteLog    ( axlogp_notice   | AXLOG_DEFAULT_FACILITY );
	static detail::CReportProxy warningLog ( axlogp_warning  | AXLOG_DEFAULT_FACILITY );
	static detail::CReportProxy errorLog   ( axlogp_error    | AXLOG_DEFAULT_FACILITY );
	static detail::CReportProxy criticalLog( axlogp_critical | AXLOG_DEFAULT_FACILITY );
	static detail::CReportProxy alertLog   ( axlogp_alert    | AXLOG_DEFAULT_FACILITY );
	static detail::CReportProxy panicLog   ( axlogp_panic    | AXLOG_DEFAULT_FACILITY );

#define AX_DEBUG_LOG    ax::debugLog   (__FILE__,__LINE__,AXLOG_FUNCTION)
#define AX_INFO_LOG     ax::infoLog    (__FILE__,__LINE__,AXLOG_FUNCTION)
#define AX_NOTE_LOG     ax::noteLog    (__FILE__,__LINE__,AXLOG_FUNCTION)
#define AX_WARNING_LOG  ax::warningLog (__FILE__,__LINE__,AXLOG_FUNCTION)
#define AX_ERROR_LOG    ax::errorLog   (__FILE__,__LINE__,AXLOG_FUNCTION)
#define AX_CRITICAL_LOG ax::criticalLog(__FILE__,__LINE__,AXLOG_FUNCTION)
#define AX_ALERT_LOG    ax::alertLog   (__FILE__,__LINE__,AXLOG_FUNCTION)
#define AX_PANIC_LOG    ax::panicLog   (__FILE__,__LINE__,AXLOG_FUNCTION)

#define AXLOG__DO_REPORT_F( Flags_, File_, Line_, Fmt_ )\
	axlog_report_t rep;\
	char           buf[ AXLOG_FMTBUF_SIZE ];\
	\
	AXLOG_VA_T args;\
	AXLOG_VA_S( args, Fmt_ );\
	axlog_init_reportexv( buf, sizeof(buf), &rep,\
		((Flags_)&~AXLOG_FACILITY_MASK)|AXLOG_DEFAULT_FACILITY,\
		(File_), (Line_), (const axlog_str_t *)0, (const axlog_str_t *)0,\
		(Fmt_), args );\
	AXLOG_VA_E( args );\
	\
	axlog_submit_report( &rep )

	// [axlogp_debug] Report debugging information
	inline void debugf( const axlog_str_t &file, axlog_u32_t line, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_debug, &file, line, pszFmt );
	}
	// [axlogp_info] Report status/info
	inline void infof( const axlog_str_t &file, axlog_u32_t line, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_info, &file, line, pszFmt );
	}
	// [axlogp_notice] Report note
	inline void notef( const axlog_str_t &file, axlog_u32_t line, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_notice, &file, line, pszFmt );
	}
	// [axlogp_warning] Report a warning
	inline void warnf( const axlog_str_t &file, axlog_u32_t line, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_warning, &file, line, pszFmt );
	}
	// [axlogp_error] Report an error that can be recovered from
	inline void errorf( const axlog_str_t &file, axlog_u32_t line, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_error, &file, line, pszFmt );
	}
	// [axlogp_critical] Report an error that cannot be recovered from within their context
	inline void criticalf( const axlog_str_t &file, axlog_u32_t line, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_critical, &file, line, pszFmt );
	}
	// [axlogp_panic] Panic
	inline void panicf( const axlog_str_t &file, axlog_u32_t line, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_panic, &file, line, pszFmt );
	}

	// [axlogp_debug] Report debugging information
	inline void debugf( const axlog_str_t &file, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_debug, &file, 0, pszFmt );
	}
	// [axlogp_info] Report status
	inline void infof( const axlog_str_t &file, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_info, &file, 0, pszFmt );
	}
	// [axlogp_notice] Report note
	inline void notef( const axlog_str_t &file, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_notice, &file, 0, pszFmt );
	}
	// [axlogp_warning] Report a warning
	inline void warnf( const axlog_str_t &file, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_warning, &file, 0, pszFmt );
	}
	// [axlogp_error] Report an error that can be recovered from
	inline void errorf( const axlog_str_t &file, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_error, &file, 0, pszFmt );
	}
	// [axlogp_critical] Report an error that cannot be recovered from within their context
	inline void criticalf( const axlog_str_t &file, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_critical, &file, 0, pszFmt );
	}
	// [axlogp_panic] Report a panic
	inline void panicf( const axlog_str_t &file, const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_panic, &file, 0, pszFmt );
	}

	// [axlogp_debug] Report debugging information
	inline void basicDebugf( const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_debug, (const axlog_str_t*)0, 0, pszFmt );
	}
	// [axlogp_info] Report status
	inline void basicInfof( const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_info, (const axlog_str_t*)0, 0, pszFmt );
	}
	// [axlogp_notice] Report note
	inline void basicNotef( const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_notice, (const axlog_str_t*)0, 0, pszFmt );
	}
	// [axlogp_warning] Report a warning
	inline void basicWarnf( const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_warning, (const axlog_str_t*)0, 0, pszFmt );
	}
	// [axlogp_error] Report an error that can be recovered from
	inline void basicErrorf( const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_error, (const axlog_str_t*)0, 0, pszFmt );
	}
	// [axlogp_critical] Report an error that cannot be recovered from within their context
	inline void basicCriticalf( const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_critical, (const axlog_str_t*)0, 0, pszFmt );
	}
	// [axlogp_panic] Report a panic
	inline void basicPanicf( const char *pszFmt, ... )
	{
		AXLOG__DO_REPORT_F( axlogp_panic, (const axlog_str_t*)0, 0, pszFmt );
	}

#undef AXLOG__DO_REPORT_F

# ifndef AXLOG_TRACE_ENABLED
#  if defined(_DEBUG) || ( defined(AX_DEBUG_ENABLED) && AX_DEBUG_ENABLED==1 )
#   define AXLOG_TRACE_ENABLED 1
#  else
#   define AXLOG_TRACE_ENABLED 0
#  endif
# endif

# if AXLOG_TRACE_ENABLED
#  define AX_TRACE(...)\
	axlog_submitf\
	(\
		axlogp_debug | axlogc_trace | AXLOG_DEFAULT_FACILITY,\
		__FILE__, __LINE__, AXLOG_FUNCTION,\
		__VA_ARGS__\
	)
# else
#  define AX_TRACE(...) ((void)0)
# endif

# define AXLOG_CHECK(Expr_)\
	( !!(Expr_) || ((\
		axlog_submitexprf\
		(\
			axlogp_error | axlogc_check | AXLOG_DEFAULT_FACILITY,\
			__FILE__, __LINE__, AXLOG_FUNCTION,\
			( #Expr_ ),\
			"Runtime check failed"\
		)\
	), 0))
# define AXLOG_CHECK_NOT_NULL(Expr_)\
	( !!(Expr_) || ((\
		axlog_submitexprf\
		(\
			axlogp_error | axlogc_checkptr | AXLOG_DEFAULT_FACILITY,\
			__FILE__, __LINE__, AXLOG_FUNCTION,\
			( "(" #Expr_ ") == NULL" ),\
			"Runtime check failed; pointer is null"\
		)\
	), 0))

}
#endif

#endif /* INCGUARD_AX_LOGGER_H_ */
