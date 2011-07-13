/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Alert.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *			-	LGP broke support of 0.33 ratio for placement of dialogs since I put that into the
 *				windowshell/plaiun support. Right answer is probably to create an AlertShell????
 *
 * Changes:
 *	$Log: Alert.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/03  00:16:17  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.13  1992/03/26  18:35:11  lewis
 *		Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *
 *		Revision 1.11  1992/03/05  20:15:26  sterling
 *		changed AlertMainView to be GroupView
 *
 *		Revision 1.10  1992/01/31  16:32:20  sterling
 *		no more backwards compatibility
 *
 *		Revision 1.9  1992/01/29  04:47:06  sterling
 *		remove fMessage, fixed CalcDefaultSize
 *
 *		Revision 1.7  1992/01/22  15:18:29  sterling
 *		use TextView
 *
 *		Revision 1.6  1992/01/19  21:23:50  lewis
 *		Create StandardAlertWindowShell in ctor to Dialog.
 *	
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Sound.hh"

#include	"GroupView.hh"
#include	"Shell.hh"

#include	"Alert.hh"




class	AlertMainView : public GroupView {
	public:
		AlertMainView (Alert& alert, const String& okButtonTitle, const String& cancelButtonTitle);
		~AlertMainView ();
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

	public:		// leave public for now so Alert can peek - later probably not needed...
		Alert&					fAlert;
		PushButton*				fOKButton;
		PushButton*				fCancelButton;
		AbstractPixelMapButton*	fIcon;
		TextView*				fMessage;
};






/*
 ********************************************************************************
 *************************************** Alert **********************************
 ********************************************************************************
 */
Alert::Alert (const String& message, const String& okButtonTitle, const String& cancelButtonTitle):
	Dialog (new StandardAlertWindowShell ()),
	fIcon (Nil),
	fAlertMainView (Nil)
{
	SetMainView (fAlertMainView = new AlertMainView (*this, okButtonTitle, cancelButtonTitle));
	fAlertMainView->fMessage->SetSize (Point (0, 200), Panel::eNoUpdate);
	SetMessage (message);
}

Alert::~Alert ()
{
	SetMainView (Nil);
	delete fAlertMainView;
}

Point	Alert::CalcDefaultSize_ (const Point& defaultSize) const
{
	return (fAlertMainView->CalcDefaultSize (defaultSize));
}
	
String	Alert::GetMessage () const
{
	AssertNotNil (fAlertMainView->fMessage);
	return (fAlertMainView->fMessage->GetText ());
}

void	Alert::SetMessage (const String& message)
{
	AssertNotNil (fAlertMainView->fMessage);
	fAlertMainView->fMessage->SetText (message);
}
		
AbstractPixelMapButton*	Alert::GetIcon () const
{
	return (fAlertMainView->fIcon);
}

void	Alert::SetIcon (AbstractPixelMapButton* icon)
{
	if (fAlertMainView->fIcon != icon) {
		if (fAlertMainView->fIcon != Nil) {
			delete fAlertMainView->fIcon;
		}
		fAlertMainView->fIcon = icon;
		if (fAlertMainView->fIcon != Nil) {
			fAlertMainView->AddSubView (fIcon);
			fIcon->SetSize (fIcon->CalcDefaultSize ());
		}
	}
}








/*
 ********************************************************************************
 ************************************* StopAlert ********************************
 ********************************************************************************
 */
StopAlert::StopAlert (const String& message, const String& okButtonTitle, const String& cancelButtonTitle) :
	Alert (message, okButtonTitle, cancelButtonTitle)
{
	// SetIcon (???);
}


/*
 ********************************************************************************
 ************************************* NoteAlert ********************************
 ********************************************************************************
 */
NoteAlert::NoteAlert (const String& message, const String& okButtonTitle, const String& cancelButtonTitle) :
	Alert (message, okButtonTitle, cancelButtonTitle)
{
	// SetIcon (???);
}






/*
 ********************************************************************************
 *********************************** CautionAlert *******************************
 ********************************************************************************
 */
CautionAlert::CautionAlert (const String& message, const String& okButtonTitle, const String& cancelButtonTitle) :
	Alert (message, okButtonTitle, cancelButtonTitle)
{
	// SetIcon (???);
}






/*
 ********************************************************************************
 ************************************ StagedAlert *******************************
 ********************************************************************************
 */

StagedAlert::StagedAlert (const String& message, Int16 threshold, const String& buttonTitle) :
	fMessage (message),
	fPosedCount (0),
	fThreshold (threshold),
	fButtonTitle (buttonTitle)
{
}
		
Boolean	StagedAlert::Pose ()
{
	fPosedCount++;
	if (fPosedCount >= fThreshold) {
		Alert (fMessage, fButtonTitle).Pose ();
	}
	else {
		Beep ();
	}
	return (True);
}
		
String	StagedAlert::GetMessage () const
{
	return (fMessage);
}

void	StagedAlert::SetMessage (const String& message)
{
	if (fMessage != message) {
		fPosedCount = 0;
		fMessage = message;
	}
}
		
Int16	StagedAlert::GetPosedCount ()
{
	return (fPosedCount);
}

void	StagedAlert::SetPosedCount (Int16 count)
{
	fPosedCount = count;
}




/*
 ********************************************************************************
 ************************************ AlertMainView *****************************
 ********************************************************************************
 */
AlertMainView::AlertMainView (Alert& alert, const String& okButtonTitle, const String& cancelButtonTitle):
	GroupView (),
	fAlert (alert),
	fOKButton (Nil),
	fCancelButton (Nil),
	fIcon (Nil),
	fMessage (Nil)
{
	fOKButton = new PushButton (okButtonTitle, &fAlert);
	AddSubView (fOKButton);
	fAlert.SetOKButton (fOKButton);
	
	if (cancelButtonTitle != kEmptyString) {
		fCancelButton = new PushButton (cancelButtonTitle, &fAlert);
		AddSubView (fCancelButton);
		fAlert.SetCancelButton (fCancelButton);
		fAlert.SetDefaultButton (fCancelButton);
	}
	else {
		fAlert.SetDefaultButton (fOKButton);
	}
	fMessage = new TextView ();
	fMessage->SetWordWrap (True);
	AddSubView (fMessage);
}

AlertMainView::~AlertMainView ()
{
	RemoveSubView (fMessage);
	delete fMessage;
	RemoveSubView (fOKButton);
	delete fOKButton;
	if (fCancelButton != Nil) {
		RemoveSubView (fCancelButton);
		delete fCancelButton;
	}
	if (fIcon != Nil) {
		RemoveSubView (fIcon);
		delete fIcon;
	}
}

Point	AlertMainView::CalcDefaultSize_ (const Point& /* defaultSize */) const
{
	AssertNotNil (fMessage);
	Point	newSize = fMessage->CalcDefaultSize (fMessage->GetSize ()) + Point (10, 10);
	
	newSize.SetV (newSize.GetV () + fOKButton->CalcDefaultSize ().GetV () + 10);
	if (fIcon != Nil) {
		newSize.SetV (newSize.GetV () + fIcon->CalcDefaultSize ().GetV () + 8);
	}
	return (newSize);
}

void	AlertMainView::Layout ()
{
	AssertNotNil (fOKButton);
	fOKButton->SetSize (fOKButton->CalcDefaultSize ());
	if (fCancelButton != Nil) {
		fCancelButton->SetSize (fCancelButton->CalcDefaultSize ());
	}

	if (fCancelButton == Nil) {
		fOKButton->SetOrigin (Point (GetSize ().GetV () - 5 - fOKButton->GetSize ().GetV (), (GetSize ().GetH () - 10 - fOKButton->GetSize ().GetH ())/2));
	}
	else {
		fOKButton->SetOrigin (Point (GetSize ().GetV () - 5 - fOKButton->GetSize ().GetV (), GetSize ().GetH () - 10 - fOKButton->GetSize ().GetH ()));
		fCancelButton->SetOrigin (Point (GetSize ().GetV () - 5 - fCancelButton->GetSize ().GetV (), 10));
	}

	fMessage->SetSize (fMessage->CalcDefaultSize (fMessage->GetSize ()));
	if (fIcon == Nil) {
		fMessage->SetOrigin (Point (5, 5));
	}
	else {
		fIcon->SetOrigin (Point (8, 8));
		fMessage->SetOrigin (Point (5, 5) + Point (fIcon->GetExtent ().GetTop (), 0));
	}

	View::Layout ();
}

void	AlertMainView::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	GroupView::EffectiveFontChanged (newFont, updateMode);
	InvalidateLayout ();
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

