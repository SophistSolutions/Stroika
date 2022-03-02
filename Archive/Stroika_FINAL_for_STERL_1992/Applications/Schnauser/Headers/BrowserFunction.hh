/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BrowserFunction__
#define	__BrowserFunction__

/*
 * $Header: /fuji/lewis/RCS/BrowserFunction.hh,v 1.3 1992/09/01 17:40:39 sterling Exp $
 *
 * Description:
 *		This is a browser view, of a list of classes in a project.
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: BrowserFunction.hh,v $
 *		Revision 1.3  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */

#include	"Sequence.hh"

#include	"BrowserScope.hh"
#include	"BrowserType.hh"



// Need to think out how to deal with overloading - make signature an argument too???
class	BrowserFunction {
	public:
		BrowserFunction (const String& name, BrowserScope& scope);

	private:
};


#if		!qRealTemplatesAvailable
typedef	BrowserFunction*	BrowserFunctionPtr;
Declare (Collection, BrowserFunctionPtr);
Declare (Sequence, BrowserFunctionPtr);
#endif	/*!qRealTemplatesAvailable*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__BrowserFunction__*/

