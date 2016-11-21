/*

	ax_manager - public domain
	Last update: 2015-10-12 Aaron Miller


	EXAMPLE USAGE
	=============

	namespace ax
	{
		enum class EFileAccess
		{
			Read,
			Write,
			ReadWrite,
			Append
		};
		class MFileSystem
		{
		public:
			static MFileSystem &get();

			class RFile *openFile( const Str &path, EFileAccess access );

		private:
			MFileSystem();
		};
		static TManager< MFileSystem > fileSystem;

		...

		MFileSystem &MFileSystem::get()
		{
			static MFileSystem instance;
			return instance;
		}

		...

		RFile *openToRead( const Str &path )
		{
			return fileSystem->openFile( path, EFileAccess::Read );
		}
	}


	INTERACTIONS
	============

	This library will use ax_platform definitions if they are available. To use
	them include ax_platform.h before including this header.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

/*! \file  ax_manager.hpp
 *  \brief Provides a set of singleton-like classes.
 *
 *  `ax::TManager` can be used if initialization should be managed implicitly.
	\code{.cpp}
	namespace ax
	{
		enum class EFileAccess
		{
			Read,
			Write,
			ReadWrite,
			Append
		};
		class MFileSystem
		{
		public:
			static MFileSystem &get();

			class RFile *openFile( const Str &path, EFileAccess access );

		private:
			MFileSystem();
		};
		static TManager< MFileSystem > fileSystem;

		...

		MFileSystem &MFileSystem::get()
		{
			static MFileSystem instance;
			return instance;
		}

		...

		RFile *openToRead( const Str &path )
		{
			return fileSystem->openFile( path, EFileAccess::Read );
		}
	}
	\endcode
 *  In the above, `fileSystem` would be initialized automatically on its first
 *  use.
 *
 *  If you're fine with initializing and deinitializing explicitly, then you can
 *  use `ax::TTypeBuf` for a bit more performance.
	\code
	namespace ax
	{
		enum class EFileAccess
		{
			Read,
			Write,
			ReadWrite,
			Append
		};
		class MFileSystem
		{
		friend class TTypeBuf< MFileSystem >;
		public:
			class RFile *openFile( const Str &path, EFileAccess access );

		private:
			MFileSystem();
		};
		extern TTypeBuf< MFileSystem > fileSystem;

		...

		// in some .cpp file
		TTypeBuf< MFileSystem > fileSystem;

		...

		// explicit initialization
		fileSystem.init();

		...

		RFile *openToRead( const Str &path )
		{
			AX_ASSERT( fileSystem.isInitialized() && "Uninitialized!" );
			return fileSystem->openFile( path, EFileAccess::Read );
		}
	}
	\endcode
 */

#ifndef INCGUARD_AX_MANAGER_HPP_
#define INCGUARD_AX_MANAGER_HPP_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

#ifndef AXMGR_INLINE
# ifdef AX_FORCEINLINE
#  define AXMGR_INLINE				AX_FORCEINLINE
# else
#  define AXMGR_INLINE				inline
# endif
#endif

#ifndef AXMGR_TYPES_DEFINED
# define AXMGR_TYPES_DEFINED        1
# if AX_TYPES_DEFINED
typedef ax_uptr_t                   axmgr_size_t;
# else
#  include <stddef.h>
typedef size_t                      axmgr_size_t;
# endif
#endif

#ifndef AX_CONSTRUCT
# define AX_CONSTRUCT(X_)\
	::new(reinterpret_cast<void*>(&(X_)),::ax::detail::SPlcmntNw())
namespace ax { namespace detail { struct SPlcmntNw {}; } }
inline void *operator new( axmgr_size_t, void *p, ax::detail::SPlcmntNw )
{
	return p;
}
inline void operator delete( void *, void *, ax::detail::SPlcmntNw )
{
}
#endif

namespace ax
{

	/*! \brief Singleton manager class, with implicit (de)initialization. */
	template< typename tSingleton >
	class TManager
	{
	public:
		AXMGR_INLINE tSingleton *operator->()
		{
			return &tSingleton::get();
		}
		AXMGR_INLINE const tSingleton *operator->() const
		{
			return &tSingleton::get();
		}

		AXMGR_INLINE tSingleton &operator*()
		{
			return tSingleton::get();
		}
		AXMGR_INLINE const tSingleton &operator*() const
		{
			return tSingleton::get();
		}
	};

	/*! \brief Management class with explicit initialization. Ensures type is
	 *         only initialized when desired.
	 */
	template< typename T >
	class TTypeBuf
	{
	public:
		AXMGR_INLINE TTypeBuf()
		: m_valid( false )
		{
		}
		AXMGR_INLINE ~TTypeBuf()
		{
			fini();
		}

		AXMGR_INLINE T &init()
		{
			if( !initValid() ) {
				AX_CONSTRUCT(getRefConst()) T();
			}
			
			return getRefConst();
		}
		AXMGR_INLINE T &init( const T &x )
		{
			if( !initValid() ) {
				AX_CONSTRUCT(getRefConst()) T( x );
			}

			return getRefConst();
		}
		template< typename T1 >
		AXMGR_INLINE T &init( T1 x1 )
		{
			if( !initValid() ) {
				AX_CONSTRUCT(getRefConst()) T( x1 );
			}

			return getRefConst();
		}
		template< typename T1, typename T2 >
		AXMGR_INLINE T &init( T1 x1, T2 x2 )
		{
			if( !initValid() ) {
				AX_CONSTRUCT(getRefConst()) T( x1, x2 );
			}

			return getRefConst();
		}
		template< typename T1, typename T2, typename T3 >
		AXMGR_INLINE T &init( T1 x1, T2 x2, T3 x3 )
		{
			if( !initValid() ) {
				AX_CONSTRUCT(getRefConst()) T( x1, x2, x3 );
			}

			return getRefConst();
		}
		template< typename T1, typename T2, typename T3, typename T4 >
		AXMGR_INLINE T &init( T1 x1, T2 x2, T3 x3, T4 x4 )
		{
			if( !initValid() ) {
				AX_CONSTRUCT(getRefConst()) T( x1, x2, x3, x4 );
			}

			return getRefConst();
		}
		template< typename T1, typename T2, typename T3, typename T4, typename T5 >
		AXMGR_INLINE T &init( T1 x1, T2 x2, T3 x3, T4 x4, T5 x5 )
		{
			if( !initValid() ) {
				AX_CONSTRUCT(getRefConst()) T( x1, x2, x3, x4, x5 );
			}

			return getRefConst();
		}
		template< typename T1, typename T2, typename T3, typename T4, typename T5, typename T6 >
		AXMGR_INLINE T &init( T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6 )
		{
			if( !initValid() ) {
				AX_CONSTRUCT(getRefConst()) T( x1, x2, x3, x4, x5, x6 );
			}

			return getRefConst();
		}
		AXMGR_INLINE void fini()
		{
			if( !m_valid ) {
				return;
			}

			getRefConst().~T();
			m_valid = false;
		}

		AXMGR_INLINE bool isInitialized() const
		{
			return m_valid;
		}

		AXMGR_INLINE T &ref()
		{
			return getRefConst();
		}
		AXMGR_INLINE const T &ref() const
		{
			return getRefConst();
		}

		AXMGR_INLINE T *ptr()
		{
			return &getRefConst();
		}
		AXMGR_INLINE const T *ptr() const
		{
			return &getRefConst();
		}

		AXMGR_INLINE T *operator->()
		{
			return &getRefConst();
		}
		AXMGR_INLINE const T *operator->() const
		{
			return &getRefConst();
		}

	private:
		unsigned char m_data[ sizeof( T ) ];
		bool          m_valid;

		AXMGR_INLINE bool initValid()
		{
			if( m_valid ) {
				return true;
			}

			m_valid = true;
			return false;
		}
		AXMGR_INLINE T &getRefConst() const
		{
			return const_cast< T & >( reinterpret_cast< const T & >( m_data[0] ) );
		}
	};

}

#endif
