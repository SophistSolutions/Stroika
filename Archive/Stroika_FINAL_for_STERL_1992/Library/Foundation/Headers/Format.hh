/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Format__
#define	__Format__

/*
 * $Header: /fuji/lewis/RCS/Format.hh,v 1.6 1992/11/12 08:05:09 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Format.hh,v $
 *		Revision 1.6  1992/11/12  08:05:09  lewis
 *		Use double instead of Real.
 *
 *		Revision 1.5  1992/10/10  03:56:51  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.4  1992/10/10  03:16:25  lewis
 *		Cleanup headers (80 columns) and better comments. Got rid of macro
 *		version of things (Declare stuff).
 *
 *		Revision 1.2  1992/07/01  03:46:06  lewis
 *		Renamed Strings.hh String.hh
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"Exception.hh"
#include	"String.hh"
#include	"StreamUtils.hh"


/*
 * These routines throw gMathRangeException if the result of the conversion will not fit
 * in the specified size return value.
 */

// overloads of standard C functions to use String
extern	double	strtod (const String& s, size_t& last);
extern	double	strtod (const String& s);

extern	long	strtol (const String& s, size_t& last, int base = 10);
extern	long	strtol (const String& s, int base = 10);

extern	unsigned	long	strtoul (const String& s, size_t& last, int base = 10);
extern	unsigned	long	strtoul (const String& s, int base = 10);


// not sure what to call these - is there a set of standard C routines for this???
// just like strto{d,l,ul} base is restricted from 0 to 36
extern	String	ltostring (long l, int base = 10);
extern	String	ultostring (unsigned long ul, int base = 10);
extern	String	dtostring (double d, int precision = -1);	// -1 implies full




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Format__*/
