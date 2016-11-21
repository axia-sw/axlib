/*

	ax_fiber - public domain
	Last update: 2015-10-01 Aaron Miller


	This library provides fiber support (using the operating system's native
	capabilities where applicable).


	USAGE
	=====

	Define AXFIBER_IMPLEMENTATION in exactly one source file that includes this
	header, before including it.

	The following don't need to be defined, as default definitions will be
	provided, but can be defined if you want to alter default functionality
	without modifying this file.

	AXFIBER_FUNC and AXFIBER_CALL control the function declaration and calling
	conventions. Ensure that all source files including this use the same
	definitions for these. (That can be done in your Makefile or project
	settings.)


	REPLACE FIBER ALLOCATORS
	========================

	You can specify your own allocator to use with this library by defining the
	axfi_alloc and axfi_free macros. By default they are defined to the
	standard C library's malloc() and free().

	Custom allocations are presently only used to acquire stack space on UNIX
	systems.


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

#ifndef INCGUARD_AX_FIBER_H_
#define INCGUARD_AX_FIBER_H_

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

#ifdef AXFIBER_IMPLEMENTATION
# define AXFIBER_IMPLEMENT          1
#else
# define AXFIBER_IMPLEMENT          0
#endif

#ifndef AXFIBER_FUNC
# ifdef AX_FUNC
#  define AXFIBER_FUNC              AX_FUNC
# else
#  define AXFIBER_FUNC              extern
# endif
#endif
#ifndef AXFIBER_CALL
# ifdef AX_CALL
#  define AXFIBER_CALL              AX_CALL
# else
#  define AXFIBER_CALL
# endif
#endif

#ifndef AXFIBER_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXFIBER_CXX_ENABLED       AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXFIBER_CXX_ENABLED       1
# else
#  define AXFIBER_CXX_ENABLED       0
# endif
#endif

#ifndef axfi_alloc
# include <stdlib.h>
# define axfi_alloc(N_)             (malloc((N_)))
# define axfi_free(P_)              (free((P_)))
#endif

#ifdef AX_TYPES_DEFINED
typedef ax_uptr_t                   axfi_size_t;
#else
# include <stddef.h>
typedef size_t                      axfi_size_t;
#endif

#ifndef __cplusplus
# undef  AXFIBER_CXX_ENABLED
# define AXFIBER_CXX_ENABLED        0
#endif

#ifndef AXFIBER_OS_DEFINED
# ifdef INCGUARD_AX_PLATFORM_H_
#  define AXFIBER_OS_WINDOWS        AX_OS_WINDOWS
#  define AXFIBER_OS_LINUX          AX_OS_LINUX
#  define AXFIBER_OS_MACOSX         AX_OS_MACOSX
# else
#  define AXFIBER_OS_WINDOWS        0
#  define AXFIBER_OS_LINUX          0
#  define AXFIBER_OS_MACOSX         0
#  if defined( _WIN32 )
#   undef AXFIBER_OS_WINDOWS
#   define AXFIBER_OS_WINDOWS       1
#  elif defined( linux ) || defined( __linux ) || defined( __linux__ )
#   undef AXFIBER_OS_LINUX
#   define AXFIBER_OS_LINUX         1
#  elif defined( __APPLE__ )
#   undef AXFIBER_OS_MACOSX
#   define AXFIBER_OS_MACOSX        1
#  endif
# endif
# if !(AXFIBER_OS_WINDOWS|AXFIBER_OS_LINUX|AXFIBER_OS_MACOSX)
#  error Unknown operating system
# endif
# define AXFIBER_OS_DEFINED         1
#endif

#ifndef AXFIBER_IMPL_DEFINED
# define AXFIBER_IMPL_WINDOWS       0
# define AXFIBER_IMPL_UNIX          0
# if AXFIBER_OS_WINDOWS
#  undef AXFIBER_IMPL_WINDOWS
#  define AXFIBER_IMPL_WINDOWS      1
# elif AXFIBER_OS_LINUX || AXIFIBER_OS_MACOSX
#  undef AXFIBER_IMPL_UNIX
#  define AXFIBER_IMPL_UNIX         1
# endif
# if !(AXFIBER_IMPL_WINDOWS|AXFIBER_IMPL_UNIX)
#  error Unknown fiber implementation
# endif
# define AXFIBER_IMPL_DEFINED       1
#endif

#if AXFIBER_IMPL_WINDOWS
# undef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN        1
# if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0600
#  undef _WIN32_WINNT
#  define _WIN32_WINNT              0x0603
# endif
# include <Windows.h>
# undef min
# undef max
# undef Yield
# undef AddJob
#elif AXFIBER_IMPL_UNIX
# include <signal.h>
# include <ucontext.h>
# include <stdlib.h>
#endif

#ifndef AXFIBER_OS_CALL
# if AXFIBER_IMPL_WINDOWS
#  define AXFIBER_OS_CALL           __stdcall
# elif AXFIBER_IMPL_UNIX
#  define AXFIBER_OS_CALL
# else
#  error Could not determine how to define AXFIBER_OS_CALL
# endif
#endif

#ifndef AXFIBER_OS_FUNC
# define AXFIBER_OS_FUNC
#endif
#ifndef AXFIBER_OS_ATTRIBS
# define AXFIBER_OS_ATTRIBS
#endif

typedef void( AXFIBER_OS_CALL *axfi_fn_fiber_t )( void * );
#define AXFIBER_ENTRY_POINT(FuncName_,ParmName_)\
	AXFIBER_OS_FUNC void AXFIBER_OS_CALL FuncName_( void *ParmName_ ) AXFIBER_OS_ATTRIBS

typedef struct axfiber_s
{
#if AXFIBER_IMPL_WINDOWS
	LPVOID                          pFiber;
#elif AXFIBER_IMPL_UNIX
	ucontext_t                      Context;
	void *                          pStack;
	void *                          pUserData;
#else
# error Could not determine how to define axfiber_s
#endif
} axfiber_t;

#if AXFIBER_IMPLEMENT
# if AXFIBER_IMPL_WINDOWS
typedef struct axfi__tls_s
{
	axfiber_t *                     pCurrent;
	LONG                            cRefs;
} axfi__tls_t;
static DWORD                        axfi__g_dwTLS = TLS_OUT_OF_INDEXES;

static axfi__tls_t *axfi__tls( void )
{
	if( axfi__g_dwTLS == TLS_OUT_OF_INDEXES ) {
		axfi__tls_t *p;

		axfi__g_dwTLS = TlsAlloc();
		if( axfi__g_dwTLS == TLS_OUT_OF_INDEXES ) {
			/* TODO: Error invocation */
			return ( axfi__tls_t * )0;
		}

		p = ( axfi__tls_t * )axfi_alloc( sizeof( *p ) );
		if( !p ) {
			TlsFree( axfi__g_dwTLS );
			axfi__g_dwTLS = TLS_OUT_OF_INDEXES;
			/* TODO: Error invocation */
			return ( axfi__tls_t * )0;
		}

		p->pCurrent = ( axfiber_t * )0;
		p->cRefs = 0;

		TlsSetValue( axfi__g_dwTLS, ( void * )p );
		return p;
	}

	return ( axfi__tls_t * )TlsGetValue( axfi__g_dwTLS );
}
static int axfi__tls_grab( void )
{
	axfi__tls_t *p;

	p = axfi__tls();
	if( !p ) {
		return 0;
	}

	InterlockedIncrement( &p->cRefs );
	return 1;
}
static void axfi__tls_drop( void )
{
	axfi__tls_t *p;

	p = axfi__tls();
	if( !p ) {
		return;
	}

	if( InterlockedDecrement( &p->cRefs ) == 0 ) {
		TlsFree( axfi__g_dwTLS );
		axfi__g_dwTLS = TLS_OUT_OF_INDEXES;
	}
}
# elif AXFIBER_IMPL_UNIX
static __thread axfiber_t *         axfi__g_pCurrentFiber = ( axfiber_t * )0;

typedef void( *axfi__fn_context_routine_t )();
# endif

static void axfi__set_current( axfiber_t *pInFiber )
{
# if AXFIBER_IMPL_WINDOWS
	axfi__tls_t *p;

	p = axfi__tls();
	if( !p ) {
		return;
	}

	p->pCurrent = pInFiber;
# elif AXFIBER_IMPL_UNIX
	axfi__g_pCurrentFiber = pInFiber;
# else
#  error Could not determine how to implement axfi__set_current()
# endif
}
static axfiber_t *axfi__get_current( void )
{
# if AXFIBER_IMPL_WINDOWS
	axfi__tls_t *p;

	p = axfi__tls();
	if( !p ) {
		return ( axfiber_t * )0;
	}

	return p->pCurrent;
# elif AXFIBER_IMPL_UNIX
	return axfi__g_pCurrentFiber;
# else
#  error Could not determine how to implement axfi__get_current()
# endif
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

AXFIBER_FUNC axfiber_t *AXFIBER_CALL axfi_thread_to_fiber( axfiber_t *pDstFiber, void *pUserData )
#if AXFIBER_IMPLEMENT
{
# if AXFIBER_IMPL_WINDOWS
	if( !axfi__tls_grab() ) {
		return ( axfiber_t * )0;
	}

	pDstFiber->pFiber = ConvertThreadToFiber( pUserData );
	if( !pDstFiber->pFiber ) {
		axfi__tls_drop();
		return ( axfiber_t * )0;
	}

	axfi__set_current( pDstFiber );
	return pDstFiber;
# elif AXFIBER_IMPL_UNIX
	pDstFiber->pStack = ( void * )0;
	pDstFiber->pUserData = pUserData;

	if( getcontext( &pDstFiber->Context ) != 0 ) {
		return ( axfiber_t * )0;
	}

	axfi__set_current( pDstFiber );
	return pDstFiber;
# else
#  error Could not determine how to implement axfi_thread_to_fiber()
# endif
}
#else
;
#endif
AXFIBER_FUNC void AXFIBER_CALL axfi_fiber_to_thread( void )
#if AXFIBER_IMPLEMENT
{
# if AXFIBER_IMPL_WINDOWS
	axfi__set_current( ( axfiber_t * )0 );
	ConvertFiberToThread();
	axfi__tls_drop();
# elif AXFIBER_IMPL_UNIX
	axfi__set_current( ( axfiber_t * )0 );
	/* no real point here */
# else
#  error Could not determine how to implement axfi_fiber_to_thread()
# endif
}
#else
;
#endif

AXFIBER_FUNC axfiber_t *AXFIBER_CALL axfi_init( axfiber_t *pDstFiber, axfi_size_t cStackBytes, axfi_fn_fiber_t pfnRoutine, void *pUserData )
#if AXFIBER_IMPLEMENT
{
# if AXFIBER_IMPL_WINDOWS
	pDstFiber->pFiber = CreateFiber( ( SIZE_T )cStackBytes, pfnRoutine, pUserData );
	if( !pDstFiber->pFiber ) {
		return ( axfiber_t * )0;
	}

	return pDstFiber;
# elif AXFIBER_IMPL_UNIX
	pDstFiber->pStack = axfi_alloc( cStackBytes ? cStackBytes : 1024*1024 );
	if( !pDstFiber->pStack ) {
		return ( axfiber_t * )0;
	}

	if( getcontext( &pDstFiber->Context ) != 0 ) {
		axfi_free( pDstFiber->pStack );
		pDstFiber->pStack = ( void * )0;
		return ( axfiber_t * )0;
	}

	pDstFiber->Context.uc_link = 0;
	pDstFiber->Context.uc_stack.ss_sp = pDstFiber->pStack;
	pDstFiber->Context.uc_stack.ss_size = cStackBytes;
	pDstFiber->Context.uc_stack.ss_flags = 0;

	pDstFiber->pUserData = pUserData;

	makecontext( &pDstFiber->Context, ( axfi__fn_context_routine_t )pfnRoutine, 1, pUserData );

	return pDstFiber;
# else
#  error Could not determine how to implement axfi_init()
# endif
}
#else
;
#endif
AXFIBER_FUNC axfiber_t *AXFIBER_CALL axfi_fini( axfiber_t *pFiber )
#if AXFIBER_IMPLEMENT
{
# if AXFIBER_IMPL_WINDOWS
	if( !pFiber ) {
		return ( axfiber_t * )0;
	}

	DeleteFiber( pFiber->pFiber );
	return ( axfiber_t * )0;
# elif AXFIBER_IMPL_UNIX
	if( !pFiber ) {
		return ( axfiber_t * )0;
	}

	axfi_free( pFiber->pStack );
	return ( axfiber_t * )0;
# else
#  error Could not determine how to implement axfi_fini()
# endif
}
#else
;
#endif

AXFIBER_FUNC void AXFIBER_CALL axfi_switch( axfiber_t *pFiber )
#if AXFIBER_IMPLEMENT
{
# if AXFIBER_IMPL_WINDOWS
	axfi__set_current( pFiber );
	SwitchToFiber( pFiber->pFiber );
# elif AXFIBER_IMPL_UNIX
	axfiber_t *pCurrent;

	pCurrent = axfi__get_current();
	if( !pCurrent ) {
		/* TODO: Error invocation (axfi_thread_to_fiber() not called, probably) */
		return;
	}

	axfi__set_current( pFiber );
	swapcontext( &pCurrent->Context, &pFiber->Context );
# else
#  error Could not determine how to implement axfi_switch()
# endif
}
#else
;
#endif

AXFIBER_FUNC void AXFIBER_CALL axfi_set_data( void *pUserData )
#if AXFIBER_IMPLEMENT
{
# if AXFIBER_IMPL_WINDOWS
	axfiber_t *pCurrent;

	pCurrent = axfi__get_current();
	if( !pCurrent ) {
		/* TODO: Error invocation (axfi_thread_to_fiber() not called, probably) */
		return;
	}

	*((void**)pCurrent->pFiber) = pUserData;
# elif AXFIBER_IMPL_UNIX
	axfiber_t *pCurrent;

	pCurrent = axfi__get_current();
	if( !pCurrent ) {
		/* TODO: Error invocation (axfi_thread_to_fiber() not called, probably) */
		return;
	}

	pCurrent->pUserData = pUserData;
# else
#  error Could not determine how to implement axfi_set_data()
# endif
}
#else
;
#endif
AXFIBER_FUNC void *AXFIBER_CALL axfi_get_data( void )
#if AXFIBER_IMPLEMENT
{
# if AXFIBER_IMPL_WINDOWS
	return GetFiberData();
# elif AXFIBER_IMPL_UNIX
	axfiber_t *pCurrent;

	pCurrent = axfi__get_current();
	if( !pCurrent ) {
		/* TODO: Error invocation (axfi_thread_to_fiber() not called, probably) */
		return ( void * )0;
	}

	return pCurrent->pUserData;
# else
#  error Could not determine how to implement axfi_get_data()
# endif
}
#else
;
#endif

AXFIBER_FUNC axfiber_t *AXFIBER_CALL axfi_get_current( void )
#if AXFIBER_IMPLEMENT
{
	return axfi__get_current();
}
#else
;
#endif

#ifdef __cplusplus
}
#endif

#endif
