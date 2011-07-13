/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BrowserType.cc,v 1.2 1992/09/01 17:42:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: BrowserType.cc,v $
 *		Revision 1.2  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  1992/06/30  23:12:28  lewis
 *		Initial revision
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
 *********************************** BrowserClass *******************************
 ********************************************************************************
 */

BrowserType::BrowserType (const String& name, BrowserScope& scope):
	fName (name),
	fScope (scope)
{
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

