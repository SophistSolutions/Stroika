/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapButton.cc,v 1.7 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapButton.cc,v $
 *		Revision 1.7  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.5  1992/07/16  16:46:10  sterling
 *		changed GUI to UI
 *
 *		Revision 1.3  1992/07/03  01:06:01  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:24:41  sterling
 *		Lots of changes...
 *
 *		Revision 1.18  1992/05/01  01:39:47  lewis
 *		Get rid of unneeded GetLocalRegion () param to BitBlit - we are already automaticcaly clipped to
 *		our VisibleRegion which is close enuf. (maybe not ??). Anyhow, triggers incomplete code in
 *		tablet to use other version - fix that soon - but for now, just avoid it.
 *
 *		Revision 1.17  92/04/09  23:21:06  23:21:06  sterling (Sterling Wight)
 *		pulled out region clipping from BitBlit in draw cuz it caused problems
 *		and should be unnecessary. However, it should have been harmless so we need
 *		to check the BitBlit calls in View
 *		
 *		Revision 1.16  92/03/26  09:50:17  09:50:17  lewis (Lewis Pringle)
 *		Get rid of EffecitveFontChanged - did nothing and that is now default.
 *		
 *		Revision 1.14  1992/03/05  20:45:38  sterling
 *		support for borders
 *
 *		Revision 1.13  1992/02/24  06:47:22  lewis
 *		Fix coords passed to ButtonTracker -- sterl.
 *
 *		Revision 1.12  1992/02/21  20:13:49  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.11  1992/02/15  05:33:24  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.10  1992/02/03  19:43:34  sterling
 *		switched constructor args
 *
 *
 */



#include	"StreamUtils.hh"

#include	"Pallet.hh"
#include	"PixelMapButton.hh"








/*
 ********************************************************************************
 ******************************* AbstractPixelMapButton *************************
 ********************************************************************************
 */
AbstractPixelMapButton::AbstractPixelMapButton (ButtonController* controller) :
	Toggle (controller)
{
}

AbstractPixelMapButton::~AbstractPixelMapButton ()
{
}









/*
 ********************************************************************************
 ****************************** AbstractPixelMapButton_MacUI *******************
 ********************************************************************************
 */
AbstractPixelMapButton_MacUI::AbstractPixelMapButton_MacUI (ButtonController* controller) :
	AbstractPixelMapButton (controller)
{
}

AbstractPixelMapButton_MacUI::~AbstractPixelMapButton_MacUI ()
{
}







/*
 ********************************************************************************
 ************************** PixelMapButton_MacUI_Portable **********************
 ********************************************************************************
 */
PixelMapButton_MacUI_Portable::PixelMapButton_MacUI_Portable (const PixelMap& pixelMap, ButtonController* controller):
	AbstractPixelMapButton_MacUI (controller),
	fPixelMap (pixelMap)
{
}

PixelMapButton_MacUI_Portable::PixelMapButton_MacUI_Portable (ButtonController* controller):
	AbstractPixelMapButton_MacUI (controller),
	fPixelMap (kDefaultPixelMap)
{
}

PixelMapButton_MacUI_Portable::~PixelMapButton_MacUI_Portable ()
{
}

void	PixelMapButton_MacUI_Portable::Draw (const Region& /*update*/)
{
	Rect	pmBounds	=	GetPixelMap ().GetBounds ();
	Point	inset = GetBorder () + GetMargin ();
	BitBlit (GetPixelMap (), pmBounds, GetLocalExtent ().InsetBy (inset), eCopyTMode/*, GetLocalRegion ()*/);

	if (GetOn ()) {
//		Invert ();
		Hilight ();	// should be invert dammit!!!	(24 bit color prob)
	}
	if (not GetEffectiveLive ()) {
		Gray ();
	}
	DrawMacBorder (GetLocalExtent (), GetBorder ());
}

Point	PixelMapButton_MacUI_Portable::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	p	=	GetPixelMap ().GetBounds ().GetSize ();
	return (p + GetBorder ()*2 + GetMargin ()*2);
}

void	PixelMapButton_MacUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	Require (GetOn () != on);
	AbstractPixelMapButton_MacUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		Hilight ();	// should be invert dammit!!!	(24 bit color prob)
	}
	else {
		Refresh (updateMode);
	}
}

const PixelMap&	PixelMapButton_MacUI_Portable::GetPixelMap () const
{
	return (fPixelMap);
}

void	PixelMapButton_MacUI_Portable::SetPixelMap (const PixelMap& pixelMap, Panel::UpdateMode updateMode)
{
	fPixelMap = pixelMap;
	Refresh (updateMode);
}

Boolean	PixelMapButton_MacUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	Hilight ();	// should be invert dammit!!!	(24 bit color prob)
	return (AbstractPixelMapButton_MacUI::Track (phase, mouseInButton));
}






/*
 ********************************************************************************
 ***************************** AbstractPixelMapButton_MotifUI ******************
 ********************************************************************************
 */
AbstractPixelMapButton_MotifUI::AbstractPixelMapButton_MotifUI (ButtonController* controller) :
	AbstractPixelMapButton (controller)
{
}

AbstractPixelMapButton_MotifUI::~AbstractPixelMapButton_MotifUI ()
{
}

Boolean	AbstractPixelMapButton_MotifUI::HandleKeyStroke (const KeyStroke& keyStroke)
{
// Maybe should have some auxiliary functions (static maybe) in FocusItem to say which keystrokes get
// interpretted as being part of the mouseless interface, and how they get interpretted???
	if ((keyStroke == KeyStroke::kEnter) or (keyStroke == KeyStroke::kReturn) or (keyStroke == KeyStroke::kTab)) {
		AbstractPixelMapButton::SetOn (not GetOn (), eImmediateUpdate);
		return (True);
	}
	return (False);
}

Boolean	AbstractPixelMapButton_MotifUI::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	refocused = False;
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		refocused = True;
	}

	return (Boolean (AbstractPixelMapButton::TrackPress (mouseInfo) or refocused));
}

void	AbstractPixelMapButton_MotifUI::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractPixelMapButton::EffectiveLiveChanged (newLive, updateMode);
}

Boolean	AbstractPixelMapButton_MotifUI::GetEffectiveLive () const
{
	// only View overrides this, but C++ insists we disambiguate anyhow
	return (View::GetEffectiveLive ());
}

Boolean	AbstractPixelMapButton_MotifUI::GetLive () const
{
	return (AbstractPixelMapButton::GetLive ());
}







/*
 ********************************************************************************
 ************************* PixelMapButton_MotifUI_Portable *********************
 ********************************************************************************
 */
PixelMapButton_MotifUI_Portable::PixelMapButton_MotifUI_Portable (const PixelMap& pixelMap, ButtonController* controller) :
	AbstractPixelMapButton_MotifUI (controller),
	fPixelMap (pixelMap)
{
}

PixelMapButton_MotifUI_Portable::PixelMapButton_MotifUI_Portable (ButtonController* controller):
	AbstractPixelMapButton_MotifUI (controller),
	fPixelMap (kDefaultPixelMap)
{
}

PixelMapButton_MotifUI_Portable::~PixelMapButton_MotifUI_Portable ()
{
}

void	PixelMapButton_MotifUI_Portable::Draw (const Region& /*update*/)
{
	Rect	pmBounds	=	GetPixelMap ().GetBounds ();
	Point	inset = GetBorder () + GetMargin ();
//	BitBlit (GetPixelMap (), pmBounds, pmBounds - inset, eCopyTMode);
	BitBlit (GetPixelMap (), pmBounds, GetLocalExtent ().InsetBy (inset), eCopyTMode/*, GetLocalRegion ()*/);
	if (GetOn ()) {
		Invert ();
	}
	if (not Button::GetEffectiveLive ()) {
		Gray ();
	}
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (GetLocalExtent (), GetBorder (), c, GetPlane ());
}

Point	PixelMapButton_MotifUI_Portable::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	p	=	GetPixelMap ().GetBounds ().GetSize ();
	return (p + GetBorder ()*2 + GetMargin ()*2);
}

void	PixelMapButton_MotifUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode update)
{
	Require (GetOn () != on);
	AbstractPixelMapButton_MotifUI::SetOn_ (on, eNoUpdate);
	if (update == eImmediateUpdate) {
		Invert ();
	}
	else {
		Refresh (update);
	}
}

const PixelMap&	PixelMapButton_MotifUI_Portable::GetPixelMap () const
{
	return (fPixelMap);
}

void	PixelMapButton_MotifUI_Portable::SetPixelMap (const PixelMap& pixelMap, Panel::UpdateMode updateMode)
{
	fPixelMap = pixelMap;
	Refresh (updateMode);
}

Boolean	PixelMapButton_MotifUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	Invert ();
	return (AbstractPixelMapButton_MotifUI::Track (phase, mouseInButton));
}



/*
 ********************************************************************************
 ***************************** AbstractPixelMapButton_WinUI ******************
 ********************************************************************************
 */
AbstractPixelMapButton_WinUI::AbstractPixelMapButton_WinUI (ButtonController* controller) :
	AbstractPixelMapButton (controller)
{
}

AbstractPixelMapButton_WinUI::~AbstractPixelMapButton_WinUI ()
{
}

Boolean	AbstractPixelMapButton_WinUI::HandleKeyStroke (const KeyStroke& keyStroke)
{
// Maybe should have some auxiliary functions (static maybe) in FocusItem to say which keystrokes get
// interpretted as being part of the mouseless interface, and how they get interpretted???
	if ((keyStroke == KeyStroke::kEnter) or (keyStroke == KeyStroke::kReturn) or (keyStroke == KeyStroke::kTab)) {
		AbstractPixelMapButton::SetOn (not GetOn (), eImmediateUpdate);
		return (True);
	}
	return (False);
}

Boolean	AbstractPixelMapButton_WinUI::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	refocused = False;
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		refocused = True;
	}

	return (Boolean (AbstractPixelMapButton::TrackPress (mouseInfo) or refocused));
}

void	AbstractPixelMapButton_WinUI::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractPixelMapButton::EffectiveLiveChanged (newLive, updateMode);
}

Boolean	AbstractPixelMapButton_WinUI::GetEffectiveLive () const
{
	// only View overrides this, but C++ insists we disambiguate anyhow
	return (View::GetEffectiveLive ());
}

Boolean	AbstractPixelMapButton_WinUI::GetLive () const
{
	return (AbstractPixelMapButton::GetLive ());
}







/*
 ********************************************************************************
 ************************* PixelMapButton_WinUI_Portable *********************
 ********************************************************************************
 */
PixelMapButton_WinUI_Portable::PixelMapButton_WinUI_Portable (const PixelMap& pixelMap, ButtonController* controller) :
	AbstractPixelMapButton_WinUI (controller),
	fPixelMap (pixelMap)
{
}

PixelMapButton_WinUI_Portable::PixelMapButton_WinUI_Portable (ButtonController* controller):
	AbstractPixelMapButton_WinUI (controller),
	fPixelMap (kDefaultPixelMap)
{
}

PixelMapButton_WinUI_Portable::~PixelMapButton_WinUI_Portable ()
{
}

void	PixelMapButton_WinUI_Portable::Draw (const Region& /*update*/)
{
	Rect	pmBounds	=	GetPixelMap ().GetBounds ();
	Point	inset = GetBorder () + GetMargin ();
//	BitBlit (GetPixelMap (), pmBounds, pmBounds - inset, eCopyTMode);
	BitBlit (GetPixelMap (), pmBounds, GetLocalExtent ().InsetBy (inset), eCopyTMode/*, GetLocalRegion ()*/);
	if (GetOn ()) {
		Invert ();
	}
	if (not Button::GetEffectiveLive ()) {
		Gray ();
	}
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawWinBorder (GetLocalExtent (), GetBorder (), c, GetPlane ());
}

Point	PixelMapButton_WinUI_Portable::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	p	=	GetPixelMap ().GetBounds ().GetSize ();
	return (p + GetBorder ()*2 + GetMargin ()*2);
}

void	PixelMapButton_WinUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode update)
{
	Require (GetOn () != on);
	AbstractPixelMapButton_WinUI::SetOn_ (on, eNoUpdate);
	if (update == eImmediateUpdate) {
		Invert ();
	}
	else {
		Refresh (update);
	}
}

const PixelMap&	PixelMapButton_WinUI_Portable::GetPixelMap () const
{
	return (fPixelMap);
}

void	PixelMapButton_WinUI_Portable::SetPixelMap (const PixelMap& pixelMap, Panel::UpdateMode updateMode)
{
	fPixelMap = pixelMap;
	Refresh (updateMode);
}

Boolean	PixelMapButton_WinUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	Invert ();
	return (AbstractPixelMapButton_WinUI::Track (phase, mouseInButton));
}







/*
 ********************************************************************************
 ********************************* PixelMapButton_MacUI *************************
 ********************************************************************************
 */
PixelMapButton_MacUI::PixelMapButton_MacUI (const PixelMap& pixelMap, ButtonController* controller) :
	PixelMapButton_MacUI_Portable (pixelMap, controller)
{
}

PixelMapButton_MacUI::PixelMapButton_MacUI (ButtonController* controller) :
	PixelMapButton_MacUI_Portable (controller)
{
}

#if		qSoleInlineVirtualsNotStripped
PixelMapButton_MacUI::~PixelMapButton_MacUI ()
{
}
#endif




/*
 ********************************************************************************
 ********************************* PixelMapButton_MotifUI *************************
 ********************************************************************************
 */
PixelMapButton_MotifUI::PixelMapButton_MotifUI (const PixelMap& pixelMap, ButtonController* controller) :
	PixelMapButton_MotifUI_Portable (pixelMap, controller)
{
}

PixelMapButton_MotifUI::PixelMapButton_MotifUI (ButtonController* controller) :
	PixelMapButton_MotifUI_Portable (controller)
{
}

#if		qSoleInlineVirtualsNotStripped
PixelMapButton_MotifUI::~PixelMapButton_MotifUI ()
{
}
#endif




/*
 ********************************************************************************
 ********************************* PixelMapButton_WinUI *************************
 ********************************************************************************
 */
PixelMapButton_WinUI::PixelMapButton_WinUI (const PixelMap& pixelMap, ButtonController* controller) :
	PixelMapButton_WinUI_Portable (pixelMap, controller)
{
}

PixelMapButton_WinUI::PixelMapButton_WinUI (ButtonController* controller) :
	PixelMapButton_WinUI_Portable (controller)
{
}

#if		qSoleInlineVirtualsNotStripped
PixelMapButton_WinUI::~PixelMapButton_WinUI ()
{
}
#endif





/*
 ********************************************************************************
 ************************************* PixelMapButton ***************************
 ********************************************************************************
 */
PixelMapButton::PixelMapButton (const PixelMap& pixelMap, ButtonController* controller):
#if		qMacUI
	PixelMapButton_MacUI (pixelMap, controller)
#elif		qMotifUI
	PixelMapButton_MotifUI (pixelMap, controller)
#elif		qWinUI
	PixelMapButton_WinUI (pixelMap, controller)
#endif
{
}

PixelMapButton::PixelMapButton (ButtonController* controller):
#if		qMacUI
	PixelMapButton_MacUI (controller)
#elif		qMotifUI
	PixelMapButton_MotifUI (controller)
#elif		qWinUI
	PixelMapButton_WinUI (controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
PixelMapButton::~PixelMapButton ()
{
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

