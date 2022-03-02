/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FocusView.cc,v 1.5 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: FocusView.cc,v $
 *		Revision 1.5  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/04  14:29:56  lewis
 *		Take advantage of new shape letter/envelope support- pass by value, instead of reference.
 *
 *		Revision 1.3  1992/03/26  15:16:07  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of
 *		 oldFocused first argument.
 *
 *		Revision 1.2  1992/03/26  09:24:26  lewis
 *		Got rid of oldLive first argument to EffectiveLiveChanged () method.
 *
 * 		Revision 1.1  1992/03/13  16:00:55  lewis
 * 		Initial revision
 *
 *
 */

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Pallet.hh"
#include	"Shape.hh"

#include	"Adornment.hh"

#include	"FocusView.hh"




/*
 ********************************************************************************
 ********************************** FocusView ***********************************
 ********************************************************************************
 */
FocusView::FocusView ():
	FocusItem (),
	EnableView (EnableView::kEnabled),
	fFocusAdornment (Nil)
{
}

void	FocusView::EffectiveFocusChanged (Boolean newFocused, UpdateMode updateMode)
{
	if (newFocused) {
		Assert (fFocusAdornment == Nil);
		fFocusAdornment = BuildFocusAdornment_ ();
		AssertNotNil (fFocusAdornment);
		AddAdornment (fFocusAdornment, updateMode);
	}
	else {
		AssertNotNil (fFocusAdornment);
		RemoveAdornment (fFocusAdornment, updateMode);
		fFocusAdornment = Nil;
	}
	FocusItem::EffectiveFocusChanged (newFocused, updateMode);
}

Boolean	FocusView::GetLive () const
{
	return (EnableView::GetLive ());
}

Boolean	FocusView::GetEffectiveLive () const
{
	return (EnableView::GetEffectiveLive ());
}

void	FocusView::EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode)
{
	EnableView::EffectiveLiveChanged (newLive, updateMode);
}

Boolean	FocusView::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	refocused = False;
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		refocused = True;
	}

	return (Boolean (View::TrackPress (mouseInfo) or refocused));
}

Adornment*	FocusView::BuildFocusAdornment_ ()
{
// Should this be light gray outline - maybe GUI depenedent???? On mac i like light gray better?
// Should it be pure virtual ? who takes advantage of default implementation???
// Maybe default is Nil ????
	static	const	Pen	kGrayPen = Pen (PalletManager::Get ().MakeTileFromColor (kGrayColor), Point (1, 1));
	return (new ShapeAdornment (Rectangle (), kGrayPen));
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
