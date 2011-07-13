/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BrowserView.cc,v 1.2 1992/09/01 17:42:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: BrowserView.cc,v $
 *		Revision 1.2  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *		Revision 1.1  1992/06/30  04:00:31  lewis
 *		Initial revision
 *
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"DeskTop.hh"
#include	"Grid.hh"
#include	"MenuOwner.hh"
#include	"PixelMapButton.hh"
#include	"Shell.hh"

#include	"PickList.hh"

#include	"BrowserView.hh"



#if		!qRealTemplatesAvailable
Implement (Collection, BrowserViewPtr);
Implement (Sequence, BrowserViewPtr);
#endif	/*!qRealTemplatesAvailable*/



/*
 ********************************************************************************
 ************************************* BrowserView ******************************
 ********************************************************************************
 */

BrowserView::BrowserView ():
	fName (kEmptyString)
{
}

BrowserView::~BrowserView ()
{
}

String	BrowserView::GetName () const
{
	return (fName);
}

void	BrowserView::SetName (const String& name)
{
	fName = name;	 
}

PixelMap	BrowserView::GetIcon () const
{
	return (GetIcon_ ());
}





/*
 ********************************************************************************
 ************************************ BrowserViews ******************************
 ********************************************************************************
 */
BrowserViews::BrowserViews ()
{
}

BrowserViews::~BrowserViews ()
{
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

