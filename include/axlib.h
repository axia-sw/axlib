/*

	axlib - public domain
	Last update: 2014-12-12 Aaron Miller


	Header file that includes all the C-only axlibs.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AXLIB_H_
#define INCGUARD_AXLIB_H_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

/*! \def   AX_NO_PRAGMA_ONCE
 *  \brief Optional user-defined macro which will prevent the axlibs from using
 *         `#pragma once`.
 *  \note  The EASTL team has found that `#pragma once` brought them a 3 to 4%
 *         speed up when building their codebase. It is advised that this not be
 *         defined unless absolutely necessary, as GCC, Clang, and MSVC++ all
 *         support `#pragma once`.
 */

/*! \def   AX_NO_INCLUDES
 *  \brief Optional user-defined macro which will prevent axlibs from
 *         automatically including other axlibs that they can interact with on
 *         compilers with `__has_include`.
 */

/*! \def   AX_FUNC
 *  \brief Optional user-defined macro which represents the default "function"
 *         annotation to be used across axlibs.
 *
 *  The "function" annotation can be used to, for example, mark functions for
 *  DLL export.
 *
 *  \note  Each axlib has an equivalent `AX<XXX>_FUNC`, which allows control
 *         over that library's functions, specifically. `AX_FUNC` is used in the
 *         absence of the library-specific equivalent.
 *
 *  \sa AX_CALL
 */

/*! \def   AX_CALL
 *  \brief Optional user-defined macro which represents the default calling
 *         convention to be used across axlibs.
 *
 *  This allows you to make platform-specific optimizations, such as setting all
 *  axlib functions to use `__fastcall` on x86/64. Alternatively, it may be a
 *  necessary setting for interfacing with other languages which might only
 *  support `__cdecl` or `__stdcall`.
 *
 *  \note  Each axlib has an equivalent `AX<XXX>_CALL`, which allows control
 *         over that library's calling conventions, specifically. `AX_CALL` is
 *         used in the absence of the library-specific equivalent.
 *
 *  \sa AX_FUNC
 */

#ifdef AX_IMPLEMENTATION

# ifndef AXLIB_NO_ASSERT
#  define AXASSERT_IMPLEMENTATION
# endif

# ifndef AXLIB_NO_CONF
#  define   AXCONF_IMPLEMENTATION
#endif

# ifndef AXLIB_NO_FIBER
#  define  AXFIBER_IMPLEMENTATION
# endif

# ifndef AXLIB_NO_LOG
#  define    AXLOG_IMPLEMENTATION
# endif

# ifndef AXLIB_NO_MM
#  define     AXMM_IMPLEMENTATION
# endif

# ifndef AXLIB_NO_PRINTF
#  define AXPRINTF_IMPLEMENTATION
# endif

# ifndef AXLIB_NO_STR
#  define    AXSTR_IMPLEMENTATION
# endif

# ifndef AXLIB_NO_THREAD
#  define AXTHREAD_IMPLEMENTATION
# endif

# ifndef AXLIB_NO_TIME
#  define   AXTIME_IMPLEMENTATION
# endif

#endif

/* These libraries are commonly used by others, so include first */
#include "ax_intdatetime.h"

#include "ax_platform.h"
#include "ax_types.h"

#include "ax_printf.h"
#include "ax_string.h"
#include "ax_logger.h"
#include "ax_assert.h"

/* General libraries */
#include "ax_thread.h"
#include "ax_memory.h"

/* System libraries */
#include "ax_time.h"
#include "ax_thread.h"
#include "ax_fiber.h"

/* Utility libraries */
#include "ax_config.h"

#endif
