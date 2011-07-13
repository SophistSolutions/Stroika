/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PopUpMenu.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: PopUpMenu.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/03  02:10:50  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:26:09  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_. +++.
 *
 *		Revision 1.22  92/05/18  16:40:06  16:40:06  lewis (Lewis Pringle)
 *		Make a few private methods const.
 *		
 *		Revision 1.21  92/04/30  13:24:36  13:24:36  sterling (Sterling Wight)
 *		various bug fixes
 *		
 *		Revision 1.20  92/04/30  03:45:43  03:45:43  lewis (Lewis Pringle)
 *		Comment out suspend/resume of idletasks - prevents update event processing currently.
 *		This whole area is a mess. We must debug but low priority now.
 *		
 *		Revision 1.19  92/04/24  09:07:06  09:07:06  lewis (Lewis Pringle)
 *		Worked on motif trackpress(). Tried XmNentryCallback, and it didnt seem to work - not sure
 *		why. Also, turned back on disabling idletasks, and command processing. Also, did more hacking
 *		with current seelction.
 *		
 *		Revision 1.18  92/04/20  14:33:47  14:33:47  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		
 *		Revision 1.17  92/04/17  19:53:03  19:53:03  lewis (Lewis Pringle)
 *		Hack motif popup menus - quick for release.
 *		
 *		Revision 1.16  92/04/13  00:57:15  00:57:15  lewis (Lewis Pringle)
 *		Get rid of mac GUI specific stuff in TrackPress(), inverting, etc.
 *		Also, make the OptionMenu, instead of Label. And set the XmNsubWinId resource for the option menu
 *		to the widget of the menu.
 *		
 *		Revision 1.15  92/04/08  15:30:01  15:30:01  lewis (Lewis Pringle)
 *		ported to mac.
 *		
 *		Revision 1.13  92/04/02  13:07:30  13:07:30  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.12  92/03/26  18:36:54  18:36:54  lewis (Lewis Pringle)
 *		Add override of EffectiveFontChanged for popupmenutitiles, to inval layout, etc.
 *		
 *		Revision 1.11  1992/03/26  09:58:47  lewis
 *		Get rid of oldLive arg to EffectiveLiveChanged.
 *
 *		Revision 1.9  1992/03/10  00:04:21  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.8  1992/02/28  22:32:55  lewis
 *		Cleanup realize/unrealize code - reflect changes in View definitions.
 *
 *		Revision 1.7  1992/02/28  16:27:48  lewis
 *		fixup to compile under unix.
 *
 *		Revision 1.6  1992/02/27  23:44:45  lewis
 *		PopUpMenuTitle_MacUI_Portable no longer inherits from EnableView since it
 *		gets EnableItem, and View from base classes.
 *		Get rid of fNativeMenu.
 *
 *		Revision 1.4  1992/02/16  16:38:09  lewis
 *		Minor cleanups to make it compile on mac.
 *
 *		Revision 1.3  1992/02/15  07:01:27  sterling
 *		use EffectiveLiveChanged
 *
 *		Revision 1.2  1992/02/15  06:48:33  lewis
 *		Added class PopUpMenuTitle instead of #defines, and tried to rationalize their seconds args. Decided
 *		what we did for mac was better (sterl says so), and later we must fix usage of second arg in
 *		motif version.
 *
 *		Revision 1.1  1992/02/15  00:48:30  lewis
 *		Initial revision
 *
 *
 *
 *
 *		********** From PopUpMenuControl.cc- now PopUpMenu.cc ***********
 *
 *		Revision 1.15  1992/02/12  07:11:09  lewis
 *		Use Menu instead of StandardMenu.
 *
 *		Revision 1.14  1992/02/04  17:08:05  lewis
 *		Use StandardMenu instead of MenU_MacUI_Standard since the later class has disappeared.
 *
 *		Revision 1.13  1992/01/31  16:53:10  sterling
 *		temp hack to SetUptheMenus in a click
 *		chould do this for all MenuOwners
 *
 *		Revision 1.10  1992/01/08  06:28:50  lewis
 *		Sterl- merged his changes.
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacToolkit
#elif	qXmToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#if		qXmToolkit
#include	<Xm/RowColumn.h>
#include	<Xm/CascadeBG.h>
#include	<Xm/Label.h>
#endif
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"Application.hh"
#include	"DeskTop.hh"
#include	"OSControls.hh"
#include	"Menu.hh"
#include	"PullDownMenu.hh"

#include	"PopUpMenu.hh"	







/*
 ********************************************************************************
 ******************************* AbstractPopUpMenuTitle *************************
 ********************************************************************************
 */

AbstractPopUpMenuTitle::AbstractPopUpMenuTitle (const String& title, Menu* menu):
	MenuTitle (title, menu),
	fCurrentItem (Nil)
{
	RequireNotNil (menu);
}

MenuItem*	AbstractPopUpMenuTitle::GetCurrentItem () const
{
	return (fCurrentItem);
}

void	AbstractPopUpMenuTitle::SetCurrentItem (MenuItem* item)
{
	if (GetCurrentItem () != item) {
		SetCurrentItem_ (item);
	}
	Ensure (GetCurrentItem () == item);
}

void	AbstractPopUpMenuTitle::SetCurrentItem_ (MenuItem* item)
{
	fCurrentItem = item;
}

void	AbstractPopUpMenuTitle::HandleMenuSelection (const MenuItem& item)
{
	MenuOwner::HandleMenuSelection (item);
	if (&item != GetCurrentItem ()) {
		SetCurrentItem ((MenuItem*)&item);
	}
	Ensure (&item == GetCurrentItem ());
}

Boolean	AbstractPopUpMenuTitle::GetLive () const
{
	return (Boolean (MenuTitle::GetLive () and (GetParentView () != Nil)));
}






#if		qMacToolkit

/*
 ********************************************************************************
 ************************ PopUpMenuTitle_MacUI_Portable ************************
 ********************************************************************************
 */
PopUpMenuTitle_MacUI_Portable::PopUpMenuTitle_MacUI_Portable (const String& title, ButtonController* controller):
	AbstractPopUpMenuTitle (title, new Menu ()),
	fAutoResize (True)
{
	SetController (controller);
	SetFont (&kSystemFont);
	SetBackground (&kWhiteTile);
}

PopUpMenuTitle_MacUI_Portable::~PopUpMenuTitle_MacUI_Portable ()
{
}

void	PopUpMenuTitle_MacUI_Portable::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	AbstractPopUpMenuTitle::EffectiveLiveChanged (newLive, updateMode);
	if (newLive) {
		InstallMenu ();
	}
	else {
		DeinstallMenu ();
	}
}

void	PopUpMenuTitle_MacUI_Portable::EffectiveFontChanged (const Font& newFont, Panel::UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */
	AbstractPopUpMenuTitle::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}

	Refresh (updateMode);					// cuz our display may change
}

Point	PopUpMenuTitle_MacUI_Portable::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	RequireNotNil (GetMenu ());
	const Font&	f = GetEffectiveFont ();
	Coordinate	newWidth = TextWidth (GetTitle ());
	if (GetCurrentItem () == Nil) {
		newWidth += 50;			// some reasonable default
	}
	else {
		newWidth += TextWidth (GetCurrentItemText ()) + GetItemOffset () +
			2 +		
			5;		// extra whitespace so name not pressed up against drop shadow...
	}
	Coordinate	newHeight	=	f.GetFontHeight () + 3;
	return (Point (newHeight, newWidth));
}

Boolean	PopUpMenuTitle_MacUI_Portable::TrackPress (const MousePressInfo& /*mouseInfo*/)
{
	Require (GetLive ());

	Rect			labelRect					=	GetLocalExtent ();
	Rect			menuRect					=	CalcMenuRect ();

Application::Get ().SetupTheMenus ();	// find a way to do this for all menutitles
	labelRect.SetSize (Point (labelRect.GetHeight (), TextWidth (GetTitle ())));
	if (GetCurrentItem () != Nil) {
		labelRect.SetSize (Point (labelRect.GetHeight (), labelRect.GetWidth () + GetItemOffset ()));
	}
	Invert (Rectangle (), labelRect);
	MenuItem*	item = GetCurrentItem ();
	(void) DoMenuSelection (LocalToAncestor (menuRect.GetOrigin (), &DeskTop::Get ()), item);
	Invert (Rectangle (), labelRect);

	return (True);
}

Boolean	PopUpMenuTitle_MacUI_Portable::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)
{
	if (GetMenu () != Nil) {
		return (MacDoMenuSelection (GetMenu (), startPt, selected));		
	}
	return (False);
}

void	PopUpMenuTitle_MacUI_Portable::Draw (const Region& /*update*/)
{
	DrawText (GetTitle (), kZeroPoint);
	if (GetCurrentItem () == Nil) {
// look in HIG guidelines for what to do
//		OutLine (Rectangle (), GetLocalExtent ());
	}

	DrawPopUpBox ();
	if (not GetLive ()) {
		Gray ();
	}
}

Boolean	PopUpMenuTitle_MacUI_Portable::GetAutoResize () const
{
	return (fAutoResize);
}

void	PopUpMenuTitle_MacUI_Portable::SetAutoResize (Boolean autoResize)
{
	fAutoResize = autoResize;
}

Coordinate	PopUpMenuTitle_MacUI_Portable::GetItemOffset () const
{
	if (GetTitle () == kEmptyString) {
		return (0);
	}
	else {
		return (2);	// leave some room after the text
	}
}

Rect	PopUpMenuTitle_MacUI_Portable::CalcMenuRect () const
{
	Rect		r			=	GetLocalExtent ();
	Coordinate	itemOffset	=	0;
	if (GetTitle () != kEmptyString) {
		itemOffset	=	TextWidth (GetTitle ()) + GetItemOffset ();
	}
	r.InsetBy (Point (1, 1));
	r.SetTLBR (r.GetTop (), r.GetLeft () + itemOffset, r.GetBottom (), r.GetRight ());
	return (r);
}

void	PopUpMenuTitle_MacUI_Portable::DrawPopUpBox ()
{
	Rect	menuRect	=	CalcMenuRect ();
	if (menuRect.GetSize () > kZeroPoint) {
		menuRect.InsetBy (Point (-1, -1));
		const	Coordinate	kRightSlop	=	1;
		const	Coordinate	kLeftSlop	=	5;
		const	Coordinate	kBotSlop	=	6;

		String		theItem	=	GetCurrentItemText ();
		Coordinate	wid		=	menuRect.GetWidth () - (kLeftSlop - kRightSlop);

		// shorten the item to fit...
		for (Coordinate newWid = TextWidth (theItem); (newWid > wid) and (theItem.GetLength () > 0);
			newWid = TextWidth (theItem)) {
			theItem.SetLength (theItem.GetLength () - 1);
			if (theItem.GetLength () != 0) {
				theItem.SetCharAt ('É', theItem.GetLength ());
			}
		}

		// Draw the text.
		const Font&	f	=	GetEffectiveFont ();
		DrawText (theItem, Point (menuRect.GetTop (), menuRect.GetLeft () + kLeftSlop));

		/*
		 * Draw the drop-shaddow
		 */
		menuRect = 	Rect (menuRect.GetOrigin (), menuRect.GetSize () - Point (1, 1));
		OutLine (Rectangle (), menuRect);

		OutLine (Line (menuRect.GetBotLeft () + Point (0, 3), menuRect.GetBotRight ()));
		OutLine (Line (menuRect.GetBotRight (), menuRect.GetTopRight () + Point (3, 0)));
	}
}

void	PopUpMenuTitle_MacUI_Portable::HandleMenuSelection (const MenuItem& item)
{
	if (GetCurrentItem () != &item) {
		AbstractPopUpMenuTitle::HandleMenuSelection (item);
		if (GetAutoResize ()) {
			SetSize (CalcDefaultSize ());
		}
		Refresh ();
		if (GetController () != Nil) {
			GetController ()->ButtonPressed (this);
		}
	}
}

String		PopUpMenuTitle_MacUI_Portable::GetCurrentItemText () const
{
	if (GetCurrentItem () == Nil) {
		return (kEmptyString);
	}
	return (GetCurrentItem ()->GetName ());
}



#elif	qXmToolkit



/*
 ********************************************************************************
 ***************************** PopUpMenuTitle_MotifUI **************************
 ********************************************************************************
 */
PopUpMenuTitle_MotifUI::PopUpMenuTitle_MotifUI (const String& title, Menu* menu):
	AbstractPopUpMenuTitle (title, (menu==Nil)? new Menu (): menu),
	FocusItem (),
	fOSControl (Nil)
{
	class	MyLabelWidget : public MotifOSControl {
		public:
			MyLabelWidget () :
				MotifOSControl ()
			{
			}

			override	osWidget*	DoBuild_ (osWidget* parent)
			{
				RequireNotNil (parent);
				osWidget*	w	=	::XmCreateOptionMenu (parent, "OptionMenu", Nil, 0);
				return (w);
			}	
	};
	AddSubView (fOSControl = new MyLabelWidget ());
}

PopUpMenuTitle_MotifUI::~PopUpMenuTitle_MotifUI ()
{
	DeinstallMenu ();
}

void	PopUpMenuTitle_MotifUI::Layout ()
{
	AssertNotNil (fOSControl);
	Assert (fOSControl->GetOrigin () == kZeroPoint);
	fOSControl->SetSize (GetSize ());
	AbstractPopUpMenuTitle::Layout ();
}

void	PopUpMenuTitle_MotifUI::SetTitle_ (const String& title)
{
	AssertNotNil (fOSControl);
	fOSControl->SetStringResourceValue (XmNlabelString, title);
	AbstractPopUpMenuTitle::SetTitle_ (title);
}

void	PopUpMenuTitle_MotifUI::EffectiveLiveChanged (Boolean newLive, UpdateMode update)
{
	AbstractPopUpMenuTitle::EffectiveLiveChanged (newLive, update);
	AssertNotNil (fOSControl);
	fOSControl->SetBooleanResourceValue (XmNsensitive, newLive);

	if (newLive) {
		InstallMenu ();
	}
	else {
		DeinstallMenu ();
	}
}

void	PopUpMenuTitle_MotifUI::EffectiveFontChanged (const Font& newFont, UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */

	AbstractPopUpMenuTitle::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}

	Refresh (updateMode);					// cuz our display may change
}

// hack to talk to menu stuff...
Boolean gDoPopup = False;
#if 0
Arg gPopupArgs [1];
int gPopupArgCount = 0;

static void sEntryCallBackProc (osWidget* w, void* client_data, void* call_data)
{
//gDebugStream << "GOT CALLBACK FOR XmNentryCallback" << newline;
}
#endif

void	PopUpMenuTitle_MotifUI::Realize (osWidget* parent)
{
	AssertNotNil (fOSControl);
	AssertNotNil (GetMenu ());

	fOSControl->Realize (parent);
	fOSControl->SetBooleanResourceValue (XmNsensitive, GetLive ());

	/*
	 * Although one might intuitively expect the menu to have as its parent, the button that it pops down from,
	 * the Motif Programmers Reference (hardback) 1-378:
	 * ...
	 *	o	If the Pulldown MenuPane is to be pulled down from an OptionMenu, its parent must be the same as the
	 * 		OptionMenu parent.
	 * ...
	 */
Assert (not gDoPopup);
gDoPopup = True;
//XtSetArg (gPopupArgs[0], XmNentryCallback, sEntryCallBackProc);
//XtSetArg (gPopupArgs[0], XmNentryCallback, Nil);
//gPopupArgCount = 1;

	GetMenu ()->Realize (parent);
//::XtAddCallback (GetMenu ()->GetOSRepresentation (), XmNentryCallback, (XtCallbackProc)sEntryCallBackProc, (XtPointer)this);

gDoPopup = False;

#if 0
	Arg	args [1];
	XtSetArg (args[0], XmNsubMenuId, GetMenu ()->GetOSRepresentation ());
	::XtSetValues (fOSControl->GetOSRepresentation (), args, 1);
#endif

	SetTitle_ (GetTitle ());	// use protected interface so always called (no comparision with existing title)
}

void	PopUpMenuTitle_MotifUI::UnRealize ()
{
	AssertNotNil (GetMenu ());
	AssertNotNil (fOSControl);
    GetMenu ()->UnRealize ();
	fOSControl->UnRealize ();
}


static	Boolean	sButtonPressed = False;	
static	void	HandleButtonUpDuringTrackPressWait (osWidget* /*w*/, char* /*clientData*/, osEventRecord* /*e*/,
													osBoolean* /*continue_to_dispatch*/)
{
	Assert (sButtonPressed);
	sButtonPressed = False;
}


Boolean	PopUpMenuTitle_MotifUI::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)
{
//gDebugStream << "Entering trackpress for PopUpMenuTitle_MotifUI::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)" << newline;
	osEventRecord	er;

	AssertNotNil (GetMenu ());

	// cloned from OSControl
	Point	localPoint	=	AncestorToLocal (startPt, &DeskTop::Get ());

	EventManager::Get ().GetCurrentEvent (er);
	er.xbutton.type = ButtonPress;
	er.xbutton.display = XtDisplay (GetMenu ()->GetOSRepresentation ());
	er.xbutton.window = XtWindow (GetMenu ()->GetOSRepresentation ());		// IMPLIES WIDGET - NOT GADGET - CLEANUP FOR GADGET
	AssertNotNil (er.xbutton.window);
	er.xbutton.root = DefaultRootWindow (er.xbutton.display);
	er.xbutton.x = (int)localPoint.GetH ();
	er.xbutton.y = (int)localPoint.GetV ();
	er.xbutton.x_root = (int)startPt.GetH ();
	er.xbutton.y_root = (int)startPt.GetV ();
	er.xbutton.same_screen = True;
	er.xbutton.button = Button1;

    RequireNotNil (GetMenu ());
	::XmMenuPosition (GetMenu ()->GetOSRepresentation (), (XButtonPressedEvent*) &er);
	::XtManageChild (GetMenu ()->GetOSRepresentation ());

Require (GetMenuItemSelected () == Nil);	// to start sb nil - ONLY WHILE DEBUGGING - LATER SET TO OLD VALUE???

//Application::Get ().SuspendIdleTasks ();
Application::Get ().SuspendCommandProcessing ();
//GoReal ();
::XtDispatchEvent (&er);

/*
 * The below hack of installing an eventhandler and then removing it after a while loop is to provide
 * the semantics that when you call TrackPress() it doesn't return until the user is done with the event
 * (ie has finished tracking). This is not the greatest way of solving the problem. In particular, it seems
 * important to put our handler at the HEAD of the list (thats why we dont call XtAddEventHandler) so that
 * we dont get called til too late.
 *
 * The whole business is necessary, for example, in textedit, you want to inval the menus when the tracking is
 * done so they get updated properly (thats when we first noticed the bug).
 */
::XtInsertEventHandler (GetMenu ()->GetOSRepresentation (), ButtonReleaseMask, FALSE, HandleButtonUpDuringTrackPressWait, Nil, XtListHead);
sButtonPressed = True;
Exception* caught = Nil;
Try {
	while (sButtonPressed) {
		EventManager::Get ().RunOnce ();
	}
}
Catch () {
	caught = _this_catch_;
}
//GoUnReal ();
//Application::Get ().ResumeIdleTasks ();
Application::Get ().ResumeCommandProcessing ();
//gDebugStream << "done with while loop and removing event handler" << newline;
::XtRemoveEventHandler (GetMenu ()->GetOSRepresentation (), ButtonReleaseMask, FALSE, HandleButtonUpDuringTrackPressWait, Nil);
if (caught != Nil) {
	caught->Raise ();
}

Boolean	result	=	Boolean (GetMenuItemSelected () != Nil);

	if (GetMenuItemSelected () != Nil) {
		selected = GetMenuItemSelected ();
	}

	SetMenuItemSelected (Nil);
	::XtUnmanageChild (GetMenu ()->GetOSRepresentation ());

	return (result);
}

Point	PopUpMenuTitle_MotifUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	RequireNotNil (GetMenu ());
	const Font&	f = GetEffectiveFont ();
	Coordinate	newWidth = TextWidth (GetTitle ());
	if (GetCurrentItem () == Nil) {
		newWidth += 50;			// some reasonable default
	}
	else {
		newWidth += TextWidth (GetCurrentItemText ()) + GetItemOffset () +
			2 +		
			5;		// extra whitespace so name not pressed up against drop shadow...
	}
	Coordinate	newHeight	=	f.GetFontHeight () + 3;
	return (Point (newHeight, newWidth));
}

Boolean	PopUpMenuTitle_MotifUI::TrackPress (const MousePressInfo& /*mouseInfo*/)
{
//ngDebugStream << "entering popupmenuittle_MotifUI:track;ress" << newline;
	Require (GetLive ());

Application::Get ().SetupTheMenus ();
	Rect menuRect 	=	CalcMenuRect ();

	MenuItem*	item = GetCurrentItem ();
	(void) DoMenuSelection (LocalToAncestor (menuRect.GetOrigin (), &DeskTop::Get ()), item);

//gDebugStream << "exiting popupmenuittle_MotifUI:track;ress" << newline;
	return (True);
}

Rect	PopUpMenuTitle_MotifUI::CalcMenuRect () const
{
	Rect		r			=	GetLocalExtent ();
	Coordinate	itemOffset	=	0;
	if (GetTitle () != kEmptyString) {
		itemOffset	=	TextWidth (GetTitle ()) + GetItemOffset ();
	}
	r.InsetBy (Point (1, 1));
	r.SetTLBR (r.GetTop (), r.GetLeft () + itemOffset, r.GetBottom (), r.GetRight ());
	return (r);
}

String		PopUpMenuTitle_MotifUI::GetCurrentItemText () const
{
	if (GetCurrentItem () == Nil) {
		return (kEmptyString);
	}
	return (GetCurrentItem ()->GetName ());
}

Coordinate	PopUpMenuTitle_MotifUI::GetItemOffset () const
{
	if (GetTitle () == kEmptyString) {
		return (0);
	}
	else {
		return (2);	// leave some room after the text
	}
}

#endif    /*Toolkit*/








/*
 ********************************************************************************
 ********************************* PopUpMenuTitle *******************************
 ********************************************************************************
 */

PopUpMenuTitle::PopUpMenuTitle (const String& title, ButtonController* buttonController):
#if		qMacToolkit
	PopUpMenuTitle_MacUI_Portable (title, Nil)
#elif	qXmToolkit
	PopUpMenuTitle_MotifUI (title, Nil)
#endif	/*Toolkit*/
{
	SetController (buttonController);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
