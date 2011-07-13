/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Slider.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Slider.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/03  02:16:31  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.3  1992/07/02  04:59:08  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  08:39:15  sterling
 *		Lots of changes - see header.
 *
 *		Revision 1.14  1992/06/09  14:24:36  lewis
 *		Sterl had me comment out an assertion that was failing - he will fix more properly later - in
 *		SetMax().
 *
 *		Revision 1.13  92/04/30  13:26:00  13:26:00  sterling (Sterling Wight)
 *		use AbstractBordered
 *		
 *		Revision 1.12  92/03/26  09:44:25  09:44:25  lewis (Lewis Pringle)
 *		Reduced args to EffectiveLiveChanged - no oldLive arg anymore.
 *		
 *		Revision 1.11  1992/03/13  16:06:58  lewis
 *		Slider now inherits from EnableItem and View, instead of EnableView.
 *
 *		Revision 1.10  1992/02/21  20:21:59  lewis
 *		Comment, and Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.8  1992/02/04  07:45:40  lewis
 *		Add #if qDebug around Invariant_ definition.
 *
 *		Revision 1.7  1992/02/04  04:58:34  sterling
 *		temp hack to setvalue in setminmax, should pass in instead
 *
 *		Revision 1.6  1992/02/03  21:51:11  sterling
 *		massive interface changes using virtual protected methods and invariants
 *
 *
 */



#include	"Slider.hh"

#if		!qRealTemplatesAvailable
	Implement (Iterator, SliderPtr);
	Implement (Collection, SliderPtr);
	Implement (AbSequence, SliderPtr);
	Implement (Array, SliderPtr);
	Implement (Sequence_Array, SliderPtr);
	Implement (Sequence, SliderPtr);
#endif





/*
 ********************************************************************************
 ************************************* SliderBase *******************************
 ********************************************************************************
 */
SliderBase::SliderBase () :
	fController (Nil)
{
}

SliderBase::~SliderBase ()
{
}
		
SliderController*	SliderBase::GetSliderController () const
{
	return (fController);
}

void	SliderBase::SetSliderController (SliderController* sliderController)
{
	Require ((sliderController == Nil) or (GetSliderController () == Nil));
	fController = sliderController;
	Ensure (GetSliderController () == sliderController);
}

Boolean	SliderBase::Changing (Tracker::TrackPhase phase, Real oldValue, Real newValue)
{
	if (GetSliderController () != Nil) {
		return (GetSliderController ()->SliderChanging (*this, phase, oldValue, newValue));
	}
	return (False);
}

void	SliderBase::Changed (Real oldValue, Real newValue)
{
	if (GetSliderController () != Nil) {
		GetSliderController ()->SliderChanged (*this, oldValue, newValue);
	}
}





/*
 ********************************************************************************
 ********************************** SliderController ****************************
 ********************************************************************************
 */
SliderController::SliderController ()
{
}

Boolean	SliderController::SliderChanging (SliderBase& theSlider, Tracker::TrackPhase phase, Real oldValue, Real newValue)
{
	if (GetSliderController () != Nil) {
		return (GetSliderController ()->SliderChanging (theSlider, phase, oldValue, newValue));
	}
	return (False);
}

void	SliderController::SliderChanged (SliderBase& theSlider, Real oldValue, Real newValue)
{
	if (GetSliderController () != Nil) {
		GetSliderController ()->SliderChanged (theSlider, oldValue, newValue);
	}
}
	







/*
 ********************************************************************************
 ***************************************** Slider *******************************
 ********************************************************************************
 */

Slider::Slider (SliderController* sliderController):
	SliderBase (),
	EnableItem (),
	View (),
	fEnabled (True)
{
	SetSliderController (sliderController);
}

Real	Slider::GetMin () const
{
	Invariant ();
	return (GetMin_ ());
}

void	Slider::SetMin (Real newMin, Panel::UpdateMode updateMode)
{
	SetBounds (newMin, GetMax (), GetValue (), updateMode);
}

Real	Slider::GetMax () const
{
	Invariant ();
	return (GetMax_ ());
}

void	Slider::SetMax (Real newMax, Panel::UpdateMode updateMode)
{
	SetBounds (GetMin (), newMax, GetValue (), updateMode);
}

Real	Slider::GetValue () const
{
	Invariant ();
	return (GetValue_ ());
}

void	Slider::SetValue (Real newValue, Panel::UpdateMode updateMode)
{
	Require (newValue >= GetMin_ ());
	Require (newValue <= GetMax_ ());
	
	Invariant ();
	if (newValue != GetValue ()) {
		Real	oldValue	=	GetValue ();
		SetValue_ (newValue, updateMode);
		Changed (oldValue, GetValue_ ());
		Invariant ();
	}
}

void	Slider::SetBounds (Real newMin, Real newMax, Real newValue, Panel::UpdateMode updateMode)
{
	Require (newMin <= newMax);
	
	Invariant ();
	if ((newMin != GetMin ()) or (newMax != GetMax ())) {
		SetBounds_ (newMin, newMax, newValue, updateMode);
		Invariant ();
	}
	Ensure (newMin == GetMin ());
	Ensure (newMax == GetMax ());
}

Boolean	Slider::GetLive () const
{
	return (Boolean (GetEnabled () and View::GetLive ()));
}

Boolean	Slider::GetEnabled_ () const
{
	return (fEnabled);
}

void	Slider::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	View::EffectiveLiveChanged (newLive, eNoUpdate);
	Refresh (updateMode);
}

void	Slider::SetEnabled_ (Boolean enabled, Panel::UpdateMode updateMode)
{
	Boolean	oldLive = GetEffectiveLive ();
	fEnabled = enabled;
	
	if (oldLive != GetEffectiveLive ()) {
		EffectiveLiveChanged (not oldLive, updateMode);
	}
}

#if		qDebug
void	Slider::Invariant_ () const
{
	Require (GetMin_ () <= GetMax_ ());
	Require (GetValue_ () >= GetMin_ ());
	Require (GetValue_ () <= GetMax_ ());
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

