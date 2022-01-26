/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuTitle.cc,v 1.9 1992/09/11 19:46:04 sterling Exp $
 *
 * TODO:
 *			Be sure that we are enabled iff our subitems are - maybe get rid of fEnabled boolean and
 *			always compute.. Or just and in GetEnabled of menu in GetLive computation. Thats the trouble
 *			with putting too much into abstract base classes like EnableItem - not sure I want to be able
 *			to call SetEnabled on a menu title - really probably want to to be totally tied to the menu
 *			I own...
 *
 * Changes:
 *	$Log: MenuTitle.cc,v $
 *		Revision 1.9  1992/09/11  19:46:04  sterling
 *		Get rid of peeks.
 *
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:41:45  sterling
 *		minor speed tweak to EffectiveLiveChanged
 *
 *		Revision 1.3  1992/07/03  01:01:41  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/07/02  04:56:58  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.20  92/05/18  16:25:13  16:25:13  lewis (Lewis Pringle)
 *		Cleaning up mac compiler warnings.
 *		
 *		Revision 1.19  92/05/12  23:59:41  23:59:41  lewis (Lewis Pringle)
 *		Get rid of MenuTitle::Draw () override - if needed in some subclasses, do it there.
 *		
 *		Revision 1.18  92/05/11  20:10:02  20:10:02  lewis (Lewis Pringle)
 *		Made GetMenu_ () const.
 *		
 *		Revision 1.17  92/04/20  14:27:17  14:27:17  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		
 *		Revision 1.16  92/04/13  00:56:32  00:56:32  lewis (Lewis Pringle)
 *		Get rid of MenuTitle::Draw ().
 *		If needed for some cases on the mac, then do mac specifically.
 *		
 *		Revision 1.15  92/04/02  17:42:55  17:42:55  lewis (Lewis Pringle)
 *		Check that the given Menu in MenuTitle::CTOR is not already a subview some place. This is most
 *		likely to happen if you've accidentally added the given menu to two different menu items.
 *		
 *		Revision 1.14  92/04/02  13:07:27  13:07:27  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.13  92/03/26  09:56:49  09:56:49  lewis (Lewis Pringle)
 *		Get rid of oldLive arg to EffectiveLiveChanged.
 *		
 *		Revision 1.11  1992/03/19  16:52:49  lewis
 *		Added Draw override so for non-native siturations like the mac we at least see something. Needs
 *		work to look right though.
 *		Also, re-enabled code to do SetTitle_ for motif menus.
 *
 *		Revision 1.10  1992/03/17  17:05:42  lewis
 *		Start trying to add SetTitle_ override for MenuTitle for Motif - but caused bugs will pulldownmenutitle_MotifUI.
 *		Comment out and fix later.
 *
 *		Revision 1.9  1992/03/10  13:06:07  lewis
 *		Temporarily make hack to change case of accelerators to be if mac or motif.
 *
 *		Revision 1.8  1992/03/06  02:34:45  lewis
 *		Added dtor to MenuTitle to call SetMenu (Nil), so it would be removed as a subview - in reaction
 *		to sterls change in View - not deleting subviews. I like the change, but must study its impact
 *		better on MenuTitles, and related classes.
 *
 *		Revision 1.6  1992/02/28  22:27:12  lewis
 *		Make GetMenuItem_ const.
 *
 *		Revision 1.4  1992/02/27  23:41:04  lewis
 *		Require menu parameter for MenuTitle (no nil default).
 *		Be an EnableItem, and View (big change). Involves adding boolean, and setenabled/getenabled
 *		effectivelivechanged overrides,a nd GetLive. Also, disambiguate SetEnabled method in favor of
 *		menuowner version. Moved stuff like MacDoMenuSelection () here from the now defunct MenuTitle_Mac_Native.
 *		no longer delete menu in setmenu.
 *		fixed subtle bug in mac settitle code with subtraction of unsinged quantities (CollectionSize).
 *
 *		Revision 1.2  1992/02/15  06:52:22  lewis
 *		In GetMenuItem return kBadSequenceIndex instead of zero.
 *
 *		Revision 1.1  1992/02/15  00:48:30  lewis
 *		Initial revision
 *
 */



#include	<ctype.h>
#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Memory.h>
#include	<Menus.h>
#include	<ToolUtils.h>
#elif	qXmToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/Xm.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"DeskTop.hh"
#include	"Menu.hh"

#include	"MenuTitle.hh"




#if		!qRealTemplatesAvailable
	Implement (Iterator, MenuTitlePtr);
	Implement (Collection, MenuTitlePtr);
	Implement (AbSequence, MenuTitlePtr);
	Implement (Array, MenuTitlePtr);
	Implement (Sequence_Array, MenuTitlePtr);
	Implement (Sequence, MenuTitlePtr);
#endif







/*
 ********************************************************************************
 ********************************** MenuTitle ***********************************
 ********************************************************************************
 */
MenuTitle::MenuTitle (const String& title, Menu* menu):
	MenuOwner (),
	EnableItem (),
	View (),
	fEnabled (True),
	fMenu (Nil),
	fTitle (kEmptyString)
{
	RequireNotNil (menu);							// cuz we use it in SetTitle... Simpler to require it always passed???
	Require (menu->GetParentView () == Nil);		// not already installed someplaces...
	SetMenu (menu);
	SetTitle (title);
}

MenuTitle::~MenuTitle ()
{
	SetMenu (Nil);
}

String	MenuTitle::GetTitle () const
{
	return (fTitle);
}

void	MenuTitle::SetTitle (const String& title)
{
	if (GetTitle () != title) {
		SetTitle_ (title);
	}
	Ensure (GetTitle () == title);
}
		
Menu*	MenuTitle::GetMenu () const
{
	return (fMenu);
}

void	MenuTitle::SetMenu (Menu* menu)
{
	if (GetMenu () != menu) {
		SetMenu_ (menu);
	}
	Ensure (GetMenu () == menu);
}

Boolean	MenuTitle::DoMenuSelection_ (Character keyPressed, MenuItem*& selected)
{
// hack to do this for motif --- not really sure if its right for motif or not??
#if		qMacUI || qMotifUI
	// mac is case insensitive in interpretting accellerators...
	char	key = keyPressed.GetAsciiCode ();
	if (islower (key)) {
		keyPressed = toupper (key);
	}
#endif
	ForEach (MenuItemPtr, it, GetMenu ()->MakeMenuItemIterator ()) {
		selected = it.Current ();
		if (it.Current ()->GetEnabled ()) {
			Accelerator	accelerator = selected->GetAccelerator ();
			if ((not accelerator.Empty ()) and (accelerator.GetAccChar () == keyPressed)) {
				HandleMenuSelection (*selected);
				return (True);
			}
		}
	}
	return (False);
}

Boolean	MenuTitle::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)
{
	RequireNotNil (GetMenu ());
#if		qMacToolkit
	return (MacDoMenuSelection (GetMenu (), startPt, selected));		
#else
	AssertNotImplemented ();
#endif
}

void	MenuTitle::HandleMenuSelection (const MenuItem& item)
{
	RequireNotNil (GetMenu ());
#if		qMacToolkit
	::HiliteMenu ((*GetMenu ()->GetOSRepresentation ())->menuID);
#endif
	MenuOwner::HandleMenuSelection (item);
}

void	MenuTitle::SetTitle_ (const String& title)
{
	RequireNotNil (GetMenu ());
	fTitle = title;
#if		qMacToolkit
	// rename magic
	osMenu**	theMenuHandle	=	GetMenu ()->GetOSRepresentation ();
	RequireNotNil (theMenuHandle);
	String	oldTitle = String (&(*theMenuHandle)->menuData[1], (*theMenuHandle)->menuData[0]);
	CollectionSize	oldTitleLength = oldTitle.GetLength () + 1;
	CollectionSize	newTitleLength = title.GetLength () + 1;
	
	int sizeDelta = int (newTitleLength) - int (oldTitleLength);
	size_t	oldSize = size_t (::GetHandleSize ((osHandle) theMenuHandle));

	char*	dataStart = (char*) &(*theMenuHandle)->menuData;
	char*	dataPtr =  dataStart + oldTitleLength;
	size_t	bytesToCopy = oldSize - (dataPtr - (char*)*theMenuHandle);
	if (sizeDelta > 0) {
		::SetHandleSize ((osHandle) theMenuHandle, oldSize + sizeDelta);
		// refetch as handle could have moved
		dataStart = (char*) &(*theMenuHandle)->menuData;
		dataPtr =  dataStart + oldTitleLength;
		::BlockMove (dataPtr, dataPtr + sizeDelta, bytesToCopy);
	}
	else {
		::BlockMove (dataPtr, dataPtr + sizeDelta, bytesToCopy);
		::SetHandleSize ((osHandle) theMenuHandle, oldSize + sizeDelta);
	}
#if 1
	title.ToPStringTrunc (dataStart, title.GetLength ());
#else	
	dataStart[0] = (char) title.GetLength ();
	memcpy (&dataStart[1], title.Peek (), title.GetLength ());
#endif
#elif	qXmToolkit
	if (GetWidget () != Nil) {
		char*	tmp = title.ToCString ();
		XmString mStrTmp = XmStringCreate (tmp, XmSTRING_DEFAULT_CHARSET);
		Arg		arg;
		XtSetArg (arg, XmNlabelString, mStrTmp);
		::XtSetValues (GetWidget (), &arg, 1);
		::XmStringFree (mStrTmp);
		delete (tmp);
	}
#endif
}

void	MenuTitle::SetMenu_ (Menu* menu)
{
	if (fMenu != Nil) {
		RemoveSubView (fMenu);
	}

	fMenu = menu;

	if (fMenu != Nil) {
		AddSubView (fMenu);
	}
}

CollectionSize	MenuTitle::GetLength_ () const
{
	return ((fMenu == Nil)? 0: 1);
}

CollectionSize	MenuTitle::GetMenuIndex_ (Menu* menu)
{
	return ((fMenu == menu)? 1: kBadSequenceIndex);
}

Menu*	MenuTitle::GetMenu_ (CollectionSize index) const
{
	Require (index == 1);
	RequireNotNil (fMenu);
	return (fMenu);
}
		
MenuItem*	MenuTitle::GetMenuItem_ (CommandNumber commandNumber) const
{
	RequireNotNil (fMenu);
	return (fMenu->GetMenuItemByCommand (commandNumber));
}

void	MenuTitle::SetEnabled_ (CommandNumber commandNumber, Boolean enabled)
{
	RequireNotNil (fMenu);
	fMenu->SetEnabled (commandNumber, enabled);
}

#if		qSupportMneumonics		
void	MenuTitle::SetMneumonic_ (CommandNumber commandNumber, const String& mneumonic)
{
	RequireNotNil (fMenu);
	fMenu->SetMneumonic (commandNumber, mneumonic);
}
#endif	/* qSupportMneumonics */

void	MenuTitle::SetName_ (CommandNumber commandNumber, const String& name)
{
	RequireNotNil (fMenu);
	fMenu->SetName (commandNumber, name);
}

void	MenuTitle::SetOn_ (CommandNumber commandNumber, Boolean on)
{
	RequireNotNil (fMenu);
	fMenu->SetOn (commandNumber, on);
}

void	MenuTitle::BeginUpdate_ ()
{
	RequireNotNil (fMenu);
	fMenu->BeginUpdate ();
}

Boolean	MenuTitle::EndUpdate_ ()
{
	RequireNotNil (fMenu);
	return (fMenu->EndUpdate ());
}

Boolean	MenuTitle::GetLive () const
{
	return (Boolean (GetEnabled () and View::GetLive ()));
}

void	MenuTitle::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
// Calling this inherited EffectiveLiveChanged is a HUGE performance hit. Also, tis not clearly worth anything - our only
// subview is a menu?? ?Must think out carefully before changing, and talk to sterl - but 20% of time spent rbing
// up dlogs spent in here!! I think!!!

// Also not too sure why we need to do the Refresh()...
#if 0
	View::EffectiveLiveChanged (newLive, eNoUpdate);
#endif
	Refresh (updateMode);
}

Boolean	MenuTitle::GetEnabled_ () const
{
	return (fEnabled);
}

void	MenuTitle::SetEnabled_ (Boolean enabled, Panel::UpdateMode updateMode)
{
	Boolean	oldLive = GetEffectiveLive ();
	fEnabled = enabled;
	
	if (oldLive != GetEffectiveLive ()) {
		EffectiveLiveChanged (not oldLive, updateMode);
	}
}

#if		qMacToolkit
Boolean	MenuTitle::MacDoMenuSelection (Menu* nativeMenu, const Point& startPt, MenuItem*& selected)
{
	if (nativeMenu != Nil) {
		CollectionSize	startAt = kBadSequenceIndex;
		if (selected != Nil) {
			startAt = selected->GetIndex ();
		}

		osPoint	tmpPt;
		os_cvt (startPt, tmpPt);
		osMenu**	theOSMenu	=	nativeMenu->GetOSRepresentation ();

		::InsertMenu (theOSMenu, -1);
		long	menuSelectResult	=	::PopUpMenuSelect (theOSMenu, tmpPt.v, tmpPt.h, short (startAt));
		::DeleteMenu ((*theOSMenu)->menuID);

		short	whichMenu	=	HiWord (menuSelectResult);
		if (whichMenu != 0) {					// See IM I-355
			short	whichItem	=	LoWord (menuSelectResult);
			Assert (whichItem >= 1);
			Assert (whichItem <= nativeMenu->GetLength ());
			selected = nativeMenu->GetMenuItemByIndex (whichItem);
			return (True);
		}
	}
	return (False);
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

