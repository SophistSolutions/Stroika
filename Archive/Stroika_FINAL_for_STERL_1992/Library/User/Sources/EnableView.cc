/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EnableView.cc,v 1.2 1992/09/01 15:58:05 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: EnableView.cc,v $
 *		Revision 1.2  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/03/26  09:24:26  lewis
 *		Got rid of oldLive first argument to EffectiveLiveChanged () method.
 *
 *		Revision 1.1  1992/03/13  16:00:55  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"EnableView.hh"




/*
 ********************************************************************************
 ********************************** EnableView **********************************
 ********************************************************************************
 */
EnableView::EnableView (Boolean enabled):
	fEnabled (enabled)
{
}

Boolean	EnableView::GetLive () const
{
	return (Boolean (GetEnabled () and View::GetLive ()));
}

Boolean	EnableView::GetEnabled_ () const
{
	return (fEnabled);
}

void	EnableView::EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode)
{
	View::EffectiveLiveChanged (newLive, eNoUpdate);
	Refresh (updateMode);
}

void	EnableView::SetEnabled_ (Boolean enabled, UpdateMode updateMode)
{
	Boolean	oldLive = GetEffectiveLive ();
	fEnabled = enabled;
	
	if (oldLive != GetEffectiveLive ()) {
		EffectiveLiveChanged (not oldLive, updateMode);
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


