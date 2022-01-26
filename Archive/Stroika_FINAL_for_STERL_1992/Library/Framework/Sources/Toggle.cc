/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Toggle.cc,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: Toggle.cc,v $
// Revision 1.4  1992/09/08  15:34:00  lewis
// Renamed NULL -> Nil.
//
// Revision 1.3  1992/09/01  15:46:50  sterling
// Lots of Foundation changes.
//
// Revision 1.2  1992/07/03  02:20:13  lewis
// Use Panel:: to scope access to nested UpdateMode enum.
//
// Revision 1.1  1992/06/25  07:36:19  sterling
// Initial revision
//
 *
 */

#include	"Toggle.hh"






/*
 ********************************************************************************
 ************************************* Toggle ***********************************
 ********************************************************************************
 */
const	Boolean	Toggle::kOn		=	True;

Toggle::Toggle (ButtonController* controller):
	Button (controller),
	fOn (False)
{
}

Boolean	Toggle::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	Boolean	result = Button::Track (phase, mouseInButton);
	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		// tough choice for eNoUpdate here, but is only choice that allows subclasses to 
		// avoid flicker. In cases where the tracking is different than the on state (such
		// as Mac checkboxes), override Track (needed anyhow) and call refresh as needed
		SetOn (not GetOn (), eNoUpdate);
	}
	
	return (result);
}

void	Toggle::SetOn (Boolean on, Panel::UpdateMode updateMode)
{
	if (on != fOn) {
		SetOn_ (on, updateMode);
		if (GetController () != Nil) {
			GetController ()->ButtonPressed (this);
		}
	}
	Ensure (GetOn () == on);
}

void	Toggle::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	fOn = on;
	Refresh (updateMode);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

