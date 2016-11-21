AxLib
=====

A set of public domain libraries for various purposes.

Each file contains the entirety of the library. Read the individual files for
more information.

Most libraries are self-contained. (i.e., they don't depend on anything else.)

Also, check out Sean Barrett's STB public domain single-header-file library as
well! https://github.com/nothings/stb/


Libraries
=========

ax_array
--------
Dynamic array library for C++11. The `Ax::TMutArr<>` class allows easy
exception-free manipulation of arrays. Unlike most other libraries here, this
one does not require an `AXARR_IMPLEMENTATION`-style definition anywhere.


ax_assert
---------
Flexible (can override and configure) assert, verify, and expect interface.
`AX_ASSERT()` only evaluates in debug and test builds. `AX_EXPECT()` is always
evaluated. `AX_VERIFY()` is like the prior two but can be used within if
statements as it will return.


ax_config
---------
Configuration file support, partially INI styled, with a unique tagging and
command system with a flexible log reporting interface.


ax_fiber
--------
Implements cross-platform (Windows and UNIX) support for fibers. The Windows
version uses the standard OS fiber functions, while the UNIX version makes use
of ucontext to implement them. This programming interface is minimal.


ax_intdatetime
--------------
Has `AX_INT_DATE` and `AX_INT_TIME` macros, giving you the compilation date and
time in integer format. e.g., December 11th, 2014 is represented as `141211`
(2014-12-11) by `AX_INT_DATE`.


ax_list
-------
Linked-list and intrusive linked-list library for C++. As with ax_array, this
library does not require an `AXLS_IMPLEMENTATION`-style definition anywhere.


ax_logger
---------
Logging system that supports custom filters/endpoints.


ax_manager
----------
Singleton helper classes. As with ax_array and ax_list, there's no need to
define an `AXMGR_IMPLEMENTATION`-style macro anywhere.


ax_memory
---------
Memory management system with support for a wide range of features. Has
interfaces for allocation listening (for logging/debugging purposes), page
management, hierarchical memory objects, (high performance!) phased allocations,
etc...


ax_platform
-----------
Holds a bunch of macros that help to identify the target architecture and OS, as
well as compiler information. Also includes helper definitions for common
compiler extensions and features.


ax_printf
---------
Implements a wide range of `printf()`-style functionality with several\
formatting extensions, such as arrays (e.g., "%{3}f" will print an array of
three floats), several FreeBSD kernel formatting extensions, and a
`syslog`-style `%m` (with support for specifying your own `errno` value).


ax_string
---------
String library for C and C++. All strings encoded in UTF-8 with various
conversion routines and helper functions. When built with C++ this includes a
string class and extra helper functions.


ax_thread
---------
Atomics, synchronization primitives, and threading. When built with C++ this
includes a range of helper functions and classes.


ax_time
-------
High-precision timing queries and timing conversion functions. When built with
C++ this includes several utility functions and a timer class (`Ax::CTimer`).


ax_types
--------
Standard type definitions.


ax_typetraits
-------------
C++ library for various type traits and minor routines.

`ax_typetraits` depends on `ax_platform` and `ax_types`.

