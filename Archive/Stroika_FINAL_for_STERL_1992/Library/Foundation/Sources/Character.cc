/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Character.cc,v 1.6 1992/12/10 05:54:42 lewis Exp $
 *
 * TODO:
 *			-	everything.
 *
 *			-	Think about ctype...
 *
 * Changes:
 *	$Log: Character.cc,v $
 *		Revision 1.6  1992/12/10  05:54:42  lewis
 *		*** empty log message ***
 *
 *		Revision 1.5  1992/12/10  05:53:38  lewis
 *		Add wchar_t CTOR to this file - not quite right - must use locale stuff???
 *
 *		Revision 1.4  1992/12/07  04:07:50  lewis
 *		Added stream inserter/extractor for hello world demo.
 *
 *		Revision 1.3  1992/10/10  04:27:57  lewis
 *		Got rid of CollectionSize typedef - use size_t directly instead.
 *
 *		Revision 1.2  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 */

#include	<iostream.h>

#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Memory.hh"

#include	"Character.hh"







/*
 ********************************************************************************
 ************************************* Character*********************************
 ********************************************************************************
 */

Character::Character (wchar_t wc) :
	fCharacterCode (wc)//,
	//fCharacterCode ()
{
	// use some sort of locale stuff to get the current character set...
	AssertNotImplemented();
}





/*
 ********************************************************************************
 ************************************* operator<< *******************************
 ********************************************************************************
 */
ostream&	operator<< (ostream& out, Character aChar)
{
// Crappy, temp hack implementation...Needs more thought to do right
	out << aChar.GetAsciiCode ();
	return (out);
}




/*
 ********************************************************************************
 ************************************* operator>> *******************************
 ********************************************************************************
 */
istream&	operator>> (istream& in, Character& aChar)
{
// Crappy, temp hack implementation...Needs more thought to do right
	char	c;
	in >> c;
	if (in) {
		aChar = c;
	}
	return (in);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

