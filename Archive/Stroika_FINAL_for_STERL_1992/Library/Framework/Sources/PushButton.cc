/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PushButton.cc,v 1.9 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *		-	Need to add pushbutton controls, etc, and do something similar to FocusOwners for
 *		dealing with default buttons and the mosueless interface. Should be fairly transparent
 *		change since we can do alot in Emily codegen, and Dialog itself. We must implement that when
 *		we get the focus (if we are using mouseless interface) we then also set self as default button.
 *		(May want to parameterize this behaviour, but it is the default). Very weird. LGP April 2, 1992.
 *
 * Changes:
 *	$Log: PushButton.cc,v $
 *		Revision 1.9  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.8  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.7  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.6  1992/07/16  16:49:42  sterling
 *		default to portable version on Mac, to avoid native control bug in system 7
 *
 *		Revision 1.5  1992/07/08  03:29:11  lewis
 *		Cleanup comments.
 *		Rename all GUI class names to UI (as per Toms suggestion - didnt change #defines
 *		yet. Added PushButton_MacUI/PushButton_MotifUI to pick out native/portable,
 *		and then have PushButton subclass of these. I like this MUCH better, but
 *		without good compilers/flunging you pay slight size/performance cost,
 *		so we must be sure worth while, and/or that we get those technologies moving
 *		along.
 *
 *		Revision 1.4  1992/07/04  14:21:04  lewis
 *		Take advantage of new shape letter/envelope support- pass by value,
 *		instead of reference.
 *
 *		Revision 1.3  1992/07/03  02:11:37  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:29:09  sterling
 *		Lots of changes including base class Button changes, and mixin of
 *		FocusItem in right place and renamed CalcDefaultSize to CalcDefaultSize_.
 *		Use new Bordered stuff to fixup appearance of motif PMB.
 *		+++ (LGP checked in).
 *
 *		Revision 1.57  1992/06/09  14:25:16  lewis
 *		Added #include.
 *
 *		Revision 1.56  92/06/09  15:24:54  15:24:54  sterling (Sterling Wight)
 *		momotif portable version
 *		
 *		Revision 1.55  92/05/13  02:12:14  02:12:14  lewis (Lewis Pringle)
 *		STERL: overload SetLabel () to take const char* arg.
 *		Get rid of default arg, and overload more CTOR for PushButton so we can
 *		specify no args.
 *		
 *		Revision 1.54  92/04/20  14:34:06  14:34:06  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h
 *		the distribute not posix compliant. Also, use Min (GetBorderHeight (), GetBorderWidth ())
 *		instead of Bordered::CalcShadowThickness () since that rouitine went away -
 *		See rcslog in Bordered.hh.
 *
 *		Revision 1.53  92/04/19  23:13:16  23:13:16  lewis (Lewis Pringle)
 *		Made tenative change in processing of native pushbuttons - hack to call
 *		seton/setoff is really a bad idea. Must negotiate the concept with sterl,
 *		but I think we must get rid of that - at any rate, it lead to bugs with
 *		mac os controls.
 *
 *		Revision 1.52  92/04/17  01:18:46  01:18:46  lewis (Lewis Pringle)
 *		Changed how motif native pushbnuttons handles clicks - have the callback
 *		just set flag, and when trackpress returns, then call seton/setoff.
 *
 *		Revision 1.51  92/04/16  17:00:02  17:00:02  lewis (Lewis Pringle)
 *		Quickie implementation of Motif portable pushbutton. Needs alot more work -
 *		I really only tried to do draw, and not tracking properly. Need to figure
 *		out how to get proper shadows colors/tiles for shadows.
 *
 *		Revision 1.50  92/04/15  00:35:09  00:35:09  lewis (Lewis Pringle)
 *		Call View::Realize instead of fOSControl->Realize () so we get
 *		OurHackWidget created properly.
 *		Not sure how all this will work out - much work tobe done on this crap.
 *		
 *		Revision 1.49  92/04/14  19:40:28  19:40:28  lewis (Lewis Pringle)
 *		Add EnsureNotNil at end of Motif Realize, and put name into creation of
 *		XmPushButton - easeir for debugging.
 *		Maybe only do with debug turned on???
 *		
 *		Revision 1.48  92/04/14  07:34:29  07:34:29  lewis (Lewis Pringle)
 *		Added out of line virtual dtor for PushButton conditioned on qSoleInlineVirtualsNotStripped.
 *		
 *		Revision 1.47  92/04/02  13:07:31  13:07:31  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.46  92/03/26  18:34:29  18:34:29  lewis (Lewis Pringle)
 *		Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *		
 *		Revision 1.44  1992/03/26  15:15:20  lewis
 *		Changed EffectiveFocusedChanged () to EffectiveFocusChanged(), and got rid of oldFocused first argument.
 *
 *		Revision 1.43  1992/03/26  09:51:52  lewis
 *		Got rid of oldLive param to EffectiveLiveChanged.
 *
 *		Revision 1.41  1992/03/19  16:59:39  lewis
 *		Minor cleanups, and call AbstractPushButton_MotifUI::EffectiveFocusedChanged instead of skipping to
 *		FocusItem:: - was done cuz button did bad things there, but thats now gone.
 *		Also, tried to get rid of flicker in clicking in button.
 *
 *		Revision 1.40  1992/03/17  17:03:42  lewis
 *		Override EffectiveFocusChanged () to do XmProcessTraversal for motif - not sure how to do unfocus case??
 *		Also, skip base classes inherited - go directly to FocusItem - since we still have crap in Button.cc.
 *
 *		Revision 1.39  1992/03/10  00:04:03  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.37  1992/03/05  20:49:33  sterling
 *		support for borders
 *
 *		Revision 1.36  1992/02/24  06:49:16  lewis
 *		Fix coords passed to ButtonTracker -- sterl.
 *
 *		Revision 1.35  1992/02/19  22:20:18  sterling
 *		better default size for Motif
 *
 *		Revision 1.34  1992/02/15  05:33:24  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.32  1992/02/03  17:15:27  lewis
 *		Changed #define/typedef implementation of PushButton to real class.
 *
 *		Revision 1.31  1992/01/27  06:17:14  lewis
 *		Make more terse motif code for activate callback, and call os control update of label directly rather than
 *		relying on SetLabel_ to do the trick - mostly for consistentcy with the others - not too sure about this but
 *		question arises about what guarantees we make to subclasses about when this method gets called. Here things seem safe,
 *		but they are not with the SetOn_ method in our base class... It triggers a ButtonPressed in the controller if you call it!!!!
 *
 *		Revision 1.30  1992/01/24  17:58:33  lewis
 *		Get rid of ensures that Sets of resources worked since we may be unrealized.
 *
 *		Revision 1.28  1992/01/23  19:57:41  lewis
 *		override unrealize for consistency sake, and also, call needed oscontrol setters in realize override (motif native pushbutton).
 *
 *		Revision 1.27  1992/01/23  08:10:40  lewis
 *		Use new protected _ technique, and also kept extra info for pushbuttons to support change in motifoscontrols - it
 *		no longer keeps track of things while the widget is unrealized.
 *
 *		Revision 1.24  1992/01/17  17:52:54  lewis
 *		Deleted bogus motif portable implementation.
 *
 *		Revision 1.18  1992/01/03  18:12:33  lewis
 *		Do better CalcDefaultSize (though still not right) for motif.
 *
 *		Revision 1.17  1991/12/27  22:39:20  lewis
 *		Updated SetSensative for motif pushbuttons on activate, and setlive calls.
 *
 *		Revision 1.16  1991/12/27  17:03:18  lewis
 *		Added support for activate callback under motif.
 *
 *
 */


#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Controls.h>
#include	<Windows.h>
#elif 	qXmToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/PushB.h>
#endif	/*qToolkit*/
#include	"OSRenamePost.hh"

#include	"StreamUtils.hh"

#include	"Pallet.hh"
#include	"Shape.hh"

#include	"Adornment.hh"
#include	"EventManager.hh"
#include	"OSControls.hh"

#include	"PushButton.hh"










/*
 ********************************************************************************
 *********************************** AbstractPushButton *************************
 ********************************************************************************
 */

AbstractPushButton::AbstractPushButton (ButtonController* controller):
	Button (controller)
{
}

String	AbstractPushButton::GetLabel () const
{
	return (GetLabel_ ());
}

void	AbstractPushButton::SetLabel (const String& label, Panel::UpdateMode updateMode)
{
	if (label != GetLabel ()) {
		SetLabel_ (label, updateMode);
	}
	Ensure (label == GetLabel ());
}

void	AbstractPushButton::SetLabel (const char* cString, Panel::UpdateMode updateMode)
{
	RequireNotNil (cString);
	String label = String (cString);
	if (label != GetLabel ()) {
		SetLabel_ (label, updateMode);
	}
	Ensure (label == GetLabel ());
}

Boolean	AbstractPushButton::GetIsDefault () const
{
	return (GetIsDefault_ ());
}

void	AbstractPushButton::SetIsDefault (Boolean isDefault, Panel::UpdateMode updateMode)
{
	if (isDefault != GetIsDefault ()) {
		SetIsDefault_ (isDefault, updateMode);
	}
}

void	AbstractPushButton::Flash ()
{
	Flash_ ();

	Pressed ();
}










/*
 ********************************************************************************
 ****************************** AbstractPushButton_MacUI ************************
 ********************************************************************************
 */

AbstractPushButton_MacUI::AbstractPushButton_MacUI (ButtonController* controller):
	AbstractPushButton (controller),
	fDefaultButtonAdornment (Nil)
{
	SetBackground (&kWhiteTile);
	SetFont (&kSystemFont);
	SetBorder (1, 1, eNoUpdate);
	SetMargin (1, 4, eNoUpdate);
}

void	AbstractPushButton_MacUI::SetSize_ (const Point& newSize, Panel::UpdateMode updateMode)
{
	AbstractPushButton::SetSize_ (newSize, updateMode);
	
	Coordinate	newRounding = GetSize ().GetV () / 2;
	SetShape (RoundedRectangle (Point (newRounding, newRounding)));
	RebuildAdornment_ (GetIsDefault (), updateMode);	/* needed if adornment is based on shape */
}

Point	AbstractPushButton_MacUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	const	Coordinate	kMinWidth = 50;
	Point size = Point (
		GetEffectiveFont ().GetFontHeight (), 
		Max (TextWidth (GetLabel ()), kMinWidth));

	return (size + GetBorder ()*2 + GetMargin ()*2);
}

Boolean	AbstractPushButton_MacUI::GetIsDefault_ ()	const
{	
	return (Boolean (fDefaultButtonAdornment != Nil));	
}

void	AbstractPushButton_MacUI::SetIsDefault_ (Boolean isDefault, Panel::UpdateMode updateMode)
{
	if (GetIsDefault () != isDefault) {
		RebuildAdornment_ (isDefault, updateMode);
	}
	Ensure (GetIsDefault () == isDefault);
}

void	AbstractPushButton_MacUI::RebuildAdornment_ (Boolean isDefault, Panel::UpdateMode updateMode)
{
	if (isDefault) {
#if 	1		
		/*
		 * According to IM 1-407, the rounding should be 16, 16.  Perhaps we should
		 * keep it variable, but lets try apples way for a while.  Also, there is where
		 * the say PenSize = 3,3 and Inset buttonrect by -4, -4.
		 */
		const	Coordinate	kRounding = 16;
#else
		const	Coordinate	kRounding = GetSize ().GetV () * 4 / 5;
#endif
		if (fDefaultButtonAdornment != Nil) {
			RemoveAdornment (fDefaultButtonAdornment, updateMode);
			fDefaultButtonAdornment = Nil;
		}
		const	Pen		kDefaultAdornPen	=	Pen (kBlackTile, eCopyTMode, Point (3, 3));
		fDefaultButtonAdornment = new ShapeAdornment (RoundedRectangle (Point (kRounding, kRounding)), kDefaultAdornPen, Point (-4, -4));
		AddAdornment (fDefaultButtonAdornment, updateMode);
		Ensure (fDefaultButtonAdornment != Nil);
	}
	else {
		if (fDefaultButtonAdornment != Nil) {
			RemoveAdornment (fDefaultButtonAdornment, updateMode);
			fDefaultButtonAdornment = Nil;
		}
		Ensure (fDefaultButtonAdornment == Nil);
	}
}

void	AbstractPushButton_MacUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */
	AbstractPushButton::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);				// cuz our display may change
	}
}










/*
 ********************************************************************************
 ***************************** AbstractPushButton_MotifUI ***********************
 ********************************************************************************
 */

AbstractPushButton_MotifUI::AbstractPushButton_MotifUI (ButtonController* controller):
	AbstractPushButton (controller),
	fDefaultButtonAdornment (Nil)
{
	SetFont (&kSystemFont);
	SetBorder (2, 2, eNoUpdate);
	SetMargin (2, 8, eNoUpdate);	// changed default on recommendation of Kobara, pg 57
}

Boolean	AbstractPushButton_MotifUI::HandleKeyStroke (const KeyStroke& keyStroke)
{
// Maybe should have some auxiliary functions (static maybe) in FocusItem to say which keystrokes get
// interpretted as being part of the mouseless interface, and how they get interpretted???
	if ((keyStroke == KeyStroke::kEnter) or (keyStroke == KeyStroke::kReturn) or (keyStroke == KeyStroke::kTab)) {
		Flash ();
		return (True);
	}
	return (False);
}

Boolean	AbstractPushButton_MotifUI::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	refocused = False;
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		refocused = True;
	}

	return (Boolean (AbstractPushButton::TrackPress (mouseInfo) or refocused));
}

void	AbstractPushButton_MotifUI::SetSize_ (const Point& newSize, Panel::UpdateMode updateMode)
{
	AbstractPushButton::SetSize_ (newSize, updateMode);
}

Point	AbstractPushButton_MotifUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	const	Coordinate	kMinWidth = 50;
	Point size = Point (
		GetEffectiveFont ().GetFontHeight (),
		Max (TextWidth (GetLabel ()), kMinWidth));

	return (size + GetBorder ()*2 + GetMargin ()*2);
}

Boolean	AbstractPushButton_MotifUI::GetIsDefault_ ()	const
{	
//	AssertNotImplemented ();
return (False);
}

void	AbstractPushButton_MotifUI::SetIsDefault_ (Boolean isDefault, Panel::UpdateMode updateMode)
{
//	AssertNotImplemented ();
}

void	AbstractPushButton_MotifUI::RebuildAdornment_ (Boolean isDefault, Panel::UpdateMode updateMode)
{
//	AssertNotImplemented ();
}

void	AbstractPushButton_MotifUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */
	AbstractPushButton::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);				// cuz our display may change
	}
}

Boolean	AbstractPushButton_MotifUI::GetLive () const
{
	return (AbstractPushButton::GetLive ());
}

void	AbstractPushButton_MotifUI::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractPushButton::EffectiveLiveChanged (newLive, updateMode);
}

Boolean	AbstractPushButton_MotifUI::GetEffectiveLive () const
{
	// only View overrides this, but C++ insists we disambiguate anyhow
	return (View::GetEffectiveLive ());
}





/*
 ********************************************************************************
 ***************************** AbstractPushButton_WinUI *************************
 ********************************************************************************
 */

AbstractPushButton_WinUI::AbstractPushButton_WinUI (ButtonController* controller):
	AbstractPushButton (controller),
	fDefaultButtonAdornment (Nil)
{
	/*
	 * hack - Tom says this is a good choice for winUI
	 */
	Font	f	=	Font ("Helvetica", 12, Set_BitString(FontStyle) (Font::eBoldFontStyle));
	SetFont (&f);

	SetBorder (2, 2, eNoUpdate);
	SetMargin (2, 8, eNoUpdate);	// changed default on recommendation of Kobara, pg 57
}

Boolean	AbstractPushButton_WinUI::HandleKeyStroke (const KeyStroke& keyStroke)
{
// Maybe should have some auxiliary functions (static maybe) in FocusItem to say which keystrokes get
// interpretted as being part of the mouseless interface, and how they get interpretted???
	if ((keyStroke == KeyStroke::kEnter) or (keyStroke == KeyStroke::kReturn) or (keyStroke == KeyStroke::kTab)) {
		Flash ();
		return (True);
	}
	return (False);
}

Boolean	AbstractPushButton_WinUI::TrackPress (const MousePressInfo& mouseInfo)
{
	Boolean	refocused = False;
	if ((not GetEffectiveFocused ()) and CanBeFocused ()) {
		GrabFocus (eDelayedUpdate, kValidate);
		refocused = True;
	}

	return (Boolean (AbstractPushButton::TrackPress (mouseInfo) or refocused));
}

void	AbstractPushButton_WinUI::SetSize_ (const Point& newSize, Panel::UpdateMode updateMode)
{
	AbstractPushButton::SetSize_ (newSize, updateMode);
}

Point	AbstractPushButton_WinUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	const	Coordinate	kMinWidth = 50;
	Point size = Point (
		GetEffectiveFont ().GetFontHeight (),
		Max (TextWidth (GetLabel ()), kMinWidth));

	return (size + GetBorder ()*2 + GetMargin ()*2);
}

Boolean	AbstractPushButton_WinUI::GetIsDefault_ ()	const
{	
//	AssertNotImplemented ();
return (False);
}

void	AbstractPushButton_WinUI::SetIsDefault_ (Boolean isDefault, Panel::UpdateMode updateMode)
{
//	AssertNotImplemented ();
}

void	AbstractPushButton_WinUI::RebuildAdornment_ (Boolean isDefault, Panel::UpdateMode updateMode)
{
//	AssertNotImplemented ();
}

void	AbstractPushButton_WinUI::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */
	AbstractPushButton::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
		Refresh (updateMode);				// cuz our display may change
	}
}

Boolean	AbstractPushButton_WinUI::GetLive () const
{
	return (AbstractPushButton::GetLive ());
}

void	AbstractPushButton_WinUI::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractPushButton::EffectiveLiveChanged (newLive, updateMode);
}

Boolean	AbstractPushButton_WinUI::GetEffectiveLive () const
{
	// only View overrides this, but C++ insists we disambiguate anyhow
	return (View::GetEffectiveLive ());
}





/*
 ********************************************************************************
 **************************** PushButton_MacUI_Portable *************************
 ********************************************************************************
 */
PushButton_MacUI_Portable::PushButton_MacUI_Portable (const String& label, ButtonController* controller) :
	AbstractPushButton_MacUI (controller),
	fLabel (label)
{
}

void	PushButton_MacUI_Portable::Draw (const Region& /*update*/)
{
	// should take GetEffectiveLive into account when drawing text
	DrawText (GetLabel (), 
		(GetSize () - Point (GetEffectiveFont ().GetFontHeight (), TextWidth (fLabel)))/2);
	DrawMacBorder (GetLocalExtent (), GetBorder ());
}

String	PushButton_MacUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	PushButton_MacUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}
		
Boolean	PushButton_MacUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if ((mouseInButton) or (phase != Tracker::eTrackRelease)) {
		Invert ();
	}
	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		Pressed ();
	}
	
	return (AbstractPushButton_MacUI::Track (phase, mouseInButton));
}

void	PushButton_MacUI_Portable::Flash_ ()
{
	Invert ();

#if		qSunCFront_OverloadingConfusionBug
	EventManager::Get ().WaitFor ((BigReal).15);
#else
	EventManager::Get ().WaitFor (.15);
#endif

	Invert ();
}










/*
 ********************************************************************************
 **************************** PushButton_MotifUI_Portable ***********************
 ********************************************************************************
 */
#if qXmToolkit && 0
// From  _XmPrimitiveTopShadowPixmapDefault (Widget widget, int offset, XrmValue *value)
// MAKE THIS PART OF GDI CONFIG???
static	Tile	mkTopShadowTile (osWidget* widget)
{
   XmPrimitiveWidget pw = (XmPrimitiveWidget) widget;

   if (pw->primitive.top_shadow_color == pw->core.background_pixel) {
	   return (XmGetPixmap (XtScreen (pw), "50_foreground", pw->primitive.top_shadow_color, pw->primitive.foreground));
   }
   else if (DefaultDepthOfScreen (XtScreen (widget)) == 1) {
	   return (XmGetPixmap (XtScreen (pw), "50_foreground", pw->primitive.top_shadow_color, pw->core.background_pixel));
   }
   AssertNotReached (); // notherwise    pixmap = XmUNSPECIFIED_PIXMAP;
}
#endif

PushButton_MotifUI_Portable::PushButton_MotifUI_Portable (const String& label, ButtonController* controller) :
	AbstractPushButton_MotifUI (controller),
	fLabel (label)
{
}

void	PushButton_MotifUI_Portable::Draw (const Region& /*update*/)
{
	DrawToggle (False);
}

void	PushButton_MotifUI_Portable::DrawToggle (Boolean on)
{
	Rect	extent = GetLocalExtent ();
	Color bg = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (extent, GetBorder (), bg, (on) ? Bordered::eIn : Bordered::eOut);

	if (on) {
		Color	selectColor;
		Color	topShadow;
		Color	bottomShadow;
		
		CalcColorSet (bg, selectColor, topShadow, bottomShadow);
		bg = selectColor;
	}

	Tile	selectTile	=	PalletManager::Get ().MakeTileFromColor (bg);
	Paint (Rectangle (), extent.InsetBy (GetBorder ()), selectTile);

	Font	f = GetEffectiveFont ();
	Coordinate	textV = (GetSize ().GetV () - f.GetFontHeight ())/2;

	if (GetEffectiveLive ()) {
		DrawText (fLabel, Point (textV, (GetSize ().GetH () - TextWidth (fLabel))/2));
	}
	else {
		// must draw text in gray
		DrawText (fLabel, Point (textV, (GetSize ().GetH () - TextWidth (fLabel))/2));
	}
}

Boolean	PushButton_MotifUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	DrawToggle (Boolean (mouseInButton and (phase != Tracker::eTrackRelease)));
	if ((phase == Tracker::eTrackRelease) and mouseInButton) {
		Pressed ();
	}
	return (AbstractPushButton_MotifUI::Track (phase, mouseInButton));
}

void	PushButton_MotifUI_Portable::Flash_ ()
{
	DrawToggle (True);
	EventManager::Get ().WaitFor (.15);
	DrawToggle (False);
}

String	PushButton_MotifUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	PushButton_MotifUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}





/*
 ********************************************************************************
 **************************** PushButton_WinUI_Portable ***********************
 ********************************************************************************
 */
#if qXmToolkit && 0
// From  _XmPrimitiveTopShadowPixmapDefault (Widget widget, int offset, XrmValue *value)
// MAKE THIS PART OF GDI CONFIG???
static	Tile	mkTopShadowTile (osWidget* widget)
{
   XmPrimitiveWidget pw = (XmPrimitiveWidget) widget;

   if (pw->primitive.top_shadow_color == pw->core.background_pixel) {
	   return (XmGetPixmap (XtScreen (pw), "50_foreground", pw->primitive.top_shadow_color, pw->primitive.foreground));
   }
   else if (DefaultDepthOfScreen (XtScreen (widget)) == 1) {
	   return (XmGetPixmap (XtScreen (pw), "50_foreground", pw->primitive.top_shadow_color, pw->core.background_pixel));
   }
   AssertNotReached (); // notherwise    pixmap = XmUNSPECIFIED_PIXMAP;
}
#endif

PushButton_WinUI_Portable::PushButton_WinUI_Portable (const String& label, ButtonController* controller) :
	AbstractPushButton_WinUI (controller),
	fLabel (label)
{
}

void	PushButton_WinUI_Portable::Draw (const Region& /*update*/)
{
	DrawToggle (False);
}

void	PushButton_WinUI_Portable::DrawToggle (Boolean on)
{
	Rect	extent = GetLocalExtent ();
	Color bg = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (extent, GetBorder (), bg, (on) ? Bordered::eIn : Bordered::eOut);

	if (on) {
		Color	selectColor;
		Color	topShadow;
		Color	bottomShadow;
		
		CalcColorSet (bg, selectColor, topShadow, bottomShadow);
		bg = selectColor;
	}

	Tile	selectTile	=	PalletManager::Get ().MakeTileFromColor (bg);
	Paint (Rectangle (), extent.InsetBy (GetBorder ()), selectTile);

	Font	f = GetEffectiveFont ();
	Coordinate	textV = (GetSize ().GetV () - f.GetFontHeight ())/2;

	if (GetEffectiveLive ()) {
		DrawText (fLabel, Point (textV, (GetSize ().GetH () - TextWidth (fLabel))/2));
	}
	else {
		// must draw text in gray
		DrawText (fLabel, Point (textV, (GetSize ().GetH () - TextWidth (fLabel))/2));
	}
}

Boolean	PushButton_WinUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	DrawToggle (Boolean (mouseInButton and (phase != Tracker::eTrackRelease)));
	if ((phase == Tracker::eTrackRelease) and mouseInButton) {
		Pressed ();
	}
	return (AbstractPushButton_WinUI::Track (phase, mouseInButton));
}

void	PushButton_WinUI_Portable::Flash_ ()
{
	DrawToggle (True);
	EventManager::Get ().WaitFor (.15);
	DrawToggle (False);
}

String	PushButton_WinUI_Portable::GetLabel_ () const
{
	return (fLabel);
}

void	PushButton_WinUI_Portable::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	fLabel = label;
	Refresh (updateMode);
}





#if		qMacToolkit
/*
 ********************************************************************************
 ***************************** PushButton_MacUI_Native **************************
 ********************************************************************************
 */

//q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
	class	MacOSPushButton : public MacOSControl {
	  public:
		MacOSPushButton (PushButton_MacUI_Native& thePB):
			MacOSControl (pushButProc),
			fPushButton (thePB)
			{
			}

			override	void	ActionProc (short partCode)
			{
				fPushButton.Tracking (Tracker::eTrackMove, Boolean (partCode == inButton));
			}
			
			override	Boolean	TrackPress (const MousePressInfo& mouseInfo)
			{
				Boolean aborted = fPushButton.Tracking (Tracker::eTrackPress, True);
				return (Boolean (aborted or MacOSControl::TrackPress (mouseInfo)));
			}
			
			override	void	DoneProc (short partCode)
			{
				Boolean aborted = fPushButton.Tracking (Tracker::eTrackRelease, Boolean (partCode == inButton));
				
				// Simulate a press in the button so our action proc gets called...
				if ((not aborted) and (partCode == inButton)) {
					fPushButton.Pressed ();
				}
			}
	  private:
		PushButton_MacUI_Native& fPushButton;
	};

PushButton_MacUI_Native::PushButton_MacUI_Native (const String& label, ButtonController* controller):
	AbstractPushButton_MacUI (controller),
	fMacOSControl (Nil)
{
	AddSubView (fMacOSControl = new MacOSPushButton (*this));
	SetLabel (label, eNoUpdate);
	fMacOSControl->SetHiliteState (GetEffectiveLive () ? 0 : 0xff, eNoUpdate);
}

PushButton_MacUI_Native::~PushButton_MacUI_Native ()
{
	RemoveSubView (fMacOSControl);
	delete fMacOSControl;
}

void	PushButton_MacUI_Native::Draw (const Region& /*update*/)
{
	if (GetBorder () >= Point (1, 1)) {
		DrawBorder_ (GetLocalExtent (), GetBorder () - Point (1, 1));
	}
}

void	PushButton_MacUI_Native::Layout ()
{
	AssertNotNil (fMacOSControl);
	
	Point	inset = GetBorder ();
	if (GetBorder () != kZeroPoint) {
		inset -= Point (1, 1);
	}
	
	fMacOSControl->SetOrigin (inset);
	fMacOSControl->SetSize (GetSize () - inset*2);
	AbstractPushButton_MacUI::Layout ();
}

void	PushButton_MacUI_Native::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractPushButton_MacUI::EffectiveLiveChanged (newLive, eNoUpdate);
	AssertNotNil (fMacOSControl);
	fMacOSControl->SetHiliteState (newLive ? 0 : 0xff, updateMode);
}

String	PushButton_MacUI_Native::GetLabel_ () const
{
	AssertNotNil (fMacOSControl);
	return (fMacOSControl->GetTitle ());
}

void	PushButton_MacUI_Native::SetLabel_ (const String& label, Panel::UpdateMode updateMode)
{
	AssertNotNil (fMacOSControl);
	fMacOSControl->SetTitle (label, updateMode);
}

osControlRecord**	PushButton_MacUI_Native::GetOSRepresentation () const
{
	AssertNotNil (fMacOSControl);
	return (fMacOSControl->GetOSRepresentation ());
}

void	PushButton_MacUI_Native::Flash_ ()
{
	fMacOSControl->SetHiliteState ((not GetEffectiveLive ()) ? 0xff : 1, eImmediateUpdate);	// pushbuttons don't invert if value is 1, (Apple bug if you ask me)

#if		qSunCFront_OverloadingConfusionBug
	EventManager::Get ().WaitFor ((BigReal).15);
#else
	EventManager::Get ().WaitFor (.15);
#endif

	fMacOSControl->SetHiliteState ((not GetEffectiveLive ()) ? 0xff : 0, eImmediateUpdate);	// pushbuttons don't invert if value is 1, (Apple bug if you ask me)
}
#endif	/*qMacToolkit*/










#if		qXmToolkit
/*
 ********************************************************************************
 *************************** PushButton_MotifUI_Native **************************
 ********************************************************************************
 */

PushButton_MotifUI_Native::PushButton_MotifUI_Native (const String& label, ButtonController* controller):
	AbstractPushButton_MotifUI (controller),
    fLabel (label),
	fOSControl (Nil),
	fGotClick (False)
{
	class	MyOSPushButton : public MotifOSControl {
		public:
			MyOSPushButton (XtCallbackProc callBack, XtPointer clientData):
				MotifOSControl (),
				fCallBack (callBack),
				fClientData (clientData)
			{
			}
			
			override	osWidget*	DoBuild_ (osWidget* parent)
			{
				osWidget* w = ::XmCreatePushButton (parent, "PushButton", Nil, 0);
				::XtAddCallback (w, XmNactivateCallback, fCallBack, fClientData);
				return (w);
			}
			
			XtCallbackProc	fCallBack;
			XtPointer		fClientData;
	};
	AddSubView (fOSControl = new MyOSPushButton ((XtCallbackProc)ActivateCallBack, (XtPointer)this));
	SetLabel_ (label, eNoUpdate);
	SetBorder (2, 2, eNoUpdate);
	SetMargin (2, 2, eNoUpdate);
}

PushButton_MotifUI_Native::~PushButton_MotifUI_Native ()
{
	RemoveSubView (fOSControl);
	delete fOSControl;
}

Boolean	PushButton_MotifUI_Native::TrackPress (const MousePressInfo& mouseInfo)
{
	Require (GetEffectiveLive ());
	Assert (fGotClick == False);
	AssertNotNil (fOSControl);
	
	Boolean aborted = Tracking (Tracker::eTrackPress, True);
	if (not aborted) {
		Boolean	result	=	fOSControl->TrackPress (MousePressInfo (mouseInfo, fOSControl->EnclosureToLocal (mouseInfo.fPressAt)));
	
		aborted = Tracking (Tracker::eTrackRelease, fGotClick);		
		if ((not aborted) and (fGotClick)) {
			Pressed ();
		}
		fGotClick = False;
	}

	return (True);
}

void	PushButton_MotifUI_Native::ActivateCallBack (osWidget* /*w*/, char* client_data, char* /*call_data*/)
{
	PushButton_MotifUI_Native*	mpb	=	((PushButton_MotifUI_Native*)client_data);
	AssertNotNil (mpb);
	Assert (not mpb->fGotClick);
	mpb->fGotClick = True;
}

void	PushButton_MotifUI_Native::SetBorder_ (const Point& border, UpdateMode updateMode)
{
	AbstractPushButton_MotifUI::SetBorder_ (border, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetDimensionResourceValue (XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
	Refresh (updateMode);
}

void	PushButton_MotifUI_Native::SetMargin_ (const Point& margin, UpdateMode updateMode)
{
	AbstractPushButton_MotifUI::SetMargin_ (margin, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetDimensionResourceValue (XmNmarginHeight, (unsigned short)GetMarginHeight ());
    fOSControl->SetDimensionResourceValue (XmNmarginWidth, (unsigned short)GetMarginWidth ());
	Refresh (updateMode);
}

void	PushButton_MotifUI_Native::Layout ()
{
	AssertNotNil (fOSControl);
	Assert (fOSControl->GetOrigin () == kZeroPoint);
	fOSControl->SetSize (GetSize ());
	AbstractPushButton_MotifUI::Layout ();
}
		
String	PushButton_MotifUI_Native::GetLabel_ () const
{
	AssertNotNil (fOSControl);
	return (fLabel);
}

void	PushButton_MotifUI_Native::SetLabel_ (const String& label, UpdateMode updateMode)
{
	fLabel = label;
	AssertNotNil (fOSControl);
	fOSControl->SetStringResourceValue (XmNlabelString, label);
}

void	PushButton_MotifUI_Native::EffectiveFocusChanged (Boolean newFocused, UpdateMode updateMode)
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
	AbstractPushButton_MotifUI::EffectiveFocusChanged (newFocused, updateMode);
}

void	PushButton_MotifUI_Native::EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode)
{
	AbstractPushButton_MotifUI::EffectiveLiveChanged (newLive, eNoUpdate);
	AssertNotNil (fOSControl);
	fOSControl->SetBooleanResourceValue (XmNsensitive, newLive);
}

osWidget*	PushButton_MotifUI_Native::GetOSRepresentation () const
{
	AssertNotNil (fOSControl);
	return (fOSControl->GetOSRepresentation ());
}

void	PushButton_MotifUI_Native::Flash_ ()
{
	fOSControl->SetBooleanResourceValue (XmNset, True);

#if		qSunCFront_OverloadingConfusionBug
	EventManager::Get ().WaitFor ((BigReal).15);
#else
	EventManager::Get ().WaitFor (.15);
#endif

	fOSControl->SetBooleanResourceValue (XmNset, False);
}

void	PushButton_MotifUI_Native::Realize (osWidget* parent)
{
	// Dont call inherited version since we call our childs realize directly...
	RequireNotNil (parent);
	AssertNotNil (fOSControl);

#if 1
	View::Realize (parent);
#else
	fOSControl->Realize (parent);
#endif

	fOSControl->SetBooleanResourceValue (XmNsensitive, GetEffectiveLive ());
	fOSControl->SetBooleanResourceValue (XmNset, False);
	fOSControl->SetStringResourceValue (XmNlabelString, GetLabel ());
	fOSControl->SetDimensionResourceValue 	(XmNmarginHeight, 	(unsigned short)GetMarginHeight ());
    fOSControl->SetDimensionResourceValue 	(XmNmarginWidth, 	(unsigned short)GetMarginWidth ());
	fOSControl->SetDimensionResourceValue 	(XmNshadowThickness, (unsigned short)Min (GetBorderHeight (), GetBorderWidth ()));
	EnsureNotNil (GetOSRepresentation ());
}

void	PushButton_MotifUI_Native::UnRealize ()
{
#if 1
	View::UnRealize ();
#else
	AssertNotNil (fOSControl);
	fOSControl->UnRealize ();
#endif
}

#endif	/*qXmToolkit*/










/*
 ********************************************************************************
 ******************************** PushButton_MacUI ******************************
 ********************************************************************************
 */
PushButton_MacUI::PushButton_MacUI ():
// see comment in header
#if		qNative && qMacToolkit && 0
	PushButton_MacUI_Native (kEmptyString, Nil)
#else
	PushButton_MacUI_Portable (kEmptyString, Nil)
#endif
{
}

PushButton_MacUI::PushButton_MacUI (const String& label):
// see comment in header
#if		qNative && qMacToolkit && 0
	PushButton_MacUI_Native (label, Nil)
#else
	PushButton_MacUI_Portable (label, Nil)
#endif
{
}

PushButton_MacUI::PushButton_MacUI (const String& label, ButtonController* controller):
// see comment in header
#if		qNative && qMacToolkit && 0
	PushButton_MacUI_Native (label, controller)
#else
	PushButton_MacUI_Portable (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
PushButton_MacUI::~PushButton_MacUI () {}
#endif










/*
 ********************************************************************************
 ******************************* PushButton_MotifUI *****************************
 ********************************************************************************
 */
PushButton_MotifUI::PushButton_MotifUI ():
#if		qNative && qXmToolkit
	PushButton_MotifUI_Native (kEmptyString, Nil)
#else
	PushButton_MotifUI_Portable (kEmptyString, Nil)
#endif
{
}

PushButton_MotifUI::PushButton_MotifUI (const String& label):
#if		qNative && qXmToolkit
	PushButton_MotifUI_Native (label, Nil)
#else
	PushButton_MotifUI_Portable (label, Nil)
#endif
{
}

PushButton_MotifUI::PushButton_MotifUI (const String& label, ButtonController* controller):
#if		qNative && qXmToolkit
	PushButton_MotifUI_Native (label, controller)
#else
	PushButton_MotifUI_Portable (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
PushButton_MotifUI::~PushButton_MotifUI () {}
#endif




/*
 ********************************************************************************
 ******************************* PushButton_WinUI *****************************
 ********************************************************************************
 */
PushButton_WinUI::PushButton_WinUI ():
	PushButton_WinUI_Portable (kEmptyString, Nil)
{
}

PushButton_WinUI::PushButton_WinUI (const String& label):
	PushButton_WinUI_Portable (label, Nil)
{
}

PushButton_WinUI::PushButton_WinUI (const String& label, ButtonController* controller):
	PushButton_WinUI_Portable (label, controller)
{
}

#if		qSoleInlineVirtualsNotStripped
PushButton_WinUI::~PushButton_WinUI () {}
#endif






/*
 ********************************************************************************
 ************************************* PushButton *******************************
 ********************************************************************************
 */
PushButton::PushButton () :
#if		qMacUI
	PushButton_MacUI (kEmptyString, Nil)
#elif	qMotifUI
	PushButton_MotifUI (kEmptyString, Nil)
#elif	qWinUI
	PushButton_WinUI (kEmptyString, Nil)
#endif
{
}

PushButton::PushButton (const String& label) :
#if		qMacUI
	PushButton_MacUI (label, Nil)
#elif	qMotifUI
	PushButton_MotifUI (label, Nil)
#elif	qWinUI
	PushButton_WinUI (label, Nil)
#endif
{
}

PushButton::PushButton (const String& label, ButtonController* controller) :
#if		qMacUI
	PushButton_MacUI (label, controller)
#elif	qMotifUI
	PushButton_MotifUI (label, controller)
#elif	qWinUI
	PushButton_WinUI (label, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
PushButton::~PushButton () {}
#endif






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



