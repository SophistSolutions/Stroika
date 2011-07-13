/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/RadioButton.cc,v 1.10 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: RadioButton.cc,v $
 *		Revision 1.10  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.9  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.7  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.6  1992/07/16  16:50:49  sterling
 *		changed GUI to UI
 *
 *		Revision 1.5  1992/07/04  14:21:49  lewis
 *		Shapes now pass by value using new letter / env stuff. As side effect of that
 *		I have (at least temporarily gotten rid of Setter methods for fields of shapes
 *		So, I changed updates of line with SetFrom/SetTo to line = Line (from,to).
 *		I think thats better anyhow, but I could add back the setter methods?
 *
 *		Revision 1.4  1992/07/03  02:12:46  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.3  1992/07/02  04:58:05  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  08:31:06  sterling
 *		Lots of changes including motif portable version, react to Button->Toggle change, and much more.
 *
 *		Revision 1.44  1992/06/09  14:26:19  lewis
 *		Add #include      "Pallet.hh"
 *		Move SetBorder/Margin around in _Native so we dont crap out - not so well orchastrated...
 *		Motif native CTOR.
 *
 *		Revision 1.43  92/06/09  15:23:50  15:23:50  sterling (Sterling Wight)
 *		motif portable version
 *		
 *		Revision 1.42  92/05/13  12:21:53  12:21:53  lewis (Lewis Pringle)
 *		STERL: Added overloaded SetLabel() method taking const char*.
 *		Changed default arg to overloading in RadioButton CTOR, and added no-arg case.
 *		
 *		Revision 1.41  92/04/20  14:34:47  14:34:47  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		Also, use )Min (GetBorderHeight (), GetBorderWidth ()) instead of Bordered::CalcShadowThickness () since that
 *		rouitine went away - See rcslog in Bordered.hh.
 *		
 *		Revision 1.40  92/04/14  07:33:47  07:33:47  lewis (Lewis Pringle)
 *		Added out of line virtual dtor for RadioButton because of qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.39  92/04/02  13:07:32  13:07:32  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.38  92/03/26  18:34:29  18:34:29  lewis (Lewis Pringle)
 *		Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *		
 *		Revision 1.37  1992/03/26  15:15:20  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of oldFocused first argument.
 *
 *		Revision 1.36  1992/03/26  09:50:34  lewis
 *		Added newFont/updateMode args to EffectiveFontChanged, and got rid of oldLive arg to EffecitveLiveChanged.
 *
 *		Revision 1.34  1992/03/19  17:02:27  lewis
 *		Minor cleanups, and call AbstractRadioButton_MotifUI::EffectiveFocusedChanged instead of skipping to
 *		FocusItem:: - was done cuz button did bad things there, but thats now gone.
 *		Also, got rid of flicker on selection by having callback call seton (on, eNoUpdate).
 *
 *		Revision 1.33  1992/03/17  17:03:42  lewis
 *		Override EffectiveFocusChanged () to do XmProcessTraversal for motif - not sure how to do unfocus case??
 *		Also, skip base classes inherited - go directly to FocusItem - since we still have crap in Button.cc.
 *
 *		Revision 1.31  1992/03/09  15:18:27  lewis
 *		Added workaround for qCFRONT_ARRAYS_IN_INLINE_CAUSE_ERROR_BUG on motif.
 *
 *		Revision 1.27  1992/03/05  20:58:55  sterling
 *		support for borders
 *
 *		Revision 1.26  1992/02/24  06:49:39  lewis
 *		Fix coords passed to ButtonTracker -- sterl.
 *
 *		Revision 1.25  1992/02/21  20:15:25  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.24  1992/02/19  22:20:18  sterling
 *		better default size for Motif
 *
 *		Revision 1.22  1992/02/15  05:33:24  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.21  1992/02/03  17:06:02  lewis
 *		Use class rather than #define or typedef for RadioButton.
 *
 *		Revision 1.20  1992/01/24  17:59:05  lewis
 *		Get rid of ensures that Sets of resources worked since we may be unrealized.
 *
 *		Revision 1.19  1992/01/23  19:55:14  lewis
 *		Override realize in motif so we can patch values in widget after its realized. Also, covnertr macUI to MacUI.
 *		Also, use protected virtual _ methods with public accessor wrappers style.
 *		Also, fixed funny thing with radiobutton MacUI portable, where it was using xor mode, and copymode was more sensible.
 *		It should have been OK, but under motif we were getting two draw calls without an intervening erase, so things looked
 *		bad - really a bug with our update support, but too hard to fix yet, and want things to look right now.
 *
 *		Revision 1.18  1992/01/23  08:11:30  lewis
 *		Update for changes in OSControls - motif os controls no longer keep track of values while were unrealized.
 *
 *		Revision 1.9  1992/01/10  08:54:51  lewis
 *		Got rid of bogus portable motif implementaion, and added setsensative call to radiobutton.
 *
 */


#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Controls.h>
#include	<Windows.h>
#elif	qXtToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#if		qXmToolkit
#include 	<Xm/ToggleB.h>
#endif
#endif	/*GDI*/
#include	"OSRenamePost.hh"


#include	"StreamUtils.hh"

#include	"Pallet.hh"
#include	"Shape.hh"

#include	"OSControls.hh"

#include	"RadioButton.hh"




#if		!qRealTemplatesAvailable
	Implement (Iterator, RadioBtPtr);
	Implement (Collection, RadioBtPtr);
	Implement (AbSequence, RadioBtPtr);
	Implement (Array, RadioBtPtr);
	Implement (Sequence_Array, RadioBtPtr);
	Implement (Sequence, RadioBtPtr);
#endif




/*
 ********************************************************************************
 ********************************** RadioBank ***********************************
 ********************************************************************************
 */
RadioBank::RadioBank ():
	ButtonController (),
	fRadioButtons (),
	fCurrentButton (Nil)
{
}

void	RadioBank::AddRadioButton (AbstractRadioButton* button)
{
	RequireNotNil (button);
	button->SetController (this);
	fRadioButtons.Append (button);
}

void	RadioBank::RemoveRadioButton (AbstractRadioButton* button)
{
	RequireNotNil (button);
	button->SetController (Nil);
	fRadioButtons.Remove (button);
}

void	RadioBank::ButtonPressed (AbstractButton* button)
{
	if (button == fCurrentButton) {
		// so we don't react to the below code where we are shutting some button off...
		Assert (not (fCurrentButton->GetOn ()));
		fCurrentButton = Nil;
	}
	else {
		if (fCurrentButton != Nil) {
			fCurrentButton->SetOn (False, View::eImmediateUpdate);
		}
		Assert (fCurrentButton == Nil);
		ForEach (RadioBtPtr, it, fRadioButtons) {
			AbstractRadioButton*	rb = it.Current ();
			if (rb == button) {
				Assert (rb->GetOn ());
				fCurrentButton = rb;
				break;
			}
		}
		ButtonController::ButtonPressed (this);
	}
}

AbstractRadioButton*	RadioBank::GetCurrentButton () const
{
	return (fCurrentButton);
}

CollectionSize	RadioBank::GetCurrentIndex () const
{
	if (fCurrentButton == Nil) {
		return (kBadSequenceIndex);
	}
	else {
		return (fRadioButtons.IndexOf (fCurrentButton));
	}
}

void	RadioBank::SetCurrentIndex (CollectionSize index, Panel::UpdateMode updateMode)
{
	if (index == kBadSequenceIndex) {
		if (fCurrentButton != Nil) {
			fCurrentButton->SetOn (False, updateMode);
		}
	}
	else {
		AbstractRadioButton*	rb = fRadioButtons[index];
		AssertNotNil (rb);
		rb->SetOn (True, updateMode);
	}
}







/*
 ********************************************************************************
 ***************************** AbstractRadioButton ******************************
 ********************************************************************************
 */

AbstractRadioButton::AbstractRadioButton (ButtonController* controller):
	Toggle (controller)
{
}

String	AbstractRadioButton::GetLabel () const
{
	return (GetLabel_ ());
}

void	AbstractRadioButton::SetLabel (const String& label, Panel::UpdateMode updateMode)
{
	if (label != GetLabel ()) {
		SetLabel_ (label, updateMode);
	}
	Ensure (label == GetLabel ());
}

void	AbstractRadioButton::SetLabel (const char* cString, Panel::UpdateMode updateMode)
{
	RequireNotNil (cString);
	String	label = String (cString);
	if (label != GetLabel ()) {
		SetLabel_ (label, updateMode);
	}
	Ensure (label == GetLabel ());
}









/*
 ********************************************************************************
 *************************** AbstractRadioButton_MacUI *************************
 ********************************************************************************
 */
AbstractRadioButton_MacUI::AbstractRadioButton_MacUI (ButtonController* controller):
	AbstractRadioButton (controller)
{
	SetFont (&kSystemFont);
	SetBackground (&kWhiteTile);
}

Coordinate	AbstractRadioButton_MacUI::GetToggleWidth () const
{
	return (12);
}

Coordinate	AbstractRadioButton_MacUI::GetToggleGap () const
{
	return (4);
}

Point	AbstractRadioButton_MacUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	size = Point (
		Max (GetEffectiveFont ().GetFontHeight (), GetToggleWidth ()), 
		3 + GetToggleWidth () + GetToggleGap () + TextWidth (GetLabel ()));

	return (size + GetBorder ()*2 + GetMargin ()*2);
}

void	AbstractRadioButton_MacUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes, our Layout ()
	 * since our placement of subcomponents changes, and Refresh () since our visual display depends
	 * on the current font.
	 */

	AbstractRadioButton::EffectiveFontChanged (newFont, eNoUpdate);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);				// cuz our display may change
	}
}






/*
 ********************************************************************************
 ************************* RadioButton_MacUI_Portable **************************
 ********************************************************************************
 */

RadioButton_MacUI_Portable::RadioButton_MacUI_Portable (const String& label, ButtonController* controller):
	AbstractRadioButton_MacUI (controller),
	fLabel (label)
{
}

void	RadioButton_MacUI_Portable::Draw (const Region& /*update*/)
{
	DrawToggle (GetOn ());

	// should take GetEffectiveLive into account when drawing text
	DrawText (GetLabel (), Point (
		(GetSize ().GetV () - GetEffectiveFont ().GetFontHeight ())/2, 
		GetBox ().GetRight () + GetToggleGap ()));

	DrawBorder ();
}

String	RadioButton_MacUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	RadioButton_MacUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}

void	RadioButton_MacUI_Portable::DrawToggle (Boolean on)
{
	Rect	r = GetBox ();
	OutLine (Oval (), r);
	if (on) {
		Paint (Oval (), r.InsetBy (3, 3));
	}
	else {
		Erase (Oval (), r.InsetBy (3, 3));
	}
}

void	RadioButton_MacUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	AbstractRadioButton_MacUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		DrawToggle (on);
	}
	else if (updateMode != eNoUpdate) {
		Refresh (Oval ().ToRegion (GetBox ().InsetBy (Point (1, 1))), updateMode);
	}
}

Boolean	RadioButton_MacUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	static	Pen	kFeedbackPen = Pen (kBlackTile, eXorTMode);
	OutLine (Oval (), GetBox ().InsetBy (1, 1), kFeedbackPen);
	Boolean	result = Button::Track (phase, mouseInButton);
	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		Toggle::SetOn (True, eImmediateUpdate);
	}
	
	return (result);
}

Rect	RadioButton_MacUI_Portable::GetBox () const
{
	Rect extent = GetLocalExtent ();
	if (Odd (extent.GetHeight ())) {
		extent.SetSize (extent.GetSize () + Point (1, 0));	// we round up in placing the toggle box
	}
	Rect r =  CenterRectAroundRect (Rect (kZeroPoint, Point (GetToggleWidth (), GetToggleWidth ())), extent);
	r.SetOrigin (Point (r.GetOrigin ().GetV (), GetBorder ().GetH () + GetMargin ().GetH () + 2));
	return (r);
}






#if		qMacToolkit
/*
 ********************************************************************************
 *************************** RadioButton_MacUI_Native **************************
 ********************************************************************************
 */

class	MacOSRadioButton : public MacOSControl {
	public:
		MacOSRadioButton (RadioButton_MacUI_Native& theRB):
			MacOSControl (radioButProc),
			fRadioButton (theRB)
			{
				SetMin (0);
				SetMax (1);
				SetValue (theRB.GetOn ());
			}

			override	void	ActionProc (short partCode)
			{
				fRadioButton.Tracking (Tracker::eTrackMove, Boolean (partCode == inCheckBox));
			}
			
			override	Boolean	TrackPress (const MousePressInfo& mouseInfo)
			{
				Boolean aborted = fRadioButton.Tracking (Tracker::eTrackPress, True);
				return (Boolean (aborted or MacOSControl::TrackPress (mouseInfo)));
			}

			override	void	DoneProc (short partCode)
			{
				Boolean aborted = fRadioButton.Tracking (Tracker::eTrackRelease, Boolean (partCode == inCheckBox));				
				if ((not aborted) and (partCode == inCheckBox)) {
					fRadioButton.SetOn (True, eImmediateUpdate);
				}
			}
			
	private:
		RadioButton_MacUI_Native& fRadioButton;
};


RadioButton_MacUI_Native::RadioButton_MacUI_Native (const String& label, ButtonController* controller):
	AbstractRadioButton_MacUI (controller),
	fMacOSControl (Nil)
{
	AddSubView (fMacOSControl = new MacOSRadioButton (*this));
	SetLabel_ (label, eNoUpdate);
	fMacOSControl->SetHiliteState (GetEffectiveLive () ? 0 : 0xff, eNoUpdate);
}

RadioButton_MacUI_Native::~RadioButton_MacUI_Native ()
{
	RemoveSubView (fMacOSControl);
	delete fMacOSControl;
}

void	RadioButton_MacUI_Native::Draw (const Region& /*update*/)
{
	DrawBorder ();
}

void	RadioButton_MacUI_Native::SetBorder_ (const Point& border, Panel::UpdateMode updateMode)
{
	InvalidateLayout ();
	AbstractRadioButton_MacUI::SetBorder_ (border, updateMode);
}

void	RadioButton_MacUI_Native::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode)
{
	InvalidateLayout ();
	AbstractRadioButton_MacUI::SetMargin_ (margin, updateMode);
}

void	RadioButton_MacUI_Native::Layout ()
{
	AssertNotNil (fMacOSControl);
	Point	offset = GetBorder () + GetMargin ();
	fMacOSControl->SetOrigin (offset);
	fMacOSControl->SetSize (GetSize () - offset*2);
	AbstractRadioButton_MacUI::Layout ();
}

void	RadioButton_MacUI_Native::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	AbstractRadioButton_MacUI::SetOn_ (on, eNoUpdate);
	AssertNotNil (fMacOSControl);
	fMacOSControl->SetValue (on, updateMode);
}

void	RadioButton_MacUI_Native::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractRadioButton_MacUI::EffectiveLiveChanged (newLive, eNoUpdate);
	AssertNotNil (fMacOSControl);
	fMacOSControl->SetHiliteState (newLive ? 0 : 0xff, updateMode);
}

String	RadioButton_MacUI_Native::GetLabel_ () const
{
	AssertNotNil (fMacOSControl);
	return (fMacOSControl->GetTitle ());
}

void	RadioButton_MacUI_Native::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	AssertNotNil (fMacOSControl);
	fMacOSControl->SetTitle (label, updateMode);
}

#endif	/* qMacToolkit */




/*
 ********************************************************************************
 ************************* AbstractRadioButton_MotifUI *************************
 ********************************************************************************
 */
AbstractRadioButton_MotifUI::AbstractRadioButton_MotifUI (ButtonController* controller):
	AbstractRadioButton (controller)
{
	SetBorder (2, 2, eNoUpdate);
	SetMargin (2, 8, eNoUpdate);
	SetFont (&kSystemFont);
}

Boolean	AbstractRadioButton_MotifUI::HandleKeyStroke (const KeyStroke& keyStroke)
{
// Maybe should have some auxiliary functions (static maybe) in FocusItem to say which keystrokes get
// interpretted as being part of the mouseless interface, and how they get interpretted???
	if ((keyStroke == KeyStroke::kEnter) or (keyStroke == KeyStroke::kReturn) or (keyStroke == KeyStroke::kTab)) {
		AbstractRadioButton::SetOn (not GetOn (), eImmediateUpdate);
		return (True);
	}
	return (False);
}

Boolean	AbstractRadioButton_MotifUI::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	refocused = False;
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		refocused = True;
	}

	return (Boolean (AbstractRadioButton::TrackPress (mouseInfo) or refocused));
}

Coordinate	AbstractRadioButton_MotifUI::GetToggleWidth () const
{
	// plus one is motif black magic
   	return (GetEffectiveFont ().GetFontHeight () + 1);
}

Coordinate	AbstractRadioButton_MotifUI::GetToggleGap () const
{
   	return (GetToggleWidth () / 4);
}

Point	AbstractRadioButton_MotifUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Coordinate toggleWidth = GetToggleWidth ();
	Point newDefaultSize = Point (toggleWidth, TextWidth (GetLabel ()) + GetToggleGap () + toggleWidth + 5); // plus five is pure black magic

	// Motif RadioButtones are screwy in that only the margin affects the size of the control
	return (newDefaultSize + GetMargin ()*2);
}

void	AbstractRadioButton_MotifUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */

	AbstractRadioButton::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);				// cuz our display may change
	}
}

Boolean	AbstractRadioButton_MotifUI::GetLive () const
{
	return (AbstractRadioButton::GetLive ());
}

void	AbstractRadioButton_MotifUI::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractRadioButton::EffectiveLiveChanged (newLive, updateMode);
}

Boolean	AbstractRadioButton_MotifUI::GetEffectiveLive () const
{
	// only View overrides this, but C++ insists we disambiguate anyhow
	return (View::GetEffectiveLive ());
}



RadioButton_MotifUI_Portable::RadioButton_MotifUI_Portable (const String& label, ButtonController* controller) :
	AbstractRadioButton_MotifUI (controller),
	fLabel (label)
{
}

void	RadioButton_MotifUI_Portable::Draw (const Region& /*update*/)
{
	DrawToggle (GetOn ());

	// should take GetEffectiveLive into account when drawing text
	DrawText (GetLabel (), Point (
		(GetSize ().GetV () - GetEffectiveFont ().GetFontHeight ())/2, 
		GetBox ().GetRight () + GetToggleGap ()));
}

void	RadioButton_MotifUI_Portable::DrawToggle (Boolean on)
{
	Rect	box = GetBox ();

	Color	backGroundColor = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	Color	selectColor;
	Color	topShadow;
	Color	bottomShadow;
	
	CalcColorSet (backGroundColor, selectColor, topShadow, bottomShadow);
	
	if (on) {
		Color temp = topShadow;
		topShadow = bottomShadow;
		bottomShadow = temp;
	}
	
	Tile	topShadowTile		=	PalletManager::Get ().MakeTileFromColor (topShadow);
	Tile	bottomShadowTile	=	PalletManager::Get ().MakeTileFromColor (bottomShadow);

	if (GetBorder () != kZeroPoint) {
		box.SetSize (box.GetSize () - Point (1, 2));
		Pen		topPen = Pen (topShadowTile);
		Pen		botPen = Pen (bottomShadowTile);
		Point	boxCenter = box.GetSize ()/2;
		Line	line	=	Line (Point (0, boxCenter.GetH ()), Point (boxCenter.GetV (), 0));

		for (int i = 1; i <= GetBorder ().GetV (); i++) {
			OutLine (line, box, topPen);
			line = Line (line.GetFrom () + Point (1, 0), line.GetTo () + Point (0, 1));
		}
		line = Line (Point (0, boxCenter.GetH ()), Point (boxCenter.GetV (), box.GetSize ().GetH ()));
		for (i = 1; i <= GetBorder ().GetV (); i++) {
			OutLine (line, box, topPen);
			line = Line (line.GetFrom () + Point (1, 0), line.GetTo () - Point (0, 1));
		}
		line = Line (Point (box.GetSize ().GetV (), boxCenter.GetH ()), Point (boxCenter.GetV ()+1, 0));
		for (i = 1; i <= GetBorder ().GetV (); i++) {
			OutLine (line, box, botPen);
			line = Line (line.GetFrom () - Point (1, 0), line.GetTo () + Point (0, 1));
		}
		line = Line (Point (box.GetSize ().GetV (), boxCenter.GetH ()), Point (boxCenter.GetV ()+1, box.GetSize ().GetH ()));
		for (i = 1; i <= GetBorder ().GetV (); i++) {
			OutLine (line, box, botPen);
			line = Line (line.GetFrom () - Point (1, 0), line.GetTo () - Point (0, 1));
		}
	}
#if 0
	if (on) {
		Tile	selectTile	=	PalletManager::Get ().MakeTileFromColor (selectColor);
		Paint (Rectangle (), InsetBy (box, GetBorder ()), selectTile);
	}
#endif
}

Boolean	RadioButton_MotifUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	DrawToggle (mouseInButton ^ GetOn ());	
	Boolean	result = Button::Track (phase, mouseInButton);
	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		Toggle::SetOn (True, eNoUpdate);	// no update needed as drawtoggle is symmetric
	}
	return (result);
}

Rect	RadioButton_MotifUI_Portable::GetBox () const
{
	return (Rect (GetMargin (), Point (GetToggleWidth (), GetToggleWidth ())));
}

void	RadioButton_MotifUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	AbstractRadioButton_MotifUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		DrawToggle (on);
	}
	else if (updateMode != eNoUpdate) {
		Refresh (GetBox (), updateMode);
	}
}

String	RadioButton_MotifUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	RadioButton_MotifUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}



#if 	qXmToolkit
/*
 ********************************************************************************
 ************************** RadioButton_MotifUI_Native *************************
 ********************************************************************************
 */
RadioButton_MotifUI_Native::RadioButton_MotifUI_Native (const String& label, ButtonController* controller) :
	AbstractRadioButton_MotifUI (controller),
	fOSControl (Nil),
	fLabel (label)
{
	class	MyOSRadioButton : public MotifOSControl {
		public:
			MyOSRadioButton (AbstractRadioButton& radioButton) :
				MotifOSControl (),
				fRadioButton (radioButton)
			{
			}
			
			override	osWidget*	DoBuild_ (osWidget* parent)
			{
				RequireNotNil (parent);

#if		qCFRONT_ARRAYS_IN_INLINE_CAUSE_ERROR_BUG
				// hack to avoid CFront 2.1 unumplemented featur...
				struct {
					Arg	args [2];
				} xxx;
				Arg* args = xxx.args;
#else
				Arg	args [2];
#endif
				XtSetArg (args[0], XmNindicatorType, 	XmONE_OF_MANY);
				XtSetArg (args[1], XmNalignment, 		XmALIGNMENT_BEGINNING);

				osWidget* w = ::XmCreateToggleButton (parent, "", args, 2);

				::XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc)sCallBackProc, (XtPointer)&fRadioButton);

				return (w);
			}

			static void sCallBackProc (osWidget* w, caddr_t client_data, caddr_t /*call_data*/)
		  		{
					fRadioButton.Tracking (Tracker::eTrackRelease, True);
					
					osBoolean	osb = 0;
					Arg		arg;
					XtSetArg (arg, XmNset, &osb);
					::XtGetValues (w, &arg, 1);

					RadioButton_MotifUI_Native* fred = (RadioButton_MotifUI_Native*)client_data;
					AssertNotNil (fred);
					fred->AbstractRadioButton::SetOn (Boolean (!!osb), eNoUpdate);
				}

		
		private:
			AbstractRadioButton&	fRadioButton;
	};
	AddSubView (fOSControl = new MyOSRadioButton (*this));
	SetLabel_ (label, eNoUpdate);
	SetBorder_ (GetBorder (), eNoUpdate);
	SetMargin_ (GetMargin (), eNoUpdate);
}

RadioButton_MotifUI_Native::~RadioButton_MotifUI_Native ()
{
	RemoveSubView (fOSControl);
	delete fOSControl;
}

void	RadioButton_MotifUI_Native::SetBorder_ (const Point& border, UpdateMode updateMode)
{
	AbstractRadioButton_MotifUI::SetBorder_ (border, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
	Refresh (updateMode);
}

void	RadioButton_MotifUI_Native::SetMargin_ (const Point& margin, UpdateMode updateMode)
{
	AbstractRadioButton_MotifUI::SetMargin_ (margin, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetDimensionResourceValue (XmNmarginHeight, (unsigned short)GetMarginHeight ());
    fOSControl->SetDimensionResourceValue (XmNmarginWidth, (unsigned short)GetMarginWidth ());
	Refresh (updateMode);
}

String	RadioButton_MotifUI_Native::GetLabel_ () const
{
	return (fLabel);
}

void	RadioButton_MotifUI_Native::SetLabel_ (const String& label, UpdateMode updateMode)
{
	fLabel = label;
	AssertNotNil (fOSControl);
	fOSControl->SetStringResourceValue (XmNlabelString, label);
}

void	RadioButton_MotifUI_Native::Layout ()
{
	AbstractRadioButton_MotifUI::Layout ();
	AssertNotNil (fOSControl);
	Assert (fOSControl->GetOrigin () == kZeroPoint);
	fOSControl->SetSize (GetSize ());
}

void	RadioButton_MotifUI_Native::EffectiveFocusChanged (Boolean newFocused, UpdateMode updateMode)
{
	if (GetEffectiveLive ()) {
		if (fOSControl->GetWidget () != Nil) {
			if (newFocused) {
				::XmProcessTraversal (fOSControl->GetWidget (), XmTRAVERSE_CURRENT);
			}
			else {
// Doesnt appear to be anyway to unfocus, other than for someone else to be the focus - must find a way, cuz
// new guy might not be a motif widget...
//				::XmProcessTraversal (fOSControl->GetWidget (), XmTRAVERSE_CURRENT);
			}
		}
		updateMode = eNoUpdate;
	}
	AbstractRadioButton_MotifUI::EffectiveFocusChanged (newFocused, updateMode);
}

void	RadioButton_MotifUI_Native::EffectiveFontChanged (const Font& newFont, UpdateMode updateMode)
{
	AbstractRadioButton_MotifUI::EffectiveFontChanged (newFont, updateMode);

	RequireNotNil (fOSControl);
	Coordinate toggleSize = GetToggleWidth ();
   	Coordinate toggleSpacing = GetToggleSpacing ();
	fOSControl->SetDimensionResourceValue (XmNspacing, (unsigned short)toggleSpacing);
	
	// these next seven are a work-around for a bug in the current (HP?) Motif
	// implementation, which seems to correctly grow the margin, but never
	// shrinks it back again
	fOSControl->SetDimensionResourceValue (XmNmarginHeight, (unsigned short)GetMargin ().GetV ());
    fOSControl->SetDimensionResourceValue (XmNmarginWidth, (unsigned short)GetMargin ().GetH ());
	fOSControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));

	fOSControl->SetDimensionResourceValue (XmNmarginTop, 0);
    fOSControl->SetDimensionResourceValue (XmNmarginBottom, 0);
    fOSControl->SetDimensionResourceValue (XmNmarginLeft, (unsigned short) (toggleSize + toggleSpacing));
    fOSControl->SetDimensionResourceValue (XmNmarginRight, 0);
}

void	RadioButton_MotifUI_Native::SetOn_ (Boolean on, UpdateMode updateMode)
{
	AbstractRadioButton_MotifUI::SetOn_ (on, updateMode);
	fOSControl->SetBooleanResourceValue (XmNset, on);
}

void	RadioButton_MotifUI_Native::EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode)
{
	AbstractRadioButton_MotifUI::EffectiveLiveChanged (newLive, updateMode);
	AssertNotNil (fOSControl);
	fOSControl->SetBooleanResourceValue (XmNsensitive, newLive);
}

void	RadioButton_MotifUI_Native::Realize (osWidget* parent)
{
	// Dont call inherited version since we call our childs realize directly...
	RequireNotNil (parent);
	AssertNotNil (fOSControl);
	fOSControl->Realize (parent);
	fOSControl->SetBooleanResourceValue (XmNsensitive, GetEffectiveLive ());
	fOSControl->SetBooleanResourceValue (XmNset, GetOn ());
	fOSControl->SetDimensionResourceValue (XmNmarginHeight, (unsigned short)GetMarginHeight ());
    fOSControl->SetDimensionResourceValue (XmNmarginWidth, (unsigned short)GetMarginWidth ());
	fOSControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
	SetLabel_ (GetLabel (), eNoUpdate);
}

void	RadioButton_MotifUI_Native::UnRealize ()
{
	AssertNotNil (fOSControl);
	fOSControl->UnRealize ();
}

#endif	/*qXmToolkit*/




/*
 ********************************************************************************
 *************************** AbstractRadioButton_WinUI *************************
 ********************************************************************************
 */
AbstractRadioButton_WinUI::AbstractRadioButton_WinUI (ButtonController* controller):
	AbstractRadioButton (controller)
{
	/*
	 * hack - Tom says this is a good choice for winUI
	 */
	Font	f	=	Font ("Helvetica", 12, Set_BitString(FontStyle) (Font::eBoldFontStyle));
	SetFont (&f);

	SetBackground (&kWhiteTile);
}

Coordinate	AbstractRadioButton_WinUI::GetToggleWidth () const
{
	return (12);
}

Coordinate	AbstractRadioButton_WinUI::GetToggleGap () const
{
	return (4);
}

Point	AbstractRadioButton_WinUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	size = Point (
		Max (GetEffectiveFont ().GetFontHeight (), GetToggleWidth ()), 
		3 + GetToggleWidth () + GetToggleGap () + TextWidth (GetLabel ()));

	return (size + GetBorder ()*2 + GetMargin ()*2);
}

void	AbstractRadioButton_WinUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes, our Layout ()
	 * since our placement of subcomponents changes, and Refresh () since our visual display depends
	 * on the current font.
	 */

	AbstractRadioButton::EffectiveFontChanged (newFont, eNoUpdate);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);				// cuz our display may change
	}
}






/*
 ********************************************************************************
 ************************* RadioButton_WinUI_Portable **************************
 ********************************************************************************
 */

RadioButton_WinUI_Portable::RadioButton_WinUI_Portable (const String& label, ButtonController* controller):
	AbstractRadioButton_WinUI (controller),
	fLabel (label)
{
}

void	RadioButton_WinUI_Portable::Draw (const Region& /*update*/)
{
	DrawToggle (GetOn ());

	// should take GetEffectiveLive into account when drawing text
	DrawText (GetLabel (), Point (
		(GetSize ().GetV () - GetEffectiveFont ().GetFontHeight ())/2, 
		GetBox ().GetRight () + GetToggleGap ()));

	DrawBorder ();
}

String	RadioButton_WinUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	RadioButton_WinUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}

void	RadioButton_WinUI_Portable::DrawToggle (Boolean on)
{
	Rect	r = GetBox ();
	OutLine (Oval (), r);
	if (on) {
		Paint (Oval (), r.InsetBy (3, 3));
	}
	else {
		Erase (Oval (), r.InsetBy (3, 3));
	}
}

void	RadioButton_WinUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	AbstractRadioButton_WinUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		DrawToggle (on);
	}
	else if (updateMode != eNoUpdate) {
		Refresh (Oval ().ToRegion (GetBox ().InsetBy (Point (1, 1))), updateMode);
	}
}

Boolean	RadioButton_WinUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	static	Pen	kFeedbackPen = Pen (kBlackTile, eXorTMode);
	OutLine (Oval (), GetBox ().InsetBy (1, 1), kFeedbackPen);
	Boolean	result = Button::Track (phase, mouseInButton);
	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		Toggle::SetOn (True, eImmediateUpdate);
	}
	
	return (result);
}

Rect	RadioButton_WinUI_Portable::GetBox () const
{
	Rect extent = GetLocalExtent ();
	if (Odd (extent.GetHeight ())) {
		extent.SetSize (extent.GetSize () + Point (1, 0));	// we round up in placing the toggle box
	}
	Rect r =  CenterRectAroundRect (Rect (kZeroPoint, Point (GetToggleWidth (), GetToggleWidth ())), extent);
	r.SetOrigin (Point (r.GetOrigin ().GetV (), GetBorder ().GetH () + GetMargin ().GetH () + 2));
	return (r);
}








/*
 ********************************************************************************
 ******************************** RadioButton_MacUI ******************************
 ********************************************************************************
 */
RadioButton_MacUI::RadioButton_MacUI ():
#if		qNative && qMacToolkit
	RadioButton_MacUI_Native (kEmptyString, Nil)
#else
	RadioButton_MacUI_Portable (kEmptyString, Nil)
#endif
{
}

RadioButton_MacUI::RadioButton_MacUI (const String& label):
#if		qNative && qMacToolkit
	RadioButton_MacUI_Native (label, Nil)
#else
	RadioButton_MacUI_Portable (label, Nil)
#endif
{
}

RadioButton_MacUI::RadioButton_MacUI (const String& label, ButtonController* controller):
#if		qNative && qMacToolkit
	RadioButton_MacUI_Native (label, controller)
#else
	RadioButton_MacUI_Portable (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
RadioButton_MacUI::~RadioButton_MacUI () {}
#endif










/*
 ********************************************************************************
 ******************************* RadioButton_MotifUI *****************************
 ********************************************************************************
 */
RadioButton_MotifUI::RadioButton_MotifUI ():
#if		qNative && qXmToolkit
	RadioButton_MotifUI_Native (kEmptyString, Nil)
#else
	RadioButton_MotifUI_Portable (kEmptyString, Nil)
#endif
{
}

RadioButton_MotifUI::RadioButton_MotifUI (const String& label):
#if		qNative && qXmToolkit
	RadioButton_MotifUI_Native (label, Nil)
#else
	RadioButton_MotifUI_Portable (label, Nil)
#endif
{
}

RadioButton_MotifUI::RadioButton_MotifUI (const String& label, ButtonController* controller):
#if		qNative && qXmToolkit
	RadioButton_MotifUI_Native (label, controller)
#else
	RadioButton_MotifUI_Portable (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
RadioButton_MotifUI::~RadioButton_MotifUI () {}
#endif





/*
 ********************************************************************************
 ******************************** RadioButton_WinUI ******************************
 ********************************************************************************
 */
RadioButton_WinUI::RadioButton_WinUI ():
	RadioButton_WinUI_Portable (kEmptyString, Nil)
{
}

RadioButton_WinUI::RadioButton_WinUI (const String& label):
	RadioButton_WinUI_Portable (label, Nil)
{
}

RadioButton_WinUI::RadioButton_WinUI (const String& label, ButtonController* controller):
	RadioButton_WinUI_Portable (label, controller)
{
}

#if		qSoleInlineVirtualsNotStripped
RadioButton_WinUI::~RadioButton_WinUI () {}
#endif





/*
 ********************************************************************************
 ************************************* RadioButton *******************************
 ********************************************************************************
 */
RadioButton::RadioButton () :
#if		qMacUI
	RadioButton_MacUI (kEmptyString, Nil)
#elif	qMotifUI
	RadioButton_MotifUI (kEmptyString, Nil)
#elif	qWinUI
	RadioButton_WinUI (kEmptyString, Nil)
#endif
{
}

RadioButton::RadioButton (const String& label) :
#if		qMacUI
	RadioButton_MacUI (label, Nil)
#elif	qMotifUI
	RadioButton_MotifUI (label, Nil)
#elif	qWinUI
	RadioButton_WinUI (label, Nil)
#endif
{
}

RadioButton::RadioButton (const String& label, ButtonController* controller) :
#if		qMacUI
	RadioButton_MacUI (label, controller)
#elif	qMotifUI
	RadioButton_MotifUI (label, controller)
#elif	qWinUI
	RadioButton_WinUI (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
RadioButton::~RadioButton () {}
#endif





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

