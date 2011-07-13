/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BrowserScope.cc,v 1.2 1992/09/01 17:42:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: BrowserScope.cc,v $
 *		Revision 1.2  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"BrowserScope.hh"

#include	"BrowserType.hh"


#if		0 && !qRealTemplatesAvailable
Implement (Collection, BrowserClassPtr);
Implement (Sequence, BrowserClassPtr);
#endif	/*!qRealTemplatesAvailable*/




/*
 ********************************************************************************
 *********************************** BrowserScope *******************************
 ********************************************************************************
 */

BrowserScope::BrowserScope (BrowserScope* parentScope):
	fParentScope (parentScope)
{
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

