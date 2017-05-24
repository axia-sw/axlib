#pragma once

#include "axlib.hpp"

#define AX_DICT_ALPHALOWER "a-z"
#define AX_DICT_ALPHAUPPER "A-Z"
#define AX_DICT_DIGITS     "0-9"
#define AX_DICT_MISC       "_"
#define AX_DICT_UNICODE    "+U"

#define AX_DICT_ALPHA      AX_DICT_ALPHALOWER AX_DICT_ALPHAUPPER
#define AX_DICT_ALNUM      AX_DICT_ALPHA      AX_DICT_DIGITS

#define AX_DICT_IDENT      AX_DICT_ALNUM      AX_DICT_MISC

typedef ax_uptr_t          axdict_size_t;
#ifndef axdict_alloc
# define axdict_alloc(N_)  malloc(N_)
#endif
#ifndef axdict_free
# define axdict_free(P_)   free(P_)
#endif

namespace ax
{

	namespace policy
	{

		template< typename TElement >
		struct DictionaryAllocator
		{
			typedef axdict_size_t AllocSizeType;

			inline Void *allocate( AllocSizeType cBytes )
			{
				return axdict_alloc( cBytes );
			}
			inline Void deallocate( Void *pBytes, AllocSizeType cBytes )
			{
				((Void)cBytes);
				axdict_free( pBytes );
			}
		};

	}

	namespace EFindOption { enum Type { ExistingOnly, CreateIfNotExist }; }

	namespace detail
	{

		// The set of characters used to encode a Unicode code-point in the dictionary
#define AX__DICT_UNICODE_ENC_CHARS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_"

		inline U8 generateConvmap( U8( &dstConvmap )[ 256 ], const char *pszAllowed, ECase::Type casing )
		{
			U8 i, k, k2;
			U8 uEntryIndex;

			memset( &dstConvmap[0], 0xFF, sizeof( dstConvmap ) );

			uEntryIndex = 0;
			for( i = 0; i < 255; i++ ) {
				k = ( ( const U8 * )pszAllowed )[ i ];
				if( k == '\0' ) {
					break;
				}

				k2 = k;
				if( pszAllowed[ i + 1 ] == '-' ) {
					++i;

					if( k != '-' ) {
						k2 = ( ( const U8 * )pszAllowed )[ ++i ];
					}
				} else if( k == '+' && pszAllowed[ i + 1 ] == 'U' ) {
					++i;

					AX_ASSERT_MSG( uEntryIndex < 0xFE, "Entry index overflow" );

					dstConvmap[ 0xF3 ] = uEntryIndex++;
					dstConvmap[ 0xF4 ] = uEntryIndex++;

					continue;
				}

				if( casing == ECase::Insensitive ) {
					do {
						if( k >= 'a' && k <= 'z' ) {
							AX_ASSERT_MSG( dstConvmap[ k - 'a' + 'A' ] == 0xFF, "Overwritten entry" );
							dstConvmap[ k - 'a' + 'A' ] = uEntryIndex;
						} else if( k >= 'A' && k <= 'Z' ) {
							AX_ASSERT_MSG( dstConvmap[ k - 'A' + 'a' ] == 0xFF, "Overwritten entry" );
							dstConvmap[ k - 'A' + 'a' ] = uEntryIndex;
						}

						AX_ASSERT_MSG( dstConvmap[ k ] == 0xFF, "Overwritten entry" );
						dstConvmap[ k ] = uEntryIndex++;
					} while( k++ < k2 );
				} else {
					do {
						AX_ASSERT_MSG( dstConvmap[ k ] == 0xFF, "Overwritten entry" );
						dstConvmap[ k ] = uEntryIndex++;
					} while( k++ < k2 );
				}
			}

			return uEntryIndex;
		}

		inline U8 *readKeyChars( U8 *pLookupBuf, UPtr cMaxBuf, Str &key )
		{
			static const U8 pszChars[] = AX__DICT_UNICODE_ENC_CHARS;
			static const unsigned uRadix = sizeof( pszChars ) - 1;

			UPtr i = 0;
			while( !key.isEmpty() && i + 5 < cMaxBuf ) {
				axstr_utf32_t u = key.readChar();

				if( /*u >= 0x00 &&*/ u <= 0x7F ) {
					pLookupBuf[ i++ ] = ( U8 )u;
				} else if( u >= 0x80 && u <= 0xC5DC ) {
					pLookupBuf[ i++ ] = '\xF3';

					for( unsigned j = 0; j < 3; ++j ) {
						pLookupBuf[ i++ ] = pszChars[ u % uRadix ];
						u /= uRadix;
					}

					AX_ASSERT_MSG( u == 0, "Encoding method did not have its radix adjusted" );
				} else if( u >= 0xC5DD && u <= 0x1C98F0 ) {
					pLookupBuf[ i++ ] = '\xF4';

					for( unsigned j = 0; j < 4; ++j ) {
						pLookupBuf[ i++ ] = pszChars[ u % uRadix ];
						u /= uRadix;
					}

					AX_ASSERT_MSG( u == 0, "Encoding method did not have its radix adjusted" );
				} else {
					// Invalid sequence
					return NULL;
				}
			}

			pLookupBuf[ i ] = '\0';

			return pLookupBuf;
		}

	}

	template< typename TElement, typename TPointer = TElement *, typename TAlloc = policy::DictionaryAllocator< TElement > >
	class TDictionary: private TAlloc
	{
	public:
		typedef TDictionary< TElement, TPointer, TAlloc > ThisType;
		typedef TAlloc                                    Allocator;
		typedef typename Allocator::AllocSizeType         AllocSizeType;
		typedef TElement                                  ElementType;
		typedef TPointer                                  PointerType;

		struct SEntry
		{
			SEntry * pEntries;
			TPointer pData;
		};

		inline TDictionary()
		: m_pEntries( nullptr )
		, m_cEntries( 0 )
		{
		}
		inline ~TDictionary()
		{
			fini();
		}

		inline Bool isInitialized() const
		{
			return m_cEntries > 0;
		}
		inline Bool init( const char *pszAllowed, ECase::Type casing = ECase::Sensitive )
		{
			AX_ASSERT( m_cEntries == 0 );
			AX_ASSERT_NOT_NULL( pszAllowed );

			m_cEntries = detail::generateConvmap( m_convmap, pszAllowed, casing );
			AX_ASSERT_MSG( m_cEntries > 0, "Invalid characters in `pszAllowed`" );

			const UPtr cBytes = sizeof( SEntry )*m_cEntries;

			m_pEntries = ( SEntry * )Allocator::allocate( cBytes );
			if( !AX_VERIFY_NOT_NULL( m_pEntries ) ) {
				m_cEntries = 0;
				return false;
			}

			memset( ( void * )m_pEntries, 0, cBytes );
			return true;
		}
		inline Void fini()
		{
			if( !isInitialized() ) {
				return;
			}

			purge();

			Allocator::deallocate( reinterpret_cast< Void * >( m_pEntries ), sizeof( SEntry )*m_cEntries );
			m_pEntries = nullptr;
			m_cEntries = 0;
		}
		inline Void purge()
		{
			AX_ASSERT( isInitialized() );

			for( U8 i = 0; i < m_cEntries; ++i ) {
				deallocEntries( m_pEntries[ i ], m_cEntries );
				m_pEntries[ i ].pData = nullptr;
			}
		}

		inline SEntry *find( const Str &key ) const
		{
			AX_ASSERT( isInitialized() );

			return const_cast< ThisType * >( this )->findFromEntry( const_cast< SEntry * >( m_pEntries ), key, EFindOption::ExistingOnly );
		}
		inline SEntry *lookup( const Str &key )
		{
			AX_ASSERT( isInitialized() );

			return findFromEntry( m_pEntries, key, EFindOption::CreateIfNotExist );
		}

		inline SEntry *findFrom( const Str &key, SEntry &entry ) const
		{
			AX_ASSERT( isInitialized() );

			if( !entry.pEntries ) {
				return nullptr;
			}

			return const_cast< ThisType * >( this )->findFromEntry( const_cast< SEntry * >( entry.pEntries ), key, EFindOption::ExistingOnly );
		}
		inline SEntry *lookupFrom( const Str &key, SEntry &entry )
		{
			AX_ASSERT( isInitialized() );

			if( !allocEntries( entry, m_cEntries ) ) {
				return nullptr;
			}

			return findFromEntry( entry.pEntries, key, EFindOption::CreateIfNotExist );
		}

		inline Bool isValidChar( char ch ) const
		{
			return m_convmap[ U8( ch ) ] != 0xFF;
		}

	private:
		SEntry *m_pEntries;

		U8      m_convmap[ 256 ];
		U8      m_cEntries;

		inline SEntry *findFromEntry( SEntry *pEntries, const Str &key, EFindOption::Type Opt )
		{
			AX_ASSERT_NOT_NULL( m_pEntries );
			AX_ASSERT_NOT_NULL( pEntries );
			AX_ASSERT( !key.isEmpty() );
			AX_ASSERT( m_cEntries > 0 );

			U8 lookupBuf[ 128 ];

			SEntry *pResEntries = pEntries;
			Str workingKey = key;
			while( !workingKey.isEmpty() ) {
				/*register*/ const U8 *pLookup =
					detail::readKeyChars( lookupBuf, sizeof( lookupBuf ), workingKey );
				if( !pLookup ) {
					return nullptr;
				}

				while( *pLookup != '\0' ) {
					const U8 i = m_convmap[ *pLookup++ ];
					if( i == 0xFF ) {
						// Invalid sequence
						return nullptr;
					}

					if( *pLookup == '\0' && workingKey.isEmpty() ) {
						pResEntries = &pResEntries[ i ];
						break;
					}

					if( !pResEntries[ i ].pEntries ) {
						if( Opt != EFindOption::CreateIfNotExist ) {
							return nullptr;
						}

						if( !allocEntries( pResEntries[ i ], m_cEntries ) ) {
							// TODO: Indicate failure because out of memory
							return nullptr;
						}
					}

					pResEntries = pResEntries[ i ].pEntries;
				}
			}

			return pResEntries;
		}

		inline SEntry *allocEntries( SEntry &from, U8 cEntries )
		{
			if( from.pEntries != nullptr ) {
				return from.pEntries;
			}

			const UPtr cBytes = sizeof( SEntry )*cEntries;

			from.pEntries = reinterpret_cast< SEntry * >( Allocator::allocate( cBytes ) );
			if( !AX_VERIFY_MEMORY( from.pEntries ) ) {
				return nullptr;
			}

			memset( reinterpret_cast< Void * >( from.pEntries ), 0, cBytes );
			return from.pEntries;
		}
		inline NullPtr deallocEntries( SEntry &from, U8 cEntries )
		{
			const UPtr cBytes = sizeof( SEntry )*cEntries;

			if( !from.pEntries ) {
				return NULL;
			}

			for( U8 i = 0; i < cEntries; ++i ) {
				deallocEntries( from.pEntries[ i ], cEntries );
			}

			Allocator::deallocate( reinterpret_cast< Void * >( from.pEntries ), cBytes );
			return nullptr;
		}

		AX_DELETE_COPYFUNCS( TDictionary );
	};

	typedef TDictionary< Void > CVoidDictionary;

}
