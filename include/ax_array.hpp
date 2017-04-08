/*

	ax_array - public domain
	last update: 2015-10-01 Aaron Miller


	This library provides dynamic array support for C++.


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

/*! \file  ax_array.hpp
 *  \brief Provides dynamic array support for C++.
 *
 *  See `ax::TArr` and `ax::TMutArr` for more details.
 */

#ifndef INCGUARD_AX_ARRAY_HPP_
#define INCGUARD_AX_ARRAY_HPP_

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
# if __has_include( "ax_typetraits.hpp" )
#  include "ax_typetraits.hpp"
# endif
# if __has_include( "ax_assert.h" )
#  include "ax_assert.h"
# endif
#endif

/*! \typedef axarr_size_t
 *  \brief   Unsigned integer that will be used for array indexes and counts.
 */

/*! \typedef axarr_ptrdiff_t
 *  \brief   Signed integer equivalent of `axarr_size_t`.
 */

#ifdef AX_TYPES_DEFINED
typedef ax_size_t    axarr_size_t;
typedef ax_ptrdiff_t axarr_ptrdiff_t;
#else
# include <stddef.h>
typedef size_t    axarr_size_t;
typedef ptrdiff_t axarr_ptrdiff_t;
#endif

/*! \def   axarr_alloc
 *  \brief Optional user-supplied macro for allocating n-bytes of memory for
 *         arrays.
 *  \note  If this is defined then `axarr_free` must also be defined.
 */

/*! \def   axarr_free
 *  \brief Optional user-supplied macro for deallocating a pointer previously
 *         allocated by `axarr_alloc`.
 *  \note  This must also be defined if `axarr_alloc` has been defined.
 */

#ifndef axarr_alloc
# include <stdlib.h>
# define axarr_alloc(N_)            (malloc((N_)))
# define axarr_free(P_)             (free((P_)))
#endif

#ifndef AXARR_ASSERT
# ifdef AX_ASSERT
#  define AXARR_ASSERT              AX_ASSERT
# elif defined( assert )
#  define AXARR_ASSERT              assert
# else
#  define AXARR_ASSERT(Expr_)       ((void)0)
# endif
#endif

/*! \def     axarr_cxx_error
 *  \brief   Optional user-supplied macro for handling errors.
 *  \details You can replace this with, e.g., `throw Error(Msg_)` or
 *           `assert(0 && Msg_)`. Otherwise errors in initialization or
 *           assignment of C++ classes might go ignored.
 */
#ifndef axarr_cxx_error
# ifdef AX_EXPECT_MSG
#  define axarr_cxx_error(Msg_)     AX_EXPECT_MSG( false, Msg_ )
# else
#  define axarr_cxx_error(Msg_)     ((void)0)
# endif
#endif
/*! \def     AXARR_LOCMSG(Msg_)
 *  \brief   Optional user-supplied macro for localizing messages.
 *  \details If undefined, this defaults to `AX_LOCMSG` if that is defined, or
 *           just `Msg_` otherwise. By defining this, you can control how
 *           messages within `ax_array` are localized.
 */
#ifndef AXARR_LOCMSG
# ifdef AX_LOCMSG
#  define AXARR_LOCMSG(Msg_)        AX_LOCMSG(Msg_)
# else
#  define AXARR_LOCMSG(Msg_)        Msg_
# endif
#endif
/*! \def     AXARR_MSG_FAILEDINIT
 *  \brief   Optional user-supplied localized message string for "Failed to
 *           initialize array."
 *  \details Defaults to `AXARR_LOCMSG("Failed to initialize array.")`.
 */
#ifndef AXARR_MSG_FAILEDINIT
# define AXARR_MSG_FAILEDINIT       AXARR_LOCMSG("Failed to initialize array.")
#endif
/*! \def     AXARR_MSG_FAILEDCOPY
 *  \brief   Optional user-supplied localized message string for "Failed to
 *           assign array."
 *  \details Defaults to `AXARR_LOCMSG("Failed to assign array.")`.
 */
#ifndef AXARR_MSG_FAILEDCOPY
# define AXARR_MSG_FAILEDCOPY       AXARR_LOCMSG("Failed to assign to array.")
#endif
/*! \def     AXARR_MSG_FAILEDADD
 *  \brief   Optional user-supplied localized message string for "Failed to
 *           append array."
 *  \details Defaults to `AXARR_LOCMSG("Failed to append array.")`.
 */
#ifndef AXARR_MSG_FAILEDADD
# define AXARR_MSG_FAILEDADD        AXARR_LOCMSG("Failed to append to array.")
#endif

/*! \def     AXARR_CXX11_MOVE_ENABLED
 *  \brief   Optional user-supplied macro for enabling/disabling C++11 move
 *           operations.
 *  \details If undefined, then this checks for an `AX_CXX_RVALUE_REFS_ENABLED`
 *           definition. If that is also undefined, then an appropriate version
 *           of MSVC++, GCC, or Clang are checked. If support is found, this
 *           will be defined as `1`; otherwise there's no support and this is
 *           defined as `0`.
 */
#ifndef AXARR_CXX11_MOVE_ENABLED
# ifdef AX_CXX_RVALUE_REFS_ENABLED
#  define AXARR_CXX11_MOVE_ENABLED AX_CXX_RVALUE_REFS_ENABLED
# elif defined( _MSC_VER ) && _MSC_VER >= 1600
#  define AXARR_CXX11_MOVE_ENABLED 1
# elif defined( __GNUC__ ) && __GNUC__ >= 4
#  define AXARR_CXX11_MOVE_ENABLED 1
# elif defined( __clang__ )
#  define AXARR_CXX11_MOVE_ENABLED 1
# else
#  define AXARR_CXX11_MOVE_ENABLED 0
# endif
#endif

#ifndef AX_CONSTRUCT
# define AX_CONSTRUCT(X_)\
	::new(reinterpret_cast<void*>(&(X_)),::ax::detail::SPlcmntNw())
namespace ax { namespace detail { struct SPlcmntNw {}; } }
inline void *operator new( axarr_size_t, void *p, ax::detail::SPlcmntNw )
{
	return p;
}
inline void operator delete( void *, void *, ax::detail::SPlcmntNw )
{
}
#endif

#ifndef AXARR_HAS_TRIVIAL_CONSTRUCTOR
# ifdef AX_HAS_TRIVIAL_CONSTRUCTOR
#  define AXARR_HAS_TRIVIAL_CONSTRUCTOR(T_) AX_HAS_TRIVIAL_CONSTRUCTOR(T_)
# else
#  define AXARR_HAS_TRIVIAL_CONSTRUCTOR(T_) false
# endif
#endif
#ifndef AXARR_HAS_TRIVIAL_DESTRUCTOR
# ifdef AX_HAS_TRIVIAL_DESTRUCTOR
#  define AXARR_HAS_TRIVIAL_DESTRUCTOR(T_) AX_HAS_TRIVIAL_DESTRUCTOR(T_)
# else
#  define AXARR_HAS_TRIVIAL_DESTRUCTOR(T_) false
# endif
#endif
#ifndef AXARR_HAS_TRIVIAL_RELOCATE
# ifdef AX_HAS_TRIVIAL_RELOCATE
#  define AXARR_HAS_TRIVIAL_RELOCATE(T_) AX_HAS_TRIVIAL_RELOCATE(T_)
# else
#  define AXARR_HAS_TRIVIAL_RELOCATE(T_) false
# endif
#endif

#ifndef AXARR_MEMSET
# include <string.h>
# define AXARR_MEMSET(Dst_,Val_,Num_) memset((void*)(Dst_),(Val_),(Num_))
# define AXARR_MEMCPY(Dst_,Src_,Num_) memcpy((void*)(Dst_),(const void*)(Src_),(Num_))
#endif
#ifndef AXARR_MEMCPY
# error AXARR_MEMCPY needs to be defined if AXARR_MEMSET is also defined
#endif

namespace ax
{

	template< typename TElement >
	class TArr;

	template< typename TElement, typename TAllocator >
	class TMutArr;

	template< typename TElement, typename TAllocator, axarr_size_t tGranularity >
	class TBlockArr;

	namespace detail
	{

		namespace check
		{

			struct SFailed { char x[16]; };

			template<typename T>
			SFailed operator!( const T & ) { return SFailed(); }

			template<typename T>
			struct THasOperatorNot
			{
				static const bool value = (sizeof((!*(T*)0))) != sizeof(SFailed);
			};

		}

		template< typename TElement, bool bEnabled = check::THasOperatorNot<TElement>::value >
		struct TIsZero
		{
			static inline bool test( const TElement & )
			{
				return false;
			}
		};
		template< typename TElement >
		struct TIsZero< TElement, true >
		{
			static inline bool test( const TElement &x )
			{
				return !x;
			}
		};

	}

	namespace policy
	{

		template< typename TElement >
		struct ArrayIndexing
		{
			typedef axarr_size_t    SizeType;
			typedef axarr_ptrdiff_t DiffType;
		};

		template< typename TElement >
		struct ArrayAllocator
		{
			typedef axarr_size_t AllocSizeType;

			inline void *allocate( AllocSizeType cBytes )
			{
				return axarr_alloc( cBytes );
			}
			inline void deallocate( void *pBytes, AllocSizeType cBytes )
			{
				((void)cBytes);
				axarr_free( pBytes );
			}
		};

	}
	template< typename TElement >
	struct ArrayPolicies
	{
		typedef TElement                          Type;
		typedef TArr<Type>                        ArrayView;

		typedef policy::ArrayIndexing<Type>       Indexing;
		typedef policy::ArrayAllocator<Type>      Allocator;

		typedef typename Indexing::SizeType       SizeType;
		typedef typename Indexing::DiffType       DiffType;

		typedef typename Allocator::AllocSizeType AllocSizeType;
	};

	/*! \brief View of an array.
	 *
	 *  This is a lightweight class that only shows a portion of an array, but
	 *  does not own it. Consider using `TMutArr` if you'll need to modify an
	 *  array.
	 *
	 *  \warning You must be careful to ensure these views do not exceed the
	 *           lifetime of the arrays they're looking into. In general, this
	 *           should not be used as a member of a class, but rather as a
	 *           parameter to a function expecting an input array.
	 */
	template< typename TElement >
	class TArr
	{
	public:
		typedef ArrayPolicies<TElement>          Policies;
		typedef typename Policies::Type          Type;
		typedef typename Policies::ArrayView     ArrayView;
		typedef typename Policies::SizeType      SizeType;
		typedef typename Policies::DiffType      DiffType;
		typedef typename Policies::AllocSizeType AllocSizeType;
		typedef Type *                           Iterator;
		typedef const Type *                     ConstIterator;

		//! \brief Empty constructor.
		inline TArr()
		: m_pArr( ( Type * )0 )
		, m_cArr( 0 )
		{
		}
		//! \brief Constructs a view of the elements between `s` and `e`, but
		//! not `e` itself.
		//!
		//! \param s starting pointer of the array.
		//! \param e pointer to the just past the last accessible array element.
		inline TArr( const Type *s, const Type *e )
		: m_pArr( s )
		, m_cArr( ( SizeType )( e - s ) )
		{
		}
		//! \brief Constructs a view of `n`-elements starting at `p`.
		//!
		//! \param p pointer to the base of the array.
		//! \param n number of elements in the array.
		inline TArr( const Type *p, const SizeType n )
		: m_pArr( p )
		, m_cArr( n )
		{
		}
		//! \brief Copy constructor.
		//!
		//! \param arr array view we're copying from.
		//!
		//! \note  No elements are copied, just the address of the array and the
		//!        number of elements in it. This is a lightweight operation.
		inline TArr( const TArr &arr )
		: m_pArr( arr.m_pArr )
		, m_cArr( arr.m_cArr )
		{
		}
		//! \brief Constructs a view of a mutable array.
		//!
		//! \param arr mutable array we will be viewing.
		template< typename TAllocator >
		TArr( const TMutArr< TElement, TAllocator > &arr );

		//! \brief Constructs a view of a local/static array.
		//!
		//! \param arr array we will be viewing.
		template< SizeType tNumElements >
		TArr( const Type( &arr )[ tNumElements ] )
		: m_pArr( arr )
		, m_cArr( tNumElements )
		{
		}
		//! \brief Empty destructor.
		inline ~TArr()
		{
		}

		//! \brief Check whether this view is empty.
		//! \return `true` if this view has no elements.
		inline bool isEmpty() const { return m_cArr == 0; }
		//! \brief Check whether this view is not empty.
		//! \return `true` if this view has at least one element.
		inline bool isUsed() const { return m_cArr > 0; }
		//! \brief Retrieve the length of the array.
		//! \return number of elements in the array.
		inline SizeType len() const { return m_cArr; }
		//! \brief Retrieve the length of the array.
		//!
		//! Provided for compatibility with `ax::MutStr`, where `num()` shows
		//! the length of the string in bytes, including the `NUL` terminator.
		//!
		//! \return number of elements in the array.
		inline SizeType num() const { return m_cArr; }
		//! \brief Retrieve the capacity of the array.
		//! \return number of elements in the array.
		inline SizeType max() const { return m_cArr; }
		//! \brief Retrieve the number of bytes this object owns.
		//! \return number of bytes this instance owns.
		inline AllocSizeType memSize() const { return sizeof( *this ); }

		//! \brief Retrieve the beginning iterator for this array.
		//!
		//! Provided for C++11 range-based for-loop support.
		//!
		//! \return beginning iterator.
		inline ConstIterator begin() const { return m_pArr; }
		//! \brief Retrieve the ending iterator for this array.
		//!
		//! Provided for C++11 range-based for-loop support.
		//!
		//! \return ending iterator.
		inline ConstIterator end() const { return m_pArr + m_cArr; }

		//! \brief Retrieve the starting pointer of this array.
		//!
		//! This is the same as `begin()`.
		//!
		//! \return `const` starting pointer of the array.
		inline ConstIterator get() const { return m_pArr; }
		//! \brief Retrieve the ending pointer of this array.
		//!
		//! This is the same as `end()`.
		//!
		//! \return `const` ending pointer of the array.
		inline ConstIterator getEnd() const { return m_pArr + m_cArr; }

		//! \brief  Retrieve a pointer to a particular element within the array.
		//! \param  index zero-based index of the desired element.
		//! \return `const` pointer to the element, or `nullptr` if the index is
		//!         out of bounds.
		inline const Type *pointer( SizeType index = 0 ) const { return index >= m_cArr ? ( const Type * )0 : m_pArr + index; }
		//! \brief  Retrieve the index a given pointer would be at within the
		//!         array.
		//! \param  ptr pointer to an element within this array whose index we
		//!             wish to retrieve.
		//! \return index to the element on success, or `~SizeType(0)` on
		//!         failure.
		inline SizeType index( const Type *ptr ) const { return ptr >= m_pArr ? ( SizeType )( ptr - m_pArr )/sizeof( Type ) : ~SizeType(0); }

		//! \brief  Check whether the array is empty.
		//! \return `isEmpty()`
		inline bool operator!() const { return isEmpty(); }
		//! Check whether the array is not empty.
		//! \return `isUsed()`
		inline operator bool() const { return isUsed(); }

		//! \brief   Retrieve a reference to the element at the given index in
		//!          the array.
		//! \param   index zero-based index of the desired element.
		//! \return  `const` reference to the element.
		//! \warning If `index` is out of bounds, this will crash.
		inline const Type &at( SizeType index ) const { return *pointer( index ); }

		//! \brief   Retrieve a reference to the first element within the array.
		//! \return  `const` reference to the first element.
		//! \warning This will crash if the array is empty.
		inline const Type &first() const { return *pointer( 0 ); }
		//! \brief   Retrieve a reference to the last element within the array.
		//! \return  `const` reference to the last element.
		//! \warning This will crash if the array is empty.
		inline const Type &last() const { return *pointer( m_cArr - 1 ); }

		//! \brief  Reference an element in the array.
		//! \param  index zero-based index of the desired element.
		//! \return `const` reference to the element.
		//! \sa at
		inline const Type &operator[]( SizeType index ) const { return *pointer( index ); }

		//! \brief  Move the front of this array's view forward by one element.
		//!
		//! For example, let's suppose you have an array `{ 42, 23, 17, 99 }`.
		//! Then, let's say you did `skip(1)` on a view of that array. The view
		//! would then show `{ 23, 17, 99 }`, but the original array would
		//! remain unmodified.
		//!
		//! \param  n number of elements to skip.
		//! \return reference to this array view. (`*this`)
		inline TArr &skip( SizeType n = 1 ) { if( n < m_cArr ) { m_pArr += n; m_cArr -= n; } else { m_pArr = ( Type * )0; m_cArr = 0; } return *this; }
		//! \brief  Move the back of this array's view backward by one element.
		//!
		//! For example, let's suppose you have an array `{ 42, 23, 17, 99 }`.
		//! Then, let's say you did `drop(1)` on a view of that array. The view
		//! would then show `{ 42, 23, 17 }`, but the original array would
		//! remain unmodified.
		//!
		//! \param  n number of elements to drop.
		//! \return reference to this array view. (`*this`)
		inline TArr &drop( SizeType n = 1 ) { if( n < m_cArr ) { m_cArr -= n; } else { m_cArr = 0; } return *this; }

		//! \brief  Create another view from a slice of this view.
		//! \param  iLeft  index to the beginning of the view. Negative values
		//!                mean "from the end." A value of `-1` means "starting
		//!                from the last element."
		//! \param  iRight index to the end of the view. Negative values mean
		//!                "from the end." A value of `-1` means "ending at the
		//!                last element."
		//! \return A view of the given range.
		//! \sa mid
		inline TArr slice( DiffType iLeft, DiffType iRight ) const
		{
			const SizeType uBegin = mapIndex( iLeft );
			const SizeType uEnd   = mapIndex( iRight );

			if( uBegin == uEnd ) { return TArr< Type >(); }

			const SizeType s = uBegin < uEnd ? uBegin : uEnd;
			const SizeType e = uBegin < uEnd ? uEnd : uBegin;

			return TArr< Type >( m_pArr + s, m_pArr + e );
		}
		//! \brief  Create another view from a range of this view.
		//!
		//! This differs from `slice()` in that the size of the view is given
		//! explicitly as a quantity.
		//!
		//! \param  iBase  index to the beginning of the view. Negative values
		//!                mean "from the end." A value of `-1` means "starting
		//!                from the last element."
		//! \param  cItems number of elements from the base element in the new
		//!                view.
		//! \return A view of the given range.
		//! \sa slice
		inline TArr mid( DiffType iBase, SizeType cItems = 1 ) const
		{
			if( !cItems ) { return TArr< Type >(); }

			const SizeType uBegin = mapIndex( iBase );
			const SizeType uEnd = clipIndex( uBegin + cItems );

			return TArr< Type >( m_pArr + uBegin, m_pArr + uEnd );
		}

		//! \brief  Search for an element that matches the given element in this
		//!         array.
		//! \param  x     Element to compare against. The first element found
		//!               that compares equal to this will be returned.
		//! \param  pFrom Optional starting location to begin searching from.
		//!               `nullptr` (default) is treated the same as `begin()`.
		//! \return Pointer to the element if found, or `nullptr` if not.
		inline const Type *find( const Type &x, const Type *pFrom = ( const Type * )0 ) const
		{
			AXARR_ASSERT( ( !pFrom || pFrom >= m_pArr && pFrom <= m_pArr + m_cArr ) &&
				"`pFrom` points outside of this array" );

			const Type *const e = m_pArr + m_cArr;
			for( const Type *p = !pFrom ? m_pArr : pFrom; p != e; ++p ) {
				if( *p == x ) {
					return p;
				}
			}

			return ( const Type * )0;
		}
		//! \brief  Determine whether a given element exists within the array.
		//! \param  x Element to compare against. If any element in the array
		//!           compares equal to this, then `true` will be returned.
		//! \return `true` if an equivalent element was found; `false`
		//!         otherwise.
		inline bool contains( const Type &x ) const
		{
			return find( x ) != ( const Type * )0;
		}

	private:
		const Type *            m_pArr;
		SizeType                m_cArr;

		inline SizeType clipIndex( DiffType iIndex ) const
		{
			return iIndex < 0 ? 0 : ( ( SizeType )iIndex > m_cArr ? m_cArr : iIndex );
		}
		inline SizeType mapIndex( DiffType iIndex ) const
		{
			const DiffType iTransformed = iIndex < 0 ? m_cArr + 1 + iIndex : iIndex;
			const SizeType uClipped = iTransformed < 0 ? 0 : ( ( SizeType )iTransformed > m_cArr ? m_cArr : iTransformed );

			return uClipped;
		}
	};

	/*! \brief A mutable array.
	 *
	 *  Represents a modifiable array. Each instance owns the memory it
	 *  represents, as opposed to `TArr` which only references memory owned by
	 *  other arrays.
	 *
	 *  Rather than passing `TMutArr` into functions which will only be reading
	 *  from arrays, consider using `TArr` instead. Using `TArr` allows more
	 *  flexibility in that the array can come from sources besides just
	 *  `TMutArr` while being more lightweight as well.
	 *
	 *  \warning By itself, this class is not thread-safe. You will need to
	 *           manage synchronization manually. This is by design.
	 */
	template< typename TElement, typename TAllocator = typename ArrayPolicies<TElement>::Allocator >
	class TMutArr: private TAllocator
	{
	friend class TArr< TElement >;
	public:
		typedef ArrayPolicies<TElement>            Policies;
		typedef typename Policies::Type            Type;
		typedef typename Policies::ArrayView       ArrayView;
		typedef TMutArr<TElement, TAllocator>      DynamicArray;
		typedef typename Policies::SizeType        SizeType;
		typedef typename Policies::DiffType        DiffType;
		typedef typename TAllocator::AllocSizeType AllocSizeType;
		typedef Type *                             Iterator;
		typedef const Type *                       ConstIterator;

		//! \brief Default number of elements to round `resize()`/`reserve()`
		//!        operations up to.
		static const SizeType   kDefaultGranularity = sizeof( Type ) > 512 ? 1 : 512/sizeof( Type );

		//! \brief Empty constructor.
		//!
		//! The empty constructor does *not* allocate any memory.
		TMutArr();
		//! \brief Copy constructor.
		//!
		//! This will allocate memory if `arr` is not empty.
		//!
		//! \param arr Array to copy from.
		TMutArr( const TMutArr &arr );
		//! \brief Construct from an array view.
		//!
		//! This will allocate memory if `arr` is not empty.
		//!
		//! \param arr Array to copy from.
		TMutArr( const ArrayView &arr );
		//! \brief Construct from a range.
		//! \param cItems number of items held in the array.
		//! \param pItems pointer to the base of the items. Cannot be `nullptr`
		//!               if `cItems > 0`.
		TMutArr( SizeType cItems, const Type *pItems );
#if AXARR_CXX11_MOVE_ENABLED
		//! \brief Move constructor.
		//! \param x mutable array to move from.
		inline TMutArr( TMutArr &&x )
		: m_cArr( x.m_cArr )
		, m_cMax( x.m_cMax )
		, m_pArr( x.m_pArr )
		, m_cGranularity( x.m_cGranularity )
		{
			x.m_cArr = 0;
			x.m_cMax = 0;
			x.m_pArr = nullptr;
		}
#endif
		//! \brief Destructor.
		//!
		//! Invokes `purge()` to get rid of all memory used.
		~TMutArr();

		//! \brief Check whether this view is empty.
		//! \return `true` if this view has no elements.
		inline bool isEmpty() const { return m_cArr == 0; }
		//! \brief Check whether this view is not empty.
		//! \return `true` if this view has at least one element.
		inline bool isUsed() const { return m_cArr > 0; }
		//! \brief Retrieve the length of the array.
		//! \return number of elements in the array.
		inline SizeType len() const { return m_cArr; }
		//! \brief Retrieve the length of the array.
		//!
		//! Provided for compatibility with `ax::MutStr`, where `num()` shows
		//! the length of the string in bytes, including the `NUL` terminator.
		//!
		//! \return number of elements in the array.
		inline SizeType num() const { return m_cArr; }
		//! \brief Retrieve the capacity of the array.
		//! \return number of elements in the array.
		inline SizeType max() const { return m_cMax; }
		//! \brief Retrieve the number of bytes this object owns.
		//! \return number of bytes this instance owns.
		inline AllocSizeType memSize() const { return sizeof( *this ) + m_cMax*sizeof( Type ); }

		//! \brief Reset the array to 0 elements.
		//!
		//! This only invokes the destructors of the elements, but does not
		//! remove the memory used explicitly by this array. That is useful when
		//! the array needs to be rapidly filled and reset. (For example, when
		//! containing per-frame commands.)
		//!
		//! If you need to reset the memory usage of the array to its initial
		//! empty state, use `purge()`.
		void clear();
		//! \brief Reset the array to 0 elements, freeing all memory.
		//!
		//! Unlike `clear()`, this will also free the array itself rather than
		//! just destroying the elements and resetting the count.
		void purge();

		//! \brief  Resize the array so that it contains `size`-elements.
		//!
		//! If `size` is less than the current number of elements, then the
		//! outside elements will be destroyed. If `size` is greater than the
		//! current number of elements then the array will be resized to hold
		//! enough elements. If, however, `size` is also greater than the
		//! current capacity of the array (`max()`) and "no grow" is enabled
		//! (see `setNoGrow()`) then the resize operation will fail.
		//!
		//! \param  size  Number of elements to resize to.
		//! \param  pInit Optional pointer to the element to initialize new
		//!               elements to. If `nullptr` then new elements are left
		//!               uninitialized.
		//! \return `true` on success; `false` otherwise. If `false` is returned
		//!         then the array couldn't be resized, either because there was
		//!         not enough memory to do so, or because "no grow" is enabled.
		//!
		//! \warning Do not use this function unless you explicitly need to
		//!          allow for uninitialized elements. Keep in mind, though,
		//!          that this class assumes all elements are initialized when
		//!          working with them. As such, it's possible to invoke a
		//!          destructor on an element that has not been constructed.
		bool resize( SizeType size, const Type *pInit );
		//! \brief  Resize the array so that it contains `size`-elements.
		//!
		//! This is the same as `resize(SizeType, const Type *)`, with
		//! the exception that new elements are always initialized. (This is
		//! safest.)
		//!
		//! \param  size Number of elements to resize to.
		//! \param  x    Element to initialize to. The default is an empty
		//!              constructed equivalent of the type.
		//! \return `true` on success; `false` otherwise. If `false` is returned
		//!         then the array couldn't be resized, either because there was
		//!         not enough memory to do so, or because "no grow" is enabled.
		inline bool resize( SizeType size, const Type &x = Type() )
		{
			return resize( size, &x );
		}
		//! \brief  Reserves enough space in the array to hold `size`-elements.
		//!
		//! This respects the granularity and "no grow" (see `setNoGrow()`)
		//! settings. If you need to set the array to an exact size, without
		//! respecting those settings, use `setAllocated()`.
		//!
		//! If `size` is less than the current number of elements in the array,
		//! then the outside elements will be destroyed, as with `resize()`.
		//!
		//! The biggest difference between this and `resize()` is that this does
		//! not change the number of elements that `len()` and `num()` report
		//! (unless we're shrinking). It only ensures that the next call to
		//! `resize()` will succeed *if* this succeeds.
		//!
		//! \param  size Number of elements to reserve.
		//! \return `true` on success; `false` otherwise. If `false` is returned
		//!         then enough space could not be reserved, either because
		//!         there was not enough memory to do so, or because "no grow"
		//!         is enabled.
		bool reserve( SizeType size );
		//! \brief  Set the exact number of elements the array should hold.
		//!
		//! This does *not* respect granularity, nor the "no grow" (see
		//! `setNoGrow()`) settings. Other than that, this behaves the same as
		//! `reserve()` with the exception that this can also shrink the array.
		//!
		//! \note When shrinking the array, memory will need to be allocated
		//!       again to hold the location of the new array. As such, even if
		//!       trying to shrink an array there is still a possibility the
		//!       operation will fail.
		//!
		//! \param  size Number of elements the array shall hold.
		//! \return `true` on success; `false` if there was not enough memory to
		//!         complete the operation.
		//!
		//! \todo When the type of the array is trivial, a call to a `realloc()`
		//!       equivalent is possible. In such instances, the array can be
		//!       safely resized without the need to move elements from the old
		//!       array to the new array.
		bool setAllocated( SizeType size );

		//! \brief  Set the allocation granularity, in elements.
		//!
		//! The granularity is used when more memory is needed for the array. In
		//! such cases, the granularity is used as the number of elements to
		//! round up by.
		//!
		//! For example, if the array is at capacity with 16 elements and a
		//! granularity of 8, and an append operation comes in, the array will
		//! then have a capacity of 24 elements (16 + 8).
		//!
		//! In such a way, the granularity reduces the number of allocations
		//! when an array is being filled.
		//!
		//! The default granularity is equivalent to the number of elements that
		//! can fit within 512 bytes, or 1 if each element is greater than that.
		//! The default can be retrieved as a compile-time constant through the
		//! `kDefaultGranularity` member.
		//!
		//! \param  granularity Number of elements allocations should round up
		//!                     toward. The most significant bit must be clear.
		inline void setGranularity( SizeType granularity ) { AXARR_ASSERT( ( granularity & kGranBits ) == granularity ); m_cGranularity = granularity & kGranBits; }
		//! \brief Retrieve the allocation granularity, in elements.
		//!
		//! See `setGranularity()` for more details.
		//!
		//! \return Number of elements allocations should round up toward.
		inline SizeType getGranularity() const { return m_cGranularity & kGranBits; }

		//! \brief Enable or disable the "no grow" setting. (Default: `false`.)
		//!
		//! The "no grow" setting refers to whether attempts to increase the
		//! capacity of the array ("growing") are allowed. If enabled, growing
		//! the array will fail even if there is enough memory to do so.
		//! Otherwise, growing the array will only fail if there isn't enough
		//! memory to do so.
		//!
		//! This setting is disabled by default.
		//!
		//! \param bNoGrow Whether the array can grow (`false`), or not
		//!        (`true`).
		inline void setNoGrow( bool bNoGrow ) { if( bNoGrow ) { m_cGranularity |= kGranF_NoGrow; } else { m_cGranularity &= ~kGranF_NoGrow; } }
		//! \brief Enable the "no grow" setting.
		//!
		//! Disabled by default. See `setNoGrow()` for more details.
		inline void enableNoGrow() { m_cGranularity |= kGranF_NoGrow; }
		//! \brief Disable the "no grow" setting. (Default.)
		//!
		//! See `setNoGrow()` for more details.
		inline void disableNoGrow() { m_cGranularity &= ~kGranF_NoGrow; }
		//! \brief  Retrieve the "no grow" setting.
		//!
		//! See `setNoGrow()` for more details.
		//!
		//! \return Whether "no grow" is enabled (`true`) or disabled (`false`,
		//!         default.)
		inline bool isNoGrowEnabled() const { return ( m_cGranularity & kGranF_NoGrow ) == kGranF_NoGrow; }

		//! \brief  Set this to be another array.
		//!
		//! The existing elements of this array will be discarded if present.
		//! The new elements will be constructed in their place. This command
		//! will fail if there is not enough memory for the new array. If this
		//! does fail, the old array remains intact.
		//!
		//! \param  cItems Number of items contained in `pItems`.
		//! \param  pItems Pointer to the items array to assign. Must not be
		//!                `nullptr`.
		//! \return `true` on success; `false` otherwise.
		bool assign( SizeType cItems, const Type *pItems );
		//! \brief  Set this to be another array.
		//!
		//! The existing elements of this array will be discarded if present.
		//! The new elements will be constructed in their place. This command
		//! will fail if there is not enough memory for the new array. If this
		//! does fail, the old array remains intact.
		//!
		//! \param  x Single element of the new array.
		//! \return `true` on success; `false` otherwise.
		inline bool assign( const Type &x ) { return assign( 1, &x ); }
		//! \brief  Set this to be another array.
		//!
		//! The existing elements of this array will be discarded if present.
		//! The new elements will be constructed in their place. This command
		//! will fail if there is not enough memory for the new array. If this
		//! does fail, the old array remains intact.
		//!
		//! \param  arr Array to assign.
		//! \return `true` on success; `false` otherwise.
		template< SizeType tLen >
		inline bool assign( const Type( &arr )[ tLen ] ) { return assign( tLen, arr ); }
		//! \brief  Set this to be another array.
		//!
		//! The existing elements of this array will be discarded if present.
		//! The new elements will be constructed in their place. This command
		//! will fail if there is not enough memory for the new array. If this
		//! does fail, the old array remains intact.
		//!
		//! \param  arr    Mutable array to assign from.
		//! \param  first  Index of the first element in the mutable array.
		//! \param  cItems Number of items including and after `first` to
		//!                assign.
		//! \return `true` on success; `false` otherwise.
		inline bool assign( const TMutArr &arr, SizeType first = 0, DiffType cItems = -1 ) { return assign( cItems < 0 ? arr.num() + 1 + cItems : cItems, arr.pointer( first ) ); }
		//! \brief  Set this to be another array.
		//!
		//! The existing elements of this array will be discarded if present.
		//! The new elements will be constructed in their place. This command
		//! will fail if there is not enough memory for the new array. If this
		//! does fail, the old array remains intact.
		//!
		//! \param  arr Array view to assign from.
		//! \return `true` on success; `false` otherwise.
		inline bool assign( const ArrayView &arr ) { return assign( arr.num(), arr.get() ); }

		//! \brief  Append another array to the end of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  cItems Number of items to append.
		//! \param  pItems Pointer to the array of items to append.
		//! \return `true` on success; `false` otherwise.
		bool append( SizeType cItems, const Type *pItems );
		//! \brief  Append a single element to this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  x Element to append.
		//! \return `true` on success; `false` otherwise.
		inline bool append( const Type &x ) { return append( 1, &x ); }
		//! \brief  Append another array to the end of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr Array to append.
		//! \return `true` on success; `false` otherwise.
		template< SizeType tLen >
		inline bool append( const Type( &arr )[ tLen ] ) { return append( tLen, arr ); }
		//! \brief  Append another array to the end of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr    Mutable array to append.
		//! \param  first  Index of the first element within the array to append
		//!                from.
		//! \param  cItems Number of elements after `first` to append. If this
		//!                is negative, then this is the number of elements to
		//!                append from the end of `arr`.
		//! \return `true` on success; `false` otherwise.
		inline bool append( const TMutArr &arr, SizeType first = 0, DiffType cItems = -1 ) { return append( cItems < 0 ? arr.num() + 1 + cItems : cItems, arr.pointer( first ) ); }
		//! \brief  Append a default constructed element to the end of this
		//!         array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \return `true` on success; `false` otherwise.
		inline bool append() { return resize( m_cArr + 1 ); }
		//! \brief  Append another array to the end of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr Array view to append.
		//! \return `true` on success; `false` otherwise.
		inline bool append( const ArrayView &arr ) { return append( arr.num(), arr.get() ); }

		//! \brief  Insert another array to the beginning of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  cItems Number of elements in `pItems` to "prepend."
		//! \param  pItems Array to "prepend."
		//! \return `true` on success; `false` otherwise.
		bool prepend( SizeType cItems, const Type *pItems );
		//! \brief  Insert an element at the beginning of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  x Element to "prepend."
		//! \return `true` on success; `false` otherwise.
		inline bool prepend( const Type &x ) { return prepend( 1, &x ); }
		//! \brief  Insert another array to the beginning of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr Array to "prepend."
		//! \return `true` on success; `false` otherwise.
		template< SizeType tLen >
		inline bool prepend( const Type( &arr )[ tLen ] ) { return prepend( tLen, arr ); }
		//! \brief  Insert another array to the beginning of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr    Mutable array to "prepend."
		//! \param  first  Index of the first item in `arr` to "prepend."
		//! \param  cItems Number of items after `first` to "prepend."
		//! \return `true` on success; `false` otherwise.
		inline bool prepend( const TMutArr &arr, SizeType first = 0, DiffType cItems = -1 ) { return prepend( cItems < 0 ? arr.num() + 1 + cItems : cItems, arr.pointer( first ) ); }
		//! \brief  Insert another array to the beginning of this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr Array view to "prepend."
		//! \return `true` on success; `false` otherwise.
		inline bool prepend( const ArrayView &arr ) { return prepend( arr.num(), arr.get() ); }

		//! \brief  Insert another array into this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  before Pointer to the element within this array that will
		//!                have elements written just prior to it. (Note that
		//!                this pointer will no longer be valid if the operation
		//!                succeeds.)
		//! \return `true` on success; `false` on failure.
		bool insert( SizeType cItems, const Type *pItems, Type *before );
		//! \brief  Insert an element into this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  x      Element to insert into this array.
		//! \param  before Pointer to the element within this array that will
		//!                have elements written just prior to it. (Note that
		//!                this pointer will no longer be valid if the operation
		//!                succeeds.)
		//! \return `true` on success; `false` on failure.
		inline bool insert( const Type &x, Type *before ) { return insert( 1, &x, before ); }
		//! \brief  Insert another array into this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr    Array to insert into this.
		//! \param  before Pointer to the element within this array that will
		//!                have elements written just prior to it. (Note that
		//!                this pointer will no longer be valid if the operation
		//!                succeeds.)
		//! \return `true` on success; `false` on failure.
		template< SizeType tLen >
		inline bool insert( const Type( &arr )[ tLen ], Type *before ) { return insert( tLen, arr, before ); }
		//! \brief  Insert another array into this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr    Mutable array to insert into this.
		//! \param  before Pointer to the element within this array that will
		//!                have elements written just prior to it. (Note that
		//!                this pointer will no longer be valid if the operation
		//!                succeeds.)
		//! \param  first  Index of the first element in `arr` to insert.
		//! \param  cItems Number of elements from `first` in `arr` to insert.
		//! \return `true` on success; `false` on failure.
		inline bool insert( const TMutArr &arr, Type *before, SizeType first = 0, DiffType cItems = -1 ) { return insert( cItems < 0 ? arr.num() + 1 + cItems : cItems, arr.pointer( first ), before ); }
		//! \brief  Insert another array into this array.
		//!
		//! This command will fail if there is not enough memory to hold the new
		//! array. If this does fail, the old array remains intact. If you want
		//! to add to the array without its size increasing, you can enable the
		//! "no grow" setting. (See `setNoGrow()` for more details.)
		//!
		//! \param  arr    Array view to insert into this.
		//! \param  before Pointer to the element within this array that will
		//!                have elements written just prior to it. (Note that
		//!                this pointer will no longer be valid if the operation
		//!                succeeds.)
		//! \return `true` on success; `false` on failure.
		inline bool insert( const ArrayView &arr, Type *before ) { return insert( arr.num(), arr.get(), before ); }

		//! \brief  Remove a section of this array.
		//!
		//! This command will not fail as no allocations need to take place.
		//! The range of elements specified by `first` and `count` will be
		//! destructed, and the trailing elements will move down to take their
		//! place.
		//!
		//! For example, if you have an array `{ 37, 26, 17, 23, 42 }` and were
		//! to invoke `remove( 1, 3 )` on that array, you'd have `{ 37, 42 }`
		//! remaining.
		//!
		//! \param  first Index of the first element to remove.
		//! \param  count Number of elements to remove.
		void remove( SizeType first, SizeType count = 1 );
		//! \brief Remove the last element of this array, if present.
		inline void removeLast() { if( m_cArr > 0 ) { resize( m_cArr - 1 ); } }
		//! \brief Pop the last element off the array, returning a copy of it.
		//!
		//! This makes and returns a local copy of the last element of the
		//! array. After the local copy of the last element is made, that last
		//! element is removed from the array (with its destructor invoked).
		//!
		//! If the array is empty, a default constructed item is returned
		//! instead.
		//!
		//! \return Copy of the last element in the array prior to this call, or
		//!         if the array was empty, a default constructed element.
		Type popLast();

		//! \brief Retrieve a pointer to the beginning of this array.
		//!
		//! Provided for compatibility with C++11's range-based for-loops.
		//!
		//! \return Pointer to the first element in the array.
		inline Iterator begin() { return m_pArr; }
		//! \brief Retrieve a pointer to the end of this array.
		//!
		//! Provided for compatibility with C++11's range-based for-loops.
		//!
		//! \return Pointer to one past the last element in the array.
		inline Iterator end() { return m_pArr + m_cArr; }

		//! \copydoc begin()
		inline ConstIterator begin() const { return m_pArr; }
		//! \copydoc end()
		inline ConstIterator end() const { return m_pArr + m_cArr; }

		//! \brief  Retrieve a pointer to the given element in the array.
		//!
		//! \param  index Zero-based index of the element whose pointer within
		//!               the array is to be retrieved.
		//! \return Pointer to the element at `index`, or `nullptr` if `index`
		//!         is out of bounds.
		inline       Type *pointer( SizeType index = 0 )       { return index >= m_cArr ? ( Type * )0 : m_pArr + index; }
		//! \copydoc pointer(axarr_size_t)
		inline const Type *pointer( SizeType index = 0 ) const { return index < 0 ? ( const Type * )0 : m_pArr + index; }

		//! \brief  Retrieve the index a given pointer would be at within this
		//!         array.
		//!
		//! \param  ptr Pointer to an element within this array whose index
		//!             number is to be retrieved.
		//! \return Index of the element pointed to by `ptr`, or
		//!         `~axarr_size_t(0)` if no such element exists.
		inline SizeType index( const Type *ptr ) const { return ptr >= m_pArr && ptr < m_pArr + m_cArr ? ( SizeType )( ptr - m_pArr )/sizeof( Type ) : ~SizeType(0); }

		//! \brief  Check whether the array is empty.
		//! \return `isEmpty()`
		inline bool operator!() const { return isEmpty(); }
		//! \brief  Check whether the array is not empty.
		//! \return `isUsed()`
		inline operator bool() const { return isUsed(); }

		//! \brief  Assign an array view to this array.
		//!
		//! See `assign()` for more details. If the assignment fails, then
		//! `axarr_cxx_error` is used to handle the error. (The error might get
		//! ignored if not configured properly.)
		//!
		//! \param  arr Array to be assigned to this.
		//! \return `*this`
		inline TMutArr &operator=( const ArrayView &arr ) { if( !assign( arr ) ) { axarr_cxx_error( AXARR_MSG_FAILEDCOPY ); } return *this; }
		//! \copydoc operator=()
		inline TMutArr &operator=( const TMutArr &arr ) { if( !assign( arr ) ) { axarr_cxx_error( AXARR_MSG_FAILEDCOPY ); } return *this; }
#if AXARR_CXX11_MOVE_ENABLED
		//! \brief  Move another array to this array.
		//!
		//! \param  x Array to be swapped with this.
		//! \return `*this`
		inline TMutArr &operator=( TMutArr &&x )
		{
			const axarr_size_t cArr = m_cArr;
			const axarr_size_t cMax = m_cMax;
			TElement *const    pArr = m_pArr;
			const axarr_size_t cGra = m_cGranularity;

			m_cArr = x.m_cArr;
			m_cMax = x.m_cMax;
			m_pArr = x.m_pArr;
			m_cGranularity = x.m_cGranularity;

			x.m_cArr = cArr;
			x.m_cMax = cMax;
			x.m_pArr = pArr;
			x.m_cGranularity = cGra;

			return *this;
		}
#endif
		//! \copydoc operator=()
		template< SizeType tLen >
		inline TMutArr &operator=( const Type( &arr )[ tLen ] ) { if( !assign( arr ) ) { axarr_cxx_error( AXARR_MSG_FAILEDCOPY ); } return *this; }

		//! \brief  Append an array to this.
		//!
		//! See `append()` for more details. If the append fails, then
		//! `axarr_cxx_error` is used to handle the error. (The error might get
		//! ignored if not configured properly.)
		//!
		//! \return `*this`
		inline TMutArr &operator+=( const ArrayView &arr ) { if( !append( arr ) ) { axarr_cxx_error( AXARR_MSG_FAILEDADD ); } return *this; }
		//! \copydoc operator+=()
		template< SizeType tLen >
		inline TMutArr &operator+=( const Type( &arr )[ tLen ] ) { if( !append( arr ) ) { axarr_cxx_error( AXARR_MSG_FAILEDADD ); } return *this; }
		//! \copydoc operator+=()
		inline TMutArr &operator<<( const ArrayView &arr ) { if( !append( arr ) ) { axarr_cxx_error( AXARR_MSG_FAILEDADD ); } return *this; }

		//! \brief  Retrieve a reference to the element at the given index.
		//!
		//! \param  index Zero-based index of the element to be referenced.
		//! \return Reference to the element at `index`.
		inline       Type &at( SizeType index )       { AXARR_ASSERT( index < m_cArr ); return *pointer( index ); }
		//! \copydoc at()
		inline const Type &at( SizeType index ) const { AXARR_ASSERT( index < m_cArr ); return *pointer( index ); }

		//! \brief  Retrieve a reference to the first element in the array.
		//!
		//! The array must not be empty.
		//!
		//! \return Reference to the first element in the array.
		inline       Type &first()       { AXARR_ASSERT( isUsed() ); return *pointer( 0 ); }
		//! \copydoc first()
		inline const Type &first() const { AXARR_ASSERT( isUsed() ); return *pointer( 0 ); }
		//! \brief  Retrieve a reference to the last element in the array.
		//!
		//! The array must not be empty.
		//!
		//! \return Reference to the last element in the array.
		inline       Type &last ()       { AXARR_ASSERT( isUsed() ); return *pointer( m_cArr - 1 ); }
		//! \copydoc last()
		inline const Type &last () const { AXARR_ASSERT( isUsed() ); return *pointer( m_cArr - 1 ); }

		//! \brief  Access the element at the given index.
		//! \param  index Zero-based index to the element to be accessed.
		//! \return Reference to the element at `index`.
		inline       Type &operator[]( SizeType index )       { AXARR_ASSERT( index < m_cArr ); return *pointer( index ); }
		//! \copydoc operator[]()
		inline const Type &operator[]( SizeType index ) const { AXARR_ASSERT( index < m_cArr ); return *pointer( index ); }

		//! \copydoc TArr::slice()
		inline ArrayView slice( DiffType iLeft, DiffType iRight ) const
		{
			return ArrayView( *this ).slice( iLeft, iRight );
		}
		//! \copydoc TArr::mid()
		inline ArrayView mid( DiffType iBase, SizeType cItems = 1 ) const
		{
			return ArrayView( *this ).mid( iBase, cItems );
		}

		//! \brief  Retrieve a view of this array.
		//! \return `TArr<TElement>(*this)`. See `TArr` for more details.
		inline ArrayView view() const
		{
			return ArrayView( *this );
		}

		//! \brief  Search for an element that matches the given element in this
		//!         array.
		//! \param  x     Element to compare against. The first element found
		//!               that compares equal to this will be returned.
		//! \param  pFrom Optional starting location to begin searching from.
		//!               `nullptr` (default) is treated the same as `begin()`.
		//! \return Pointer to the element if found, or `nullptr` if not.
		inline const Type *find( const Type &x, const Type *pFrom = ( const Type * )0 ) const
		{
			return view().find( x, pFrom );
		}
		//! \brief  Determine whether a given element exists within the array.
		//! \param  x Element to compare against. If any element in the array
		//!           compares equal to this, then `true` will be returned.
		//! \return `true` if an equivalent element was found; `false`
		//!         otherwise.
		inline bool contains( const Type &x ) const
		{
			return view().contains( x );
		}

	private:
		// If set on `m_cGranularity`, then growing the array is disabled.
		static const SizeType kGranF_NoGrow = SizeType(1)<<(sizeof(SizeType)*8-1);
		static const SizeType kGranBits     = kGranF_NoGrow - 1;

		SizeType m_cArr;
		SizeType m_cMax;
		Type *   m_pArr;
		SizeType m_cGranularity;

		static inline void construct( Type &x )
		{
			AX_CONSTRUCT(x) Type();
		}
		static inline void construct( Type &x, const Type &y )
		{
			AX_CONSTRUCT(x) Type(y);
		}

		static inline void destroy( Type &x )
		{
			x.~Type();
		}
	};

	// ### Note: This class is a work-in-progress !!!

	/*! \brief A mutable block array.
	 *
	 *  Represents a modifiable non-contiguous array. Each instance owns the
	 *  memory it represents, as opposed to `TArr` which only references memory
	 *  owned by other arrays.
	 *
	 *  This class optimizes resize/append operations by allocating fixed-size
	 *  chunks of memory that can be indexed in constant-time, which is
	 *  accomplished by an indirection table. Therefore, there is an extra
	 *  potential cache-miss cost for each indirection into this array.
	 *
	 *  \note This is not a "sparse array" where there can be large chunks of
	 *        missing unallocated elements.
	 *
	 *  \warning By itself, this class is not thread-safe. You will need to
	 *           manage synchronization manually. This is by design.
	 */
	template< typename TElement, typename TAllocator = typename ArrayPolicies<TElement>::Allocator, axarr_size_t tGranularity = ( sizeof( TElement ) > 512 ? 1 : 512/sizeof( TElement ) ) >
	class TBlockArr: private TAllocator
	{
	public:
		typedef ArrayPolicies<TElement>            Policies;
		typedef typename Policies::Type            Type;
		typedef typename Policies::ArrayView       ArrayView;
		typedef typename Policies::DynamicArray    DynamicArray;
		typedef typename Policies::SizeType        SizeType;
		typedef typename Policies::DiffType        DiffType;
		typedef typename TAllocator::AllocSizeType AllocSizeType;

		static const SizeType kGranularity = tGranularity;

		inline TBlockArr()
		: m_cArr( 0 )
		, m_ppArr( ( Type ** )0 )
		{
		}
		inline ~TBlockArr()
		{
			purge();
		}

		inline void purge()
		{
			clear();
			deleteCache();
		}
		inline void clear()
		{
			for( SizeType j = 0; j < m_cArr; j += kGranularity ) {
				Type *const pArr = m_ppArr[ j/kGranularity ];
				const SizeType n = m_cArr - j < kGranularity ? m_cArr - j : kGranularity;
				for( SizeType i = 0; i < n; ++i ) {
					destroy( pArr[ i ] );
				}
			}

			m_cArr = 0;
		}
		inline SizeType numTables() const
		{
			return m_cArr/kGranularity + ( +( m_cArr%kGranularity != 0 ) );
		}
		inline SizeType maxTables() const
		{
			return m_cMax/kGranularity + ( +( m_cMax%kGranularity != 0 ) );
		}

		inline SizeType num() const
		{
			return m_cArr;
		}
		inline SizeType max() const
		{
			return m_cMax;
		}

		inline AllocSizeType memSize() const
		{
			return sizeof(*this) + maxTables()*sizeof(Type)*kGranularity;
		}

		inline const Type *ptr_const( SizeType index ) const
		{
			if( index >= m_cArr ) {
				return reinterpret_cast< Type * >( 0 );
			}

			return &m_ppArr[ index/kGranularity ][ index%kGranularity ];
		}
		inline const Type *ptr( SizeType index ) const
		{
			return ptr_const( index );
		}
		inline Type *ptr( SizeType index )
		{
			return const_cast< Type * >( ptr_const( index ) );
		}

		inline const Type &at( SizeType index ) const
		{
			AXARR_ASSERT( index < m_cArr );
			const Type *const p = ptr_const( index );
			return *p;
		}
		inline       Type &at( SizeType index )
		{
			AXARR_ASSERT( index < m_cArr );
			Type *const p = const_cast< Type * >( ptr_const( index ) );
			return *p;
		}

		inline const Type &operator[]( SizeType index ) const
		{
			AXARR_ASSERT( index < m_cArr );
			return *ptr_const( index );
		}
		inline       Type &operator[]( SizeType index )
		{
			AXARR_ASSERT( index < m_cArr );
			return *const_cast< Type * >( ptr_const( index ) );
		}

	private:
		SizeType m_cArr;
		SizeType m_cMax;
		Type **  m_ppArr;

		static inline void construct( Type &x )
		{
			AX_CONSTRUCT(x) Type();
		}
		static inline void construct( Type &x, const Type &y )
		{
			AX_CONSTRUCT(x) Type(y);
		}

		static inline void destroy( Type &x )
		{
			x.~Type();
		}

		static inline bool alloc( Type *&p )
		{
			p = reinterpret_cast< Type * >( Policies::Allocator::allocate( sizeof( Type )*kGranularity ) );
			return p != reinterpret_cast< Type * >( 0 );
		}
		static inline void free( Type *&p )
		{
			Policies::Allocator::deallocate( reinterpret_cast< void * >( p ), sizeof( Type )*kGranularity );
			p = reinterpret_cast< Type * >( 0 );
		}

		static inline bool realloc( Type **&pp, SizeType oldn, SizeType n )
		{
			Type **const qq = n ? reinterpret_cast< Type ** >( Policies::Allocator::allocate( sizeof( Type * )*n ) ) : reinterpret_cast< Type ** >( 0 );
			if( n && !qq ) {
				return false;
			}

			if( pp != reinterpret_cast< Type ** >( 0 ) ) {
				Policies::Allocator::deallocate( reinterpret_cast< void * >( pp ), sizeof( Type * )*oldn );
			}

			pp = qq;
			return true;
		}
		static inline void free( TElement **&pp, SizeType oldn )
		{
			realloc( pp, oldn, 0 );
		}

		inline void deleteCache()
		{
			for( SizeType j = 0; j < m_cMax; j += kGranularity ) {
				const SizeType i = j/kGranularity;
				free( m_ppArr[ i ] );
			}
			free( m_ppArr );
		}
	};

	template< typename TElement, typename TAllocator >
	inline TMutArr< TElement, TAllocator >::TMutArr()
	: m_cArr( 0 )
	, m_cMax( 0 )
	, m_pArr( NULL )
	, m_cGranularity( kDefaultGranularity )
	{
	}
	template< typename TElement, typename TAllocator >
	inline TMutArr< TElement, TAllocator >::TMutArr( const TMutArr &arr )
	: m_cArr( 0 )
	, m_cMax( 0 )
	, m_pArr( NULL )
	, m_cGranularity( arr.m_cGranularity )
	{
		if( !append( arr ) ) {
			axarr_cxx_error( AXARR_MSG_FAILEDINIT );
		}
	}
	template< typename TElement, typename TAllocator >
	inline TMutArr< TElement, TAllocator >::TMutArr( const ArrayView &arr )
	: m_cArr( 0 )
	, m_cMax( 0 )
	, m_pArr( NULL )
	, m_cGranularity( kDefaultGranularity )
	{
		if( !append( arr ) ) {
			axarr_cxx_error( AXARR_MSG_FAILEDINIT );
		}
	}
	template< typename TElement, typename TAllocator >
	inline TMutArr< TElement, TAllocator >::TMutArr( SizeType cItems, const Type *pItems )
	: m_cArr( 0 )
	, m_cMax( 0 )
	, m_pArr( NULL )
	, m_cGranularity( kDefaultGranularity )
	{
		if( !append( cItems, pItems ) ) {
			axarr_cxx_error( AXARR_MSG_FAILEDINIT );
		}
	}
	template< typename TElement, typename TAllocator >
	inline TMutArr< TElement, TAllocator >::~TMutArr()
	{
		purge();
	}

	template< typename TElement, typename TAllocator >
	inline void TMutArr< TElement, TAllocator >::clear()
	{
		if( !AXARR_HAS_TRIVIAL_DESTRUCTOR(TElement) ) {
			for( SizeType i = m_cArr; i > 0; --i ) {
				destroy( m_pArr[ i - 1 ] );
			}
		}
		m_cArr = 0;
	}
	template< typename TElement, typename TAllocator >
	inline void TMutArr< TElement, TAllocator >::purge()
	{
		setAllocated( 0 );
	}

	template< typename TElement, typename TAllocator >
	inline bool TMutArr< TElement, TAllocator >::resize( SizeType size, const Type *pInit )
	{
		if( !reserve( size ) ) {
			return false;
		}

		if( !AXARR_HAS_TRIVIAL_DESTRUCTOR(Type) ) {
			for( SizeType i = m_cArr; i > size; --i ) {
				destroy( m_pArr[ i - 1 ] );
			}
		}

		if( pInit != ( const Type * )0 ) {
			const Type &x = *pInit;

			if( AXARR_HAS_TRIVIAL_CONSTRUCTOR(Type) && detail::TIsZero<Type>::test(x) && m_cArr < size ) {
				AXARR_MEMSET( &m_pArr[m_cArr], 0, ( size - m_cArr )*sizeof(Type) );
			} else {
				for( SizeType i = m_cArr; i < size; ++i ) {
					construct( m_pArr[ i ], x );
				}
			}
		}

		m_cArr = size;
		return true;
	}
	template< typename TElement, typename TAllocator >
	inline bool TMutArr< TElement, TAllocator >::reserve( SizeType size )
	{
		if( m_cMax >= size ) {
			return true;
		} else if( ( m_cGranularity & kGranF_NoGrow ) == kGranF_NoGrow ) {
			return false;
		}

		const SizeType gran = m_cGranularity & kGranBits;

		const SizeType granulatedSize = size%gran != 0 ? size + ( gran - size%gran ) : size;
		return setAllocated( granulatedSize );
	}
	template< typename TElement, typename TAllocator >
	inline bool TMutArr< TElement, TAllocator >::setAllocated( SizeType size )
	{
		if( AXARR_HAS_TRIVIAL_DESTRUCTOR(Type) ) {
			if( m_cArr > size ) {
				m_cArr = size;
			}
		} else {
			while( m_cArr > size ) {
				destroy( m_pArr[ --m_cArr ] );
			}
		}

		Type *pItems = NULL;
		if( size > 0 ) {
			pItems = reinterpret_cast< Type * >( Policies::Allocator::allocate( sizeof( Type )*size) );
			if( !pItems ) {
				return false;
			}

			if( AXARR_HAS_TRIVIAL_RELOCATE(Type) && m_cArr > 0 ) {
				const SizeType n = m_cArr < size ? m_cArr : size;
				AXARR_MEMCPY( pItems, m_pArr, sizeof( Type )*n );
			} else {
#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:6385)
#endif
				for( SizeType i = 0; i < m_cArr; ++i ) {
					construct( pItems[ i ], m_pArr[ i ] );
				}
#ifdef _MSC_VER
# pragma warning(pop)
#endif
			}

		}

		Policies::Allocator::deallocate( reinterpret_cast< void * >( m_pArr ), sizeof( Type )*m_cMax );
		m_pArr = pItems;
		m_cMax = size;

		return true;
	}

	template< typename TElement, typename TAllocator >
	inline bool TMutArr< TElement, TAllocator >::assign( SizeType cItems, const Type *pItems )
	{
		if( ( cItems > 0 && !pItems ) || !reserve( cItems ) ) {
			return false;
		}

		if( AXARR_HAS_TRIVIAL_DESTRUCTOR(Type) ) {
			m_cArr = 0;
		} else {
			while( m_cArr > 0 ) {
				destroy( m_pArr[ --m_cArr ] );
			}
		}

		if( AXARR_HAS_TRIVIAL_CONSTRUCTOR(Type) ) {
			AXARR_MEMCPY( m_pArr, pItems, sizeof( Type )*cItems );
			m_cArr = cItems;
		} else {
			while( m_cArr < cItems ) {
				construct( m_pArr[ m_cArr ], pItems[ m_cArr ] );
				++m_cArr;
			}
		}

		return true;
	}
	template< typename TElement, typename TAllocator >
	inline bool TMutArr< TElement, TAllocator >::append( SizeType cItems, const Type *pItems )
	{
		if( ( cItems > 0 && !pItems ) || !reserve( m_cArr + cItems ) ) {
			return false;
		}

		if( AXARR_HAS_TRIVIAL_CONSTRUCTOR(Type) ) {
			AXARR_MEMCPY( &m_pArr[m_cArr], pItems, sizeof( Type )*cItems );
			m_cArr += cItems;
		} else {
			for( SizeType i = 0; i < cItems; ++i ) {
				construct( m_pArr[ m_cArr++ ], pItems[ i ] );
			}
		}

		return true;
	}
	template< typename TElement, typename TAllocator >
	inline bool TMutArr< TElement, TAllocator >::prepend( SizeType cItems, const Type *pItems )
	{
		if( !m_cArr ) {
			return append( cItems, pItems );
		}

		return insert( cItems, pItems, m_pArr );
	}
	template< typename TElement, typename TAllocator >
	inline bool TMutArr< TElement, TAllocator >::insert( SizeType cItems, const Type *pItems, Type *pBefore )
	{
		const SizeType cBefore = index( pBefore );
		if( cBefore > m_cArr || !pItems || !reserve( m_cArr + cItems ) ) {
			return false;
		}

		const SizeType top = m_cArr + cItems;
		for( SizeType i = 0; i < m_cArr - cBefore; ++i ) {
			construct( m_pArr[ top - i ], m_pArr[ m_cArr - i ] );
			destroy( m_pArr[ m_cArr - i ] );
		}

		if( AXARR_HAS_TRIVIAL_CONSTRUCTOR(Type) ) {
			AXARR_MEMCPY( &m_pArr[ cBefore ], pItems, sizeof( Type )*cItems );
		} else {
			for( SizeType i = 0; i < cItems; ++i ) {
				construct( m_pArr[ cBefore + i ], pItems[ i ] );
			}
		}

		m_cArr += cItems;
		return true;
	}

	/*

		[ 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 ]
		.                 R----------->

		RM: 4 .. 7 (inclusive)
		... start = 4
		... cItems = 4


		[ 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 ]
		.                             D
		.                         D
		.                     D
		.                 D
		... destruct in reverse order from 7 to 4


		[ 0 | 1 | 2 | 3 | 8 | 9 | 6 | 7 | 8 | 9 ]
		.                         X   X   |   |
		.                 M               |   |
		.                     M           |   |
		.                                 |   D
		.                                 D
		... NOTE: pItems 6 and 7 are dead
		... move pItems 8 and 9 to spots 4 and 5 respectively (in that order)
		... destruct where pItems 8 and 9 were


		[ 0 | 1 | 2 | 3 | 8 | 9 ]

		... final

	*/

	template< typename TElement, typename TAllocator >
	inline void TMutArr< TElement, TAllocator >::remove( SizeType first, SizeType count )
	{
		if( first >= m_cArr || !count ) {
			return;
		}

		const SizeType top = first + count > m_cArr ? m_cArr : first + count;
		if( !AXARR_HAS_TRIVIAL_DESTRUCTOR(Type) ) {
			for( SizeType i = top; i > first; --i ) {
				destroy( m_pArr[ i - 1 ] );
			}
		}

		const SizeType remainder = m_cArr - top;
		for( SizeType i = 0; i < remainder; ++i ) {
			construct( m_pArr[ first + i ], m_pArr[ top + i ] );
			destroy( m_pArr[ top + i ] );
		}

		const SizeType removed = count > m_cArr ? m_cArr : count;
		m_cArr -= removed;
	}

	template< typename TElement, typename TAllocator >
	inline typename TMutArr<TElement, TAllocator>::Type TMutArr< TElement, TAllocator >::popLast()
	{
		if( !m_cArr ) {
			return Type();
		}

		const Type x = m_pArr[ m_cArr - 1 ];
		resize( m_cArr - 1 );
		return x;
	}

	/* ---------------------------------------------------------------------- */

	template< typename TElement >
	template< typename TAllocator >
	inline TArr< TElement >::TArr( const TMutArr< TElement, TAllocator > &arr )
	: m_pArr( arr.m_pArr )
	, m_cArr( arr.m_cArr )
	{
	}

}

#endif
