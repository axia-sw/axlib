/*

	axlib - public domain
	Last update: 2014-12-12 Aaron Miller


	Header file that includes all the C/C++ axlibs.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AXLIB_HPP_
#define INCGUARD_AXLIB_HPP_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

/* Include all the C-only libraries */
#include "axlib.h"

/* All the C++ libraries */
#include "ax_typetraits.hpp"
#include "ax_manager.hpp"
#include "ax_list.hpp"
#include "ax_array.hpp"

/* Unofficial supplemental axlibs */
#include "ax_dictionary.hpp"

#endif
