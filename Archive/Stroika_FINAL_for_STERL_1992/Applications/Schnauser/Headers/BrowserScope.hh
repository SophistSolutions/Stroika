/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BrowserScope__
#define	__BrowserScope__

/*
 * $Header: /fuji/lewis/RCS/BrowserScope.hh,v 1.3 1992/09/01 17:40:39 sterling Exp $
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
 *	$Log: BrowserScope.hh,v $
 *		Revision 1.3  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */

#include	"Sequence.hh"

#include	"SchnauserConfig.hh"


class	BrowserScope {
	public:
		BrowserScope (BrowserScope* parentScope);

		// has
		//		list of declarations
		// 		list of functions
		//		list of classes/structs
		//		list of enums
		// ??? Roughly...
		//		
	private:
		BrowserScope* fParentScope;
};





#if		0 && !qRealTemplatesAvailable
// something different here??
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

