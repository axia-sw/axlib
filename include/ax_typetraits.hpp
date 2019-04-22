/*

	ax_typetraits - public domain
	Last update: 2014-12-10 Aaron Miller


	DEPENDENCIES
	
	- ax_platform -- for compiler defines
	- ax_types -- for TSizedInt


	LICENSE

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_TYPETRAITS_HPP_
#define INCGUARD_AX_TYPETRAITS_HPP_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

#include "ax_platform.h"
#include "ax_types.h"

#if !AX_TYPES_CXX_ENABLED
# error ax_typetraits only works in C++
#endif

namespace ax
{

	/// Declare an integral constant
	template< typename TElement, TElement tValue >
	struct TIntConst
	{
#if AX_CXX_CONSTEXPR_ENABLED
		static constexpr TElement	value = tValue;
#else
		static const TElement		value = tValue;
#endif

		typedef TElement			value_type;
		typedef TIntConst< TElement, tValue > type;
#if AX_CXX_CONSTEXPR_ENABLED
		constexpr operator value_type() const { return value; }
#endif
	};

	/// Classification of TIntConst for TTrue
	typedef TIntConst< bool, true > TTrue;
	/// Classification of TIntConst for TFalse
	typedef TIntConst< bool, false > TFalse;

	/// Determine whether a given type is an integer
	template< typename TElement > struct TIsInt: public TFalse {};
	/// Specialization of TIsInt for uint8
	template<> struct TIsInt< uint8 >: public TTrue {};
	/// Specialization of TIsInt for uint16
	template<> struct TIsInt< uint16 >: public TTrue {};
	/// Specialization of TIsInt for uint32
	template<> struct TIsInt< uint32 >: public TTrue {};
	/// Specialization of TIsInt for uint64
	template<> struct TIsInt< uint64 >: public TTrue {};
	/// Specialization of TIsInt for int8
	template<> struct TIsInt< int8 >: public TTrue {};
	/// Specialization of TIsInt for int16
	template<> struct TIsInt< int16 >: public TTrue {};
	/// Specialization of TIsInt for int32
	template<> struct TIsInt< int32 >: public TTrue {};
	/// Specialization of TIsInt for int64
	template<> struct TIsInt< int64 >: public TTrue {};
#if !defined(_LP64) || defined(__APPLE__)
	/// Specialization of TIsInt for signed long
	template<> struct TIsInt< signed long >: public TTrue {};
	/// Specialization of TIsInt for unsigned long
	template<> struct TIsInt< unsigned long >: public TTrue {};
#endif

	/// Determine whether a type is signed
	template< typename TInt >
	struct TIsSigned: public TIntConst< bool, TInt( -1 ) < TInt( 0 ) > {};
	/// Determine whether a type is unsigned
	template< typename TInt >
	struct TIsUnsigned: public TIntConst< bool, TInt( 0 ) < TInt( -1 ) > {};

	/// Retrieve the limits of a given integer type
	template< typename TInt >
	struct TIntLimits
	{
#if AX_CXX_N1720
		static_assert( TIsInt< TInt >::value == true, "Non-integer type passed to TIntLimits<>" );
#endif

		static const uint32			kNumBits			= sizeof( TInt )*8;
		static const uint32			kLastBit			= sizeof( TInt )*8 - 1;
		static const TInt			kMax				= ( TIsUnsigned< TInt >::value ) ? ~TInt( 0 ) : TInt( ( 1ULL<<( sizeof( TInt )*8 - 1 ) ) - 1 );
		static const TInt			kMin				= ( TIsUnsigned< TInt >::value ) ?        0   : TInt(   1ULL<<( sizeof( TInt )*8 - 1 ) );
	};

	static const int8				kInt8Min			= TIntLimits< int8 >::kMin;
	static const int8				kInt8Max			= TIntLimits< int8 >::kMax;
	static const int16				kInt16Min			= TIntLimits< int16 >::kMin;
	static const int16				kInt16Max			= TIntLimits< int16 >::kMax;
	static const int32				kInt32Min			= TIntLimits< int32 >::kMin;
	static const int32				kInt32Max			= TIntLimits< int32 >::kMax;
	static const int64				kInt64Min			= TIntLimits< int64 >::kMin;
	static const int64				kInt64Max			= TIntLimits< int64 >::kMax;
	static const intptr				kIntPtrMin			= TIntLimits< intptr >::kMin;
	static const intptr				kIntPtrMax			= TIntLimits< intptr >::kMax;
	static const intcpu				kIntCpuMin			= TIntLimits< intcpu >::kMin;
	static const intcpu				kIntCpuMax			= TIntLimits< intcpu >::kMax;

	static const uint8				kUint8Min			= TIntLimits< uint8 >::kMin;
	static const uint8				kUint8Max			= TIntLimits< uint8 >::kMax;
	static const uint16				kUint16Min			= TIntLimits< uint16 >::kMin;
	static const uint16				kUint16Max			= TIntLimits< uint16 >::kMax;
	static const uint32				kUint32Min			= TIntLimits< uint32 >::kMin;
	static const uint32				kUint32Max			= TIntLimits< uint32 >::kMax;
	static const uint64				kUint64Min			= TIntLimits< uint64 >::kMin;
	static const uint64				kUint64Max			= TIntLimits< uint64 >::kMax;
	static const uintptr			kUintPtrMin			= TIntLimits< uintptr >::kMin;
	static const uintptr			kUintPtrMax			= TIntLimits< uintptr >::kMax;
	static const uintcpu			kUintCpuMin			= TIntLimits< uintcpu >::kMin;
	static const uintcpu			kUintCpuMax			= TIntLimits< uintcpu >::kMax;

#if AX_CXX_N2118
	/// Determine whether a type is an l-value reference
	template< typename TElement > struct TIsLValueRef: public TFalse {};
	/// Specialization of is_lvalue_reference for l-value references
	template< typename TElement > struct TIsLValueRef< TElement & >: public TTrue {};

	/// Determine whether a type is an r-value reference
	template< typename TElement > struct TIsRValueRef: public TFalse {};
	/// Specialization of is_rvalue_reference for r-value references
	template< typename TElement > struct TIsRValueRef< TElement && >: public TTrue {};
#endif

#ifndef AX__HAS_TRIVIAL_CONSTRUCTOR_DEF
# if AX_CXX_TYPE_TRAITS_ENABLED
#  define AX__HAS_TRIVIAL_CONSTRUCTOR_DEF( T_ ) __is_trivially_constructible( T_ )
# else
#  define AX__HAS_TRIVIAL_CONSTRUCTOR_DEF( T_ ) false
# endif
#endif
#ifndef AX__HAS_TRIVIAL_DESTRUCTOR_DEF
# if AX_CXX_TYPE_TRAITS_ENABLED
#  define AX__HAS_TRIVIAL_DESTRUCTOR_DEF( T_ ) __has_trivial_destructor( T_ )
# else
#  define AX__HAS_TRIVIAL_DESTRUCTOR_DEF( T_ ) false
# endif
#endif
#ifndef AX__HAS_TRIVIAL_RELOCATE_DEF
# if AX_CXX_TYPE_TRAITS_ENABLED
#  define AX__HAS_TRIVIAL_RELOCATE_DEF( T_ ) __is_trivial( T_ ) /*FIXME:be more intelligent*/
# else
#  define AX__HAS_TRIVIAL_RELOCATE_DEF( T_ ) false
# endif
#endif

	/// Determine whether a type has a trivial-constructor
	template< typename TObject >
	struct THasTrivialConstructor: public TIntConst< bool, AX__HAS_TRIVIAL_CONSTRUCTOR_DEF( TObject ) > {};
	/// Determine whether a type has a trivial-destructor
	template< typename TObject >
	struct THasTrivialDestructor: public TIntConst< bool, AX__HAS_TRIVIAL_DESTRUCTOR_DEF( TObject ) > {};
	/// Determine whether a type can be relocated trivially
	template< typename TObject >
	struct THasTrivialRelocate: public TIntConst< bool, AX__HAS_TRIVIAL_RELOCATE_DEF( TObject ) > {};

#define AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( T_ )\
	template<> struct THasTrivialConstructor< T_ >: public TTrue {}
#define AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( T_ )\
	template<> struct THasTrivialDestructor< T_ >: public TTrue {}
#define AX_DECLARE_HAS_TRIVIAL_RELOCATE( T_ )\
	template<> struct THasTrivialRelocate< T_ >: public TTrue {}

#define AX_HAS_TRIVIAL_CONSTRUCTOR(T_) ax::THasTrivialConstructor<T_>::value
#define AX_HAS_TRIVIAL_DESTRUCTOR(T_)  ax::THasTrivialDestructor<T_>::value
#define AX_HAS_TRIVIAL_RELOCATE(T_)    ax::THasTrivialRelocate<T_>::value

	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( int8 );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( int16 );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( int32 );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( int64 );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( uint8 );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( uint16 );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( uint32 );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( uint64 );
#ifndef _LP64
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( signed long );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( unsigned long );
#endif
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( float );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( double );
	AX_DECLARE_HAS_TRIVIAL_CONSTRUCTOR( long double );

	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( int8 );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( int16 );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( int32 );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( int64 );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( uint8 );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( uint16 );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( uint32 );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( uint64 );
#ifndef _LP64
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( signed long );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( unsigned long );
#endif
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( float );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( double );
	AX_DECLARE_HAS_TRIVIAL_DESTRUCTOR( long double );

	AX_DECLARE_HAS_TRIVIAL_RELOCATE( int8 );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( int16 );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( int32 );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( int64 );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( uint8 );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( uint16 );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( uint32 );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( uint64 );
#ifndef _LP64
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( signed long );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( unsigned long );
#endif
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( float );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( double );
	AX_DECLARE_HAS_TRIVIAL_RELOCATE( long double );


	namespace detail
	{
		template< typename TBase, typename TDerived >
		struct TIsBaseOfHelper
		{
		private:
			typedef char Yes[ 1 ];
			typedef char No[ 2 ];

			static Yes &test( TBase * );
			static No  &test( ... );

		public:
#if AX_CXX_CONSTEXPR_ENABLED
			static constexpr bool value = sizeof( test( ( TBase * )0 ) ) == sizeof( Yes );
#else
			static const     bool value = sizeof( test( ( TBase * )0 ) ) == sizeof( Yes );
#endif
		};
	}

	/// Determine whether `TBase` is a base type of `TDerived`
	template< typename TBase, typename TDerived >
	struct TIsBaseOf: public TIntConst< bool, detail::TIsBaseOfHelper< TBase, TDerived >::value >
	{
	};

	/// \brief Removes the const-qualifier from a type if it has one.
	///
	/// TRemoveConst< const int >::type would be 'int' without the 'const'. This
	/// is useful for template programming.
	template< typename TElement >
	struct TRemoveConst
	{
		typedef TElement type;
	};
	/// Specialization of TRemoveConst for <tt>const TElement</tt>.
	template< typename TElement >
	struct TRemoveConst< const TElement >
	{
		typedef TElement type;
	};

	/// \brief Removes the volatile-qualifier from a type if it has one.
	///
	/// TRemoveVolatile< volatile int >::type would be 'int' without the
	/// 'volatile'. This is useful for template programming.
	template< typename TElement >
	struct TRemoveVolatile { typedef TElement type; };
	/// Specialization of TRemoveVolatile for <tt>volatile TElement</tt>.
	template< typename TElement >
	struct TRemoveVolatile< volatile TElement > { typedef TElement type; };
	/// Remove the const and volatile qualifiers from a type if it has any.
	template< typename TElement > struct TRemoveCV { typedef TElement type; };
	/// Specialization of TRemoveCV for const types
	template< typename TElement > struct TRemoveCV< const TElement > { typedef TElement type; };
	/// Specialization of TRemoveCV for volatile types
	template< typename TElement > struct TRemoveCV< volatile TElement > { typedef TElement type; };
	/// Specialization of TRemoveCV for const and volatile types
	template< typename TElement >
	struct TRemoveCV< const volatile TElement > { typedef TElement type; };
	/// Remove a reference from a type
	template< typename TElement > struct TRemoveRef { typedef TElement type; };
	/// Specialization of TRemoveRef for l-value references
	template< typename TElement > struct TRemoveRef< TElement & > { typedef TElement type; };
#if AX_CXX_RVALUE_REFS_ENABLED
	/// Specialization of TRemoveRef for r-value references
	template< typename TElement > struct TRemoveRef< TElement && > { typedef TElement type; };
#endif
	/// Remove a pointer from a type
	template< typename TElement > struct TRemovePtr { typedef TElement type; };
	/// Specialization of TRemovePtr for pointers
	template< typename TElement > struct TRemovePtr< TElement * > { typedef TElement type; };

	/// Adds a const-qualifier to a type if it doesn't already have one.
	template< typename TElement >
	struct TAddConst
	{
		typedef const typename TRemoveConst< TElement >::type type;
	};
	/// Adds a volatile-qualifier to a type if it doesn't already have one.
	template< typename TElement >
	struct TAddVolatile
	{
		typedef volatile typename TRemoveVolatile< TElement >::type type;
	};
	/// Adds a const and volatile qualifier to a type if it doesn't have one
	template< typename TElement >
	struct TAddCV
	{
		typedef const volatile typename TRemoveCV< TElement >::type type;
	};
	/// Add a reference to a type if it doesn't already have one
	template< typename TElement >
	struct TAddRef
	{
		typedef typename TRemoveRef< TElement >::type &type;
	};
	/// Specialization of TAddRef for void
	template<> struct TAddRef< void > { typedef void type; };
	/// Specialization of TAddRef for const-qualified void
	template<> struct TAddRef< const void > { typedef const void type; };
	/// Specialization of TAddRef for volatile-qualified void
	template<>
	struct TAddRef< volatile void >
	{
		typedef volatile void type;
	};
	/// Specialization of TAddRef for const-volatile-qualified void
	template<>
	struct TAddRef< const volatile void >
	{
		typedef const volatile void type;
	};
	/// Add a l-value reference to a type if it doesn't already have one
	template< typename TElement >
	struct TAddLValueRef: public TAddRef< TElement > {};

#if AX_CXX_RVALUE_REFS_ENABLED
	/// Add a r-value reference to a type if it doesn't already have one
	template< typename TElement >
	struct TAddRValueRef
	{
		typedef typename TRemoveRef< TElement >::type &&type;
	};
	/// Specialization of TAddRValueRef for void
	template<>
	struct TAddRValueRef< void > { typedef void type; };
	/// Specialization of TAddRValueRef for const-void
	template<>
	struct TAddRValueRef< const void > { typedef const void type; };
	/// Specialization of TAddRValueRef for volatile-void
	template<>
	struct TAddRValueRef< volatile void >
	{
		typedef volatile void type;
	};
	/// Specialization of TAddRValueRef for const-volatile-void
	template<>
	struct TAddRValueRef< const volatile void >
	{
		typedef const volatile void type;
	};
#endif

	/// Add a pointer to a type if it doesn't already have one
	template< typename TElement >
	struct TAddPtr
	{
		typedef typename TRemovePtr< TElement >::type *type;
	};

	/// Copy the const and volatile qualifiers of a source type to a destination
	/// type
	template< typename TDst, typename TSrc >
	struct TCopyCV
	{
		typedef TDst type;
	};
	/// Specialization of TCopyCV for const source
	template< typename TDst, typename TSrc >
	struct TCopyCV< TDst, const TSrc >
	{
		typedef typename TAddConst< TDst >::type type;
	};
	/// Specialization of TCopyCV for volatile source
	template< typename TDst, typename TSrc >
	struct TCopyCV< TDst, volatile TSrc >
	{
		typedef typename TAddVolatile< TDst >::type type;
	};
	/// Specialization of TCopyCV for const and volatile source
	template< typename TDst, typename TSrc >
	struct TCopyCV< TDst, const volatile TSrc >
	{
		typedef typename TAddCV< TDst >::type type;
	};

#if AX_CXX_RVALUE_REFS_ENABLED
	/// Get an r-value reference
	template< class TElement >
	inline TElement &&forward( typename TRemoveRef< TElement >::type &x ) AX_NOTHROW
	{
		return static_cast< TElement && >( x );
	}
	/// Get an r-value reference (from another r-value reference)
	template< class TElement >
	inline TElement &&forward( typename TRemoveRef< TElement >::type &&x ) AX_NOTHROW
	{
		static_assert( !TIsLValueRef< TElement >::value, "Unable to forward" );
		return static_cast< TElement && >( x );
	}

	/// Helper function to bind to move functions rather than copy functions
	template< class TElement >
	inline typename TRemoveRef< TElement >::type &&move( TElement &&x ) AX_NOTHROW
	{
		return static_cast< typename TRemoveRef< TElement >::type && >( x );
	}
#endif

	/// Mark a type as signed
	template< typename TInt >
	struct TMakeSigned
	{
#if AX_HAS_STATIC_ASSERT
		static_assert( TIsInt< TInt >::value, "Integral type required" );
#endif

		typedef
			typename TCopyCV<
				typename TSizedInt< sizeof( TInt ) >::sint,
				TInt
			>::type type;
	};
	/// Mark a type as unsigned
	template< typename TInt >
	struct TMakeUnsigned
	{
#if AX_HAS_STATIC_ASSERT
		static_assert( TIsInt< TInt >::value, "Integral type required" );
#endif

		typedef
			typename TCopyCV<
				typename TSizedInt< sizeof( TInt ) >::uint,
				TInt
			>::type type;
	};

	/// Determine whether an integer can be down-casted without a loss in
	/// precision.
	template< typename TSrcInt, typename TDstInt >
	inline bool canDowncast( const TSrcInt &src ) AX_NOTHROW
	{
		typedef typename TSizedInt< sizeof( TSrcInt ) >::uint src_uint;
		typedef typename TSizedInt< sizeof( TDstInt ) >::uint dst_uint;

		if( sizeof( TDstInt ) >= sizeof( TSrcInt ) ) {
			return true;
		}

		if( src_uint( src ) <= src_uint( dst_uint( -1 ) ) ) {
			return true;
		}

		return false;
	}
	/// Perform a Downcast from one type to another, warning on precision loss
	template< typename TDstInt, typename TSrcInt >
	inline TDstInt downcast( const TSrcInt &src )
	{
		AX_ASSERT_MSG( ( canDowncast< TSrcInt, TDstInt >( src ) ), "Cannot Downcast without a loss in precision" );
		return static_cast< TDstInt >( src );
	}
	/// Cast from a virtual base class to a derived class
	template< typename TDst, typename TSrc >
	inline TDst *virtualCast( TSrc *x )
	{
#if AX_HAS_STATIC_ASSERT
		// We are making an important assumption here...
		static_assert( TIsBaseOf< TSrc, TDst >::value, "`TSrc` must be the base type of `TDst`" );
#endif

		// Need to know how big TDst is on its own (if it didn't inherit from TSrc)
		static const size_t diffsize = sizeof( TDst ) - sizeof( TSrc );

		// The dst address is the src address minus the dst's relative size
		const size_t srcaddr = size_t( x );
		const size_t dstaddr = srcaddr - diffsize;

		// We now have the casted address
		return reinterpret_cast< TDst * >( dstaddr );
	}

	/// Retrieve the size of an array
	template< typename TElement, size_t tSize >
	AX_CONSTEXPR_INLINE size_t arraySize( const TElement( & )[ tSize ] )
	{
		return tSize;
	}
	/// Retrieve the total size of a two-dimensional array
	template< typename TElement, size_t tSize1, size_t tSize2 >
	AX_CONSTEXPR_INLINE size_t arraySize( const TElement( & )[ tSize1 ][ tSize2 ] )
	{
		return tSize1*tSize2;
	}
	/// Retrieve the total size of a three-dimensional array
	template< typename TElement, size_t tSize1, size_t tSize2, size_t tSize3 >
	AX_CONSTEXPR_INLINE size_t arraySize( const TElement( & )[ tSize1 ][ tSize2 ][ tSize3 ] )
	{
		return tSize1*tSize2*tSize3;
	}
	/// Retrieve the total size of a four-dimensional array
	template< typename TElement, size_t tSize1, size_t tSize2, size_t tSize3, size_t tSize4 >
	AX_CONSTEXPR_INLINE size_t arraySize( const TElement( & )[ tSize1 ][ tSize2 ][ tSize3 ][ tSize4 ] )
	{
		return tSize1*tSize2*tSize3*tSize4;
	}

	/// Evaluate an expression and select a type depending on the result
	template< bool tExpression, typename TExprTrue, typename TExprFalse >
	struct TConditional
	{
		typedef TExprFalse type;
	};
	/// Specialization of TConditional<> for a true expression
	template< typename TExprTrue, typename TExprFalse >
	struct TConditional< true, TExprTrue, TExprFalse >
	{
		typedef TExprTrue type;
	};

	/// \brief Select an index type based on a buffer size
	template< uint64 tBufferSize >
	struct TSelectIndex
	{
		typedef
			typename TConditional
			<
				( tBufferSize > 1ULL<<32 ),
				uint64,
				typename TConditional
				<
					( tBufferSize > 1ULL<<16 ),
					uint32,
					typename TConditional
					<
						( tBufferSize > 1ULL<<8 ),
						uint16,
						uint8
					>::type
				>::type
			>::type
			type;
	};

#define AX_TEST_IDX__( Size, Type )\
	( sizeof( TSelectIndex< Size >::type ) == sizeof( Type ) )

#if AX_HAS_STATIC_ASSERT
	static_assert( AX_TEST_IDX__( 0x100UL, uint8 ), "Bad selection (uint8)" );
	static_assert( AX_TEST_IDX__( 0x101UL, uint16 ), "Bad selection (uint16,1)" );
	static_assert( AX_TEST_IDX__( 0x10000UL, uint16 ), "Bad selection (uint16)" );
	static_assert( AX_TEST_IDX__( 0x10001UL, uint32 ), "Bad selection (uint32)" );
#endif

}


#endif
