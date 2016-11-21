/*

	ax_intdatetime - public domain
	Last update: 2015-10-01 Aaron Miller


	Defines AX_INT_DATE and AX_INT_TIME (among others).

		AX_INT_DATE
		The current (compilation) date as an integer.
		e.g., December 13th, 2011 would be 111213
		e.g., November 4th, 2006 would be 61104

		AX_INT_TIME
		The current (compilation) time as an integer.
		e.g., 9PM (21:00) exactly would be 210000
		e.g., 8:30AM (08:30) and 27 seconds would be 83027

	Also defines the following synonyms:

		AX_BUILD_DATE (same as AX_INT_DATE)
		AX_BUILD_TIME (same as AX_INT_TIME)
		AX_BUILD_DATE_TIME (same as AX_INT_DATE_TIME)


	LICENSE

	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy
	and modify this file as you see fit. There are no warranties of any
	kind.

*/

#ifndef INCGUARD_AX_INTDATETIME_H_
#define INCGUARD_AX_INTDATETIME_H_

#ifndef AX_NO_PRAGMA_ONCE
# pragma once
#endif

/*
===============================================================================

	COMPILER DATE/TIME (INTEGERS)

===============================================================================
*/

/**
 *  \def AX_DateCharLit__(c)
 *  \brief Converts a single character in the __DATE__ string to an integer
 **/
#define AX_DateCharLit__( c )\
	( ( ( c ) == ' ' ) ? ( 0 ) : ( ( c ) - '0' ) )
/**
 *  \def AX_StrLitToInt__(strLit,index0,index1)
 *  \brief Converts a set of two characters in the __DATE__ string to a
 *         (potentially) two-digit integer
 **/
#define AX_StrLitToInt__( strLit, index0, index1 )\
	( AX_DateCharLit__( +strLit[ index0 ] )*10 + ( +strLit[ index1 ] - '0' ) )
/**
 *  \def AX_DoDateMonth__(a,b,c,code)
 *  \brief If the first three letters of __DATE__ match the passed a, b, and c,
 *         then the integer specified in code is returned, or 0 if not.
 **/
#define AX_DoDateMonth__( a,b,c, code )\
	( ( __DATE__[ 0 ] == a && __DATE__[ 1 ] == b && __DATE__[ 2 ] == c ) ?\
		code : 0 )

/**
 *  \def AX_INT_YEAR
 *  \brief Retrieves the current year as an integer (y2k compatible!)
 **/
#define AX_INT_YEAR\
	( AX_StrLitToInt__( __DATE__, 9, 10 ) )
/**
 *  \def AX_INT_MONTH
 *  \brief Retrieves the current month as an integer
 **/
#define AX_INT_MONTH (\
	AX_DoDateMonth__( 'J','a','n', 1 ) +\
	AX_DoDateMonth__( 'F','e','b', 2 ) +\
	AX_DoDateMonth__( 'M','a','r', 3 ) +\
	AX_DoDateMonth__( 'A','p','r', 4 ) +\
	AX_DoDateMonth__( 'M','a','y', 5 ) +\
	AX_DoDateMonth__( 'J','u','n', 6 ) +\
	AX_DoDateMonth__( 'J','u','l', 7 ) +\
	AX_DoDateMonth__( 'A','u','g', 8 ) +\
	AX_DoDateMonth__( 'S','e','p', 9 ) +\
	AX_DoDateMonth__( 'O','c','t', 10 ) +\
	AX_DoDateMonth__( 'N','o','v', 11 ) +\
	AX_DoDateMonth__( 'D','e','c', 12 ) )
/**
 *  \def AX_INT_DAY
 *  \brief Retrieves the current day of the month as an integer
 **/
#define AX_INT_DAY\
	( AX_StrLitToInt__( __DATE__, 4, 5 ) )

/**
 *  \def AX_INT_HOUR
 *  \brief The current hour (24 hour format) as an integer
 **/
#define AX_INT_HOUR\
	( AX_StrLitToInt__( __TIME__, 0, 1 ) )
/**
 *  \def AX_INT_MINUTE
 *  \brief The current minute of the hour as an integer
 **/
#define AX_INT_MINUTE\
	( AX_StrLitToInt__( __TIME__, 3, 4 ) )
/**
 *  \def AX_INT_SECOND
 *  \brief The current second of the minute as an integer
 **/
#define AX_INT_SECOND\
	( AX_StrLitToInt__( __TIME__, 6, 7 ) )

/**
 *  \def AX_INT_DATE
 *  \brief Integer version of the date in the format of YYMMDD (decimal)
 **/
#define AX_INT_DATE\
	( AX_INT_YEAR*10000 + AX_INT_MONTH*100 + AX_INT_DAY )
/**
 *  \def AX_INT_TIME
 *  \brief Integer version of the time in the format of HHMMSS (decimal)
 **/
#define AX_INT_TIME\
	( AX_INT_HOUR*10000 + AX_INT_MINUTE*100 + AX_INT_SECOND )

/**
 *  \def AX_INT_DATE_TIME
 *  \brief Integer version of the date and time combined in the format of
 *         YYMMDDHHMM (decimal)
 **/
#define AX_INT_DATE_TIME\
	( AX_INT_DATE*10000 + AX_INT_TIME/100 )

/**
 *  \def AX_BUILD_DATE
 *  \brief Same as AX_INT_DATE (YYMMDD)
 **/
#define AX_BUILD_DATE AX_INT_DATE
/**
 *  \def AX_BUILD_TIME
 *  \brief Same as AX_INT_TIME (HHMMSS)
 **/
#define AX_BUILD_TIME AX_INT_TIME
/**
 *  \def AX_BUILD_DATE_TIME
 *  \brief Same as AX_INT_DATE_TIME (YYMMDDHHMM)
 **/
#define AX_BUILD_DATE_TIME AX_INT_DATE_TIME

#endif
