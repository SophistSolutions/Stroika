/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BrowserClass.cc,v 1.2 1992/09/01 17:42:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: BrowserClass.cc,v $
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

#include	"BrowserClass.hh"


#if		!qRealTemplatesAvailable
	Implement (Collection, BrowserClassPtr);
	Implement (Iterator, BrowserClassPtr);
	Implement (AbSequence, BrowserClassPtr);
	Implement (Array, BrowserClassPtr);
	Implement (Sequence_Array, BrowserClassPtr);
	Implement (Sequence, BrowserClassPtr);
#endif







/*
 ********************************************************************************
 *********************************** BrowserClass *******************************
 ********************************************************************************
 */

BrowserClass::BrowserClass (const String& name, BrowserScope& scope):
	BrowserType (name, scope),
	BrowserScope (&scope)
{
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

