/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuBar.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuBar.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/02  04:53:46  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  08:12:45  sterling
 *		Rename CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.25  1992/05/18  16:21:09  lewis
 *		Added forgotten return statement at end of MenuBar_MacUI_Native::TrackPress.
 *
 *		Revision 1.24  92/05/11  20:10:15  20:10:15  lewis (Lewis Pringle)
 *		Made GetMenu_ () const.
 *		
 *		Revision 1.23  92/05/01  01:36:29  01:36:29  lewis (Lewis Pringle)
 *		Comment out Application::Get ().SuspendCommandProcessing (); for now - fix later...
 *		
 *		Revision 1.22  92/04/30  03:44:22  03:44:22  lewis (Lewis Pringle)
 *		Comment out suspend/resume of idletasks - prevents update event processing currently.
 *		This whole area is a mess. We must debug but low priority now.
 *		
 *		Revision 1.20  92/04/24  09:01:17  09:01:17  lewis (Lewis Pringle)
 *		Try to support help - attempt failed - for motif GUI - see why it didnt work.
 *		Also, worked on elaborate DoMenuSelection_ routine for motif - cloned and modified from
 *		MotifOSControls.
 *		
 *		Revision 1.19  92/04/20  14:25:52  14:25:52  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *		And got rid of some unneeded motif includes.
 *		
 *		Revision 1.18  92/04/14  07:12:17  07:12:17  lewis (Lewis Pringle)
 *		Override GetWidget for Motif MenuBar to return widget of owned oscontrol.
 *		Get rid of XtConfigureWidget call in Layout override for MotifMenuBar, since it was unnessary.
 *		
 *		Revision 1.17  92/04/07  15:58:17  15:58:17  sterling (Sterling Wight)
 *		fixed Calcdefaultsize for Motif
 *		
 *		Revision 1.16  92/04/06  15:23:20  15:23:20  sterling (Sterling Wight)
 *		CalcDefaultSize for Mac GUI
 *		
 *		Revision 1.15  92/04/02  13:07:25  13:07:25  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.13  1992/03/19  16:50:49  lewis
 *		Got rid of MenuBar_MacUI_Native::Draw (const Region& update) - just a hack and not needed - instead do
 *		draws in MenuTitles.
 *
 *		Revision 1.12  1992/03/16  17:23:33  sterling
 *		commented out fill of gray
 *
 *		Revision 1.11  1992/03/16  17:13:55  lewis
 *		Got rid of SetUseMagicView stuff for motif menubar - irrelevant. Add require GetLength=0 in dtor
 *		for menubars. CalcDefaultsize for Motifmenubar assures at least 34 tall - hack til I understand better.
 *
 *		Revision 1.10  1992/03/13  16:05:36  lewis
 *		Rewrite motif menubar code to use motifoscontrols. This is so that when we build one of these
 *		outside of a XmMainWindowArea, it still can behave properly. We no longer build one of those
 *		in Application.
 *
 *		Revision 1.9  1992/03/10  00:06:47  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.8  1992/03/06  02:33:23  lewis
 *		Had to add DeleteMenuTitle () in reaction to sterls change to View - not deleting subviews -
 *		probably for the best, but impact of change needs more thought with respect to menubars (and related classes).
 *
 *		Revision 1.7  1992/03/05  22:58:03  lewis
 *		Add quickie (wrong) definitions of CalcDefaultSize () for mac and motif mbars.
 *
 *		Revision 1.6  1992/03/05  18:40:30  lewis
 *		Add layout, draw, and a notion of being REALLY installed in the menubar (just a prototype
 *		design - needs work). Killed MenuBar_MacUI_Portable - it didnt work, and needed a lot
 *		more thought.
 *
 *		Revision 1.5  1992/02/29  15:52:37  lewis
 *		Had to get rid of override of dtor and call to Assert (GetLength () == 0) since that indirectly
 *		called GetMenus () - a pure virtual function.
 *
 *		Revision 1.4  1992/02/28  22:24:26  lewis
 *		Add AbstractMenuBar::~AbstractMenuBar () to require that menus all deleted.
 *		Make some things const methods.
 *		Added settablet to desktop tablet hack for motif.
 *
 *		Revision 1.3  1992/02/27  22:05:41  lewis
 *		Got rid of GetNativeMenu stuff.
 *		AbstractMenuBar now a view so dont mix in later in various subclasses.
 *		Override GetParentPanel () const in MenuBar_MacUI_Native to say its in the desktop.
 *		Use menutitle instead of MenuTitle_Mac_NativePtr since thats now gone.
 *		Fix sequence usage - const method should return const sequence, and should return Sequence() not Seqeunce_DLL...()
 *		Properly add menutitles as subviews of menubar...
 *
 *		Revision 1.2  1992/02/15  06:14:46  lewis
 *		Do class MenuBar instead of #defines, and other misc cleanups, including asserting
 *		unrealized at destruction for motif menubar, ...
 *
 *		Revision 1.1  1992/02/15  00:48:30  lewis
 *		Initial revision
 *
 *
 *
 */



#include	<ctype.h>
#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Desk.h>				// just for OpenDeskAcc
#include	<Memory.h>
#include	<Menus.h>
#include	<ToolUtils.h>			// just for HiWord/LoWord
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
#if		qXmToolkit
#include	<Xm/Xm.h>
#include	<Xm/RowColumn.h>
#include	<Xm/RowColumnP.h>
// Min/Max defined in Xm/XmP.h
#undef 	Min
#undef 	Max
// undef XtDisplay/XtWindow from Xm/XmP.h since it casts arg to Widget* instaed of osWidget* - with function version no problem...
#undef	XtDisplay
#undef	XtWindow
#endif
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"Application.hh"
#include	"DeskTop.hh"
#include	"Menu.hh"
#include	"MenuItem.hh"
#include	"OSControls.hh"
#include	"PushButton.hh"

#include	"MenuBar.hh"






/*
 ********************************************************************************
 ********************************** AbstractMenuBar******************************
 ********************************************************************************
 */

AbstractMenuBar::AbstractMenuBar ()
{
}
	
void	AbstractMenuBar::AddMenu (Menu* menu, const String& title, CollectionSize index)
{
	AddMenuTitle (BuildMenuTitle (menu, title), index);
}

void	AbstractMenuBar::RemoveMenu (Menu* menu)
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		if (it.Current ()->GetMenu () == menu) {
			DeleteMenuTitle (it.Current ());
			break;
		}
	}
}
	
void	AbstractMenuBar::ReorderMenu (Menu* menu, CollectionSize index)
{
	MenuTitle*	menuTitle = Nil;
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		if (it.Current ()->GetMenu () == menu) {
			menuTitle = it.Current ();
			break;
		}
	}
	RequireNotNil (menuTitle);
	RemoveMenuTitle (menuTitle);
	AddMenuTitle (menuTitle, index);
}

void	AbstractMenuBar::AddMenuTitle (MenuTitle* menuTitle, CollectionSize index)
{
	RequireNotNil (menuTitle);
	
	if ((index == eAppend) or (index > GetLength ())) {
		index = GetLength () + 1;
	}
	AddMenuTitle_ (menuTitle, index);
}

void	AbstractMenuBar::RemoveMenuTitle (MenuTitle* menuTitle)
{
	RequireNotNil (menuTitle);
	Require (GetMenuIndex (menuTitle->GetMenu ()) != kBadSequenceIndex);
	RemoveMenuTitle_ (menuTitle);
}

void	AbstractMenuBar::DeleteMenuTitle (MenuTitle* menuTitle)
{
	RequireNotNil (menuTitle);
	Require (GetMenuIndex (menuTitle->GetMenu ()) != kBadSequenceIndex);
	RemoveMenuTitle (menuTitle);
	delete (menuTitle);
}

void	AbstractMenuBar::AddMenuTitle_ (MenuTitle* menuTitle, CollectionSize index)
{
	RequireNotNil (menuTitle);
	AddSubView (menuTitle, index);
}

void	AbstractMenuBar::RemoveMenuTitle_ (MenuTitle* menuTitle)
{
	RequireNotNil (menuTitle);
	RemoveSubView (menuTitle);
}

CollectionSize	AbstractMenuBar::GetLength_ () const
{
	return (GetMenus ().GetLength ());
}

CollectionSize	AbstractMenuBar::GetMenuIndex_ (Menu* menu)
{
	CollectionSize	index = 1;
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		if (it.Current ()->GetMenu () == menu) {
			return (index);
		}
		index++;
	}
	return (kBadSequenceIndex);
}

Menu*	AbstractMenuBar::GetMenu_ (CollectionSize index) const
{
	// This should be implementable in some CONST way!!!
	return (GetMenus ()[index]->GetMenu ());
}	

MenuItem*	AbstractMenuBar::GetMenuItem_ (CommandNumber commandNumber) const
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		MenuItem*	item = it.Current ()->GetMenuItem (commandNumber);
		if (item != Nil) {
			return (item);
		}
	}
	return (Nil);
}

void	AbstractMenuBar::SetEnabled_ (CommandNumber commandNumber, Boolean enabled)
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		it.Current ()->SetEnabled (commandNumber, enabled);
	}
}
	
#if		qSupportMneumonics		
void	AbstractMenuBar::SetMneumonic_ (CommandNumber commandNumber, const String& mneumonic)
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		it.Current ()->SetMneumonic (commandNumber, mneumonic);
	}
}
#endif	/* qSupportMneumonics */		

void	AbstractMenuBar::SetName_ (CommandNumber commandNumber, const String& name)
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		it.Current ()->SetName (commandNumber, name);
	}
}

void	AbstractMenuBar::SetOn_ (CommandNumber commandNumber, Boolean on)
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		it.Current ()->SetOn (commandNumber, on);
	}
}

void	AbstractMenuBar::BeginUpdate_ ()
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		it.Current ()->BeginUpdate_ ();
	}
}

Boolean	AbstractMenuBar::EndUpdate_ ()
{
	Boolean	changed = False;
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		changed = Boolean (it.Current ()->EndUpdate_ () or changed);
	}
	if (changed) {
		Redraw ();
	}
	
	return (changed);
}

void	AbstractMenuBar::Redraw ()
{
	Redraw_ ();
}

Boolean	AbstractMenuBar::DoMenuSelection_ (Character keyPressed, MenuItem*& selected)
{
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MenuTitlePtr, it, GetMenus ().operator Iterator(MenuTitlePtr)* ()) {
#else
	ForEach (MenuTitlePtr, it, GetMenus ()) {
#endif
		if (it.Current ()->DoMenuSelection_ (keyPressed, selected)) {
			return (True);
		}
	}
	return (False);
}











#if		qMacToolkit
/*
 ********************************************************************************
 ******************************* MenuBar_MacUI_Native ***************************
 ********************************************************************************
 */

Sequence(MenuTitlePtr)	MenuBar_MacUI_Native::sCascadeMenus;


static	MenuBar_MacUI_Native*	sREALLYInstalledInSystemMBar	=	Nil;



MenuBar_MacUI_Native::MenuBar_MacUI_Native ():
	AbstractMenuBar (),
	fOSMenuBar (Nil)
{
	fOSMenuBar = ::GetMenuBar ();

SetTablet (DeskTop::Get ().GetTablet ());		// hack for mac ...
}

MenuBar_MacUI_Native::~MenuBar_MacUI_Native ()
{
	Require (GetLength () == 0);		// remove menus before destroying the menubar...
}

Boolean	MenuBar_MacUI_Native::TrackPress (const MousePressInfo& mouseInfo)
{
	MenuItem*	selected = Nil;
	Application::Get ().SetupTheMenus ();
	if (DoMenuSelection (mouseInfo.fPressAt, selected)) {
		AssertNotNil (selected);
		Application::Get ().ProcessCommand (*selected);
	}
	return (True);
}

Boolean	MenuBar_MacUI_Native::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)
{
// need to check if were installed in the real menubar right now???
	osPoint	tmpPt;
	long	menuSelectResult = ::MenuSelect (os_cvt (startPt, tmpPt));

	short	whichMenu	=	HiWord (menuSelectResult);
	if (whichMenu != 0) {					// See IM I-355
		ForEach (MenuTitlePtr, it, fMenus) {
			AssertNotNil (it.Current ());
			if (whichMenu == (*it.Current ()->GetMenu ()->GetOSRepresentation ())->menuID) {
				short	whichItem	=	LoWord (menuSelectResult);
				
				if (whichItem > it.Current ()->GetMenu ()->GetLength ()) {
					// hack to support Apple's System Menu, should probably come up with a better test for this				
					osStr255	name;
					::GetItem (it.Current ()->GetMenu ()->GetOSRepresentation (), whichItem, name);
					::OpenDeskAcc (name);
					::HiliteMenu (0);
					return (False);
				}
				
				selected = 	it.Current()->GetMenu ()->GetMenuItemByIndex (whichItem);
				return (True);
			}
		}
		ForEach (MenuTitlePtr, it1, sCascadeMenus) {
			AssertNotNil (it1.Current ());
			if (whichMenu == (*it1.Current ()->GetMenu ()->GetOSRepresentation ())->menuID) {
				short	whichItem	=	LoWord (menuSelectResult);
				selected = 	it1.Current()->GetMenu ()->GetMenuItemByIndex (whichItem);
				return (True);
			}
		}
		AssertNotReached ();	// why would we get a menu we dont know?  Could be some damn patch?
	}
	return (False);
}

void	MenuBar_MacUI_Native::HandleMenuSelection (const MenuItem& item)
{
// need to check if were installed in the real menubar right now???
	Menu*	menu = item.GetOwner ();
	ForEach (MenuTitlePtr, it, fMenus) {
		if (it.Current ()->GetMenu () == menu) {
			::HiliteMenu ((*it.Current ()->GetMenu ()->GetOSRepresentation ())->menuID);
			break;
		}
	}
}

const	Sequence(MenuTitlePtr)&	MenuBar_MacUI_Native::GetMenus () const
{
	return (fMenus);
}

MenuTitle*	MenuBar_MacUI_Native::BuildMenuTitle (Menu* menu, const String& title)
{
	return (new PullDownMenuTitle (title, menu));
}

void	MenuBar_MacUI_Native::AddMenuTitle_ (MenuTitle* menuTitle, CollectionSize index)
{
	fMenus.InsertAt (menuTitle, index);
	AbstractMenuBar::AddMenuTitle_ (menuTitle, index);

	/*
	 * Since Insert/DeleteMenu on the mac refer to the current menu bar, we must save the current menu
	 * bar, modify the global one, copy that to our local instance variable, and restore original.
	 * If we are the menu bar currently installed, then dont bother with much of the magic.
	 */
	osHandle	oldMenuBar	=	(sREALLYInstalledInSystemMBar==this)? Nil: ::GetMenuBar ();
	if (oldMenuBar != Nil) {
		AssertNotNil (fOSMenuBar);
		::SetMenuBar (fOSMenuBar);
	}


	RequireNotNil (menuTitle->GetMenu ()->GetOSRepresentation ());
	if (index >= GetLength ()) {
		::InsertMenu (menuTitle->GetMenu ()->GetOSRepresentation (), 0);
	}	
	else {
		Require (index >= 1);
		
		MenuTitle*	menuT = fMenus[index+1];	// plus one cuz we inserted ourself into index slot
		RequireNotNil (menuT->GetMenu ()->GetOSRepresentation ());
		::InsertMenu (menuTitle->GetMenu ()->GetOSRepresentation (), (*menuT->GetMenu ()->GetOSRepresentation ())->menuID);
	}


	// resnag real value of menu bar now...
	::DisposeHandle (fOSMenuBar); fOSMenuBar = Nil;
	fOSMenuBar = ::GetMenuBar ();
	if (oldMenuBar != Nil) {
		::SetMenuBar (oldMenuBar);
	}
}

void	MenuBar_MacUI_Native::RemoveMenuTitle_ (MenuTitle* menuTitle)
{
	/*
	 * Since Insert/DeleteMenu on the mac refer to the current menu bar, we must save the current menu
	 * bar, modify the global one, copy that to our local instance variable, and restore original.
	 * If we are the menu bar currently installed, then dont bother with much of the magic.
	 */
	osHandle	oldMenuBar	=	(sREALLYInstalledInSystemMBar==this)? Nil: ::GetMenuBar ();
	if (oldMenuBar != Nil) {
		AssertNotNil (fOSMenuBar);
		::SetMenuBar (fOSMenuBar);
	}

	RequireNotNil (menuTitle->GetMenu ()->GetOSRepresentation ());
	::DeleteMenu ((*menuTitle->GetMenu ()->GetOSRepresentation ())->menuID);

	AbstractMenuBar::RemoveMenuTitle_ (menuTitle);
	fMenus.Remove (menuTitle);

	// resnag real value of menu bar now...
	::DisposeHandle (fOSMenuBar); fOSMenuBar = Nil;
	fOSMenuBar = ::GetMenuBar ();
	if (oldMenuBar != Nil) {
		::SetMenuBar (oldMenuBar);
	}
}

void	MenuBar_MacUI_Native::Redraw_ ()
{
// we really want to say - if installed in mac menu bar, then return desktop, else, default...
// hack for now...
	if (View::GetParentPanel () == Nil) {
		::DrawMenuBar ();
	}
	else {
		Refresh (eImmediateUpdate);
	}
}

Point	MenuBar_MacUI_Native::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Coordinate	height = 20;
	Coordinate	width = 0;
	ForEach (MenuTitlePtr, it, fMenus) {
		MenuTitle*	title	=	it.Current ();
		AssertNotNil (title);
		height = title->GetSize ().GetV ();
		width += title->GetSize ().GetH ();
	}

	return (Point (height, width));
}

void	MenuBar_MacUI_Native::Layout ()
{
	// might want to own a <STERL NEW > Grid - to implement this...
	Coordinate	left	=	0;
	ForEach (MenuTitlePtr, it, fMenus) {
		MenuTitle*	title	=	it.Current ();
		AssertNotNil (title);
		title->SetOrigin (Point (0, left));
		title->SetSize (title->CalcDefaultSize ());
		left += title->GetSize ().GetH ();
	}
	View::Layout ();
}

Panel*	MenuBar_MacUI_Native::GetParentPanel () const
{
// we really want to say - if installed in mac menu bar, then return desktop, else, default...
// hack for now...
	if (View::GetParentPanel () == Nil) {
		return (&DeskTop::Get ());
	}
	else {
		return (View::GetParentPanel ());
	}
}

// Start attempt at supporting multiple menu bars, and cooperation as to the installed one...
void	MenuBar_MacUI_Native::BeREALLYInstalledInSystemMBar ()
{
	if (sREALLYInstalledInSystemMBar != this) {
		sREALLYInstalledInSystemMBar = this;
		::SetMenuBar (fOSMenuBar);
		Redraw ();
	}
}

MenuBar_MacUI_Native*	MenuBar_MacUI_Native::GetREALLYInstalledInSystemMBar ()
{
	return (sREALLYInstalledInSystemMBar);
}






#elif	qXmToolkit




/*
 ********************************************************************************
 ********************************** MenuBar_MotifUI ****************************
 ********************************************************************************
 */

MenuBar_MotifUI::MenuBar_MotifUI ():
	AbstractMenuBar (),
	fOSControl (Nil),
	fHelpMenu (Nil)
{
	class	MyOSMenuBar : public MotifOSControl {
		public:
			MyOSMenuBar ():
				MotifOSControl ()
			{
			}
			
			override	osWidget*	DoBuild_ (osWidget* parent)
			{
				RequireNotNil (parent);
				osWidget* w = ::XmCreateMenuBar (parent, "MenuBar", Nil, 0);
				return (w);
			}
	};
	
	AddSubView (fOSControl = new MyOSMenuBar ());
}

MenuBar_MotifUI::~MenuBar_MotifUI ()
{
	Require (GetLength () == 0);		// remove menus before destroying the menubar...

	if (fOSControl != Nil) {
		RemoveSubView (fOSControl);
		delete (fOSControl);
	}
}

void	MenuBar_MotifUI::Realize (osWidget* parent)
{
	RequireNotNil (parent);
	RequireNotNil (fOSControl);
	/*
	 * Somewhat unnatural to have one subview being teh parent widget for my other subviews. Not
	 * too sure but this might get me in trouble???
	 */
	fOSControl->Realize (parent);
	AbstractMenuBar::Realize (fOSControl->GetOSRepresentation ());

	if ((GetWidget () != Nil) and (fHelpMenu != Nil)) {
		osWidget*	helpWidget	=	fHelpMenu->GetWidget ();
		AssertNotNil (helpWidget);
		::XtVaSetValues (GetWidget (), XmNmenuHelpWidget, helpWidget, Nil);
	}
}

void	MenuBar_MotifUI::UnRealize ()
{
	RequireNotNil (fOSControl);
	AbstractMenuBar::UnRealize ();
	fOSControl->UnRealize ();
}

osWidget*	MenuBar_MotifUI::GetWidget () const
{
	RequireNotNil (fOSControl);
	return (fOSControl->GetWidget ());
}

Boolean	MenuBar_MotifUI::TrackPress (const MousePressInfo& mouseInfo)
{
//gDebugStream << "entering MenuBar_MotifUI::TrackPress (const MousePressInfo& mouseInfo)" << newline;
	MenuItem*	selected = Nil;
	Application::Get ().SetupTheMenus ();
	if (DoMenuSelection (mouseInfo.fPressAt, selected)) {
		AssertNotNil (selected);
		Application::Get ().ProcessCommand (*selected);
	}
}

void	MenuBar_MotifUI::SetHelpMenu (Menu* whichMenu)
{
	fHelpMenu = whichMenu;
	if ((GetWidget () != Nil) and (whichMenu != Nil)) {
		osWidget*	helpWidget	=	whichMenu->GetWidget ();
		AssertNotNil (helpWidget);
		::XtVaSetValues (GetWidget (), XmNmenuHelpWidget, helpWidget, Nil);
	}
}

Point	MenuBar_MotifUI::CalcDefaultSize_ (const Point& defaultSize) const
{
	Coordinate	height = 34;
	Coordinate	width = 0;
	ForEach (MenuTitlePtr, it, fMenus) {
		MenuTitle*	title	=	it.Current ();
		AssertNotNil (title);
		Point titleSize = title->CalcDefaultSize (defaultSize);
		height = titleSize.GetV ();
		width += titleSize.GetH ();
	}

	/*
	 * Make sure that even with no menu titles, by default we see a little something.
	 */
	if (width <= 20) {
		width = 20;
	}

	return (Point (height, width));
}

Boolean	MenuBar_MotifUI::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)
{
	Require (GetMenuItemSelected () == Nil);		// should always leave nil to be sure stuff being used properly...
	SetMenuItemSelected (Nil);

	/*
	 * Generate a bogus event for the widget. The only trouble is that there may be
	 * some info from the real event that generated this that we want to keep. Thats tough, since
	 * at this point, we don't event know if there was such a beast.
	 *
	 * Ultimately, the best solution to this problem, is probably to add an eventRecord to the
	 * mouse info, so we can just use it here (must be a boolean to say if MosuePressInfo created
	 * with event or not).
	 *
	 * For now, just snag last event, and copy a few stray fields...
	 */
	osWidget*	w	=	fOSControl->GetWidget ();
	RequireNotNil (w);
	osEventRecord	er;

	EventManager::Get ().GetCurrentEvent (er);
	er.xbutton.type = ButtonPress;
	er.xbutton.display = XtDisplay (w);
	er.xbutton.window = XtWindow (w);		// IMPLIES WIDGET - NOT GADGET - CLEANUP FOR GADGET
	AssertNotNil (er.xbutton.window);
	er.xbutton.root = DefaultRootWindow (er.xbutton.display);
	er.xbutton.x = int (startPt.GetH ());
	er.xbutton.y = int (startPt.GetV ());
	Point	globalPoint	=	LocalToAncestor (startPt, &DeskTop::Get ());
	er.xbutton.x_root = int (globalPoint.GetH ());
	er.xbutton.y_root = int (globalPoint.GetV ());
	er.xbutton.same_screen = True;
	if (er.xbutton.button == 0) {
		er.xbutton.button = Button1;		 // if probably bogus event...
	}

//	Application::Get ().SuspendIdleTasks ();
//	Application::Get ().SuspendCommandProcessing ();
	::XtDispatchEvent (&er);

	Exception* caught = Nil;
	Try {
		while (RC_IsArmed (w)) {
			EventManager::Get ().RunOnce ();
		}
	}
	Catch () {
		caught = _this_catch_;
	}
//	Application::Get ().ResumeIdleTasks ();
//	Application::Get ().ResumeCommandProcessing ();
	if (caught != Nil) {
		caught->Raise ();
	}

	Boolean	result	=	Boolean (GetMenuItemSelected () != Nil);
	if (result) {
		selected = GetMenuItemSelected ();
	}

	SetMenuItemSelected (Nil);

	return (result);
}

const	Sequence(MenuTitlePtr)&	MenuBar_MotifUI::GetMenus () const
{
	return (fMenus);
}

void	MenuBar_MotifUI::Redraw_ ()
{
	Refresh ();
}

MenuTitle*	MenuBar_MotifUI::BuildMenuTitle (Menu* menu, const String& title)
{
	return (new PullDownMenuTitle (title, menu));
}

void	MenuBar_MotifUI::AddMenuTitle_ (MenuTitle* menuTitle, CollectionSize index)
{
	fMenus.InsertAt (menuTitle, index);
	AbstractMenuBar::AddMenuTitle_ (menuTitle, index);
}

void	MenuBar_MotifUI::RemoveMenuTitle_ (MenuTitle* menuTitle)
{
	AbstractMenuBar::RemoveMenuTitle_ (menuTitle);
	fMenus.Remove (menuTitle);
}

void	MenuBar_MotifUI::Layout ()
{
	AssertNotNil (fOSControl);
	Assert (fOSControl->GetOrigin () == kZeroPoint);
	fOSControl->SetSize (GetSize ());
	AbstractMenuBar::Layout ();
}

#endif    /*Toolkit*/









/*
 ********************************************************************************
 *************************************** MenuBar ********************************
 ********************************************************************************
 */

MenuBar::MenuBar ():
#if		qMacToolkit
	MenuBar_MacUI_Native ()
#elif	qXmToolkit
	MenuBar_MotifUI ()
#endif
{
}



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

