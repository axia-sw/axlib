/*

	ax_memory - public domain
	Last update: 2015-10-10 Aaron Miller


	This library provides memory management utilities and allocators for various
	purposes. For instance, an implementation of Naughty Dog's Tagged Heaps is
	provided.

	
	USAGE
	=====

	Define AXMM_IMPLEMENTATION in exactly one source file that includes this
	header, before including it.

	The following don't need to be defined, as default definitions will be
	provided, but can be defined if you want to alter default functionality
	without modifying this file.

	AXMM_FUNC and AXMM_CALL control the function declaration and calling
	conventions. Ensure that all source files including this use the same
	definitions for these. (That can be done in your Makefile or project
	settings.)


	CONFIGURATION MACROS
	====================

	Define any of these to either 1 or 0 prior to including this header, if you
	want to alter the default functionality.

		AXMM_CXX_ENABLED
		----------------
		Enables the C++ parts of this. All C++ code is in the `ax` namespace.


	REPLACE HEAP ALLOCATORS
	========================

	You can specify your own allocator to use with this library by defining the
	axmm_def_alloc and axmm_def_free macros. By default they are defined to the
	standard C library's `malloc()` and `free()`.


	INTERACTIONS
	============

	ax_platform will be used if it's available, or has been included prior to
	this header.

	ax_types will be used if it's available, or has been included prior to this
	header.

	ax_thread will be used if it's available, or has been included prior to this
	header.

	ax_logger will be used if it's available, or has been included prior to this
	header.

	To prevent automatic inclusion on compilers with the `__has_include` macro,
	define `AX_NO_INCLUDES` prior to including this header.


	THANK YOU!
	==========

	The following was helpful in determining some cross-platform crap:

		http://blog.nervus.org/managing-virtual-address-spaces-with-mmap/


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_MEMORY_H_
#define INCGUARD_AX_MEMORY_H_

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
# if __has_include( "ax_assert.h" )
#  include "ax_assert.h"
# endif
# if __has_include( "ax_thread.h" )
#  include "ax_thread.h"
# endif
#endif

#ifdef AXMM_IMPLEMENTATION
# define AXMM_IMPLEMENT             1
#else
# define AXMM_IMPLEMENT             0
#endif

#ifndef AXMM_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXMM_CXX_ENABLED          AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXMM_CXX_ENABLED          1
# else
#  define AXMM_CXX_ENABLED          0
# endif
#endif
#ifndef __cplusplus
# undef AXMM_CXX_ENABLED
# define AXMM_CXX_ENABLED           0
#endif

#ifndef AXMM__ENTER_C
# ifdef __cplusplus
#  define AXMM__ENTER_C             extern "C" {
# else
#  define AXMM__ENTER_C
# endif
#endif
#ifndef AXMM__LEAVE_C
# ifdef __cplusplus
#  define AXMM__LEAVE_C             }
# else
#  define AXMM__LEAVE_C
# endif
#endif

#ifndef AXMM_DEFPARM
# ifdef __cplusplus
#  define AXMM_DEFPARM(X_)          = (X_)
# else
#  define AXMM_DEFPARM(X_)
# endif
#endif

#ifndef AXMM_FUNC
# ifdef AX_FUNC
#  define AXMM_FUNC                 AX_FUNC
# else
#  define AXMM_FUNC                 extern
# endif
#endif
#ifndef AXMM_CALL
# ifdef AX_CALL
#  define AXMM_CALL                 AX_CALL
# else
#  define AXMM_CALL
# endif
#endif

#ifndef AXMM_THREADLOCAL
# if !defined( _MSC_VER ) /*|| _MSC_VER >= someversion*/
#  define AXMM_THREADLOCAL          __thread
# else
#  define AXMM_THREADLOCAL          __declspec(thread)
# endif
#endif

#ifndef AXMM_THREADSAFE
# define AXMM_THREADSAFE            1
#endif
#ifndef AXMM_THREADSAFE_LISTENERS
# define AXMM_THREADSAFE_LISTENERS  AXMM_THREADSAFE
#endif
#ifndef AXMM_THREADSAFE_ALLOCS
# define AXMM_THREADSAFE_ALLOCS     AXMM_THREADSAFE
#endif

#ifndef AXMM_LISTENERS_ENABLED
# define AXMM_LISTENERS_ENABLED     1
#endif

#ifndef AXMM_FORCEINLINE
# ifdef AX_FORCEINLINE
#  define AXMM_FORCEINLINE          AX_FORCEINLINE
# elif defined( _MSC_VER )
#  define AXMM_FORCEINLINE          __forceinline
# else
#  define AXMM_FORCEINLINE          inline
# endif
#endif

#ifndef AXMM_TYPES_DEFINED
# define AXMM_TYPES_DEFINED         1
# if AX_TYPES_DEFINED
typedef ax_u64_t                    axmm_u64_t;
typedef ax_s64_t                    axmm_s64_t;
typedef ax_u32_t                    axmm_u32_t;
typedef ax_s32_t                    axmm_s32_t;
typedef ax_u16_t                    axmm_u16_t;
typedef ax_s16_t                    axmm_s16_t;
typedef ax_sptr_t                   axmm_ptrdiff_t;
typedef ax_uptr_t                   axmm_size_t;
# elif defined( _MSC_VER )
#  include <stddef.h>
typedef unsigned __int64            axmm_u64_t;
typedef   signed __int64            axmm_s64_t;
typedef unsigned __int32            axmm_u32_t;
typedef   signed __int32            axmm_s32_t;
typedef unsigned __int16            axmm_u16_t;
typedef   signed __int16            axmm_s16_t;
typedef ptrdiff_t                   axmm_ptrdiff_t;
typedef size_t                      axmm_size_t;
# else
#  include <stdint.h>
#  include <stddef.h>
typedef uint64_t                    axmm_u64_t;
typedef  int64_t                    axmm_s64_t;
typedef uint32_t                    axmm_u32_t;
typedef  int32_t                    axmm_s32_t;
typedef uint16_t                    axmm_u16_t;
typedef  int16_t                    axmm_s16_t;
typedef ptrdiff_t                   axmm_ptrdiff_t;
typedef size_t                      axmm_size_t;
# endif
#endif
#ifdef ax_static_assert
ax_static_assert( sizeof( axmm_s64_t ) == 8, "ax_memory: size mismatch" );
ax_static_assert( sizeof( axmm_u64_t ) == 8, "ax_memory: size mismatch" );
ax_static_assert( sizeof( axmm_s32_t ) == 4, "ax_memory: size mismatch" );
ax_static_assert( sizeof( axmm_u32_t ) == 4, "ax_memory: size mismatch" );
ax_static_assert( sizeof( axmm_s16_t ) == 2, "ax_memory: size mismatch" );
ax_static_assert( sizeof( axmm_u16_t ) == 2, "ax_memory: size mismatch" );
ax_static_assert( sizeof( axmm_ptrdiff_t ) == sizeof( void * ), "ax_memory: size mismatch" );
ax_static_assert( sizeof( axmm_size_t ) == sizeof( void * ), "ax_memory: size mismatch" );
#endif

#ifndef axmm_def_alloc
# include <stdlib.h>
# define axmm_def_alloc(N_)         (malloc((N_)))
# define axmm_def_free(P_)          (free((P_)))
#endif

#ifndef axmm_assert
# ifdef AX_ASSERT
#  define axmm_assert               AX_ASSERT
# elif defined( DEBUG ) || defined( _DEBUG ) || defined( __DEBUG__ )
#  include <assert.h>
#  define axmm_assert               assert
# else
#  define axmm_assert(X_)           ((void)0)
# endif
#endif

#ifndef axmm_atomic_add32
# ifdef AX_ATOMIC_FETCH_ADD_FULL32
#  define axmm_atomic_add32         AX_ATOMIC_FETCH_ADD_FULL32
#  define axmm_atomic_sub32         AX_ATOMIC_FETCH_SUB_FULL32
#  define axmm_atomic_cmpxchg32     AX_ATOMIC_COMPARE_EXCHANGE_FULL32
#  define axmm_atomic_xchg32        AX_ATOMIC_EXCHANGE_FULL32
# elif defined( _MSC_VER )
#  include <intrin.h>
#  define axmm_atomic_add32(Dst_,Src_)\
	((axmm_u32_t)(_InterlockedExchangeAdd((volatile long*)(Dst_),(long)(Src_))))
#  define axmm_atomic_sub32(Dst_,Src_)\
	((axmm_u32_t)(_InterlockedExchangeAdd((volatile long*)(Dst_),-(long)(Src_))))
#  define axmm_atomic_xchg32(Dst_,Src_)\
	((axmm_u32_t)(_InterlockedExchange((volatile long*)(Dst_),(long)(Src_))))
#  define axmm_atomic_cmpxchg32(Dst_,Src_,Cmp_)\
\	((axmm_u32_t)(_InterlockedCompareExchange((volatile long*)(Dst_),(long)(Src_),(long)(Cmp_))))
# elif defined( __GNUC__ ) || defined( __clang__ )
#  define axmm_atomic_add32(Dst_,Src_)\
	((axmm_u32_t)(__sync_fetch_and_add((volatile axmm_u32_t*)(Dst_),(axmm_u32_t)(Src_))))
#  define axmm_atomic_sub32(Dst_,Src_)\
	((axmm_u32_t)(__sync_fetch_and_sub((volatile axmm_u32_t*)(Dst_),(axmm_u32_t)(Src_))))
#  define axmm_atomic_xchg32(Dst_,Src_)\
	((axmm_u32_t)(__sync_lock_test_and_set((volatile axmm_u32_t*)(Dst_),(axmm_u32_t)(Src_))))
#  define axmm_atomic_cmpxchg32(Dst_,Src_,Cmp_)\
	((axmm_u32_t)(__sync_val_compare_and_swap((volatile axmm_u32_t*)(Dst_),(axmm_u32_t)(Src_),(axmm_u32_t)(Cmp_))))
# else
#  error ax_memory: unimplemented atomic operations on this platform/compiler
# endif
#endif
#ifndef axmm_atomic_sub32
# error ax_memory: axmm_atomic_sub32 was not defined
#endif
#ifndef axmm_atomic_cmpxchg32
# error ax_memory: axmm_atomic_cmpxchg32 was not defined
#endif
#ifndef axmm_atomic_xchg32
# error ax_memory: axmm_atomic_xchg32 was not defined
#endif

#ifndef axmm_spin
# ifdef INCGUARD_AX_THREAD_H_
#  define axmm_spin()\
	axth_local_spin( AXTHREAD_DEFAULT_SPIN_COUNT )
# else
#  define axmm_spin()\
	do{volatile unsigned spin__i;\
		for(spin__i=0;spin__i<(AXTHREAD_DEFAULT_SPIN_COUNT);++spin__i){}\
	}while(0)
# endif
#endif

/* axmm__c: casts to type; exists to prevent cast warnings from -Weverything */
#ifndef axmm__c
# ifdef __cplusplus
#  define axmm__c(T_) static_cast< T_ >
# else
#  define axmm__c(T_) ( T_ )
# endif
#endif
#ifndef axmm__rc
# ifdef __cplusplus
#  define axmm__rc(T_) reinterpret_cast< T_ >
# else
#  define axmm__rc(T_) ( T_ )
# endif
#endif
#ifndef axmm__pc
# ifdef __cplusplus
#  define axmm__pc(T_,X_) (const_cast<T_>(reinterpret_cast<const T_>(X_)))
# else
#  define axmm__pc(T_,X_) ( (T_)(X_) )
# endif
#endif

#ifndef AXMM_NULL
# ifdef __cplusplus
#  define AXMM_NULL(T_) reinterpret_cast<T_ *>(0)
# else
#  define AXMM_NULL(T_) ((T_*)0)
# endif
#endif




/*
----------------
Operating System
----------------
*/
#ifndef AXMM_OS_DEFINED
# ifdef INCGUARD_AX_PLATFORM_H_
#  define AXMM_OS_WINDOWS           AX_OS_WINDOWS
#  define AXMM_OS_LINUX             AX_OS_LINUX
#  define AXMM_OS_MACOSX            AX_OS_MACOSX
# else
#  define AXMM_OS_WINDOWS           0
#  define AXMM_OS_LINUX             0
#  define AXMM_OS_MACOSX            0
#  if defined( _WIN32 )
#   undef AXMM_OS_WINDOWS
#   define AXMM_OS_WINDOWS          1
#  elif defined( linux ) || defined( __linux ) || defined( __linux__ )
#   undef AXMM_OS_LINUX
#   define AXMM_OS_LINUX            1
#  elif defined( __APPLE__ )
#   undef AXMM_OS_MACOSX
#   define AXMM_OS_MACOSX           1
#  endif
# endif
# define AXMM_OS_DEFINED            1
#endif
#if !(AXMM_OS_WINDOWS|AXMM_OS_LINUX|AXMM_OS_MACOSX)
# error ax_memory: AXMM_OS not determined!
#endif

#if AXMM_IMPLEMENT
# if AXMM_OS_WINDOWS
#  undef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN       1
#  if !defined( _WIN32_WINNT ) || _WIN32_WINNT < 0x0600
#   undef _WIN32_WINNT
#   define _WIN32_WINNT             0x0603
#  endif
#  include <Windows.h>
#  undef min
#  undef max
#  undef AddJob
#  undef Yield
# endif
# if AXMM_OS_LINUX || AXMM_OS_MACOSX
#  include <sys/types.h>
#  include <unistd.h>
#  include <sys/mman.h>
# endif
#endif




/*
----------------
Page Management
----------------
*/
#ifndef AXMM_PAGE_METHOD_DEFINED
# define AXMM_PAGE_METHOD_WINDOWS   0
# define AXMM_PAGE_METHOD_MMAP      0

# if AXMM_OS_WINDOWS
#  undef AXMM_PAGE_METHOD_WINDOWS
#  define AXMM_PAGE_METHOD_WINDOWS  1
# elif AXMM_OS_LINUX || AXMM_OS_MACOSX
#  undef AXMM_PAGE_METHOD_MMAP
#  define AXMM_PAGE_METHOD_MMAP     1
# endif
# define AXMM_PAGE_METHOD_DEFINED   1
#endif
#if !(AXMM_PAGE_METHOD_WINDOWS|AXMM_PAGE_METHOD_MMAP)
# error ax_memory: AXMM_PAGE_METHOD not determined!
#endif




/*
===============================================================================

	RESULT CODES

	Result codes are shared across multiple functions. Each function defines
	its own enum, but the members will match some of the values of this enum.

	Each function gets its own enum to make it clear to both the compiler and
	the programmer how a function can fail. Some compilers will warn when not
	all enum members are handled in a `switch`-statement's cases. This enables
	that to still hold true in instances where you want to deal with errors
	differently based on what they were. On the other hand, if you want to deal
	with function errors generically, they can be safely cast to
	`axmm_result_t`.

	All failure codes are negative.
	All success codes are positive and nonzero.

===============================================================================
*/

typedef enum axmm_result_e
{
	axmm_result_ok    = 1,

	axmm_result_fail = -1000,
	axmm_result_nomem,
	axmm_result_badarg,

	axmm_result_toomany,
	axmm_result_notfound
} axmm_result_t;

#define AXMM_SUCCEEDED(X_) (axmm__c(axmm_result_t)(X_)==axmm_result_ok)
#define AXMM_FAILED(X_)    (axmm__c(axmm_result_t)(X_)!=axmm_result_ok)




/*
===============================================================================

	PLACEMENT NEW

===============================================================================
*/

#if AXMM_CXX_ENABLED
# ifndef AX_CONSTRUCT
#  define AX_CONSTRUCT(X_)\
	::new(reinterpret_cast<void*>(&(X_)),::ax::detail::SPlcmntNw())
namespace ax { namespace detail { struct SPlcmntNw {}; } }
inline void *operator new( axmm_size_t, void *p, ax::detail::SPlcmntNw )
{
	return p;
}
inline void operator delete( void *, void *, ax::detail::SPlcmntNw )
{
}
# endif

namespace ax
{

	template< typename T >
	inline T &construct( T &x )
	{
		AX_CONSTRUCT(x) T();
		return x;
	}
	template< typename T >
	inline T &construct( T &x, const T &y )
	{
		AX_CONSTRUCT(x) T(y);
		return x;
	}
	template< typename T, typename T1 >
	inline T &construct( T &x, T1 x1 )
	{
		AX_CONSTRUCT(x) T(x1);
		return x;
	}
	template< typename T, typename T1, typename T2 >
	inline T &construct( T &x, T1 x1, T2 x2 )
	{
		AX_CONSTRUCT(x) T(x1, x2);
		return x;
	}
	template< typename T, typename T1, typename T2, typename T3 >
	inline T &construct( T &x, T1 x1, T2 x2, T3 x3 )
	{
		AX_CONSTRUCT(x) T(x1, x2, x3);
		return x;
	}
	template< typename T, typename T1, typename T2, typename T3, typename T4 >
	inline T &construct( T &x, T1 x1, T2 x2, T3 x3, T4 x4 )
	{
		AX_CONSTRUCT(x) T(x1, x2, x3, x4);
		return x;
	}
	template< typename T, typename T1, typename T2, typename T3, typename T4, typename T5 >
	inline T &construct( T &x, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5 )
	{
		AX_CONSTRUCT(x) T(x1, x2, x3, x4, x5);
		return x;
	}
	template< typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
	inline T &construct( T &x, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6 )
	{
		AX_CONSTRUCT(x) T(x1, x2, x3, x4, x5, x6);
		return x;
	}
	template< typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7 >
	inline T &construct( T &x, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7 )
	{
		AX_CONSTRUCT(x) T(x1, x2, x3, x4, x5, x6, x7);
		return x;
	}

	template< typename T >
	inline void destroy( T &x )
	{
		x.~T();
	}

}
#endif




/*
===============================================================================

	Allocation Listener
	-------------------

	Allows the programmer to install memory listening hooks, such as for
	tracking/logging allocations to an event viewer. This is directly inspired
	by EASTL's `icorealloclistener.h`.

===============================================================================
*/

AXMM__ENTER_C

typedef struct axmm_alloc_s {
	void *      pBytes;
	axmm_size_t cBytes;
	const char *pszName;
	axmm_size_t cNameBytes;
	axmm_u32_t  uTag;
	axmm_u32_t  uFlags;
	axmm_u32_t  uAlign;
	axmm_u32_t  uAlignOffset;
} axmm_alloc_t;
typedef struct axmm_free_s {
	void *      pBytes;
	axmm_size_t cBytes;
} axmm_free_t;

typedef void( AXMM_CALL *axmm_alloclistener_t )( void *, const axmm_alloc_t * );
typedef void( AXMM_CALL *axmm_freelistener_t )( void *, const axmm_free_t * );

#if AXMM_IMPLEMENT && AXMM_LISTENERS_ENABLED
typedef struct axmm__listener_s {
	void *                   data;
	axmm_alloclistener_t     pfnprealloc;  /* no need to check for NULL */
	axmm_alloclistener_t     pfnpostalloc; /* no need to check for NULL */
	axmm_freelistener_t      pfnprefree;   /* no need to check for NULL */
	axmm_freelistener_t      pfnpostfree;  /* no need to check for NULL */
} axmm__listener_t;

/* implementation: alloc callback that does nothing; used instead of NULL */
static void AXMM_CALL axmm__nullalloclistener( void *, const axmm_alloc_t * )
{
}
/* implementation: free callback that does nothing; used instead of NULL */
static void AXMM_CALL axmm__nullfreelistener( void *, const axmm_free_t * )
{
}

# ifndef AXMM__MAX_LISTENERS
#  define AXMM__MAX_LISTENERS 16
# endif

static axmm__listener_t      axmm__g_listeners[ AXMM__MAX_LISTENERS ];
static axmm_u32_t            axmm__g_cListeners = 0;

# if AXMM_THREADSAFE_LISTENERS
#  ifdef INCGUARD_AX_THREAD_H_
static axth_rwlock_t         axmm__g_listenerLock = AXTHREAD_RWLOCK_INITIALIZER;
#  else
#   define AXMM__MAX_READERS 128
static struct {
	volatile axmm_u32_t r;
	volatile axmm_u32_t w;
}                            axmm__g_listenerLock = { 0, 0 };
#  endif
# endif

/* implementation: acquire write access to the listeners */
static void AXMM_CALL axmm__wlock_listeners( void )
{
# if AXMM_THREADSAFE_LISTENERS
#  ifdef INCGUARD_AX_THREAD_H_
	axth_rwlock_wracquire( &axmm__g_listenerLock );
#  else
	while( axmm_atomic_cmpxchg32( &axmm__g_listenerLock.w, 1, 0 ) != 1 ) {
		axmm_spin();
	}

	axmm_atomic_add32( &axmm__g_listenerLock.r, AXMM__MAX_READERS );
	while( axmm_atomic_cmpxchg32( &axmm__g_listenerLock.r, 0, 0 ) != AXMM__MAX_READERS ) {
		axmm_spin();
	}

	axmm_xchg32( &axmm__g_listenerLock.w, 0 );
#  endif
# endif
}
/* implementation: release write access of the listeners */
static void AXMM_CALL axmm__wunlock_listeners( void )
{
# if AXMM_THREADSAFE_LISTENERS
#  ifdef INCGUARD_AX_THREAD_H_
	axth_rwlock_wrrelease( &axmm__g_listenerLock );
#  else
	axmm_atomic_sub32( &axmm__g_listenerLock.r, AXMM__MAX_READERS );
#  endif
# endif
}

/* implementation: acquire read access to the listeners */
static void AXMM_CALL axmm__rlock_listeners( void )
{
# if AXMM_THREADSAFE_LISTENERS
#  ifdef INCGUARD_AX_THREAD_H_
	axth_rwlock_rdacquire( &axmm__g_listenerLock );
#  else
	axmm_u32_t r;

	do {
		r = axmm_atomic_add32( &axmm__g_listenerLock.r, 1 ) + 1;
		if( r >= AXMM__MAX_READERS ) {
			axmm_atomic_sub32( &axmm__g_listenerLock.r, 1 );
			r = 0;
			axmm_spin();
		}
	} while( !r );
#  endif
# endif
}
/* implementation: release read access of the listeners */
static void AXMM_CALL axmm__runlock_listeners( void )
{
# if AXMM_THREADSAFE_LISTENERS
#  ifdef INCGUARD_AX_THREAD_H_
	axth_rwlock_rdrelease( &axmm__g_listenerLock );
#  else
	axmm_atomic_sub32( &axmm__g_listenerLock.r, 1 );
#  endif
}
# endif
#endif

typedef enum axmm_add_listener_result_e
{
	axmm_add_listener_result_ok      = axmm_result_ok,

	axmm_add_listener_result_badarg  = axmm_result_badarg,
	axmm_add_listener_result_toomany = axmm_result_toomany
} axmm_add_listener_result_t;

AXMM_FUNC axmm_add_listener_result_t
AXMM_CALL axmm_add_listener
(
	void *               pCallbackData,
	axmm_alloclistener_t pfnPreAlloc,
	axmm_alloclistener_t pfnPostAlloc,
	axmm_freelistener_t  pfnPreFree,
	axmm_freelistener_t  pfnPostFree
)
#if AXMM_IMPLEMENT
{
# if AXMM_LISTENERS_ENABLED
	axmm_add_listener_result_t r = axmm_add_listener_result_ok;
	axmm__listener_t *p;

	if( !pfnPreAlloc && !pfnPostAlloc && !pfnPreFree && !pfnPostFree ) {
		return axmm_add_listener_result_badarg;
	}

	if( !pfnPreAlloc  ) { pfnPreAlloc  = &axmm__nullalloclistener; }
	if( !pfnPostAlloc ) { pfnPostAlloc = &axmm__nullalloclistener; }
	if( !pfnPreFree   ) { pfnPreFree   = &axmm__nullfreelistener;  }
	if( !pfnPostFree  ) { pfnPostFree  = &axmm__nullfreelistener;  }

	axmm__wlock_listeners();
	do {
		if( axmm__g_cListeners == AXMM__MAX_LISTENERS ) {
			r = axmm_add_listener_result_toomany;
			break;
		}

		p = &axmm__g_listeners[ axmm__g_cListeners++ ];

		p->data         = pCallbackData;
		p->pfnprealloc  = pfnPreAlloc;
		p->pfnpostalloc = pfnPostAlloc;
		p->pfnprefree   = pfnPreFree;
		p->pfnpostfree  = pfnPostFree;
	} while(0);
	axmm__wunlock_listeners();

	return r;
# else
	return axmm_add_listener_result_toomany;
# endif
}
#else
;
#endif

typedef enum axmm_remove_listener_result_e
{
	axmm_remove_listener_result_ok       = axmm_result_ok,

	axmm_remove_listener_result_notfound = axmm_result_notfound
} axmm_remove_listener_result_t;

AXMM_FUNC axmm_remove_listener_result_t
AXMM_CALL axmm_remove_listener
(
	void *               pCallbackData,
	axmm_alloclistener_t pfnPreAlloc,
	axmm_alloclistener_t pfnPostAlloc,
	axmm_freelistener_t  pfnPreFree,
	axmm_freelistener_t  pfnPostFree
)
#if AXMM_IMPLEMENT
{
# if AXMM_LISTENERS_ENABLED
	axmm_remove_listener_result_t r = axmm_remove_listener_result_notfound;
	axmm__listener_t *p, *q, *e;

	if( !pfnPreAlloc  ) { pfnPreAlloc  = &axmm__nullalloclistener; }
	if( !pfnPostAlloc ) { pfnPostAlloc = &axmm__nullalloclistener; }
	if( !pfnPreFree   ) { pfnPreFree   = &axmm__nullfreelistener;  }
	if( !pfnPostFree  ) { pfnPostFree  = &axmm__nullfreelistener;  }

	axmm__wlock_listeners();
	e = &axmm__g_listeners[ axmm__g_cListeners ];
	for( p = &axmm__g_listeners[ 0 ]; p != e; ++p ) {
		if( p->data         == pCallbackData
		 && p->pfnprealloc  == pfnPreAlloc
		 && p->pfnpostalloc == pfnPostAlloc
		 && p->pfnprefree   == pfnPreFree
		 && p->pfnpostfree  == pfnPostFree ) {
			r = axmm_remove_listener_result_ok;
			for( q = p + 1; q != e; ++q, ++p ) {
				*p = *q;
			}
			--axmm__g_cListeners;
			break;
		}
	}
	axmm__wunlock_listeners();

	return r;
# else
	return axmm_remove_listener_result_notfound;
# endif
}
#else
;
#endif

AXMM_FUNC void AXMM_CALL axmm_prealloc( const axmm_alloc_t *p )
#if AXMM_IMPLEMENT
{
# if AXMM_LISTENERS_ENABLED
	axmm_u32_t i;

	if( !p ) {
		return;
	}

	axmm__rlock_listeners();
	for( i = 0; i < axmm__g_cListeners; ++i ) {
		axmm__g_listeners[ i ].pfnprealloc( axmm__g_listeners[ i ].data, p );
	}
	axmm__runlock_listeners();
# else
	((void)p);
# endif
}
#else
;
#endif
AXMM_FUNC void AXMM_CALL axmm_postalloc( const axmm_alloc_t *p )
#if AXMM_IMPLEMENT
{
# if AXMM_LISTENERS_ENABLED
	axmm_u32_t i;

	if( !p ) {
		return;
	}

	axmm__rlock_listeners();
	for( i = 0; i < axmm__g_cListeners; ++i ) {
		axmm__g_listeners[ i ].pfnpostalloc( axmm__g_listeners[ i ].data, p );
	}
	axmm__runlock_listeners();
# else
	((void)p);
# endif
}
#else
;
#endif
AXMM_FUNC void AXMM_CALL axmm_prefree( const axmm_free_t *p )
#if AXMM_IMPLEMENT
{
# if AXMM_LISTENERS_ENABLED
	axmm_u32_t i;

	if( !p ) {
		return;
	}

	axmm__rlock_listeners();
	for( i = 0; i < axmm__g_cListeners; ++i ) {
		axmm__g_listeners[ i ].pfnprefree( axmm__g_listeners[ i ].data, p );
	}
	axmm__runlock_listeners();
# else
	((void)p);
# endif
}
#else
;
#endif
AXMM_FUNC void AXMM_CALL axmm_postfree( const axmm_free_t *p )
#if AXMM_IMPLEMENT
{
# if AXMM_LISTENERS_ENABLED
	axmm_u32_t i;

	if( !p ) {
		return;
	}

	axmm__rlock_listeners();
	for( i = 0; i < axmm__g_cListeners; ++i ) {
		axmm__g_listeners[ i ].pfnpostfree( axmm__g_listeners[ i ].data, p );
	}
	axmm__runlock_listeners();
# else
	((void)p);
# endif
}
#else
;
#endif

AXMM__LEAVE_C

#if AXMM_CXX_ENABLED
namespace ax
{

	typedef axmm_alloc_t AllocDetails;
	typedef axmm_free_t  FreeDetails;

	class IAllocatorListener
	{
	public:
		IAllocatorListener() {}
		virtual ~IAllocatorListener() {}

		virtual void preAlloc( const AllocDetails & ) = 0;
		virtual void postAlloc( const AllocDetails & ) = 0;

		virtual void preFree( const FreeDetails & ) = 0;
		virtual void postFree( const FreeDetails & ) = 0;
	};

	namespace detail
	{

		AXMM_FUNC void AXMM_CALL axmm__cxxPreAllocListener( void *p, const axmm_alloc_t *details );
		AXMM_FUNC void AXMM_CALL axmm__cxxPostAllocListener( void *p, const axmm_alloc_t *details );
		AXMM_FUNC void AXMM_CALL axmm__cxxPreFreeListener( void *p, const axmm_free_t *details );
		AXMM_FUNC void AXMM_CALL axmm__cxxPostFreeListener( void *p, const axmm_free_t *details );

# if AXMM_IMPLEMENT
		AXMM_FUNC void AXMM_CALL axmm__cxxPreAllocListener( void *p, const axmm_alloc_t *details )
		{
			IAllocatorListener *const pListener = reinterpret_cast< IAllocatorListener * >( p );

			pListener->preAlloc( *details );
		}
		AXMM_FUNC void AXMM_CALL axmm__cxxPostAllocListener( void *p, const axmm_alloc_t *details )
		{
			IAllocatorListener *const pListener = reinterpret_cast< IAllocatorListener * >( p );

			pListener->postAlloc( *details );
		}
		AXMM_FUNC void AXMM_CALL axmm__cxxPreFreeListener( void *p, const axmm_free_t *details )
		{
			IAllocatorListener *const pListener = reinterpret_cast< IAllocatorListener * >( p );

			pListener->preFree( *details );
		}
		AXMM_FUNC void AXMM_CALL axmm__cxxPostFreeListener( void *p, const axmm_free_t *details )
		{
			IAllocatorListener *const pListener = reinterpret_cast< IAllocatorListener * >( p );

			pListener->postFree( *details );
		}
# endif

	}

	inline axmm_add_listener_result_t AXMM_CALL addListener( IAllocatorListener *p )
	{
		if( !p ) {
			return axmm_add_listener_result_badarg;
		}

		return
			axmm_add_listener
			(
				reinterpret_cast<void*>(p),
				&detail::axmm__cxxPreAllocListener,
				&detail::axmm__cxxPostAllocListener,
				&detail::axmm__cxxPreFreeListener,
				&detail::axmm__cxxPostFreeListener
			);
	}
	inline axmm_remove_listener_result_t AXMM_CALL removeListener( IAllocatorListener *p )
	{
		return
			axmm_remove_listener
			(
				reinterpret_cast<void*>(p),
				&detail::axmm__cxxPreAllocListener,
				&detail::axmm__cxxPostAllocListener,
				&detail::axmm__cxxPreFreeListener,
				&detail::axmm__cxxPostFreeListener
			);
	}

}
#endif




/*
===============================================================================

	Thread Worker Identifier
	------------------------

	This is used to get a useful per-thread identifier that can be used to
	potentially index into an array.

	The tagged heap allocator uses this.

===============================================================================
*/

AXMM__ENTER_C

#ifndef axmm_get_worker_id
# ifdef axth_get_worker_id
#  define axmm_get_worker_id()      axth_get_worker_id()
# else
#  define axmm_get_worker_id()      axmm__get_worker_id_impl()
#  define AXMM__NEED_GET_WORKER_ID_IMPL
# endif
#endif

#ifdef AXMM__NEED_GET_WORKER_ID_IMPL
# if AXMM_IMPLEMENT
static AXMM_THREADLOCAL axmm_u32_t  axmm__g_uWorkerId = ~0U;
# endif

AXMM_FUNC axmm_u32_t AXMM_CALL axmm__get_worker_id_impl( void )
# if AXMM_IMPLEMENT
{
	static axmm_u32_t uBaseId = 0;

	if( axmm__g_uWorkerId == ~0U ) {
		axmm__g_uWorkerId = axmm_atomic_add32( &uBaseId, 1 );
	}

	return axmm__g_uWorkerId;
}
# else
;
# endif
#endif

AXMM__LEAVE_C




/*
===============================================================================

	Hierarchical Memory
	-------------------

	- Allows attaching of memory objects to other memory objects, which will
	  free all sub-allocations when the super-allocation is freed.
	- Memory objects can have an "at-free" function pointer, which will be
	  invoked upon freeing the memory.
	- Reference counting is enabled on memory objects. Memory will only be
	  freed once all reference counts reach zero.

===============================================================================
*/

AXMM__ENTER_C

#ifndef axmm_def_h_alloc
# define axmm_def_h_alloc           axmm_def_alloc
# define axmm_def_h_free            axmm_def_free
#endif

#ifndef AXMM_HMALLOC_THREADSAFE
# define AXMM_HMALLOC_THREADSAFE    1
#endif

typedef void( *axmm_pfn_fini_t )( void * );

typedef struct axmm__h_alloc_s
{
	axmm_size_t             n;
	axmm_u32_t              refcnt;
	struct axmm__h_alloc_s *a_prnt;
	struct axmm__h_alloc_s *a_prev, *a_next;
	struct axmm__h_alloc_s *a_head, *a_tail;
	axmm_pfn_fini_t         fini;
} axmm__h_alloc_t;

#if AXMM_IMPLEMENT
static void AXMM_CALL axmm__h_unlink( axmm__h_alloc_t *a )
{
	if( a->a_prev != AXMM_NULL(axmm__h_alloc_t) ) {
		a->a_prev->a_next = a->a_next;
	} else if( a->a_prnt != AXMM_NULL(axmm__h_alloc_t) ) {
		a->a_prnt->a_head = a->a_next;
	}
	if( a->a_next != AXMM_NULL(axmm__h_alloc_t) ) {
		a->a_next->a_prev = a->a_prev;
	} else if( a->a_prnt != AXMM_NULL(axmm__h_alloc_t) ) {
		a->a_prnt->a_tail = a->a_prev;
	}
}
#endif

AXMM_FUNC void *AXMM_CALL axmm_h_alloc( axmm_size_t n )
#if AXMM_IMPLEMENT
{
	axmm__h_alloc_t *p;
	void *vp;

	if( !( vp = axmm_def_h_alloc( n + sizeof( axmm__h_alloc_t ) ) ) ) {
		return AXMM_NULL(void);
	}

	p = axmm__c(axmm__h_alloc_t *)( vp );

	p->n      = n;
	p->refcnt = 1;
	p->a_prnt = AXMM_NULL(axmm__h_alloc_t);
	p->a_head = AXMM_NULL(axmm__h_alloc_t);
	p->a_tail = AXMM_NULL(axmm__h_alloc_t);
	p->a_prev = AXMM_NULL(axmm__h_alloc_t);
	p->a_next = AXMM_NULL(axmm__h_alloc_t);
	p->fini   = ( axmm_pfn_fini_t )0;

	vp = axmm__c( void * )( p + 1 );

	memset( vp, 0, n );
	return vp;
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_free( void *p )
#if AXMM_IMPLEMENT
{
	axmm__h_alloc_t *a;

	if( !p ) {
		return AXMM_NULL(void);
	}

	a = ( axmm__h_alloc_t * )p - 1;
# if AXMM_HMALLOC_THREADSAFE
	if( axmm_atomic_sub32( &a->refcnt, 1 ) > 1 ) {
		return AXMM_NULL(void);
	}
# else
	if( --a->refcnt != 0 ) {
		return AXMM_NULL(void);
	}
# endif

	axmm__h_unlink( a );

	if( a->fini != axmm__c( axmm_pfn_fini_t )(0) ) {
		a->fini( p );
	}

	while( a->a_head != AXMM_NULL(axmm__h_alloc_t) ) {
		axmm_h_free( axmm__c( void * )( a->a_head + 1 ) );
	}

	axmm_def_h_free( axmm__c( void * )( a ) );
	return AXMM_NULL(void);
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_addref( void *p )
#if AXMM_IMPLEMENT
{
	axmm_assert( p != AXMM_NULL(void) );

# if AXMM_HMALLOC_THREADSAFE
	axmm_atomic_add32( &((axmm__h_alloc_t*)p-1)->refcnt, 1 );
# else
	++( axmm__c( axmm__h_alloc_t * )( p ) - 1 )->refcnt;
# endif
	return p;
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_atfree( void *p, axmm_pfn_fini_t pfn_fini )
#if AXMM_IMPLEMENT
{
	axmm_assert( p != AXMM_NULL(void) );

	( axmm__c( axmm__h_alloc_t * )( p ) - 1 )->fini = pfn_fini;
	return p;
}
#else
;
#endif
axmm_size_t axmm_h_size( const void *p )
#if AXMM_IMPLEMENT
{
	if( !p ) {
		return 0;
	}

	return ( axmm__c( const axmm__h_alloc_t * )( p ) - 1 )->n;
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_detach( void *subobject )
#if AXMM_IMPLEMENT
{
	axmm__h_alloc_t *chld;

	axmm_assert( subobject != AXMM_NULL(void) );

	chld = axmm__c( axmm__h_alloc_t * )( subobject ) - 1;

	axmm__h_unlink( chld );
	return subobject;
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_attach( void *subobject, void *superobject )
#if AXMM_IMPLEMENT
{
	axmm__h_alloc_t *chld, *prnt;

	axmm_assert( subobject != AXMM_NULL(void) );
	axmm_assert( superobject != AXMM_NULL(void) );

	chld = axmm__c( axmm__h_alloc_t * )( subobject ) - 1;
	prnt = axmm__c( axmm__h_alloc_t * )( superobject ) - 1;

	axmm__h_unlink( chld );

	chld->a_prnt = prnt;
	chld->a_prev = prnt->a_tail;
	chld->a_next = AXMM_NULL(axmm__h_alloc_t);
	if( chld->a_prev != AXMM_NULL(axmm__h_alloc_t) ) {
		chld->a_prev->a_next = chld;
	} else {
		prnt->a_head = chld;
	}
	prnt->a_tail = chld;

	return subobject;
}
#else
;
#endif

#if AXMM_IMPLEMENT
# define axmm__h_ptrfield(Ptr_,Field_)\
	(\
		( ( (axmm__c(const axmm__h_alloc_t *)(Ptr_) - 1)->Field_ ) != AXMM_NULL(axmm__h_alloc_t) )\
		? (axmm__pc(void*,( axmm__c(const axmm__h_alloc_t * )(Ptr_) - 1 )->Field_ + 1 ))\
		: (AXMM_NULL(void))\
	)
#endif

AXMM_FUNC void *AXMM_CALL axmm_h_parent( const void *subobject )
#if AXMM_IMPLEMENT
{
	axmm_assert( subobject != AXMM_NULL(const void) );

	return axmm__h_ptrfield( subobject, a_prnt );
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_prev( const void *object )
#if AXMM_IMPLEMENT
{
	axmm_assert( object != AXMM_NULL(const void) );

	return axmm__h_ptrfield( object, a_prev );
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_next( const void *object )
#if AXMM_IMPLEMENT
{
	axmm_assert( object != AXMM_NULL(const void) );

	return axmm__h_ptrfield( object, a_next );
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_head( const void *superobject )
#if AXMM_IMPLEMENT
{
	axmm_assert( superobject != AXMM_NULL(const void) );

	return axmm__h_ptrfield( superobject, a_head );
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_h_tail( const void *superobject )
#if AXMM_IMPLEMENT
{
	axmm_assert( superobject != AXMM_NULL(const void) );

	return axmm__h_ptrfield( superobject, a_tail );
}
#else
;
#endif

AXMM__LEAVE_C

#if AXMM_CXX_ENABLED
namespace ax
{

	AXMM_FORCEINLINE void *AXMM_CALL h_alloc( axmm_size_t n )
	{
		return axmm_h_alloc( n );
	}
	AXMM_FORCEINLINE void *AXMM_CALL h_suballoc( void *superobject, axmm_size_t n )
	{
		void *const subobject = axmm_h_alloc( n );
		if( !subobject ) {
			return AXMM_NULL(void);
		}

		return !superobject ? subobject : axmm_h_attach( subobject, superobject );
	}
	AXMM_FORCEINLINE void *AXMM_CALL h_dealloc( void *p )
	{
		return axmm_h_free( p );
	}

	AXMM_FORCEINLINE void *AXMM_CALL h_addRef( void *p )
	{
		return axmm_h_addref( p );
	}

	AXMM_FORCEINLINE void *AXMM_CALL h_atDealloc( void *p, axmm_pfn_fini_t pfnFini )
	{
		return axmm_h_atfree( p, pfnFini );
	}
	AXMM_FORCEINLINE axmm_size_t AXMM_CALL h_size( const void *p )
	{
		return axmm_h_size( p );
	}

	AXMM_FORCEINLINE void *AXMM_CALL h_attach( void *subobject, void *superobject )
	{
		return axmm_h_attach( subobject, superobject );
	}
	AXMM_FORCEINLINE void *AXMM_CALL h_detach( void *subobject )
	{
		return axmm_h_detach( subobject );
	}

	AXMM_FORCEINLINE void *AXMM_CALL h_parent( void *subobject )
	{
		return axmm_h_parent( subobject );
	}
	AXMM_FORCEINLINE void *AXMM_CALL h_prev( void *object )
	{
		return axmm_h_prev( object );
	}
	AXMM_FORCEINLINE void *AXMM_CALL h_next( void *object )
	{
		return axmm_h_next( object );
	}
	AXMM_FORCEINLINE void *AXMM_CALL h_head( void *superobject )
	{
		return axmm_h_head( superobject );
	}
	AXMM_FORCEINLINE void *AXMM_CALL h_tail( void *superobject )
	{
		return axmm_h_tail( superobject );
	}

	AXMM_FORCEINLINE const void *AXMM_CALL h_parent( const void *subobject )
	{
		return axmm_h_parent( subobject );
	}
	AXMM_FORCEINLINE const void *AXMM_CALL h_prev( const void *object )
	{
		return axmm_h_prev( object );
	}
	AXMM_FORCEINLINE const void *AXMM_CALL h_next( const void *object )
	{
		return axmm_h_next( object );
	}
	AXMM_FORCEINLINE const void *AXMM_CALL h_head( const void *superobject )
	{
		return axmm_h_head( superobject );
	}
	AXMM_FORCEINLINE const void *AXMM_CALL h_tail( const void *superobject )
	{
		return axmm_h_tail( superobject );
	}

}
#endif




/*
===============================================================================

	System Page Allocator
	---------------------

	Uses the OS's native functions for allocating a page directly, rather than
	using the heap.

===============================================================================
*/

AXMM__ENTER_C

typedef enum axmm_page_prot_e
{
	axmem_pp_noaccess = 0x0,
	axmem_ppf_read    = 0x1,
	axmem_ppf_write   = 0x2,
	axmem_ppf_execute = 0x4,

	axmem_pp_na  = axmem_pp_noaccess,
	axmem_pp_r   = axmem_ppf_read,
	axmem_pp_w   = axmem_ppf_write,
	axmem_pp_rw  = axmem_ppf_read | axmem_ppf_write,
	axmem_pp_rwx = axmem_ppf_read | axmem_ppf_write | axmem_ppf_execute,
	axmem_pp_rx  = axmem_ppf_read | axmem_ppf_execute,
	axmem_pp_x   = axmem_ppf_execute
} axmm_page_prot_t;

AXMM_FUNC axmm_size_t AXMM_CALL axmm_get_page_size( void )
#if AXMM_IMPLEMENT
{
# if AXMM_OS_WINDOWS
	SYSTEM_INFO si;

	GetSystemInfo( &si );
	return axmm__c( axmm_size_t )( si.dwPageSize );
# elif AXMM_OS_LINUX || AXMM_OS_MACOSX
	int r;

	do {
#  if defined( _SC_PAGE_SIZE )
		if( ( r = sysconf( _SC_PAGE_SIZE ) ) != -1 ) {
			break;
		}
#  endif

#  if defined( PAGE_SIZE )
		if( ( r = sysconf( PAGE_SIZE ) ) != -1 ) {
			break;
		}
#  endif

		r = 4096;
	} while(0);

	return axmm__c( axmm_size_t )( axmm__c( unsigned )( r ) );
# else
#  error Could not determine how to implement axmm_get_page_size()
# endif
}
#else
;
#endif

#if AXMM_IMPLEMENT
# if AXMM_PAGE_METHOD_WINDOWS
static DWORD AXMM_CALL axmm__mswin_protf( axmm_u32_t uProtFlags )
{
	switch( uProtFlags & axmem_pp_rwx ) {
	case axmem_pp_r:
		return PAGE_READONLY;
	case axmem_pp_w:
	case axmem_pp_rw:
		return PAGE_READWRITE;
	case axmem_pp_x:
		return PAGE_EXECUTE;

	case axmem_pp_rx:
		return PAGE_EXECUTE_READ;
	case axmem_pp_w | axmem_pp_x:
	case axmem_pp_rwx:
		return PAGE_EXECUTE_READWRITE;

	default:
		break;
	}

	/* can only reach this point if the above evaluates to 0 */
	return PAGE_NOACCESS;
}
# elif AXMM_PAGE_METHOD_MMAP
static int AXMM_CALL axmm__posix_protf( axmm_u32_t uProtFlags )
{
	switch( uProtFlags & axmem_pp_rwx ) {
	case 0:
		return PROT_NONE;

	default:
		return
			( ( uProtFlags & axmem_pp_r ) ? PROT_READ  : 0 ) |
			( ( uProtFlags & axmem_pp_w ) ? PROT_WRITE : 0 ) |
			( ( uProtFlags & axmem_pp_x ) ? PROT_EXEC  : 0 );
	}
}
# endif
#endif

AXMM_FUNC void *AXMM_CALL axmm_page_reserve( axmm_size_t cBytes )
#if AXMM_IMPLEMENT
{
# if AXMM_PAGE_METHOD_WINDOWS
	return VirtualAlloc( AXMM_NULL(void), cBytes, MEM_RESERVE, PAGE_NOACCESS );
# elif AXMM_PAGE_METHOD_MMAP
	void *p;

	p = mmap( AXMM_NULL(void), cBytes, PROT_NONE, MAP_PRIVATE|MAP_ANON, -1, 0 );
	if( !p ) {
		return AXMM_NULL(void);
	}

	msync( p, cBytes, MS_SYNC|MS_INVALIDATE );
	return p;
# else
#  error Unhandled AXMM_PAGE_METHOD
# endif
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_page_commit( void *pBase, axmm_size_t cBytes, axmm_u32_t uProtFlags AXMM_DEFPARM(axmem_pp_rw) )
#if AXMM_IMPLEMENT
{
# if AXMM_PAGE_METHOD_WINDOWS
	return VirtualAlloc( pBase, cBytes, MEM_COMMIT, axmm__mswin_protf( uProtFlags ) );
# elif AXMM_PAGE_METHOD_MMAP
	void *p;

	p = mmap( pBase, cBytes, axmm__posix_protf( uProtFlags ), MAP_FIXED|MAP_SHARED|MAP_ANON, -1, 0 );
	if( !p ) {
		return AXMM_NULL(void);
	}

	msync( pBase, cBytes, MS_SYNC|MS_INVALIDATE );
	return p;
# else
#  error Unhandled AXMM_PAGE_METHOD
# endif
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_page_decommit( void *pBase, axmm_size_t cBytes )
#if AXMM_IMPLEMENT
{
# if AXMM_PAGE_METHOD_WINDOWS
#  ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable:6250)
#  endif
	VirtualFree( pBase, cBytes, MEM_DECOMMIT );
#  ifdef _MSC_VER
#   pragma warning(pop)
#  endif
# elif AXMM_PAGE_METHOD_MMAP
	/*
		Cannot unmap the address because kernel ignores.

		Claim this is a different address. Due to demand paging this will not be
		committed until used.

		TODO: (RESEARCH) Would we benefit from calling munmap() after this?
	*/

	mmap( pBase, cBytes, PROT_NONE, MAP_FIXED|MAP_PRIVATE|MAP_ANON, -1, 0 );
	msync( pBase, cBytes, MS_SYNC|MS_INVALIDATE );
# else
#  error Unhandled AXMM_PAGE_METHOD
# endif
	return AXMM_NULL(void);
}
#else
;
#endif
AXMM_FUNC void *AXMM_CALL axmm_page_release( void *pBase, axmm_size_t cBytes )
#if AXMM_IMPLEMENT
{
# if AXMM_PAGE_METHOD_WINDOWS
	( void )cBytes;
	VirtualFree( pBase, 0, MEM_RELEASE );
# elif AXMM_PAGE_METHOD_MMAP
	msync( pBase, cBytes, MS_SYNC );
	munmap( pBase, cBytes );
# else
#  error Unhandled AXMM_PAGE_METHOD
# endif

	return AXMM_NULL(void);
}
#else
;
#endif

AXMM_FUNC int AXMM_CALL axmm_page_protect( void *pBaseAddr, axmm_size_t cBytes, axmm_u32_t uProtFlags )
#if AXMM_IMPLEMENT
{
# if AXMM_OS_WINDOWS
	DWORD dwOldProtect;
	return VirtualProtect( pBaseAddr, cBytes, axmm__mswin_protf( uProtFlags ), &dwOldProtect ) != 0;
# elif AXMM_OS_LINUX || AXMM_OS_MACOSX
	return mprotect( pBaseAddr, cBytes, axmm__posix_protf( uProtFlags ) ) == 0;
# else
#  error Could not determine how to implement axmm_page_protect()
# endif
}
#else
;
#endif

AXMM__LEAVE_C

#if AXMM_CXX_ENABLED
namespace ax
{

	namespace EPageProt
	{
		typedef axmm_page_prot_t    Type;
		static const Type Read      = axmem_ppf_read;
		static const Type Write     = axmem_ppf_write;
		static const Type Exec      = axmem_ppf_execute;
		static const Type NoAccess  = axmem_pp_noaccess;
		static const Type R         = axmem_pp_r;
		static const Type W         = axmem_pp_w;
		static const Type RW        = axmem_pp_rw;
		static const Type RWX       = axmem_pp_rwx;
		static const Type RX        = axmem_pp_rx;
		static const Type X         = axmem_pp_x;
	}

	AXMM_FORCEINLINE axmm_size_t getPageSize()
	{
		return axmm_get_page_size();
	}

	AXMM_FORCEINLINE void *pageReserve( axmm_size_t cBytes )
	{
		return axmm_page_reserve( cBytes );
	}
	AXMM_FORCEINLINE void *pageCommit( void *pBase, axmm_size_t cBytes, axmm_u32_t uProtFlags = axmem_pp_rw )
	{
		return axmm_page_commit( pBase, cBytes, uProtFlags );
	}
	AXMM_FORCEINLINE void *pageDecommit( void *pBase, axmm_size_t cBytes )
	{
		return axmm_page_decommit( pBase, cBytes );
	}
	AXMM_FORCEINLINE void *pageRelease( void *pBase, axmm_size_t cBytes )
	{
		return axmm_page_release( pBase, cBytes );
	}

	AXMM_FORCEINLINE bool pageProtect( void *pBaseAddr, axmm_size_t cBytes, axmm_u32_t uProtFlags )
	{
		return axmm_page_protect( pBaseAddr, cBytes, uProtFlags ) != 0;
	}

}
#endif




/*
===============================================================================

	Phased Allocator
	----------------

	Inspired by Naughty Dog's presentation "Parallelizing the Naughty Dog
	Engine Using Fibers," which in part described their high-performance high-
	volume multi-threading and memory efficient allocator.

	You have a memory pool that hosts a collection of fixed-size blocks. Blocks
	can be assigned tags. There is no "free(ptr)" interface. In its place is a
	"free(tag)" interface, which will bulk free all blocks associated with that
	tag. (e.g., "Game," or "Render," or "GPU," etc.)

	Sub-allocations will be made within a given block as the block size is quite
	large (2MB). The allocations will all be linear.

	To speed up allocations, and also make them thread-safe, threads will grab
	private blocks they will make allocations from. There will be at most one
	block per tag in the thread's local pool. This means only grabbing new
	blocks from the global pool is necessary.

	If an allocation is larger than one block then multiple contiguous blocks
	will need to be pulled from the global pool. This operation is somewhat more
	expensive, but is expected to be relatively rare for the use case.

	！！！ Only use this allocator if you require a fixed-size memory pool,
	`      and have explicit control over synchronization of allocations and
	`      object life-time. This is usually only the case for well-developed,
	`      mission-critical, and memory-hungry games.

	＊ Tags should be thought of as life-times rather than systems. From the
	`  Naughty Dog presentation, the following are listed:

		ⅰ Single Game Logic Stage (scratch memory)
		ⅱ Double Game Logic Stage (low-priority ray casts)
		ⅲ Game To Render Logic Stage (object instance arrays)
		ⅳ Game To GPU Stage (skinning matrices)
		ⅴ Render To GPU Stage (command buffers)

		(Those are doubled as the tags apply to both CPU and GPU memory.)




	Implementation Details
	----------------------

	A pool structure holds a pointer to the memory blocks, which can either be
	heap allocated or statically allocated. (We don't care. We only expect the
	memory to be valid for the duration we use it.)

	The pool structure additionally holds an array of indexes representing the
	used blocks and the allocated blocks. Another integer represents where in
	this array the free blocks exist at-and-after, implying where the used
	blocks exist. This integer is called the "split position."

	Indexes are encoded as 12-bits representing each 2MB block (giving 8GB of
	addressable memory per allocator), and 4-bits representing the tag (allowing
	16 unique tags, per allocator).

	The following is a basic set of operations for working with the data
	structure. Be aware that these aren't optimized and that the source-level
	implementation may differ. However the process is mostly the same.

	To allocate a block:
	
		① Atomic-fetch-then-add on the "split position" for the array.
		② If the split position has the high-bit set, then a lock is held and
		`  we must wait for it to be released. Spin-loop until the high-bit is
		`  not set, then try again from ①.
		③ If the split position is out of range, then handle "out of memory"
		`  in whichever way is appropriate.
		④ The value we retrieved from the atomic-fetch-and-add gives us the
		`  block index we can use. Change its tag to whatever tag we've
		`  allocated. Done.

	To deallocate a *tag*:

		① Acquire a lock. (Described in a below section.)
		② For each used block, remove it from that section of the array by
		`  swapping it with the last element in the section. Decrement the
		`  split position of the array. (This simultaneously marks the value
		`  as free.)
		③ Optionally sort the array to improve performance for allocating
		`  multiple blocks at once.
		④ Release the lock. (Described in a below section.)

		＊ Also note that thread-local caches will need to be wiped, or the
		`  thread needs to become aware that the blocks it had cached are no
		`  longer valid.
	
	To allocate multiple blocks:

		① Acquire a lock. (Described in a below section.)
		② Search for enough contiguous blocks, and remove them from that
		`  section of the array. (Again, order does not matter, but to speed up
		`  the algorithm it is preferred that these blocks are kept in order.)
		③ Adjust the split position (the allocated blocks should be in the
		`  "used" section rather than the "free" section now). 
		④ Release the lock. (Described in a below section.)

	To acquire/release a lock:

		① Read the split position. If it is currently locked (high-bit set),
		`  then spin until it is not.
		② Attempt to atomic-compare-and-set the value to the read current
		`  position with the high-bit newly set. If this fails, go back to ①.
		③ Do whatever you need to with the lock. Do not attempt to grab the
		`  lock again from the same thread -- it is not re-entrant.
		④ Release the lock with an atomic-set to the new value of "current
		`  position." (The high-bit must not be set; it must be cleared.)

	The thread has a local set of sixteen block pointers. The pointers
	correspond to the tags for the blocks. (The pointers can be indexes instead
	as long as the thread has access to the base address to resolve the
	appropriate pointer for sub-allocations.) The pointers should be null (or in
	the case of indexes, be set to a specific "invalid value") when a new block
	would be needed for the tag, and by default.
	
	When a thread has acquired a block it will make linear allocations from it.
	Linear allocations will always be a multiple of 16 bytes, whether requested
	or not. This ensures that as long as the base pointer is aligned that all
	subsequent allocations will also be aligned (to 16 bytes). The value
	tracking the split position of the linear allocation should exist within
	the last 16 bytes of the block. This is to allow it to be overridden once
	the block becomes full (allowing more memory to be squeezed out of
	individual blocks -- this is opposed to "at the start of the block" or "in
	the thread-local data"). The allocator will know it needs a new block after
	that, so it will set its internal pointer of that block to null. (Which
	signals it will need another block.)

	If an allocation is requested that would not fit within the current block
	then one of the blocks must be "wasted." (It will be unavailable for further
	sub-allocations.) The block that is wasted should be the block that has less
	available space. (For a multiple block allocation, check the last block
	rather than the first.) The block with more available space should be used
	as the thread-local block. If the available space is equal between the
	blocks, then do not change the thread-local block.




	Optimization Avenues and Other Alterations
	------------------------------------------

	【Dynamic Memory】
	It is not necessary to use a fixed-pool. For example, you can reserve a set
	of pages from the virtual address space (esp. if targeting 64-bit) without
	actually allocating the memory. When a block is "allocated" commit that
	block of memory then. Doing so is mostly useful for editors/tools or for
	development builds when you're testing to see what your memory budget needs
	to be. Note, however, that each block will need to be page-sized (or a
	multiple thereof) for this to make sense.




	Synchronization Bug
	-------------------

	If a tag is being freed while other tags are still being allocated there's
	the possibility that a tag that has just been allocated will be touched in
	the process of the tag that's being free'd. See axmm_phased_free() and
	axmm__phased_block_fetch().

	This can be solved by either adjusting the algorithm to just always avoid
	the fetch-add, or by using a separate "writers count" that tracks the
	number of interested allocators. (Things that are fetching a block and
	marking that block as active.)

===============================================================================
*/

AXMM__ENTER_C

#ifndef AXMM_BLOCK_SIZE
/* Size of each block in the tagged heap allocator */
# define AXMM_BLOCK_SIZE            (2*1024*1024) /*2MB*/
#endif

#ifndef AXMM_CACHE_SIZE
# ifdef AX_CACHE_SIZE
/* The assumed size of the target D$ */
#  define AXMM_CACHE_SIZE           AX_CACHE_SIZE
# else
#  define AXMM_CACHE_SIZE           64
# endif
#endif

#ifndef AXMM_MAX_THREADS
/* Maximum number of threads to support for the tagged heap allocator */
# define AXMM_MAX_THREADS           ( sizeof( void * )*8 )
#endif

#ifndef AXMM_BLOCK_INDEX_BITS
/* Number of bits used by the block index type to index into a block */
# define AXMM_BLOCK_INDEX_BITS      12
#endif
#ifndef AXMM_BLOCK_INDEX_TYPE
# if (AXMM_BLOCK_INDEX_BITS) < 16
/* Type used for indexing blocks in the tagged heap allocator */
#  define AXMM_BLOCK_INDEX_TYPE     axmm_u16_t
# else
#  define AXMM_BLOCK_INDEX_TYPE     axmm_u32_t
# endif
#endif

/* Number of total bits available in the block index type */
#define AXMM_BLOCK_INDEX_TYPE_BITS  (sizeof(AXMM_BLOCK_INDEX_TYPE)*8)

/* Number of indexes usable within the tagged heap */
#define AXMM_MAX_INDEXES            (1<<(AXMM_BLOCK_INDEX_BITS))
/* Number of tags usable based on the block index type and the number of bits used for indexing blocks */
#define AXMM_MAX_TAGS               (1<<(AXMM_BLOCK_INDEX_TYPE_BITS-(AXMM_BLOCK_INDEX_BITS)))

/* Block index's index mask */
#define AXMM_BLOCK_INDEX_MASK       ( axmm__c( AXMM_BLOCK_INDEX_TYPE )( AXMM_MAX_INDEXES - 1 ) )

/* Half the total usable user space */
#define AXMM_MAX_USER_SPACE         (((1+(sizeof(char)-sizeof(char)))<<(sizeof(void*)*4))-1)

/* Theoretical maximum addressable bytes from the blocks */
#define AXMM_MAX_ADDRESSABLE_BLOCK_BYTES_UNCAPPED \
	( ( axmm__c( axmm_u64_t )( AXMM_MAX_INDEXES ) )*( axmm__c( axmm_u64_t )( AXMM_BLOCK_SIZE ) ) )

/* Capped maximum addressable bytes from the blocks */
#define AXMM_MAX_ADDRESSABLE_BLOCK_BYTES \
	( AXMM_MAX_ADDRESSABLE_BLOCK_BYTES_UNCAPPED < AXMM_MAX_USER_SPACE \
	? AXMM_MAX_ADDRESSABLE_BLOCK_BYTES_UNCAPPED : AXMM_MAX_USER_SPACE )

/* Rounded integer division */
#define AXMM__INTDIV(X_,Y_)         ( (X_)/(Y_) + ( +( ( (X_)%(Y_) ) != 0 ) ) )

/* Maximum number of addressable blocks */
#ifndef AXMM_MAX_BLOCKS
# define AXMM_MAX_BLOCKS            AXMM__INTDIV(AXMM_MAX_ADDRESSABLE_BLOCK_BYTES,AXMM_BLOCK_SIZE)
#endif

#ifndef AXMM_PHASED_HEAP_ALIGNMENT
/* Granularity of linear alignment within a block */
# define AXMM_PHASED_HEAP_ALIGNMENT 16
#endif

/* Allocation data for a block that is still allowed to be allocated from */
typedef struct axmm__phased_block_s
{
	/* Direct pointer to the block we're accessing */
	void *                          pBlockBase;
	/* Number of bytes currently used in this block (linear allocations) -- aligned to `AXMM_PHASED_HEAP_ALIGNMENT` */
	axmm_u32_t                      cBytesUsed;
} axmm__phased_block_t;

/* Per-thread data for the phased-heap allocator */
typedef struct axmm__phased_heap_thread_s
{
	/* Current working block for each of the tags */
	axmm__phased_block_t            workingBlocks       [ AXMM_MAX_TAGS ];
} axmm__phased_heap_thread_t;

/* Primary structure for a phased-heap */
typedef struct axmm_phased_heap_s
{
	/* Per-thread data */
	axmm__phased_heap_thread_t      threads             [ AXMM_MAX_THREADS ];
	/* Current divide between used and free block indexes in `blockIndexes` */
	axmm_u32_t                      uSplitPos;
	/* BlockIndexes[ 0 < i < uSplitPos ] are allocated; `blockIndexes[ uSplitPos <= i < AXMM_MAX_INDEXES ]` are available */
	AXMM_BLOCK_INDEX_TYPE           blockIndexes        [ AXMM_MAX_INDEXES ];
	/* Base address for the blocks -- should be aligned to `AXMM_PHASED_HEAP_ALIGNMENT` */
	void *                          pBaseBlock;
	/* Maximum number of blocks that can be used */
	axmm_u32_t                      cMaxBlocks;
	/* Flags */
	unsigned                        uFlags;
} axmm_phased_heap_t;

typedef enum axmm__phased_heap_flag_e
{
	axmem__phasedf_usrptr           = 1<<0
} axmm__phased_heap_flag_t;

/* -------------------------------------------------------------------------- */

#ifndef axmm__phased_reserve
# define axmm__phased_reserve(BasePtr_,NumBytes_)\
	axmm_page_reserve(NumBytes_) /*axmm_page_alloc( (BasePtr_), (NumBytes_), axmem_pa_reserve, 0 )*/
#endif
#ifndef axmm__phased_commit
# define axmm__phased_commit(BasePtr_,NumBytes_)\
	axmm_page_commit(BasePtr_, NumBytes_, axmem_pp_rw) /*axmm_page_alloc( (BasePtr_), (NumBytes_), axmem_pa_commit, axmem_pp_rw )*/
#endif
#ifndef axmm__phased_decommit
# define axmm__phased_decommit(BasePtr_,NumBytes_)\
	axmm_page_decommit(BasePtr_, NumBytes_)
#endif
#ifndef axmm__phased_release
# define axmm__phased_release(BasePtr_,NumBytes_)\
	axmm_page_release(BasePtr_, NumBytes_) /*axmm_page_free( (BasePtr_), (NumBytes_) )*/
#endif
#ifndef axmm__phased_protect_rw
# define axmm__phased_protect_rw(BlockPtr_,NumBytes_)\
	axmm_page_protect((BlockPtr_),(NumBytes_),axmem_pp_rw)
#endif
#ifndef axmm__phased_protect_na
# define axmm__phased_protect_na(BlockPtr_,NumBytes_)\
	axmm_page_protect((BlockPtr_),(NumBytes_),axmem_pp_noaccess)
#endif

/* Initialize a phased-heap allocator -- if pBaseOrNull is not-NULL then cBaseBytesOrZero must be valid */
AXMM_FUNC axmm_phased_heap_t *AXMM_CALL axmm_phased_init( axmm_phased_heap_t *p, void *pBaseOrNull, axmm_size_t cBaseBytesOrZero )
#if AXMM_IMPLEMENT
{
	axmm_size_t i, j;

	for( i = 0; i < AXMM_MAX_THREADS; ++i ) {
		for( j = 0; j < AXMM_MAX_TAGS; ++j ) {
			p->threads[ i ].workingBlocks[ j ].pBlockBase = AXMM_NULL(void);
			p->threads[ i ].workingBlocks[ j ].cBytesUsed = AXMM_BLOCK_SIZE;
		}
	}

	p->uSplitPos = 0;

	for( i = 0; i < AXMM_MAX_INDEXES; ++i ) {
		p->blockIndexes[ i ] = ( AXMM_BLOCK_INDEX_TYPE )i;
	}

	p->pBaseBlock =
		( pBaseOrNull != AXMM_NULL(void) )
		? pBaseOrNull
		: axmm__phased_reserve( AXMM_NULL(void), AXMM_MAX_ADDRESSABLE_BLOCK_BYTES );
	if( !p->pBaseBlock ) {
		return AXMM_NULL(axmm_phased_heap_t);
	}

	p->cMaxBlocks =
		axmm__c( axmm_u32_t )(
			( pBaseOrNull != AXMM_NULL(void) )
			? cBaseBytesOrZero/AXMM_BLOCK_SIZE
			: AXMM_MAX_BLOCKS
		);
	if( !p->cMaxBlocks ) {
		return AXMM_NULL(axmm_phased_heap_t);
	}

	p->uFlags = 0;
	if( pBaseOrNull != AXMM_NULL(void) ) {
		p->uFlags |= axmem__phasedf_usrptr;
	}

	return p;
}
#else
;
#endif

/* Finish using a phased-heap allocator */
AXMM_FUNC axmm_phased_heap_t *AXMM_CALL axmm_phased_fini( axmm_phased_heap_t *p )
#if AXMM_IMPLEMENT
{
	if( p->uFlags & axmem__phasedf_usrptr ) {
		axmm__phased_protect_rw( p->pBaseBlock, (axmm__c(axmm_size_t)(p->cMaxBlocks))*AXMM_BLOCK_SIZE );
	} else {
		axmm__phased_release( p->pBaseBlock, (axmm__c(axmm_size_t)(p->cMaxBlocks))*AXMM_BLOCK_SIZE );
	}
	p->pBaseBlock = AXMM_NULL(void);
	p->cMaxBlocks = 0;

	return AXMM_NULL(axmm_phased_heap_t);
}
#else
;
#endif

/* Retrieve the axmm__phased_heap_thread_t for the current thread and the given phased-heap */
AXMM_FUNC axmm__phased_heap_thread_t *AXMM_CALL axmm_phased_thread( axmm_phased_heap_t *p )
#if AXMM_IMPLEMENT
{
	axmm_u32_t uWorkerId;

	uWorkerId = axmm_get_worker_id();
	if( uWorkerId >= AXMM_MAX_THREADS ) {
		return AXMM_NULL(axmm__phased_heap_thread_t);
	}

	return &p->threads[ uWorkerId ];
}
#else
;
#endif

/* Fetch a single block from the phased-heap (this is not a user allocation) */
AXMM_FUNC void *AXMM_CALL axmm__phased_block_fetch( axmm_phased_heap_t *p, unsigned uTag )
#if AXMM_IMPLEMENT
{
	AXMM_BLOCK_INDEX_TYPE uBlockIndex;
	axmm_u32_t uFetchedPos = 0;

	for(;;) {
		/* (1) Fetch-add on the split position */
		uFetchedPos = axmm_atomic_add32( &p->uSplitPos, 1 );
		if( ~uFetchedPos & 0x80000000 ) {
			break;
		}

		/* (2) High-bit is set (a lock is held); wait for release */
		do {
			axmm_spin();
			uFetchedPos = *axmm__c( volatile axmm_u32_t * )( &p->uSplitPos );
		} while( uFetchedPos & 0x80000000 );
	}

	/* (3) Split position is out of range; out of memory */
	if( uFetchedPos >= p->cMaxBlocks ) {
		return AXMM_NULL(void);
	}

	/* (4) Set the tag for this block */
	uBlockIndex = p->blockIndexes[ uFetchedPos ] & ( AXMM_MAX_INDEXES - 1 );
	p->blockIndexes[ uFetchedPos ] =
		uBlockIndex |
		axmm__c( AXMM_BLOCK_INDEX_TYPE )( uTag << AXMM_BLOCK_INDEX_BITS );

	/* Done */
	return
		axmm__rc( void * )(
			((axmm_size_t)p->pBaseBlock) +
			((axmm_size_t)uBlockIndex)*AXMM_BLOCK_SIZE
		);
}
#else
;
#endif

AXMM_FUNC axmm_u32_t AXMM_CALL axmm__phased_lock( axmm_phased_heap_t *p )
#if AXMM_IMPLEMENT
{
	axmm_u32_t uSplitPos;

	for(;;) {
		uSplitPos = p->uSplitPos;

		uSplitPos = axmm_atomic_cmpxchg32( &p->uSplitPos, uSplitPos|0x80000000, uSplitPos&0x7FFFFFFF );
		if( uSplitPos & 0x80000000 ) {
			axmm_spin();
			continue;
		}

		return uSplitPos;
	}
}
#else
;
#endif
AXMM_FUNC void AXMM_CALL axmm__phased_unlock( axmm_phased_heap_t *p, axmm_u32_t uSplitPos )
#if AXMM_IMPLEMENT
{
	axmm_atomic_xchg32( &p->uSplitPos, uSplitPos );
}
#else
;
#endif

/* Allocate memory from the phased-heap */
AXMM_FUNC void *AXMM_CALL axmm_phased_alloc( axmm_phased_heap_t *p, unsigned uTag, axmm_size_t cBytes )
#if AXMM_IMPLEMENT
{
	axmm__phased_heap_thread_t *t;
	axmm__phased_block_t *tb;
	void *u;

	if( cBytes%16 != 0 ) {
		cBytes += 16 - cBytes%16;
	}

	if( cBytes > AXMM_BLOCK_SIZE ) {
		/* FIXME: Use contiguous allocation method instead of erroring */
		/*
		axmm__phased_lock( p );
		② Search for enough contiguous blocks, and remove them from that
		`  section of the array. (Again, order does not matter, but to speed up
		`  the algorithm it is preferred that these blocks are kept in order.)
		③ Adjust the split position (the allocated blocks should be in the
		`  "used" section rather than the "free" section now). 
		axmm__phased_unlock( p );
		*/
		return AXMM_NULL(void);
	}

	t = axmm_phased_thread( p );
	if( !t ) {
		/* ERROR: Cannot make an allocation from this thread */
		return AXMM_NULL(void);
	}

	tb = &t->workingBlocks[ uTag ];
	if( tb->cBytesUsed + cBytes > AXMM_BLOCK_SIZE ) {
		void *b;

		b = axmm__phased_block_fetch( p, uTag );
		if( !b ) {
			/* ERROR: Out of memory */
			return AXMM_NULL(void);
		}

		u = b;
		if( cBytes < tb->cBytesUsed ) {
			tb->pBlockBase = b;
			tb->cBytesUsed = axmm__c( axmm_u32_t )( cBytes );
		}

		axmm__phased_commit( b, AXMM_BLOCK_SIZE );
	} else {
		u = axmm__rc( void * )( ( axmm__rc( axmm_size_t )( tb->pBlockBase ) ) + tb->cBytesUsed );
		tb->cBytesUsed += axmm__c( axmm_u32_t )( cBytes );
	}

	return u;
}
#else
;
#endif

/* Free all memory associated with a given phase */
AXMM_FUNC void AXMM_CALL axmm_phased_free( axmm_phased_heap_t *p, unsigned uTag )
#if AXMM_IMPLEMENT
{
	AXMM_BLOCK_INDEX_TYPE uBlockIndex;
	axmm_u32_t uSplitPos;
	axmm_u32_t uIndexPlusOne, i;
	unsigned uTestTag;

	/*
		TODO: axmm__phased_decommit() calls
	*/

	uSplitPos = axmm__phased_lock( p );
	/*printf( "Freeing tag %u\n", uTag );*/
	for( uIndexPlusOne = uSplitPos; uIndexPlusOne > 0; --uIndexPlusOne ) {
		i = uIndexPlusOne - 1;

		/*printf( "Working on block %u of %u\n", i, uSplitPos );*/

		uTestTag = axmm__c( unsigned )( p->blockIndexes[ i ] >> AXMM_BLOCK_INDEX_BITS );
		/*printf( "  Tag: %u (%.1Xh) (testing for %u)\n", uTestTag, uTestTag, uTag );*/
		if( uTestTag != uTag ) {
			continue;
		}

		uBlockIndex = p->blockIndexes[ i ] & AXMM_BLOCK_INDEX_MASK;
		/*
		printf( "  Idx: %u (%.2Xh)\n", ( unsigned )uBlockIndex, ( unsigned )uBlockIndex );
		printf( "  -- Swapping %u (%.4Xh) with %u (%.4Xh)\n", i, ( unsigned )p->BlockIndexes[ i ], uSplitPos - 1, ( unsigned )p->BlockIndexes[ uSplitPos - 1 ] );
		*/
		--uSplitPos;
		p->blockIndexes[ i ] = p->blockIndexes[ uSplitPos ];
		p->blockIndexes[ uSplitPos ] = uBlockIndex;
	}
	for( i = 0; i < AXMM_MAX_THREADS; ++i ) {
		p->threads[ i ].workingBlocks[ uTag ].pBlockBase = AXMM_NULL(void);
		p->threads[ i ].workingBlocks[ uTag ].cBytesUsed = AXMM_BLOCK_SIZE;
	}
	axmm__phased_unlock( p, uSplitPos );
}
#else
;
#endif

AXMM__LEAVE_C

#if AXMM_CXX_ENABLED
namespace ax
{

	class CPhasedHeap: private axmm_phased_heap_t
	{
	public:
		AXMM_FORCEINLINE CPhasedHeap()
		: axmm_phased_heap_t()
		{
			axmm_phased_init( this, AXMM_NULL(void), 0 );
		}
		AXMM_FORCEINLINE CPhasedHeap( void *pBaseAddr, axmm_size_t cBaseBytes )
		: axmm_phased_heap_t()
		{
			axmm_phased_init( this, pBaseAddr, cBaseBytes );
		}
		AXMM_FORCEINLINE ~CPhasedHeap()
		{
			axmm_phased_fini( this );
		}

		AXMM_FORCEINLINE void *alloc( unsigned uTag, axmm_size_t cBytes )
		{
			return axmm_phased_alloc( this, uTag, cBytes );
		}
		AXMM_FORCEINLINE void freeTag( unsigned uTag )
		{
			axmm_phased_free( this, uTag );
		}
	};

}
#endif




#endif
