/*

	ax_assert - public domain
	Last update: 2015-10-10 Aaron Miller


	This library provides an alternate and more flexible assert interface than
	the standard C library's.


	INTERACTIONS
	============

	ax_platform will be used if it's available, or has been included prior to
	this header.

	ax_types will be used if it's available, or has been included prior to this
	header.

	ax_printf will be used if it's available, or has been included prior to this
	header.

	ax_logger will be used if it's available, or has been included prior to this
	header.

	To prevent automatic inclusion on compilers with the `__has_include` macro,
	define AX_NO_INCLUDES prior to including this header.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_ASSERT_H_
#define INCGUARD_AX_ASSERT_H_

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
# if __has_include( "ax_printf.h" )
#  include "ax_printf.h"
# endif
# if __has_include( "ax_logger.h" )
#  include "ax_logger.h"
# endif
#endif

#ifdef AXASSERT_IMPLEMENTATION
# define AXASSERT_IMPLEMENT         1
#else
# define AXASSERT_IMPLEMENT         0
#endif

#ifndef AXASSERT_FUNC
# ifdef AX_FUNC
#  define AXASSERT_FUNC             AX_FUNC
# else
#  define AXASSERT_FUNC             extern
# endif
#endif
#ifndef AXASSERT_CALL
# ifdef AX_CALL
#  define AXASSERT_CALL             AX_CALL
# else
#  define AXASSERT_CALL
# endif
#endif

#ifndef AXASSERT_NORETURN
# ifdef AX_NORETURN
#  define AXASSERT_NORETURN         AX_NORETURN
# else
#  define AXASSERT_NORETURN
# endif
#endif

#ifndef AXASSERT_PRETTY_FUNCTION
# ifdef AX_PRETTY_FUNCTION
#  define AXASSERT_PRETTY_FUNCTION  AX_PRETTY_FUNCTION
# elif defined( _MSC_VER )
#  define AXASSERT_PRETTY_FUNCTION  __FUNCTION__
# elif defined( __GNUC__ ) || defined( __clang__ )
#  define AXASSERT_PRETTY_FUNCTION  __func__
# else
#  define AXASSERT_PRETTY_FUNCTION  ((const char *)0)
# endif
#endif

#ifndef AXASSERT_DEBUG_ENABLED
# ifdef AX_DEBUG_ENABLED
#  define AXASSERT_DEBUG_ENABLED    AX_DEBUG_ENABLED
# elif defined( _DEBUG ) || defined( DEBUG ) || defined( __DEBUG__ )
#  define AXASSERT_DEBUG_ENABLED    1
# else
#  define AXASSERT_DEBUG_ENABLED    0
# endif
#endif

#ifndef AXASSERT_TEST_ENABLED
# ifdef AX_TEST_ENABLED
#  define AXASSERT_TEST_ENABLED     AX_TEST_ENABLED
# else
#  define AXASSERT_TEST_ENABLED     0
# endif
#endif

#ifndef AX_ASSERT_ENABLED
# if AXASSERT_DEBUG_ENABLED || AXASSERT_TEST_ENABLED
#  define AX_ASSERT_ENABLED         1
# else
#  define AX_ASSERT_ENABLED         0
# endif
#endif

#ifndef AXASSERT_HAS_MACROVARARGS
# if !defined(_MSC_VER) || _MSC_VER>=1600
#  define AXASSERT_HAS_MACROVARARGS 1
# else
#  define AXASSERT_HAS_MACROVARARGS 0
# endif
#endif

#ifndef AXASSERT_OS_DEFINED
# ifdef INCGUARD_AX_PLATFORM_H_
#  define AXASSERT_OS_WINDOWS       AX_OS_WINDOWS
#  define AXASSERT_OS_LINUX         AX_OS_LINUX
#  define AXASSERT_OS_MACOSX        AX_OS_MACOSX
# else
#  define AXASSERT_OS_WINDOWS       0
#  define AXASSERT_OS_LINUX         0
#  define AXASSERT_OS_MACOSX        0
#  if defined( _WIN32 )
#   undef AXASSERT_OS_WINDOWS
#   define AXASSERT_OS_WINDOWS      1
#  elif defined( linux ) || defined( __linux ) || defined( __linux__ )
#   undef AXASSERT_OS_LINUX
#   define AXASSERT_OS_LINUX        1
#  elif defined( __APPLE__ )
#   undef AXASSERT_OS_MACOSX
#   define AXASSERT_OS_MACOSX       1
#  endif
# endif
# if !(AXASSERT_OS_WINDOWS|AXASSERT_OS_LINUX|AXASSERT_OS_MACOSX)
#  error Unknown operating system
# endif
# define AXASSERT_OS_DEFINED        1
#endif

#ifndef AXASSERT_BREAKPOINT
# if defined(EA_DEBUG_BREAK) && defined(EA_DEBUG_BREAK_DEFINED)
#  define AXASSERT_BREAKPOINT()     EA_DEBUG_BREAK()
# elif defined( _MSC_VER )
#  define AXASSERT_BREAKPOINT()     __debugbreak()
# elif ( defined(__GNUC__) || defined(__clang__) ) && ( defined(__x86_64__) || defined(__amd64__) )
#  define AXASSERT_BREAKPOINT()     __asm__( "int $3" )
# elif ( defined(__GNUC__) || defined(__clang__) ) && ( defined(__powerpc__) || defined(__arm__) || defined(__aarch64__) )
#  define AXASSERT_BREAKPOINT()     __asm__( "trap" )
# else
#  define AXASSERT_BREAKPOINT()     __builtin_trap()
# endif
#endif

typedef enum axassert_type_e
{
	axassert_ty_assert,
	axassert_ty_verify,
	axassert_ty_expect
} axassert_type_t;
typedef enum axassert_cause_e
{
	axassert_cause_generic,
	axassert_cause_invalid,
	axassert_cause_nomemory
} axassert_cause_t;

typedef struct axassert_info_s
{
	axassert_type_t  type;
	axassert_cause_t cause;
	const char *     file;
	unsigned         line;
	const char *     func;
	const char *     expr;
	const char *     msg;
	unsigned         facility;
} axassert_info_t;

typedef void( AXASSERT_CALL *axassert_fn_handler_t )( axassert_info_t );

#if AXASSERT_IMPLEMENT
# include <stdio.h> /*default output*/
# include <stdlib.h> /*exit()*/
# if AXASSERT_OS_WINDOWS
#  undef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN       1
#  if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0600
#   undef _WIN32_WINNT
#   define _WIN32_WINNT             0x0603
#  endif
#  include <Windows.h>
#  undef min
#  undef max
#  undef Yield
#  undef AddJob
#  include <string.h> /*strlen()*/
# endif
static axassert_fn_handler_t axassert__g_routine = ( axassert_fn_handler_t )0;
#endif

#ifdef __cplusplus
extern "C" {
#endif

AXASSERT_FUNC axassert_fn_handler_t AXASSERT_CALL axassert_set( axassert_fn_handler_t routine )
#if AXASSERT_IMPLEMENT
{
	axassert_fn_handler_t old;

	old = axassert__g_routine;
	axassert__g_routine = routine;

	return old;
}
#else
;
#endif
AXASSERT_FUNC axassert_fn_handler_t AXASSERT_CALL axassert_get( void )
#if AXASSERT_IMPLEMENT
{
	return axassert__g_routine;
}
#else
;
#endif

AXASSERT_FUNC int AXASSERT_CALL axassert_is_developer( void )
#if AXASSERT_IMPLEMENT
{
	return AXASSERT_DEBUG_ENABLED || AXASSERT_TEST_ENABLED;
}
#else
;
#endif
AXASSERT_FUNC int AXASSERT_CALL axassert_is_enabled( void )
#if AXASSERT_IMPLEMENT
{
# if AX_ASSERT_ENABLED
	return 1;
# else
	return 0;
# endif
}
#else
;
#endif

AXASSERT_FUNC int AXASSERT_CALL axassert_can_debug_break( void )
#if AXASSERT_IMPLEMENT
{
# if AXASSERT_OS_WINDOWS
#  if AXASSERT_DEBUG_ENABLED || AXASSERT_TEST_ENABLED
	return IsDebuggerPresent() != 0;
#  else
	return 0;
#  endif
# else
	return axassert_is_developer();
# endif
}
#else
;
#endif

#if AXASSERT_IMPLEMENT
static const char *axassert__title1( axassert_type_t type )
{
	switch( type ) {
	case axassert_ty_assert: return "assert error";
	case axassert_ty_verify: return "verify error";
	case axassert_ty_expect: return "expect error";
	}

	return "(unknown)";
}
static const char *axassert__title2( axassert_type_t type )
{
	switch( type ) {
	case axassert_ty_assert: return "Assert Error";
	case axassert_ty_verify: return "Verify Error";
	case axassert_ty_expect: return "Expect Error";
	}

	return "(Unknown Error)";
}
static const char *axassert__title3( axassert_type_t type )
{
	switch( type ) {
	case axassert_ty_assert: return "Assert Error - Debug?";
	case axassert_ty_verify: return "Verify Error - Debug?";
	case axassert_ty_expect: return "Expect Error - Debug?";
	}

	return "(Unknown Error - Debug?)";
}
#if AXASSERT_OS_WINDOWS
static void axassert__copy_to_clipboard( const char *pszText )
{
	HGLOBAL hClipMem;
	size_t cLen;
	char *pszClipBuf;

	if( !pszText || !OpenClipboard( NULL ) ) {
		return;
	}

	/*
	 *	Setting the window to NULL in OpenClipboard() will cause
	 *	EmptyClipboard() to associate the clipboard with NULL which will then
	 *	cause SetClipboardData() to fail
	`*/
# if 0
	EmptyClipboard();
# endif

	cLen = strlen( pszText );

	hClipMem = GlobalAlloc( GMEM_DDESHARE, cLen + 1 );
	if( !hClipMem ) {
		CloseClipboard();
		return;
	}

	pszClipBuf = ( char * )GlobalLock( hClipMem );
	if( !pszClipBuf ) {
		GlobalFree( hClipMem );
		CloseClipboard();
		return;
	}

	memcpy( pszClipBuf, pszText, cLen );
	pszClipBuf[ cLen ] = '\0';

	GlobalUnlock( hClipMem );

	SetClipboardData( CF_TEXT, hClipMem );
	CloseClipboard();
}
#endif
static void AXASSERT_CALL axassert__default( axassert_info_t info )
{
#if !defined(AXASSERT_NO_AXPF) && defined(INCGUARD_AX_PRINTF_H_)
# define axassert__spf  axspf
# define axassert__errf axerrf
# define axassert__err
#else
# ifdef _MSC_VER
#  define axassert__spf sprintf_s
# else
#  define axassert__spf snprintf
# endif
# define axassert__errf fprintf
# define axassert__err  stderr,
#endif
	const char *title1, *title2, *title3;
# if AXASSERT_OS_WINDOWS
	const char *wannadebug;
	int candebug;
# endif
	char buf[ 4096 ];
# ifdef INCGUARD_AX_LOGGER_H_
	axlog_report_t rep;
	unsigned logflags;
# endif
	int doerr;

	title1 = axassert__title1( info.type ); (void)title1;
	title2 = axassert__title2( info.type ); (void)title2;
	title3 = axassert__title3( info.type ); (void)title3;
	doerr  = 1;

# ifdef INCGUARD_AX_LOGGER_H_
	logflags = axlogf_sysinfo | ( info.facility & AXLOG_FACILITY_MASK );
	if( info.cause == axassert_cause_nomemory ) {
		logflags |= axlogc_nomem;
	} else if( info.cause == axassert_cause_invalid ) {
		logflags |= axlogc_nullptr;
	} else {
		logflags |= axlogc_check;
	}

	if( info.type == axassert_ty_verify ) {
		if( info.cause == axassert_cause_generic ) {
			logflags |= axlogp_alert;
		} else {
			logflags |= axlogp_critical;
		}
	} else {
		logflags |= axlogp_panic;
	}

	logflags |= axlogf_sysinfo;

	do {
		axlog_str_t file, func, expr, msg;

		file.s = info.file;
		file.e = ( const char * )0;

		func.s = info.func;
		func.e = ( const char * )0;

		expr.s = info.expr;
		expr.e = ( const char * )0;

		doerr = 0;
		if( AXLOG_SUCCEEDED( axlog_init_reportex( buf,sizeof(buf),&rep, logflags,
		&file, info.line, &func, &expr, "%s", info.msg ) ) ) {
			doerr = +AXLOG_FAILED( axlog_submit_report( &rep ) );
		}
	} while(0);

# if AXASSERT_OS_WINDOWS
	axassert__spf( buf, sizeof( buf ), "%s(%u): %s: in %s: %s (%s)", info.file, info.line, title1, info.func, info.msg, info.expr );
	buf[ sizeof( buf ) - 1 ] = '\0';

	axassert__copy_to_clipboard( buf );
# endif

	if( doerr ) {
#  if AXASSERT_OS_WINDOWS
		axassert__errf( axassert__err "%s\n", buf );
#  else
		axassert__errf( axassert__err "%s:%u: %s: in %s: %s (%s)\n",
			info.file, info.line, title1, info.func, info.msg, info.expr );
#  endif
# endif
	}

# if AXASSERT_OS_WINDOWS
	candebug = axassert_can_debug_break();
	wannadebug = candebug ? "\n\nWould you like to debug?" : "";

	axassert__spf( buf, sizeof( buf ), "File: %s\nLine: %u\nFunction: %s\nExpression: %s\n\n%s%s",
		info.file, info.line, info.func, info.expr, info.msg, wannadebug );
	buf[ sizeof( buf ) -1 ] = '\0';

	if( candebug ) {
		if( MessageBoxA( GetActiveWindow(), buf, title3, MB_ICONERROR | MB_YESNO ) == IDYES ) {
			DebugBreak();
		}
	} else {
#  ifdef _MSC_VER
#   pragma warning( push )
#   pragma warning( suppress: 6054 )
#  endif
		( void )MessageBoxA( GetActiveWindow(), buf, title2, MB_ICONERROR | MB_OK );
#  ifdef _MSC_VER
#   pragma warning( pop )
#  endif
	}
# else
	if( axassert_is_enabled() ) {
		AXASSERT_BREAKPOINT();
	}
# endif
}
#endif

AXASSERT_FUNC void AXASSERT_CALL axassert__handle
(
	axassert_type_t  type,
	axassert_cause_t cause,
	const char *     file,
	unsigned         line,
	const char *     func,
	const char *     expr,
	const char *     msg,
	unsigned         facility
)
#if AXASSERT_IMPLEMENT
{
	axassert_info_t info;

	info.type     = type;
	info.cause    = cause;
	info.file     = file;
	info.line     = line;
	info.func     = func;
	info.expr     = expr;
	info.msg      = msg;
	info.facility = facility;

	if( axassert__g_routine != ( axassert_fn_handler_t )0 ) {
		axassert__g_routine( info );
	} else {
		axassert__default( info );
	}
}
#else
;
#endif
AXASSERT_NORETURN
AXASSERT_FUNC void AXASSERT_CALL axassert__handle_noreturn
(
	axassert_type_t  type,
	axassert_cause_t cause,
	const char *     file,
	unsigned         line,
	const char *     func,
	const char *     expr,
	const char *     msg,
	unsigned         facility
)
#if AXASSERT_IMPLEMENT
{
	axassert__handle( type, cause, file, line, func, expr, msg, facility );
	exit( EXIT_FAILURE );
}
#else
;
#endif

#ifdef __cplusplus
}
#endif

#ifndef AXASSERT_FACILITY
# define AXASSERT_FACILITY AXLOG_DEFAULT_FACILITY
#endif

#define AXASSERT_LINEINFO\
	__FILE__, __LINE__, AXASSERT_PRETTY_FUNCTION

#define AX_ENABLED_ASSERT_MSG__( Cause_, Expr_, Msg_ )\
	( !(Expr_) && (axassert__handle_noreturn( axassert_ty_assert, (Cause_), AXASSERT_LINEINFO, #Expr_, (Msg_), AXASSERT_FACILITY ), 1) )
#define AX_ENABLED_VERIFY_MSG__( Cause_, Expr_, Msg_ )\
	( !(Expr_)  ? (axassert__handle( axassert_ty_verify, (Cause_), AXASSERT_LINEINFO, #Expr_, (Msg_), AXASSERT_FACILITY ), 0) : 1 )
#define AX_ENABLED_EXPECT_MSG__( Cause_, Expr_, Msg_ )\
	( !(Expr_) && (axassert__handle_noreturn( axassert_ty_expect, (Cause_), AXASSERT_LINEINFO, #Expr_, (Msg_), AXASSERT_FACILITY ), 0) )

#if defined( AX_STATIC_ANALYSIS ) && AX_STATIC_ANALYSIS
# define AX_DISABLED_ASSERT_MSG__( Cause_, Expr_, Msg_ )\
	AX_STATIC_ASSUME( Expr_ )
#else
# define AX_DISABLED_ASSERT_MSG__( Cause_, Expr_, Msg_ )\
	( ( void )0 )
#endif
#define AX_DISABLED_VERIFY_MSG__( Cause_, Expr_, Msg_ ) (Expr_)

#if AX_ASSERT_ENABLED
# define AX_ASSERT_MSG_X( Cause_, Expr_, Msg_ ) AX_ENABLED_ASSERT_MSG__( Cause_, Expr_, Msg_ )
# define AX_VERIFY_MSG_X( Cause_, Expr_, Msg_ ) AX_ENABLED_VERIFY_MSG__( Cause_, Expr_, Msg_ )
#else
# define AX_ASSERT_MSG_X( Cause_, Expr_, Msg_ ) AX_DISABLED_ASSERT_MSG__( Cause_, Expr_, Msg_ )
# define AX_VERIFY_MSG_X( Cause_, Expr_, Msg_ ) AX_DISABLED_VERIFY_MSG__( Cause_, Expr_, Msg_ )
#endif
#define AX_EXPECT_MSG_X( Cause_, Expr_, Msg_ ) AX_ENABLED_EXPECT_MSG__( Cause_, Expr_, Msg_ )

#if AX_ASSERT_ENABLED
# define AX_ASSERT_MSG( Expr_, Msg_ ) AX_ENABLED_ASSERT_MSG__( axassert_cause_generic, Expr_, Msg_ )
# define AX_VERIFY_MSG( Expr_, Msg_ ) AX_ENABLED_VERIFY_MSG__( axassert_cause_generic, Expr_, Msg_ )
#else
# define AX_ASSERT_MSG( Expr_, Msg_ ) AX_DISABLED_ASSERT_MSG__( axassert_cause_generic, Expr_, Msg_ )
# define AX_VERIFY_MSG( Expr_, Msg_ ) AX_DISABLED_VERIFY_MSG__( axassert_cause_generic, Expr_, Msg_ )
#endif
#define AX_EXPECT_MSG( Expr_, Msg_ ) AX_ENABLED_EXPECT_MSG__( axassert_cause_generic, Expr_, Msg_ )

#define AX_ASSERT( Expr_ ) AX_ASSERT_MSG( Expr_, "Assertion failed" )
#define AX_VERIFY( Expr_ ) AX_VERIFY_MSG( Expr_, "Verification failed" )
#define AX_EXPECT( Expr_ ) AX_EXPECT_MSG( Expr_, "Expectation failed" )

#define AX_ASSERT_NOT_NULL( Var_ ) AX_ASSERT_MSG_X( axassert_cause_invalid, !!( Var_ ), #Var_ " is NULL" )
#define AX_VERIFY_NOT_NULL( Var_ ) AX_VERIFY_MSG_X( axassert_cause_invalid, !!( Var_ ), #Var_ " is NULL" )
#define AX_EXPECT_NOT_NULL( Var_ ) AX_EXPECT_MSG_X( axassert_cause_invalid, !!( Var_ ), #Var_ " is NULL" )

#define AX_ASSERT_IS_NULL( Var_ ) AX_ASSERT_MSG_X( axassert_cause_invalid, !( Var_ ), #Var_ " is not NULL" )
#define AX_VERIFY_IS_NULL( Var_ ) AX_VERIFY_MSG_X( axassert_cause_invalid, !( Var_ ), #Var_ " is not NULL" )
#define AX_EXPECT_IS_NULL( Var_ ) AX_EXPECT_MSG_X( axassert_cause_invalid, !( Var_ ), #Var_ " is not NULL" )

#define AX_VERIFY_MEMORY( Var_ ) AX_VERIFY_MSG_X( axassert_cause_nomemory, !!( Var_ ), "Out of memory" )
#define AX_EXPECT_MEMORY( Var_ ) AX_EXPECT_MSG_X( axassert_cause_nomemory, !!( Var_ ), "Out of memory" )

#endif
