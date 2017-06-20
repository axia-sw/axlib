/*

	ax_platform - public domain
	Last update: 2015-10-01 Aaron Miller


	TODO
	
	- Add __has_feature macro emulation on non-Clang compilers
	- Add C99 and C11 feature listings

		http://clang.llvm.org/docs/LanguageExtensions.html


	LICENSE

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_PLATFORM_H_
#define INCGUARD_AX_PLATFORM_H_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

#ifndef AX_DEBUG_ENABLED
# if defined(DEBUG)||defined(_DEBUG)||defined(__DEBUG__)
#  define AX_DEBUG_ENABLED 1
# elif defined(__DEBUG)||defined(__debug__)||defined(MK_DEBUG)
#  define AX_DEBUG_ENABLED 1
# else
#  define AX_DEBUG_ENABLED 0
# endif
#endif
#ifndef AX_TEST_ENABLED
# if defined(_TEST)||defined(__TEST__)||defined(MK_TEST)
#  define AX_TEST_ENABLED 1
# elif defined(UT_TEST)||defined(__UNIT_TEST__)
#  define AX_TEST_ENABLED 1
# else
#  define AX_TEST_ENABLED 0
# endif
#endif

#ifndef AX_HAS_FEATURE
# ifdef __has_feature
#  define AX_HAS_FEATURE(x)        __has_feature(x)
# else
#  define AX_HAS_FEATURE(x)        0
# endif
#endif
#ifndef AX_USE_FEATURE
# ifdef __has_feature
#  define AX_USE_FEATURE(x)        __has_feature(x)
# else
#  define AX_USE_FEATURE(x)        1
# endif
#endif

#ifndef AX_HAS_EXTENSION
# ifdef __has_extension
#  define AX_HAS_EXTENSION(x)      __has_extension(x)
# else
#  define AX_HAS_EXTENSION(x)      0
# endif
#endif

#ifndef AX_HAS_ATTRIBUTE
# ifdef __has_attribute
#  define AX_HAS_ATTRIBUTE(x)      __has_attribute(x)
# else
#  define AX_HAS_ATTRIBUTE(x)      0
# endif
#endif

#ifndef AX_HAS_BUILTIN
# ifdef __has_builtin
#  define AX_HAS_BUILTIN(x)        __has_builtin(x)
# else
#  define AX_HAS_BUILTIN(x)        0
# endif
#endif

#ifndef AX_EXCEPTIONS_ENABLED
# if AX_HAS_FEATURE(cxx_exceptions)
#  define AX_EXCEPTIONS_ENABLED    1
# else
#  define AX_EXCEPTIONS_ENABLED    0
# endif
#endif


/*
===============================================================================

	COMPILER

	Compiler versions are defined as the following:

		AX_CLANG_VER (Clang)
		Major*100 + Minor*10 + PatchLevel. e.g., Clang 3.1 is 310.

		AX_GCC_VER (GNU C/C++ Compiler)
		Major*100 + Minor*10 + PatchLevel. e.g., GCC 4.9.1 is 491.

		AX_ICC_VER (Intel C/C++ Compiler)
		Version/10. e.g., Intel C/C++ 12.1 is 121.

		AX_MSC_VER (Microsoft C++ Compiler)
		Version/10. e.g., VS2010 is 160. (1600/10.)

	Additional helper macros are defined.

	For MSVC++, the following are especially useful:

		AX_MSC_2010 is defined to 1 if using the 2010 (or better) compiler.
		AX_MSC_2012 is defined to 1 if using the 2012 (or better) compiler.
		AX_MSC_2013, etc, are defined likewise.

	The following (boolean) macros are defined as well.

		AX_COMPILER_CLANG is 1 when using any Clang compiler version.
		AX_COMPILER_GCC is 1 when using any GCC/G++ compiler version.
		AX_COMPILER_ICC is 1 when using any Intel C/C++ compiler version.
		AX_COMPILER_MSC is 1 when using any Microsoft C++ compiler version.

	The above macros are always defined. They will be set to 0 if they are
	unavailable. (e.g., if not using Clang then AX_CLANG_VER is 0.)

===============================================================================
*/
#if defined( __clang__ )
# define AX_CLANG_VER ( __clang_major__*100 + __clang_minor__*10 + __clang_patchlevel__ )
#else
# define AX_CLANG_VER 0
#endif

#if defined( __GNUC__ )
# define AX_GCC_VER ( __GNUC__*100 + __GNUC_MINOR__*10 + __GNUC_PATCHLEVEL__ )
#else
# define AX_GCC_VER 0
#endif

#if defined( __INTEL_COMPILER )
# define AX_ICC_VER ( __INTEL_COMPILER/10 )
#else
# define AX_ICC_VER 0
#endif

#if defined( _MSC_VER )
# define AX_MSC_VER ( _MSC_VER/10 )
#else
# define AX_MSC_VER 0
#endif

#define AX_CLANG_2_9                ( AX_CLANG_VER >= 290 )
#define AX_CLANG_3_0                ( AX_CLANG_VER >= 300 )
#define AX_CLANG_3_1                ( AX_CLANG_VER >= 310 )
#define AX_CLANG_3_2                ( AX_CLANG_VER >= 320 )
#define AX_CLANG_3_3                ( AX_CLANG_VER >= 330 )
#define AX_CLANG_3_4                ( AX_CLANG_VER >= 340 )
#define AX_CLANG_3_5                ( AX_CLANG_VER >= 350 )
#define AX_CLANG_3_6                ( AX_CLANG_VER >= 360 )
#define AX_CLANG_3_7                ( AX_CLANG_VER >= 370 )

#define AX_GCC_4_0                  ( AX_GCC_VER >= 400 )
#define AX_GCC_4_1                  ( AX_GCC_VER >= 410 )
#define AX_GCC_4_2                  ( AX_GCC_VER >= 420 )
#define AX_GCC_4_3                  ( AX_GCC_VER >= 430 )
#define AX_GCC_4_4                  ( AX_GCC_VER >= 440 )
#define AX_GCC_4_5                  ( AX_GCC_VER >= 450 )
#define AX_GCC_4_6                  ( AX_GCC_VER >= 460 )
#define AX_GCC_4_7                  ( AX_GCC_VER >= 470 )
#define AX_GCC_4_8                  ( AX_GCC_VER >= 480 )
#define AX_GCC_4_8_1                ( AX_GCC_VER >= 481 )
#define AX_GCC_4_8_2                ( AX_GCC_VER >= 482 )
#define AX_GCC_4_9                  ( AX_GCC_VER >= 490 )
#define AX_GCC_4_9_1                ( AX_GCC_VER >= 491 )
#define AX_GCC_4_9_2                ( AX_GCC_VER >= 492 )
#define AX_GCC_5_0                  ( AX_GCC_VER >= 500 )

#define AX_ICC_11_0                 ( AX_ICC_VER >= 110 )
#define AX_ICC_11_1                 ( AX_ICC_VER >= 111 )
#define AX_ICC_12_0                 ( AX_ICC_VER >= 120 )
#define AX_ICC_12_1                 ( AX_ICC_VER >= 121 )
#define AX_ICC_13_0                 ( AX_ICC_VER >= 130 )
#define AX_ICC_14_0                 ( AX_ICC_VER >= 140 )
#define AX_ICC_15_0                 ( AX_ICC_VER >= 150 )

#define AX_MSC_2010                 ( AX_MSC_VER >= 160 )
#define AX_MSC_2012                 ( AX_MSC_VER >= 170 )
#define AX_MSC_2013                 ( AX_MSC_VER >= 180 )
#define AX_MSC_2013_CTP             ( AX_MSC_VER >= 181 ) // TODO
#define AX_MSC_2015                 ( AX_MSC_VER >= 190 )
#define AX_MSC_2017                 ( AX_MSC_VER >= 200 )

#define AX_COMPILER_CLANG           ( AX_CLANG_VER > 0 )
#define AX_COMPILER_GCC             ( AX_GCC_VER > 0 )
#define AX_COMPILER_ICC             ( AX_ICC_VER > 0 )
#define AX_COMPILER_MSC             ( AX_MSC_VER > 0 )

#define AX__CXXCHECK(A,B,C,D,F)     ( ( AX_##A || AX_##B || AX_##C || AX_##D ) && AX_USE_FEATURE(F) )
#define AX__CXXCHECK3(A,B,C,D,F)    ( ( AX_##A || AX_##B || AX_##C           ) && AX_USE_FEATURE(F) )
#define AX__CXXCHECK2(A,B,C,D,F)    ( ( AX_##A || AX_##B                     ) && AX_USE_FEATURE(F) )
#define AX__CXXCHECK1(A,B,C,D,F)    ( ( AX_##A                               ) && AX_USE_FEATURE(F) )
#define AX__CXXCHECKNF(A,B,C,D)     ( ( AX_##A || AX_##B || AX_##C || AX_##D )                      )
#define AX__CXXCHECK3NF(A,B,C,D,F)  ( ( AX_##A || AX_##B || AX_##C           )                      )
#define AX__CXXCHECK2NF(A,B,C,D,F)  ( ( AX_##A || AX_##B                     )                      )
#define AX__CXXCHECK1NF(A,B,C,D,F)  ( ( AX_##A                               )                      )


/*
===============================================================================

	C++11 SUPPORT

===============================================================================
*/

//
//  GENERAL
//

// Initialization of class objects by r-values
#define AX_CXX_N1610 AX__CXXCHECK(CLANG_2_9,GCC_4_3,ICC_11_1,MSC_2010,cxx_rvalue_references)
// static_assert
#define AX_CXX_N1720 AX__CXXCHECK(CLANG_2_9,GCC_4_3,ICC_11_1,MSC_2010,cxx_static_assert)
// Multi-declarator auto
#define AX_CXX_N1737 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_12_0 )
// Right angle brackets
#define AX_CXX_N1757 ( AX_CLANG_2_9 || AX_GCC_4_3 || AX_ICC_11_1 || AX_MSC_2010 )
// Extended friend declarations
#define AX_CXX_N1791 ( AX_CLANG_2_9 || AX_GCC_4_7 || AX_ICC_12_0 || AX_MSC_2010 )
// Built-in type traits
#define AX_CXX_N1836 ( AX_CLANG_3_0 || AX_GCC_4_3 || AX_ICC_11_0 || AX_MSC_2010 )
// auto: Deducing the type of variable from its initializer expression
#define AX_CXX_N1984 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_12_0 || AX_MSC_2010 )
// Delegating constructors
#define AX_CXX_N1986 ( AX_CLANG_3_0 || AX_GCC_4_7 || AX_ICC_14_0 || AX_MSC_2010 )
// extern templates
#define AX_CXX_N1987 ( AX_CLANG_2_9 || AX_GCC_4_3 || AX_ICC_11_1 || AX_MSC_2010 )
// R-value references
#define AX_CXX_N2118 AX__CXXCHECK(CLANG_2_9,GCC_4_3,ICC_12_0,MSC_2010,cxx_rvalue_references)
// Universal character names in literals
#define AX_CXX_N2170 ( AX_CLANG_3_1 || AX_GCC_4_5 || AX_ICC_12_1 )
// Generalized constant expressions
#define AX_CXX_N2235 AX__CXXCHECK(CLANG_3_1,GCC_4_6,ICC_14_0,MSC_2015,cxx_constexpr)
// Variadic templates v0.9
#define AX_CXX_N2242 AX__CXXCHECK(CLANG_2_9,GCC_4_3,ICC_14_0,MSC_2013,cxx_variadic_templates)
// char16_t/char32_t types
#define AX_CXX_N2249 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_14_0 || AX_MSC_2015 )
// Extended sizeof
#define AX_CXX_N2253 ( AX_CLANG_3_1 || AX_GCC_4_4 || AX_ICC_14_0 || AX_MSC_2015 )
// Alias templates
#define AX_CXX_N2258 ( AX_CLANG_3_0 || AX_GCC_4_7 || AX_ICC_12_1 || AX_MSC_2013 )
// Alignment support in language
#define AX_CXX_N2341 ( AX_CLANG_3_3 || AX_GCC_4_8 || AX_ICC_15_0 || AX_MSC_2015 )
// Standard-layout and trivial types
#define AX_CXX_N2342 ( AX_CLANG_3_0 || AX_GCC_4_5 || AX_ICC_14_0 || AX_MSC_2012 )
// decltype v1.0
#define AX_CXX_N2343 ( AX_CLANG_2_9 || AX_GCC_4_3 || AX_ICC_12_0 || AX_MSC_2010 )
// Defaulted and deleted functions
#define AX_CXX_N2346 AX__CXXCHECK(CLANG_3_0,GCC_4_4,ICC_12_0,MSC_2013,cxx_deleted_functions)
// Explicit enum bases and scoped enums
#define AX_CXX_N2347_PARTIAL ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_13_0 || AX_MSC_2012 )
// Strongly typed enums
#define AX_CXX_N2347 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_14_0 || AX_MSC_2012 )
// nullptr
#define AX_CXX_N2431 AX__CXXCHECK(CLANG_3_0,GCC_4_6,ICC_12_1,MSC_2010,cxx_nullptr)
// Explicit conversion operators
#define AX_CXX_N2437 ( AX_CLANG_3_0 || AX_GCC_4_5 || AX_ICC_13_0 || AX_MSC_2013 )
// R-value references for *this
#define AX_CXX_N2439 AX__CXXCHECK3(CLANG_2_9,GCC_4_8_1,ICC_14_0,cxx_rvalue_references)
// Raw and unicode string literals
#define AX_CXX_N2442 ( AX_CLANG_3_0 || AX_GCC_4_5 || AX_ICC_14_0 || AX_MSC_2015 )
// Inline namespaces
#define AX_CXX_N2535 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_14_0 || AX_MSC_2015 )
// Inheriting constructors
#define AX_CXX_N2540 ( AX_CLANG_3_3 || AX_GCC_4_8 || AX_ICC_15_0 || AX_MSC_2015 )
// auto: Trailing return types; late-specified return types
#define AX_CXX_N2541 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_12_1 || AX_MSC_2010 )
// Unrestricted unions
#define AX_CXX_N2544 ( AX_CLANG_3_1 || AX_GCC_4_6 || AX_MSC_2015 )
// auto v1.0: Removal of auto as storage-class specifier
#define AX_CXX_N2546 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_12_0 || AX_MSC_2010 )
// Lambdas v0.9: Lambda expressions and closures
#define AX_CXX_N2550 ( AX_CLANG_3_1 || AX_GCC_4_5 || AX_ICC_12_0 || AX_MSC_2010 )
// Variadic templates v1.0
#define AX_CXX_N2555 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_12_1 || AX_MSC_2013 )
// New-style expression SFINAE
#define AX_CXX_N2634 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_12_1 )
// Local and unnamed types as template arguments
#define AX_CXX_N2657 ( AX_CLANG_2_9 || AX_GCC_4_5 || AX_ICC_12_0 || AX_MSC_2010 )
// Lambdas v1.0: Constness of lambda functions
#define AX_CXX_N2658 ( AX_CLANG_3_1 || AX_GCC_4_5 || AX_ICC_12_0 || AX_MSC_2010 )
// Minimal support for garbage collection
#define AX_CXX_N2670 ( AX_ICC_15_0 || AX_MSC_2010 )
// General initializer lists
#define AX_CXX_N2672 ( AX_CLANG_3_1 || AX_GCC_4_4 || AX_ICC_14_0 || AX_MSC_2013 )
// Non-static data member initializers
#define AX_CXX_N2756 ( AX_CLANG_3_0 || AX_GCC_4_7 || AX_ICC_14_0 )
// Standard attributes
#define AX_CXX_N2761 AX__CXXCHECK3(CLANG_3_3,GCC_4_8,ICC_12_1,cxx_attributes)
// Forward declared enums
#define AX_CXX_N2764 ( AX_CLANG_3_1 || AX_GCC_4_6 || AX_ICC_14_0 || AX_MSC_2010 )
// User-defined literals
#define AX_CXX_N2765 AX__CXXCHECK(CLANG_3_1,GCC_4_7,ICC_15_0,MSC_2015,cxx_user_literals)
// R-value references v2
#define AX_CXX_N2844 ( AX_CLANG_3_0 || AX_GCC_4_6 || AX_ICC_12_0 || AX_MSC_2010 )
// Lambdas v1.1: New wording for C++0x/C++11 lambdas
#define AX_CXX_N2927 ( AX_CLANG_3_1 || AX_GCC_4_5 || AX_ICC_12_0 || AX_MSC_2012 )
// Explicit virtual overrides
#define AX_CXX_N2928 ( AX_ICC_14_0 )
// Range-based for loops
#define AX_CXX_N2930 ( AX_CLANG_3_0 || AX_GCC_4_6 || AX_ICC_13_0 || AX_MSC_2012 )
// Additional type traits
#define AX_CXX_N2947 ( AX_ICC_13_0 )
// noexcept; allowing move constructors to throw
#define AX_CXX_N3050 ( AX_CLANG_3_0 || AX_GCC_4_6 || AX_ICC_14_0 || AX_MSC_2015 )
// Conversions of lambdas to function pointers
#define AX_CXX_N3052 ( AX_ICC_13_0 )
// R-values: Defining move special member functions
#define AX_CXX_N3053 ( AX_CLANG_3_0 || AX_GCC_4_6 || AX_ICC_14_0 )
// Explicit virtual overrides: Override control (eliminating attributes)
#define AX_CXX_N3206 ( AX_CLANG_3_0 || AX_GCC_4_7 || AX_ICC_14_0 || AX_MSC_2012 )
// override and final
#define AX_CXX_N3272 ( AX_CLANG_3_0 || AX_GCC_4_7 || AX_ICC_14_0 || AX_MSC_2012 )
// decltype v1.1: Decltype and call expressions
#define AX_CXX_N3276 ( AX_CLANG_3_1 || AX_GCC_4_8_1 || AX_ICC_12_0 || AX_MSC_2012 )
// Allow typename outside of templates
#define AX_CXX_CI382 ( AX_ICC_12_0 )
// Default template arguments for function templates
#define AX_CXX_DR226 ( AX_CLANG_2_9 || AX_GCC_4_3 || AX_ICC_12_1 || AX_MSC_2013 )

//
//  CONCURRENCY
//

// exception_ptr: Propagating exceptions
#define AX_CXX_N2179 ( AX_CLANG_2_9 || AX_GCC_4_4 || AX_ICC_12_0 || AX_MSC_2010 )
// Reworded sequence points
#define AX_CXX_N2239 ( AX_CLANG_3_3 || AX_GCC_VER || AX_ICC_15_0 )
// quick_exit: Abandoning a process and at_quick_exit
#define AX_CXX_N2440 ( AX_GCC_4_8 )
// Atomic types and operations
#define AX_CXX_N2427 ( AX_CLANG_3_1 || AX_GCC_4_4 || AX_ICC_13_0 || AX_MSC_2012 )
// Memory model
#define AX_CXX_N2429 ( AX_CLANG_3_2 || AX_GCC_4_8 )
// Atomics in signal handlers
#define AX_CXX_N2547 ( AX_CLANG_3_1 || AX_GCC_VER )
// Thread-local storage
#define AX_CXX_N2659 ( AX_CLANG_3_3 || AX_GCC_4_8 )
// Magic statics: Dynamic initialization and destruction with concurrency
#define AX_CXX_N2660 ( AX_CLANG_2_9 || AX_GCC_4_3 )
// Data-dependency ordering
#define AX_CXX_N2664 ( AX_CLANG_3_2 || AX_GCC_4_4 || AX_MSC_2012 )
// Strong compare and exchange
#define AX_CXX_N2748 ( AX_CLANG_3_1 || AX_GCC_4_5 || AX_ICC_13_0 || AX_MSC_2012 )
// Bidirectional fences
#define AX_CXX_N2752 ( AX_CLANG_3_1 || AX_GCC_4_8 || AX_ICC_13_0 || AX_MSC_2012 )
// Data-dependency ordering: function annotation
#define AX_CXX_N2782 ( AX_ICC_15_0 )

//
//  C99 SUPPORT
//

// C99 preprocessor
#define AX_CXX_N1653 ( AX_CLANG_2_9 || AX_GCC_4_3 || AX_ICC_11_1 /*|| AX_MSC_2015 -- buggy*/ )
// long long
#define AX_CXX_N1811 ( AX_CLANG_2_9 || AX_GCC_4_3 || AX_ICC_11_1 || AX_MSC_2010 )
// Extended integer types
#define AX_CXX_N1988 ( AX_GCC_VER )
// __func__
#define AX_CXX_N2340 ( AX_CLANG_2_9 || AX_GCC_4_3 || AX_ICC_11_1 || AX_MSC_2015 )



/*
===============================================================================

	C++14 SUPPORT

===============================================================================
*/

//
//  CORE
//

// Tweak to certain C++ contextual conversions
#define AX_CXX_N3323 AX__CXXCHECK2(CLANG_3_4,GCC_4_9,cxx_contextual_conversions)
// Binary literals
#define AX_CXX_N3472 AX__CXXCHECK(CLANG_VER,GCC_4_9,ICC_11_1,MSC_2015,cxx_binary_literals)
// decltype( auto ); return type deduction for normal functions
#define AX_CXX_N3638 AX__CXXCHECK(CLANG_3_4,GCC_4_9,ICC_15_0,MSC_2013_CTP,cxx_decltype_auto)
// Initializd lambda captures
#define AX_CXX_N3648 ( AX_CLANG_3_4 || AX_GCC_4_9 || AX_ICC_15_0 || AX_MSC_2015 )
// Generic lambdas
#define AX_CXX_N3649 AX__CXXCHECK3(CLANG_3_4,GCC_4_9,MSC_2015,cxx_generic_lambdas)
// Variable templates
#define AX_CXX_N3651 AX__CXXCHECK2(CLANG_3_4,GCC_5_0,cxx_variable_templates)
// Relaxed requirements on constexpr functions
#define AX_CXX_N3652 AX__CXXCHECK2(CLANG_3_4,GCC_5_0,cxx_relaxed_constexpr)
// Member initializers and aggregates
#define AX_CXX_N3653 ( AX_CLANG_3_3 || AX_GCC_5_0 )
// Clarifying memory allocation
#define AX_CXX_N3664 ( AX_CLANG_3_4 )
// [[deprecated]] attribute
#define AX_CXX_N3760 ( AX_CLANG_3_4 || AX_GCC_4_9 )
// Sized deallocation
#define AX_CXX_N3778 ( AX_CLANG_3_4 || AX_GCC_5_0 )
// Single quotation mark as digit separator
#define AX_CXX_N3781 ( AX_CLANG_3_4 || AX_GCC_4_9 )



/*
===============================================================================

	C++1z SUPPORT

===============================================================================
*/

//
//  CORE
//

// Resumable functions (resume/await)
#define AX_CXX_N3858 ( AX_MSC_2015 )
// New auto rules for direct-list-initialization
#define AX_CXX_N3922 0
// static_assert with no message
#define AX_CXX_N3928 ( AX_CLANG_3_5 )
// Terse range-based for loops
#define AX_CXX_N3994 ( AX_MSC_2015 )
// typename in a template template parameter
#define AX_CXX_N4051 ( AX_CLANG_3_5 )
// Disabling trigraph expansion by default
#define AX_CXX_N4086 ( AX_CLANG_3_5 )
// Nested namespace definition
#define AX_CXX_N4230 ( AX_CLANG_3_6 )
// Attributes for namespaces and enumerators
#define AX_CXX_N4266 ( AX_CLANG_3_6 )
// u8 character literals
#define AX_CXX_N4267 ( AX_CLANG_3_6 )
// Allow constant evaluation for all non-type template arguments
#define AX_CXX_N4268 ( AX_CLANG_3_6 )
// Fold expressions
#define AX_CXX_N4295 ( AX_CLANG_3_6 )


/*
===============================================================================

	C++11 SUPPORT (FRIENDLY-NAMED)

===============================================================================
*/

//
//  alignas [N2341]
//
#ifndef AX_CXX_ALIGNAS_ENABLED
# define AX_CXX_ALIGNAS_ENABLED AX_CXX_N2341
#endif

//
//  alignof [N2341]
//
#ifndef AX_CXX_ALIGNOF_ENABLED
# define AX_CXX_ALIGNOF_ENABLED AX_CXX_N2341
#endif

//
//  C++11 Atomics [N2427]
//
#ifndef AX_CXX_ATOMICS_ENABLED
# define AX_CXX_ATOMICS_ENABLED AX_CXX_N2427
#endif

//
//  auto [N1984,N2546]
//
#ifndef AX_CXX_AUTO_ENABLED
# define AX_CXX_AUTO_ENABLED AX_CXX_N1984
#endif

//
//  C99 preprocessor [N1653]
//
#ifndef AX_C99_PREPROCESSOR_ENABLED
# define AX_C99_PREPROCESSOR_ENABLED AX_CXX_N1653
#endif

//
//  constexpr [N2235]
//
#ifndef AX_CXX_CONSTEXPR_ENABLED
# define AX_CXX_CONSTEXPR_ENABLED AX_CXX_N2235
#endif

//
//  decltype [N2343,N3276]
//
#ifndef AX_CXX_DECLTYPE_ENABLED
# define AX_CXX_DECLTYPE_ENABLED AX_CXX_N2343
#endif

//
//  Defaulted and Deleted Functions [N2346]
//
#ifndef AX_CXX_DEFAULT_DELETE_FUNCTIONS_ENABLED
# define AX_CXX_DEFAULT_DELETE_FUNCTIONS_ENABLED AX_CXX_N2346
#endif

//
//  Delegating Constructors [N1986]
//
#ifndef AX_CXX_DELEGATING_CTORS_ENABLED
# define AX_CXX_DELEGATING_CTORS_ENABLED AX_CXX_N1986
#endif

//
//  Explicit Conversion Operators [N2437]
//
#ifndef AX_CXX_EXPLICIT_CONVERSION_OPERATORS_ENABLED
# define AX_CXX_EXPLICIT_CONVERSION_OPERATORS_ENABLED AX_CXX_N2437
#endif

//
//  Extended 'friend' Declarations [N1791]
//
#ifndef AX_CXX_EXTENDED_FRIEND_DECLARATIONS_ENABLED
# define AX_CXX_EXTENDED_FRIEND_DECLARATIONS_ENABLED AX_CXX_N1791
#endif

//
//  extern template [N1987]
//
#ifndef AX_CXX_EXTERN_TEMPLATE_ENABLED
# define AX_CXX_EXTERN_TEMPLATE_ENABLED AX_CXX_N1987
#endif

//
//  Forward Declarations For Enums [N2764]
//
#ifndef AX_CXX_FORWARD_ENUMS_ENABLED
# define AX_CXX_FORWARD_ENUMS_ENABLED AX_CXX_N2764
#endif

//
//  Inheriting Constructors [N2540]
//
#ifndef AX_CXX_INHERITING_CONSTRUCTORS_ENABLED
# define AX_CXX_INHERITING_CONSTRUCTORS_ENABLED AX_CXX_N2540
#endif

//
//  Initializer Lists [N2672]
//
#ifndef AX_CXX_INIT_LISTS_ENABLED
# define AX_CXX_INIT_LISTS_ENABLED AX_CXX_N2672
#endif

//
//  Lambda Expressions and Closures [N2550,N2658,N2927]
//
#ifndef AX_CXX_LAMBDA_ENABLED
# define AX_CXX_LAMBDA_ENABLED AX_CXX_N2550
#endif

//
//  Local/Anonymous Types as Template Arguments [N2657]
//
#ifndef AX_CXX_LOCAL_TYPE_TEMPLATE_ARGS_ENABLED
# define AX_CXX_LOCAL_TYPE_TEMPLATE_ARGS_ENABLED AX_CXX_N2657
#endif

//
//  Long Long [N1811]
//
#ifndef AX_CXX_LONG_LONG_ENABLED
# define AX_CXX_LONG_LONG_ENABLED AX_CXX_N1811
#endif

//
//  New Function Declaration Syntax For Deduced Return Types [N2541]
//
#ifndef AX_CXX_AUTO_FUNCTIONS_ENABLED
# define AX_CXX_AUTO_FUNCTIONS_ENABLED AX_CXX_N2541
#endif

//
//  nullptr [N2431]
//
#ifndef AX_CXX_NULLPTR_ENABLED
# define AX_CXX_NULLPTR_ENABLED AX_CXX_N2431
#endif

//
//  R-Value References / std::move [N2118,N2844,N2844+,N3053]
//
#ifndef AX_CXX_RVALUE_REFS_ENABLED
# define AX_CXX_RVALUE_REFS_ENABLED AX_CXX_N2118
#endif

//
//  static_assert [N1720]
//
#ifndef AX_CXX_STATIC_ASSERT_ENABLED
# define AX_CXX_STATIC_ASSERT_ENABLED AX_CXX_N1720
#endif

//
//  Strongly-typed Enums [N2347]
//
#ifndef AX_CXX_STRONG_ENUMS_ENABLED
# define AX_CXX_STRONG_ENUMS_ENABLED AX_CXX_N2347
#endif

//
//  Template Aliases [N2258]
//
#ifndef AX_CXX_TEMPLATE_ALIASES_ENABLED
# define AX_CXX_TEMPLATE_ALIASES_ENABLED ( AX_CXX_N2258 )
#endif

//
//  Thread Local Storage [N2659]
//
#ifndef AX_CXX_TLS_ENABLED
# define AX_CXX_TLS_ENABLED AX_CXX_N2659
#endif

//
//  Built-in Type Traits [N1836]
//
#ifndef AX_CXX_TYPE_TRAITS_ENABLED
# define AX_CXX_TYPE_TRAITS_ENABLED AX_CXX_N1836
#endif

//
//  Variadic Templates [N2242,N2555]
//
#ifndef AX_CXX_VARIADIC_TEMPLATES_ENABLED
//# define AX_CXX_VARIADIC_TEMPLATES_ENABLED AX_CXX_N2242
# define AX_CXX_VARIADIC_TEMPLATES_ENABLED 0
#endif

//
//  Range-based For-loops [N2930]
//
#ifndef AX_CXX_RANGE_FOR_ENABLED
# define AX_CXX_RANGE_FOR_ENABLED AX_CXX_N2930
#endif

//
//  override and final [N2928,N3206,N3272]
//
#ifndef AX_CXX_OVERRIDE_FINAL_ENABLED
# define AX_CXX_OVERRIDE_FINAL_ENABLED AX_CXX_N2928
#endif

//
//  Attributes [N2761]
//
#ifndef AX_CXX_ATTRIBUTES_ENABLED
# define AX_CXX_ATTRIBUTES_ENABLED AX_CXX_N2761
#endif

//
//  Ref-Qualifiers [N2439]
//
#ifndef AX_CXX_REF_QUALIFIERS_ENABLED
# define AX_CXX_REF_QUALIFIERS_ENABLED AX_CXX_N2439
#endif

//
//  Non-static Data Member Initializers [N2756]
//
#ifndef AX_CXX_NONSTATIC_INIT_ENABLED
# define AX_CXX_NONSTATIC_INIT_ENABLED AX_CXX_N2756
#endif


/*
===============================================================================

	C++11 HELPERS

===============================================================================
*/

#ifndef __cplusplus
# undef AX_CXX_CONSTEXPR_ENABLED
# define AX_CXX_CONSTEXPR_ENABLED   0
/* TODO: This needs friends! Please bring it friends! */
#endif

#ifndef AX_DELETE_FUNC
# if AX_CXX_N2346
#  define AX_DELETE_FUNC            = delete
# else
#  define AX_DELETE_FUNC
# endif
#endif

#ifndef AX_DELETE_COPYCTOR
# if AX_CXX_N2346
#  define AX_DELETE_COPYCTOR(N_)    N_( const N_ & ) = delete
# else
#  define AX_DELETE_COPYCTOR(N_)
# endif
#endif
#ifndef AX_DELETE_ASSIGNOP
# if AX_CXX_N2346
#  define AX_DELETE_ASSIGNOP(N_)    N_ &operator=( const N_ & ) = delete
# else
#  define AX_DELETE_ASSIGNOP(N_)
# endif
#endif
#ifndef AX_DELETE_COPYFUNCS
# define AX_DELETE_COPYFUNCS(T_)\
									AX_DELETE_COPYCTOR(T_);\
									AX_DELETE_ASSIGNOP(T_)
#endif

#ifndef AX_NOTHROW
# define AX_NOTHROW                 throw()
#endif

#ifndef AX_CONSTEXPR
# if AX_CXX_CONSTEXPR_ENABLED
#  define AX_CONSTEXPR              constexpr
# else
#  define AX_CONSTEXPR
# endif
#endif

#ifndef AX_CONSTEXPR_INLINE
# if AX_CXX_CONSTEXPR_ENABLED
#  define AX_HAS_CONSTEXPR_INLINE   1
#  define AX_CONSTEXPR_INLINE       AX_CONSTEXPR
# elif defined( __cplusplus )
#  define AX_HAS_CONSTEXPR_INLINE   1
#  define AX_CONSTEXPR_INLINE       inline
# else
#  define AX_HAS_CONSTEXPR_INLINE   0
#  define AX_CONSTEXPR_INLINE
# endif
#endif

#ifndef AX_STATIC_CONST
# if AX_CXX_CONSTEXPR_ENABLED
#  define AX_STATIC_CONST           constexpr
# else
#  define AX_STATIC_CONST           static const
# endif
#endif

#ifndef AX_HAS_STATIC_ASSERT
# define AX_HAS_STATIC_ASSERT       AX_CXX_N1720
#endif

#ifndef AX_STATIC_ASSERT_ENABLED
# define AX_STATIC_ASSERT_ENABLED   1
#endif

#if AX_STATIC_ASSERT_ENABLED
# if AX_HAS_STATIC_ASSERT
#  define ax_static_assert( expr, msg ) static_assert( expr, msg )
# elif AX_HAS_FEATURE(c_static_assert)
#  define ax_static_assert( expr, msg ) _Static_assert( expr, msg )
# elif defined(__COUNTER__)
#  define ax_static_assert( expr, msg )\
	enum { AX_CONCAT( AX_StaticAssert_, __COUNTER__ ) = 1/( +!!( expr ) ) }
# else
#  define ax_static_assert( expr, msg )\
	enum { AX_CONCAT( AX_StaticAssert_, __LINE__ ) = 1/( +!!( expr ) ) }
# endif
#endif

#ifndef AX_OVERRIDE
# if AX_CXX_N3272
#  define AX_OVERRIDE               override
# else
#  define AX_OVERRIDE
# endif
#endif
#ifndef AX_FINAL
# if AX_CXX_N3272
#  define AX_FINAL                  final
# else
#  define AX_FINAL
# endif
#endif


/*
===============================================================================

	PLATFORM ARCHITECTURE

	AX_ARCH_X86 is defined to 1 when targeting a x86/x64 platform; else 0.
	AX_ARCH_ARM is defined to 1 when targeting an ARM platform; else 0.

	AX_ARCH_64BIT is defined to 1 when the target architecture is 64-bit.

===============================================================================
*/

#if defined( __amd64__ ) || defined( __x86_64__ ) || defined( _M_AMD64 )
# define AX_ARCH_X86                1
# define AX_ARCH_64BIT              1
#elif defined( __x86__ ) || defined( __i386__ ) || defined( _M_IX86 )
# define AX_ARCH_X86                1
#elif defined( __aarch64__ )
# define AX_ARCH_ARM                1
# define AX_ARCH_64BIT              1
#elif defined( __arm__ ) || defined( _ARM )
# define AX_ARCH_ARM                1
#endif

#if defined( __arm__ )
# define AX_ARCH_ARM                1
#endif

#if !defined( AX_ARCH_X86 )
# define AX_ARCH_X86                0
#endif
#if !defined( AX_ARCH_ARM )
# define AX_ARCH_ARM                0
#endif

#if !defined( AX_ARCH_64BIT )
# define AX_ARCH_64BIT              0
#endif

#if !AX_ARCH_X86 && !AX_ARCH_ARM
# error Unknown architecture
#endif


/*
===============================================================================

	PLATFORM SYSTEM

	AX_OS_WINDOWS is defined to 1 when targeting desktop/uwp windows; else 0.
	AX_OS_LINUX is defined to 1 when targeting linux; else 0.
	AX_OS_MACOSX is defined to 1 when targeting mac os x; else 0.
	AX_OS_ANDROID is defined to 1 when targeting android; else 0.
	AX_OS_IOS is defined to 1 when targeting ios; else 0.
	AX_OS_UWP is defined to 1 when targeting universal windows apps; else 0.

	You can override the OS outside of this file by defining it beforehand.
	e.g., in your Makefile or project settings.

===============================================================================
*/

#if defined( _WIN32 )
# ifndef AX_OS_WINDOWS
#  define AX_OS_WINDOWS             1
# endif
# if !defined( AX_OS_UWP ) && AX_OS_WINDOWS
#  ifdef WINAPI_FAMILY /* WINAPI_FAMILY might not be defined yet */
#   ifdef WINAPI_FAMILY_PC_APP /* PC App = UWP */
#    if WINAPI_FAMILY == WINAPI_FAMILY_PC_APP
#     undef AX_OS_UWP__ENABLE
#     define AX_OS_UWP__ENABLE
#    endif
#   endif
#   ifdef WINAPI_FAMILY_PHONE_APP /* Phone App = UWP */
#    if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#     undef AX_OS_UWP__ENABLE
#     define AX_OS_UWP__ENABLE
#    endif
#   endif
#  endif
#  if defined( WINAPI_PARTITION_PHONE_APP ) /* Phone app partition is definitely UWP */
#   if WINAPI_PARTITION_PHONE_APP
#    undef AX_OS_UWP__ENABLE
#    define AX_OS_UWP__ENABLE
#   endif
#  endif
#  if defined( WINAPI_PARTITION_PC_APP ) /* PC app partition *might* be UWP */
#   if WINAPI_PARTITION_PC_APP
#    undef AX_OS_UWP__ENABLE
#    define AX_OS_UWP__ENABLE
#   endif
#  endif
#  if defined( WINAPI_PARTITION_APP ) /* App partition *might* be UWP */
#   if WINAPI_PARTITION_APP
#    undef AX_OS_UWP__ENABLE
#    define AX_OS_UWP__ENABLE
#   endif
#  endif
#  ifdef WINAPI_PARTITION_DESKTOP /* Desktop partition means *NOT* UWP */
#   if WINAPI_PARTITION_DESKTOP
#    undef AX_OS_UWP__ENABLE /* undef only */
#   endif
#  endif
#  ifdef AX_OS_UWP__ENABLE
#   undef AX_OS_UWP__ENABLE
#   define AX_OS_UWP                1
#  endif
# endif
#elif defined( __linux__ )
# if defined( ANDROID )
#  ifndef AX_OS_ANDROID
#   define AX_OS_ANDROID            1
#  endif
# else
#  ifndef AX_OS_LINUX
#   define AX_OS_LINUX              1
#  endif
# endif
#elif defined( __APPLE__ )
# if defined(__has_include) && !defined(AX_APPLE_NO_TARGETCONDITIONALS_H)
#  if __has_include("TargetConditionals.h")
#   include "TargetConditionals.h"
#  endif
# endif
# if defined(TARGET_OS_OSX) || defined(TARGET_OS_MAC) || !defined(TARGET_OS_IPHONE)
#  ifndef AX_OS_MACOSX
#   define AX_OS_MACOSX             1
#  endif
# elif defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#  ifndef AX_OS_IOS
#   define AX_OS_IOS                1
#  endif
# else /* Handle edge cases */
#  ifndef AX_OS_MACOSX
#   define AX_OS_MACOSX             1
#  endif
# endif
#endif

#ifndef AX_OS_WINDOWS
# define AX_OS_WINDOWS              0
#endif
#ifndef AX_OS_LINUX
# define AX_OS_LINUX                0
#endif
#ifndef AX_OS_MACOSX
# define AX_OS_MACOSX               0
#endif
#ifndef AX_OS_ANDROID
# define AX_OS_ANDROID              0
#endif
#ifndef AX_OS_IOS
# define AX_OS_IOS                  0
#endif
#ifndef AX_OS_UWP
# define AX_OS_UWP                  0
#endif

#if !( AX_OS_WINDOWS|AX_OS_LINUX|AX_OS_MACOSX|AX_OS_ANDROID|AX_OS_IOS )
# error Unknown OS
#endif


/*
===============================================================================

	PLATFORM INTRINSICS

	AX_INTRIN_SSE is defined to 1 when targeting x86/x64; else 0.
	AX_INTRIN_NEON is defined to 1 when targeting ARM; else 0.

	If AX_INTRINSICS_ENABLED is defined to 0 then none of the above will be
	defined to 1.

===============================================================================
*/

#ifndef AX_INTRINSICS_ENABLED
# define AX_INTRINSICS_ENABLED      1
#endif

#define AX_INTRIN_SSE               ( AX_INTRINSICS_ENABLED && AX_ARCH_X86 )
#define AX_INTRIN_NEON              ( AX_INTRINSICS_ENABLED && AX_ARCH_ARM )

#define AX_INTRIN_NONE              ( !AX_INTRIN_SSE && !AX_INTRIN_NEON )


/*
===============================================================================

	HELPER MACROS/DEFINES

===============================================================================
*/

/// @def AX_FUNCTION
/// Current function identifier.
#ifndef AX_FUNCTION
# if AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_FUNCTION               __func__
# elif AX_COMPILER_MSC || AX_COMPILER_ICC
#  define AX_FUNCTION               __FUNCTION__
# else
#  define AX_FUNCTION               ( ( const char * )0 )
# endif
#endif

/// @def AX_PRETTY_FUNCTION
/// Current function identifier (with extra information)
#ifndef AX_PRETTY_FUNCTION
# if AX_COMPILER_MSC || AX_COMPILER_ICC
#  define AX_PRETTY_FUNCTION        __FUNCTION__
# elif AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_PRETTY_FUNCTION        __PRETTY_FUNCTION__
# elif __STDC_VERSION__ >= 19901
#  define AX_PRETTY_FUNCTION        __func__
# else
#  define AX_PRETTY_FUNCTION        ( ( const char * )0 )
# endif
#endif

/// @def AX_ATTRIB_UNUSED
/// Mark function or variable as being potentially unused to prevent compiler
/// warnings.
#ifndef AX_ATTRIB_UNUSED
# if AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_ATTRIB_UNUSED __attribute__((unused))
# else
#  define AX_ATTRIB_UNUSED
# endif
#endif

/// @def AX_PURE
/// Pure-functional attribute.
///
/// Functions marked as pure-functional do not depend on varying state except
/// for their input parameters, and do not alter state. i.e., their output is
/// solely dependent on their input, and nothing else.
#ifndef AX_PURE
# if AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_PURE                   __attribute__( ( const ) )
# else
#  define AX_PURE
# endif
#endif

/// @def AX_WEAK
/// Weak-linking attribute.
///
/// Symbols marked as weak-linked can appear multiple times across several
/// object files. Only one symbol will be selected. This helps reduce memory and
/// storage consumption.
///
/// @def AX_HAS_WEAK
/// Set to 1 if AX_WEAK is available, or 0 if not.
#ifndef AX_WEAK
# if AX_COMPILER_MSC || AX_COMPILER_ICC
#  define AX_WEAK                   __declspec( selectany )
#  define AX_HAS_WEAK               1
# elif 0 && ( AX_COMPILER_GCC || AX_COMPILER_CLANG )
#  define AX_WEAK                   __attribute__( ( weak ) )
#  define AX_HAS_WEAK               1
# else
#  define AX_WEAK
#  define AX_HAS_WEAK               0
# endif
#endif

/// @def AX_GLOBAL
/// Preferred qualifiers and attributes for weak globals
#ifndef AX_GLOBAL
# if AX_HAS_WEAK
#  define AX_GLOBAL                 AX_WEAK static extern
# else
#  define AX_GLOBAL                 static
# endif
#endif

/// @def AX_GLOBAL_CONST
/// Preferred qualifiers and attributes for global constants.
#ifndef AX_GLOBAL_CONST
# if AX_HAS_WEAK
#  define AX_GLOBAL_CONST           AX_WEAK extern const AX_ATTRIB_UNUSED
# else
#  define AX_GLOBAL_CONST           static const AX_ATTRIB_UNUSED
# endif
#endif

/// @def AX_NORETURN
/// Function attribute stating that it will not return to the calling code.
#ifndef AX_NORETURN
# if AX_COMPILER_MSC || AX_COMPILER_INTEL
#  define AX_NORETURN               __declspec( noreturn )
# elif AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_NORETURN               __attribute__( ( noreturn ) )
# else
#  define AX_NORETURN
# endif
#endif

/// @def AX_CACHE_SIZE
/// Default cache-size. (Useful for adjusting algorithms or buffer sizes.)
#ifndef AX_CACHE_SIZE
# if AX_ARCH_X86 && AX_ARCH_64BIT
#  define AX_CACHE_SIZE             128
# else
#  define AX_CACHE_SIZE             64
# endif
#endif
#define AX_POINTERS_PER_CACHELINE   ( AX_CACHE_SIZE/sizeof( void * ) )

/// @def AX_ALIGN
/// Compiler attribute for aligning.
///
/// @def AX_HAS_ALIGN
/// Set to 1 if the compiler provides a method for aligning symbols, or 0 if
/// not.
///
/// @def AX_CACHEALIGN
/// Enables alignment to the default cache-size
#ifndef AX_ALIGN
# if AX_COMPILER_MSC || AX_COMPILER_ICC
#  define AX_ALIGN( x )             __declspec( align( x ) )
#  define AX_HAS_ALIGN              1
# elif AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_ALIGN( x )             __attribute__( ( align( x ) ) )
#  define AX_HAS_ALIGN              1
# else
#  define AX_ALIGN( x )
#  define AX_HAS_ALIGN              0
# endif
# define AX_CACHEALIGN              AX_ALIGN( AX_CACHE_SIZE )
#endif

/// @def AX_HAS_VECTORCALL
/// Set to 1 if the vector-call calling convention is supported by the compiler,
/// or 0 if not.
#ifndef AX_HAS_VECTORCALL
# define AX_HAS_VECTOR_CALL         0
# if AX_COMPILER_MSC && AX_ARCH_X86 && ( !defined( _M_IX86_FP ) || _M_IX86_FP > 1 )
#  if ( _MSC_FULL_VER >= 170065501 && _MSC_VER < 1800 ) || _MSC_FULL_VER >= 180020418
#   undef  AX_HAS_VECTOR_CALL
#   define AX_HAS_VECTOR_CALL       1
#  endif
# endif
#endif

/// @def AX_VECTORCALL
/// Preferred calling convention for functions taking SIMD vector parameters.
#ifndef AX_VECTORCALL
# if AX_HAS_VECTOR_CALL
#  define AX_VECTORCALL             __vectorcall
# elif AX_ARCH_X86 && AX_OS_WINDOWS
#  define AX_VECTORCALL             __fastcall
# else
#  define AX_VECTORCALL             /*blank*/
# endif
#endif
/// @def AX_VCALL
/// Same as AX_VECTORCALL, but shorter for convenience.
#ifndef AX_VCALL
# define AX_VCALL                   AX_VECTORCALL
#endif

/// @def AX_CONCAT( x,y )
/// Concatenates two symbol names ( \a x and \a y ) into one.
///
/// @def AX_ANONVAR( x )
/// Creates an anonymous variable with base name \a x.
#define AX_CONCAT_IMPL( x, y )      x##y
#define AX_CONCAT( x, y )           AX_CONCAT_IMPL( x, y )

#ifdef __COUNTER__
# define AX_ANONVAR( x )            AX_CONCAT( x, __COUNTER__ )
#else
# define AX_ANONVAR( x )            AX_CONCAT( x, __LINE__ )
#endif

/// @def AX_IF_LIKELY
/// An if-statement marked as being likely to succeed.
///
/// @warning Use this sparingly and only when you are certain that a particular
///          if-statement will succeed often. Failure to do so can have adverse
///          effects on performance.
///
/// @def AX_IF_UNLIKELY
/// An if-statement marked as being unlikely to succeed.
///
/// @warning Use this sparingly and only when you are certain that a particular
///          if-statement will not succeed often. Failure to do so can have
///          adverse effects on performance.
#if AX_COMPILER_GCC || AX_COMPILER_CLANG
# define AX_IF_LIKELY( expr )       if( __builtin_expect( !!( expr ), 1 ) )
# define AX_IF_UNLIKELY( expr )     if( __builtin_expect( !!( expr ), 0 ) )
#else
# define AX_IF_LIKELY( expr )       if( expr )
# define AX_IF_UNLIKELY( expr )     if( expr )
#endif

#ifndef AX_PRINTF_PARM
# if AX_MSC_2010 && !AX_MSC_2015
#  include <CodeAnalysis/sourceannotations.h>
#  define AX_PRINTF_PARM            [SA_FormatString(Style="printf")]
# else
#  define AX_PRINTF_PARM
# endif
#endif

#ifndef AX_PRINTF_ATTR
# if 0 && ( AX_COMPILER_GCC || AX_COMPILER_CLANG ) // disabled for now
#  if defined( _WIN32 )
#   define AX_PRINTF_ATTR( x, y )   __attribute__( ( format( ms_printf, x, y ) ) )
#  else
#   define AX_PRINTF_ATTR( x, y )   __attribute__( ( format( printf, x, y ) ) )
#  endif
# else
#  define AX_PRINTF_ATTR( x, y )
# endif
#endif

#ifndef AX_INLINE
# if defined( __cplusplus )
#  define AX_INLINE                 inline
#  define AX_HAS_INLINE             1
# elif defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 199901L
#  define AX_INLINE                 inline
#  define AX_HAS_INLINE             1
# elif AX_COMPILER_MSC || AX_COMPILER_INTEL
#  define AX_INLINE                 __inline
#  define AX_HAS_INLINE             1
# else
/*
	TODO: GCC/Clang equivalent of __inline for non-C99 code
*/
#  define AX_INLINE
#  define AX_HAS_INLINE             0
# endif
#endif

#ifndef AX_FORCEINLINE
# if AX_COMPILER_MSC || AX_COMPILER_ICC
#  define AX_FORCEINLINE            __forceinline
#  define AX_HAS_FORCEINLINE        1
# elif AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_FORCEINLINE            AX_INLINE __attribute__((always_inline))
#  define AX_HAS_FORCEINLINE        1
# else
#  define AX_FORCEINLINE            AX_INLINE
#  define AX_HAS_FORCEINLINE        0
# endif
#endif

#ifndef AX_NOINLINE
# if AX_COMPILER_MSC || AX_COMPILER_ICC
#  define AX_NOINLINE               __declspec( noinline )
#  define AX_HAS_NOINLINE           1
# elif AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_NOINLINE               AX_INLINE __attribute__((noinline))
#  define AX_HAS_NOINLINE           1
# else
#  define AX_NOINLINE
#  define AX_HAS_NOINLINE           0
# endif
#endif

#ifndef AX_THREADLOCAL
# ifdef _WIN32
#  define AX_THREADLOCAL            __declspec( thread )
#  define AX_HAS_THREADLOCAL        1
# elif AX_COMPILER_GCC || AX_COMPILER_CLANG
#  define AX_THREADLOCAL            __attribute__((thread_local))
#  define AX_HAS_THREADLOCAL        1
# else
#  define AX_THREADLOCAL
#  define AX_HAS_THREADLOCAL        0
# endif
#endif

#ifndef AX_DLLIMPORT
# ifdef _WIN32
#  define AX_DLLIMPORT              __declspec(dllimport)
# else
#  define AX_DLLIMPORT
# endif
#endif

#ifndef AX_DLLEXPORT
# ifdef _WIN32
#  define AX_DLLEXPORT              __declspec(dllexport)
# else
#  define AX_DLLEXPORT
# endif
#endif

#ifndef AX_UNREACHABLE
# if defined( _MSC_VER )
#  define AX_UNREACHABLE()          __assume(false)
#  define AX_HAS_UNREACHABLE        1
# elif AX_HAS_BUILTIN(__builtin_unreachable) || AX_GCC_4_5
#  define AX_UNREACHABLE()          __builtin_unreachable()
#  define AX_HAS_UNREACHABLE        1
# else
#  define AX_UNREACHABLE()
#  define AX_HAS_UNREACHABLE        0
# endif
#elif !defined( AX_HAS_UNREACHABLE )
# define AX_HAS_UNREACHABLE         1 /* user provided; assume yes */
#endif

#ifndef AX_TRAP
# if defined( _MSC_VER )
#  define AX_TRAP()                 __debugbreak()
#  define AX_HAS_TRAP               1
# elif AX_HAS_BUILTIN( __builtin_trap ) || AX_GCC_4_3
#  define AX_TRAP()                 __builtin_trap()
#  define AX_HAS_TRAP               1
# else
#  define AX_TRAP()                 do{*(volatile int*)23=0;}while(0)
#  define AX_HAS_TRAP               0
# endif
#elif !defined( AX_HAS_TRAP )
# define AX_HAS_TRAP                1 /* user provided; assume yes */
#endif

#ifndef AX_DEPRECATED
# if defined( _MSC_VER )
#  define AX_DEPRECATED(Dcl_,Msg_)  __declspec(deprecated(Msg_)) Dcl_
#  define AX_HAS_DEPRECATED         1
#  define AX_HAS_DEPRECATED_MSG     1
# elif AX_HAS_FEATURE(attribute_deprecated_with_message)
#  define AX_DEPRECATED(Dcl_,Msg_)  _Dcl __attribute__((deprecated(Msg_)))
#  define AX_HAS_DEPRECATED         1
#  define AX_HAS_DEPRECATED_MSG     1
# elif defined( __GNUC__ )
#  define AX_DEPRECATED(Dcl_,Msg_)  _Dcl __attribute__((deprecated))
#  define AX_HAS_DEPRECATED         1
#  define AX_HAS_DEPRECATED_MSG     0
# else
#  define AX_DEPRECATED(Dcl_)       _Dcl
#  define AX_HAS_DEPRECATED         0
#  define AX_HAS_DEPRECATED_MSG     0
# endif
#else
# ifndef AX_HAS_DEPRECATED
#  define AX_HAS_DEPRECATED         1 /* user provided; assume yes */
# endif
# ifndef AX_HAS_DEPRECATED_MSG
#  define AX_HAS_DEPRECATED_MSG     0 /* assume not because left undefined */
# endif
#endif




/*
===============================================================================

	STATIC ANALYSIS AND DIAGNOSTICS

===============================================================================
*/

#ifndef AX_PUSH_DISABLE_WARNING_MSVC
# if AX_COMPILER_MSC
/* Temporarily disable a warning in MSVC++ */
#  define AX_PUSH_DISABLE_WARNING_MSVC(N_)\
									__pragma(warning(push))\
									__pragma(warning(disable:N_))
/* Restore whether the prior warning was enabled or disabled for MSVC++ */
#  define AX_POP_DISABLE_WARNING_MSVC()\
									__pragma(warning(pop))
# else
#  define AX_PUSH_DISABLE_WARNING_MSVC(N_)
#  define AX_POP_DISABLE_WARNING_MSVC()
# endif
#endif

#ifndef AX_GCC_PRAGMA
# ifdef __GNUC__
#  define AX_GCC_PRAGMA(X_) _Pragma(#X_)
# else
#  define AX_GCC_PRAGMA(X_)
# endif
#endif

#ifndef AX_PUSH_DISABLE_WARNING_GCC
# define AX_PUSH_DISABLE_WARNING_GCC(Flag_)\
									AX_GCC_PRAGMA(GCC diagnostic push)\
									AX_GCC_PRAGMA(GCC diagnostic ignored Flag_)
# define AX_POP_DISABLE_WARNING_GCC()\
									AX_GCC_PRAGMA(GCC diagnostic pop)
#endif

#ifndef AX_PUSH_DISABLE_WARNING_CLANG
# if AX_COMPILER_CLANG
/* Temporarily disable a warning in Clang */
#  define AX_PUSH_DISABLE_WARNING_CLANG(N_)\
									__pragma(clang diagnostic push)\
									__pragma(clang diagnostic ignored N_)
/* Restore whether the prior warning was enabled or disabled for Clang */
#  define AX_POP_DISABLE_WARNING_CLANG()\
									__pragma(clang diagnostic pop)
# else
#  define AX_PUSH_DISABLE_WARNING_CLANG(N_)
#  define AX_POP_DISABLE_WARNING_CLANG()
# endif
#endif

/* Static code analysis enabled */
#ifndef AX_STATIC_ANALYSIS
# if defined( _MSC_VER ) && ( defined( _PREFAST_ ) || defined( __CODE_ANALYSIS__ ) )
#  define AX_STATIC_ANALYSIS 1
# else
#  define AX_STATIC_ANALYSIS 0
# endif
#endif

#if AX_STATIC_ANALYSIS && defined( _MSC_VER )
# define AX__SAL 1
# include <sal.h>
#else
# define AX__SAL 0
#endif

/* Declare that the next instance of a warning should be ignored */
#ifndef AX_STATIC_SUPPRESS
# ifdef _MSC_VER
#  define AX_STATIC_SUPPRESS(code) __pragma( warning( suppress: code ) )
# else
#  define AX_STATIC_SUPPRESS(code)
# endif
#endif

/* Tell the analyzer to assume a certain expression to be true */
#ifndef AX_STATIC_ASSUME
# ifdef _MSC_VER
#  define AX_STATIC_ASSUME(expr) __analysis_assume( !!( expr ) )
# else
#  define AX_STATIC_ASSUME(expr)
# endif
#endif

/* "In" annotation */
#ifndef AX_IN
# if AX__SAL
#  define AX_IN __in
# else
#  define AX_IN
# endif
#endif

/* "Out" annotation */
#ifndef AX_OUT
# if AX__SAL
#  define AX_OUT __out
# else
#  define AX_OUT
# endif
#endif

/* "In_z" annotation, declares that the input must be NUL-terminated */
#ifndef AX_IN_Z
# if AX__SAL
#  define AX_IN_Z _In_z_
# else
#  define AX_IN_Z
# endif
#endif

/* "Out_z" annotation, declares that the output will be NUL-terminated */
#ifndef AX_OUT_Z
# if AX__SAL
#  define AX_OUT_Z(MaxChars_) _Out_writes_z_(MaxChars_)
# else
#  define AX_OUT_Z(MaxChars_)
# endif
#endif

/* "Update_z" annotation, declares that the input will be updated and NUL-terminated */
#ifndef AX_INOUT_Z
# if AX__SAL
#  define AX_INOUT_Z(MaxChars_) _Inout_updates_z_(MaxChars_)
# else
#  define AX_INOUT_Z(MaxChars_)
# endif
#endif

/* Declare that the given parameter will only be read */
#ifndef AX_READ_ONLY
# if AX__SAL
#  define AX_READ_ONLY __readonly //[Pre(Access=Read)]
# else
#  define AX_READ_ONLY
# endif
#endif

/* Declare that the given parameter will only be written to */
#ifndef AX_WRITE_ONLY
# if AX__SAL
#  define AX_WRITE_ONLY _Pre1_impl_(__writeaccess_impl) //[Pre(Access=Write)]
# else
#  define AX_WRITE_ONLY
# endif
#endif

/* Declare that the given pointer must be non-NULL (and valid) */
#ifndef AX_NOT_NULL
# if AX__SAL
#  define AX_NOT_NULL __notnull //[Pre(Null=No,Valid=Yes)]
# else
#  define AX_NOT_NULL
# endif
#endif

/* Declare that the return value of a function is always not null */
#ifndef AX_RETURN_NOT_NULL
# if AX__SAL
#  define AX_RETURN_NOT_NULL _Ret_notnull_
# else
#  define AX_RETURN_NOT_NULL
# endif
#endif

/* Declare that the return value of a function is always null */
#ifndef AX_RETURN_NULL
# if AX__SAL
#  define AX_RETURN_NULL _Ret_null_
# else
#  define AX_RETURN_NULL
# endif
#endif

/* Declare that a caller to a function must check the return value from that function */
#ifndef AX_CHECK_RETURN
# if AX__SAL
#  define AX_CHECK_RETURN _Check_return_
# else
#  define AX_CHECK_RETURN
# endif
#endif

#endif
