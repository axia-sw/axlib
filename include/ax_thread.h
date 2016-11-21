/*


	ax_thread - public domain
	Last update: 2015-10-01 Aaron Miller


	This library provides atomic operations, threading synchronization and
	management routines, and miscellaneous processor queries.


	USAGE
	=====

	Define AXTHREAD_IMPLEMENTATION in exactly one source file that includes this
	header, before including it.

	The following don't need to be defined, as default definitions will be
	provided, but can be defined if you want to alter default functionality
	without modifying this file.

	AXTHREAD_FUNC and AXTHREAD_CALL control the function declaration and calling
	conventions. Ensure that all source files including this use the same
	definitions for these. (That can be done in your Makefile or project
	settings.)


	CONFIGURATION MACROS
	====================

	Define any of these to either 1 or 0 prior to including this header, if you
	want to alter the default functionality.

		AXTHREAD_CXX_ENABLED
		------------------
		Enables the C++ parts of this. All C++ code is in the ax namespace. If
		C++ is enabled then the pointer size is guaranteed to be accurate.


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

#ifndef INCGUARD_AX_THREAD_H_
#define INCGUARD_AX_THREAD_H_

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

#ifdef AXTHREAD_IMPLEMENTATION
# define AXTHREAD_IMPLEMENT         1
#else
# define AXTHREAD_IMPLEMENT         0
#endif

#ifndef AXTHREAD_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXTHREAD_CXX_ENABLED      AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXTHREAD_CXX_ENABLED      1
# else
#  define AXTHREAD_CXX_ENABLED      0
# endif
#endif

#ifndef AXTHREAD__ENTER_C
# ifdef __cplusplus
#  define AXTHREAD__ENTER_C         extern "C" {
#  define AXTHREAD__LEAVE_C         }
# else
#  define AXTHREAD__ENTER_C
#  define AXTHREAD__LEAVE_C
# endif
#endif

#ifndef AXTHREAD_FUNC
# ifdef AX_FUNC
#  define AXTHREAD_FUNC             AX_FUNC
# else
#  define AXTHREAD_FUNC             extern
# endif
#endif
#ifndef AXTHREAD_CALL
# ifdef AX_CALL
#  define AXTHREAD_CALL             AX_CALL
# else
#  define AXTHREAD_CALL
# endif
#endif

#ifndef AXTHREAD_INLINE
# ifdef AX_FORCEINLINE
#  define AXTHREAD_INLINE           AX_FORCEINLINE
# elif AXTHREAD_CXX_ENABLED
#  define AXTHREAD_INLINE           inline
# else
#  define AXTHREAD_INLINE           static
# endif
#endif

#ifndef AXTHREAD_DECLTYPE_ENABLED
# if defined( AX_CXX_N2343 )
#  if AX_CXX_N2343
#   define AXTHREAD_DECLTYPE_ENABLED 1
#  else
#   define AXTHREAD_DECLTYPE_ENABLED 0
#  endif
# elif defined( __has_feature )
#  if __has_feature( cxx_decltype )
#   define AXTHREAD_DECLTYPE_ENABLED 1
#  else
#   define AXTHREAD_DECLTYPE_ENABLED 0
#  endif
# elif defined( __GNUC__ ) && ( __GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 3 ) )
#  define AXTHREAD_DECLTYPE_ENABLED  1
# elif defined( _MSC_VER ) && _MSC_VER >= 1600
#  define AXTHREAD_DECLTYPE_ENABLED  1
# elif defined( __INTEL_COMPILER ) && __INTEL_COMPILER >= 1200
#  define AXTHREAD_DECLTYPE_ENABLED  1
# else
#  define AXTHREAD_DECLTYPE_ENABLED  0
# endif
#endif

#ifndef AXTHREAD_NULLPTR_ENABLED
# if defined( AX_CXX_N2343 )
#  if AX_CXX_N2431
#   define AXTHREAD_NULLPTR_ENABLED 1
#  else
#   define AXTHREAD_NULLPTR_ENABLED 0
#  endif
# elif defined( __has_feature )
#  if __has_feature( cxx_nullptr )
#   define AXTHREAD_NULLPTR_ENABLED 1
#  else
#   define AXTHREAD_NULLPTR_ENABLED 0
#  endif
# elif defined( __GNUC__ ) && ( __GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 6 ) )
#  define AXTHREAD_NULLPTR_ENABLED  1
# elif defined( _MSC_VER ) && _MSC_VER >= 1600
#  define AXTHREAD_NULLPTR_ENABLED  1
# elif defined( __INTEL_COMPILER ) && __INTEL_COMPILER >= 1210
#  define AXTHREAD_NULLPTR_ENABLED  1
# else
#  define AXTHREAD_NULLPTR_ENABLED  0
# endif
#endif

#ifndef AXTHREAD_DECLTYPE_NULLPTR_ENABLED
# if AXTHREAD_DECLTYPE_ENABLED && AXTHREAD_NULLPTR_ENABLED
#  define AXTHREAD_DECLTYPE_NULLPTR_ENABLED 1
# else
#  define AXTHREAD_DECLTYPE_NULLPTR_ENABLED 0
# endif
#endif

#ifndef AXTHREAD_TYPES_DEFINED
# define AXTHREAD_TYPES_DEFINED     1
# if AX_TYPES_DEFINED
typedef ax_u64_t                    axth_u64_t;
typedef ax_s64_t                    axth_s64_t;
typedef ax_u32_t                    axth_u32_t;
typedef ax_s32_t                    axth_s32_t;
typedef ax_sint_t                   axth_sint_t;
typedef ax_uint_t                   axth_uint_t;
typedef ax_uptr_t                   axth_size_t;
# elif defined( _MSC_VER )
#  include <stddef.h>
typedef unsigned __int64            axth_u64_t;
typedef signed __int64              axth_s64_t;
typedef unsigned __int32            axth_u32_t;
typedef signed __int32              axth_s32_t;
typedef signed                      axth_sint_t;
typedef unsigned                    axth_uint_t;
typedef size_t                      axth_size_t;
# else
#  include <stdint.h>
#  include <stddef.h>
typedef uint64_t                    axth_u64_t;
typedef int64_t                     axth_s64_t;
typedef uint32_t                    axth_u32_t;
typedef int32_t                     axth_s32_t;
typedef int_fast32_t                axth_sint_t;
typedef uint_fast32_t               axth_uint_t;
typedef size_t                      axth_size_t;
# endif
#endif
#ifdef ax_static_assert
ax_static_assert( sizeof( axth_s64_t ) == 8, "ax_thread: size mismatch" );
ax_static_assert( sizeof( axth_u64_t ) == 8, "ax_thread: size mismatch" );
ax_static_assert( sizeof( axth_s32_t ) == 4, "ax_thread: size mismatch" );
ax_static_assert( sizeof( axth_u32_t ) == 4, "ax_thread: size mismatch" );
ax_static_assert( sizeof( axth_sint_t ) >= 4, "ax_thread: size mismatch" );
ax_static_assert( sizeof( axth_uint_t ) >= 4, "ax_thread: size mismatch" );
ax_static_assert( sizeof( axth_size_t ) == sizeof( void * ), "ax_thread: size mismatch" );
#endif

#ifndef AXTHREAD_SPIN_CPUPAUSE_ENABLED
# define AXTHREAD_SPIN_CPUPAUSE_ENABLED 1
#endif

#ifndef AXTHREAD_DEFAULT_SPIN_COUNT
# if AXTHREAD_SPIN_CPUPAUSE_ENABLED
#  define AXTHREAD_DEFAULT_SPIN_COUNT 256
# else
#  define AXTHREAD_DEFAULT_SPIN_COUNT 1536
# endif
#endif
#ifndef AXTHREAD_MAX_BACKOFF_SPIN_COUNT
# define AXTHREAD_MAX_BACKOFF_SPIN_COUNT 16384
#endif

#ifndef axth_atexit
# include <stdlib.h>
# define axth_atexit                atexit
#endif
#ifndef AXTHREAD_ATEXIT_FUNC
# define AXTHREAD_ATEXIT_FUNC       static
#endif
#ifndef AXTHREAD_ATEXIT_CALL
# define AXTHREAD_ATEXIT_CALL       __cdecl
#endif

/*
	You can replace this with, e.g., `throw Error(Msg_)` or `assert(0 && Msg_)`.
	Otherwise errors in initialization of C++ classes will go ignored.
*/
#ifndef axth_cxx_error
# ifdef AX_EXPECT_MSG
#  define axth_cxx_error(Msg_)      AX_EXPECT_MSG( false, Msg_ )
# else
#  define axth_cxx_error(Msg_)      ((void)0)
# endif
#endif

/*
----------------
Architecture
----------------
*/
#ifndef AXTHREAD_ARCH_DEFINED
# ifdef INCGUARD_AX_PLATFORM_H_
#  define AXTHREAD_ARCH_X86         AX_ARCH_X86
#  define AXTHREAD_ARCH_ARM         AX_ARCH_ARM
#  define AXTHREAD_ARCH_64BIT       AX_ARCH_64BIT
# else
#  define AXTHREAD_ARCH_X86         0
#  define AXTHREAD_ARCH_ARM         0
#  define AXTHREAD_ARCH_64BIT       0
#  if defined( __amd64__ ) || defined( __x86_64__ ) || defined( _M_AMD64 )
#   undef AXTHREAD_ARCH_X86
#   undef AXTHREAD_ARCH_64BIT
#   define AXTHREAD_ARCH_X86        1
#   define AXTHREAD_ARCH_64BIT      1
#  elif defined( __x86__ ) || defined( __i386__ ) || defined( _M_IX86 )
#   undef AXTHREAD_ARCH_X86
#   define AXTHREAD_ARCH_X86        1
#  elif defined( __arm__ )
#   undef AXTHREAD_ARCH_ARM
#   define AXTHREAD_ARCH_ARM        1
#  endif
# endif
# if !(AXTHREAD_ARCH_X86|AXTHREAD_ARCH_ARM)
#  error Unknown architecture
# endif
# define AXTHREAD_ARCH_DEFINED      1
#endif

/*
----------------
Operating System
----------------
*/
#ifndef AXTHREAD_OS_DEFINED
# ifdef INCGUARD_AX_PLATFORM_H_
#  define AXTHREAD_OS_WINDOWS       AX_OS_WINDOWS
#  define AXTHREAD_OS_LINUX         AX_OS_LINUX
#  define AXTHREAD_OS_MACOSX        AX_OS_MACOSX
# else
#  define AXTHREAD_OS_WINDOWS       0
#  define AXTHREAD_OS_LINUX         0
#  define AXTHREAD_OS_MACOSX        0
#  if defined( _WIN32 )
#   undef AXTHREAD_OS_WINDOWS
#   define AXTHREAD_OS_WINDOWS      1
#  elif defined( linux ) || defined( __linux ) || defined( __linux__ )
#   undef AXTHREAD_OS_LINUX
#   define AXTHREAD_OS_LINUX        1
#  elif defined( __APPLE__ )
#   undef AXTHREAD_OS_MACOSX
#   define AXTHREAD_OS_MACOSX       1
#  endif
# endif
# if !(AXTHREAD_OS_WINDOWS|AXTHREAD_OS_LINUX|AXTHREAD_OS_MACOSX)
#  error Unknown operating system
# endif
# define AXTHREAD_OS_DEFINED        1
#endif

/*
----------------
Threading Model
----------------
*/
#ifndef AXTHREAD_MODEL_DEFINED
# define AXTHREAD_MODEL_WINDOWS     0
# define AXTHREAD_MODEL_PTHREAD     0
# if AXTHREAD_OS_WINDOWS
#  undef AXTHREAD_MODEL_WINDOWS
#  define AXTHREAD_MODEL_WINDOWS    1
# else
#  undef AXTHREAD_MODEL_PTHREAD
#  define AXTHREAD_MODEL_PTHREAD    1
# endif
# define AXTHREAD_MODEL_DEFINED     1
#endif




/*
===============================================================================

	PLATFORM THREADING MODEL

===============================================================================
*/

#undef _GNU_SOURCE
#define _GNU_SOURCE                 1

/*
----------------
====Windows ====
----------------
*/

#if AXTHREAD_MODEL_WINDOWS

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

# include <intrin.h>
# ifdef _MSC_VER
#  pragma intrinsic( _ReadWriteBarrier )
# endif

# define AXTHREAD_ASYNCCALL         __stdcall
typedef DWORD                       axthread_return_t;

/*
----------------
====PThreads====
----------------
*/

#elif AXTHREAD_MODEL_PTHREAD

# include <pthread.h>
# include <semaphore.h>

# define AXTHREAD_ASYNCCALL
typedef void *                      axthread_return_t;

/*
----------------
====*ERROR!*====
----------------
*/

#else
# error No threading model is active
#endif




/*
===============================================================================

	ATOMICS

===============================================================================
*/

/*
----------------
Memory Ordering
----------------
*/

#if defined( _MSC_VER )
# include <intrin.h>
# pragma intrinsic( _ReadWriteBarrier )
# pragma intrinsic( _mm_mfence )
#elif defined( __GNUC__ ) || defined( __clang__ )
#endif

#ifndef AX_COMPILER_FENCE
# if defined( _MSC_VER )
#  define AX_COMPILER_FENCE()       _ReadWriteBarrier()
# elif defined( __GNUC__ ) || defined( __clang__ )
#  define AX_COMPILER_FENCE()       __sync_synchronize() /*__asm__ __volatile__( "" : : : "memory" )*/
# else
#  error Could not determine how to define AX_COMPILER_FENCE()
# endif
#endif

#ifndef AX_PLATFORM_FENCE
# if defined( _MSC_VER )
#  if AXTHREAD_ARCH_X86
#   define AX_PLATFORM_FENCE()      _mm_mfence()
#  endif
# elif defined( __GNUC__ ) || defined( __clang__ )
#  if AXTHREAD_ARCH_X86
#   define AX_PLATFORM_FENCE()      __asm__ __volatile__( "mfence" : : : "memory" )
#  endif
# else
#  error Could not determine how to define AX_PLATFORM_FENCE()
# endif
#endif

#ifndef AX_MEMORY_BARRIER
# if defined( AX_PLATFORM_FENCE )
#  if defined( AX_COMPILER_FENCE )
#   define AX_MEMORY_BARRIER()      AX_COMPILER_FENCE(); AX_PLATFORM_FENCE()
#  else
#   define AX_MEMORY_BARRIER()      AX_PLATFORM_FENCE()
#  endif
# else
#  error Could not determine how to define AX_MEMORY_BARRIER()
# endif
#endif

/* Memory ordering semantics */
typedef enum axthread_memorder_e
{
	/* use acquire semantics; operation's effects are visible prior to any
	   operation after */
	axthread_mem_acquire,
	/* use release semantics; operation's effects are visible after any
	   operation prior */
	axthread_mem_release,
	/* use acquire and release (full); operation's effects are visible to any
	   operation after, and after any operation prior */
	axthread_mem_full,
	/* no memory semantics are explicitly imposed; it doesn't matter when the
	   operation's effects are visible as long as it happens eventually */
	axthread_mem_relaxed
} axthread_memorder_t;

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	struct SMemorySemantic
	{
	};

	namespace EMemorySemantic
	{
		enum Type
		{
			Acquire = axthread_mem_acquire,
			Release = axthread_mem_release,
			Full    = axthread_mem_full,
			Relaxed = axthread_mem_relaxed
		};
	}

	namespace Mem
	{

		/// Uses acquire semantics (operation's effects are visible prior to any
		/// operation after)
		struct Acquire: public SMemorySemantic
		{
			static const EMemorySemantic::Type value = EMemorySemantic::Acquire;
		};
		/// Uses release semantics (operation's effects are visible after any
		/// operation prior)
		struct Release: public SMemorySemantic
		{
			static const EMemorySemantic::Type value = EMemorySemantic::Release;
		};
		/// Uses acquire and release (full) semantics (operation's effects are
		/// visible prior to any operation after and after any operation prior)
		struct Full: public SMemorySemantic
		{
			static const EMemorySemantic::Type value = EMemorySemantic::Full;
		};
		/// No memory semantics are explicitly imposed (it doesn't matter when
		/// the operation's effects are visible as long as it happens
		/// eventually)
		struct Relaxed: public SMemorySemantic
		{
			static const EMemorySemantic::Type value = EMemorySemantic::Relaxed;
		};

	}

}
#endif /*AXTHREAD_CXX_ENABLED*/

/*
----------------
MSVC++ Atomics
----------------
*/

#if defined( _MSC_VER )

# define AX_ATOMIC_EXCHANGE_FULL32( Dst, Src )\
	( ( _InterlockedExchange( ( volatile long * )( Dst ), ( long )( Src ) ) ) )
# define AX_ATOMIC_COMPARE_EXCHANGE_FULL32( Dst, Src, Cmp )\
	( ( axth_u32_t )( _InterlockedCompareExchange( ( volatile long * )( Dst ), ( long )( Src ), ( long )( Cmp ) ) ) )

# define AX_ATOMIC_FETCH_ADD_FULL32( Dst, Src )\
	( ( axth_u32_t )( _InterlockedExchangeAdd( ( volatile long * )( Dst ), ( long )( Src ) ) ) )
# define AX_ATOMIC_FETCH_SUB_FULL32( Dst, Src )\
	( ( axth_u32_t )( _InterlockedExchangeAdd( ( volatile long * )( Dst ), -( long )( Src ) ) ) )
# define AX_ATOMIC_FETCH_AND_FULL32( Dst, Src )\
	( ( axth_u32_t )( _InterlockedAnd( ( volatile long * )( Dst ), ( long )( Src ) ) ) )
# define AX_ATOMIC_FETCH_OR_FULL32( Dst, Src )\
	( ( axth_u32_t )( _InterlockedOr( ( volatile long * )( Dst ), ( long )( Src ) ) ) )
# define AX_ATOMIC_FETCH_XOR_FULL32( Dst, Src )\
	( ( axth_u32_t )( _InterlockedXor( ( volatile long * )( Dst ), ( long )( Src ) ) ) )


# if defined( _WIN64 ) || ( defined( AX_ARCH ) && ( AX_ARCH & AX_ARCH_64 ) )

#  define AX_ATOMIC_EXCHANGE_FULL64( Dst, Src )\
	( ( axth_u64_t )( _InterlockedExchange64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )
#  define AX_ATOMIC_COMPARE_EXCHANGE_FULL64( Dst, Src, Cmp )\
	( ( axth_u64_t )( _InterlockedCompareExchange64( ( volatile long long * )( Dst ), ( long long )( Src ), ( long long )( Cmp ) ) ) )

#  define AX_ATOMIC_FETCH_ADD_FULL64( Dst, Src )\
	( ( axth_u64_t )( _InterlockedExchangeAdd64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )
#  define AX_ATOMIC_FETCH_SUB_FULL64( Dst, Src )\
	( ( axth_u64_t )( _InterlockedExchangeAdd64( ( volatile long long * )( Dst ), -( long long )( Src ) ) ) )
#  define AX_ATOMIC_FETCH_AND_FULL64( Dst, Src )\
	( ( axth_u64_t )( _InterlockedAnd64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )
#  define AX_ATOMIC_FETCH_OR_FULL64( Dst, Src )\
	( ( axth_u64_t )( _InterlockedOr64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )
#  define AX_ATOMIC_FETCH_XOR_FULL64( Dst, Src )\
	( ( axth_u64_t )( _InterlockedXor64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )

# else

#  define AX_ATOMIC_EXCHANGE_FULL64( Dst, Src )\
	( ( axth_u64_t )( InterlockedExchange64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )
#  define AX_ATOMIC_COMPARE_EXCHANGE_FULL64( Dst, Src, Cmp )\
	( ( axth_u64_t )( InterlockedCompareExchange64( ( volatile long long * )( Dst ), ( long long )( Src ), ( long long )( Cmp ) ) ) )

#  define AX_ATOMIC_FETCH_ADD_FULL64( Dst, Src )\
	( ( axth_u64_t )( InterlockedExchangeAdd64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )
#  define AX_ATOMIC_FETCH_SUB_FULL64( Dst, Src )\
	( ( axth_u64_t )( InterlockedExchangeAdd64( ( volatile long long * )( Dst ), -( long long )( Src ) ) ) )
#  define AX_ATOMIC_FETCH_AND_FULL64( Dst, Src )\
	( ( axth_u64_t )( InterlockedAnd64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )
#  define AX_ATOMIC_FETCH_OR_FULL64( Dst, Src )\
	( ( axth_u64_t )( InterlockedOr64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )
#  define AX_ATOMIC_FETCH_XOR_FULL64( Dst, Src )\
	( ( axth_u64_t )( InterlockedXor64( ( volatile long long * )( Dst ), ( long long )( Src ) ) ) )

# endif /*_WIN64 || AX_ARCH*/

# define AX_ATOMIC_EXCHANGE_FULLPTR( Dst, Src )\
	( ( void * )( _InterlockedExchangePointer( ( void *volatile * )( Dst ), ( void * )( Src ) ) ) )
# define AX_ATOMIC_COMPARE_EXCHANGE_FULLPTR( Dst, Src, Cmp )\
	( ( void * )( _InterlockedCompareExchangePointer( ( void *volatile * )( Dst ), ( void * )( Src ), ( void * )( Cmp ) ) ) )

/*
----------------
GCC/Clang Atomics
----------------
*/

#elif defined( __GNUC__ ) || defined( __clang__ )

# define AX_ATOMIC_EXCHANGE_FULL32( Dst, Src )\
	( ( axth_u32_t )( __sync_lock_test_and_set( ( volatile axth_u32_t * )( Dst ), ( axth_u32_t )( Src ) ) ) )
# define AX_ATOMIC_COMPARE_EXCHANGE_FULL32( Dst, Src, Cmp )\
	( ( axth_u32_t )( __sync_val_compare_and_swap( ( volatile axth_u32_t * )( Dst ), ( axth_u32_t )( Src ), ( axth_u32_t )( Cmp ) ) ) )

# define AX_ATOMIC_FETCH_ADD_FULL32( Dst, Src )\
	( ( axth_u32_t )( __sync_fetch_and_add( ( volatile axth_u32_t * )( Dst ), ( axth_u32_t )( Src ) ) ) )
# define AX_ATOMIC_FETCH_SUB_FULL32( Dst, Src )\
	( ( axth_u32_t )( __sync_fetch_and_sub( ( volatile axth_u32_t * )( Dst ), ( axth_u32_t )( Src ) ) ) )
# define AX_ATOMIC_FETCH_AND_FULL32( Dst, Src )\
	( ( axth_u32_t )( __sync_fetch_and_and( ( volatile axth_u32_t * )( Dst ), ( axth_u32_t )( Src ) ) ) )
# define AX_ATOMIC_FETCH_OR_FULL32( Dst, Src )\
	( ( axth_u32_t )( __sync_fetch_and_or( ( volatile axth_u32_t * )( Dst ), ( axth_u32_t )( Src ) ) ) )
# define AX_ATOMIC_FETCH_XOR_FULL32( Dst, Src )\
	( ( axth_u32_t )( __sync_fetch_and_xor( ( volatile axth_u32_t * )( Dst ), ( axth_u32_t )( Src ) ) ) )


# define AX_ATOMIC_EXCHANGE_FULL64( Dst, Src )\
	( ( axth_u64_t )( __sync_lock_test_and_set( ( volatile axth_u64_t * )( Dst ), ( axth_u64_t )( Src ) ) ) )
# define AX_ATOMIC_COMPARE_EXCHANGE_FULL64( Dst, Src, Cmp )\
	( ( axth_u64_t )( __sync_val_compare_and_swap( ( volatile axth_u64_t * )( Dst ), ( axth_u64_t )( Src ), ( axth_u64_t )( Cmp ) ) ) )

# define AX_ATOMIC_FETCH_ADD_FULL64( Dst, Src )\
	( ( axth_u64_t )( __sync_fetch_and_add( ( volatile axth_u64_t * )( Dst ), ( axth_u64_t )( Src ) ) ) )
# define AX_ATOMIC_FETCH_SUB_FULL64( Dst, Src )\
	( ( axth_u64_t )( __sync_fetch_and_sub( ( volatile axth_u64_t * )( Dst ), ( axth_u64_t )( Src ) ) ) )
# define AX_ATOMIC_FETCH_AND_FULL64( Dst, Src )\
	( ( axth_u64_t )( __sync_fetch_and_and( ( volatile axth_u64_t * )( Dst ), ( axth_u64_t )( Src ) ) ) )
# define AX_ATOMIC_FETCH_OR_FULL64( Dst, Src )\
	( ( axth_u64_t )( __sync_fetch_and_or( ( volatile axth_u64_t * )( Dst ), ( axth_u64_t )( Src ) ) ) )
# define AX_ATOMIC_FETCH_XOR_FULL64( Dst, Src )\
	( ( axth_u64_t )( __sync_fetch_and_xor( ( volatile axth_u64_t * )( Dst ), ( axth_u64_t )( Src ) ) ) )


# define AX_ATOMIC_EXCHANGE_FULLPTR( Dst, Src )\
	( ( void * )( __sync_lock_test_and_set( ( void *volatile * )( Dst ), ( void * )( Src ) ) ) )
# define AX_ATOMIC_COMPARE_EXCHANGE_FULLPTR( Dst, Src, Cmp )\
	( ( void * )( __sync_val_compare_and_swap( ( void *volatile * )( Dst ), ( void * )( Src ), ( void * )( Cmp ) ) ) )

/*
----------------
Unknown Atomics
----------------
*/

#else
# error Could not determine how to define atomic operations
#endif


/*
----------------
Atomics: acquire
----------------
*/

#ifndef AX_ATOMIC_EXCHANGE_ACQ32
# define AX_ATOMIC_EXCHANGE_ACQ32( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULL32( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_ACQ32( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULL32( Dst, Src, Cmp )
#endif
#ifndef AX_ATOMIC_FETCH_ADD_ACQ32
# define AX_ATOMIC_FETCH_ADD_ACQ32( Dst, Src )\
	AX_ATOMIC_FETCH_ADD_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_SUB_ACQ32( Dst, Src )\
	AX_ATOMIC_FETCH_SUB_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_AND_ACQ32( Dst, Src )\
	AX_ATOMIC_FETCH_AND_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_OR_ACQ32( Dst, Src )\
	AX_ATOMIC_FETCH_OR_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_XOR_ACQ32( Dst, Src )\
	AX_ATOMIC_FETCH_XOR_FULL32( Dst, Src )
#endif

#ifndef AX_ATOMIC_EXCHANGE_ACQ64
# define AX_ATOMIC_EXCHANGE_ACQ64( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULL64( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_ACQ64( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULL64( Dst, Src, Cmp )
#endif
#ifndef AX_ATOMIC_FETCH_ADD_ACQ64
# define AX_ATOMIC_FETCH_ADD_ACQ64( Dst, Src )\
	AX_ATOMIC_FETCH_ADD_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_SUB_ACQ64( Dst, Src )\
	AX_ATOMIC_FETCH_SUB_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_AND_ACQ64( Dst, Src )\
	AX_ATOMIC_FETCH_AND_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_OR_ACQ64( Dst, Src )\
	AX_ATOMIC_FETCH_OR_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_XOR_ACQ64( Dst, Src )\
	AX_ATOMIC_FETCH_XOR_FULL64( Dst, Src )
#endif

#ifndef AX_ATOMIC_EXCHANGE_ACQPTR
# define AX_ATOMIC_EXCHANGE_ACQPTR( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULLPTR( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_ACQPTR( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULLPTR( Dst, Src, Cmp )
#endif

/*
----------------
Atomics: release
----------------
*/

#ifndef AX_ATOMIC_EXCHANGE_REL32
# define AX_ATOMIC_EXCHANGE_REL32( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULL32( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_REL32( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULL32( Dst, Src, Cmp )
#endif
#ifndef AX_ATOMIC_FETCH_ADD_REL32
# define AX_ATOMIC_FETCH_ADD_REL32( Dst, Src )\
	AX_ATOMIC_FETCH_ADD_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_SUB_REL32( Dst, Src )\
	AX_ATOMIC_FETCH_SUB_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_AND_REL32( Dst, Src )\
	AX_ATOMIC_FETCH_AND_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_OR_REL32( Dst, Src )\
	AX_ATOMIC_FETCH_OR_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_XOR_REL32( Dst, Src )\
	AX_ATOMIC_FETCH_XOR_FULL32( Dst, Src )
#endif

#ifndef AX_ATOMIC_EXCHANGE_REL64
# define AX_ATOMIC_EXCHANGE_REL64( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULL64( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_REL64( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULL64( Dst, Src, Cmp )
#endif
#ifndef AX_ATOMIC_FETCH_ADD_REL64
# define AX_ATOMIC_FETCH_ADD_REL64( Dst, Src )\
	AX_ATOMIC_FETCH_ADD_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_SUB_REL64( Dst, Src )\
	AX_ATOMIC_FETCH_SUB_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_AND_REL64( Dst, Src )\
	AX_ATOMIC_FETCH_AND_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_OR_REL64( Dst, Src )\
	AX_ATOMIC_FETCH_OR_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_XOR_REL64( Dst, Src )\
	AX_ATOMIC_FETCH_XOR_FULL64( Dst, Src )
#endif

#ifndef AX_ATOMIC_EXCHANGE_RELPTR
# define AX_ATOMIC_EXCHANGE_RELPTR( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULLPTR( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_RELPTR( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULLPTR( Dst, Src, Cmp )
#endif

/*
----------------
Atomics: Relaxed (Non-Sequential)
----------------
*/

#ifndef AX_ATOMIC_EXCHANGE_NSQ32
# define AX_ATOMIC_EXCHANGE_NSQ32( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULL32( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_NSQ32( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULL32( Dst, Src, Cmp )
#endif
#ifndef AX_ATOMIC_FETCH_ADD_NSQ32
# define AX_ATOMIC_FETCH_ADD_NSQ32( Dst, Src )\
	AX_ATOMIC_FETCH_ADD_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_SUB_NSQ32( Dst, Src )\
	AX_ATOMIC_FETCH_SUB_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_AND_NSQ32( Dst, Src )\
	AX_ATOMIC_FETCH_AND_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_OR_NSQ32( Dst, Src )\
	AX_ATOMIC_FETCH_OR_FULL32( Dst, Src )
# define AX_ATOMIC_FETCH_XOR_NSQ32( Dst, Src )\
	AX_ATOMIC_FETCH_XOR_FULL32( Dst, Src )
#endif

#ifndef AX_ATOMIC_EXCHANGE_NSQ64
# define AX_ATOMIC_EXCHANGE_NSQ64( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULL64( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_NSQ64( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULL64( Dst, Src, Cmp )
#endif
#ifndef AX_ATOMIC_FETCH_ADD_NSQ64
# define AX_ATOMIC_FETCH_ADD_NSQ64( Dst, Src )\
	AX_ATOMIC_FETCH_ADD_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_SUB_NSQ64( Dst, Src )\
	AX_ATOMIC_FETCH_SUB_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_AND_NSQ64( Dst, Src )\
	AX_ATOMIC_FETCH_AND_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_OR_NSQ64( Dst, Src )\
	AX_ATOMIC_FETCH_OR_FULL64( Dst, Src )
# define AX_ATOMIC_FETCH_XOR_NSQ64( Dst, Src )\
	AX_ATOMIC_FETCH_XOR_FULL64( Dst, Src )
#endif

#ifndef AX_ATOMIC_EXCHANGE_NSQPTR
# define AX_ATOMIC_EXCHANGE_NSQPTR( Dst, Src )\
	AX_ATOMIC_EXCHANGE_FULLPTR( Dst, Src )
# define AX_ATOMIC_COMPARE_EXCHANGE_NSQPTR( Dst, Src, Cmp )\
	AX_ATOMIC_COMPARE_EXCHANGE_FULLPTR( Dst, Src, Cmp )
#endif

/*
----------------
C++ Atomics
----------------
*/

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	/*
	-----------------------------------

		ATOMICS - FULL BARRIER - 32-BIT

	-----------------------------------
	*/
	AXTHREAD_INLINE axth_s32_t atomicSet( volatile axth_s32_t *dst, axth_s32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_EXCHANGE_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicSetEq( volatile axth_s32_t *dst, axth_s32_t src, axth_s32_t cmp, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_FULL32( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_s32_t atomicInc( volatile axth_s32_t *dst, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_ADD_FULL32( dst, 1 );
	}
	AXTHREAD_INLINE axth_s32_t atomicDec( volatile axth_s32_t *dst, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_SUB_FULL32( dst, 1 );
	}
	AXTHREAD_INLINE axth_s32_t atomicAdd( volatile axth_s32_t *dst, axth_s32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_ADD_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicSub( volatile axth_s32_t *dst, axth_s32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_SUB_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicAnd( volatile axth_s32_t *dst, axth_s32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_AND_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicOr( volatile axth_s32_t *dst, axth_s32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_OR_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicXor( volatile axth_s32_t *dst, axth_s32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_XOR_FULL32( dst, src );
	}


	AXTHREAD_INLINE axth_u32_t atomicSet( volatile axth_u32_t *dst, axth_u32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_EXCHANGE_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicSetEq( volatile axth_u32_t *dst, axth_u32_t src, axth_u32_t cmp, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_FULL32( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_u32_t atomicInc( volatile axth_u32_t *dst, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_ADD_FULL32( dst, 1 );
	}
	AXTHREAD_INLINE axth_u32_t atomicDec( volatile axth_u32_t *dst, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_SUB_FULL32( dst, 1 );
	}
	AXTHREAD_INLINE axth_u32_t atomicAdd( volatile axth_u32_t *dst, axth_u32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_ADD_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicSub( volatile axth_u32_t *dst, axth_u32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_SUB_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicAnd( volatile axth_u32_t *dst, axth_u32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_AND_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicOr( volatile axth_u32_t *dst, axth_u32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_OR_FULL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicXor( volatile axth_u32_t *dst, axth_u32_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_XOR_FULL32( dst, src );
	}


	/*
	-----------------------------------

		ATOMICS - FULL BARRIER - 64-BIT

	-----------------------------------
	*/
	AXTHREAD_INLINE axth_s64_t atomicSet( volatile axth_s64_t *dst, axth_s64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_EXCHANGE_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicSetEq( volatile axth_s64_t *dst, axth_s64_t src, axth_s64_t cmp, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_FULL64( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_s64_t atomicInc( volatile axth_s64_t *dst, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_ADD_FULL64( dst, 1 );
	}
	AXTHREAD_INLINE axth_s64_t atomicDec( volatile axth_s64_t *dst, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_SUB_FULL64( dst, 1 );
	}
	AXTHREAD_INLINE axth_s64_t atomicAdd( volatile axth_s64_t *dst, axth_s64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_ADD_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicSub( volatile axth_s64_t *dst, axth_s64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_SUB_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicAnd( volatile axth_s64_t *dst, axth_s64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_AND_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicOr( volatile axth_s64_t *dst, axth_s64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_OR_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicXor( volatile axth_s64_t *dst, axth_s64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_XOR_FULL64( dst, src );
	}


	AXTHREAD_INLINE axth_u64_t atomicSet( volatile axth_u64_t *dst, axth_u64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_EXCHANGE_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicSetEq( volatile axth_u64_t *dst, axth_u64_t src, axth_u64_t cmp, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_FULL64( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_u64_t atomicInc( volatile axth_u64_t *dst, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_ADD_FULL64( dst, 1 );
	}
	AXTHREAD_INLINE axth_u64_t atomicDec( volatile axth_u64_t *dst, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_SUB_FULL64( dst, 1 );
	}
	AXTHREAD_INLINE axth_u64_t atomicAdd( volatile axth_u64_t *dst, axth_u64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_ADD_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicSub( volatile axth_u64_t *dst, axth_u64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_SUB_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicAnd( volatile axth_u64_t *dst, axth_u64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_AND_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicOr( volatile axth_u64_t *dst, axth_u64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_OR_FULL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicXor( volatile axth_u64_t *dst, axth_u64_t src, Mem::Full = Mem::Full() )
	{
		return AX_ATOMIC_FETCH_XOR_FULL64( dst, src );
	}

	/*
	-----------------------------------

		ATOMICS - FULL BARRIER - POINTERS

	-----------------------------------
	*/
	template< typename tDst, typename tSrc >
	AXTHREAD_INLINE tDst *atomicSetPtr( tDst *volatile *dst, tSrc *src, Mem::Full = Mem::Full() )
	{
		return ( tDst * )AX_ATOMIC_EXCHANGE_FULLPTR( dst, src );
	}
#if AXTHREAD_DECLTYPE_NULLPTR_ENABLED
	template< typename tPtr >
	AXTHREAD_INLINE tPtr *atomicSetPtr( tPtr *volatile *dst, decltype( nullptr ), Mem::Full = Mem::Full() )
	{
		return ( tPtr * )AX_ATOMIC_EXCHANGE_FULLPTR( dst, NULL );
	}
#endif

	template< typename tDst, typename tSrc, typename tCmp >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, tSrc *src, tCmp *cmp, Mem::Full = Mem::Full() )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_FULLPTR( dst, src, cmp );
	}
#if AXTHREAD_DECLTYPE_NULLPTR_ENABLED
	template< typename tDst, typename tSrc >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, tSrc *src, decltype( nullptr ), Mem::Full = Mem::Full() )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_FULLPTR( dst, src, NULL );
	}
	template< typename tDst, typename tCmp >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, decltype( nullptr ), tCmp *cmp, Mem::Full = Mem::Full() )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_FULLPTR( dst, NULL, cmp );
	}
#endif


	/*
	-----------------------------------

		ATOMICS - ACQUIRE BARRIER - 32-BIT

	-----------------------------------
	*/
	AXTHREAD_INLINE axth_s32_t atomicSet( volatile axth_s32_t *dst, axth_s32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_EXCHANGE_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicSetEq( volatile axth_s32_t *dst, axth_s32_t src, axth_s32_t cmp, Mem::Acquire )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_ACQ32( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_s32_t atomicInc( volatile axth_s32_t *dst, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_ADD_ACQ32( dst, 1 );
	}
	AXTHREAD_INLINE axth_s32_t atomicDec( volatile axth_s32_t *dst, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_SUB_ACQ32( dst, 1 );
	}
	AXTHREAD_INLINE axth_s32_t atomicAdd( volatile axth_s32_t *dst, axth_s32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_ADD_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicSub( volatile axth_s32_t *dst, axth_s32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_SUB_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicAnd( volatile axth_s32_t *dst, axth_s32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_AND_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicOr( volatile axth_s32_t *dst, axth_s32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_OR_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicXor( volatile axth_s32_t *dst, axth_s32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_XOR_ACQ32( dst, src );
	}


	AXTHREAD_INLINE axth_u32_t atomicSet( volatile axth_u32_t *dst, axth_u32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_EXCHANGE_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicSetEq( volatile axth_u32_t *dst, axth_u32_t src, axth_u32_t cmp, Mem::Acquire )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_ACQ32( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_u32_t atomicInc( volatile axth_u32_t *dst, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_ADD_ACQ32( dst, 1 );
	}
	AXTHREAD_INLINE axth_u32_t atomicDec( volatile axth_u32_t *dst, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_SUB_ACQ32( dst, 1 );
	}
	AXTHREAD_INLINE axth_u32_t atomicAdd( volatile axth_u32_t *dst, axth_u32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_ADD_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicSub( volatile axth_u32_t *dst, axth_u32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_SUB_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicAnd( volatile axth_u32_t *dst, axth_u32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_AND_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicOr( volatile axth_u32_t *dst, axth_u32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_OR_ACQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicXor( volatile axth_u32_t *dst, axth_u32_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_XOR_ACQ32( dst, src );
	}


	/*
	-----------------------------------

		ATOMICS - ACQUIRE BARRIER - 64-BIT

	-----------------------------------
	*/
	AXTHREAD_INLINE axth_s64_t atomicSet( volatile axth_s64_t *dst, axth_s64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_EXCHANGE_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicSetEq( volatile axth_s64_t *dst, axth_s64_t src, axth_s64_t cmp, Mem::Acquire )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_ACQ64( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_s64_t atomicInc( volatile axth_s64_t *dst, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_ADD_ACQ64( dst, 1 );
	}
	AXTHREAD_INLINE axth_s64_t atomicDec( volatile axth_s64_t *dst, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_SUB_ACQ64( dst, 1 );
	}
	AXTHREAD_INLINE axth_s64_t atomicAdd( volatile axth_s64_t *dst, axth_s64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_ADD_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicSub( volatile axth_s64_t *dst, axth_s64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_SUB_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicAnd( volatile axth_s64_t *dst, axth_s64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_AND_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicOr( volatile axth_s64_t *dst, axth_s64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_OR_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicXor( volatile axth_s64_t *dst, axth_s64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_XOR_ACQ64( dst, src );
	}


	AXTHREAD_INLINE axth_u64_t atomicSet( volatile axth_u64_t *dst, axth_u64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_EXCHANGE_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicSetEq( volatile axth_u64_t *dst, axth_u64_t src, axth_u64_t cmp, Mem::Acquire )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_ACQ64( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_u64_t atomicInc( volatile axth_u64_t *dst, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_ADD_ACQ64( dst, 1 );
	}
	AXTHREAD_INLINE axth_u64_t atomicDec( volatile axth_u64_t *dst, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_SUB_ACQ64( dst, 1 );
	}
	AXTHREAD_INLINE axth_u64_t atomicAdd( volatile axth_u64_t *dst, axth_u64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_ADD_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicSub( volatile axth_u64_t *dst, axth_u64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_SUB_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicAnd( volatile axth_u64_t *dst, axth_u64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_AND_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicOr( volatile axth_u64_t *dst, axth_u64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_OR_ACQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicXor( volatile axth_u64_t *dst, axth_u64_t src, Mem::Acquire )
	{
		return AX_ATOMIC_FETCH_XOR_ACQ64( dst, src );
	}

	/*
	-----------------------------------

		ATOMICS - ACQUIRE BARRIER - POINTERS

	-----------------------------------
	*/
	template< typename tDst, typename tSrc >
	AXTHREAD_INLINE tDst *atomicSetPtr( tDst *volatile *dst, tSrc *src, Mem::Acquire )
	{
		return ( tDst * )AX_ATOMIC_EXCHANGE_ACQPTR( dst, src );
	}
#if AXTHREAD_DECLTYPE_NULLPTR_ENABLED
	template< typename tPtr >
	AXTHREAD_INLINE tPtr *atomicSetPtr( tPtr *volatile *dst, decltype( nullptr ), Mem::Acquire )
	{
		return ( tPtr * )AX_ATOMIC_EXCHANGE_ACQPTR( dst, NULL );
	}
#endif

	template< typename tDst, typename tSrc, typename tCmp >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, tSrc *src, tCmp *cmp, Mem::Acquire )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_ACQPTR( dst, src, cmp );
	}
#if AXTHREAD_DECLTYPE_NULLPTR_ENABLED
	template< typename tDst, typename tSrc >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, tSrc *src, decltype( nullptr ), Mem::Acquire )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_ACQPTR( dst, src, NULL );
	}
	template< typename tDst, typename tCmp >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, decltype( nullptr ), tCmp *cmp, Mem::Acquire )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_ACQPTR( dst, NULL, cmp );
	}
#endif


	/*
	-----------------------------------

		ATOMICS - RELEASE BARRIER - 32-BIT

	-----------------------------------
	*/
	AXTHREAD_INLINE axth_s32_t atomicSet( volatile axth_s32_t *dst, axth_s32_t src, Mem::Release )
	{
		return AX_ATOMIC_EXCHANGE_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicSetEq( volatile axth_s32_t *dst, axth_s32_t src, axth_s32_t cmp, Mem::Release )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_REL32( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_s32_t atomicInc( volatile axth_s32_t *dst, Mem::Release )
	{
		return AX_ATOMIC_FETCH_ADD_REL32( dst, 1 );
	}
	AXTHREAD_INLINE axth_s32_t atomicDec( volatile axth_s32_t *dst, Mem::Release )
	{
		return AX_ATOMIC_FETCH_SUB_REL32( dst, 1 );
	}
	AXTHREAD_INLINE axth_s32_t atomicAdd( volatile axth_s32_t *dst, axth_s32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_ADD_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicSub( volatile axth_s32_t *dst, axth_s32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_SUB_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicAnd( volatile axth_s32_t *dst, axth_s32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_AND_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicOr( volatile axth_s32_t *dst, axth_s32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_OR_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicXor( volatile axth_s32_t *dst, axth_s32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_XOR_REL32( dst, src );
	}


	AXTHREAD_INLINE axth_u32_t atomicSet( volatile axth_u32_t *dst, axth_u32_t src, Mem::Release )
	{
		return AX_ATOMIC_EXCHANGE_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicSetEq( volatile axth_u32_t *dst, axth_u32_t src, axth_u32_t cmp, Mem::Release )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_REL32( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_u32_t atomicInc( volatile axth_u32_t *dst, Mem::Release )
	{
		return AX_ATOMIC_FETCH_ADD_REL32( dst, 1 );
	}
	AXTHREAD_INLINE axth_u32_t atomicDec( volatile axth_u32_t *dst, Mem::Release )
	{
		return AX_ATOMIC_FETCH_SUB_REL32( dst, 1 );
	}
	AXTHREAD_INLINE axth_u32_t atomicAdd( volatile axth_u32_t *dst, axth_u32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_ADD_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicSub( volatile axth_u32_t *dst, axth_u32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_SUB_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicAnd( volatile axth_u32_t *dst, axth_u32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_AND_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicOr( volatile axth_u32_t *dst, axth_u32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_OR_REL32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicXor( volatile axth_u32_t *dst, axth_u32_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_XOR_REL32( dst, src );
	}


	/*
	-----------------------------------

		ATOMICS - RELEASE BARRIER - 64-BIT

	-----------------------------------
	*/
	AXTHREAD_INLINE axth_s64_t atomicSet( volatile axth_s64_t *dst, axth_s64_t src, Mem::Release )
	{
		return AX_ATOMIC_EXCHANGE_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicSetEq( volatile axth_s64_t *dst, axth_s64_t src, axth_s64_t cmp, Mem::Release )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_REL64( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_s64_t atomicInc( volatile axth_s64_t *dst, Mem::Release )
	{
		return AX_ATOMIC_FETCH_ADD_REL64( dst, 1 );
	}
	AXTHREAD_INLINE axth_s64_t atomicDec( volatile axth_s64_t *dst, Mem::Release )
	{
		return AX_ATOMIC_FETCH_SUB_REL64( dst, 1 );
	}
	AXTHREAD_INLINE axth_s64_t atomicAdd( volatile axth_s64_t *dst, axth_s64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_ADD_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicSub( volatile axth_s64_t *dst, axth_s64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_SUB_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicAnd( volatile axth_s64_t *dst, axth_s64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_AND_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicOr( volatile axth_s64_t *dst, axth_s64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_OR_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicXor( volatile axth_s64_t *dst, axth_s64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_XOR_REL64( dst, src );
	}


	AXTHREAD_INLINE axth_u64_t atomicSet( volatile axth_u64_t *dst, axth_u64_t src, Mem::Release )
	{
		return AX_ATOMIC_EXCHANGE_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicSetEq( volatile axth_u64_t *dst, axth_u64_t src, axth_u64_t cmp, Mem::Release )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_REL64( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_u64_t atomicInc( volatile axth_u64_t *dst, Mem::Release )
	{
		return AX_ATOMIC_FETCH_ADD_REL64( dst, 1 );
	}
	AXTHREAD_INLINE axth_u64_t atomicDec( volatile axth_u64_t *dst, Mem::Release )
	{
		return AX_ATOMIC_FETCH_SUB_REL64( dst, 1 );
	}
	AXTHREAD_INLINE axth_u64_t atomicAdd( volatile axth_u64_t *dst, axth_u64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_ADD_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicSub( volatile axth_u64_t *dst, axth_u64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_SUB_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicAnd( volatile axth_u64_t *dst, axth_u64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_AND_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicOr( volatile axth_u64_t *dst, axth_u64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_OR_REL64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicXor( volatile axth_u64_t *dst, axth_u64_t src, Mem::Release )
	{
		return AX_ATOMIC_FETCH_XOR_REL64( dst, src );
	}

	/*
	-----------------------------------

		ATOMICS - RELEASE BARRIER - POINTERS

	-----------------------------------
	*/
	template< typename tDst, typename tSrc >
	AXTHREAD_INLINE tDst *atomicSetPtr( tDst *volatile *dst, tSrc *src, Mem::Release )
	{
		return ( tDst * )AX_ATOMIC_EXCHANGE_RELPTR( dst, src );
	}
#if AXTHREAD_DECLTYPE_NULLPTR_ENABLED
	template< typename tPtr >
	AXTHREAD_INLINE tPtr *atomicSetPtr( tPtr *volatile *dst, decltype( nullptr ), Mem::Release )
	{
		return ( tPtr * )AX_ATOMIC_EXCHANGE_RELPTR( dst, NULL );
	}
#endif

	template< typename tDst, typename tSrc, typename tCmp >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, tSrc *src, tCmp *cmp, Mem::Release )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_RELPTR( dst, src, cmp );
	}
#if AXTHREAD_DECLTYPE_NULLPTR_ENABLED
	template< typename tDst, typename tSrc >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, tSrc *src, decltype( nullptr ), Mem::Release )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_RELPTR( dst, src, NULL );
	}
	template< typename tDst, typename tCmp >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, decltype( nullptr ), tCmp *cmp, Mem::Release )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_RELPTR( dst, NULL, cmp );
	}
#endif


	/*
	-----------------------------------

		ATOMICS - RELAXED BARRIER - 32-BIT

	-----------------------------------
	*/
	AXTHREAD_INLINE axth_s32_t atomicSet( volatile axth_s32_t *dst, axth_s32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_EXCHANGE_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicSetEq( volatile axth_s32_t *dst, axth_s32_t src, axth_s32_t cmp, Mem::Relaxed )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_NSQ32( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_s32_t atomicInc( volatile axth_s32_t *dst, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_ADD_NSQ32( dst, 1 );
	}
	AXTHREAD_INLINE axth_s32_t atomicDec( volatile axth_s32_t *dst, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_SUB_NSQ32( dst, 1 );
	}
	AXTHREAD_INLINE axth_s32_t atomicAdd( volatile axth_s32_t *dst, axth_s32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_ADD_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicSub( volatile axth_s32_t *dst, axth_s32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_SUB_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicAnd( volatile axth_s32_t *dst, axth_s32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_AND_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicOr( volatile axth_s32_t *dst, axth_s32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_OR_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_s32_t atomicXor( volatile axth_s32_t *dst, axth_s32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_XOR_NSQ32( dst, src );
	}


	AXTHREAD_INLINE axth_u32_t atomicSet( volatile axth_u32_t *dst, axth_u32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_EXCHANGE_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicSetEq( volatile axth_u32_t *dst, axth_u32_t src, axth_u32_t cmp, Mem::Relaxed )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_NSQ32( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_u32_t atomicInc( volatile axth_u32_t *dst, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_ADD_NSQ32( dst, 1 );
	}
	AXTHREAD_INLINE axth_u32_t atomicDec( volatile axth_u32_t *dst, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_SUB_NSQ32( dst, 1 );
	}
	AXTHREAD_INLINE axth_u32_t atomicAdd( volatile axth_u32_t *dst, axth_u32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_ADD_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicSub( volatile axth_u32_t *dst, axth_u32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_SUB_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicAnd( volatile axth_u32_t *dst, axth_u32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_AND_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicOr( volatile axth_u32_t *dst, axth_u32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_OR_NSQ32( dst, src );
	}
	AXTHREAD_INLINE axth_u32_t atomicXor( volatile axth_u32_t *dst, axth_u32_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_XOR_NSQ32( dst, src );
	}


	/*
	-----------------------------------

		ATOMICS - RELAXED BARRIER - 64-BIT

	-----------------------------------
	*/
	AXTHREAD_INLINE axth_s64_t atomicSet( volatile axth_s64_t *dst, axth_s64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_EXCHANGE_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicSetEq( volatile axth_s64_t *dst, axth_s64_t src, axth_s64_t cmp, Mem::Relaxed )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_NSQ64( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_s64_t atomicInc( volatile axth_s64_t *dst, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_ADD_NSQ64( dst, 1 );
	}
	AXTHREAD_INLINE axth_s64_t atomicDec( volatile axth_s64_t *dst, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_SUB_NSQ64( dst, 1 );
	}
	AXTHREAD_INLINE axth_s64_t atomicAdd( volatile axth_s64_t *dst, axth_s64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_ADD_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicSub( volatile axth_s64_t *dst, axth_s64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_SUB_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicAnd( volatile axth_s64_t *dst, axth_s64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_AND_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicOr( volatile axth_s64_t *dst, axth_s64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_OR_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_s64_t atomicXor( volatile axth_s64_t *dst, axth_s64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_XOR_NSQ64( dst, src );
	}


	AXTHREAD_INLINE axth_u64_t atomicSet( volatile axth_u64_t *dst, axth_u64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_EXCHANGE_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicSetEq( volatile axth_u64_t *dst, axth_u64_t src, axth_u64_t cmp, Mem::Relaxed )
	{
		return AX_ATOMIC_COMPARE_EXCHANGE_NSQ64( dst, src, cmp );
	}
	AXTHREAD_INLINE axth_u64_t atomicInc( volatile axth_u64_t *dst, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_ADD_NSQ64( dst, 1 );
	}
	AXTHREAD_INLINE axth_u64_t atomicDec( volatile axth_u64_t *dst, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_SUB_NSQ64( dst, 1 );
	}
	AXTHREAD_INLINE axth_u64_t atomicAdd( volatile axth_u64_t *dst, axth_u64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_ADD_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicSub( volatile axth_u64_t *dst, axth_u64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_SUB_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicAnd( volatile axth_u64_t *dst, axth_u64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_AND_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicOr( volatile axth_u64_t *dst, axth_u64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_OR_NSQ64( dst, src );
	}
	AXTHREAD_INLINE axth_u64_t atomicXor( volatile axth_u64_t *dst, axth_u64_t src, Mem::Relaxed )
	{
		return AX_ATOMIC_FETCH_XOR_NSQ64( dst, src );
	}

	/*
	-----------------------------------

		ATOMICS - RELAXED BARRIER - POINTERS

	-----------------------------------
	*/
	template< typename tDst, typename tSrc >
	AXTHREAD_INLINE tDst *atomicSetPtr( tDst *volatile *dst, tSrc *src, Mem::Relaxed )
	{
		return ( tDst * )AX_ATOMIC_EXCHANGE_NSQPTR( dst, src );
	}
#if AXTHREAD_DECLTYPE_NULLPTR_ENABLED
	template< typename tPtr >
	AXTHREAD_INLINE tPtr *atomicSetPtr( tPtr *volatile *dst, decltype( nullptr ), Mem::Relaxed )
	{
		return ( tPtr * )AX_ATOMIC_EXCHANGE_NSQPTR( dst, NULL );
	}
#endif

	template< typename tDst, typename tSrc, typename tCmp >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, tSrc *src, tCmp *cmp, Mem::Relaxed )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_NSQPTR( dst, src, cmp );
	}
#if AXTHREAD_DECLTYPE_NULLPTR_ENABLED
	template< typename tDst, typename tSrc >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, tSrc *src, decltype( nullptr ), Mem::Relaxed )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_NSQPTR( dst, src, NULL );
	}
	template< typename tDst, typename tCmp >
	AXTHREAD_INLINE tDst *atomicSetPtrEq( tDst *volatile *dst, decltype( nullptr ), tCmp *cmp, Mem::Relaxed )
	{
		return ( tDst * )AX_ATOMIC_COMPARE_EXCHANGE_NSQPTR( dst, NULL, cmp );
	}
#endif


	/*
	-----------------------------------

		ATOMICS - WRAPPER CLASS

	-----------------------------------
	*/
	typedef axth_uint_t atomic_t;

	struct SAtomicInteger
	{
	public:
									// Constructor
		AXTHREAD_INLINE             SAtomicInteger      ( atomic_t value = 0 )
									: mValue( value )
									{
									}
									// Destructor
		AXTHREAD_INLINE             ~SAtomicInteger     ()
									{
									}

									// Increment the current value
		AXTHREAD_INLINE atomic_t    increment           ()
									{
										return atomicInc( &mValue, Mem::Acquire() );
									}
									// Decrement the current value
		AXTHREAD_INLINE atomic_t    decrement           ()
									{
										return atomicDec( &mValue, Mem::Release() );
									}

									// Set the current value
		AXTHREAD_INLINE atomic_t    set                 ( atomic_t x )
									{
										return atomicSet( &mValue, x );
									}
									// Retrieve the current value
		AXTHREAD_INLINE atomic_t    get                 () const
									{
										return mValue;
									}

									// Prefix increment
		AXTHREAD_INLINE
		SAtomicInteger &            operator++          ()
									{
										atomicInc( &mValue, Mem::Acquire() );
										return *this;
									}
									// Postfix increment
		AXTHREAD_INLINE
		SAtomicInteger              operator++          ( int )
									{
										return atomicInc( &mValue, Mem::Acquire() );
									}

									// Prefix decrement
		AXTHREAD_INLINE
		SAtomicInteger &            operator--          ()
									{
										atomicDec( &mValue, Mem::Release() );
										return *this;
									}
									// Postfix decrement
		AXTHREAD_INLINE
		SAtomicInteger              operator--          ( int )
									{
										return atomicDec( &mValue, Mem::Release() );
									}

	private:
		volatile atomic_t           mValue;
	};

}
#endif /*AXTHREAD_CXX_ENABLED*/




/*
===============================================================================

	PROCESSOR FUNCTIONS

===============================================================================
*/

#ifndef AX_CPU_PAUSE
# if AXTHREAD_ARCH_X86
#  if ( defined( _MSC_VER ) && _MSC_VER >= 1300 ) || defined( __INTEL_COMPILER )
#   pragma intrinsic( _mm_pause )
#   define AX_CPU_PAUSE() _mm_pause()
#  elif ( defined( __GNUC__ ) || defined( __clang__ ) )
#   define AX_CPU_PAUSE() __asm__ __volatile__( "pause;" )
#  else
#   error Could not determine how to define AX_CPU_PAUSE() [x86/x64]
#  endif
# else
#  error Could not determine how to define AX_CPU_PAUSE()
# endif
#endif

#if AXTHREAD_IMPLEMENT

# if !AXTHREAD_OS_WINDOWS
#  include <time.h> /* needed for appropriate timeout for interrupted events */
#  include <sched.h> /* for sched_yield() call in axth_yield() */
#  include <unistd.h>
#  include <sys/wait.h>
# endif

# if AXTHREAD_ARCH_X86
typedef enum axth__x86_vendor_e
{
	kAxthreadX86__UnknownVendor,
	kAxthreadX86__Intel,
	kAxthreadX86__AMD
} axth__x86_vendor_t;

typedef struct axth__x86_info_s
{
	char                szBrand             [ 48 ];
	char                szVendor            [ 16 ];
	axth__x86_vendor_t  Vendor;
	axth_u32_t          uLastNormalId;
	axth_u32_t          uLastExtendedId;

	axth_u32_t          uF01ECX;
	axth_u32_t          uF01EDX;
	axth_u32_t          uF07EBX;
	axth_u32_t          uF07ECX;
	axth_u32_t          uF81ECX;
	axth_u32_t          uF81EDX;
} axth__x86_info_t;

static void axth__x86_cpuid( axth_u32_t *pCPUInfo, axth_u32_t uFunc, axth_u32_t uSubfunc )
{
#  if defined( _MSC_VER ) || defined( __INTEL_COMPILER )
	__cpuidex( ( int * )pCPUInfo, uFunc, uSubfunc );
#  elif defined( __GNUC__ ) || defined( __clang__ )
	__asm__ __volatile__
	(
		"cpuid"
		:
			"=a" (pCPUInfo[0]),
			"=b" (pCPUInfo[1]),
			"=c" (pCPUInfo[2]),
			"=d" (pCPUInfo[3])
		:
			"a" (uFunc),
			"c" (uSubfunc),
			"d" (0)
	);
#  else
#   error Could not determine how to implement axth__x86_cpuid()
#  endif
}

static int axth__x86_streq12( const char *a, const char *b )
{
	const axth_u32_t *p, *q;

	p = ( const axth_u32_t * )a;
	q = ( const axth_u32_t * )b;

	return p[0]==q[0] && p[1]==q[1] && p[2]==q[2] && p[3]==q[3];
}
axth__x86_info_t *axth__x86_info( void )
{
	static int didinit = 0;
	static axth__x86_info_t info;

	if( !didinit ) {
		enum { EAX=0, EBX=1, ECX=2, EDX=3 };
		axth_u32_t Regs[ 4 ];

		info.szVendor[ 0 ] = '\0';
		info.szBrand[ 0 ] = '\0';

		info.Vendor = kAxthreadX86__UnknownVendor;
		info.uLastNormalId = 0;
		info.uLastExtendedId = 0;

		info.uF01ECX = 0;
		info.uF01EDX = 0;
		info.uF07EBX = 0;
		info.uF07ECX = 0;
		info.uF81ECX = 0;
		info.uF81EDX = 0;

		/* CPUID( EAX=00h ) → EAX: highest function number; EBX,ECX,EDX: vendor string */
		axth__x86_cpuid( &Regs[0], 0, 0 );
		info.uLastNormalId = Regs[ EAX ];
		*( axth_u32_t * )&info.szVendor[ 0x00 ] = Regs[ EBX ];
		*( axth_u32_t * )&info.szVendor[ 0x04 ] = Regs[ ECX ];
		*( axth_u32_t * )&info.szVendor[ 0x08 ] = Regs[ EDX ];
		*( axth_u32_t * )&info.szVendor[ 0x0C ] = 0;

		if( axth__x86_streq12( info.szVendor, "GenuineIntel" ) ) {
			info.Vendor = kAxthreadX86__Intel;
		} else if( axth__x86_streq12( info.szVendor, "AuthenticAMD" ) ) {
			info.Vendor = kAxthreadX86__AMD;
		}

		/* Get function 01h */
		if( info.uLastNormalId >= 0x01 ) {
			axth__x86_cpuid( Regs, 0x01, 0 );

			info.uF01ECX = Regs[ ECX ];
			info.uF01EDX = Regs[ EDX ];
		}

		/* Get function 07h */
		if( info.uLastNormalId >= 0x07 ) {
			axth__x86_cpuid( Regs, 0x07, 0 );

			info.uF07EBX = Regs[ EBX ];
		}

		/* CPUID( EAX=80000000h ) → EAX: highest extended function number */
		axth__x86_cpuid( Regs, 0x80000000, 0 );
		info.uLastExtendedId = Regs[ EAX ];

		/* Get extended function 01h */
		if( info.uLastExtendedId >= 0x80000001 ) {
			axth__x86_cpuid( Regs, 0x80000001, 0 );
			info.uF81ECX = Regs[ ECX ];
			info.uF81EDX = Regs[ EDX ];
		}

		/* Get brand */
		if( info.uLastExtendedId >= 0x80000004 ) {
			axth__x86_cpuid( Regs, 0x80000002, 0 );
			*( axth_u32_t * )&info.szBrand[ 0x00 ] = Regs[ EAX ];
			*( axth_u32_t * )&info.szBrand[ 0x04 ] = Regs[ EBX ];
			*( axth_u32_t * )&info.szBrand[ 0x08 ] = Regs[ ECX ];
			*( axth_u32_t * )&info.szBrand[ 0x0C ] = Regs[ EDX ];

			axth__x86_cpuid( Regs, 0x80000003, 0 );
			*( axth_u32_t * )&info.szBrand[ 0x10 ] = Regs[ EAX ];
			*( axth_u32_t * )&info.szBrand[ 0x14 ] = Regs[ EBX ];
			*( axth_u32_t * )&info.szBrand[ 0x18 ] = Regs[ ECX ];
			*( axth_u32_t * )&info.szBrand[ 0x1C ] = Regs[ EDX ];

			axth__x86_cpuid( Regs, 0x80000004, 0 );
			*( axth_u32_t * )&info.szBrand[ 0x20 ] = Regs[ EAX ];
			*( axth_u32_t * )&info.szBrand[ 0x24 ] = Regs[ EBX ];
			*( axth_u32_t * )&info.szBrand[ 0x28 ] = Regs[ ECX ];
			*( axth_u32_t * )&info.szBrand[ 0x2C ] = Regs[ EDX ];

			*( axth_u32_t * )&info.szBrand[ 0x30 ] = 0;
		}

		didinit = 1;
	}

	return &info;
}

AXTHREAD_INLINE int axth__x86_F01ECX( axth_u32_t i )    { return ( axth__x86_info()->uF01ECX >> i ) & 1; }
AXTHREAD_INLINE int axth__x86_F01EDX( axth_u32_t i )    { return ( axth__x86_info()->uF01EDX >> i ) & 1; }
AXTHREAD_INLINE int axth__x86_F07EBX( axth_u32_t i )    { return ( axth__x86_info()->uF07EBX >> i ) & 1; }
AXTHREAD_INLINE int axth__x86_F07ECX( axth_u32_t i )    { return ( axth__x86_info()->uF07ECX >> i ) & 1; }
AXTHREAD_INLINE int axth__x86_F81ECX( axth_u32_t i )    { return ( axth__x86_info()->uF81ECX >> i ) & 1; }
AXTHREAD_INLINE int axth__x86_F81EDX( axth_u32_t i )    { return ( axth__x86_info()->uF81EDX >> i ) & 1; }

AXTHREAD_INLINE int axth__x86_is_intel( void )          { return axth__x86_info()->Vendor == kAxthreadX86__Intel; }
AXTHREAD_INLINE int axth__x86_is_amd( void )            { return axth__x86_info()->Vendor == kAxthreadX86__AMD; }

AXTHREAD_INLINE int axth__x86_SSE3( void )              { return axth__x86_F01ECX( 0 ); }
AXTHREAD_INLINE int axth__x86_PCLMULQDQ( void )         { return axth__x86_F01ECX( 1 ); }
AXTHREAD_INLINE int axth__x86_MONITOR( void )           { return axth__x86_F01ECX( 3 ); }
AXTHREAD_INLINE int axth__x86_SSSE3( void )             { return axth__x86_F01ECX( 9 ); }
AXTHREAD_INLINE int axth__x86_FMA( void )               { return axth__x86_F01ECX( 12 ); }
AXTHREAD_INLINE int axth__x86_CMPXCHG16B( void )        { return axth__x86_F01ECX( 13 ); }
AXTHREAD_INLINE int axth__x86_SSE41( void )             { return axth__x86_F01ECX( 19 ); }
AXTHREAD_INLINE int axth__x86_SSE42( void )             { return axth__x86_F01ECX( 20 ); }
AXTHREAD_INLINE int axth__x86_MOVBE( void )             { return axth__x86_F01ECX( 22 ); }
AXTHREAD_INLINE int axth__x86_POPCNT( void )            { return axth__x86_F01ECX( 23 ); }
AXTHREAD_INLINE int axth__x86_AES( void )               { return axth__x86_F01ECX( 25 ); }
AXTHREAD_INLINE int axth__x86_XSAVE( void )             { return axth__x86_F01ECX( 26 ); }
AXTHREAD_INLINE int axth__x86_OSXSAVE( void )           { return axth__x86_F01ECX( 27 ); }
AXTHREAD_INLINE int axth__x86_AVX( void )               { return axth__x86_F01ECX( 28 ); }
AXTHREAD_INLINE int axth__x86_F16C( void )              { return axth__x86_F01ECX( 29 ); }
AXTHREAD_INLINE int axth__x86_RDRAND( void )            { return axth__x86_F01ECX( 30 ); }

AXTHREAD_INLINE int axth__x86_MSR( void )               { return axth__x86_F01EDX( 5 ); }
AXTHREAD_INLINE int axth__x86_CX8( void )               { return axth__x86_F01EDX( 8 ); }
AXTHREAD_INLINE int axth__x86_SEP( void )               { return axth__x86_F01EDX( 11 ); }
AXTHREAD_INLINE int axth__x86_CMOV( void )              { return axth__x86_F01EDX( 15 ); }
AXTHREAD_INLINE int axth__x86_CLFSH( void )             { return axth__x86_F01EDX( 19 ); }
AXTHREAD_INLINE int axth__x86_MMX( void )               { return axth__x86_F01EDX( 23 ); }
AXTHREAD_INLINE int axth__x86_FXSR( void )              { return axth__x86_F01EDX( 24 ); }
AXTHREAD_INLINE int axth__x86_SSE( void )               { return axth__x86_F01EDX( 25 ); }
AXTHREAD_INLINE int axth__x86_SSE2( void )              { return axth__x86_F01EDX( 26 ); }
AXTHREAD_INLINE int axth__x86_HTT( void )               { return axth__x86_is_intel() && axth__x86_F01EDX( 28 ); }

AXTHREAD_INLINE int axth__x86_FSGSBASE( void )          { return axth__x86_F07EBX( 0 ); }
AXTHREAD_INLINE int axth__x86_BMI1( void )              { return axth__x86_F07EBX( 3 ); }
AXTHREAD_INLINE int axth__x86_HLE( void )               { return axth__x86_is_intel() && axth__x86_F07EBX( 4 ); }
AXTHREAD_INLINE int axth__x86_AVX2( void )              { return axth__x86_F07EBX( 5 ); }
AXTHREAD_INLINE int axth__x86_BMI2( void )              { return axth__x86_F07EBX( 8 ); }
AXTHREAD_INLINE int axth__x86_ERMS( void )              { return axth__x86_F07EBX( 9 ); }
AXTHREAD_INLINE int axth__x86_INVPCID( void )           { return axth__x86_F07EBX( 10 ); }
AXTHREAD_INLINE int axth__x86_RTM( void )               { return axth__x86_is_intel() && axth__x86_F07EBX( 11 ); }
AXTHREAD_INLINE int axth__x86_AVX512F( void )           { return axth__x86_F07EBX( 16 ); }
AXTHREAD_INLINE int axth__x86_RDSEED( void )            { return axth__x86_F07EBX( 18 ); }
AXTHREAD_INLINE int axth__x86_ADX( void )               { return axth__x86_F07EBX( 19 ); }
AXTHREAD_INLINE int axth__x86_AVX512PF( void )          { return axth__x86_F07EBX( 26 ); }
AXTHREAD_INLINE int axth__x86_AVX512ER( void )          { return axth__x86_F07EBX( 27 ); }
AXTHREAD_INLINE int axth__x86_AVX512CD( void )          { return axth__x86_F07EBX( 28 ); }
AXTHREAD_INLINE int axth__x86_SHA( void )               { return axth__x86_F07EBX( 29 ); }

AXTHREAD_INLINE int axth__x86_PREFETCHWT1( void )       { return axth__x86_F07ECX( 0 ); }

AXTHREAD_INLINE int axth__x86_LAHF( void )              { return axth__x86_F81ECX( 0 ); }
AXTHREAD_INLINE int axth__x86_LZCNT( void )             { return axth__x86_is_intel() && axth__x86_F81ECX( 5 ); }
AXTHREAD_INLINE int axth__x86_ABM( void )               { return axth__x86_is_amd()   && axth__x86_F81ECX( 5 ); }
AXTHREAD_INLINE int axth__x86_SSE4a( void )             { return axth__x86_is_amd()   && axth__x86_F81ECX( 6 ); }
AXTHREAD_INLINE int axth__x86_XOP( void )               { return axth__x86_is_amd()   && axth__x86_F81ECX( 11 ); }
AXTHREAD_INLINE int axth__x86_TBM( void )               { return axth__x86_is_amd()   && axth__x86_F81ECX( 21 ); }

AXTHREAD_INLINE int axth__x86_SYSCALL( void )           { return axth__x86_is_intel() && axth__x86_F81EDX( 11 ); }
AXTHREAD_INLINE int axth__x86_MMXEXT( void )            { return axth__x86_is_amd()   && axth__x86_F81EDX( 22 ); }
AXTHREAD_INLINE int axth__x86_RDTSCP( void )            { return axth__x86_is_intel() && axth__x86_F81EDX( 27 ); }
AXTHREAD_INLINE int axth__x86_AMD3DNOWEXT( void )       { return axth__x86_is_amd()   && axth__x86_F81EDX( 30 ); }
AXTHREAD_INLINE int axth__x86_AMD3DNOW( void )          { return axth__x86_is_amd()   && axth__x86_F81EDX( 31 ); }

# endif /*AXTHREAD_ARCH_X86*/

#endif /*AXTHREAD_IMPLEMENT*/

AXTHREAD__ENTER_C

AXTHREAD_FUNC int AXTHREAD_CALL axth_has_hyperthreading( void )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_ARCH_X86
	return axth__x86_HTT();
# else
	return 0;
# endif
}
#else
;
#endif

AXTHREAD_FUNC const char *AXTHREAD_CALL axth_get_cpu_vendor( void )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_ARCH_X86
	return axth__x86_info()->szVendor;
# else
#  error Could not determine how to implement axth_get_cpu_vendor()
# endif
}
#else
;
#endif
AXTHREAD_FUNC const char *AXTHREAD_CALL axth_get_cpu_brand( void )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_ARCH_X86
	return axth__x86_info()->szBrand;
# else
#  error Could not determine how to implement axth_get_cpu_brand()
# endif
}
#else
;
#endif

AXTHREAD_FUNC axth_u32_t AXTHREAD_CALL axth_count_cpu_cores( void )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_OS_WINDOWS
		SYSTEM_INFO si;
		GetSystemInfo( &si );
		return ( axth_u32_t )si.dwNumberOfProcessors;
# elif AXTHREAD_OS_LINUX
		int count;

		count = sysconf( _SC_NPROCESSORS_ONLN );
		if( count < 1 ) {
			return 1;
		}

		return ( axth_u32_t )count;
# elif AXTHREAD_OS_MACOSX
		/* TODO! */
		return 1;
# else
#  error Could not determine how to implement axth_count_cpu_cores()
# endif
}
#else
;
#endif
AXTHREAD_FUNC axth_u32_t AXTHREAD_CALL axth_count_cpu_threads( void )
#if AXTHREAD_IMPLEMENT
{
	return axth_count_cpu_cores()*( 1 + axth_has_hyperthreading() );
}
#else
;
#endif

AXTHREAD_FUNC axth_u64_t AXTHREAD_CALL axth_get_cpu_cycles( void )
#if AXTHREAD_IMPLEMENT
{
# if defined( _MSC_VER )
#  if AXTHREAD_ARCH_X86
	return __rdtsc();
#  else
#   error Could not determine how to implement axth_get_cpu_cycles() for MSVC/ARM
#  endif
# elif defined( __GNUC__ ) || defined( __clang__ )
#  if AXTHREAD_ARCH_X86
	axth_u32_t lo, hi;

	__asm__ __volatile__( "rdtsc" : "=a" (lo), "=d" (hi) );

	return (((axth_u64_t)hi)<<32)|((axth_u64_t)lo);
#  else
#   error Could not determine how to implement axth_get_cpu_cycles() for GCC/ARM
#  endif
# else
#  error Could not determine how to implement axth_get_cpu_cycles()
# endif
}
#else
;
#endif
AXTHREAD_FUNC void AXTHREAD_CALL axth_pause_cpu( axth_u32_t cSpins )
#if AXTHREAD_IMPLEMENT
{
	while( cSpins-- > 0 ) {
		AX_CPU_PAUSE();
	}
}
#else
;
#endif

AXTHREAD_FUNC void AXTHREAD_CALL axth_local_spin( axth_u32_t cSpins )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_SPIN_CPUPAUSE_ENABLED
	axth_pause_cpu( cSpins );
# else
	volatile axth_u32_t n;

	n = cSpins;
	while( n > 0 ) {
		--n;
	}
# endif
}
#else
;
#endif
/* Yield to another thread */
AXTHREAD_FUNC void AXTHREAD_CALL axth_yield( void )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_OS_WINDOWS
	SwitchToThread();
# else
	sched_yield();
# endif
}
#else
;
#endif
/*
 * Spins for a given number of iterations, gradually waiting longer each time.
 *
 * [inout]   pcSpins: Holds the current number of spins (iterations)
 * [in]    cMaxSpins: Maximum number of spins; *pcSpins not to exceed
 */
AXTHREAD_FUNC void AXTHREAD_CALL axth_backoff( axth_u32_t *pcSpins, axth_u32_t cMaxSpins )
#if AXTHREAD_IMPLEMENT
{
	axth_u32_t cSpins;

	cSpins = *pcSpins;
	if( cSpins > cMaxSpins ) {
		/* Provide work to something else because we're taking too long */
		axth_yield();
		return;
	}

	/* wait for a little while */
	axth_local_spin( cSpins );
	*pcSpins = cSpins*2 - cSpins/2; /*int equivalent of mul by 1.5*/
}
#else
;
#endif

#if AXTHREAD_IMPLEMENT
# if AXTHREAD_OS_WINDOWS
static DWORD                        axth__g_dwTLS       = TLS_OUT_OF_INDEXES;

AXTHREAD_ATEXIT_FUNC void AXTHREAD_ATEXIT_CALL axth__atexit_f( void )
{
	if( axth__g_dwTLS != TLS_OUT_OF_INDEXES ) {
		TlsFree( axth__g_dwTLS );
		axth__g_dwTLS = TLS_OUT_OF_INDEXES;
	}
}
# else
static __thread axth_u32_t          axth__g_uWorkerId   = ( axth_u32_t )0;
# endif
#endif

/*
 * Set the current thread's worker ID.
 */
AXTHREAD_FUNC void AXTHREAD_CALL axth_set_worker_id( axth_u32_t uWorkerId )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_OS_WINDOWS
	DWORD dwGlobTLS;
	DWORD dwTLS;

	dwGlobTLS = AX_ATOMIC_COMPARE_EXCHANGE_FULL32( &axth__g_dwTLS, 0, 0 );

	if( dwGlobTLS == TLS_OUT_OF_INDEXES ) {
		dwTLS = TlsAlloc();
		if( dwTLS == TLS_OUT_OF_INDEXES ) {
			/* TODO: ERROR CALLBACK */
			return;
		}

		if( AX_ATOMIC_COMPARE_EXCHANGE_FULL32( &axth__g_dwTLS, dwTLS, TLS_OUT_OF_INDEXES ) != TLS_OUT_OF_INDEXES ) {
			TlsFree( dwTLS );
			dwTLS = AX_ATOMIC_COMPARE_EXCHANGE_FULL32( &axth__g_dwTLS, 0, 0 );
		} else {
			atexit( &axth__atexit_f );
		}
	} else {
		/* Doing a direct read is "dangerous" */
		dwTLS = dwGlobTLS;
	}

	TlsSetValue( dwTLS, ( void * )( axth_size_t )uWorkerId );
# else
	axth__g_uWorkerId = uWorkerId;
# endif
}
#else
;
#endif
/*
 * Get the current thread's worker ID
 */
AXTHREAD_FUNC axth_u32_t AXTHREAD_CALL axth_get_worker_id( void )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_OS_WINDOWS
	if( axth__g_dwTLS == TLS_OUT_OF_INDEXES ) {
		return 0;
	}

	return ( axth_u32_t )( axth_size_t )TlsGetValue( axth__g_dwTLS );
# else
	return axth__g_uWorkerId;
# endif
}
#else
;
#endif

AXTHREAD__LEAVE_C

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	AXTHREAD_INLINE bool isHyperthreadingAvailable()
	{
		return !!axth_has_hyperthreading();
	}

	AXTHREAD_INLINE const char *getCPUVendor()
	{
		return axth_get_cpu_vendor();
	}
	AXTHREAD_INLINE const char *getCPUBrand()
	{
		return axth_get_cpu_brand();
	}

	AXTHREAD_INLINE axth_u32_t getCPUCoreCount()
	{
		return axth_count_cpu_cores();
	}
	AXTHREAD_INLINE axth_u32_t getCPUThreadCount()
	{
		return axth_count_cpu_threads();
	}

	AXTHREAD_INLINE axth_u64_t getCPUCycles()
	{
		return axth_get_cpu_cycles();
	}
	AXTHREAD_INLINE void cpuPause( axth_u32_t cSpins )
	{
		axth_pause_cpu( cSpins );
	}

	AXTHREAD_INLINE void yieldThread()
	{
		axth_yield();
	}

	AXTHREAD_INLINE void localSpin( axth_u32_t cSpins = AXTHREAD_DEFAULT_SPIN_COUNT )
	{
		axth_local_spin( cSpins );
	}
	AXTHREAD_INLINE void backoff( axth_u32_t &cSpins, axth_u32_t cMaxSpins = AXTHREAD_MAX_BACKOFF_SPIN_COUNT )
	{
		axth_backoff( &cSpins, cMaxSpins );
	}

}
#endif /*AXTHREAD_CXX_ENABLED*/




/*
===============================================================================

	THREADS

===============================================================================
*/

AXTHREAD__ENTER_C

struct axthread_s;

typedef enum axth_thread_priority_e
{
	kAxthread_Priority_Idle,
	kAxthread_Priority_VeryLow,
	kAxthread_Priority_Low,
	kAxthread_Priority_Normal,
	kAxthread_Priority_High,
	kAxthread_Priority_VeryHigh,
	kAxthread_Priority_Critical
} axth_thread_priority_t;

typedef int( AXTHREAD_CALL *axth_fn_thread_t )( struct axthread_s *, void * );

typedef struct axthread_times_s
{
	/* Time (since app start) the thread was started [microseconds] */
	axth_u64_t uEnterTime;
	/* Time (since app start) the thread was terminated (0 if still active) [microseconds] */
	axth_u64_t uLeaveTime;
	/* Time the thread has spent in kernel mode [microseconds] */
	axth_u64_t uKernelTime;
	/* Time the thread has spent in user mode [microseconds] */
	axth_u64_t uUserTime;
} axthread_times_t;

typedef struct axthread_s
{
#if AXTHREAD_MODEL_WINDOWS
	HANDLE hThread;
	DWORD dwThreadId;
#elif AXTHREAD_MODEL_PTHREAD
	pthread_t thread;
	pid_t threadId;
#else
# error Could not determine how to define axthread_s
#endif
	volatile int iTerminate;
	int iExitValue;
	axth_fn_thread_t pfnThread;
	void *parm;
} axthread_t;

#if AXTHREAD_MODEL_WINDOWS
# define AXTHREAD_INITIALIZER       {(HANDLE)0,(DWORD)0,0,0,((axth_fn_thread_t)0),((void*)0)}
#elif AXTHREAD_MODEL_PTHREAD
# define AXTHREAD_INITIALIZER       {(pthread_t)0,(pid_t)0,0,0,((axth_fn_thread_t)0),((void*)0)}
#else
# error Could not determine how to define AXTHREAD_INITIALIZER
#endif

#if AXTHREAD_IMPLEMENT
static axthread_return_t AXTHREAD_ASYNCCALL axth__thread_f( void *p_ )
{
	axthread_t *p;

	p = ( axthread_t * )p_;

# if AXTHREAD_MODEL_PTHREAD
	p->threadId = getpid();
	AX_MEMORY_BARRIER();
# endif

	p->iExitValue = p->pfnThread( p, p->parm );
	AX_MEMORY_BARRIER();
	p->iTerminate |= 2;

	return ( axthread_return_t )( axth_size_t )p->iExitValue;
}
#endif

AXTHREAD_FUNC axthread_t *AXTHREAD_CALL axthread_init( axthread_t *p, axth_fn_thread_t routine, void *parm )
#if AXTHREAD_IMPLEMENT
{
	p->iTerminate = 0;
	p->iExitValue = 0;
	p->pfnThread = routine;
	p->parm = parm;

	AX_MEMORY_BARRIER();

# if AXTHREAD_MODEL_WINDOWS
	p->hThread = CreateThread( NULL, 0, &axth__thread_f, ( void * )p, 0, &p->dwThreadId );
	if( !p->hThread ) {
		return ( axthread_t * )0;
	}
# elif AXTHREAD_MODEL_PTHREAD
	p->threadId = 0;
	AX_MEMORY_BARRIER();
	if( pthread_create( &p->thread, ( const pthread_attr_t * )0, &axth__thread_f, ( void * )p ) != 0 ) {
		return ( axthread_t * )0;
	}
	
# else
#  error Could not determine how to implement axthread_init()
# endif

	return p;
}
#else
;
#endif
AXTHREAD_FUNC axthread_t *AXTHREAD_CALL axthread_fini( axthread_t *p )
#if AXTHREAD_IMPLEMENT
{
	p->iTerminate = 1;

	AX_MEMORY_BARRIER();

# if AXTHREAD_MODEL_WINDOWS
	if( WaitForSingleObject( p->hThread, /*timeoutMilliseconds=*/200 ) != WAIT_OBJECT_0 ) {
		/* TODO: Define some logging interface */
	}

	p->dwThreadId = 0;

	CloseHandle( p->hThread );
	p->hThread = NULL;
# elif AXTHREAD_MODEL_PTHREAD
	( void )pthread_cancel( p->thread );
	( void )pthread_join( p->thread, ( void ** )0 );
	p->thread = ( pthread_t )0;
# else
#  error Could not determine how to implement axthread_fini()
# endif

	return ( axthread_t * )0;
}
#else
;
#endif

AXTHREAD_FUNC axth_thread_priority_t AXTHREAD_CALL axthread_get_priority( const axthread_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	int prio;

	prio = GetThreadPriority( p->hThread );
	if( prio == THREAD_PRIORITY_ERROR_RETURN ) {
		/* TODO: Logging (query failed) */
		return kAxthread_Priority_Normal;
	}

	switch( prio ) {
#  define AXTH__PRIO(A_,B_)\
	case THREAD_PRIORITY_##B_: return kAxthread_Priority_##A_
	AXTH__PRIO( VeryLow     , LOWEST        );
	AXTH__PRIO( Low         , BELOW_NORMAL  );
	AXTH__PRIO( Normal      , NORMAL        );
	AXTH__PRIO( High        , ABOVE_NORMAL  );
	AXTH__PRIO( VeryHigh    , HIGHEST       );
#  undef AXTH__PRIO
	default:
		return
			( prio > THREAD_PRIORITY_HIGHEST )
			?
				kAxthread_Priority_Critical
			:
				kAxthread_Priority_Idle
			;
	}
# elif AXTHREAD_MODEL_PTHREAD
	/* http://stackoverflow.com/questions/3649281/how-to-increase-thread-priority-in-pthreads */
	/* I will need to experiment with this to see what maps well in practice. */
	( void )p;
	return kAxthread_Priority_Normal;
# else
#  error Could not determine how to implement axthread_get_priority()
# endif
}
#else
;
#endif
AXTHREAD_FUNC axth_thread_priority_t AXTHREAD_CALL axthread_set_priority( axthread_t *p, axth_thread_priority_t prio )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	axth_thread_priority_t oldprio;
	int newprio;

	oldprio = axthread_get_priority( p );

	switch( prio ) {
#  define AXTH__PRIO(A_,B_)\
	case kAxthread_Priority_##A_: newprio = THREAD_PRIORITY_##B_; break
	AXTH__PRIO( Idle        , IDLE          );
	AXTH__PRIO( VeryLow     , LOWEST        );
	AXTH__PRIO( Low         , BELOW_NORMAL  );
	AXTH__PRIO( Normal      , NORMAL        );
	AXTH__PRIO( High        , ABOVE_NORMAL  );
	AXTH__PRIO( VeryHigh    , HIGHEST       );
	AXTH__PRIO( Critical    , TIME_CRITICAL );
#  undef AXTH__PRIO
	default:
		/* TODO: Logging (invalid priority) */
		return oldprio;
	}

	if( !SetThreadPriority( p->hThread, newprio ) ) {
		/* TODO: Logging (operation failed) */
	}

	return oldprio;
# elif AXTHREAD_MODEL_PTHREAD
	/* http://stackoverflow.com/questions/3649281/how-to-increase-thread-priority-in-pthreads */
	/* I will need to experiment with this to see what maps well in practice. */
	( void )p;
	( void )prio;
	return kAxthread_Priority_Normal;
# else
#  error Could not determine how to implement axthread_set_priority()
# endif
}
#else
;
#endif

AXTHREAD_FUNC void AXTHREAD_CALL axthread_set_name( axthread_t *p, const char *pszName )
#if AXTHREAD_IMPLEMENT
{
# if defined( _MSC_VER ) || defined( __INTEL_COMPILER )
#  pragma pack(push,8)
	struct {
		DWORD dwType;     /* must be 0x1000 */
		LPCSTR szName;    /* pointer to name (in user address space) */
		DWORD dwThreadID; /* thread identifier (-1 for caller thread) */
		DWORD dwFlags;    /* zero */
	} threadnameInfo;
#  pragma pack(pop)

	threadnameInfo.dwType = 0x1000;
	threadnameInfo.szName = pszName;
	threadnameInfo.dwThreadID = p->dwThreadId;
	threadnameInfo.dwFlags = 0;

#  ifndef AXTHREAD__EXCEPTION_SET_NAME
#   define AXTHREAD__EXCEPTION_SET_NAME 0x406D1388
#  endif

	__try {
		RaiseException( AXTHREAD__EXCEPTION_SET_NAME, 0, sizeof( threadnameInfo )/sizeof( ULONG_PTR ), ( ULONG_PTR * )&threadnameInfo );
	} __except( GetExceptionCode()==AXTHREAD__EXCEPTION_SET_NAME ? EXCEPTION_EXECUTE_HANDLER  : EXCEPTION_CONTINUE_SEARCH ) {
		// "Empty _except block" warning
		do {
			break;
		} while( 0 );
	}
# else
	( void )p;
	( void )pszName;
# endif
}
#else
;
#endif

AXTHREAD_FUNC void AXTHREAD_CALL axthread_signal_quit( axthread_t *p )
#if AXTHREAD_IMPLEMENT
{
	p->iTerminate = 1;
}
#else
;
#endif
AXTHREAD_FUNC int AXTHREAD_CALL axthread_is_quitting( const axthread_t *p )
#if AXTHREAD_IMPLEMENT
{
	return p->iTerminate != 0;
}
#else
;
#endif
AXTHREAD_FUNC int AXTHREAD_CALL axthread_is_running( const axthread_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	return p->hThread != (HANDLE)0 && WaitForSingleObject( p->hThread, 0 ) != WAIT_OBJECT_0;
# elif AXTHREAD_MODEL_PTHREAD
	int status;

	if( !p->threadId || waitpid( p->threadId, &status, WNOHANG | WUNTRACED ) == -1 ) {
		return 0;
	}

	if( WIFEXITED( status ) || WIFSIGNALED( status ) ) {
		return 0;
	}

	return 1;
# else
#  error Could not determine how to implement axthread_is_running()
# endif
}
#else
;
#endif

AXTHREAD_FUNC int AXTHREAD_CALL axthread_get_exit_code( const axthread_t *p )
#if AXTHREAD_IMPLEMENT
{
	return p->iExitValue;
}
#else
;
#endif

AXTHREAD_FUNC axthread_times_t *AXTHREAD_CALL axthread_get_times( const axthread_t *p, axthread_times_t *dst )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	static axth_u64_t MainThreadCreationTime = 0;
	FILETIME CreationTime, ExitTime, KernelTime, UserTime;
	axth_u64_t EnterTime, LeaveTime, LocalEnterTime, LocalLeaveTime, LocalKernTime, LocalUserTime;

	if( !MainThreadCreationTime ) {
		if( !GetProcessTimes( GetCurrentProcess(), &CreationTime, &ExitTime, &KernelTime, &UserTime ) ) {
			return ( axthread_times_t * )0;
		}

		MainThreadCreationTime = ( ( axth_u64_t )( CreationTime.dwHighDateTime )<<32 ) | ( axth_u64_t )( CreationTime.dwLowDateTime );
	}

	if( !GetThreadTimes( p->hThread, &CreationTime, &ExitTime, &KernelTime, &UserTime ) ) {
		dst->uEnterTime = 0;
		dst->uLeaveTime = 0;
		dst->uKernelTime = 0;
		dst->uUserTime = 0;

		return ( axthread_times_t * )0;
	}

	EnterTime = ( ( axth_u64_t )( CreationTime.dwHighDateTime )<<32 ) | ( axth_u64_t )( CreationTime.dwLowDateTime );
	LeaveTime = ( ( axth_u64_t )( ExitTime.dwHighDateTime )<<32 ) | ( axth_u64_t )( ExitTime.dwLowDateTime );

	LocalEnterTime = EnterTime - MainThreadCreationTime;
	LocalLeaveTime = LeaveTime - MainThreadCreationTime;
	LocalKernTime = ( ( axth_u64_t )( KernelTime.dwHighDateTime )<<32 ) | ( axth_u64_t )( KernelTime.dwLowDateTime );
	LocalUserTime = ( ( axth_u64_t )( UserTime.dwHighDateTime )<<32 ) | ( axth_u64_t )( UserTime.dwLowDateTime );

	dst->uEnterTime = LocalEnterTime/10; /* hundred-nanoseconds-to-microseconds */
	dst->uLeaveTime = LocalLeaveTime/10;
	dst->uKernelTime = LocalKernTime/10;
	dst->uUserTime = LocalUserTime/10;

	return dst;
# else
	( void )p;
	( void )dst;
	return ( axthread_times_t * )0;
# endif
}
#else
;
#endif

#if AXTHREAD_MODEL_WINDOWS
AXTHREAD_FUNC HANDLE AXTHREAD_CALL axthread_get_handle_mswin( axthread_t *p )
# if AXTHREAD_IMPLEMENT
{
	return p->hThread;
}
# else
;
# endif
#elif AXTHREAD_MODEL_PTHREAD
AXTHREAD_FUNC pthread_t AXTHREAD_CALL axthread_get_handle_posix( axthread_t *p )
# if AXTHREAD_IMPLEMENT
{
	return p->thread;
}
# else
;
# endif
#endif

AXTHREAD__LEAVE_C




/*
===============================================================================

	QUICK (SPIN) MUTEX - NON-RECURSIVE

===============================================================================
*/

AXTHREAD__ENTER_C

typedef volatile axth_u32_t axth_qmutex_t;
#define AXTHREAD_QMUTEX_INITIALIZER ((axth_u32_t)0)

AXTHREAD_FUNC axth_qmutex_t *AXTHREAD_CALL axth_qmutex_init( axth_qmutex_t *p )
#if AXTHREAD_IMPLEMENT
{
	*p = AXTHREAD_QMUTEX_INITIALIZER;
	return p;
}
#else
;
#endif
AXTHREAD_FUNC axth_qmutex_t *AXTHREAD_CALL axth_qmutex_fini( axth_qmutex_t *p )
#if AXTHREAD_IMPLEMENT
{
	( void )p;
	return ( axth_qmutex_t * )0;
}
#else
;
#endif

/* Attempt to acquire the lock (as with axth_qmutex_try_acquire()), but does not
 * decrement upon failure */
AXTHREAD_FUNC int AXTHREAD_CALL axth_qmutex_try_acquire_unsafe( axth_qmutex_t *p )
#if AXTHREAD_IMPLEMENT
{
	return AX_ATOMIC_FETCH_ADD_FULL32( p, 1 ) == 0;
}
#else
;
#endif
/* Attempt to acquire the lock without waiting */
AXTHREAD_FUNC int AXTHREAD_CALL axth_qmutex_try_acquire( axth_qmutex_t *p )
#if AXTHREAD_IMPLEMENT
{
	if( AX_ATOMIC_FETCH_ADD_FULL32( p, 1 ) == 0 ) {
		AX_ATOMIC_FETCH_SUB_FULL32( p, 1 );
		return 0;
	}

	return 1;
}
#else
;
#endif
/* Acquires the lock (!!! NON-RECURSIVE !!!) */
AXTHREAD_FUNC void AXTHREAD_CALL axth_qmutex_acquire( axth_qmutex_t *p, axth_u32_t cSpins )
#if AXTHREAD_IMPLEMENT
{
	/* wait for access to the lock */
	while( AX_ATOMIC_FETCH_ADD_FULL32( p, 1 ) != 0 ) {
		/* don't overload the lock */
		axth_backoff( &cSpins, AXTHREAD_MAX_BACKOFF_SPIN_COUNT );

		/* access not granted; decrement */
		AX_ATOMIC_FETCH_SUB_FULL32( p, 1 );
	}
}
#else
;
#endif
/* Releases the lock */
AXTHREAD_FUNC void AXTHREAD_CALL axth_qmutex_release( axth_qmutex_t *p )
#if AXTHREAD_IMPLEMENT
{
	AX_ATOMIC_FETCH_SUB_FULL32( p, 1 );
}
#else
;
#endif

AXTHREAD__LEAVE_C

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	/// Simple mutex implementation with a low-contention spin-lock
	class CQuickMutex
	{
	public:
		AXTHREAD_INLINE CQuickMutex()
		: m_mutex()
		{
			if( !axth_qmutex_init( &m_mutex ) ) {
				axth_cxx_error( "axth_qmutex_init failed" );
			}
		}
		AXTHREAD_INLINE ~CQuickMutex()
		{
			axth_qmutex_fini( &m_mutex );
		}

		/// Attempts to acquire the lock (as with tryAcquire()) but does not
		/// decrement if it failed
		AXTHREAD_INLINE bool tryAcquireUnsafe()
		{
			return !!axth_qmutex_try_acquire_unsafe( &m_mutex );
		}
		/// Attempts to acquire the lock without waiting
		AXTHREAD_INLINE bool tryAcquire()
		{
			return !!axth_qmutex_try_acquire( &m_mutex );
		}
		/// Acquires the lock
		AXTHREAD_INLINE void acquire( axth_u32_t spinCount = AXTHREAD_DEFAULT_SPIN_COUNT )
		{
			axth_qmutex_acquire( &m_mutex, spinCount );
		}
		/// Releases the lock
		AXTHREAD_INLINE void release()
		{
			axth_qmutex_release( &m_mutex );
		}
		/// Retrieves the current value of the lock
		AXTHREAD_INLINE axth_u32_t getValue() const
		{
			return m_mutex;
		}


		// For compatibility with TLockGuard<>

		AXTHREAD_INLINE void lock()
		{
			acquire();
		}
		AXTHREAD_INLINE void unlock()
		{
			release();
		}

	private:
		axth_qmutex_t               m_mutex;

# ifdef AX_DELETE_COPYFUNCS
		AX_DELETE_COPYFUNCS( CQuickMutex );
# endif
	};

}
#endif /*AXTHREAD_CXX_ENABLED*/




/*
===============================================================================

	QUICK (SPIN) SEMAPHORE - NON-RECURSIVE

===============================================================================
*/

AXTHREAD__ENTER_C

#ifndef AXTHREAD_QSEM_SLEEP_ENABLED
# ifdef INCGUARD_AXTIME_H_
#  define AXTHREAD_QSEM_SLEEP_ENABLED 1
# else
#  define AXTHREAD_QSEM_SLEEP_ENABLED 0
# endif
#endif
#ifndef AXTHREAD_QSEM_SLEEP_RESISTANCE_MS
# define AXTHREAD_QSEM_SLEEP_RESISTANCE_MS 20
#endif

typedef struct axth_qsem_s
{
	volatile axth_u32_t             uCur;
	axth_u32_t                      uMax;
} axth_qsem_t;

#define AXTHREAD_QSEM_MAXIMUM       ((axth_u32_t)0x7FFFFFFFUL)
#define AXTHREAD_QSEM_INITIALIZER   {((axth_u32_t)0),AXTHREAD_QSEM_MAXIMUM}

AXTHREAD_FUNC axth_qsem_t *AXTHREAD_CALL axth_qsem_init( axth_qsem_t *p, axth_u32_t uBase, axth_u32_t uMax )
#if AXTHREAD_IMPLEMENT
{
	if( uBase > uMax ) {
		return ( axth_qsem_t * )0;
	}

	p->uCur = uBase;
	p->uMax = uMax;

	return p;
}
#else
;
#endif
AXTHREAD_FUNC axth_qsem_t *AXTHREAD_CALL axth_qsem_fini( axth_qsem_t *p )
#if AXTHREAD_IMPLEMENT
{
	( void )p;
	return ( axth_qsem_t * )0;
}
#else
;
#endif

AXTHREAD_FUNC int AXTHREAD_CALL axth_qsem_signal( axth_qsem_t *p, axth_u32_t count, axth_u32_t *ppriorcount )
#if AXTHREAD_IMPLEMENT
{
	axth_u32_t uMax;
	axth_u32_t cSpin = 1;

	uMax = p->uMax;
	for(;;) {
		const axth_u32_t uCur = p->uCur;
		const axth_u32_t uNew = uCur + count;

		if( uNew > uMax ) {
			return 0;
		}

		if( AX_ATOMIC_COMPARE_EXCHANGE_FULL32( &p->uCur, uNew, uCur ) == uNew ) {
			if( ppriorcount != ( axth_u32_t * )0 ) {
				*ppriorcount = uCur;
			}

			break;
		}

		axth_backoff( &cSpin, AXTHREAD_MAX_BACKOFF_SPIN_COUNT );
	}

	return 1;
}
#else
;
#endif

AXTHREAD_FUNC int AXTHREAD_CALL axth_qsem_try_wait( axth_qsem_t *p )
#if AXTHREAD_IMPLEMENT
{
	axth_u32_t uCur;
	axth_u32_t uNew;

	uCur = p->uCur;
	if( !uCur ) {
		return 0;
	}

	uNew = uCur - 1;
	return AX_ATOMIC_COMPARE_EXCHANGE_FULL32( &p->uCur, uNew, uCur ) == uNew;
}
#else
;
#endif

AXTHREAD_FUNC void AXTHREAD_CALL axth_qsem_wait( axth_qsem_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_QSEM_SLEEP_ENABLED
	axth_u32_t uBaseMs;

	uBaseMs = axtm_milliseconds_lowlatency();
	while( !axth_qsem_try_wait( p ) ) {
		axth_u32_t uElapsedMs;

		uElapsedMs = axtm_milliseconds_lowlatency() - uBaseMs;
		if( uElapsedMs >= AXTHREAD_QSEM_SLEEP_RESISTANCE_MS ) {
			axth_yield();
		}

		axth_local_spin( AXTHREAD_DEFAULT_SPIN_COUNT );
	}
# else
	axth_u32_t cSpin = 1;

	while( !axth_qsem_try_wait( p ) ) {
		axth_backoff( &cSpin, AXTHREAD_MAX_BACKOFF_SPIN_COUNT );
	}
# endif
}
#else
;
#endif

#ifdef INCGUARD_AXTIME_H_
# define AXTHREAD_QSEM_TIMED_WAIT_DEFINED 1
AXTHREAD_FUNC int AXTHREAD_CALL axth_qsem_timed_wait( axth_qsem_t *p, axth_u32_t milliseconds )
# if AXTHREAD_IMPLEMENT
{
	axth_u32_t uBaseMs;
	axth_u32_t cSpin = 1;

	uBaseMs = axtm_milliseconds_lowlatency();
	while( !axth_qsem_try_wait( p ) ) {
		axth_u32_t uElapsed;

		uElapsed = axtm_milliseconds_lowlatency() - uBaseMs;
		if( uElapsed >= milliseconds ) {
			return 0;
		}

#  if AXTHREAD_QSEM_SLEEP_ENABLED
		if( uElapsed >= AXTHREAD_QSEM_SLEEP_RESISTANCE_MS ) {
			axth_yield();
		}
#  endif

		axth_backoff( &cSpin, AXTHREAD_MAX_BACKOFF_SPIN_COUNT );
	}

	return 1;
}
# else
;
# endif
#else
# define AXTHREAD_QSEM_TIMED_WAIT_DEFINED 0
#endif /*INCGUARD_AXTIME_H_*/

AXTHREAD__LEAVE_C

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	/// Simple implementation of a semaphore which avoids OS scheduling
	class CQuickSemaphore
	{
	public:
		static const axth_u32_t kMaxCount = AXTHREAD_QSEM_MAXIMUM;

		/// Constructor
		AXTHREAD_INLINE CQuickSemaphore( axth_u32_t baseCount = 0, axth_u32_t maxCount = kMaxCount )
		: m_semaphore()
		{
			if( !axth_qsem_init( &m_semaphore, baseCount, maxCount ) ) {
				axth_cxx_error( "axth_qsem_init failed" );
			}
		}
		/// Destructor
		AXTHREAD_INLINE ~CQuickSemaphore()
		{
			axth_qsem_fini( &m_semaphore) ;
		}
		/// Signal the semaphore (increases current count by the amount given)
		AXTHREAD_INLINE bool signal( axth_u32_t count = 1, axth_u32_t *pPrevCount = ( axth_u32_t * )0 )
		{
			return !!axth_qsem_signal( &m_semaphore, count, pPrevCount );
		}
		/// wait for the semaphore to be signalled (decreases current count by one upon returning)
		AXTHREAD_INLINE void wait()
		{
			axth_qsem_wait( &m_semaphore );
		}
#if AXTHREAD_QSEM_TIMED_WAIT_DEFINED
		/// wait for the semaphore to be signalled for a specific amount of time (decreases current count by one upon returning)
		AXTHREAD_INLINE bool timedWait( axth_u32_t milliseconds )
		{
			return !!axth_qsem_timed_wait( &m_semaphore, milliseconds );
		}
#endif
		/// Check to see whether this signal is triggered (this will decrement upon successful return)
		AXTHREAD_INLINE bool tryWait()
		{
			return !!axth_qsem_try_wait( &m_semaphore );
		}

	private:
		axth_qsem_t                 m_semaphore;

# ifdef AX_DELETE_COPYFUNCS
		AX_DELETE_COPYFUNCS( CQuickSemaphore );
# endif
	};

}
#endif /*AXTHREAD_CXX_ENABLED*/




/*
===============================================================================

	READ / WRITE LOCK

===============================================================================
*/

AXTHREAD__ENTER_C

typedef struct axth_rwlock_s
{
	volatile axth_u32_t cReaders;
	volatile axth_u32_t uWriting;
} axth_rwlock_t;

#define AXTHREAD_RWLOCK_MAX_READERS ((axth_u32_t)128)
#define AXTHREAD_RWLOCK_INITIALIZER {((axth_u32_t)0),((axth_u32_t)0)}

#if AXTHREAD_IMPLEMENT
static void axth_rwlock__acquire_write_privilege( axth_rwlock_t *p )
{
	axth_u32_t cSpin = 1;
	while( AX_ATOMIC_COMPARE_EXCHANGE_FULL32( &p->uWriting, 1, 0 ) != 1 ) {
		axth_backoff( &cSpin, AXTHREAD_MAX_BACKOFF_SPIN_COUNT );
	}
}
static void axth_rwlock__release_write_privilege( axth_rwlock_t *p )
{
	AX_ATOMIC_EXCHANGE_FULL32( &p->uWriting, 0 );
}
#endif

AXTHREAD_FUNC axth_rwlock_t *AXTHREAD_CALL axth_rwlock_init( axth_rwlock_t *p )
#if AXTHREAD_IMPLEMENT
{
	p->cReaders = 0;
	p->uWriting = 0;

	return p;
}
#else
;
#endif
AXTHREAD_FUNC axth_rwlock_t *AXTHREAD_CALL axth_rwlock_fini( axth_rwlock_t *p )
#if AXTHREAD_IMPLEMENT
{
	( void )p;
	return ( axth_rwlock_t * )0;
}
#else
;
#endif

AXTHREAD_FUNC int AXTHREAD_CALL axth_rwlock_try_rdacquire( axth_rwlock_t *p )
#if AXTHREAD_IMPLEMENT
{
	axth_u32_t cReaders;

	AX_MEMORY_BARRIER();

	/* try to gain access */
	cReaders = AX_ATOMIC_FETCH_ADD_FULL32( &p->cReaders, 1 ) + 1;
	if( cReaders >= AXTHREAD_RWLOCK_MAX_READERS ) {
		AX_ATOMIC_FETCH_SUB_FULL32( &p->cReaders, 1 );
		return 0;
	}

	/* we gained access */
	return 1;
}
#else
;
#endif
AXTHREAD_FUNC void AXTHREAD_CALL axth_rwlock_rdacquire( axth_rwlock_t *p )
#if AXTHREAD_IMPLEMENT
{
	axth_u32_t cSpin = 1;
	while( !axth_rwlock_try_rdacquire( p ) ) {
		axth_backoff( &cSpin, AXTHREAD_MAX_BACKOFF_SPIN_COUNT );
	}
}
#else
;
#endif
AXTHREAD_FUNC void AXTHREAD_CALL axth_rwlock_rdrelease( axth_rwlock_t *p )
#if AXTHREAD_IMPLEMENT
{
	AX_ATOMIC_FETCH_SUB_FULL32( &p->cReaders, 1 );
}
#else
;
#endif

/*
	TODO: axth_rwlock_try_wracquire()
*/

AXTHREAD_FUNC void AXTHREAD_CALL axth_rwlock_wracquire( axth_rwlock_t *p )
#if AXTHREAD_IMPLEMENT
{
	axth_u32_t cSpin = 1;

	AX_MEMORY_BARRIER();

	/* only one writer can acquire access at a time, to avoid deadlock */
	axth_rwlock__acquire_write_privilege( p );
	AX_ATOMIC_FETCH_ADD_FULL32( &p->cReaders, AXTHREAD_RWLOCK_MAX_READERS );
	while( p->cReaders > AXTHREAD_RWLOCK_MAX_READERS ) {
		axth_backoff( &cSpin, AXTHREAD_MAX_BACKOFF_SPIN_COUNT );
	}
	axth_rwlock__release_write_privilege( p );
}
#else
;
#endif
AXTHREAD_FUNC void AXTHREAD_CALL axth_rwlock_wrrelease( axth_rwlock_t *p )
#if AXTHREAD_IMPLEMENT
{
	AX_ATOMIC_FETCH_SUB_FULL32( &p->cReaders, AXTHREAD_RWLOCK_MAX_READERS );
}
#else
;
#endif

AXTHREAD__LEAVE_C

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	/// Single-writer multiple-reader synchronization primitive
	class CRWLock
	{
	public:
									/// Maximum number of readers at any given time
		static const axth_u32_t     kMaxReaders         = AXTHREAD_RWLOCK_MAX_READERS;

									/// Constructor
		AXTHREAD_INLINE             CRWLock             ()
									: m_rwLock()
									{
										if( !axth_rwlock_init( &m_rwLock ) ) {
											axth_cxx_error( "axth_rwlock_init failed" );
										}
									}
									/// Destructor
		AXTHREAD_INLINE             ~CRWLock            ()
									{
										axth_rwlock_fini( &m_rwLock );
									}

									/// Try to acquire read access; return immediately upon failure
		AXTHREAD_INLINE bool        tryReadLock         ()
									{
										return !!axth_rwlock_try_rdacquire( &m_rwLock );
									}
									/// acquire read access
		AXTHREAD_INLINE void        readLock            ()
									{
										axth_rwlock_rdacquire( &m_rwLock );
									}
									/// release read access
		AXTHREAD_INLINE void        readUnlock          ()
									{
										axth_rwlock_rdrelease( &m_rwLock );
									}
									/// acquire write access
		AXTHREAD_INLINE void        writeLock           ()
									{
										axth_rwlock_wracquire( &m_rwLock );
									}
									/// release write access
		AXTHREAD_INLINE void        writeUnlock         ()
									{
										axth_rwlock_wrrelease( &m_rwLock );
									}

		AXTHREAD_INLINE             operator axth_rwlock_t &()
									{
										return m_rwLock;
									}

	private:
		axth_rwlock_t               m_rwLock;

# ifdef AX_DELETE_COPYFUNCS
		AX_DELETE_COPYFUNCS( CRWLock );
# endif
	};

	/// Scope-based read-locker
	class ReadLockGuard
	{
	public:
		typedef axth_rwlock_t       mutex_type;

									/// acquire a lock (for automatic release on destruct)
		AXTHREAD_INLINE             ReadLockGuard       ( mutex_type &m )
									: m_mutex( m )
									{
										axth_rwlock_rdacquire( &m_mutex );
									}
									/// release the acquired lock
		AXTHREAD_INLINE             ~ReadLockGuard      ()
									{
										axth_rwlock_rdrelease( &m_mutex );
									}

	private:
		mutex_type &                m_mutex;

# ifdef AX_DELETE_COPYFUNCS
		AX_DELETE_COPYFUNCS( ReadLockGuard );
# endif
	};

	/// Scope-based write-locker
	class WriteLockGuard
	{
	public:
		typedef axth_rwlock_t       mutex_type;

									/// acquire a lock (for automatic release on destruct)
		AXTHREAD_INLINE             WriteLockGuard      ( mutex_type &m )
									: m_mutex( m )
									{
										axth_rwlock_wracquire( &m_mutex );
									}
									/// release the acquired lock
		AXTHREAD_INLINE             ~WriteLockGuard     ()
									{
										axth_rwlock_wrrelease( &m_mutex );
									}

	private:
		mutex_type &                m_mutex;

# ifdef AX_DELETE_COPYFUNCS
		AX_DELETE_COPYFUNCS( WriteLockGuard );
# endif
	};

}
#endif




/*
===============================================================================

	SIGNALS (MANUAL-RESET EVENTS)

===============================================================================
*/

AXTHREAD__ENTER_C

typedef struct axth_signal_s
{
#if AXTHREAD_MODEL_WINDOWS
	HANDLE h;
#elif AXTHREAD_MODEL_PTHREAD
	pthread_mutex_t m;
	pthread_cond_t c;
	int v;
#else
# error Could not determine how to define axth_signal_s
#endif
} axth_signal_t;

#if AXTHREAD_MODEL_WINDOWS
# define AXTHREAD_SIGNAL_INITIALIZER\
	{(HANDLE)0}
#else
# define AXTHREAD_SIGNAL_INITIALIZER\
	{PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER,((int)0)}
#endif

AXTHREAD_FUNC axth_signal_t *AXTHREAD_CALL axth_sig_init( axth_signal_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	p->h = CreateEventW( ( LPSECURITY_ATTRIBUTES )0, TRUE, FALSE, ( LPCWSTR )0 );
	if( !p->h ) { return ( axth_signal_t * )0; }
# elif AXTHREAD_MODEL_PTHREAD
	if( pthread_mutex_init( &p->m, 0 ) != 0 ) { return ( axth_signal_t * )0; }
	if( pthread_cond_init( &p->c, 0 ) != 0 ) { return ( axth_signal_t * )0; }
	p->v = 0;
# else
#  error Could not determine how to implement axth_sig_init()
# endif

	return p;
}
#else
;
#endif
AXTHREAD_FUNC axth_signal_t *AXTHREAD_CALL axth_sig_fini( axth_signal_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	CloseHandle( p->h );
	p->h = NULL;
# elif AXTHREAD_MODEL_PTHREAD
	pthread_cond_destroy( &p->c );
	pthread_mutex_destroy( &p->m );
# else
#  error Could not determine how to implement axth_sig_fini()
# endif

	return ( axth_signal_t * )0;
}
#else
;
#endif

AXTHREAD_FUNC void AXTHREAD_CALL axth_sig_set( axth_signal_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	SetEvent( p->h );
# elif AXTHREAD_MODEL_PTHREAD
	pthread_mutex_lock( &p->m );
	p->v = 1;
	pthread_cond_signal( &p->c );
	pthread_mutex_unlock( &p->m );
# else
#  error Could not determine how to implement axth_sig_set()
# endif
}
#else
;
#endif
AXTHREAD_FUNC void AXTHREAD_CALL axth_sig_clear( axth_signal_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	ResetEvent( p->h );
# elif AXTHREAD_MODEL_PTHREAD
	pthread_mutex_lock( &p->m );
	p->v = 0;
	pthread_mutex_unlock( &p->m );
# else
#  error Could not determine how to implement axth_sig_clear()
# endif
}
#else
;
#endif
AXTHREAD_FUNC void AXTHREAD_CALL axth_sig_wait( axth_signal_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	WaitForSingleObject( p->h, INFINITE );
# elif AXTHREAD_MODEL_PTHREAD
	pthread_mutex_lock( &p->m );
	while( !p->v ) {
		pthread_cond_wait( &p->c, &p->m );
	}
	pthread_mutex_unlock( &p->m );
# else
#  error Could not determine how to implement axth_sig_wait()
# endif
}
#else
;
#endif
AXTHREAD_FUNC int AXTHREAD_CALL axth_sig_timed_wait( axth_signal_t *p, axth_u32_t milliseconds )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	return WaitForSingleObject( p->h, milliseconds ) == WAIT_OBJECT_0;
# elif AXTHREAD_MODEL_PTHREAD
#  ifdef CLOCK_MONOTONIC
	struct timespec basets;
#  endif
	struct timespec ts;
	int r = 0;

	/*
	
		NOTE: It's possible that this may fail multiple times prior to the time-
		`     out period elapsing. pthread_cond_timedwait() will not modify the
		`     passed value. It's conceptually possible for this to wait forever.
		`     To compensate, we try to keep track of the amount of time that
		`     elapses as well (if CLOCK_MONOTONIC was defined). Otherwise we
		`     allow returning a failure status prior to the timeout elapsing.
	
	*/

	ts.tv_sec = milliseconds/1000;
	ts.tv_nsec = ( milliseconds%1000 )*1000000;

#  ifdef CLOCK_MONOTONIC
	if( clock_gettime( CLOCK_MONOTONIC, &basets ) != 0 ) {
#  endif
		pthread_mutex_lock( &p->m );
		r = ( p->v != 0 ) || ( pthread_cond_timedwait( &p->c, &p->m, &ts ) != ETIMEDOUT && p->v != 0 );
		pthread_mutex_unlock( &p->m );
#  ifdef CLOCK_MONOTONIC
	}

	pthread_mutex_lock( &p->m );
	while( !p->v ) {
		struct timespec tmpts;
		axth_u64_t a, b;

		if( pthread_cond_timedwait( &p->c, &p->m, &ts ) == ETIMEDOUT || p->v != 0 ) {
			r = p->v != 0;
			break;
		}

		if( clock_gettime( CLOCK_MONOTONIC, &tmpts ) != 0 ) {
			break;
		}

		a = ((axth_u64_t)tmpts.tv_sec)*1000000000 + (axth_u64_t)tmpts.tv_nsec;
		b = ((axth_u64_t)basets.tv_sec)*1000000000 + (axth_u64_t)basets.tv_nsec;

		a -= b;
		if( a <= milliseconds*1000000 ) {
			break;
		}

		ts.tv_sec = (axth_u32_t)(a/1000000000);
		ts.tv_nsec = (axth_u32_t)(a%1000000000);
	}
	pthread_mutex_unlock( &p->m );
#  endif
	return r;
# else
#  error Could not determine how to implement axth_sig_timed_wait()
# endif
}
#else
;
#endif
AXTHREAD_FUNC int AXTHREAD_CALL axth_sig_try_wait( axth_signal_t *p )
#if AXTHREAD_IMPLEMENT
{
	return axth_sig_timed_wait( p, 0 );
}
#else
;
#endif

AXTHREAD__LEAVE_C

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	class CSignal
	{
	public:
									/// Constructor
		inline                      CSignal             ()
									: m_sig()
									{
										if( !axth_sig_init( &m_sig ) ) {
											axth_cxx_error( "axth_sig_init failed" );
										}
									}
									/// Destructor
		inline                      ~CSignal                ()
									{
										axth_sig_fini( &m_sig );
									}

									/// trigger the signal (mark it as "on" / "active")
		inline void                 trigger             ()
									{
										axth_sig_set( &m_sig );
									}
									/// reset the signal (mark it as "off" / "deactivated")
		inline void                 reset               ()
									{
										axth_sig_clear( &m_sig );
									}
		
									/// wait for this signal to be triggered ("on" / "active")
		inline void                 wait                ()
									{
										axth_sig_wait( &m_sig );
									}
									/// wait for this signal to be triggered for the specified number of milliseconds
		inline bool                 timedWait           ( axth_u32_t milliseconds )
									{
										return !!axth_sig_timed_wait( &m_sig, milliseconds );
									}
									/// Check to see whether this signal is triggered
		inline bool                 tryWait             ()
									{
										return !!axth_sig_try_wait( &m_sig );
									}

	private:
		axth_signal_t               m_sig;
	};

}
#endif /*AXTHREAD_CXX_ENABLED*/




/*
===============================================================================

	SEMAPHORE (SYSTEM)

===============================================================================
*/

AXTHREAD__ENTER_C

#if AXTHREAD_MODEL_WINDOWS
# define AXTHREAD_SEM_INITIALIZER ((HANDLE)0)
typedef HANDLE axth_sem_t;
#elif AXTHREAD_MODEL_PTHREAD
# define AXTHREAD_SEM_INITIALIZER {}
typedef sem_t axth_sem_t;
#else
# error Could not determine how to define axth_sem_t.
#endif

AXTHREAD_FUNC axth_sem_t *AXTHREAD_CALL axth_sem_init( axth_sem_t *p, axth_u32_t uBase )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	/* FIXME: Or does it return INVALID_HANDLE_VALUE instead of NULL? */
	if( !( *p = CreateSemaphoreW( NULL, uBase, 0x7FFFFFFFU, NULL ) ) ) {
		return ( axth_sem_t * )0;
	}

	return p;
# elif AXTHREAD_MODEL_PTHREAD
	if( sem_init( p, 0, uBase ) == -1 ) {
		return ( axth_sem_t * )0;
	}

	return p;
# else
#  error Could not determine how to implement axth_sem_init()
# endif
}
#else
;
#endif
AXTHREAD_FUNC axth_sem_t *AXTHREAD_CALL axth_sem_fini( axth_sem_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	if( p != ( axth_sem_t * )0 ) {
		CloseHandle( *p );
	}
	return ( axth_sem_t * )0;
# elif AXTHREAD_MODEL_PTHREAD
	if( p != ( axth_sem_t * )0 ) {
		sem_destroy( p );
	}
	return ( axth_sem_t * )0;
# else
#  error Could not determine how to implement axth_sem_fini()
# endif
}
#else
;
#endif

AXTHREAD_FUNC int AXTHREAD_CALL axth_sem_signal( axth_sem_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	return (int)!!ReleaseSemaphore( *p, 1, ( LONG * )0 );
# elif AXTHREAD_MODEL_PTHREAD
	return (int)( sem_post( p ) == 0 );
# else
#  error Could not determine how to implement axth_sem_signal()
# endif
}
#else
;
#endif

AXTHREAD_FUNC int AXTHREAD_CALL axth_sem_wait( axth_sem_t *p )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	return ( int )( WaitForSingleObject( *p, INFINITE ) == WAIT_OBJECT_0 );
# elif AXTHREAD_MODEL_PTHREAD
	return ( int )( sem_wait( p ) == 0 );
# else
#  error Could not determine how to implement axth_sem_wait()
# endif
}
#else
;
#endif
AXTHREAD_FUNC int AXTHREAD_CALL axth_sem_timed_wait( axth_sem_t *p, axth_u32_t milliseconds )
#if AXTHREAD_IMPLEMENT
{
# if AXTHREAD_MODEL_WINDOWS
	return ( int )( WaitForSingleObject( *p, milliseconds ) == WAIT_OBJECT_0 );
# elif AXTHREAD_MODEL_PTHREAD
	if( !milliseconds ) {
		return ( int )( sem_trywait( p ) == 0 );
	}
	
	struct timespec ts;
	ts.tv_sec = milliseconds/1000;
	ts.tv_nsec = (milliseconds%1000)*1000;

	return ( int )( sem_timedwait( p, &ts ) == 0 );
# else
#  error Could not determine how to implement axth_sem_timed_wait()
# endif
}
#else
;
#endif

AXTHREAD__LEAVE_C

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	class CSemaphore
	{
	public:
		inline CSemaphore( axth_u32_t uBase = 0 )
		: m_semaphore()
		{
			if( !axth_sem_init( &m_semaphore, uBase ) ) {
				axth_cxx_error( "axth_sem_init() failed" );
			}
		}
		inline ~CSemaphore()
		{
			axth_sem_fini( &m_semaphore );
		}

		inline bool signal()
		{
			return !!axth_sem_signal( &m_semaphore );
		}

		inline bool wait()
		{
			return !!axth_sem_wait( &m_semaphore );
		}
		inline bool timedWait( axth_u32_t cMilliseconds )
		{
			return !!axth_sem_timed_wait( &m_semaphore, cMilliseconds );
		}

	private:
		axth_sem_t m_semaphore;
	};

}
#endif /*AXTHREAD_CXX_ENABLED*/




/*
===============================================================================

	LOCK GUARDS

===============================================================================
*/

#if AXTHREAD_CXX_ENABLED
namespace ax
{

	template< typename TLock >
	class TLockGuard
	{
	public:
		typedef TLock LockType;

		inline TLockGuard( TLock &lock )
		: m_lock( lock )
		{
			m_lock.lock();
		}
		inline ~TLockGuard()
		{
			m_lock.unlock();
		}

	private:
		TLock &m_lock;
	};

	typedef TLockGuard<CQuickMutex> CQuickMutexGuard;

}
#endif




#endif
