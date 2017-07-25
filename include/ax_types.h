/*

	ax_types - public domain
	Last update: 2015-10-01 Aaron Miller


	This library defines the following types:

		ax_s8_t, ax_s16_t, ax_s32_t, ax_s64_t
		Signed integers of 8-, 16-, 32-, and 64-bits in size, respectively.

		ax_u8_t, ax_u16_t, ax_u32_t, ax_u64_t
		Unsigned equivalents of the signed counterparts above.

		ax_sptr_t, ax_uptr_t
		Signed and unsigned pointer-sized integers.

		ax_sint_t, ax_uint_t
		"Fast" signed and unsigned integers (potentially 64-bit). Effectively,
		register-sized integers.

		ax_bool_t (with constants "false" and "true")
		Boolean type with false defined as 0 and true as 1. Meant to match C++'s
		bool type.

	Additionally, if C++ is enabled (see AX_TYPES_CXX_ENABLED) the following are
	also defined:

		ax::int8, ax::int16, ax::int32, ax::int64, ax::intptr, ax::intcpu
		Aliases for ax_s8_t and friends.

		ax::uint8, ax::uint16, ax::uint32, ax::uint64, ax::uintptr, ax::uintcpu
		Aliases for ax_u8_t and friends.

		ax::uint
		Alias for unsigned int.

		ax::TSizedInt<>, ax::TPointerInt
		TSizedInt<> is a template structure containing only typedefs called
		sint and uint. sint represents a signed integer, with uint being
		unsigned. For example, ax::TSizedInt<4>::uint represents a 32-bit
		unsigned integer.

	
	USAGE
	=====

	Simply include this header file. Define any of the macros explained in the
	following sections BEFORE including the header.


	CONFIGURATION MACROS
	====================

	Define any of these to either 1 or 0 prior to including this header, if you
	want to alter the default functionality.

		AX_TYPES_CXX_ENABLED
		--------------------
		Enables the C++ parts of this. All C++ code is in the ax namespace. If
		C++ is enabled then the pointer size is guaranteed to be accurate.

		AX_HAS_STDINT_H
		---------------
		If set to 1 then stdint.h will be used to pull the type definitions.


	DEFINITION MACROS
	=================

	Define any of these to prevent the library from doing so.

		AX_BOOL_DEFINED
		---------------
		If not defined then ax_bool_t will be defined. Potentially with a C99
		_Bool type.

		AX_TYPES_DEFINED
		----------------
		If not defined then the ax_s<x>_t and ax_u<x>_t types (including
		ax_uptr_t, ax_sptr_t, ax_uint_t, and ax_sint_t) will be defined.


	INTERACTIONS
	============

	This library will use ax_platform definitions if they are available. To use
	them include ax_platform.h before including this header.

	Uses of ax_platform in this library include ax_static_assert to verify that
	the types defined here are of the expected sizes; to determine whether we're
	targeting a 64-bit (address) architecture; to determine whether the target
	architecture's 64-bit integers (if in 64-bit mode) are "fast." (This is not
	always the case.)


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_TYPES_H_
#define INCGUARD_AX_TYPES_H_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

#ifndef AX_TYPES_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AX_TYPES_CXX_ENABLED      AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AX_TYPES_CXX_ENABLED      1
# else
#  define AX_TYPES_CXX_ENABLED      0
# endif
#endif

#ifndef AX_TYPES_CXX_NULLPTR_ENABLED
# if defined( AX_CXX_N2431 )
#  define AX_TYPES_CXX_NULLPTR_ENABLED  AX_CXX_N2431
# elif defined( __has_extension )
#  if __has_extension(cxx_nullptr)
#   define AX_TYPES_CXX_NULLPTR_ENABLED 1
#  else
#   define AX_TYPES_CXX_NULLPTR_ENABLED 0
#  endif
# elif defined( _MSC_VER ) && _MSC_VER >= 1600
#  define AX_TYPES_CXX_NULLPTR_ENABLED  1
# elif defined( __GNUC__ ) && ( __GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 6 ) )
#  define AX_TYPES_CXX_NULLPTR_ENABELD  1
# elif defined( __clang__ )
#  define AX_TYPES_CXX_NULLPTR_ENABLED  1
# elif defined( __INTEL_COMPILER ) && __INTEL_COMPILER >= 1210
#  define AX_TYPES_CXX_NULLPTR_ENABLED  1
# else
#  define AX_TYPES_CXX_NULLPTR_ENABLED  0
# endif
#endif

#ifndef AX_HAS_STDINT_H
# if !defined( _MSC_VER ) || _MSC_VER >= 1600
#  define AX_HAS_STDINT_H           1
# else
#  define AX_HAS_STDINT_H           0
# endif
#endif

#ifndef AX_BOOL_DEFINED
# define AX_BOOL_DEFINED            1
# if AX_TYPES_CXX_ENABLED
typedef bool                        ax_bool_t;
# elif !defined( _MSC_VER ) || _MSC_VER >= 1600
# include <stdbool.h>
typedef _Bool                       ax_bool_t;
# else
#  ifndef __bool_true_false_are_defined
#   define __bool_true_false_are_defined 1
typedef enum { false = 0, true = 1 } _Bool;
#  endif
typedef _Bool                       ax_bool_t;
# endif
#endif

#ifndef AX_TYPES_DEFINED
/*
 *  NOTE: Do not define AX_TYPES_DEFINED here
 *  -     This is purposely separated out so the C++ specific code can use the
 *  -     typedefs here, then proceed to define ax_sptr_t and ax_uptr_t.
 */
# if AX_HAS_STDINT_H
#  include <stdint.h>
typedef int8_t                      ax_s8_t;
typedef int16_t                     ax_s16_t;
typedef int32_t                     ax_s32_t;
typedef int64_t                     ax_s64_t;
typedef uint8_t                     ax_u8_t;
typedef uint16_t                    ax_u16_t;
typedef uint32_t                    ax_u32_t;
typedef uint64_t                    ax_u64_t;
typedef intmax_t                    ax_smax_t;
typedef uintmax_t                   ax_umax_t;
# elif defined( _MSC_VER )
typedef signed __int8               ax_s8_t;
typedef signed __int16              ax_s16_t;
typedef signed __int32              ax_s32_t;
typedef signed __int64              ax_s64_t;
typedef unsigned __int8             ax_u8_t;
typedef unsigned __int16            ax_u16_t;
typedef unsigned __int32            ax_u32_t;
typedef unsigned __int64            ax_u64_t;
typedef ax_s64_t                    ax_smax_t;
typedef ax_u64_t                    ax_umax_t;
# else
typedef signed char                 ax_s8_t;
typedef signed short                ax_s16_t;
typedef signed int                  ax_s32_t;
typedef signed long long int        ax_s64_t;
typedef unsigned char               ax_u8_t;
typedef unsigned short              ax_u16_t;
typedef unsigned int                ax_u32_t;
typedef unsigned long long int      ax_u64_t;
typedef ax_s64_t                    ax_smax_t;
typedef ax_u64_t                    ax_umax_t;
# endif
#endif

#if AX_TYPES_CXX_ENABLED
namespace ax
{

	template< unsigned int tSize >
	struct TSizedInt
	{
	};

	template<>
	struct TSizedInt< 1 >
	{
		typedef ax_s8_t             sint;
		typedef ax_u8_t             uint;
	};
	template<>
	struct TSizedInt< 2 >
	{
		typedef ax_s16_t            sint;
		typedef ax_u16_t            uint;
	};
	template<>
	struct TSizedInt< 4 >
	{
		typedef ax_s32_t            sint;
		typedef ax_u32_t            uint;
	};
	template<>
	struct TSizedInt< 8 >
	{
		typedef ax_s64_t            sint;
		typedef ax_u64_t            uint;
	};

	typedef TSizedInt< sizeof( void * ) > TPointerInt;

}
#endif

#ifndef AX_TYPES_DEFINED
# define AX_TYPES_DEFINED           1

# if AX_TYPES_CXX_ENABLED
typedef ax::TPointerInt::sint       ax_sptr_t;
typedef ax::TPointerInt::uint       ax_uptr_t;
# elif defined( AX_ARCH_64BITS )
#  if AX_ARCH_64BITS
typedef ax_s64_t                    ax_sptr_t;
typedef ax_u64_t                    ax_uptr_t;
#  else
typedef ax_s32_t                    ax_sptr_t;
typedef ax_u32_t                    ax_uptr_t;
#  endif
# elif defined( __LP__ ) || defined( __amd64__ ) || defined( __x86_64__ ) || defined( _M_AMD64 )
typedef ax_s64_t                    ax_sptr_t;
typedef ax_u64_t                    ax_uptr_t;
# else
typedef ax_s32_t                    ax_sptr_t;
typedef ax_u32_t                    ax_uptr_t;
# endif

# if defined( AX_ARCH_X86 ) && AX_ARCH_X86
typedef ax_sptr_t                   ax_sint_t;
typedef ax_uptr_t                   ax_uint_t;
# else
typedef ax_s32_t                    ax_sint_t;
typedef ax_u32_t                    ax_uint_t;
# endif
#endif

#ifndef AX_NO_STDDEF_H
# if defined(__cplusplus) && AX_TYPES_CXX_ENABLED!=0
#  include <cstddef>
# else
#  include <stddef.h>
# endif
typedef size_t ax_size_t;
typedef ptrdiff_t ax_ptrdiff_t;
#else
typedef ax_uptr_t ax_size_t;
typedef ax_sptr_t ax_ptrdiff_t;
#endif

#ifdef ax_static_assert
ax_static_assert( sizeof( ax_u8_t ) == 1, "ax: Size mismatch" );
ax_static_assert( sizeof( ax_u16_t ) == 2, "ax: Size mismatch" );
ax_static_assert( sizeof( ax_u32_t ) == 4, "ax: Size mismatch" );
ax_static_assert( sizeof( ax_u64_t ) == 8, "ax: Size mismatch" );
ax_static_assert( sizeof( ax_s8_t ) == 1, "ax: Size mismatch" );
ax_static_assert( sizeof( ax_s16_t ) == 2, "ax: Size mismatch" );
ax_static_assert( sizeof( ax_s32_t ) == 4, "ax: Size mismatch" );
ax_static_assert( sizeof( ax_s64_t ) == 8, "ax: Size mismatch" );

ax_static_assert( sizeof( ax_sptr_t ) == sizeof( void * ), "ax: Size mismatch" );
ax_static_assert( sizeof( ax_uptr_t ) == sizeof( void * ), "ax: Size mismatch" );

ax_static_assert( sizeof( ax_uint_t ) >= sizeof( ax_u32_t ), "ax: Size mismatch" );
ax_static_assert( sizeof( ax_sint_t ) >= sizeof( ax_s32_t ), "ax: Size mismatch" );
#endif

#if AX_TYPES_CXX_ENABLED
namespace ax
{

	typedef void                    Void;
	typedef bool                    Bool;

	typedef float                   f32, F32, Float32;
	typedef double                  f64, F64, Float64;

	typedef ax_s8_t                 int8;
	typedef ax_s16_t                int16;
	typedef ax_s32_t                int32;
	typedef ax_s64_t                int64;
	typedef ax_u8_t                 uint8;
	typedef ax_u16_t                uint16;
	typedef ax_u32_t                uint32;
	typedef ax_u64_t                uint64;

	typedef ax_smax_t               intmax;
	typedef ax_umax_t               uintmax;

	typedef ax_sptr_t               intptr;
	typedef ax_uptr_t               uintptr;
	typedef ax_sint_t               intcpu;
	typedef ax_uint_t               uintcpu;

	typedef unsigned int            uint;

	typedef ax_s8_t                 S8 , Int8;
	typedef ax_s16_t                S16, Int16;
	typedef ax_s32_t                S32, Int32;
	typedef ax_s64_t                S64, Int64;
	typedef ax_u8_t                 U8 , UInt8;
	typedef ax_u16_t                U16, UInt16;
	typedef ax_u32_t                U32, UInt32;
	typedef ax_u64_t                U64, UInt64;

	typedef ax_smax_t               SMax, IntMax;
	typedef ax_umax_t               UMax, UIntMax;

	typedef ax_sptr_t               SPtr, IntPtr;
	typedef ax_uptr_t               UPtr, UIntPtr;
	typedef ax_sint_t               SCPU, IntCPU;
	typedef ax_uint_t               UCPU, UIntCPU;

    typedef ax_size_t               SizeType;
    typedef ax_ptrdiff_t            DiffType, PtrDiffType;

#if AX_TYPES_CXX_NULLPTR_ENABLED
	typedef decltype(nullptr)       NullPtr;
#endif

}
#endif

#ifndef AX_COUNTOF
# define AX_COUNTOF(Arr1D_)         (sizeof((Arr1D_))/sizeof((Arr1D_)[0]))
#endif

#endif
