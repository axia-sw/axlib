/*

	ax_indexmap - public domain
	last update: 2021-11-21 Aaron Miller


	This library provides a container that maps an arbitrary key to an array
	index a la id Software's "Hash Index."


	USAGE
	=====

	The "index map" is an acceleration structure for looking up values based on
	some hash. It does not contain everything in one structure. It does not deal
	with hash collisions (directly), either. You would use an index map in
	conjunction with a separate array. When adding elements to that array you
	would then add corresponding hashes and indexes to the index map.

	Adding a value:

		ax_index_map_t g_book_hash;
		Book **        g_books;

		Book *new_book(const char *title) {
			size_t             index = array_append_and_return_last_index(g_books);
			ax_index_map_key_t key   = some_hash_function( title );

			// We use the index of the new item we're adding to the g_books
			// array as the value we're mapping the hash to.
			ax_index_map_append( &g_book_hash, key, (ax_index_map_value_t)index );
			return g_books[ index ];
		}

		-** C++ version **-

		struct {
			IndexMap       map;
			TMutArr<Book*> arr;
		} g_books;

		Book *new_book( Str title ) {
			g_books.arr.append(new Book(title));
			g_books.map.append(title.murmur3(), g_books.arr.lastIndex());
			return g_books.arr.last();
		}

	Finding a value:

		ax_index_map_t g_book_hash;
		Book **        g_books;

		Book *get_book(const char *title) {
			ax_index_map_key_t   key = some_hash_function( title );
			ax_index_map_value_t i   = ax_index_map_begin(&g_book_hash, key);
			while( i != AX_INDEX_MAP_INVALID ) {
				Book *book = g_books[i];

				// We must resolve collisions ourselves. One way to do this is
				// to simply check if the unhashed keys match what's stored in
				// the companion array (g_books).
				if( book != NULL && strcmp( book->title, title ) == 0 ) {
					return book;
				}

				i = ax_index_map_next(&g_book_hash, i);
			}

			return NULL;
		}

		-** C++ version **-

		struct {
			IndexMap       map;
			TMutArr<Book*> arr;
		} g_books;

		Book *get_book(Str title) {
			for(IndexMap::Index i = g_books.map.first(title.murmur3()); i != AX_INDEX_MAP_INVALID; i = g_books.map.next(i)) {
				Book *const book = g_books.arr[i];
				if( !book ) {
					continue;
				}

				if( book->title == title ) {
					return book;
				}
			}

			return nullptr;
		}

	Finding a value, but easier:

		Book *get_book(const char *title) {
			ax_index_map_key_t key = some_hash_function( title );

			AX_INDEX_MAP_FOR(&g_book_hash, key, i) {
				Book *book = g_books[i];

				if( book != NULL && strcmp( book->title, title ) == 0 ) {
					return book;
				}
			}

			return NULL;
		}

		-** C++ version **-

		Book *get_book(Str title) {
			for(IndexMap::Index i : g_books.map.query(title)) {
				Book *book = g_books.arr[i];
				
				if( book != nullptr && book->title == title ) {
					return book;
				}
			}

			return nullptr;
		}


	INTERACTIONS
	============

	This library will use ax_platform definitions if they are available. To use
	them include ax_platform.h before including this header.

	This library will use ax_types, ax_assert, and/or ax_memory if they've been
	included prior to this header.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_INDEXMAP_H_
#define INCGUARD_AX_INDEXMAP_H_

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
# if __has_include( "ax_memory.h" )
#  include "ax_memory.h"
# endif
#endif

#ifdef AXIM_IMPLEMENTATION
# define AXIM_IMPLEMENT             1
#else
# define AXIM_IMPLEMENT             0
#endif

#ifndef AXIM_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXIM_CXX_ENABLED          AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXIM_CXX_ENABLED          1
# else
#  define AXIM_CXX_ENABLED          0
# endif
#endif
#ifndef __cplusplus
# undef AXIM_CXX_ENABLED
# define AXIM_CXX_ENABLED           0
#endif

#ifndef AXIM__ENTER_C
# ifdef __cplusplus
#  define AXIM__ENTER_C             extern "C" {
# else
#  define AXIM__ENTER_C
# endif
#endif
#ifndef AXIM__LEAVE_C
# ifdef __cplusplus
#  define AXIM__LEAVE_C             }
# else
#  define AXIM__LEAVE_C
# endif
#endif

#ifndef AXIM_DEFPARM
# ifdef __cplusplus
#  define AXIM_DEFPARM(X_)          = (X_)
# else
#  define AXIM_DEFPARM(X_)
# endif
#endif

#ifndef AXIM_FUNC
# ifdef AX_FUNC
#  define AXIM_FUNC                 AX_FUNC
# else
#  define AXIM_FUNC                 extern
# endif
#endif
#ifndef AXIM_CALL
# ifdef AX_CALL
#  define AXIM_CALL                 AX_CALL
# else
#  define AXIM_CALL
# endif
#endif

#ifndef AX_INDEX_MAP_DEBUG_ENABLED
# if defined(AX_DEBUG_ENABLED)
#  define AX_INDEX_MAP_DEBUG_ENABLED AX_DEBUG_ENABLED
# elif defined(_DEBUG)
#  define AX_INDEX_MAP_DEBUG_ENABLED 1
# else
#  define AX_INDEX_MAP_DEBUG_ENABLED 0
# endif
#endif

#ifndef AX_INDEX_MAP_ASSERT
# if defined(AX_ASSERT)
#  define AX_INDEX_MAP_ASSERT(Expr) AX_ASSERT(Expr)
# elif defined(assert)
#  define AX_INDEX_MAP_ASSERT(Expr) assert(Expr)
# else
#  define AX_INDEX_MAP_ASSERT(Expr) ((void)0)
# endif
#endif

#ifndef AX_INDEX_MAP_INTERNAL_ASSERT
# define AX_INDEX_MAP_INTERNAL_ASSERT(Expr) AX_INDEX_MAP_ASSERT(Expr)
#endif

#ifndef AX_INDEX_MAP_PRECONDITION_ASSERT
# if AX_INDEX_MAP_DEBUG_ENABLED
#  define AX_INDEX_MAP_PRECONDITION_ASSERT(Expr) AX_INDEX_MAP_ASSERT(Expr)
# else
#  define AX_INDEX_MAP_PRECONDITION_ASSERT(Expr) ((void)0)
# endif
#endif

#ifndef axim_alloc
# ifdef axmm_def_alloc
#  define axim_alloc(N_)                 (axmm_def_alloc((N_)))
#  define axim_free(P_)                  (axmm_def_free((P_)))
# else
#  include <stdlib.h>
#  define axim_alloc(N_)                 (malloc((N_)))
#  define axim_free(P_)                  (free((P_)))
# endif
#endif
#ifndef axim_free
# error Must define axim_free() when defining axim_alloc()
#endif

#ifndef axim_memcpy
# ifndef AXIM_NO_MEMCPY
#  include <string.h>
#  define axim_memcpy(DstP_,SrcP_,BytesN_) (memcpy((DstP_),(SrcP_),(BytesN_)))
# endif
#endif
#ifndef axim_memset
# ifndef AXIM_NO_MEMSET
#  include <string.h>
#  define axim_memset(DstP_,SrcByte_,BytesN_) (memset((DstP_),(SrcByte_),(BytesN_)))
# endif
#endif

#ifndef AX_INDEX_MAP_DEFAULT_SIZE
# define AX_INDEX_MAP_DEFAULT_SIZE        512
#endif

#ifndef AX_INDEX_MAP_DEFAULT_GRANULARITY
# define AX_INDEX_MAP_DEFAULT_GRANULARITY 32
#endif

#ifndef AX_INDEX_MAP_TYPES_DEFINED
# define AX_INDEX_MAP_TYPES_DEFINED 1
# ifdef AX_TYPES_DEFINED
typedef ax_u32_t  ax_index_map_key_t;
typedef ax_size_t ax_index_map_value_t;
# else
typedef unsigned int ax_index_map_key_t;
typedef unsigned int ax_index_map_value_t;
# endif
#endif

#if AX_TYPES_DEFINED
typedef ax_size_t axim_size_t;
#else
# include <stddef.h>
typedef size_t    axim_size_t;
#endif

#define AX_INDEX_MAP_INVALID (~(ax_index_map_value_t)0)

typedef struct ax_index_map_s {
	/*
		The initial array.

		head_arr[key] is either an index to the user-specified array, an index
		into link_arr[], or INDEX_MAP_INVALID.
	*/
	axim_size_t           head_len;
	ax_index_map_value_t *head_arr;

	/*
		The link array.

		link_arr[i] is either an index to the user-specified array, an index to
		the next link in this array, or INDEX_MAP_INVALID.
	*/
	axim_size_t           link_len;
	ax_index_map_value_t *link_arr;
} ax_index_map_t;


#ifndef axim_memcpy
AXIM_FUNC void AXIM_CALL axim_memcpy(void *pDst, const void *pSrc, axim_size_t cBytes)
#if AXIM_IMPLEMENT
{
	union {
		unsigned char *p8;
		axim_size_t   *psz;
		axim_size_t    n;
		void *         p;
	} dst;
	union {
		const unsigned char *p8;
		const axim_size_t   *psz;
		axim_size_t          n;
		const void *         p;
	} src;

	/* FIXME: If the operation is sufficiently large, use SIMD loads/stores */

	/* prepare the copy operation */
	dst.p = pDst;
	src.p = pSrc;

	/* align to `axstr_size_t` if possible */
	if( dst.n%sizeof(axstr_size_t) == src.n%sizeof(axim_size_t) && cBytes>=sizeof(axim_size_t) ) {
		cBytes -= dst.n%sizeof(axim_size_t);
		while( dst.n%sizeof(axim_size_t) != 0 ) {
			*dst.p8++ = *src.p8++;
		}
	}

	/* copy in `axstr_size_t`-sized chunks (should be 4 or 8 bytes at a time) */
	while( cBytes >= sizeof(axim_size_t) ) {
		*dst.psz++ = *src.psz++;
		cBytes -= sizeof(axim_size_t);
	}

	/* copy the remaining bytes */
	while( cBytes > 0 ) {
		*dst.p8++ = *src.p8++;
		--cBytes;
	}
}
#else
;
#endif
#endif
#ifndef axim_memset
AXIM_FUNC void AXIM_CALL axim_memset( void *pDst, int srcByteInt, axim_size_t cBytes )
#if AXIM_IMPLEMENT
{
	union {
		unsigned char *p8;
		axim_size_t   *psz;
		axim_size_t    n;
		void          *p;
	} dst, end;
	union {
		unsigned char b8[sizeof(axim_size_t)];
		unsigned char b;
		axim_size_t   sz;
	} src;

	dst.p  = pDst;
	end.p8 = dst.p8 + cBytes;

	// Format the source word to be a repetition of the byte given
	// e.g., 0xFF -> 0xFFFFFFFFFFFFFFFF
	do {
		axim_size_t i;

		for( i = 0; i < sizeof(src.b8); ++i ) {
			src.b8[i] = (unsigned char)(srcByteInt & 0xFF);
		}
	} while(0);

	// While not aligned to word boundary, write bytes
	while( dst.p != end.p && dst.n % sizeof(axim_size_t) != 0 ) {
		*dst.p8++ = src.b;
	}

	// Write whole words while we have entire words to write
	while( dst.n + sizeof(axim_size_t) <= end.n ) {
		*dst.psz++ = src.sz;
	}

	// Write all remaining bytes
	while( dst.p != end.p ) {
		*dst.p8++ = src.b;
	}
}
#else
;
#endif
#endif

#if AXIM_IMPLEMENT
/*
	NOTE: To avoid an unnecessary allocation and NULL checks, a dummy array
	`     is used when nothing has been inserted into the structure. That
	`     same dummy array is used between both head_arr and link_arr.
	`
	`     To determine whether the dummy array is in use, compare head_arr
	`     to link_arr. If they are the same, the dummy array is in use. If
	`     they differ, then they are using the heap allocated array.
*/
static ax_index_map_value_t axim__g_index_map_dummy_arr[1] = { AX_INDEX_MAP_INVALID };

#define AXIM__IS_DUMMY_ARRAY() (hi->head_arr == hi->link_arr)
#define AXIM__MASK_FOR_ARRAY() (AXIM__IS_DUMMY_ARRAY() ? (ax_index_map_value_t)0 : ~(ax_index_map_value_t)0)
#define AXIM__MASK_KEY(key)    ((key) & AXIM__MASK_FOR_ARRAY())

static axim_size_t axim__align_len( axim_size_t len, axim_size_t alignment ) {
	return len + ( alignment - len%alignment )%alignment;
}
static void axim__set_invalid( ax_index_map_value_t *array, axim_size_t begin, axim_size_t end ) {
	/* NOTE: INDEX_MAP_INVALID is "-1" which, for every byte, is 0xFF. We can
	`        just memset() the array at the right offset, to 0xFF in order to
	`        invalidate that range of indices. */

	const axim_size_t byte_length = ( end - begin )*sizeof(*array);

	AX_INDEX_MAP_INTERNAL_ASSERT( AX_INDEX_MAP_INVALID == ~(index_map_value_t)0 );
	AX_INDEX_MAP_ASSERT( end >= begin );

	axim_memset( &array[begin], 0xFF, byte_length );
}
#endif

AXIM_FUNC void AXIM_CALL ax_index_map_init( ax_index_map_t *hi, size_t init_len )
#if AXIM_IMPLEMENT
{
	AX_INDEX_MAP_PRECONDITION_ASSERT( hi != (ax_index_map_t*)0 );

	if( !init_len ) {
		init_len = AX_INDEX_MAP_DEFAULT_SIZE;
	}

	hi->head_len = init_len;
	hi->head_arr = &axim__g_index_map_dummy_arr[0];

	hi->link_len = init_len;
	hi->link_arr = &axim__g_index_map_dummy_arr[0];
}
#else
;
#endif

AXIM_FUNC void AXIM_CALL ax_index_map_fini( ax_index_map_t *hi )
#if AXIM_IMPLEMENT
{
	AX_INDEX_MAP_PRECONDITION_ASSERT( hi != (ax_index_map_t*)0 );

	if( !AXIM__IS_DUMMY_ARRAY() ) {
		axim_free( (void*)hi->head_arr );
		axim_free( (void*)hi->link_arr );
	}

	hi->head_len = 0;
	hi->head_arr = &axim__g_index_map_dummy_arr[0];

	hi->link_len = 0;
	hi->link_arr = &axim__g_index_map_dummy_arr[0];
}
#else
;
#endif

#if AXIM_IMPLEMENT
static void ax_index_map_initial_alloc( ax_index_map_t *hi ) {
	AX_INDEX_MAP_INTERNAL_ASSERT( AXIM__IS_DUMMY_ARRAY() && "Cannot perform initial allocation when already allocated!" );
	AX_INDEX_MAP_INTERNAL_ASSERT( hi->head_len > 0 );
	AX_INDEX_MAP_INTERNAL_ASSERT( hi->link_len > 0 );

	hi->head_arr = (ax_index_map_value_t *)axim_alloc( sizeof(ax_index_map_value_t)*hi->head_len );
	AX_INDEX_MAP_ASSERT( hi->head_arr != NULL && "Failed to allocate Index_Map head array" );

	hi->link_arr = (ax_index_map_value_t *)axim_alloc( sizeof(ax_index_map_value_t)*hi->link_len );
	AX_INDEX_MAP_ASSERT( hi->link_arr != NULL && "Failed to allocate Index_Map link array" );

	axim__set_invalid( hi->head_arr, 0, hi->head_len );
	axim__set_invalid( hi->link_arr, 0, hi->link_len );
}
static void index_map_resize_links( ax_index_map_t *hi, axim_size_t new_len ) {
	ax_index_map_value_t *new_arr;

	AX_INDEX_MAP_INTERNAL_ASSERT( !AXIM__IS_DUMMY_ARRAY() && "Cannot resize link table when unallocated" );

	if( new_len <= hi->link_len ) {
		return;
	}

	new_len = axim__align_len( new_len, INDEX_MAP_DEFAULT_GRANULARITY );

	new_arr = (ax_index_map_value_t *)axim_alloc( sizeof(ax_index_map_value_t)*new_len );
	AX_INDEX_MAP_ASSERT( new_arr != NULL && "Failed to allocate new Index_Map link array" );

	axim_memcpy( (void*)new_arr, (const void *)hi->link_arr, sizeof(ax_index_map_value_t)*hi->link_len );
	axim__set_invalid( new_arr, hi->link_len, new_len );

	axim_free( (void *)hi->link_arr );

	hi->link_len = new_len;
	hi->link_arr = new_arr;
}
#endif

AXIM_FUNC void AXIM_CALL ax_index_map_append( ax_index_map_t *hi, ax_index_map_key_t key, ax_index_map_value_t value )
#if AXIM_IMPLEMENT
{
	ax_index_map_key_t k;

	AX_INDEX_MAP_PRECONDITION_ASSERT( hi != NULL );
	AX_INDEX_MAP_PRECONDITION_ASSERT( value != AX_INDEX_MAP_INVALID );

	if( AXIM__IS_DUMMY_ARRAY() ) {
		if( hi->link_len <= value ) {
			hi->link_len = ax__align_len( value + 1, AX_INDEX_MAP_DEFAULT_GRANULARITY );
		}

		ax_index_map_initial_alloc( hi );
	} else if( hi->link_len <= value ) {
		ax_index_map_resize_links( hi, value + 1 );
	}

	AX_INDEX_MAP_INTERNAL_ASSERT( hi->head_len > 0 );

	k = key % hi->head_len;

	hi->link_arr[ value ] = hi->head_arr[ k ];
	hi->head_arr[ k ]     = value;
}
#else
;
#endif

AXIM_FUNC void AXIM_CALL ax_index_map_remove( ax_index_map_t *hi, ax_index_map_key_t key, ax_index_map_value_t value )
#if AXIM_IMPLEMENT
{
	ax_index_map_key_t k;

	AX_INDEX_MAP_PRECONDITION_ASSERT( hi != (ax_index_map_t *)0 );
	AX_INDEX_MAP_INTERNAL_ASSERT( value < hi->link_len );

	AX_INDEX_MAP_INTERNAL_ASSERT( hi->head_len > 0 );

	k = key % hi->head_len;

	if( hi->head_arr[k] == value ) {
		hi->head_arr[k] = hi->link_arr[ value ];
	} else {
		size_t i;

		for( i = hi->head_arr[k]; i != AX_INDEX_MAP_INVALID; i = hi->link_arr[i] ) {
			if( hi->link_arr[i] == value ) {
				hi->link_arr[i] = hi->link_arr[value];
				break;
			}
		}
	}

	hi->link_arr[value] = AX_INDEX_MAP_INVALID;
}
#else
;
#endif

#define AX_INDEX_MAP_FOR(IndexMapPtr, Key, Iterator) \
	for( \
		ax_index_map_value_t Iterator = ax_index_map_begin((IndexMapPtr), (Key)); \
		(Iterator) != AX_INDEX_MAP_INVALID; \
		Iterator = ax_index_map_next((HashIndexPtr), (Iterator)) \
	)

AXIM_FUNC ax_index_map_value_t AXIM_CALL ax_index_map_begin( const ax_index_map_t *hi, ax_index_map_key_t key )
#if AXIM_IMPLEMENT
{
	AX_INDEX_MAP_PRECONDITION_ASSERT( hi != (const ax_index_map_t *)0 );
	AX_INDEX_MAP_INTERNAL_ASSERT( hi->head_len > 0 );

	return hi->head_arr[ AXIM__MASK_KEY(key)%hi->head_len ];
}
#else
;
#endif
AXIM_FUNC ax_index_map_value_t AXIM_CALL ax_index_map_next( const ax_index_map_t *hi, ax_index_map_value_t index )
#if AXIM_IMPLEMENT
{
	AX_INDEX_MAP_PRECONDITION_ASSERT( hi != (const ax_index_map_t *)0 );
	AX_INDEX_MAP_ASSERT( index >= 0 && index < hi->link_len );

	return hi->link_arr[ AXIM__MASK_KEY(index) ];
}
#else
;
#endif


#if AXIM_CXX_ENABLED
namespace ax
{

class IndexMap
{
public:
	ax_index_map_t map;

	typedef ax_index_map_key_t   Key;
	typedef ax_index_map_value_t Index;

	typedef axim_size_t SizeType;

	// Proxy iterator used to support ranged for loops.
	// This is only used to support C++ madness.
	class Iter {
		const ax_index_map_t &m_map;
		ax_index_map_value_t  m_index;
	
	public:
		Iter( const ax_index_map_t &map, ax_index_map_value_t i ): m_map(map), m_index(i) {}
		Iter( const Iter &i ): m_map(i.m_map), m_index(i.m_index) {}
		~Iter() {}

		inline operator ax_index_map_value_t() const { return m_index; }
		inline Iter &operator=(ax_index_map_value_t i) { m_index = i; return *this; }
		inline Iter &operator=(const Iter &i) { m_index = i.m_index; return *this; }

		inline Iter operator++(int) {
			Iter old(m_map, m_index);
			if( m_index != AX_INDEX_MAP_INVALID ) {
				m_index = ax_index_map_next( &m_map, m_index );
			}
			return *this;
		}
		inline Iter &operator++() {
			if( m_index != AX_INDEX_MAP_INVALID ) {
				m_index = ax_index_map_next( &m_map, m_index );
			}
			return *this;
		}

		inline ax_index_map_value_t operator*() const {
			return m_index;
		}

		inline bool operator==( const Iter &i ) const {
			return &m_map == &i.m_map && m_index == i.m_index;
		}
		inline bool operator!=( const Iter &i ) const {
			return &m_map != &i.m_map || m_index != i.m_index;
		}
	}; //class IndexMap::Iter

	// Proxy object used to support ranged for loops.
	// This is only used to forward the right parameters and calls.
	class Query {
		const ax_index_map_t       &m_map;
		const ax_index_map_value_t  m_first;
	
	public:
		Query( const ax_index_map_t &map, ax_index_map_key_t key )
		: m_map( map ), m_first(ax_index_map_begin( &map, key ))
		{
		}
		Query( const Query &q )
		: m_map( q.m_map ), m_first( q.m_first )
		{
		}
		~Query() {}

		inline Iter begin() const {
			return Iter( m_map, m_first );
		}
		inline Iter end() const {
			return Iter( m_map, AX_INDEX_MAP_INVALID );
		}
	}; //class IndexMap::Query

	inline IndexMap()
	: map()
	{
		ax_index_map_init( &map, AX_INDEX_MAP_DEFAULT_SIZE );
	}
	inline IndexMap( SizeType size )
	: map()
	{
		ax_index_map_init( &map, size );
	}
	inline ~IndexMap()
	{
		ax_index_map_fini( &map );
	}

	inline void append( Key key, Index index ) {
		return ax_index_map_append( &map, key, index );
	}
	inline void remove( Key key, Index index ) {
		return ax_index_map_remove( &map, key, index );
	}

	// Use this in range-based for loops; e.g., for(Index i : map.query(key)) { ... }
	inline Query query( Key key ) const {
		return Query( map, key );
	}

	inline Index first( Key key ) const {
		return ax_index_map_begin( &map, key );
	}
	inline Index next( Index index ) const {
		return ax_index_map_next( &map, index );
	}
}; //class IndexMap

} //namespace ax
#endif //#ifdef AXIM_CXX_ENABLED

#endif
