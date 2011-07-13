/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CheckBox.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: CheckBox.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:33:15  sterling
 *		changed GUI to UI, add Mac_UI and Motif_UI variants
 *
 *		Revision 1.3  1992/07/03  00:20:45  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  07:54:59  sterling
 *		Sterl made lots of changes - LGP checking in - subclass from Toggle instead of Button.+++ MORE.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.45  1992/06/09  14:19:07  lewis
 *		Move SetBorder/SetMargin into CheckBox_MotifUI_Portable::CTOR at sterls behest - not sure right thing
 *		todo, but we can talk about it later.
 *
 *		Revision 1.43  92/06/09  15:26:06  15:26:06  sterling (Sterling Wight)
 *		motif portable version
 *		
 *		Revision 1.42  92/05/13  02:05:06  02:05:06  lewis (Lewis Pringle)
 *		STERL: overload SetLabel () to take const char* arg.
 *		Get rid of default arg, and overload more CTOR for CheckBox so we can specify
 *		no args.
 *		
 *		Revision 1.41  92/04/20  14:31:35  14:31:35  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		And used Min (GetBorderHeight (), GetBorderWidth ()) instead of CalcShadowThickness () since that routine went away
 *		from Bordered.cc
 *		
 *		Revision 1.40  92/04/14  07:32:55  07:32:55  lewis (Lewis Pringle)
 *		Added virtual dtor for CheckBox conditioned on qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.39  92/04/02  13:07:22  13:07:22  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.38  92/03/26  18:31:25  18:31:25  lewis (Lewis Pringle)
 *		Add overrides of EffectiveFontChanged () so we inval layouts, etc, as appropriate.
 *		
 *		Revision 1.37  1992/03/26  15:15:20  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of oldFocused first argument.
 *
 *		Revision 1.36  1992/03/26  09:46:21  lewis
 *		Added new font/update mode for EffecitveFontChanged () method. Got rid of oldLive param to EffectiveLiveChanged.
 *
 *		Revision 1.34  1992/03/19  16:57:38  lewis
 *		Minor cleanups, and call AbstractCheckBox_MotifUI::EffectiveFocusedChanged instead of skipping to
 *		FocusItem:: - was done cuz button did bad things there, but thats now gone.
 *
 *		Revision 1.33  1992/03/17  17:03:42  lewis
 *		Override EffectiveFocusChanged () to do XmProcessTraversal for motif - not sure how to do unfocus case??
 *		Also, skip base classes inherited - go directly to FocusItem - since we still have crap in Button.cc.
 *
 *		Revision 1.32  1992/03/09  23:44:23  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.31  1992/03/09  15:15:39  lewis
 *		Worked around qCFRONT_ARRAYS_IN_INLINE_CAUSE_ERROR_BUG.
 *
 *		Revision 1.28  1992/03/05  21:35:21  sterling
 *		support for Borders
 *
 *		Revision 1.27  1992/02/24  06:45:47  lewis
 *		Button trackers called with wrong args.
 *
 *		Revision 1.26  1992/02/21  20:11:12  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.25  1992/02/19  22:20:18  sterling
 *		better default size for Motif
 *
 *		Revision 1.24  1992/02/15  05:31:06  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.23  1992/02/06  21:12:03  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.22  1992/01/31  22:47:29  lewis
 *		Added "CheckBox" class.
 *
 *		Revision 1.21  1992/01/24  17:56:32  lewis
 *		Delete ensure that set sensative worked since we may be unrealized.
 *
 *		Revision 1.20  1992/01/23  19:50:13  lewis
 *		Override realize in motif so we can patch values in widget after its realized. Also, covnertr macUI to MacUI
 *		Also, use protected virtual _ methods with public accessor wrappers style.
 *
 *		Revision 1.19  1992/01/23  08:08:26  lewis
 *		Keep local values, and dont count on OSControls to keep values while unrealized any longer.
 *		We still must override realize in this guy, I think...
 *
 *		Revision 1.10  1992/01/10  03:19:54  lewis
 *		Called setsensative as appropriate for motif checkboxes and got rid of half-implemented bogus
 *		native motif version.
 *		Also, tried to implement CalcDefaultSize () for native widget calling MotifOSControl, but
 *		that didn't work - investigate later.
 *
 *
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
#include	<X11/IntrinsicP.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<X11/StringDefs.h>
#include	<Xm/Xm.h>
#include	<Xm/RowColumn.h>
#include 	<Xm/ToggleB.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"


#include	"StreamUtils.hh"

#include	"Pallet.hh"
#include	"Shape.hh"

#include	"OSControls.hh"

#include	"CheckBox.hh"






/*
 ********************************************************************************
 ******************************** AbstractCheckBox ******************************
 ********************************************************************************
 */

AbstractCheckBox::AbstractCheckBox (ButtonController* controller):
	Toggle (controller)
{
}

String	AbstractCheckBox::GetLabel () const
{
	return (GetLabel_ ());
}

void	AbstractCheckBox::SetLabel (const String& label, Panel::UpdateMode updateMode)
{
	if (GetLabel () != label) {
		SetLabel_ (label, updateMode);
	}
	Ensure (GetLabel () == label);
}

void	AbstractCheckBox::SetLabel (const char* cString, Panel::UpdateMode updateMode)
{
	RequireNotNil (cString);
	String label = String (cString);
	if (GetLabel () != label) {
		SetLabel_ (label, updateMode);
	}
	Ensure (GetLabel () == label);
}







/*
 ********************************************************************************
 ****************************** AbstractCheckBox_MacUI *************************
 ********************************************************************************
 */
AbstractCheckBox_MacUI::AbstractCheckBox_MacUI (ButtonController* controller):
	AbstractCheckBox (controller)
{
	SetFont (&kSystemFont);
	SetBackground (&kWhiteTile);
}

Coordinate	AbstractCheckBox_MacUI::GetToggleWidth () const
{
	return (12);
}

Coordinate	AbstractCheckBox_MacUI::GetToggleGap () const
{
	return (4);
}

Point	AbstractCheckBox_MacUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	size = Point (
		Max (GetEffectiveFont ().GetFontHeight (), GetToggleWidth ()), 
		3 + GetToggleWidth () + GetToggleGap () + TextWidth (GetLabel ()));

	return (size + GetBorder ()*2 + GetMargin ()*2);
}

void	AbstractCheckBox_MacUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes, our Layout ()
	 * since our placement of subcomponents changes, and Refresh () since our visual display depends
	 * on the current font.
	 */

	AbstractCheckBox::EffectiveFontChanged (newFont, eNoUpdate);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);					// cuz our display may change
	}
}







/*
 ********************************************************************************
 **************************** CheckBox_MacUI_Portable **************************
 ********************************************************************************
 */
CheckBox_MacUI_Portable::CheckBox_MacUI_Portable (const String& label, ButtonController* controller):
	AbstractCheckBox_MacUI (controller),
	fLabel (label)
{
}

void	CheckBox_MacUI_Portable::Draw (const Region& /*update*/)
{
	DrawToggle (GetOn ());

	// should take GetEffectiveLive into account when drawing text
	DrawText (GetLabel (), Point (
		(GetSize ().GetV () - GetEffectiveFont ().GetFontHeight ())/2, 
		GetBox ().GetRight () + GetToggleGap ()));

	DrawBorder ();
}

String	CheckBox_MacUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	CheckBox_MacUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}

void	CheckBox_MacUI_Portable::DrawToggle (Boolean on)
{
	Rect	r = GetBox ();
	OutLine (Rectangle (), r);
	if (on) {
		r.SetSize (r.GetSize () - Point (1, 1));
		Rect localR = Rect (kZeroPoint, r.GetSize ());
		OutLine (Line (localR.GetOrigin (), localR.GetBotRight () + Point (-1, 0)), r); 
		OutLine (Line (localR.GetBotLeft (), localR.GetTopRight ()), r); 
	}
	else {
		Erase (Rectangle (), InsetBy (GetBox (), Point (1, 1)));
	}
}

void	CheckBox_MacUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	AbstractCheckBox_MacUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		DrawToggle (on);
	}
	else if (updateMode != eNoUpdate) {
		Refresh (InsetBy (GetBox (), Point (1, 1)), updateMode);
	}
}

Boolean	CheckBox_MacUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if (mouseInButton) {
		OutLine (Rectangle (), InsetBy (GetBox (), Point (1, 1)));
	}
	else  if (phase != Tracker::eTrackRelease) {
		Erase (Rectangle (), InsetBy (GetBox (), Point (1, 1)));
		DrawToggle (GetOn ());
	}
	
	Boolean result = AbstractCheckBox_MacUI::Track (phase, mouseInButton);	

	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		// by default, toggle calls seton with eNoUpdate, so we must manually refresh
		Refresh (InsetBy (GetBox (), Point (1, 1)), eImmediateUpdate);
	}
	
	return (result);
}

Rect	CheckBox_MacUI_Portable::GetBox () const
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
 ***************************** CheckBox_MacUI_Native ***************************
 ********************************************************************************
 */

class	MacOSCheckBox : public MacOSControl {
	public:
		MacOSCheckBox (CheckBox_MacUI_Native& theCB, short procID = checkBoxProc):
			MacOSControl (procID),
			fCheckBox (theCB)
			{
				SetMin (0);
				SetMax (1);
				SetValue (theCB.GetOn ());
			}

			override	void	ActionProc (short partCode)
			{
				fCheckBox.Tracking (Tracker::eTrackMove, Boolean (partCode == inCheckBox));
			}
			
			override	Boolean	TrackPress (const MousePressInfo& mouseInfo)
			{
				Boolean aborted = fCheckBox.Tracking (Tracker::eTrackPress, True);
				return (Boolean (aborted or MacOSControl::TrackPress (mouseInfo)));
			}

			override	void	DoneProc (short partCode)
			{
				Boolean aborted = fCheckBox.Tracking (Tracker::eTrackRelease, Boolean (partCode == inCheckBox));				
				if ((not aborted) and (partCode == inCheckBox)) {
					fCheckBox.SetOn (not fCheckBox.GetOn (), eImmediateUpdate);
				}
			}
			
	private:
		CheckBox_MacUI_Native& fCheckBox;
};




CheckBox_MacUI_Native::CheckBox_MacUI_Native (const String& label, ButtonController* controller):
	AbstractCheckBox_MacUI (controller),
	fMacOSControl (Nil)
{
	AddSubView (fMacOSControl = new MacOSCheckBox (*this));
	SetLabel_ (label, eNoUpdate);
	fMacOSControl->SetHiliteState (GetEffectiveLive () ? 0 : 0xff, eNoUpdate);
}

CheckBox_MacUI_Native::~CheckBox_MacUI_Native ()
{
	RemoveSubView (fMacOSControl);
	delete fMacOSControl;
}

void	CheckBox_MacUI_Native::Draw (const Region& /*update*/)
{
	DrawBorder ();
}

void	CheckBox_MacUI_Native::SetBorder_ (const Point& border, Panel::UpdateMode updateMode)
{
	InvalidateLayout ();
	AbstractCheckBox_MacUI::SetBorder_ (border, updateMode);
}

void	CheckBox_MacUI_Native::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode)
{
	InvalidateLayout ();
	AbstractCheckBox_MacUI::SetMargin_ (margin, updateMode);
}

void	CheckBox_MacUI_Native::Layout ()
{
	AssertNotNil (fMacOSControl);
	Point	offset = GetBorder () + GetMargin ();
	fMacOSControl->SetOrigin (offset);
	fMacOSControl->SetSize (GetSize () - offset*2);
	AbstractCheckBox_MacUI::Layout ();
}

void	CheckBox_MacUI_Native::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractCheckBox_MacUI::EffectiveLiveChanged (newLive, eNoUpdate);
	AssertNotNil (fMacOSControl);
	fMacOSControl->SetHiliteState (newLive ? 0 : 0xff, updateMode);
}

String	CheckBox_MacUI_Native::GetLabel_ () const
{
	AssertNotNil (fMacOSControl);
	return (fMacOSControl->GetTitle ());
}

void	CheckBox_MacUI_Native::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	AssertNotNil (fMacOSControl);
	fMacOSControl->SetTitle (label, updateMode);
}

void	CheckBox_MacUI_Native::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	AssertNotNil (fMacOSControl);
	fMacOSControl->SetValue (on, updateMode);
	AbstractCheckBox_MacUI::SetOn_ (on, eNoUpdate);
}


#endif	/*qMacToolkit*/







/*
 ********************************************************************************
 **************************** AbstractCheckBox_MotifUI *************************
 ********************************************************************************
 */
AbstractCheckBox_MotifUI::AbstractCheckBox_MotifUI (ButtonController* controller):
	AbstractCheckBox (controller)
{
	SetBorder (2, 2, eNoUpdate);
	SetMargin (2, 2, eNoUpdate);
    SetFont (&kSystemFont);
}

Boolean	AbstractCheckBox_MotifUI::HandleKeyStroke (const KeyStroke& keyStroke)
{
// Maybe should have some auxiliary functions (static maybe) in FocusItem to say which keystrokes get
// interpreted as being part of the mouseless interface, and how they get interpreted???
	if ((keyStroke == KeyStroke::kEnter) or (keyStroke == KeyStroke::kReturn) or (keyStroke == KeyStroke::kTab)) {
		AbstractCheckBox::SetOn (not GetOn (), eImmediateUpdate);
		return (True);
	}
	return (False);
}

Boolean	AbstractCheckBox_MotifUI::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	refocused = False;
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		refocused = True;
	}

	return (Boolean (AbstractCheckBox::TrackPress (mouseInfo) or refocused));
}

Coordinate	AbstractCheckBox_MotifUI::GetToggleWidth () const
{
	// plus one is motif black magic
   	return (GetEffectiveFont ().GetFontHeight () + 1);
}

Coordinate	AbstractCheckBox_MotifUI::GetToggleGap () const
{
   	return (GetToggleWidth () / 4);
}

Point	AbstractCheckBox_MotifUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Coordinate toggleWidth = GetToggleWidth ();
	Point newDefaultSize = Point (toggleWidth, TextWidth (GetLabel ()) + GetToggleGap () + toggleWidth + 5); // plus five is pure black magic

	// Motif checkboxes are screwy in that only the margin affects the size of the control
	return (newDefaultSize + GetMargin ()*2);
}

void	AbstractCheckBox_MotifUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */

	AbstractCheckBox::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);				// cuz our display may change
	}
}

Boolean	AbstractCheckBox_MotifUI::GetLive () const
{
	return (AbstractCheckBox::GetLive ());
}

void	AbstractCheckBox_MotifUI::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractCheckBox::EffectiveLiveChanged (newLive, updateMode);
}

Boolean	AbstractCheckBox_MotifUI::GetEffectiveLive () const
{
	// only View overrides this, but C++ insists we disambiguate anyhow
	return (View::GetEffectiveLive ());
}





/*
 ********************************************************************************
 ***************************** CheckBox_MotifUI_Portable ***********************
 ********************************************************************************
 */
CheckBox_MotifUI_Portable::CheckBox_MotifUI_Portable (const String& label, ButtonController* controller):
	AbstractCheckBox_MotifUI (controller),
	fLabel (label)
{
}

void	CheckBox_MotifUI_Portable::Draw (const Region& /*update*/)
{
	DrawToggle (GetOn ());

	// should take GetEffectiveLive into account when drawing text
	DrawText (GetLabel (), Point (
		(GetSize ().GetV () - GetEffectiveFont ().GetFontHeight ())/2, 
		GetBox ().GetRight () + GetToggleGap ()));
}

void	CheckBox_MotifUI_Portable::DrawToggle (Boolean on)
{
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (GetBox (), GetBorder (), c, (on) ? Bordered::eIn : Bordered::eOut);

	Color bg = c;
	if (on) {
		Color	selectColor;
		Color	topShadow;
		Color	bottomShadow;
		
		CalcColorSet (c, selectColor, topShadow, bottomShadow);
		bg = selectColor;
	}

	Tile	selectTile	=	PalletManager::Get ().MakeTileFromColor (bg);
	Paint (Rectangle (), GetBox ().InsetBy (GetBorder () + Point (1, 1)), selectTile);
}

Boolean	CheckBox_MotifUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	DrawToggle (mouseInButton ^ GetOn ());	
	return (AbstractCheckBox_MotifUI::Track (phase, mouseInButton));
}

Rect	CheckBox_MotifUI_Portable::GetBox () const
{
	return (Rect (GetMargin (), Point (GetToggleWidth (), GetToggleWidth ())));
}

void	CheckBox_MotifUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	AbstractCheckBox_MotifUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		DrawToggle (on);
	}
	else {
		Refresh (GetBox (), updateMode);
	}
}

String	CheckBox_MotifUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	CheckBox_MotifUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}







#if 	qXmToolkit
/*
 ********************************************************************************
 ***************************** CheckBox_MotifUI_Native *************************
 ********************************************************************************
 */

CheckBox_MotifUI_Native::CheckBox_MotifUI_Native (const String& label, ButtonController* controller):
	AbstractCheckBox_MotifUI (controller),
	fOSControl (Nil),
	fLabel (label)
{
	class	MyOSCheckBox : public MotifOSControl {
		public:
			MyOSCheckBox (AbstractCheckBox& checkBox):
				MotifOSControl (),
				fCheckBox (checkBox)
			{
			}

			override	osWidget*	DoBuild_ (osWidget* parent)
			{
				RequireNotNil (parent);
#if		qCFRONT_ARRAYS_IN_INLINE_CAUSE_ERROR_BUG
				struct hack_cuz_array_buf {
					Arg	args [2];
				} xxx;
				Arg* args = xxx.args;
#else
				Arg	args [2];
#endif
				XtSetArg (args[0], XmNindicatorType, 	XmN_OF_MANY);
				XtSetArg (args[1], XmNalignment, 		XmALIGNMENT_BEGINNING);

				osWidget* w = ::XmCreateToggleButton (parent, "", args, 2);

				::XtAddCallback (w, XmNvalueChangedCallback, (XtCallbackProc)sCallBackProc, (XtPointer)&fCheckBox);

				return (w);
			}

			static void sCallBackProc (osWidget* w, caddr_t client_data, caddr_t /*call_data*/)
		  	{
				fCheckBox.Tracking (Tracker::eTrackRelease, True);
					
				osBoolean	osb = 0;
				Arg		arg;
				XtSetArg (arg, XmNset, &osb);
				::XtGetValues (w, &arg, 1);

				CheckBox_MotifUI_Native* fred = (CheckBox_MotifUI_Native*)client_data;
				AssertNotNil (fred);
				fred->AbstractCheckBox::SetOn (Boolean (!!osb), eNoUpdate);
			}

		private:
			AbstractCheckBox&	fCheckBox;
	};
	AddSubView (fOSControl = new MyOSCheckBox (*this));
	SetLabel_ (label, eNoUpdate);
	SetBorder_ (GetBorder (), eNoUpdate);
	SetMargin_ (GetMargin (), eNoUpdate);
}

CheckBox_MotifUI_Native::~CheckBox_MotifUI_Native ()
{
	RemoveSubView (fOSControl);
	delete fOSControl;
}

void	CheckBox_MotifUI_Native::SetBorder_ (const Point& border, UpdateMode updateMode)
{
	AbstractCheckBox_MotifUI::SetBorder_ (border, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
	Refresh (updateMode);
}

void	CheckBox_MotifUI_Native::SetMargin_ (const Point& margin, UpdateMode updateMode)
{
	AbstractCheckBox_MotifUI::SetMargin_ (margin, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetDimensionResourceValue (XmNmarginHeight, (unsigned short)GetMarginHeight ());
    fOSControl->SetDimensionResourceValue (XmNmarginWidth, (unsigned short)GetMarginWidth ());
	Refresh (updateMode);
}

Point	CheckBox_MotifUI_Native::CalcDefaultSize (const Point& defaultSize) const
{
	return (AbstractCheckBox_MotifUI::CalcDefaultSize (defaultSize));
}

String	CheckBox_MotifUI_Native::GetLabel_ () const
{
	AssertNotNil (fOSControl);
	return (fLabel);
}

void	CheckBox_MotifUI_Native::SetLabel_ (const String& label, UpdateMode updateMode)
{
	fLabel = label;
	AssertNotNil (fOSControl);
	fOSControl->SetStringResourceValue (XmNlabelString, label);
}

void	CheckBox_MotifUI_Native::Layout ()
{
	AbstractCheckBox_MotifUI::Layout ();
	AssertNotNil (fOSControl);
	Assert (fOSControl->GetOrigin () == kZeroPoint);
	fOSControl->SetSize (GetSize ());
}

void	CheckBox_MotifUI_Native::EffectiveFocusChanged (Boolean newFocused, UpdateMode updateMode)
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
	AbstractCheckBox_MotifUI::EffectiveFocusChanged (newFocused, updateMode);
}

void	CheckBox_MotifUI_Native::EffectiveFontChanged (const Font& newFont, UpdateMode updateMode)
{
	AbstractCheckBox_MotifUI::EffectiveFontChanged (newFont, updateMode);

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

void	CheckBox_MotifUI_Native::SetOn_ (Boolean on, UpdateMode updateMode)
{
	AbstractCheckBox_MotifUI::SetOn_ (on, updateMode);
	fOSControl->SetBooleanResourceValue (XmNset, on);
}

void	CheckBox_MotifUI_Native::EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode)
{
	AbstractCheckBox_MotifUI::EffectiveLiveChanged (newLive, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetBooleanResourceValue (XmNsensitive, newLive);
}

void	CheckBox_MotifUI_Native::Realize (osWidget* parent)
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

void	CheckBox_MotifUI_Native::UnRealize ()
{
	AssertNotNil (fOSControl);
	fOSControl->UnRealize ();
}

#endif	/*qXmToolkit*/




/*
 ********************************************************************************
 ****************************** AbstractCheckBox_WinUI *************************
 ********************************************************************************
 */
AbstractCheckBox_WinUI::AbstractCheckBox_WinUI (ButtonController* controller):
	AbstractCheckBox (controller)
{
	/*
	 * hack - Tom says this is a good choice for winUI
	 */
	Font	f	=	Font ("Helvetica", 12, Set_BitString(FontStyle) (Font::eBoldFontStyle));
	SetFont (&f);

	SetBackground (&kWhiteTile);
}

Coordinate	AbstractCheckBox_WinUI::GetToggleWidth () const
{
	return (12);
}

Coordinate	AbstractCheckBox_WinUI::GetToggleGap () const
{
	return (4);
}

Point	AbstractCheckBox_WinUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	size = Point (
		Max (GetEffectiveFont ().GetFontHeight (), GetToggleWidth ()), 
		3 + GetToggleWidth () + GetToggleGap () + TextWidth (GetLabel ()));

	return (size + GetBorder ()*2 + GetMargin ()*2);
}

void	AbstractCheckBox_WinUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes, our Layout ()
	 * since our placement of subcomponents changes, and Refresh () since our visual display depends
	 * on the current font.
	 */

	AbstractCheckBox::EffectiveFontChanged (newFont, eNoUpdate);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);					// cuz our display may change
	}
}


/*
 ********************************************************************************
 **************************** CheckBox_WinUI_Portable **************************
 ********************************************************************************
 */
CheckBox_WinUI_Portable::CheckBox_WinUI_Portable (const String& label, ButtonController* controller):
	AbstractCheckBox_WinUI (controller),
	fLabel (label)
{
}

void	CheckBox_WinUI_Portable::Draw (const Region& /*update*/)
{
	DrawToggle (GetOn ());

	// should take GetEffectiveLive into account when drawing text
	DrawText (GetLabel (), Point (
		(GetSize ().GetV () - GetEffectiveFont ().GetFontHeight ())/2, 
		GetBox ().GetRight () + GetToggleGap ()));

	DrawBorder ();
}

String	CheckBox_WinUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	CheckBox_WinUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}

void	CheckBox_WinUI_Portable::DrawToggle (Boolean on)
{
	Rect	r = GetBox ();
	OutLine (Rectangle (), r);
	if (on) {
		r.SetSize (r.GetSize () - Point (1, 1));
		Rect localR = Rect (kZeroPoint, r.GetSize ());
		OutLine (Line (localR.GetOrigin (), localR.GetBotRight () + Point (-1, 0)), r); 
		OutLine (Line (localR.GetBotLeft (), localR.GetTopRight ()), r); 
	}
	else {
		Erase (Rectangle (), InsetBy (GetBox (), Point (1, 1)));
	}
}

void	CheckBox_WinUI_Portable::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	AbstractCheckBox_WinUI::SetOn_ (on, eNoUpdate);
	if (updateMode == eImmediateUpdate) {
		DrawToggle (on);
	}
	else if (updateMode != eNoUpdate) {
		Refresh (InsetBy (GetBox (), Point (1, 1)), updateMode);
	}
}

Boolean	CheckBox_WinUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if (mouseInButton) {
		OutLine (Rectangle (), InsetBy (GetBox (), Point (1, 1)));
	}
	else  if (phase != Tracker::eTrackRelease) {
		Erase (Rectangle (), InsetBy (GetBox (), Point (1, 1)));
		DrawToggle (GetOn ());
	}
	
	Boolean result = AbstractCheckBox_WinUI::Track (phase, mouseInButton);	

	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		// by default, toggle calls seton with eNoUpdate, so we must manually refresh
		Refresh (InsetBy (GetBox (), Point (1, 1)), eImmediateUpdate);
	}
	
	return (result);
}

Rect	CheckBox_WinUI_Portable::GetBox () const
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
 ******************************** CheckBox_MacUI ******************************
 ********************************************************************************
 */
CheckBox_MacUI::CheckBox_MacUI ():
#if		qNative && qMacToolkit
	CheckBox_MacUI_Native (kEmptyString, Nil)
#else
	CheckBox_MacUI_Portable (kEmptyString, Nil)
#endif
{
}

CheckBox_MacUI::CheckBox_MacUI (const String& label):
#if		qNative && qMacToolkit
	CheckBox_MacUI_Native (label, Nil)
#else
	CheckBox_MacUI_Portable (label, Nil)
#endif
{
}

CheckBox_MacUI::CheckBox_MacUI (const String& label, ButtonController* controller):
#if		qNative && qMacToolkit
	CheckBox_MacUI_Native (label, controller)
#else
	CheckBox_MacUI_Portable (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
CheckBox_MacUI::~CheckBox_MacUI () {}
#endif










/*
 ********************************************************************************
 ******************************* CheckBox_MotifUI *****************************
 ********************************************************************************
 */
CheckBox_MotifUI::CheckBox_MotifUI ():
#if		qNative && qXmToolkit
	CheckBox_MotifUI_Native (kEmptyString, Nil)
#else
	CheckBox_MotifUI_Portable (kEmptyString, Nil)
#endif
{
}

CheckBox_MotifUI::CheckBox_MotifUI (const String& label):
#if		qNative && qXmToolkit
	CheckBox_MotifUI_Native (label, Nil)
#else
	CheckBox_MotifUI_Portable (label, Nil)
#endif
{
}

CheckBox_MotifUI::CheckBox_MotifUI (const String& label, ButtonController* controller):
#if		qNative && qXmToolkit
	CheckBox_MotifUI_Native (label, controller)
#else
	CheckBox_MotifUI_Portable (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
CheckBox_MotifUI::~CheckBox_MotifUI () {}
#endif




/*
 ********************************************************************************
 ******************************** CheckBox_WinUI ******************************
 ********************************************************************************
 */
CheckBox_WinUI::CheckBox_WinUI ():
	CheckBox_WinUI_Portable (kEmptyString, Nil)
{
}

CheckBox_WinUI::CheckBox_WinUI (const String& label):
	CheckBox_WinUI_Portable (label, Nil)
{
}

CheckBox_WinUI::CheckBox_WinUI (const String& label, ButtonController* controller):
	CheckBox_WinUI_Portable (label, controller)
{
}

#if		qSoleInlineVirtualsNotStripped
CheckBox_WinUI::~CheckBox_WinUI () {}
#endif






/*
 ********************************************************************************
 ************************************* CheckBox *******************************
 ********************************************************************************
 */
CheckBox::CheckBox () :
#if		qMacUI
	CheckBox_MacUI (kEmptyString, Nil)
#elif	qMotifUI
	CheckBox_MotifUI (kEmptyString, Nil)
#elif	qWinUI
	CheckBox_WinUI (kEmptyString, Nil)
#endif
{
}

CheckBox::CheckBox (const String& label) :
#if		qMacUI
	CheckBox_MacUI (label, Nil)
#elif	qMotifUI
	CheckBox_MotifUI (label, Nil)
#elif	qWinUI
	CheckBox_WinUI (label, Nil)
#endif
{
}

CheckBox::CheckBox (const String& label, ButtonController* controller) :
#if		qMacUI
	CheckBox_MacUI (label, controller)
#elif	qMotifUI
	CheckBox_MotifUI (label, controller)
#elif	qWinUI
	CheckBox_WinUI (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
CheckBox::~CheckBox () {}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

