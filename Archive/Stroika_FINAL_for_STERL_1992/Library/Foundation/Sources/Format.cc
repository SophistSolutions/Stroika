/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Format.cc,v 1.9 1992/11/12 08:08:51 lewis Exp $
 *
 * TODO:
 *
 * Notes:
 *		Why do we do what we do, with kBadNumber??? and kEmptyString????
 *
 *		For the routines that convert from a string to a number, we disallow
 *	kEmptyString as an argument. there are only two reasonable alternatives to
 *	this: returning kBadNumber, and raising an exception (returning zero would
 *	lead to the unfortunate result that you could easily silently convert a
 *	kBadNumber to a zero by converting back and forth a few times). Returning
 *	kBadNumber is not a good idea because we cannot for routines that return
 *	a long, and doing so for routines that return a float would be a confusing
 *	inconsistency, and still beg the issue of what to do with routines that
 *	return a long. Raising an exception would be reasonable if checking before
 *	the call to this routine would present undue hardship to the caller, but,
 *	in fact, protecting against an exception would probably be more difficult.
 *
 *		For routines that accept a Real, we also reject as programming bugs
 *	the use of kBadNumber. It is up to the caller to check before calling that
 *	the value is a valid number.
 *
 * Changes:
 *	$Log: Format.cc,v $
 *		Revision 1.9  1992/11/12  08:08:51  lewis
 *		Use double instead of Real.
 *
 *		Revision 1.8  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.7  1992/10/09  05:35:31  lewis
 *		Get rid of remainging usage/support for kEmptyString.
 *
 *		Revision 1.6  1992/09/05  04:55:50  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.4  1992/07/21  06:03:15  lewis
 *		Previous workaround for qGCC_GPlusPlusLib_MissingStrToUL bug was insufficient.
 *		Comment out and try AssertNotReached () for now.
 *
 *		Revision 1.3  1992/07/03  06:19:44  lewis
 *		Tried to work around gcc bug - added define qGCC_GPlusPlusLib_MissingStrToUL for
 *		the bug - but not totally worked around.
 *
 *		Revision 1.2  1992/06/20  19:30:09  lewis
 *		Hack around trouble with strtol with gcc -(or was it strtoul?).
 *
 *		Revision 1.10  92/05/20  02:17:37  02:17:37  lewis (Lewis Pringle)
 *		Forgot case with number = 0 - must return "0" not empty string, and also, must reverse digits at end
 *		of ltostring and ultostring().
 *		
 *		Revision 1.9  92/05/19  23:44:48  23:44:48  lewis (Lewis Pringle)
 *		Loosened restriction on overzealous Assert in ultostring/ltostring.
 *		
 *		Revision 1.8  92/05/19  14:46:14  14:46:14  lewis (Lewis Pringle)
 *		Implemented ltostring and ultostring paying attention to base instead of sprintf which ignored it.
 *		
 *		Revision 1.7  92/04/27  18:34:41  18:34:41  lewis (Lewis Pringle)
 *		For some reason %uld doesnt work with sprintf on snake - revisit ANSI C guide to see if its their
 *		bug or mine - for now, specially appease snake.... (ultostring).
 *		
 *		Revision 1.6  92/03/30  14:23:01  14:23:01  lewis (Lewis Pringle)
 *		Ported to Borland C++.
 *		
 *		Revision 1.4  1992/02/13  23:18:49  lewis
 *		Add hack to dtostring() so that if we dont specify the precision, then we strip trailing zeros
 *		after the decimal point.
 *
 *		Revision 1.3  1992/01/29  04:31:34  sterling
 *		nedd to include stdio for Mac
 *
 *		Revision 1.2  1992/01/27  06:12:33  lewis
 *		Got rid of exceptions, and changed to requires for bad numbers, and bad strings being passed to these
 *		routines. The only exception being bad (but non-empty) string asked  to be parsed into a number.
 *
 *
 *
 */



#include	<errno.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"Math.hh"

#include	"Format.hh"







/*
 ********************************************************************************
 ************************************** strtod **********************************
 ********************************************************************************
 */

double	strtod (const String& s, size_t& last)
{
	Require (s != "");
	char*	tmp		=	s.ToCString ();
	char*	tailPtr	=	Nil;
	double	d		=	::strtod (tmp, &tailPtr);
	last = (tailPtr - tmp);
	Assert (last <= s.GetLength ());
	delete (tmp);

	if (errno == 0) {
		return (d);
	}
	else {
		Assert (errno == ERANGE);
		errno = 0;
		gMathRangeException.Raise ();
		AssertNotReached (); return (0);
	}
}

double	strtod (const String& s)
{
	Require (s != "");
	size_t	ignored	=	1;
	return (strtod (s, ignored));
}




/*
 ********************************************************************************
 ************************************** strtol **********************************
 ********************************************************************************
 */

long	strtol (const String& s, size_t& last, int base)
{
	Require (s != "");
	errno = 0;
	char*	tmp		=	s.ToCString ();
	char*	tailPtr	=	Nil;
	long	l		=	::strtol (tmp, &tailPtr, base);
	last = (tailPtr - tmp);
	Assert (last <= s.GetLength ());
	delete (tmp);

	if (errno == 0) {
		return (l);
	}
	else {
		Assert (errno == ERANGE);
		errno = 0;
		gMathRangeException.Raise ();
		AssertNotReached (); return (0);
	}
}

long	strtol (const String& s, int base)
{
	Require (s != "");
	size_t	ignored	=	1;
	return (strtol (s, ignored, base));
}




/*
 ********************************************************************************
 ************************************** strtoul *********************************
 ********************************************************************************
 */

#if		qGCC_GPlusPlusLib_MissingStrToUL
	// This really should be extern "C", but for some reason that causes the usage below
	// to fail - simplify and report the bug, and for now, just hope for the best...
	extern	/*"C"*/	unsigned long int strtoul(const char *nptr, char **endptr, int base);
#endif

unsigned	long	strtoul (const String& s, size_t& last, int base)
{
#if		qGCC_GPlusPlusLib_MissingStrToUL
	AssertNotImplemented ();
#else
	Require (s != "");
	char*				tmp		=	s.ToCString ();
	char*				tailPtr	=	Nil;
	unsigned	long	l		=	::strtoul (tmp, &tailPtr, base);
	last = (tailPtr - tmp);
	Assert (last <= s.GetLength ());
	delete (tmp);

	if (errno == 0) {
		return (l);
	}
	else {
		Assert (errno == ERANGE);
		errno = 0;
		gMathRangeException.Raise ();
		AssertNotReached (); return (0);
	}
#endif
}

unsigned	long	strtoul (const String& s, int base)
{
	Require (s != "");
	size_t	ignored	=	1;
	return (strtoul (s, ignored, base));
}






/*
 ********************************************************************************
 ************************************** ltostring *******************************
 ********************************************************************************
 */
String	ltostring (long l, int base)
{
	// This assumes ASCII...
	Require (base > 1);
	Require (base <= 35);	// otherwise we get rediculous characters - maybe limit should be bigger ???
	Boolean			isNeg	=	Boolean (l < 0);
	unsigned long	ul		=	isNeg? -l: l;		// I probably need to be more careful here???
	char			buf [1000];
	for (int i = 0; ul != 0; i++) {
		Assert (i < sizeof (buf));
		char	digit = char (ul % base);
		ul = ul / base;
		Assert (digit >= 0);
		if (digit < 10) {
			buf [i] = '0' + digit;
		}
		else {
			buf [i] = 'A' + digit - 10;
		}
	}
	if (i == 0) {
		Assert (l == 0);
		buf [i++] = '0';
	}
	buf [i] = '\0';

	// Since we built the string with the most signigicant part first, we must revese the digits
	char	reverse[sizeof (buf)];
	for (int ii = 0; ii < i; ii++) {
		reverse [ii] = buf [i-ii-1];
	}
	reverse [i] = '\0';
	if (isNeg) {
		return (String ("-") + reverse);
	}
	else {
		return (reverse);
	}
}




/*
 ********************************************************************************
 ************************************* ultostring *******************************
 ********************************************************************************
 */
String	ultostring (unsigned long ul, int base)
{
	// This assumes ASCII...
	Require (base > 1);
	Require (base <= 35);	// otherwise we get rediculous characters - maybe limit should be bigger ???
	char	buf [1000];
	for (int i = 0; ul != 0; i++) {
		Assert (i < sizeof (buf));
		char	digit	= char (ul % base);
		ul = ul / base;
		Assert (digit >= 0);
		if (digit < 10) {
			buf [i] = '0' + digit;
		}
		else {
			buf [i] = 'A' + digit - 10;
		}
	}
	if (i == 0) {
		buf [i++] = '0';
	}
	buf [i] = '\0';

	// Since we built the string with the most signigicant part first, we must revese the digits
	char	reverse[sizeof (buf)];
	for (int ii = 0; ii < i; ii++) {
		reverse [ii] = buf [i-ii-1];
	}
	reverse [i] = '\0';
	return (reverse);
}




/*
 ********************************************************************************
 ************************************* dltostring *******************************
 ********************************************************************************
 */
String	dtostring (double d, int precision)
{
	// not right - ingore precision - should check for errors!
	char	buf [1000];
	(void)sprintf (buf, "%f", d);

	// hack for now, but something like this does actually make sense...
	// strip trailing zeros...
	if (precision == -1 and strchr (buf, '.') != Nil) {
		for (int i = strlen (buf)-1; i > 0; i--) {
			if (buf [i] == '0') {
				buf [i] = '\0';
			}
			else {
				break;
			}
		}
		if (buf[i] == '.') {
			buf[i] = '\0';
		}
	}

	return (buf);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

