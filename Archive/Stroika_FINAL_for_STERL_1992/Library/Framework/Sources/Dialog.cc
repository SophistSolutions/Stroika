/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Dialog.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Dialog.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/03  00:23:09  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.30  1992/05/18  17:21:07  lewis
 *		On DTOR call SetCurrentFocus (Nil) and remove each focus item - really should have kept a reference around to one we added,
 *		and delete that.
 *
 *		Revision 1.29  92/04/09  23:16:52  23:16:52  sterling (Sterling Wight)
 *		hacked setting original focus to coincide with TextEdit hacks
 *		for the Macintosh (sigh)
 *		
 *		Revision 1.28  92/04/07  16:45:45  16:45:45  sterling (Sterling Wight)
 *		made resizing alter size of maindialogview correctly
 *		
 *		Revision 1.27  92/03/26  18:35:22  18:35:22  lewis (Lewis Pringle)
 *		In DialogMainView: Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *		
 *		Revision 1.26  1992/03/26  09:47:49  lewis
 *		Got rid of oldLive arg to EffectiveLiveChanged ().
 *
 *		Revision 1.24  1992/03/16  16:02:59  sterling
 *		fixed keyboard support for mac
 *
 *		Revision 1.23  1992/03/11  08:11:07  lewis
 *		Fix HandleKeyStroke method - remove temp hack done before, and fix new hack with option key
 *		to allow returns to be entered with a default button (really dont like the hack).
 *
 *		Revision 1.20  1992/03/10  00:12:27  lewis
 *		Use new HandleKeyStroke () interface.
 *
 *		Revision 1.19  1992/03/05  20:25:08  sterling
 *		made DialogMainView a GroupView
 *
 *		Revision 1.18  1992/02/15  05:31:51  sterling
 *		made autofocus at startup
 *
 *		Revision 1.17  1992/02/11  01:09:23  lewis
 *		Got rid of call to Window::CalcDefaultSize().
 *
 *		Revision 1.15  1992/01/31  16:41:51  sterling
 *		eliminated backwards compatibility hacks
 *
 *		Revision 1.13  1992/01/19  21:43:42  lewis
 *		Added Dialog ctor with WindowShell argument.
 *
 *		Revision 1.10  1992/01/08  06:13:13  lewis
 *		Sterl- removed some hack having to do with tabbing and focusowners.
 *
 *		Revision 1.9  1992/01/04  07:58:35  lewis
 *		Support for Window not subclassing from View. Also new shell stuff.
 *		And use mainview - used not too. But still simulate enuf of old
 *		interface to get by til we have new dialog editor and dialogs bootstrapped.
 *		Only tested on mac so far.
 *
 *		Revision 1.6  1991/12/27  17:05:39  lewis
 *		A bunch of changes to get dialogs working under motif, including fMessage->SetWordWrap (True),
 *		and changing some constructors (and obsoleting some) for new window architecture.
 *
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Sound.hh"

#include	"Application.hh"
#include	"DeskTop.hh"
#include	"Shell.hh"

#include	"Dialog.hh"








/*
 ********************************************************************************
 ************************************** Dialog **********************************
 ********************************************************************************
 */

Boolean	Dialog::kCancel = True;

Dialog::Dialog (WindowShell* windowShell):
	Window (windowShell),
	fDefaultButton (Nil),
	fCancelButton (Nil),
	fOKButton (Nil),
	fDismissed (False),
	fCancelled (False),
	fDialogMainView (Nil)
{
	RequireNotNil (windowShell);
	SetFont (&kSystemFont);
}

Dialog::Dialog (View* mainView, FocusItem* mainFocus, AbstractPushButton* acceptButton, AbstractPushButton* cancelButton):
	Window (new StandardDialogWindowShell ()),
	fDefaultButton (Nil),
	fCancelButton (cancelButton),
	fOKButton (acceptButton),
	fDismissed (False),
	fCancelled (False),
	fDialogMainView (Nil)
{
	if (mainView != Nil) {
		SetMainView (fDialogMainView = new DialogMainView (*this, *mainView));
	}
	if (mainFocus != Nil) {
		AddFocus (mainFocus);
	}
}

Dialog::Dialog (View* mainView, FocusItem* mainFocus, const String& okButtonTitle, const String& cancelButtonTitle) :
	Window (new StandardDialogWindowShell ()),
	fDefaultButton (Nil),
	fCancelButton (Nil),
	fOKButton (Nil),
	fDismissed (False),
	fCancelled (False),
	fDialogMainView (Nil)
{
	RequireNotNil (mainView);
	SetMainView (fDialogMainView = new DialogMainView (*this, *mainView, okButtonTitle, cancelButtonTitle));

	if (mainFocus != Nil) {
		AddFocus (mainFocus);
	}
}

Dialog::~Dialog ()
{	
	SetMainView (Nil);
	delete fDialogMainView;
	
	SetCurrentFocus ((FocusItem*)Nil, View::eNoUpdate, False);

// Really should have saved reference to main focus - for now, just hack and remove all...
// LGP May 18, 1992
	ForEach (FocusItemPtr, it, MakeFocusIterator ()) {
		RemoveFocus (it.Current ());
	}
}

Point	Dialog::CalcDefaultSize_ (const Point& defaultSize) const
{
	if (fDialogMainView == Nil) {
		return (defaultSize);
	}
	else {
		return (fDialogMainView->CalcDefaultSize (defaultSize));
	}
}
	
void	Dialog::Dismiss (Boolean cancel)
{
	Require (not fDismissed);

	Close ();
	fDismissed = True;
	fCancelled = cancel;
}

Boolean	Dialog::GetDismissed () const
{
	return (fDismissed);
}

Boolean	Dialog::GetCancelled () const
{
	Require (fDismissed);
	return (fCancelled);
}

AbstractPushButton*	Dialog::GetDefaultButton () const
{
	return (fDefaultButton);
}

void	Dialog::SetDefaultButton (AbstractPushButton* button, Panel::UpdateMode updateMode)
{
	if (fDefaultButton != button) {
		if (fDefaultButton != Nil) {
			fDefaultButton->SetIsDefault (not (PushButton::kIsDefaultButton), updateMode);
		}
		fDefaultButton = button;
		if (fDefaultButton != Nil) {
			fDefaultButton->SetIsDefault (PushButton::kIsDefaultButton, updateMode);
		}
	}
}

AbstractPushButton*	Dialog::GetCancelButton () const
{
	return (fCancelButton);
}

void	Dialog::SetCancelButton (AbstractPushButton* button)
{
	fCancelButton = button;
}

AbstractPushButton*	Dialog::GetOKButton () const
{
	return (fOKButton);
}

void	Dialog::SetOKButton (AbstractPushButton* button)
{
	fOKButton = button;
}

Boolean	Dialog::HandleKeyStroke (const KeyStroke& keyStroke)
{
	if (((keyStroke == KeyStroke::kReturn) or (keyStroke == KeyStroke::kEnter)) and
		(GetDefaultButton () != Nil) and GetDefaultButton ()->GetEnabled ()) {
#if		qMacToolkit
		// hack to allow return keys in edittext, is there a better interface?
		if (keyStroke.GetModifiers ().Contains (KeyStroke::eOptionKeyModifier)) {
			return (FocusOwner::HandleKeyStroke (keyStroke));
		}
#endif
		GetDefaultButton ()->Flash ();
		return (True);
	}
	else if ((keyStroke == KeyStroke::kEscape) and
		(GetCancelButton () != Nil) and GetCancelButton ()->GetEnabled ()) {
		GetCancelButton ()->Flash ();
		return (True);
	}
	else {
		return (FocusOwner::HandleKeyStroke (keyStroke));
	}
	AssertNotReached (); return (False);
}

void	Dialog::ButtonPressed (AbstractButton* button)
{
	if (button == fOKButton) {
		Dismiss (not kCancel);
	}
	else if (button == fCancelButton) {
		Dismiss (kCancel);
	}
}

void	Dialog::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	Window::EffectiveLiveChanged (newLive, updateMode);
	if (newLive and (GetCurrentFocus () == Nil)) {
		// not really right, should be eDelayedUpdate, but that conflicts
		// with a hack in TextEdit to reduce flicker on Macintosh
		TabbingFocus (eSequenceForward, View::eNoUpdate);
	}
}

Boolean	Dialog::Pose ()
{
	fDismissed = False;
	fCancelled = False;

	Select ();

// these flushprevcomds are very BAD, but are necessary to avoid bugs when the window goes
// away - it would be nice to only do this if needed?
// save cur cmd, and restore it after context???
	Application::Get ().FlushPreviousCommand ();
	while (not GetDismissed ()) {
		Application::Get ().RunOnce ();
	}
	Application::Get ().FlushPreviousCommand ();
	return (not GetCancelled ());
}

void	Dialog::DoSetupMenus ()
{
	FocusOwner::DoSetupMenus ();
}

Boolean	Dialog::DoCommand (const CommandSelection& selection)
{
	return (FocusOwner::DoCommand (selection));
}

Boolean	Dialog::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState)
{
	return (FocusOwner::DispatchKeyEvent (code, isUp, keyBoardState, composeState));
}









/*
 ********************************************************************************
 *********************************** DialogMainView *****************************
 ********************************************************************************
 */
DialogMainView::DialogMainView (Dialog& dialog, View& mainView, const String& okButtonTitle, const String& cancelButtonTitle):
	View (),
	fDialog (dialog),
	fOKButton (Nil),
	fCancelButton (Nil),
	fMainView (mainView)
{
	SetFont (&kSystemFont);

	AddSubView (&fMainView);

	if (okButtonTitle != kEmptyString) {
		fOKButton = new PushButton (okButtonTitle, &fDialog);
		AddSubView (fOKButton);
		fDialog.SetOKButton (fOKButton);
	}
	if (cancelButtonTitle != kEmptyString) {
		fCancelButton = new PushButton (cancelButtonTitle, &fDialog);
		AddSubView (fCancelButton);
		fDialog.SetCancelButton (fCancelButton);
		fDialog.SetDefaultButton (fCancelButton);
	}
	else if (okButtonTitle != kEmptyString) {
		fDialog.SetDefaultButton (fOKButton);
	}

	Require ((fOKButton != Nil) or (fCancelButton == Nil));
}

DialogMainView::DialogMainView (Dialog& dialog, View& mainView) :
	View (),
	fDialog (dialog),
	fOKButton (Nil),
	fCancelButton (Nil),
	fMainView (mainView)
{
	SetFont (&kSystemFont);
	AddSubView (&fMainView);
}

DialogMainView::~DialogMainView ()
{
	RemoveSubView (&fMainView);
	if (fOKButton != Nil) {
		RemoveSubView (fOKButton);
		delete fOKButton;
	}
	if (fCancelButton != Nil) {
		RemoveSubView (fCancelButton);
		delete fCancelButton;
	}
}

Point	DialogMainView::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	newSize = fMainView.CalcDefaultSize (defaultSize);

	if (fOKButton != Nil) {
		newSize.SetV (newSize.GetV () + fOKButton->CalcDefaultSize ().GetV () + 10 + 10);
		Coordinate minWidth = fOKButton->CalcDefaultSize ().GetH ();
		if (fCancelButton != Nil) {
			minWidth += fCancelButton->CalcDefaultSize ().GetH ();
		}
		minWidth += 20;	// left and right margins
	
		if (newSize.GetH () < minWidth) {
			newSize.SetH (minWidth);
		}
	}
	
	return (newSize);
}

void	DialogMainView::Layout ()
{
	int	buttonCount = 0;
	Coordinate	buttonWidth = 0;
	if (fOKButton != Nil) {
		fOKButton->SetSize (fOKButton->CalcDefaultSize ());
		buttonCount++;
		buttonWidth += fOKButton->GetSize ().GetH ();
	}
	if (fCancelButton != Nil) {
		fCancelButton->SetSize (fCancelButton->CalcDefaultSize ());
		buttonCount++;
		buttonWidth += fCancelButton->GetSize ().GetH ();
	}
	Coordinate buttonOffset = 0;
	if (buttonCount > 0) {	// could be zero if someone builds own mainview
		buttonWidth += 20;
		if (buttonCount > 1) {
			buttonWidth += 10;
		}
	
		buttonOffset = 20;
		Rect	buttonsBounds = Rect (Point (0, 10), Point (100, GetExtent ().GetRight () - 20));
		Rect	buttonsRect = buttonsBounds;
		buttonsRect.SetSize (Point (100, buttonWidth));
		buttonsRect = CenterRectAroundRect (buttonsRect, buttonsBounds);
		Coordinate buttonBottom = GetSize ().GetV () - 10;
		if (fCancelButton == Nil) {
			if (fOKButton != Nil) {	
				fOKButton->SetOrigin (Point (buttonBottom - fOKButton->GetSize ().GetV (), 
											 buttonsRect.GetLeft ()));
				buttonOffset += fOKButton->GetSize ().GetV ();
			}
		}
		else {
			RequireNotNil (fOKButton);
			fOKButton->SetOrigin (Point (buttonBottom - fOKButton->GetSize ().GetV (), 
										 buttonsRect.GetRight () - fOKButton->GetSize ().GetH ()));

			fCancelButton->SetOrigin (Point (buttonBottom - fCancelButton->GetSize ().GetV (), 
											 buttonsRect.GetLeft ()));
			buttonOffset += Max (fOKButton->GetSize ().GetV (), fCancelButton->GetSize ().GetV ());
		}
	}
	fMainView.SetSize (Point (GetSize ().GetV () - buttonOffset, GetSize ().GetH ()));

	View::Layout ();
}

void	DialogMainView::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	View::EffectiveFontChanged (newFont, updateMode);
	InvalidateLayout ();
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

