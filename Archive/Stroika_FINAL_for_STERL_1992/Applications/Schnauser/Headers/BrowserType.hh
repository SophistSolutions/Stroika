/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BrowserType__
#define	__BrowserType__

/*
 * $Header: /fuji/lewis/RCS/BrowserType.hh,v 1.3 1992/09/01 17:40:39 sterling Exp $
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
 *	$Log: BrowserType.hh,v $
 *		Revision 1.3  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */

#include	"String.hh"

#include	"SchnauserConfig.hh"



class	BrowserScope;
class	BrowserType {
	public:
		BrowserType (const String& name, BrowserScope& scope);

	private:
		String			fName;
		BrowserScope&	fScope;
};






#if		0 && !qRealTemplatesAvailable
typedef	BrowserType*	BrowserTypePtr;
Declare (Collection, BrowserTypePtr);
Declare (Sequence, BrowserTypePtr);
#endif	/*!qRealTemplatesAvailable*/




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__BrowserType__*/

