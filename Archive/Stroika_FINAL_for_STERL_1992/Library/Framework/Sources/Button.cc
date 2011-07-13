/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Button.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Button.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/10  22:35:44  lewis
 *		Scoped time constants.
 *
 *		Revision 1.4  1992/07/08  03:04:15  lewis
 *		Renamed PointInside->Contains.
 *
 *		Revision 1.3  1992/07/03  00:19:06  lewis
 *		Minor cleanups, and Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  07:45:32  sterling
 *		Lots of changes including getting rid of invertable flag in ButtonTracker, and SetOn/GetOn
 *		from Buttons (moved that into subclass Toggle).
 *		Did something funny with GetEffectiveLive and EffectiveLiveChanged () - discuss with sterl.
 *		Added Pressed, and Pressing methods (maybe rename? - LGP checked in for Sterl).
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.21  1992/03/26  09:41:55  lewis
 *		Not rid of oldLive arg to EffectiveLiveChanged.
 *
 *		Revision 1.20  1992/03/22  21:20:32  lewis
 *		Do much less in Button::EffectiveLiveChanged () - only call refresh, and only
 *		if updateMode != eNoUpdate - profiling indicated this was a bottleneck.
 *		Maybe shoulnd even do that here - maybe only in concrete subclasses???
 *
 *		Revision 1.19  1992/03/19  16:43:39  lewis
 *		Get rid of Button override of EffectiveFocusedChanged, and new virtual function BuildFOcusedAdornment.
 *		Its up to subclasses to decide how to do this.
 *
 *		Revision 1.18  1992/03/13  16:35:02  lewis
 *		Be a EnableItem, a View, and optionally a FocusItem (qMouselessInterface) instead of inheriting
 *		from EnableView, and optionally FocusView.
 *
 *		Revision 1.17  1992/03/10  00:02:18  lewis
 *		Use new HandleKeyStroke () interface.
 *
 *		Revision 1.16  1992/03/05  21:35:21  sterling
 *		support for Borders
 *
 *		Revision 1.14  1992/02/21  20:09:06  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *		Got rid of support for non-ntested types.
 *
 *		Revision 1.13  1992/01/27  06:15:32  lewis
 *		Allow call to Button::SetOn_ even with same value as before, since this is a protected method, the
 *		subclasser hopefully knows what he's doing.
 *
 *		Revision 1.11  1992/01/14  06:09:12  lewis
 *		Update tracker for slight change in ctor interface.
 *
 *
 */



#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"EventManager.hh"

#include	"Button.hh"









/*
 ********************************************************************************
 ******************************* AbstractButton *********************************
 ********************************************************************************
 */
void	AbstractButton::SetController (ButtonController* buttonController)
{
	Require ((buttonController == Nil) or (fController == Nil));	
	fController = buttonController;	
	Ensure (fController == buttonController);
}

Boolean	AbstractButton::Tracking (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if (GetController () != Nil) {
		return (GetController ()->ButtonTracking (this, phase, mouseInButton));
	}
	return (False);
}

void	AbstractButton::Pressed ()
{
	if (GetController () != Nil) {
		GetController ()->ButtonPressed (this);
	}
}







/*
 ********************************************************************************
 **************************** ButtonController **********************************
 ********************************************************************************
 */
Boolean	ButtonController::ButtonTracking (AbstractButton* button, Tracker::TrackPhase phase, Boolean mouseInButton)
{
	ButtonController*	c	=	GetController ();
	if (c != Nil) {
		return (c->ButtonTracking (button, phase, mouseInButton));
	}
	return (False);
}

void	ButtonController::ButtonPressed (AbstractButton* button)
{
	ButtonController*	c	=	GetController ();
	if (c != Nil) {
		c->ButtonPressed (button);
	}
}










/*
 ********************************************************************************
 ************************************* Button ***********************************
 ********************************************************************************
 */

Button::Button (ButtonController* controller):
	AbstractButton (),
	View (),
	EnableItem (),
	fEnabled (True)
{
	SetController (controller);
}

Boolean	Button::TrackPress (const MousePressInfo& mouseInfo)
{
	if (not View::TrackPress (mouseInfo)) {
		(void)ButtonTracker (*this).TrackPress (MousePressInfo (mouseInfo, LocalToLocal (mouseInfo.fPressAt, GetParentView ())));
	}
	return (True);
}

Boolean	Button::Track (Tracker::TrackPhase /*phase*/, Boolean /*mouseInButton*/)
{
	return (False);
}

Boolean	Button::GetLive () const
{
	return (Boolean (GetEnabled () and View::GetLive ()));
}

Boolean	Button::GetEnabled_ () const
{
	return (fEnabled);
}

void	Button::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	if (updateMode != eNoUpdate) {
		Refresh (updateMode);
	}
	View::EffectiveLiveChanged (newLive, updateMode);
}

void	Button::SetEnabled_ (Boolean enabled, Panel::UpdateMode updateMode)
{
	Boolean	oldLive = GetEffectiveLive ();
	fEnabled = enabled;
	
	if (oldLive != GetEffectiveLive ()) {
		EffectiveLiveChanged (not oldLive, updateMode);
	}
}





/*
 ********************************************************************************
 ******************************** ButtonTracker *********************************
 ********************************************************************************
 */

ButtonTracker::ButtonTracker (Button& button) :
	Tracker (*button.GetParentView (), *button.GetParentView ()->GetTablet (), button.GetParentView ()->LocalToTablet (kZeroPoint)),
	fButton (button)
{
	RequireNotNil (button.GetTablet ());
	SetTrackNonMovement (True);
	SetHysteresis (kZeroPoint);
	SetTimeOut (Time (0));
	SetFeedbackClip (button.LocalToEnclosure (button.GetVisibleArea ()));
}

void	ButtonTracker::TrackFeedback (TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/, const Point& /*next*/)
{
}

Boolean	ButtonTracker::TrackResult (Tracker::TrackPhase phase, const Point& /*anchor*/, const Point& previous, const Point& next)
{
	Boolean	wasIn = fButton.Contains (previous);
	Boolean	newIn = fButton.Contains (next);	
	Boolean aborted = fButton.Tracking (phase, newIn);

	if ((not aborted) and (phase != Tracker::eTrackMove) or (wasIn != newIn)) {
		aborted = fButton.Track (phase, newIn);
	}
	
	return (aborted);
}

void	ButtonTracker::TrackConstrain (TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/, Point& /*next*/)
{
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

