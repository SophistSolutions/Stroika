/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DoTranslate__
#define	__DoTranslate__

/*
 * $Header: /fuji/lewis/RCS/DoTranslate.hh,v 1.3 1992/10/10 03:13:02 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: DoTranslate.hh,v $
 *		Revision 1.3  1992/10/10  03:13:02  lewis
 *		Support templates, and include SequenceOfString_hh
 *
 *		Revision 1.2  1992/10/02  04:17:09  lewis
 *		Add Parameterize
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"Sequence.hh"


class	String;
class	istream;
class	ostream;
#if     !qRealTemplatesAvailable
#include    "TFileMap.hh"
#include    SequenceOfString_hh
#endif
void	BuildBoilerplate (const String& fromFileName, istream& from, ostream& to,
#if		qRealTemplatesAvailable
						  const Sequence<String>& arguments, const Sequence<String>& argNames,
#else
						  const Sequence(String)& arguments, const Sequence(String)& argNames,
#endif
						  Boolean doingMapping);
String	Parameterize (const String& s,
#if		qRealTemplatesAvailable
					  const Sequence<String>& arguments, const Sequence<String>& argNames);
#else
					  const Sequence(String)& arguments, const Sequence(String)& argNames);
#endif



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__DoTranslate__*/
