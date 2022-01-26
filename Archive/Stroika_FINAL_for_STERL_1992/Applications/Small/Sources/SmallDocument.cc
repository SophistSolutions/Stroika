/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SmallDocument.cc,v 1.3 1992/09/08 17:38:33 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SmallDocument.cc,v $
 *		Revision 1.3  1992/09/08  17:38:33  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  17:44:49  sterling
 *		*** empty log message ***
 *
 *		
 *
 *
 *
 */



#include	"SmallWindow.hh"
#include	"SmallDocument.hh"




/*
 ********************************************************************************
 ********************************* SmallDocument ********************************
 ********************************************************************************
 */

SmallDocument::SmallDocument ():
	Document (),
	fWindow (Nil)
{
	fWindow = new SmallWindow (*this);
}

SmallDocument::~SmallDocument ()
{
	// window should be destroyed by now????
}

void	SmallDocument::DoNewState ()
{
	AssertNotNil (fWindow);
	fWindow->Select ();
}

String	SmallDocument::GetPrintName ()
{
	return ("Small Document");
}

void	SmallDocument::TryToClose ()
{
	Close ();
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

