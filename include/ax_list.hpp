/*

	ax_list - public domain
	last update: 2015-10-01 Aaron Miller


	This library provides a typical linked list and an intrusively linked list.


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

#ifndef INCGUARD_AX_LIST_HPP_
#define INCGUARD_AX_LIST_HPP_

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

#ifdef AX_TYPES_DEFINED
typedef ax_uptr_t axls_size_t;
#else
# include <stddef.h>
typedef size_t axls_size_t;
#endif

#ifndef axls_alloc
# include <cstdlib>
# define axls_alloc(N_)				(malloc((N_)))
# define axls_free(P_)				(free((P_)))
#endif

#ifndef AXLS_CONSTEXPR_ENABLED
# if defined( AX_CXX_N2235 )
#  if AX_CXX_N2235
#   define AXLS_CONSTEXPR_ENABLED 1
#  else
#   define AXLS_CONSTEXPR_ENABLED 0
#  endif
# elif defined( __has_feature )
#  if __has_feature( cxx_constexpr )
#   define AXLS_CONSTEXPR_ENABLED 1
#  else
#   define AXLS_CONSTEXPR_ENABLED 0
#  endif
# elif defined( _MSC_VER ) && _MSC_VER >= 1900
#  define AXLS_CONSTEXPR_ENABLED 1
# elif defined( __GNUC__ ) && ( __GNUC__ > 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 6 ) )
#  define AXLS_CONSTEXPR_ENABLED 1
# elif defined( __clang__ ) && ( __clang__ > 3 || ( __clang__ == 3 && __clang_minor__ >= 1 ) )
#  define AXLS_CONSTEXPR_ENABLED 1
# elif defined( __INTEL_COMPILER ) && __INTEL_COMPILER >= 1400
#  define AXLS_CONSTEXPR_ENABLED 1
# else
#  define AXLS_CONSTEXPR_ENABLED 0
# endif
#endif

#ifndef AXLS_CONSTEXPR
# if AXLS_CONSTEXPR_ENABLED
#  define AXLS_CONSTEXPR constexpr
# else
#  define AXLS_CONSTEXPR const
# endif
#endif

#ifndef AX_CONSTRUCT
# define AX_CONSTRUCT(X_)\
	::new(reinterpret_cast<void*>(&(X_)),::ax::detail::SPlcmntNw())
namespace ax { namespace detail { struct SPlcmntNw {}; } }
inline void *operator new( axls_size_t, void *p, ax::detail::SPlcmntNw )
{
	return p;
}
inline void operator delete( void *, void *, ax::detail::SPlcmntNw )
{
}
#endif

namespace ax
{

	// Forward declarations
	template< typename TElement >
	class TIntrList;
	template< typename TElement >
	class TIntrLink;

	namespace detail
	{

		template< typename TLink >
		struct TListIter
		{
			typedef TLink LinkType;
			typedef typename TLink::ElementType ElementType;

			TLink *pLink;

			inline TListIter(): pLink( NULL ) {}
			inline TListIter( TLink &x ) : pLink( &x ) {}
			inline TListIter( TLink *x ) : pLink( x ) {}
			inline TListIter( const TListIter &iter ) : pLink( iter.pLink ) {}
			inline ~TListIter() {}

			inline ElementType *get() { return pLink != NULL ? pLink->node() : NULL; }
			inline const ElementType *get() const { return pLink != NULL ? pLink->node() : NULL; }

			inline TListIter &operator=( const TListIter &iter ) { pLink = iter.pLink; return *this; }

			inline bool operator!() const { return !pLink || !pLink->node(); }
			inline bool operator==( const TListIter &iter ) const { return pLink == iter.pLink; }
			inline bool operator!=( const TListIter &iter ) const { return pLink != iter.pLink; }
			inline operator ElementType *( ) { return pLink->node(); }
			inline operator const ElementType *( ) const { return pLink->node(); }
			inline operator bool() const { return get() != NULL; }

			inline ElementType &operator*() { return *pLink->node(); }
			inline ElementType *operator->() { return pLink->node(); }
			inline const ElementType &operator*() const { return *pLink->node(); }
			inline const ElementType *operator->() const { return pLink->node(); }

			inline TListIter &retreat() { pLink = pLink != NULL ? pLink->prevLink() : NULL; return *this; }
			inline TListIter &advance() { pLink = pLink != NULL ? pLink->nextLink() : NULL; return *this; }

			inline TListIter &operator--() { return retreat(); }
			inline TListIter &operator++() { return advance(); }

			inline TListIter operator--( int ) const { return TListIter( const_cast< TLink * >( pLink ) ).retreat(); }
			inline TListIter operator++( int ) const { return TListIter( const_cast< TLink * >( pLink ) ).advance(); }
		};

	}

	namespace policy
	{

		template< typename TElement >
		struct ListIndexing
		{
			typedef axls_size_t SizeType;
		};
		template< typename TElement >
		struct ListAllocator
		{
			typedef axls_size_t AllocSizeType;

			inline void *allocate( AllocSizeType cBytes )
			{
				return axls_alloc( cBytes );
			}
			inline void deallocate( void *pBytes, AllocSizeType cBytes )
			{
				( ( void )cBytes );
				axls_free( pBytes );
			}
		};

	}
	template< typename TElement >
	struct ListPolicies
	{
		typedef TElement                          Type;

		typedef policy::ListIndexing<Type>        Indexing;
		typedef policy::ListAllocator<Type>       Allocator;

		typedef typename Indexing::SizeType       SizeType;
		typedef typename Allocator::AllocSizeType AllocSizeType;
	};

	// A link within a list -- meant to be used as a member of whatever object owns it
	template< typename TElement >
	class TIntrLink
	{
	friend class TIntrList< TElement >;
	public:
		typedef TElement ElementType;
		typedef TIntrList< TElement > ListType;

		// Default constructor
		TIntrLink();
		// Construct with a given owner
		TIntrLink( TElement *pNode );
		// Construct with a given owner, added to the end of a list
		TIntrLink( TElement *pNode, TIntrList< TElement > &list );
		// Destructor -- unlinks from list
		~TIntrLink();

		// remove self from whatever list we're in
		void unlink();
		// Unlink from current list and insert before the given link
		void insertBefore( TIntrLink &link );
		// Unlink from current list and insert after the given link
		void insertAfter( TIntrLink &link );

		// Move this link to the front of the list it's within
		void toFront();
		// Move this link to the back of the list it's within
		void toBack();
		// Move this link to before the link immediately before this one
		void toPrior();
		// Move this link to after the link immediately after this one
		void toNext();

		// Retrieve the previous sibling link
		inline TIntrLink *prevLink() { return m_pPrev; }
		// Retrieve the previous sibling link [const]
		inline const TIntrLink *prevLink() const { return m_pPrev; }

		// Retrieve the next sibling link
		inline TIntrLink *nextLink() { return m_pNext; }
		// Retrieve the next sibling link [const]
		inline const TIntrLink *nextLink() const { return m_pNext; }

		// Retrieve the owner of the sibling link prior to this
		inline TElement *prev() { return m_pPrev != NULL ? m_pPrev->m_pNode : NULL; }
		// Retrieve the owner of the sibling link prior to this [const]
		inline const TElement *prev() const { return m_pPrev != NULL ? m_pPrev->m_pNode : NULL; }

		// Retrieve the owner of the sibling link after this
		inline TElement *next() { return m_pNext != NULL ? m_pNext->m_pNode : NULL; }
		// Retrieve the owner of the sibling link after this [const]
		inline const TElement *next() const { return m_pNext != NULL ? m_pNext->m_pNode : NULL; }

		// Set the owner of this link
		inline void setNode( TElement *pNode ) { m_pNode = pNode; }
		// Retrieve the owner of this link
		inline TElement *node() { return m_pNode; }
		// Retrieve the owner of this link [const]
		inline const TElement *node() const { return m_pNode; }

		// Retrieve the owner of this link
		inline TElement &operator *() { return *m_pNode; }
		// Retrieve the owner of this link [const]
		inline const TElement &operator *() const { return *m_pNode; }

		// Dereference the owner of this link
		inline TElement *operator->() { return m_pNode; }
		// Dereference the owner of this link [const]
		inline const TElement *operator->() const { return m_pNode; }

		// Retrieve the list this link is a part of
		inline TIntrList< TElement > *list() { return m_pList; }
		// Retrieve the list this link is a part of [const]
		inline const TIntrList< TElement > *list() const { return m_pList; }

		// Determine whether an owner is set
		inline operator bool() const { return m_pNode != NULL; }
		// Retrieve the owner of this link (through casting)
		inline operator TElement *() { return m_pNode; }
		// Retrieve the owner of this link (through casting)
		inline operator const TElement *() const { return m_pNode; }

	protected:
		// Prior sibling link
		TIntrLink< TElement > *m_pPrev;
		// next sibling link
		TIntrLink< TElement > *m_pNext;
		// List this link resides within
		TIntrList< TElement > *m_pList;
		// Current owner node
		TElement *m_pNode;

	private:
#ifdef AX_DELETE_COPYFUNCS
		AX_DELETE_COPYFUNCS( TIntrLink );
#endif
	};
	// A list of intrusive links -- does not do any allocations
	template< typename TElement >
	class TIntrList
	{
	public:
		typedef TElement ElementType;
		typedef TIntrLink< TElement > LinkType;
		typedef detail::TListIter< TIntrLink< TElement > > Iterator;
		typedef int( *FnComparator )( const TElement &, const TElement & );

		static inline int operatorLessComparator_f( const TElement &a, const TElement &b )
		{
			return +( a < b );
		}

		// Default constructor
		TIntrList();
		// Destructor -- unlinks all items in the list
		~TIntrList();

		// Unlinks the given link from this list (debug mode checks that the link is in this list)
		void unlink( TIntrLink< TElement > &link );
		// Unlink all items in this list
		void clear();
		// Unlink all items in this list, deleting the owner nodes
		void deleteAll();
		// Determine whether this list has no links in it
		bool isEmpty() const;
		// Determine whether this list has any links in it
		bool isUsed() const;
		// Count how many links are in this list (this value is not cached)
		axls_size_t num() const;

		// Add a link to the front of this list after unlinking it from whatever list it was in before
		void addHead( TIntrLink< TElement > &link );
		// Add a link to the back of this list after unlinking it from whatever list it was in before
		void addTail( TIntrLink< TElement > &link );
		// Insert 'link' before 'before' (debug mode checks that 'before' is part of this list) after unlinking 'link' from whatever list it was in before
		void insertBefore( TIntrLink< TElement > &link, TIntrLink< TElement > &before );
		// Insert 'link' after 'after' (debug mode checks that 'after' is part of this list) after unlinking 'link' from whatever list it was in before
		void insertAfter( TIntrLink< TElement > &link, TIntrLink< TElement > &after );

		// Sort the items in this list with the given comparison function
		void sort( FnComparator pfnCompare );
		// Sort the items in this list with ElementType::operator<() comparison
		inline void sort() { sort( &operatorLessComparator_f ); }

		// Retrieve the link at the front of this list
		inline TIntrLink< TElement > *headLink() { return m_pHead; }
		// Retrieve the link at the front of this list [const]
		inline const TIntrLink< TElement > *headLink() const { return m_pHead; }

		// Retrieve the link at the back of this list
		inline TIntrLink< TElement > *tailLink() { return m_pTail; }
		// Retrieve the link at the back of this list [const]
		inline const TIntrLink< TElement > *tailLink() const { return m_pTail; }

		// Retrieve the owner of the link at the front of this list
		inline TElement *head() { return m_pHead != NULL ? m_pHead->m_pNode : NULL; }
		// Retrieve the owner of the link at the front of this list [const]
		inline const TElement *head() const { return m_pHead != NULL ? m_pHead->m_pNode : NULL; }

		// Retrieve the owner of the link at the back of this list
		inline TElement *tail() { return m_pTail != NULL ? m_pTail->m_pNode : NULL; }
		// Retrieve the owner of the link at the back of this list [const]
		inline const TElement *tail() const { return m_pTail != NULL ? m_pTail->m_pNode : NULL; }

		// Determine whether this list has any items in it
		inline operator bool() const { return isUsed(); }
		// Determine whether this list has no items in it
		inline bool operator!() const { return isEmpty(); }

		// C++ range-for begin()
		inline Iterator begin() const { return Iterator( const_cast< TIntrLink< TElement > * >( m_pHead ) ); }
		// C++ range-for end()
		inline Iterator end() const { return Iterator(); }

	private:
		// Link to the front of the list
		TIntrLink< TElement > *m_pHead;
		// Link to the back of the list
		TIntrLink< TElement > *m_pTail;

#ifdef AX_DELETE_COPYFUNCS
		AX_DELETE_COPYFUNCS( TIntrList );
#endif
	};

	template< typename TElement, typename TAllocator = typename ListPolicies<TElement>::Allocator >
	class TList: private TAllocator
	{
	public:
		typedef ListPolicies<TElement>           Policies;
		typedef typename Policies::Type          Type;
		typedef typename Policies::SizeType      SizeType;
		typedef typename Policies::AllocSizeType AllocSizeType;

		typedef typename TIntrList< Type >::FnComparator         FnComparator;
		typedef detail::TListIter< TIntrLink< Type > >           Iterator;
		typedef detail::TListIter< TIntrLink< const TElement > > ConstIterator;

		TList();
		TList( const TList &ls );
		template< typename TOtherAllocator >
		TList( const TList< TElement, TOtherAllocator > &ls );
		template< SizeType tLen >
		inline TList( const Type( &arr )[ tLen ] )
		: m_list()
		{
			for( SizeType i = 0; i < tLen; ++i ) {
				addTail( arr[ i ] );
			}
		}
		TList( SizeType cItems, Type *pItems );
		~TList();

		inline TList &operator=( const TList &ls )
		{
			if( this == &ls ) {
				return *this;
			}

			clear();
			for( ConstIterator x = ls.begin(); x != ls.end(); ++x ) {
				addTail( x );
			}
			return *this;
		}
		template< typename TOtherAllocator >
		inline TList &operator=( const TList< Type, TOtherAllocator >&ls )
		{
			clear();
			for( typename TList< Type, TOtherAllocator >::ConstIterator x = ls.begin(); x != ls.end(); ++x ) {
				addTail( x );
			}
			return *this;
		}

		template< SizeType tLen >
		inline TList &operator=( const Type( &arr )[ tLen ] )
		{
			clear();
			for( SizeType i = 0; i < tLen; ++i ) {
				addTail( arr[ i ] );
			}
			return *this;
		}

		void clear();
		bool isEmpty() const;
		bool isUsed() const;
		SizeType num() const;
		SizeType len() const;
		AllocSizeType memSize() const;

		// For C++ range-based for-loops
		inline Iterator begin() { return Iterator( m_list.headLink() ); }
		inline Iterator end() { return Iterator(); }
		inline ConstIterator begin() const { return ConstIterator( m_list.headLink() ); }
		inline ConstIterator end() const { return ConstIterator(); }

		inline Iterator first() { return Iterator( m_list.headLink() ); }
		inline Iterator last() { return Iterator( m_list.tailLink() ); }

		inline ConstIterator first() const { return ConstIterator( m_list.headLink() ); }
		inline ConstIterator last() const { return ConstIterator( m_list.tailLink() ); }

		Iterator addHead();
		Iterator addTail();
		Iterator insertBefore( Iterator x );
		Iterator insertAfter( Iterator x );

		Iterator addHead( const Type &element );
		Iterator addTail( const Type &element );
		Iterator insertBefore( Iterator x, const Type &element );
		Iterator insertAfter( Iterator x, const Type &element );
		ConstIterator find_const( const Type &item ) const;
		inline ConstIterator find( const Type &item ) const { return find_const( item ); }
		inline Iterator find( const Type &item ) { return find_const( item ); }

		Iterator remove( Iterator iter );

		void sort( FnComparator pfnCompare );
		void sort();

	private:
		typedef TIntrList< Type > IntrList;
		typedef TIntrLink< Type > IntrLink;

		IntrList m_list;

		IntrLink *alloc_();
		IntrLink *alloc_( const Type &x );
		void dealloc_( IntrLink *ptr );
	};

	namespace detail
	{

		template< axls_size_t tElementSize, unsigned tNumElements, typename TOtherAllocator >
		class TSmallListAllocator: private TOtherAllocator
		{
		public:
#ifdef AX_HAS_STATIC_ASSERT
# if AX_HAS_STATIC_ASSERT
			static_assert( tNumElements > 0, "Too few elements for small list allocator" );
# endif
#endif

			TSmallListAllocator()
			: TOtherAllocator()
			, m_pFreeObj( ( SMemoryObject * )0 )
			{
				SMemoryObject *pPrev = ( SMemoryObject * )0;
				for( unsigned i = 0; i < tNumElements; ++i ) {
					m_objects[ i ].data.pNext = pPrev;
					pPrev = &m_objects[ i ];
				}
				m_pFreeObj = &m_objects[ tNumElements - 1 ];
			}
			~TSmallListAllocator()
			{
			}

			void *allocate( axls_size_t n )
			{
				if( !n ) {
					return ( void * )0;
				}

				if( n <= sizeof( SMemoryObject ) && m_pFreeObj != ( SMemoryObject * )0 ) {
					SMemoryObject *const p = m_pFreeObj;
					m_pFreeObj = m_pFreeObj->data.pNext;

					return &p->data.bytes[ 0 ];
				}

				return TOtherAllocator::allocate( n );
			}
			void deallocate( void *p, axls_size_t n )
			{
				if( !p ) {
					return;
				}

				if( n <= sizeof( SMemoryObject ) && isPointingToMemoryObject( p ) ) {
					SMemoryObject *const pMemObj = reinterpret_cast< SMemoryObject * >( p );
					pMemObj->data.pNext = m_pFreeObj;
					m_pFreeObj = pMemObj;
					return;
				}

				TOtherAllocator::deallocate( p, n );
			}

		private:
			struct SMemoryObject
			{
				union
				{
					unsigned char  bytes[ tElementSize ];
					SMemoryObject *pNext;
				} data;
			};

			SMemoryObject  m_objects[ tNumElements ];
			SMemoryObject *m_pFreeObj;

			inline bool isPointingToMemoryObject( void *p ) const
			{
				return p >= &m_objects[0] && p <= &m_objects[ tNumElements ];
			}
		};

		template< typename TElement, unsigned tNumElements, typename TOtherAllocator >
		struct TSmallListAllocatorSelector
		{
			typedef TSmallListAllocator< sizeof( TElement ) + sizeof( TIntrLink< TElement > ), tNumElements, TOtherAllocator > Allocator;
		};

		template< typename TElement, unsigned tNumElements, typename TOtherAllocator >
		struct TSmallListBase
		{
			typedef TList< TElement, typename TSmallListAllocatorSelector< TElement, tNumElements, TOtherAllocator >::Allocator > List;
		};

	}

	template< typename TElement, unsigned tNumElements, typename TOtherAllocator = typename ListPolicies<TElement>::Allocator >
	class TSmallList: public detail::TSmallListBase< TElement, tNumElements, TOtherAllocator >::List
	{
	public:
		typedef typename detail::TSmallListBase< TElement, tNumElements, TOtherAllocator >::List ListBase;

		typedef ListPolicies<TElement>           Policies;
		typedef typename Policies::Type          Type;
		typedef typename Policies::SizeType      SizeType;
		typedef typename Policies::AllocSizeType AllocSizeType;

		typedef typename TIntrList< Type >::FnComparator         FnComparator;
		typedef detail::TListIter< TIntrLink< Type > >           Iterator;
		typedef detail::TListIter< TIntrLink< const TElement > > ConstIterator;

		TSmallList(): ListBase() {}
		TSmallList( const TSmallList &ls ): ListBase( ls ) {}
		template< typename TPassedAllocator >
		TSmallList( const TList< TElement, TPassedAllocator > &ls ): ListBase( ls ) {}
		template< SizeType tLen >
		TSmallList( const Type( &arr )[ tLen ] ): ListBase( arr ) {}
		TSmallList( SizeType cItems, Type *pItems ): ListBase( cItems, pItems ) {}
		~TSmallList() {}

		TSmallList &operator=( const ListBase &x )
		{
			ListBase::operator=( x );
			return *this;
		}
	};


	/*
	===========================================================================

		INTRUSIVE LINK

	===========================================================================
	*/

	template< typename TElement >
	TIntrLink< TElement >::TIntrLink()
	: m_pPrev( NULL )
	, m_pNext( NULL )
	, m_pList( NULL )
	, m_pNode( NULL )
	{
	}
	template< typename TElement >
	TIntrLink< TElement >::TIntrLink( TElement *pNode )
	: m_pPrev( NULL )
	, m_pNext( NULL )
	, m_pList( NULL )
	, m_pNode( pNode )
	{
	}
	template< typename TElement >
	TIntrLink< TElement >::TIntrLink( TElement *pNode, TIntrList< TElement > &list )
	: m_pPrev( NULL )
	, m_pNext( NULL )
	, m_pList( NULL )
	, m_pNode( pNode )
	{
		list.addTail( *this );
	}
	template< typename TElement >
	TIntrLink< TElement >::~TIntrLink()
	{
		unlink();
	}

	template< typename TElement >
	void TIntrLink< TElement >::unlink()
	{
		if( !m_pList ) {
			return;
		}

		m_pList->unlink( *this );
	}
	template< typename TElement >
	void TIntrLink< TElement >::insertBefore( TIntrLink &link )
	{
		if( !m_pList ) {
			return;
		}

		m_pList->insertBefore( link, *this );
	}
	template< typename TElement >
	void TIntrLink< TElement >::insertAfter( TIntrLink &link )
	{
		if( !m_pList ) {
			return;
		}

		m_pList->insertAfter( link, *this );
	}

	template< typename TElement >
	void TIntrLink< TElement >::toFront()
	{
		if( !m_pList ) {
			return;
		}

		m_pList->addHead( *this );
	}
	template< typename TElement >
	void TIntrLink< TElement >::toBack()
	{
		if( !m_pList ) {
			return;
		}

		m_pList->addTail( *this );
	}
	template< typename TElement >
	void TIntrLink< TElement >::toPrior()
	{
		if( !m_pList || !m_pPrev ) {
			return;
		}

		m_pList->insertBefore( *this, *m_pPrev );
	}
	template< typename TElement >
	void TIntrLink< TElement >::toNext()
	{
		if( !m_pList || !m_pNext ) {
			return;
		}

		m_pList->insertAfter( *this, *m_pNext );
	}


	/*
	===========================================================================

		INTRUSIVE LIST

	===========================================================================
	*/

	template< typename TElement >
	TIntrList< TElement >::TIntrList()
	: m_pHead( NULL )
	, m_pTail( NULL )
	{
	}
	template< typename TElement >
	TIntrList< TElement >::~TIntrList()
	{
		clear();
	}

	template< typename TElement >
	void TIntrList< TElement >::unlink( TIntrLink< TElement > &link )
	{
		if( link.m_pList != this ) {
			return;
		}

		if( link.m_pPrev != NULL ) {
			link.m_pPrev->m_pNext = link.m_pNext;
		} else {
			m_pHead = link.m_pNext;
		}

		if( link.m_pNext != NULL ) {
			link.m_pNext->m_pPrev = link.m_pPrev;
		} else {
			m_pTail = link.m_pPrev;
		}

		link.m_pList = NULL;
		link.m_pPrev = NULL;
		link.m_pNext = NULL;
	}
	template< typename TElement >
	void TIntrList< TElement >::clear()
	{
		while( m_pHead != NULL ) {
			unlink( *m_pHead );
		}
	}
	template< typename TElement >
	void TIntrList< TElement >::deleteAll()
	{
		while( m_pHead != NULL ) {
			TIntrLink< TElement > *link = m_pHead;

			unlink( *link );
			delete link->m_pNode;
		}
	}
	template< typename TElement >
	bool TIntrList< TElement >::isEmpty() const
	{
		return m_pHead == NULL;
	}
	template< typename TElement >
	bool TIntrList< TElement >::isUsed() const
	{
		return m_pHead != NULL;
	}
	template< typename TElement >
	axls_size_t TIntrList< TElement >::num() const
	{
		axls_size_t n = 0;

		for( TIntrLink< TElement > *p = m_pHead; p != NULL; p = p->m_pNext ) {
			++n;
		}

		return n;
	}

	template< typename TElement >
	void TIntrList< TElement >::addHead( TIntrLink< TElement > &link )
	{
		if( &link == m_pHead ) {
			return;
		}

		if( m_pHead != NULL ) {
			insertBefore( link, *m_pHead );
			return;
		}

		link.unlink();
		m_pHead = &link;
		m_pTail = &link;
		link.m_pList = this;
	}
	template< typename TElement >
	void TIntrList< TElement >::addTail( TIntrLink< TElement > &link )
	{
		if( &link == m_pTail ) {
			return;
		}

		if( m_pTail != NULL ) {
			insertAfter( link, *m_pTail );
			return;
		}

		link.unlink();
		m_pHead = &link;
		m_pTail = &link;
		link.m_pList = this;
	}
	template< typename TElement >
	void TIntrList< TElement >::insertBefore( TIntrLink< TElement > &link, TIntrLink< TElement > &before )
	{
		link.unlink();

		link.m_pPrev = before.m_pPrev;
		if( before.m_pPrev != NULL ) {
			before.m_pPrev->m_pNext = &link;
		} else {
			m_pHead = &link;
		}
		before.m_pPrev = &link;
		link.m_pNext = &before;

		link.m_pList = this;
	}
	template< typename TElement >
	void TIntrList< TElement >::insertAfter( TIntrLink< TElement > &link, TIntrLink< TElement > &after )
	{
		link.unlink();

		link.m_pNext = after.m_pNext;
		if( after.m_pNext != NULL ) {
			after.m_pNext->m_pPrev = &link;
		} else {
			m_pTail = &link;
		}
		after.m_pNext = &link;
		link.m_pPrev = &after;

		link.m_pList = this;
	}

	template< typename TElement >
	void TIntrList< TElement >::sort( FnComparator pfnCompare )
	{
		TIntrLink< TElement > *pNode;
		TIntrLink< TElement > *pTemp;
		size_t cSwaps;

		if( !pfnCompare ) {
			return;
		}

		//
		//	TERRIBLE IMPLEMENTATION
		//	TODO: Use a better sorting algorithm
		//
		do {
			cSwaps = 0;

			for( pNode = m_pHead; pNode != NULL; pNode = pNode->m_pNext ) {
				if( !pNode->m_pNode || !pNode->m_pNext || !pNode->m_pNext->m_pNode ) {
					continue;
				}

				if( pfnCompare( *pNode->m_pNode, *pNode->m_pNext->m_pNode ) <= 0 ) {
					continue;
				}

				pTemp = pNode->m_pNext;
				pTemp->unlink();
				pTemp->m_pList = this;

				pTemp->m_pPrev = pNode != NULL ? pNode->m_pPrev : m_pTail;
				pTemp->m_pNext = pNode;

				if( pTemp->m_pPrev != NULL ) {
					pTemp->m_pPrev->m_pNext = pTemp;
				} else {
					m_pHead = pTemp;
				}

				if( pNode != NULL ) {
					pNode->m_pPrev = pTemp;
				} else {
					m_pTail = pTemp;
				}

				++cSwaps;
			}
		} while( cSwaps > 0 );
	}


	/*
	===========================================================================

		LIST

	===========================================================================
	*/

	template< typename TElement, typename TAllocator >
	TList< TElement, TAllocator >::TList()
	: m_list()
	{
	}
	template< typename TElement, typename TAllocator >
	TList< TElement, TAllocator >::TList( const TList &ls )
	: m_list()
	{
		for( Iterator x = ls.begin(); x != ls.end(); ++x ) {
			addTail( *x );
		}
	}
	template< typename TElement, typename TAllocator >
	TList< TElement, TAllocator >::TList( SizeType cItems, Type *pItems )
	: m_list()
	{
		if( !pItems ) {
			return;
		}

		for( SizeType i = 0; i < cItems; ++i ) {
			addTail( pItems[ i ] );
		}
	}
	template< typename TElement, typename TAllocator >
	TList< TElement, TAllocator >::~TList()
	{
		clear();
	}

	template< typename TElement, typename TAllocator >
	void TList< TElement, TAllocator >::clear()
	{
		while( isUsed() ) {
			remove( begin() );
		}
	}
	template< typename TElement, typename TAllocator >
	bool TList< TElement, TAllocator >::isEmpty() const
	{
		return m_list.isEmpty();
	}
	template< typename TElement, typename TAllocator >
	bool TList< TElement, TAllocator >::isUsed() const
	{
		return m_list.isUsed();
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::SizeType TList< TElement, TAllocator >::num() const
	{
		return m_list.num();
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::SizeType TList< TElement, TAllocator >::len() const
	{
		return m_list.num();
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::AllocSizeType TList< TElement, TAllocator >::memSize() const
	{
		static AXLS_CONSTEXPR AllocSizeType kElementSize = sizeof( IntrLink ) + sizeof( Type );

		return sizeof( *this ) + m_list.num()*kElementSize;
	}

	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::addHead()
	{
		IntrLink *const pItem = alloc_();
		if( !pItem ) {
			return end();
		}

		m_list.addHead( *pItem );
		return Iterator( pItem );
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::addTail()
	{
		IntrLink *const pItem = alloc_();
		if( !pItem ) {
			return end();
		}

		m_list.addTail( *pItem );
		return Iterator( pItem );
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::insertBefore( Iterator x )
	{
		if( !x ) {
			return addTail();
		}

		IntrLink *const pItem = alloc_();
		if( !pItem ) {
			return end();
		}

		m_list.insertBefore( *pItem, *x.link );
		return Iterator( pItem );
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::insertAfter( Iterator x )
	{
		if( !x ) {
			return addTail();
		}

		IntrLink *const pItem = alloc_();
		if( !pItem ) {
			return end();
		}

		m_list.insertAfter( *pItem, *x.link );
		return Iterator( pItem );
	}

	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::addHead( const Type &element )
	{
		IntrLink *const pItem = alloc_( element );
		if( !pItem ) {
			return end();
		}

		m_list.addHead( *pItem );
		return Iterator( pItem );
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::addTail( const Type &element )
	{
		IntrLink *const pItem = alloc_( element );
		if( !pItem ) {
			return end();
		}

		m_list.addTail( *pItem );
		return Iterator( pItem );
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::insertBefore( Iterator x, const Type &element )
	{
		if( !x ) {
			return addTail( element );
		}

		IntrLink *const pItem = alloc_( element );
		if( !pItem ) {
			return end();
		}

		m_list.insertBefore( *pItem, *x.link );
		return Iterator( pItem );
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::insertAfter( Iterator x, const Type &element )
	{
		if( !x ) {
			return addTail( element );
		}

		IntrLink *const pItem = alloc_( element );
		if( !pItem ) {
			return end();
		}

		m_list.insertAfter( *pItem, *x.link );
		return Iterator( pItem );
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::ConstIterator TList< TElement, TAllocator >::find_const( const Type &item ) const
	{
		const IntrLink *p;

		for( p = m_list.headLink(); p != NULL; p = p->nextLink() ) {
			if( *p->node() == item ) {
				return ConstIterator( p );
			}
		}

		return end();
	}

	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::Iterator TList< TElement, TAllocator >::remove( Iterator iter )
	{
		if( !iter ) {
			return end();
		}

		Iterator next = Iterator( iter.pLink->nextLink() );
		m_list.unlink( *iter.pLink );
		dealloc_( iter.pLink );
		return next;
	}

	template< typename TElement, typename TAllocator >
	void TList< TElement, TAllocator >::sort( FnComparator pfnCompare )
	{
		m_list.sort( pfnCompare );
	}
	template< typename TElement, typename TAllocator >
	void TList< TElement, TAllocator >::sort()
	{
		m_list.sort();
	}

	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::IntrLink *TList< TElement, TAllocator >::alloc_()
	{
		static AXLS_CONSTEXPR AllocSizeType kElementSize = sizeof( IntrLink ) + sizeof( Type );

		char *const p = reinterpret_cast< char * >( TAllocator::allocate( kElementSize ) );
		if( !p ) {
			return NULL;
		}

		IntrLink *const a = ( IntrLink * )( p );
		Type *const b = ( Type * )( p + sizeof( IntrLink ) );

		AX_CONSTRUCT(*a) IntrLink( b );
		AX_CONSTRUCT(*b) Type();

		return a;
	}
	template< typename TElement, typename TAllocator >
	typename TList< TElement, TAllocator >::IntrLink *TList< TElement, TAllocator >::alloc_( const Type &element )
	{
		static AXLS_CONSTEXPR AllocSizeType kElementSize = sizeof( IntrLink ) + sizeof( Type );

		char *const p = reinterpret_cast< char * >( TAllocator::allocate( kElementSize ) );
		if( !p ) {
			return NULL;
		}

		IntrLink *const a = ( IntrLink * )( p );
		Type *const b = ( Type * )( p + sizeof( IntrLink ) );

		AX_CONSTRUCT(*a) IntrLink( b );
		AX_CONSTRUCT(*b) Type( element );

		return a;
	}
	template< typename TElement, typename TAllocator >
	void TList< TElement, TAllocator >::dealloc_( IntrLink *ptr )
	{
		static AXLS_CONSTEXPR AllocSizeType kElementSize = sizeof( IntrLink ) + sizeof( Type );

		if( !ptr ) {
			return;
		}

		if( ptr->node() != NULL ) {
			ptr->node()->~Type();
		}

		TAllocator::deallocate( reinterpret_cast< void * >( ptr ), kElementSize );
	}

}

#endif
