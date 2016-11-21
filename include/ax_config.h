/*

	ax_config - public domain
	Last update: 2015-10-01 Aaron Miller


	This library provides a configuration file system.


	CONFIGURATION CONCEPTS
	======================

	Tags: These allow you to load up a bunch of configurations and quickly
	`     discard any that do not match what you're looking for. In essence,
	`     configurations have the option of stating which tags they are a part
	`     of, which tags they cannot be imported with, and adding or removing
	`     tags of their own. This is for when a simple hierarchical directory
	`     approach is not robust enough.

	Contexts: Stores information such as the active set of tags, variables, etc.
	`         When a configuration is loaded you have the option of modifying an
	`         existing context or inheriting another context and working from
	`         there.

	Data: Represents values / information.

	Variables: Names, potentially associated with data.

	Commands: Represents an action to be done. Commands can take parameters.

	Sections: Much like INI files, sections can be used to partition variables
	`         and commands. Sections are completely isolated from one-another.


	CONFIGURATION FILE FORMAT / LANGUAGE SYNTAX
	===========================================

	// C++-style (single-line) and C-style (multi-line) comments are supported.
	// C-style (block) comments can nest.
	# Hash single-line comments are allowed.
	; Semicolon single-line comments are allowed.

	The beginning of the file consists of tags.

		*tagname ← This config presents <tagname> as an active tag
		~tagname ← This config presents <tagname> as an inactive tag
		+tagname ← This config requires <tagname> to be active
		-tagname ← This config requires <tagname> to be inactive

	Multiple tags can be given within a given tag token:

		// All of 'debug' and 'x86' are required
		+debug +x86

		// Any of 'debug' or 'test' are required
		+debug,test

		// The first of these tags that are inactive shall be made active
		*included-once,included-twice,included-thrice

		// The first of these tags that are active shall be made inactive
		~nested-thrice,nested-twice,nested-once

	Another way of looking at "active" and "inactive" would be "defined" and
	"undefined." So '*apples ~oranges +fruit' at the beginning of the file would
	say "this configuration's tag is 'apples', and if 'oranges' was an active
	tag then deactivate it, but only do any of this if 'fruit' is an active
	tag."

	Variables are declared by setting them to some value. When they are set,
	they receive the type of the value they are set to.

	There are different ways to set a variable:

		varname = value     ← Weak form.
		The variable will be set to the given value. If the variable already
		exists then the value will be implicitly cast to the type of the
		variable.

		varname := value    ← Strong form.
		As with the weak form, the variable will be set to the given value.
		However, the variable cannot exist prior to this command.

		varname += value    ← Appending form.
		Appends data to the variable. If the variable doesn't exist then this
		acts like the weak/strong assignment. The meaning of "append" depends on
		the data type.

			Boolean: Appending "false" does nothing. Appending "true" sets the
			`        boolean to true.

			Number: Sets to the sum of the current value of the variable with
			`       the given value.

			String: Concatenates the variable's current contents with the given
			`       string value. If the content of the string preceding the
			`       concatenation does not have a trailing space (0x20 ' ') then
			`       a space will be appended to the end of the string prior to
			`       this operation.

			Blob: Same as with the string, except does not check the data for a
			`     trailing space character.

			Array/List: Adds elements to the end of the list.

		varname .= value    ← Unique appending form.
		As with the normal appending form. However, this only works with string
		and array/list data types.

			String: Breaks the given string into separate tokens (quoted values
			`       within strings will be checked for escapes and treated as a
			`       whole token). For each token, check for a matching token in
			`       the variable. If no such token is found, then perform a
			`       normal append operation of the given token onto the
			`       variable. e.g.,
			`
			`           cflags  = "-W -Wall -pedantic"
			`           cflags .= "-Wall -std=gnu++1 -g"
			`
			`       Would result in cflags being equal to
			`       "-W -Wall -pedantic-std=gnu++11 -g"

			Array/List: For each element of the given array, check the appendee
			`           for an equivalent element. If no equivalent element
			`           exists then append the given element to the array. e.g.,
			`
			`               numbers  = [ 7, 3, 9 ]
			`               numbers .= [ 3, 1, 5 ]
			`
			`           Would result in numbers being equal to [ 7, 3, 9, 1, 5 ]

		varname ?= value    ← Optional form.
		As with the weak-form assignment when the variable doesn't exist. If the
		variable does exist, then this is a no-op and the variable remains
		untouched. The value on the right will only be evaluated if the
		assignment will occur.

	Values (data) can be represented in several forms:

	* Boolean (e.g., false, true). There are only two acceptable values for
	` booleans. (False/0, or true/1.)

	* Number (e.g., 123, 3.14159265, 1e-6, 0xFE, 0b11010011). These are simple
	` numeric values.

	* String (e.g., "Hello, world!"). The following escape sequences are
	` supported:
	`  - \a (produces byte 0x07 - alert)
	`  - \b (produces byte 0x08 - backspace)
	`  - \f (produces byte 0x0C - form-feed)
	`  - \n (produces byte 0x0A - line-feed)
	`  - \r (produces byte 0x0D - carriage-return)
	`  - \t (produces byte 0x09 - tab)
	`  - \v (produces byte 0x0B - vertical-tab)
	:: TODO: Byte \xXX ::
	:: TODO: Unicode \uXXXX \u(X+) ::

	* Blob (e.g., < FF FF 00 2A FE FD FC FB DCBAFACEBEEFC0DE >). All data is
	` represented byte-wise. Each pair of hexadecimal values represents a single
	` byte.

	* Array/List (e.g., [ "1", "2", "3" ]). Elements do not have to be the same
	` type.

	* Null type (e.g., nil). (Represents nothing.)

	Commands can be invoked in the following manner:

		command-name arg1 arg2 ... argN

	Commands can be given as arguments to other commands like so:

		command-name arg1 (other-command arg1 arg2 ... argN) arg2 ... argN

	Commands can have return values. The special variable $store represents the
	return value of the last command whose return value was not explicitly
	assigned. The type of $store matches the return type of that command.

	Variables can be set to the result of commands in either of the following
	syntaxes:

		varname assign-op command-name arg1 arg2 ... argN
		varname assign-op (command-name arg1 arg2 ... argN)

	Variables can also be interpreted as commands which evaluate to the value of
	the variable. Arrays/lists are dereferenced by passing an argument
	representing the index to dereference. For example:

		// Create the array
		myarr = [ 221, 42, 23, 15532, 15498 ]

		// Inspect the individual elements of the array
		baker-street         = myarr 0 // Evaluates to the number 221
		answer-to-everything = myarr 1 // Evaluates to the number 42
		jim-carry            = myarr 2 // Evaluates to the number 23
		endless-eight:a      = myarr 3 // Evaluates to the number 15532
		endless-eight:n      = myarr 4 // Evaluates to the number 15498

	Variables can be removed.

		// Remove the variable "varname" from the current section
		-varname

		// Remove the variable "varname" from the current section if it matches
		// the given value
		-varname=value

	Variables may be given metadata.

		varname("description")
		varname("description", "help text")

	Sections are specified with an opening pair of square brackets with a name
	in the middle, like so:

		// Set section to "section name"
		[section name]

		// Create a new section from an existing section
		[extended section]:[base section]

		// Create a new section from a section specified in a variable of
		// another section
		[extended section]:[other section](keyname)
		[extended section]:[other section](keyname=value)

		// Add another section of the same name
		+[section name]

		// Remove an existing section by name
		-[section name]
		-[section name](keyname)
		-[section name](keyname=value)

		// Continue adding keys to the first seection found
		>[section name]
		>[section name](keyname)
		>[section name](keyname=value)

		// Continue adding keys to the last section found
		<[section name]
		<[section name](keyname)
		<[section name](keyname=value)

		// Select the global section
		[]


	INTERACTIONS
	============

	This library will use ax_platform definitions if they are available. To use
	them include ax_platform.h before including this header.

	This library will use ax_types if it has been included prior to this header.

	This library will use ax_string if it has been included prior to this
	header. If ax_string is not used then configurations must be UTF-8 encoded.
	Otherwise they can also be encoded with UTF-16 LE/BE or UTF-32 LE/BE.


	LICENSE
	=======

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_CONFIG_H_
#define INCGUARD_AX_CONFIG_H_

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
# if __has_include( "ax_memory.h" )
#  include "ax_memory.h"
# endif
# if __has_include( "ax_string.h" )
#  include "ax_string.h"
# endif
# if __has_include( "ax_logger.h" )
#  include "ax_logger.h"
# endif
#endif

#ifdef AXCONF_IMPLEMENTATION
# define AXCONF_IMPLEMENT           1
#else
# define AXCONF_IMPLEMENT           0
#endif

#ifndef AXCONF_CXX_ENABLED
# ifdef AX_CXX_ENABLED
#  define AXCONF_CXX_ENABLED        AX_CXX_ENABLED
# elif defined( __cplusplus )
#  define AXCONF_CXX_ENABLED        1
# else
#  define AXCONF_CXX_ENABLED        0
# endif
#endif
#ifndef __cplusplus
# undef AXCONF_CXX_ENABLED
# define AXCONF_CXX_ENABLED         0
#endif

#ifndef AXCONF_TYPES_DEFINED
# ifdef AX_TYPES_DEFINED
typedef ax_s64_t                    axconf_s64_t;
typedef ax_u64_t                    axconf_u64_t;
typedef ax_uptr_t                   axconf_size_t;
typedef double                      axconf_real_t;
# else
#  include <stdint.h>
#  include <stddef.h>
typedef int64_t                     axconf_s64_t;
typedef uint64_t                    axconf_u64_t;
typedef size_t                      axconf_size_t;
typedef double                      axconf_real_t;
# endif
# define AXCONF_TYPES_DEFINED       1
#endif

#ifndef axconf_alloc
# include <stdlib.h>
# define axconf_alloc               malloc
# define axconf_free                free
#endif

#ifndef axconf_buf_alloc
# define axconf_buf_alloc           axconf_alloc
# define axconf_buf_free            axconf_free
#endif

#ifndef axconf_tok_alloc
# define axconf_tok_alloc           axconf_alloc
# define axconf_tok_free            axconf_free
#endif

#ifndef axconf_prc_alloc
# define axconf_prc_alloc           axconf_alloc
# define axconf_prc_free            axconf_free
#endif

#ifndef axconf_memcpy
# include <string.h>
# define axconf_memcpy              memcpy
#endif
#ifndef axconf_memchr
# include <string.h>
# define axconf_memchr              memchr
#endif
#ifndef axconf_strlen
# ifdef INCGUARD_AX_STRING_H_
#  define axconf_strlen             axstr_len
# else
#  include <string.h>
#  define axconf_strlen             strlen
# endif
#endif

#ifndef AXCONF_ASSERT
# ifdef AX_ASSERT
#  define AXCONF_ASSERT             AX_ASSERT
# else
#  define AXCONF_ASSERT(Expr_)      ((void)0)
# endif
#endif

#ifndef AXCONF_FUNC
# ifdef AX_FUNC
#  define AXCONF_FUNC               AX_FUNC
# else
#  define AXCONF_FUNC               extern
# endif
#endif
#ifndef AXCONF_CALL
# ifdef AX_CALL
#  define AXCONF_CALL               AX_CALL
# else
#  define AXCONF_CALL
# endif
#endif

#ifndef AXCONF_ENTER_C
# ifdef __cplusplus
#  define AXCONF_ENTER_C            extern "C" {
#  define AXCONF_LEAVE_C            }
# else
#  define AXCONF_ENTER_C
#  define AXCONF_LEAVE_C
# endif
#endif

AXCONF_ENTER_C

typedef enum axconf_token_type_e
{
	kAxconfTok_Invalid = -1L,
	kAxconfTok_EOF = 0,

	kAxconfTok_LBracket = '[',
	kAxconfTok_RBracket = ']',
	kAxconfTok_LBrace = '{',
	kAxconfTok_RBrace = '}',
	kAxconfTok_LParen = '(',
	kAxconfTok_RParen = ')',
	kAxconfTok_Lt = '<',
	kAxconfTok_Gt = '>',
	kAxconfTok_LE = '<'*256 + '=',
	kAxconfTok_GE = '>'*256 + '=',
	kAxconfTok_Eq = '='*256 + '=',
	kAxconfTok_NE = '!'*256 + '=',
	kAxconfTok_Assign = '=',
	kAxconfTok_AddAssign = '+'*256 + '=',
	kAxconfTok_UniqueAssign = '.'*256 + '=',
	kAxconfTok_StrongAssign = ':'*256 + '=',
	kAxconfTok_OptionalAssign = '?'*256 + '=',

	kAxconfTok_Tag = 0x10000L,
	kAxconfTok_Name,
	kAxconfTok_String,
	kAxconfTok_Number
} axconf_token_type_t;

typedef enum axconf_token_flag_e
{
	kAxconfTokF_Start               = 1<<0,
	kAxconfTokF_Processed           = 1<<1,
	kAxconfTokF_FileStart           = 1<<2,
	kAxconfTokF_Directive           = 1<<3,

	kAxconfTokF_Overflowed          = 1<<4,

	kAxconfTokF_Number_Signed       = 1<<12,
	kAxconfTokF_Number_Float        = 1<<13
} axconf_token_flag_t;

typedef enum axconf_value_type_e
{
	/* Invalid value type -- possibly unset */
	kAxconfValTy_Invalid,

	/* Boolean (0 or 1 / false or true / no or yes) value */
	kAxconfValTy_Boolean,
	/* Signed integer (64-bit range) */
	kAxconfValTy_SignedInteger,
	/* Unsigned integer (64-bit range) */
	kAxconfValTy_UnsignedInteger,
	/* Floating-point number (stored as a special encoding) */
	kAxconfValTy_Float,
	/* Collection of text values */
	kAxconfValTy_String,
	/* Single binary blob of data */
	kAxconfValTy_Blob
} axconf_value_type_t;

typedef enum axconf_severity_e
{
	/* Fatal error that cannot be recovered from (such as "out of memory") */
	kAxconfSev_Panic,
	/* Error that prevents a stable configuration from being generated */
	kAxconfSev_Error,
	/* Potentially unwanted action detected */
	kAxconfSev_Warning,
	/* Something that can be done better, potentially (optimization) */
	kAxconfSev_Remark,
	/* A general message */
	kAxconfSev_Normal,
	/* A verbose message (usually extra details that aren't necessary) */
	kAxconfSev_Verbose,
	/* Special severity used to cancel the generation of a report */
	kAxconfSev_Silent
} axconf_severity_t;

typedef enum axconf_messageid_e
{
	/* Ran out of memory while allocating %1 byte%s1 */
	kAxconfMsg_OutOfMemory          = 0,
	/* Exiting because the limit of %1 error%s1 was reached */
	kAxconfMsg_TooManyErrors        = 1,

	/* Invalid character '%1' */
	kAxconfMsg_Lexer_InvalidToken   = 100,
	/* Number is too large */
	kAxconfMsg_Lexer_Overflow       = 101,
	/* Multi-line comment never closes */
	kAxconfMsg_Lexer_OpenComment    = 102,
	/* Invalid escape sequence '%1' in string */
	kAxconfMsg_Lexer_InvalidEscape  = 103
} axconf_messageid_t;

typedef struct axconf_stringref_s
{
	/* Number of characters in the string */
	int                             n;
	/* Pointer to the characters of the string -- # **NO** NUL terminator! # */
	const char *                    s;
} axconf_stringref_t;

typedef struct axconf_lineinfo_s
{
	/* Pointer to the name of the file */
	const char *                    pszFilename;
	/* Line number, or zero */
	unsigned                        uLine;
	/* Column number, or zero */
	unsigned                        uColumn;
	/* Text of the line */
	axconf_stringref_t              LineRef;
} axconf_lineinfo_t;

struct axconf_s;
struct axconf_report_s;
typedef struct axconf_report_s
{
	/* Pointer to the configuration that generated this report */
	struct axconf_s *               pConfig;
	/* Where the report occurred in the file, if relevant */
	axconf_lineinfo_t               Location;
	/* The severity of the report (see axconf_severity_t) */
	axconf_severity_t               Severity;
	/* Identifier of the message -- useful for localization */
	axconf_messageid_t              MessageId;
	/* Default string for the message -- en-US, use MessageId to localize */
	const char *                    pszMessage;
	/* Number of valid arguments to the message string */
	unsigned                        cArgs;
	/* The arguments passed -- Args >= cArgs will be a valid blank string */
#define AXCONF_MAX_REPORT_ARGS      4
	axconf_stringref_t              Args[ AXCONF_MAX_REPORT_ARGS ];

	/* Pointer to the report before this (NULL if no report precedes) */
	struct axconf_report_s *        pPrevReport;
	/* Pointer to the report after this (NULL if no report follows) */
	struct axconf_report_s *        pNextReport;
} axconf_report_t;

typedef struct axconf_token_s
{
	axconf_token_type_t             type;
	unsigned                        uFlags;
	const char *                    pLexanS;
	const char *                    pLexanE;
	union {
		void *                      p;
		char *                      pszEscaped;
		axconf_s64_t                iValue;
		axconf_u64_t                uValue;
		axconf_real_t               fValue;
	}                               processed;
	void *                          pOwnedMem;
} axconf_token_t;

typedef struct axconf_token_link_s
{
	axconf_token_t                  tok;
	struct axconf_token_link_s *    l_prev;
	struct axconf_token_link_s *    l_next;
} axconf_token_link_t;

typedef struct axconf_float_value_s
{
	axconf_s64_t                    iWhole;
	unsigned                        uFract;
	int                             iExp;
} axconf_float_value_t;

typedef struct axconf_blob_value_s
{
	axconf_size_t                   cBytes;
	unsigned char *                 pBytes;
} axconf_blob_value_t;

struct axconf_var_s;
struct axconf_value_link_s;
typedef struct axconf_value_link_s
{
	/* Variable owning this value */
	struct axconf_var_s *           pVar;

	/* Previous link in the variable's list */
	struct axconf_value_link_s *    l_prev;
	/* Next link in the variable's list */
	struct axconf_value_link_s *    l_next;

	/* Data for this value */
	union {
		/* Boolean value (0 or 1) */
		unsigned                    b;
		/* Signed integer value */
		axconf_s64_t                i;
		/* Unsigned integer value */
		axconf_u64_t                u;
		/* Floating-point value */
		axconf_float_value_t        f;
		/* NUL-terminated string value */
		char *                      psz;
		/* Binary blob value */
		axconf_blob_value_t         bin;
	}                               Data;
} axconf_value_link_t;

struct axconf_section_s;
struct axconf_var_s;
typedef struct axconf_var_s
{
	/* Name of this variable */
	char *                          pszName;

	/* Type for the value */
	axconf_value_type_t             ValueTy;

	/* Number of values held */
	axconf_size_t                   cValues;
	/* First value in this variable's list */
	struct axconf_value_link_s *    l_head;
	/* Last value in this variable's list */
	struct axconf_value_link_s *    l_tail;

	/* Section we belong to */
	struct axconf_section_s *       pSection;

	/* Previous variable in the list */
	struct axconf_var_s *           v_prev;
	/* Next variable in the list */
	struct axconf_var_s *           v_next;
} axconf_var_t;

struct axconf_context_s;
struct axconf_section_s;
typedef struct axconf_section_s
{
	/* Name of this section */
	char *                          pszName;

	/* Context we belong to */
	struct axconf_context_s *       pContext;

	/* Prior section */
	struct axconf_section_s *       s_prev;
	/* Next section */
	struct axconf_section_s *       s_next;
	/* First variable */
	axconf_var_t *                  v_head;
	/* Last variable */
	axconf_var_t *                  v_tail;
} axconf_section_t;

struct axconf_s;
struct axconf_context_s;
typedef struct axconf_context_s
{
	/* Context we're inheriting from */
	struct axconf_context_s *       ctx_prnt;
	/* Prior sibling context in ctx_prnt's list */
	struct axconf_context_s *       ctx_prev;
	/* Next sibling context in ctx_prnt's list */
	struct axconf_context_s *       ctx_next;
	/* First child context in our list */
	struct axconf_context_s *       ctx_head;
	/* Last child context in our list */
	struct axconf_context_s *       ctx_tail;

	/* First section in this context */
	axconf_section_t *              s_head;
	/* Last section in this context */
	axconf_section_t *              s_tail;

	/* Number of configurations this context affects */
	axconf_size_t                   cConfigs;
	/* Configurations that have affected this context */
	struct axconf_s **              ppConfigs;
} axconf_context_t;

/* Primary configuration structure -- Most operations occur on this */
typedef struct axconf_s
{
	/* Name of the file loaded */
	char *                          pszFilename;
	/* Pointer to the start of the buffer (this can be free()'d) */
	char *                          buf_s;
	/* Pointer to the end of the buffer (the NUL terminator) */
	const char *                    buf_e;
	/* First token in the list */
	axconf_token_link_t *           l_head;
	/* Last token in the list */
	axconf_token_link_t *           l_tail;
	/* Current token (needed for unlex support) */
	axconf_token_link_t *           l_curr;
	/* First report generated */
	axconf_report_t *               r_head;
	/* Last report generated */
	axconf_report_t *               r_tail;
	/* Out-of-memory report -- if generated this is fatal */
	axconf_report_t                 r_outOfMemory;
	/* Out-of-memory temporary buffer -- used with r_outOfMemory */
	char                            r_temp[ 128 ];
	/* Out-of-memory temporary buffer index -- linear allocation position */
	axconf_size_t                   r_temp_i;
	/* Maximum number of error reports to generate before terminating */
	unsigned                        cMaxErrors;
	/* Current number of error reports generated */
	unsigned                        cErrors;
	/* Current number of warning reports generated */
	unsigned                        cWarnings;
	/* How to classify warnings (e.g., warnings as errors, or "shut up") */
	axconf_severity_t               WarningSev;
	/* Minimum severity to generate for */
	axconf_severity_t               MinimumSev;
	/* Context that this config affects */
	axconf_context_t *              pContext;
} axconf_t;


/*
===============================================================================
###############################################################################
===============================================================================

	GENERAL CONFIGURATION UTILITY FUNCTIONS

===============================================================================
###############################################################################
===============================================================================
*/

#if AXCONF_IMPLEMENT
static axconf_stringref_t *AXCONF_CALL axconf__numtostringref( axconf_stringref_t *pDstRef, char *pDstBuf, axconf_size_t cDstBytes, axconf_size_t *pDstIndex, axconf_u64_t uValue )
{
	axconf_size_t cBufBytes;
	char Buf[ 32 ];
	char *pBuf, *pBufE;

	pBufE = &Buf[ sizeof( Buf ) ];
	pBuf = pBufE;

	do {
		*--pBuf = '0' + ( char )( uValue%10 );
		uValue /= 10;
	} while( uValue > 0 );

	cBufBytes = ( axconf_size_t )( pBufE - pBuf );

	if( *pDstIndex + cBufBytes > cDstBytes ) {
		pDstRef->n = 0;
		pDstRef->s = "";
		return pDstRef;
	}

	axconf_memcpy( ( void * )( pDstBuf + *pDstIndex ), ( const void * )pBuf, cBufBytes );

	pDstRef->n = (int)cBufBytes;
	pDstRef->s = pDstBuf + *pDstIndex;

	*pDstIndex += cBufBytes;

	return pDstRef;
}
static axconf_stringref_t *AXCONF_CALL axconf__toktostringref( axconf_stringref_t *pDstRef, const axconf_token_t *pTok )
{
	pDstRef->n = ( int )( axconf_size_t )( pTok->pLexanE - pTok->pLexanS );
	pDstRef->s = pTok->pLexanS;

	return pDstRef;
}

static int AXCONF_CALL axconf__sa( char *pszDst, axconf_size_t cDstBytes, char **ppszDst, const char *pSrc, axconf_size_t cSrcBytes )
{
	char *pszDstEnd;

	if( !cSrcBytes ) {
		return 1;
	}

	pszDstEnd = pszDst + cDstBytes - 1;
	if( *ppszDst + cSrcBytes > pszDstEnd ) {
		return 0;
	}

	axconf_memcpy( ( void * )*ppszDst, ( const void * )pSrc, cSrcBytes );
	*ppszDst += cSrcBytes;
	**ppszDst = '\0';

	return 1;
}
static int AXCONF_CALL axconf__saref( char *pszDst, axconf_size_t cDstBytes, char **ppszDst, const axconf_stringref_t *pStrRef )
{
	if( pStrRef->n <= 0 ) {
		return 1;
	}

	return axconf__sa( pszDst, cDstBytes, ppszDst, pStrRef->s, ( axconf_size_t )pStrRef->n );
}
static int AXCONF_CALL axconf__sachr( char *pszDst, axconf_size_t cDstBytes, char **ppszDst, char ch )
{
	return axconf__sa( pszDst, cDstBytes, ppszDst, &ch, 1 );
}
static int AXCONF_CALL axconf__saz( char *pszDst, axconf_size_t cDstBytes, char **ppszDst, const char *pszSrc )
{
	if( !pszSrc ) {
		return 1;
	}

	return axconf__sa( pszDst, cDstBytes, ppszDst, pszSrc, axconf_strlen( pszSrc ) );
}
static int AXCONF_CALL axconf__sanum( char *pszDst, axconf_size_t cDstBytes, char **ppszDst, axconf_u64_t uValue )
{
	axconf_stringref_t NumRef;
	axconf_size_t DstIndex;

	DstIndex = ( axconf_size_t )( *ppszDst - pszDst );

	axconf__numtostringref( &NumRef, pszDst, cDstBytes, &DstIndex, uValue );
	*ppszDst = pszDst + DstIndex;
	if( DstIndex < cDstBytes ) {
		pszDst[ DstIndex ] = '\0';
		return 1;
	}

	pszDst[ cDstBytes - 1 ] = '\0';
	return 0;
}

static const char *AXCONF_CALL axconf__getwordstart( const char *pszBase, const char *pszCurr )
{
	while( pszCurr > pszBase && *( ( const unsigned char * )pszCurr - 1 ) > ' ' ) {
		--pszCurr;
	} 

	return pszCurr;
}
static const char *AXCONF_CALL axconf__skiptoprcnt( const char *p )
{
	while( *p != '\0' ) {
		if( *p == '%' ) {
			return p;
		}

		++p;
	}

	return p;
}
#endif

AXCONF_FUNC const char *AXCONF_CALL axconf_format_msg( char *pszDst, axconf_size_t cDstBytes, const char *pszMessage, unsigned cArgs, const axconf_stringref_t *pArgs )
#if AXCONF_IMPLEMENT
{
	const char *b, *p;
	char *pszOut;

	AXCONF_ASSERT( pszDst != ( char * )0 && "pszDst must be set" );
	AXCONF_ASSERT( cDstBytes >= 1 && "cDstBytes must be given" );
	AXCONF_ASSERT( pszMessage != ( const char * )0 && "pszMessage is needed" );
	AXCONF_ASSERT( ( !cArgs || pArgs != ( const axconf_stringref_t * )0 )
		&& "If cArgs is greater than zero then pArgs must be set" );

	pszOut = pszDst;
	*pszOut = '\0';

	b = pszMessage;
	for(;;) {
		const char *q, *t;
		char seq;
		int isplural, r;

		p = axconf__skiptoprcnt( b );

		isplural = *p != '\0' && ( *( p + 1 ) == 's' || *( p + 1 ) == 'S' );
		q = isplural ? axconf__getwordstart( pszMessage, p ) : p;
		t = isplural && *( p + 1 ) == 'S' ? axconf__skiptoprcnt( p + 2 ) : ( const char * )0;

		if( !axconf__sa( pszDst, cDstBytes, &pszOut, b, ( axconf_size_t )( q - b ) ) ) {
			return ( const char * )0;
		}

		/* End of string? */
		if( *p == '\0' ) {
			break;
		}

		/* if 'isplural' is guaranteed to be either 1 or 0 then: *( p + 1 + isplural ) */
		seq = isplural ? *( p + 2 ) : *( p + 1 );

		if( seq == '%' ) {
			/* Simple escape (%%) */
			r = axconf__sachr( pszDst, cDstBytes, &pszOut, '%' );
		} else if( seq >= '1' && seq <= '9' ) {
			/* Parameter (%<n> -- e.g., %1 ... %9) */
			const axconf_stringref_t *pArg;
			unsigned uArg;

			uArg = +( seq - '1' );
			pArg = uArg < cArgs ? &pArgs[ uArg ] : ( const axconf_stringref_t * )0;

			/* Plural parameter? (%s<n> -- e.g., %1 byte%s1 --or-- %1 puppy%S1puppies%) */
			if( isplural ) {
				if( pArg != ( const axconf_stringref_t * )0 && pArg->n == 1 && *pArg->s == '1' ) {
					r = axconf__sa( pszDst, cDstBytes, &pszOut, q, ( axconf_size_t )( p - q ) );
				} else if( t != ( const char * )0 ) {
					/* Plural of the form: word%S<n>pluralword% */
					r = axconf__sa( pszDst, cDstBytes, &pszOut, p + 3, ( axconf_size_t )( t - ( p + 3 ) ) );
				} else {
					r = axconf__sa( pszDst, cDstBytes, &pszOut, q, ( axconf_size_t )( p - q ) );
					r = r && axconf__sachr( pszDst, cDstBytes, &pszOut, 's' );
				}
			} else {
				r = axconf__saref( pszDst, cDstBytes, &pszOut, pArg );
			}
		} else {
			/* unknown format specifier -- don't die though */
			r = axconf__sa( pszDst, cDstBytes, &pszOut, "[\?\?\?]", 5 );
		}

		p += 2 + ( isplural ? 1 : 0 );

		if( !r ) {
			return ( const char * )0;
		}

		if( t != ( const char * )0 ) {
			p = *t != '\0' ? t + 1 : t;
		}

		b = p;
	}

	return pszDst;
}
#else
;
#endif
AXCONF_FUNC const char *AXCONF_CALL axconf_format_report_msg( char *pszDst, axconf_size_t cDstBytes, const axconf_report_t *pReport )
#if AXCONF_IMPLEMENT
{
	return axconf_format_msg( pszDst, cDstBytes, pReport->pszMessage, pReport->cArgs, &pReport->Args[ 0 ] );
}
#else
;
#endif

AXCONF_FUNC const char *AXCONF_CALL axconf_severity_to_string( axconf_severity_t Severity )
#if AXCONF_IMPLEMENT
{
	switch( Severity )
	{
	case kAxconfSev_Panic:          return "Panic";
	case kAxconfSev_Error:          return "Error";
	case kAxconfSev_Warning:        return "Warning";
	case kAxconfSev_Remark:         return "Remark";
	case kAxconfSev_Normal:         return "Normal";
	case kAxconfSev_Verbose:        return "Verbose";
	case kAxconfSev_Silent:         return "Silent";
	}

	return "(unknown-severity)";
}
#else
;
#endif
AXCONF_FUNC const char *AXCONF_CALL axconf_messageid_to_string( axconf_messageid_t MessageId )
#if AXCONF_IMPLEMENT
{
	switch( MessageId )
	{
	case kAxconfMsg_OutOfMemory:
		return "Ran out of memory while allocating %1 byte%s1";
	case kAxconfMsg_TooManyErrors:
		return "Exiting because the limit of %1 error%s1 was reached";

	case kAxconfMsg_Lexer_InvalidToken:
		return "Invalid character '%1'";
	case kAxconfMsg_Lexer_Overflow:
		return "Number is too large";
	case kAxconfMsg_Lexer_OpenComment:
		return "Multi-line comment never closes";
	case kAxconfMsg_Lexer_InvalidEscape:
		return "Invalid escape sequence '%1' in string";
	}

	return "(unknown-messageid)";
}
#else
;
#endif

#if AXCONF_IMPLEMENT
static char *axconf__strndup( const char *src, axconf_size_t n )
{
	char *dst;

#ifdef _MSC_VER
	// "writable size is 'n+1' bytes, but '1' bytes might be written"
	//
	// If 'n' is  0 then the writable size *is* 1 byte.
	// If 'n' is >0 then the writable size  is >1 byte.
	//
	// This warning is a false positive unless I've missed something.
# pragma warning(push)
# pragma warning(disable:6386)
#endif

	dst = ( char * )( axconf_alloc( n + 1 ) );
	if( !dst ) {
		/* TODO: Out of memory error */
		return ( char * )0;
	}

	if( n > 0 ) {
		axconf_memcpy( ( void * )dst, ( const void * )src, n );
	}
	dst[ n ] = '\0';

#ifdef _MSC_VER
# pragma warning(pop)
#endif

	return dst;
}
static char *axconf__strdup( const char *src )
{
	axconf_size_t n;
	const char *p;

	if( src != ( const char * )0 ) {
		for( p = src; *p != '\0'; ++p ) {
		}
		n = ( axconf_size_t )( p - src );
	} else {
		n = 0;
	}

	return axconf__strndup( src, n );
}

static axconf_size_t *axconf__strmaxptr( const void *p )
{
	return ( ( axconf_size_t * )p ) - 1;
}
static axconf_size_t *axconf__strlenptr( const void *p )
{
	return ( ( axconf_size_t * )p ) - 2;
}

static int axconf__strappend( char **p, const char *s, const char *e )
{
	axconf_size_t dstlen;
	axconf_size_t srclen;

	if( s == e ) {
		return 1;
	}

	dstlen = !*p ? 0 : *axconf__strlenptr( *p );
	srclen = ( axconf_size_t )( e - s );
	if( *p != ( char * )0 && dstlen + srclen < *axconf__strmaxptr( *p ) ) {
		axconf_memcpy( *p + dstlen, ( const void * )s, srclen );
		*( *p + dstlen + srclen ) = '\0';

		*axconf__strlenptr( *p ) = dstlen + srclen;
	} else {
		axconf_size_t cap;
		char *q;

		cap = dstlen + srclen + 1;
		cap += cap%16 != 0 ? 16 - cap%16 : 0;

		q = ( char * )axconf_prc_alloc( cap + sizeof( axconf_size_t )*2 );
		if( !q ) {
			return 0;
		}

		q += sizeof( axconf_size_t )*2;

		*axconf__strmaxptr( q ) = cap;
		*axconf__strlenptr( q ) = dstlen + srclen;

		if( dstlen > 0 ) {
			axconf_memcpy( ( void * )q, ( const void * )*p, dstlen );
			axconf_prc_free( ( void * )( ( ( axconf_size_t * )*p ) - 2 ) );
		}

		axconf_memcpy( ( void * )( q + dstlen ), ( const void * )s, srclen );
		*( q + dstlen + srclen ) = '\0';

		*p = q;
	}

	return 1;
}
static int axconf__strappendch( char **p, char c )
{
	return axconf__strappend( p, &c, ( &c ) + 1 );
}
#endif


/*
===============================================================================
###############################################################################
===============================================================================

	GENERAL CONFIGURATION MANAGEMENT (CORE)

===============================================================================
###############################################################################
===============================================================================
*/

AXCONF_FUNC axconf_t *AXCONF_CALL axconf_init( axconf_t *p )
#if AXCONF_IMPLEMENT
{
	p->pszFilename = ( char * )0;

	p->buf_s = ( char * )0;
	p->buf_e = ( const char * )0;

	p->l_head = ( axconf_token_link_t * )0;
	p->l_tail = ( axconf_token_link_t * )0;
	p->l_curr = ( axconf_token_link_t * )0;

	p->r_head = ( axconf_report_t * )0;
	p->r_tail = ( axconf_report_t * )0;

	p->r_temp_i = 0;

	p->cMaxErrors = ~0U;
	p->cErrors = 0;
	p->cWarnings = 0;
	p->WarningSev = kAxconfSev_Warning;
	p->MinimumSev = kAxconfSev_Normal;

	p->pContext = ( axconf_context_t * )0;

	return p;
}
#else
;
#endif
AXCONF_FUNC axconf_t *AXCONF_CALL axconf_fini( axconf_t *p )
#if AXCONF_IMPLEMENT
{
	axconf_token_link_t *t, *tn;
	axconf_report_t *r, *rn;
	axconf_free( ( void * )p->pszFilename );
	axconf_free( ( void * )p->buf_s );

	p->pszFilename = ( char * )0;
	p->buf_s = ( char * )0;
	p->buf_e = ( const char * )0;

	/* Free each token */
	for( t = p->l_head; t != ( axconf_token_link_t * )0; t = tn ) {
		tn = t->l_next;

		axconf_prc_free( ( void * )t->tok.pOwnedMem );
		axconf_tok_free( ( void * )t );
	}

	p->l_head = ( axconf_token_link_t * )0;
	p->l_tail = ( axconf_token_link_t * )0;
	p->l_curr = ( axconf_token_link_t * )0;

	/* Free each report */
	for( r = p->r_head; r != ( axconf_report_t * )0; r = rn ) {
		rn = r->pNextReport;

		if( r == &p->r_outOfMemory ) {
			continue;
		}

		axconf_free( ( void * )r );
	}

	p->r_head = ( axconf_report_t * )0;
	p->r_tail = ( axconf_report_t * )0;

	/* Remove references of this config from the context */
	if( p->pContext != ( axconf_context_t * )0 ) {
		axconf_size_t i;

		for( i = 0; i < p->pContext->cConfigs; ++i ) {
			if( p->pContext->ppConfigs[ i ] != p ) {
				continue;
			}

			--p->pContext->cConfigs;
			if( i == p->pContext->cConfigs ) {
				continue;
			}

			p->pContext->ppConfigs[ i ] = p->pContext->ppConfigs[ p->pContext->cConfigs ];
		}

		p->pContext = ( axconf_context_t * )0;
	}

	return ( axconf_t * )0;
}
#else
;
#endif

AXCONF_FUNC int AXCONF_CALL axconf_set_filename( axconf_t *p, const char *pszFilename )
#if AXCONF_IMPLEMENT
{
	char *pszNewFilename;

	if( pszFilename != ( const char * )0 ) {
		pszNewFilename = axconf__strdup( pszFilename );
		if( !pszNewFilename ) {
			return 0;
		}
	} else {
		pszNewFilename = ( char * )0;
	}

	axconf_free( ( void * )p->pszFilename );
	p->pszFilename = pszNewFilename;

	return 1;
}
#else
;
#endif
AXCONF_FUNC const char *AXCONF_CALL axconf_get_filename( const axconf_t *p )
#if AXCONF_IMPLEMENT
{
	return p->pszFilename;
}
#else
;
#endif

AXCONF_FUNC int AXCONF_CALL axconf_set_buffer_size( axconf_t *p, axconf_size_t n )
#if AXCONF_IMPLEMENT
{
	char *pBuffer;

	if( n > 0 ) {
		pBuffer = ( char * )axconf_buf_alloc( n );
		if( !pBuffer ) {
			return 0;
		}
	} else {
		pBuffer = ( char * )0;
	}

	axconf_buf_free( ( void * )p->buf_s );
	p->buf_s = pBuffer;
	p->buf_e = pBuffer + n;

	return 1;
}
#else
;
#endif
AXCONF_FUNC int AXCONF_CALL axconf_set_buffer( axconf_t *p, const char *pszBuffer )
#if AXCONF_IMPLEMENT
{
	axconf_size_t n;

	n = pszBuffer != ( const char * )0 ? axconf_strlen( pszBuffer ) : 0;

	if( !axconf_set_buffer_size( p, n ) ) {
		return 0;
	}

	if( n > 0 ) {
		axconf_memcpy( ( void * )p->buf_s, ( const void * )pszBuffer, n );
	}

	return 1;
}
#else
;
#endif
AXCONF_FUNC void AXCONF_CALL axconf_write( axconf_t *p, const char *pszSrcBuffer, axconf_size_t cSrcBytes )
#if AXCONF_IMPLEMENT
{
	AXCONF_ASSERT( p->buf_s != ( char * )0 && "Destination buffer not set" );
	AXCONF_ASSERT( cSrcBytes == ( axconf_size_t )( p->buf_e - p->buf_s )
		&& "Source buffer size does not match destination buffer size" );

	if( cSrcBytes > 0 ) {
		axconf_memcpy( ( void * )p->buf_s, ( const void * )pszSrcBuffer, cSrcBytes );
	}
}
#else
;
#endif

AXCONF_FUNC const char *AXCONF_CALL axconf_get_buffer( const axconf_t *p )
#if AXCONF_IMPLEMENT
{
	return p->buf_s;
}
#else
;
#endif
AXCONF_FUNC const char *AXCONF_CALL axconf_get_buffer_end( const axconf_t *p )
#if AXCONF_IMPLEMENT
{
	return p->buf_e;
}
#else
;
#endif
AXCONF_FUNC axconf_size_t AXCONF_CALL axconf_get_buffer_len( const axconf_t *p )
#if AXCONF_IMPLEMENT
{
	return ( axconf_size_t )( p->buf_e - p->buf_s );
}
#else
;
#endif

#if AXCONF_IMPLEMENT
static const char *axconf__skip_line( const char *s, const char *e )
{
	while( s < e ) {
		int haveline = 0;

		if( *s == '\r' ) {
			haveline = 1;
			++s;
		}
		if( s < e && *s == '\n' ) {
			haveline = 1;
			++s;
		}

		if( haveline ) {
			break;
		}

		if( s < e ) {
			++s;
		}
	}

	return s;
}
#endif

AXCONF_FUNC axconf_lineinfo_t *AXCONF_CALL axconf_get_lineinfo( axconf_lineinfo_t *pOutLineInfo, const axconf_t *p, const axconf_token_t *tok )
#if AXCONF_IMPLEMENT
{
	const char *linep;
	const char *bufp;
	unsigned cLines;

	AXCONF_ASSERT( pOutLineInfo != ( axconf_lineinfo_t * )0 &&
		"Line info buffer must be set" );
	AXCONF_ASSERT( p != ( const axconf_t * )0 && "Configuration must be set" );
	AXCONF_ASSERT( tok != ( const axconf_token_t * )0 && "Token must be set" );
	AXCONF_ASSERT( tok->pLexanS != ( const char * )0 && "Invalid token" );

	pOutLineInfo->pszFilename = p->pszFilename;

	cLines = 0;
	linep = p->buf_s;
	bufp = linep;
	while( bufp < tok->pLexanS ) {
		int hasline = 0;

		if( *bufp == '\r' ) {
			hasline = 1;
			++bufp;
		}
		if( bufp < tok->pLexanS && *bufp == '\n' ) {
			hasline = 1;
			++bufp;
		}

		if( hasline ) {
			++cLines;
			linep = bufp;
		} else {
			++bufp;
		}
	}

	pOutLineInfo->uLine = cLines + 1;
	pOutLineInfo->uColumn = 1 + ( unsigned )( axconf_size_t )( tok->pLexanS - linep );

	pOutLineInfo->LineRef.s = linep;
	pOutLineInfo->LineRef.n = ( int )( axconf_size_t )( axconf__skip_line( linep, p->buf_e ) - linep );

	return pOutLineInfo;
}
#else
;
#endif

AXCONF_FUNC const char *AXCONF_CALL axconf_token_type_to_string( axconf_token_type_t t )
#if AXCONF_IMPLEMENT
{
	switch( t )
	{
	case kAxconfTok_Invalid:                return "(invalid)";
	case kAxconfTok_EOF:                    return "(eof)";

	case kAxconfTok_LBracket:               return "'['";
	case kAxconfTok_RBracket:               return "']'";
	case kAxconfTok_LBrace:                 return "'{'";
	case kAxconfTok_RBrace:                 return "'}'";
	case kAxconfTok_LParen:                 return "'('";
	case kAxconfTok_RParen:                 return "')'";
	case kAxconfTok_Lt:                     return "'<'";
	case kAxconfTok_Gt:                     return "'>'";
	case kAxconfTok_LE:                     return "'<='";
	case kAxconfTok_GE:                     return "'>='";
	case kAxconfTok_Eq:                     return "'=='";
	case kAxconfTok_NE:                     return "'!='";

	case kAxconfTok_Assign:                 return "'='";
	case kAxconfTok_AddAssign:              return "'+='";
	case kAxconfTok_UniqueAssign:           return "'.='";
	case kAxconfTok_StrongAssign:           return "':='";
	case kAxconfTok_OptionalAssign:         return "'?='";

	case kAxconfTok_Tag:                    return "tag";
	case kAxconfTok_Name:                   return "identifier";
	case kAxconfTok_String:                 return "string-literal";
	case kAxconfTok_Number:                 return "numeric-literal";
	}

	return "(unknown)";
}
#else
;
#endif


/*
===============================================================================
###############################################################################
===============================================================================

	REPORT / LOGGING MANAGEMENT FUNCTIONS

===============================================================================
###############################################################################
===============================================================================
*/

AXCONF_FUNC void AXCONF_CALL axconf_nomem( axconf_t *p, axconf_size_t cNumBytes )
#if AXCONF_IMPLEMENT
{
	axconf_report_t *r;
	axconf_size_t i;

	AXCONF_ASSERT( p != ( axconf_t * )0 && "Must specify configuration" );

	/* If out-of-memory was already triggered... */
	if( p->r_outOfMemory.Severity != kAxconfSev_Silent ) {
		return;
	}

	/* Prepare the report */
	r = &p->r_outOfMemory;

	r->pConfig = p;
	r->Location.pszFilename = ( const char * )0;
	r->Location.uLine = 0;
	r->Location.uColumn = 0;
	r->Location.LineRef.n = 0;
	r->Location.LineRef.s = "";
	r->Severity = kAxconfSev_Panic;
	r->MessageId = kAxconfMsg_OutOfMemory;
	r->pszMessage = axconf_messageid_to_string( kAxconfMsg_OutOfMemory );
	r->cArgs = 1;
	axconf__numtostringref( &r->Args[ 0 ], &p->r_temp[0], sizeof( p->r_temp ), &p->r_temp_i, cNumBytes );
	for( i = 1; i < AXCONF_MAX_REPORT_ARGS; ++i ) {
		r->Args[ i ].s = "";
		r->Args[ i ].n = 0;
	}

	/* Link the report to the end of the list */
	r->pNextReport = ( axconf_report_t * )0;
	r->pPrevReport = p->r_tail;
	if( r->pPrevReport != ( axconf_report_t * )0 ) {
		r->pPrevReport->pNextReport = r;
	} else {
		p->r_head = r;
	}
	p->r_tail = r;
}
#else
;
#endif

AXCONF_FUNC int AXCONF_CALL axconf_submit_report( const axconf_report_t *pReport )
#if AXCONF_IMPLEMENT
{
	axconf_size_t i, off[ AXCONF_MAX_REPORT_ARGS ], n;

	AXCONF_ASSERT( pReport != ( const axconf_report_t * )0
		&& "Report must be set" );

	do {
		const axconf_report_t *s;
		axconf_report_t *p;
		axconf_severity_t sev;

		AXCONF_ASSERT( pReport->pConfig != ( axconf_t * )0
			&& "Report must have configuration set" );

		s = pReport;
		pReport = pReport->pNextReport;

		/* Determine the effective severity of this report */
		sev = s->Severity;
		if( sev == kAxconfSev_Warning ) {
			sev = s->pConfig->WarningSev;
		}

		/* Skip reports that don't meet the minimum severity requirement */
		if( sev > s->pConfig->MinimumSev ) {
			continue;
		}

		/* Determine how much extra space to allocate for the report */
		n = 0;
		for( i = 0; i < s->cArgs; ++i ) {
			off[ i ] = n;
			n += s->Args[ i ].n;
		}

		/* Allocate the report */
		p = ( axconf_report_t * )axconf_alloc( sizeof( *p ) + n );
		if( !p ) {
			/* Ran out of memory while trying to allocate the report... */
			axconf_nomem( pReport->pConfig, sizeof( *p ) + n );
			return 0;
		}

		/* Fill in the details of the report */
		p->pConfig = s->pConfig;
		p->Location = s->Location;
		if( !p->Location.pszFilename && p->Location.uLine > 0 ) {
			p->Location.pszFilename = p->pConfig->pszFilename;
		}
		p->Severity = sev;
		p->MessageId = s->MessageId;
		p->pszMessage = !s->pszMessage ? axconf_messageid_to_string( s->MessageId ) : s->pszMessage;
		p->cArgs = s->cArgs;
		for( i = 0; i < p->cArgs; ++i ) {
			/* Copy the string from the original report to here */
			p->Args[ i ].n = s->Args[ i ].n;
			p->Args[ i ].s = ( ( char * )( p + 1 ) ) + off[ i ];
			if( p->Args[ i ].n <= 0 ) {
				continue;
			}
			axconf_memcpy( ( void * )p->Args[ i ].s, ( const void * )s->Args[ i ].s, p->Args[ i ].n );
		}
		for( i = p->cArgs; i < AXCONF_MAX_REPORT_ARGS; ++i ) {
			p->Args[ i ].n = 0;
			p->Args[ i ].s = "";
		}

		/* Link the report to the end of the list */
		p->pNextReport = ( axconf_report_t * )0;
		p->pPrevReport = p->pConfig->r_tail;
		if( p->pPrevReport != ( axconf_report_t * )0 ) {
			p->pPrevReport->pNextReport = p;
		} else {
			p->pConfig->r_head = p;
		}
		p->pConfig->r_tail = p;

		/* If the report is an error, make note of it */
		if( sev <= kAxconfSev_Error ) {
			/* Bail out if there are too many errors */
			if( ++p->pConfig->cErrors == p->pConfig->cMaxErrors ) {
				return 1;
			}
		}

		/* Also keep statistics for warnings */
		if( sev == kAxconfSev_Warning ) {
			++p->pConfig->cWarnings;
		}
	} while( pReport != ( const axconf_report_t * )0 );

	return 1;
}
#else
;
#endif

AXCONF_FUNC void AXCONF_CALL axconf_report_toomanyerrors( axconf_t *p )
#if AXCONF_IMPLEMENT
{
	axconf_report_t r;
	char Buf[ 32 ];
	axconf_size_t BufI = 0;

	r.pConfig = p;
	r.Location.pszFilename = ( const char * )0;
	r.Location.uLine = 0;
	r.Location.uColumn = 0;
	r.Location.LineRef.n = 0;
	r.Location.LineRef.s = "";
	r.Severity = kAxconfSev_Normal;
	r.MessageId = kAxconfMsg_TooManyErrors;
	r.pszMessage = ( const char * )0;
	r.cArgs = 1;
	axconf__numtostringref( &r.Args[ 0 ], &Buf[0], sizeof(Buf), &BufI, p->cMaxErrors );
	r.pPrevReport = ( axconf_report_t * )0;
	r.pNextReport = ( axconf_report_t * )0;

	axconf_submit_report( &r );
}
#else
;
#endif
AXCONF_FUNC void AXCONF_CALL axconf_report_invalidtoken( axconf_t *p )
#if AXCONF_IMPLEMENT
{
	axconf_report_t r;

	r.pConfig = p;
	r.Location.pszFilename = ( const char * )0;
	r.Location.uLine = 0;
	r.Location.uColumn = 0;
	r.Location.LineRef.n = 0;
	r.Location.LineRef.s = "";
	r.Severity = kAxconfSev_Warning;
	r.MessageId = kAxconfMsg_Lexer_InvalidToken;
	r.pszMessage = ( const char * )0;
	if( p->l_tail != ( axconf_token_link_t * )0 ) {
		r.cArgs = 1;
		axconf__toktostringref( &r.Args[ 0 ], &p->l_tail->tok );
		axconf_get_lineinfo( &r.Location, p, &p->l_tail->tok );
	} else {
		r.cArgs = 0;
	}
	r.pPrevReport = ( axconf_report_t * )0;
	r.pNextReport = ( axconf_report_t * )0;

	axconf_submit_report( &r );
}
#else
;
#endif

AXCONF_FUNC const axconf_report_t *AXCONF_CALL axconf_first_report( const axconf_t *p )
#if AXCONF_IMPLEMENT
{
	return p->r_head;
}
#else
;
#endif
AXCONF_FUNC const axconf_report_t *AXCONF_CALL axconf_last_report( const axconf_t *p )
#if AXCONF_IMPLEMENT
{
	return p->r_tail;
}
#else
;
#endif
AXCONF_FUNC const axconf_report_t *AXCONF_CALL axconf_prev_report( const axconf_report_t *r )
#if AXCONF_IMPLEMENT
{
	return !r ? ( const axconf_report_t * )0 : r->pPrevReport;
}
#else
;
#endif
AXCONF_FUNC const axconf_report_t *AXCONF_CALL axconf_next_report( const axconf_report_t *r )
#if AXCONF_IMPLEMENT
{
	return !r ? ( const axconf_report_t * )0 : r->pNextReport;
}
#else
;
#endif

AXCONF_FUNC char *AXCONF_CALL axconf_format_report( char *pDstBuf, axconf_size_t cDstBytes, const axconf_report_t *r )
#if AXCONF_IMPLEMENT
{
	const char *pszPrompt = ( const char * )0;
	char *pBufOut;

	pBufOut = pDstBuf;

	/* Record line information */
	if( r->Location.pszFilename != ( const char * )0 ) {
		axconf__saz( pDstBuf, cDstBytes, &pBufOut, r->Location.pszFilename );
		if( r->Location.uLine != 0 ) {
			axconf__sachr( pDstBuf, cDstBytes, &pBufOut, '(' );
			axconf__sanum( pDstBuf, cDstBytes, &pBufOut, r->Location.uLine );
			if( r->Location.uColumn != 0 ) {
				axconf__sachr( pDstBuf, cDstBytes, &pBufOut, ':' );
				axconf__sanum( pDstBuf, cDstBytes, &pBufOut, r->Location.uColumn );
			}
			axconf__sachr( pDstBuf, cDstBytes, &pBufOut, ')' );
		}

		axconf__sa( pDstBuf, cDstBytes, &pBufOut, ": ", 2 );
	}

	/* Record the prompt */
	switch( r->Severity )
	{
	case kAxconfSev_Panic:      pszPrompt = "error: (fatal) "; break;
	case kAxconfSev_Error:      pszPrompt = "error: "; break;
	case kAxconfSev_Warning:    pszPrompt = "warning: "; break;
	case kAxconfSev_Remark:     pszPrompt = "remark: "; break;
	case kAxconfSev_Normal:     break;
	case kAxconfSev_Verbose:    break;
	case kAxconfSev_Silent:     break;
	}

	if( pszPrompt != ( const char * )0 ) {
		axconf__saz( pDstBuf, cDstBytes, &pBufOut, pszPrompt );
	}

	/* Format the message for output */
	axconf_format_report_msg( pBufOut, ( axconf_size_t )( ( pDstBuf + cDstBytes ) - pBufOut ), r );

	return pDstBuf;
}
#else
;
#endif

#ifndef AXCONF_NO_STDIO
AXCONF_FUNC void AXCONF_CALL axconf_print_report( const axconf_report_t *r )
# if AXCONF_IMPLEMENT
{
	char Buf[ 2048 ];

	axconf_format_report( Buf, sizeof( Buf ), r );
	Buf[ sizeof( Buf ) - 1 ] = '\0';

	fprintf( stderr, "%s\n", Buf );
}
# else
;
# endif
AXCONF_FUNC void AXCONF_CALL axconf_print_reports( const axconf_t *p )
# if AXCONF_IMPLEMENT
{
	const axconf_report_t *r;

	for( r = axconf_first_report( p ); r != ( const axconf_report_t * )0; r = axconf_next_report( r ) ) {
		axconf_print_report( r );
	}
}
# else
;
# endif
#endif


/*
===============================================================================
###############################################################################
===============================================================================

	VALUE HANDLING

===============================================================================
###############################################################################
===============================================================================
*/

#if AXCONF_IMPLEMENT
static void AXCONF_CALL axconf__val_unlink( axconf_value_link_t *pVal )
{
	if( pVal->l_prev != ( axconf_value_link_t * )0 ) {
		pVal->l_prev->l_next = pVal->l_next;
	} else if( pVal->pVar != ( axconf_var_t * )0 ) {
		pVal->pVar->l_head = pVal->l_next;
	}

	if( pVal->l_next != ( axconf_value_link_t * )0 ) {
		pVal->l_next->l_prev = pVal->l_prev;
	} else if( pVal->pVar != ( axconf_var_t * )0 ) {
		pVal->pVar->l_tail = pVal->l_prev;
	}

	pVal->pVar = ( axconf_var_t * )0;
	pVal->l_prev = ( axconf_value_link_t * )0;
	pVal->l_next = ( axconf_value_link_t * )0;
}
#endif

AXCONF_FUNC axconf_value_link_t *AXCONF_CALL axconf_val_init( axconf_value_link_t *pVal )
#if AXCONF_IMPLEMENT
{
	pVal->pVar = ( axconf_var_t * )0;

	pVal->l_prev = ( axconf_value_link_t * )0;
	pVal->l_next = ( axconf_value_link_t * )0;

	pVal->Data.b = 0;
	pVal->Data.i = 0;
	pVal->Data.u = 0;
	pVal->Data.f.iWhole = 0;
	pVal->Data.f.uFract = 0;
	pVal->Data.f.iExp = 0;
	pVal->Data.psz = ( char * )0;
	pVal->Data.bin.cBytes = 0;
	pVal->Data.bin.pBytes = ( unsigned char * )0;

	return pVal;
}
#else
;
#endif
AXCONF_FUNC axconf_value_link_t *AXCONF_CALL axconf_val_fini( axconf_value_link_t *pVal )
#if AXCONF_IMPLEMENT
{
	if( !pVal ) {
		return ( axconf_value_link_t * )0;
	}

	if( pVal->pVar != ( axconf_var_t * )0 ) {
		if( pVal->pVar->ValueTy == kAxconfValTy_Blob ) {
			axconf_prc_free( ( void * )pVal->Data.bin.pBytes );
		} else if( pVal->pVar->ValueTy == kAxconfValTy_String ) {
			axconf_prc_free( ( void * )pVal->Data.psz );
		}
	}

	axconf__val_unlink( pVal );
	return ( axconf_value_link_t * )0;
}
#else
;
#endif
AXCONF_FUNC axconf_value_link_t *AXCONF_CALL axconf_val_free( axconf_value_link_t *pVal )
#if AXCONF_IMPLEMENT
{
	axconf_val_fini( pVal );
	axconf_free( ( void * )pVal );

	return ( axconf_value_link_t * )0;
}
#else
;
#endif


/*
===============================================================================
###############################################################################
===============================================================================

	VARIABLE HANDLING

===============================================================================
###############################################################################
===============================================================================
*/

AXCONF_FUNC axconf_var_t *AXCONF_CALL axconf_var_init( axconf_var_t *pVar, axconf_section_t *pSect )
#if AXCONF_IMPLEMENT
{
	pVar->pszName = ( char * )0;

	pVar->ValueTy = kAxconfValTy_Invalid;

	pVar->cValues = 0;
	pVar->l_head = ( axconf_value_link_t * )0;
	pVar->l_tail = ( axconf_value_link_t * )0;

	pVar->pSection = pSect;
	pVar->v_prev = pSect->v_tail;
	pVar->v_next = ( axconf_var_t * )0;
	if( pSect->v_tail != ( axconf_var_t * )0 ) {
		pSect->v_tail->v_next = pVar;
	} else {
		pSect->v_head = pVar;
	}
	pSect->v_tail = pVar;

	return pVar;
}
#else
;
#endif
AXCONF_FUNC axconf_var_t *AXCONF_CALL axconf_var_fini( axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	if( !pVar ) {
		return ( axconf_var_t * )0;
	}

	axconf_free( ( void * )pVar->pszName );
	pVar->pszName = ( char * )0;

	while( pVar->l_head != ( axconf_value_link_t * )0 ) {
		axconf_val_free( pVar->l_head );
	}

	if( pVar->v_prev != ( axconf_var_t * )0 ) {
		pVar->v_prev->v_next = pVar->v_next;
	} else {
		pVar->pSection->v_head = pVar->v_next;
	}

	if( pVar->v_next != ( axconf_var_t * )0 ) {
		pVar->v_next->v_prev = pVar->v_prev;
	} else {
		pVar->pSection->v_tail = pVar->v_prev;
	}

	pVar->v_prev = ( axconf_var_t * )0;
	pVar->v_next = ( axconf_var_t * )0;
	pVar->pSection = ( axconf_section_t * )0;

	return ( axconf_var_t * )0;
}
#else
;
#endif
AXCONF_FUNC axconf_var_t *AXCONF_CALL axconf_var_free( axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	axconf_var_fini( pVar );
	axconf_free( ( void * )pVar );

	return ( axconf_var_t * )0;
}
#else
;
#endif

AXCONF_FUNC int AXCONF_CALL axconf_var_set_name_n( axconf_var_t *pVar, const char *pNameBase, axconf_size_t cNameBytes )
#if AXCONF_IMPLEMENT
{
	char *p;

	p = axconf__strndup( pNameBase, cNameBytes );
	if( !p ) {
		return 0;
	}

	axconf_free( ( void * )pVar->pszName );
	pVar->pszName = p;

	return 1;
}
#else
;
#endif
AXCONF_FUNC int AXCONF_CALL axconf_var_set_name( axconf_var_t *pVar, const char *pszName )
#if AXCONF_IMPLEMENT
{
	char *p;

	p = axconf__strdup( pszName );
	if( !p ) {
		return 0;
	}

	axconf_free( ( void * )pVar->pszName );
	pVar->pszName = p;

	return 1;
}
#else
;
#endif
AXCONF_FUNC const char *AXCONF_CALL axconf_var_get_name( const axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	return !pVar->pszName ? "" : pVar->pszName;
}
#else
;
#endif

AXCONF_FUNC axconf_value_type_t AXCONF_CALL axconf_var_get_type( const axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	return pVar->ValueTy;
}
#else
;
#endif

AXCONF_FUNC axconf_value_link_t *AXCONF_CALL axconf_var_first_value( const axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	return ( axconf_value_link_t * )pVar->l_head;
}
#else
;
#endif
AXCONF_FUNC axconf_value_link_t *AXCONF_CALL axconf_var_last_value( const axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	return ( axconf_value_link_t * )pVar->l_tail;
}
#else
;
#endif

AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_var_get_section( const axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	return ( axconf_section_t * )pVar->pSection;
}
#else
;
#endif

AXCONF_FUNC axconf_var_t *AXCONF_CALL axconf_var_prev( const axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	return !pVar ? ( axconf_var_t * )0 : ( axconf_var_t * )pVar->v_prev;
}
#else
;
#endif
AXCONF_FUNC axconf_var_t *AXCONF_CALL axconf_var_next( const axconf_var_t *pVar )
#if AXCONF_IMPLEMENT
{
	return !pVar ? ( axconf_var_t * )0 : ( axconf_var_t * )pVar->v_next;
}
#else
;
#endif


/*
===============================================================================
###############################################################################
===============================================================================

	SECTION HANDLING

===============================================================================
###############################################################################
===============================================================================
*/

AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_sect_init( axconf_section_t *pSect, axconf_context_t *pCtx )
#if AXCONF_IMPLEMENT
{
	pSect->pszName = ( char * )0;
	pSect->pContext = pCtx;
	pSect->s_prev = pCtx->s_tail;
	pSect->s_next = ( axconf_section_t * )0;
	if( pCtx->s_tail != ( axconf_section_t * )0 ) {
		pCtx->s_tail->s_next = pSect;
	} else {
		pCtx->s_head = pSect;
	}
	pCtx->s_tail = pSect;
	pSect->v_head = ( axconf_var_t * )0;
	pSect->v_tail = ( axconf_var_t * )0;

	return pSect;
}
#else
;
#endif
AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_sect_fini( axconf_section_t *pSect )
#if AXCONF_IMPLEMENT
{
	if( !pSect ) {
		return ( axconf_section_t * )0;
	}

	while( pSect->v_head != ( axconf_var_t * )0 ) {
		axconf_var_free( pSect->v_head );
	}

	axconf_free( ( void * )pSect->pszName );
	pSect->pszName = ( char * )0;

	return ( axconf_section_t * )0;
}
#else
;
#endif
AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_sect_free( axconf_section_t *pSect )
#if AXCONF_IMPLEMENT
{
	axconf_sect_fini( pSect );
	axconf_free( ( void * )pSect );

	return ( axconf_section_t * )0;
}
#else
;
#endif

AXCONF_FUNC int AXCONF_CALL axconf_sect_set_name_n( axconf_section_t *pSect, const char *pNameBase, axconf_size_t cNameBytes )
#if AXCONF_IMPLEMENT
{
	char *p;

	p = axconf__strndup( pNameBase, cNameBytes );
	if( !p ) {
		return 0;
	}

	axconf_free( ( void * )pSect->pszName );
	pSect->pszName = p;

	return 1;
}
#else
;
#endif
AXCONF_FUNC int AXCONF_CALL axconf_sect_set_name( axconf_section_t *pSect, const char *pszName )
#if AXCONF_IMPLEMENT
{
	char *p;

	p = axconf__strdup( pszName );
	if( !p ) {
		return 0;
	}

	axconf_free( ( void * )pSect->pszName );
	pSect->pszName = p;

	return 1;
}
#else
;
#endif
AXCONF_FUNC const char *AXCONF_CALL axconf_sect_get_name( const axconf_section_t *pSect )
#if AXCONF_IMPLEMENT
{
	return pSect->pszName;
}
#else
;
#endif

AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_sect_prev( const axconf_section_t *pSect )
#if AXCONF_IMPLEMENT
{
	return !pSect ? ( axconf_section_t * )0 : ( axconf_section_t * )pSect->s_prev;
}
#else
;
#endif
AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_sect_next( const axconf_section_t *pSect )
#if AXCONF_IMPLEMENT
{
	return !pSect ? ( axconf_section_t * )0 : ( axconf_section_t * )pSect->s_next;
}
#else
;
#endif

AXCONF_FUNC axconf_var_t *AXCONF_CALL axconf_sect_first_var( const axconf_section_t *pSect )
#if AXCONF_IMPLEMENT
{
	return ( axconf_var_t * )pSect->v_head;
}
#else
;
#endif
AXCONF_FUNC axconf_var_t *AXCONF_CALL axconf_sect_last_var( const axconf_section_t *pSect )
#if AXCONF_IMPLEMENT
{
	return ( axconf_var_t * )pSect->v_tail;
}
#else
;
#endif


/*
===============================================================================
###############################################################################
===============================================================================

	CONTEXT HANDLING

===============================================================================
###############################################################################
===============================================================================
*/

#if AXCONF_IMPLEMENT
static int AXCONF_CALL axconf__ctx_addcfg( axconf_context_t *pCtx, axconf_t *pCfg )
{
	axconf_size_t n;
	axconf_t **pp;

	n = pCtx->cConfigs;

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable:6386)
#endif

	pp = ( axconf_t ** )axconf_alloc( ( n + 1 )*sizeof( void * ) );
	if( !pp ) {
		return 0;
	}

	if( n > 0 ) {
		axconf_memcpy( ( void * )pp, ( const void * )pCtx->ppConfigs, n*sizeof( void * ) );
		axconf_free( ( void * )pCtx->ppConfigs );
	}

	pCtx->ppConfigs = pp;
	pCtx->ppConfigs[ n ] = pCfg;

	++pCtx->cConfigs;

#ifdef _MSC_VER
# pragma warning(pop)
#endif

	return 1;
}
static void AXCONF_CALL axconf__ctx_unlink( axconf_context_t *p )
{
	if( p->ctx_prev != ( axconf_context_t * )0 ) {
		p->ctx_prev->ctx_next = p->ctx_next;
	} else if( p->ctx_prnt != ( axconf_context_t * )0 ) {
		p->ctx_prnt->ctx_head = p->ctx_next;
	}

	if( p->ctx_next != ( axconf_context_t * )0 ) {
		p->ctx_next->ctx_prev = p->ctx_prev;
	} else if( p->ctx_prnt != ( axconf_context_t * )0 ) {
		p->ctx_prnt->ctx_tail = p->ctx_prev;
	}

	p->ctx_prev = ( axconf_context_t * )0;
	p->ctx_next = ( axconf_context_t * )0;

	p->ctx_prnt = ( axconf_context_t * )0;
}
# if 0 /*TODO:Use this for context hierarchies*/
static void AXCONF_CALL axconf__ctx_linktail( axconf_context_t *pCtxPrnt, axconf_context_t *pCtxChld )
{
	pCtxChld->ctx_prev = pCtxPrnt->ctx_tail;
	pCtxChld->ctx_next = ( axconf_context_t * )0;
	if( pCtxPrnt->ctx_tail != ( axconf_context_t * )0 ) {
		pCtxPrnt->ctx_tail->ctx_next = pCtxChld;
	} else {
		pCtxPrnt->ctx_head = pCtxChld;
	}
	pCtxPrnt->ctx_tail = pCtxChld;
	pCtxChld->ctx_prnt = pCtxPrnt;
}
# endif
#endif

AXCONF_FUNC axconf_context_t *AXCONF_CALL axconf_ctx_init( axconf_context_t *pCtx, axconf_t *pCfg )
#if AXCONF_IMPLEMENT
{
	pCtx->ctx_prnt = ( axconf_context_t * )0;
	pCtx->ctx_prev = ( axconf_context_t * )0;
	pCtx->ctx_next = ( axconf_context_t * )0;
	pCtx->ctx_head = ( axconf_context_t * )0;
	pCtx->ctx_tail = ( axconf_context_t * )0;

	pCtx->s_head = ( axconf_section_t * )0;
	pCtx->s_tail = ( axconf_section_t * )0;

	pCtx->cConfigs = 0;
	pCtx->ppConfigs = ( axconf_t ** )0;

	if( !axconf__ctx_addcfg( pCtx, pCfg ) ) {
		return ( axconf_context_t * )0;
	}

	return pCtx;
}
#else
;
#endif
AXCONF_FUNC axconf_context_t *AXCONF_CALL axconf_ctx_fini( axconf_context_t *pCtx )
#if AXCONF_IMPLEMENT
{
	axconf_size_t i;

	/* Remove self from parent */
	if( pCtx->ctx_prnt != ( axconf_context_t * )0 ) {
		axconf__ctx_unlink( pCtx );
	}

	/* Remove all sub-contexts */
	while( pCtx->ctx_head != ( axconf_context_t * )0 ) {
		axconf_ctx_fini( pCtx->ctx_head );
	}

	/* Remove self from all configurations still pointing to this */
	for( i = 0; i < pCtx->cConfigs; ++i ) {
		if( pCtx->ppConfigs[ i ]->pContext != pCtx ) {
			continue;
		}

		pCtx->ppConfigs[ i ]->pContext = ( axconf_context_t * )0;
	}

	/* Remove all sections */
	while( pCtx->s_head != ( axconf_section_t * )0 ) {
		axconf_sect_free( pCtx->s_head );
	}

	return ( axconf_context_t * )0;
}
#else
;
#endif

AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_ctx_add_section( axconf_context_t *pCtx )
#if AXCONF_IMPLEMENT
{
	axconf_section_t *pSect;

	pSect = ( axconf_section_t * )axconf_alloc( sizeof( *pSect ) );
	if( !pSect ) {
		return ( axconf_section_t * )0;
	}

	if( !axconf_sect_init( pSect, pCtx ) ) {
		axconf_free( ( void * )pSect );
		return ( axconf_section_t * )0;
	}

	return pSect;
}
#else
;
#endif
AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_ctx_add_section_by_token( axconf_context_t *pCtx, const axconf_token_t *pTok )
#if AXCONF_IMPLEMENT
{
	axconf_section_t *pSect;

	pSect = axconf_ctx_add_section( pCtx );
	if( !pSect ) {
		return ( axconf_section_t * )0;
	}

	if( !axconf_sect_set_name_n( pSect, pTok->pLexanS, ( axconf_size_t )( pTok->pLexanE - pTok->pLexanS ) ) ) {
		return axconf_sect_free( pSect );
	}

	return pSect;
}
#else
;
#endif

AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_ctx_first_section( const axconf_context_t *pCtx )
#if AXCONF_IMPLEMENT
{
	return ( axconf_section_t * )pCtx->s_head;
}
#else
;
#endif
AXCONF_FUNC axconf_section_t *AXCONF_CALL axconf_ctx_last_section( const axconf_context_t *pCtx )
#if AXCONF_IMPLEMENT
{
	return ( axconf_section_t * )pCtx->s_tail;
}
#else
;
#endif


/*
===============================================================================
###############################################################################
===============================================================================

	CONFIGURATION LEXING

===============================================================================
###############################################################################
===============================================================================
*/

#if AXCONF_IMPLEMENT
static const char *axconf__skip_white( const char *s, const char *e )
{
	while( s < e && *( const unsigned char * )s <= ' ' ) {
		++s;
	}

	return s;
}
static int axconf__has_line( const char *s, const char *e )
{
	if( s == e ) { return 0; }

#if 1
	return
		axconf_memchr( ( const void * )s, '\n', e - s ) != ( void * )0 ||
		axconf_memchr( ( const void * )s, '\r', e - s ) != ( void * )0;
#else
	return
		axconf__skip_line( s, e ) != e ||
		*( e - 1 ) == '\n' ||
		*( e - 1 ) == '\r';
#endif
}
static const char *axconf__skip_comment( const char *s, const char *e )
{
	int nest = 1;

	if( s + 1 > e ) {
		return s;
	}

	if( *s == '#' || *s == ';' ) {
		return axconf__skip_line( s + 1, e );
	}

	if( s + 2 > e || *s != '/' ) {
		return s;
	}

	if( s[1] == '/' ) {
		return axconf__skip_line( s + 2, e );
	}

	if( s[1] != '*' ) {
		return s;
	}

	s += 2;
	while( s < e && nest > 0 ) {
		if( s + 2 > e ) {
			s = e;
			break;
		}

		if( s[0]=='/' && s[1]=='*' ) {
			s += 2;
			++nest;
			continue;
		}

		if( s[0]=='*' && s[1]=='/' ) {
			s += 2;
			--nest;
			continue;
		}

		++s;
	}

	return s;
}
static const char *axconf__skip_nonwhite( const char *s, const char *e )
{
	while( s < e && *( const unsigned char * )s > ' ' ) {
		++s;
	}

	return s;
}
static const char *axconf__skip_tag( const char *s, const char *e )
{
	if( s < e && *s != '*' && *s != '+' && *s != '-' && *s != '~' ) {
		return s;
	}

	return axconf__skip_nonwhite( s + 1, e );
}

static const char *axconf__skip_quoted( const char *s, const char *e, axconf_token_t *t_out, axconf_size_t *bytes_out )
{
	const char *b = ( const char * )0;
	char *pmem = ( char * )0;
	char ch;
	int state = 0;

	while( s < e && state != 3 ) {
		switch( state ) {
		/* check for initial quotation mark */
		case 0:
			if( *s != '\"' ) {
				return s;
			}

			b = ++s;
			state = 1;
			break;

		/* check for escape or end quotation mark; otherwise normal char */
		case 1:
			if( *s == '\\' ) {
				state = 2;
			} else if( *s == '\"' ) {
				state = 3;
			}

			if( state != 1 ) {
				if( !axconf__strappend( &pmem, b, s ) ) {
					/* Out of memory */
					*bytes_out = ( axconf_size_t )( s - b );
					return ( const char * )0;
				}
			}

			++s;
			break;

		/* skip the escape */
		case 2:
			ch = '_';
			if( *s == '\\' || *s == '\'' || *s == '\"' || *s == '\?' ) {
				ch = *s;
			} else if( *s == 'a' ) {
				ch = '\a';
			} else if( *s == 'b' ) {
				ch = '\b';
			} else if( *s == 'f' ) {
				ch = '\f';
			} else if( *s == 'n' ) {
				ch = '\n';
			} else if( *s == 'r' ) {
				ch = '\r';
			} else if( *s == 't' ) {
				ch = '\t';
			} else if( *s == 'v' ) {
				ch = '\v';
			}

			if( !axconf__strappendch( &pmem, ch ) ) {
				/* Out of memory */
				*bytes_out = 1;
				return ( const char * )0;
			}

			b = ++s;
			state = 1;
			break;
		}
	}

	if( pmem != ( char * )0 && state == 3 ) {
		t_out->uFlags |= kAxconfTokF_Processed;

		t_out->processed.pszEscaped = pmem;
		t_out->pOwnedMem = ( void * )( ( ( axconf_size_t * )pmem ) - 2 );
	}

	return s;
}
static int axconf__get_digit( unsigned char ch, unsigned radix )
{
	unsigned lo, hi;

	lo = radix < 10 ? radix : 10;
	hi = radix < 10 ? 0 : radix - 10;

	if( ch >= '0' && ch < '0' + lo ) {
		return ch - '0';
	}

	if( ch >= 'a' && ch < 'a' + hi ) {
		return ch - 'a' + 10;
	}

	if( ch >= 'A' && ch < 'A' + hi ) {
		return ch - 'A' + 10;
	}

	return -1;
}
static const char *axconf__skip_digits( const char *s, const char *e, unsigned radix, axconf_token_t *t_out )
{
	axconf_u64_t v;
	int x;

	t_out->processed.uValue = 0;
	while( s < e && ( x = axconf__get_digit( *s, radix ) ) != -1 ) {
		if( ~t_out->uFlags & kAxconfTokF_Overflowed ) {
			v = t_out->processed.uValue;
			v *= radix;
			v += x;
			if( t_out->processed.uValue < v ) {
				t_out->processed.uValue = v;
			} else {
				t_out->uFlags |= kAxconfTokF_Overflowed;
			}
		}
		++s;

		/* digit separators */
		if( ( *s == '\'' || *s == '_' ) && ( s + 1 ) < e ) {
			++s;
		}
	}

	return s;
}

static const char *axconf__skip_number( const char *s, const char *e, axconf_token_t *t_out )
{
	axconf_s64_t mult;
	axconf_s64_t exps;
	axconf_u64_t expv;
	axconf_u64_t whol;
	axconf_u64_t frac;
	axconf_u64_t fram;
	axconf_u64_t frat;
	const char *p=(const char*)0, *q=(const char*)0;
	unsigned radix;
	int isf;

	p = s;
	if( *p == '-' && p + 1 < e ) {
		mult = -1;
		++p;
	} else {
		if( *p == '+' && p + 1 < e ) {
			++p;
		}

		mult = 1;
	}

	radix = 10;
	if( p + 2 < e && *p == '0' ) {
		if( p[1] == 'x' || p[1] == 'X' ) {
			radix = 16;
			p += 2;
		} else if( p[1] == 'c' || p[1] == 'C' ) {
			radix = 8;
			p += 2;
		} else if( p[1] == 'b' || p[1] == 'B' ) {
			radix = 2;
			p += 2;
		}
	}

	q = axconf__skip_digits( p, e, radix, t_out );
	if( q == p ) {
		t_out->uFlags &= ~kAxconfTokF_Overflowed;
		return s;
	}

	isf = 0;
	if( *q == '.' && q + 1 < e ) {
		isf = 1;

		exps = 1;
		expv = 0;

		whol = t_out->processed.uValue;

		t_out->processed.uValue = 0;

		p = q + 1;
		q = axconf__skip_digits( p, e, radix, t_out );
		if( q != p ) {
			frac = t_out->processed.uValue;
		} else {
			frac = 0;
		}
	}

	if( ( *q == 'e' || *q == 'E' || *q == 'p' || *q == 'P' ) && q + 3 < e ) {
		++q;

		if( !isf ) {
			whol = t_out->processed.uValue;
			frac = 0;

			isf = 1;
		}
		t_out->processed.uValue = 0;

		if( *q == '-' ) {
			exps = -1;
			++q;
		} else {
			if( *q == '+' ) {
				++q;
			}

			exps = 1;
		}

		p = q;
		q = axconf__skip_digits( p, e, radix, t_out );
		if( q != p ) {
			expv = t_out->processed.uValue;
		} else {
			expv = 0;
		}
	}

	if( isf ) {
		axconf_real_t f;

		f = ( axconf_real_t )whol;
		f *= ( axconf_real_t )mult;

		fram = 1;
		frat = frac;
		while( frat > 0 ) {
			fram *= radix;
			frat /= radix;
		}

		f += ( ( axconf_real_t )frac )/( axconf_real_t )fram;
		if( exps > 0 ) {
			while( expv > 0 ) {
				f *= 10;
				--expv;
			}
		} else {
			while( expv > 0 ) {
				f /= 10;
				--expv;
			}
		}

		t_out->processed.fValue = f;
		t_out->uFlags |= kAxconfTokF_Number_Float;
	} else if( mult < 0 ) {
		t_out->processed.iValue = -( axconf_s64_t )t_out->processed.uValue;
		t_out->uFlags |= kAxconfTokF_Number_Signed;
	}

	t_out->uFlags |= kAxconfTokF_Processed;
	return q;
}
#endif

AXCONF_FUNC axconf_token_t *AXCONF_CALL axconf_lex( axconf_t *p )
#if AXCONF_IMPLEMENT
{
	axconf_token_link_t *t;
	axconf_size_t cBytes;
	const char *b, *q;
	unsigned uFlags;

	AXCONF_ASSERT( p->buf_s != ( char * )0 && "No buffer set for lexer" );

	/* GCC thinks this might be used uninitialized; initialize to be safe */
	cBytes = 0;

	/* fail if the last report generated was fatal */
	if( p->r_tail != ( axconf_report_t * )0 && p->r_tail->Severity == kAxconfSev_Panic ) {
		return ( axconf_token_t * )0;
	}
	/* if too many errors were generated then make note of that */
	if( p->cErrors > 0 && p->cErrors == p->cMaxErrors ) {
		axconf_report_toomanyerrors( p );
		return ( axconf_token_t * )0;
	}

	/* check if we have tokens that we've already read */
	if( p->l_curr != p->l_tail ) {
		t = !p->l_curr ? p->l_head : p->l_curr->l_next;
		AXCONF_ASSERT( t != ( axconf_token_link_t * )0 &&
			"Invalid internal lexer state" );
		p->l_curr = t;
		return &t->tok;
	}

	/* continue where we left off at, or start fresh if necessary */
	b = !p->l_tail ? p->buf_s : p->l_tail->tok.pLexanE;
	uFlags = b == p->buf_s ? kAxconfTokF_Start | kAxconfTokF_FileStart : 0;

	/* skip all white-space and comments */
	for(;;) {
		q = b;

		q = axconf__skip_white( q, p->buf_e );
		q = axconf__skip_comment( q, p->buf_e );

		if( q == b ) {
			break;
		}

		/* check for a newline */
		if( ( ~uFlags & kAxconfTokF_Start ) && axconf__has_line( b, q ) ) {
			uFlags |= kAxconfTokF_Start;
		}

		b = q;
	}

	/* allocate the token */
	t = ( axconf_token_link_t * )axconf_tok_alloc( sizeof( *t ) );
	if( !t ) {
		axconf_nomem( p, sizeof( *t ) );
		return ( axconf_token_t * )0;
	}

	t->l_prev = p->l_tail;
	t->l_next = ( axconf_token_link_t * )0;
	if( t->l_prev != ( axconf_token_link_t * )0 ) {
		t->l_prev->l_next = t;
	} else {
		p->l_head = t;
	}
	p->l_tail = t;
	p->l_curr = t;

	t->tok.uFlags = uFlags;

	t->tok.pLexanS = b;
	t->tok.pLexanE = b;

	t->tok.processed.uValue = 0;
	t->tok.processed.fValue = 0.0;
	t->tok.processed.p = ( void * )0;

	t->tok.pOwnedMem = ( void * )0;

	/* check for eof */
	if( b == p->buf_e ) {
		t->tok.type = kAxconfTok_EOF;
		return &t->tok;
	}

	/* if no tokens were read or the last read token was a tag then we're lexing
	`  tags still */
	if( p->l_head == p->l_tail || p->l_tail->l_prev->tok.type == kAxconfTok_Tag ) {
		/* check for a tag */
		if( ( q = axconf__skip_tag( b, p->buf_e ) ) != b ) {
			/* tag found */
			t->tok.pLexanE = q;
			t->tok.type = kAxconfTok_Tag;
			return &t->tok;
		}
	}

	/* handle a directive */
	if( ( uFlags & kAxconfTokF_Start ) && *b == '!' ) {
		++b;
		while( b < p->buf_e && ( *b == '\t' || *b == ' ' ) ) {
			++b;
		}

		uFlags |= kAxconfTokF_Directive;
	}

	/* handle a string */
	if( ( q = axconf__skip_quoted( b, p->buf_e, &t->tok, &cBytes ) ) != b ) {
		if( !q ) {
			axconf_nomem( p, cBytes );
			t->tok.type = kAxconfTok_Invalid;
		} else {
			t->tok.pLexanE = q;
			t->tok.type = kAxconfTok_String;
		}
		return &t->tok;
	}

	/* handle a number */
	if( ( q = axconf__skip_number( b, p->buf_e, &t->tok ) ) != b ) {
		t->tok.pLexanE = q;
		t->tok.type = kAxconfTok_Number;
		return &t->tok;
	}

	/* punctuation */
	switch( *b )
	{
	case '[': case ']':
	case '(': case ')':
	case '{': case '}':
		t->tok.pLexanE = b + 1;
		t->tok.type = ( axconf_token_type_t )*b;
		return &t->tok;

	case '<':
	case '>':
	case '=':
		if( b + 1 < p->buf_e && b[1] == '=' ) {
			t->tok.pLexanE = b + 2;
			// Weirdness here because MSVC++
			t->tok.type =
				( axconf_token_type_t )(
					( ( axconf_token_type_t )b[0] ) * 256 + ( axconf_token_type_t )b[1]
				);
		} else {
			t->tok.pLexanE = b + 1;
			t->tok.type = ( axconf_token_type_t )*b;
		}
		return &t->tok;
	}

	if( b + 1 < p->buf_e && b[1] == '=' ) {
		if( *b == '!' ) {
			t->tok.pLexanE = b + 2;
			t->tok.type = kAxconfTok_NE;
			return &t->tok;
		}

		if( *b == '+' ) {
			t->tok.pLexanE = b + 2;
			t->tok.type = kAxconfTok_AddAssign;
			return &t->tok;
		}

		if( *b == '.' ) {
			t->tok.pLexanE = b + 2;
			t->tok.type = kAxconfTok_UniqueAssign;
			return &t->tok;
		}

		if( *b == '?' ) {
			t->tok.pLexanE = b + 2;
			t->tok.type = kAxconfTok_OptionalAssign;
			return &t->tok;
		}

		if( *b == ':' ) {
			t->tok.pLexanE = b + 2;
			t->tok.type = kAxconfTok_StrongAssign;
			return &t->tok;
		}
	}

	/* just a generic token, read it in */
	if( ( q = axconf__skip_nonwhite( b, p->buf_e ) ) != b ) {
		t->tok.pLexanE = q;
		t->tok.type = kAxconfTok_Name;
		return &t->tok;
	}

	/* something that is invalid */
	t->tok.pLexanE = b + 1;
	t->tok.type = kAxconfTok_Invalid;
	axconf_report_invalidtoken( p );
	return &t->tok;
}
#else
;
#endif
AXCONF_FUNC axconf_token_t *AXCONF_CALL axconf_token( const axconf_t *p )
#if AXCONF_IMPLEMENT
{
	return !p->l_curr ? ( axconf_token_t * )0 : &p->l_curr->tok;
}
#else
;
#endif
AXCONF_FUNC axconf_token_t *AXCONF_CALL axconf_unlex( axconf_t *p )
#if AXCONF_IMPLEMENT
{
	if( p->l_curr != ( axconf_token_link_t * )0 ) {
		p->l_curr = p->l_curr->l_prev;
	}

	return !p->l_curr ? ( axconf_token_t * )0 : &p->l_curr->tok;
}
#else
;
#endif

AXCONF_LEAVE_C

#endif
