/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuOwner.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 *
 *
 * TODO:
 *			Fix table of Accelterators - not sure if it belongs in this file, and there is CERTIANLY
 *			a construction order problem with it!!!!
 *
 *
 *
 * Changes:
 *	$Log: MenuOwner.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/07/02  04:56:07  lewis
 *		Renamed Sequence_DoublyLLOfPointers -> SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.55  1992/06/09  15:30:25  sterling
 *		changed size of Mapping
 *
 *		Revision 1.52  92/05/14  15:14:28  15:14:28  lewis (Lewis Pringle)
 *		Use qMPW_APPLE_CPP_BROKEN_FOR_DECLARE2.
 *		
 *		Revision 1.51  92/05/11  20:06:28  20:06:28  lewis (Lewis Pringle)
 *		Made MenuOwner::GetMenu and MenuOwner::SetEnabled/2 inline.
 *		
 *		Revision 1.49  92/04/24  09:02:48  09:02:48  lewis (Lewis Pringle)
 *		Added conditionally compiled template stuff, and global variable for current menu item selected.
 *		Hack just for motif,
 *		
 *		Revision 1.46  1992/02/29  15:52:37  lewis
 *		Had to get rid of override of dtor and call to Assert (GetLength () == 0) since that indirectly
 *		called GetMenus () - a pure virtual function.
 *
 *		Revision 1.45  1992/02/28  22:26:07  lewis
 *		Made a few methods const. Made a few methods inline.
 *		Added dtor to assure all submenus deleted.
 *
 *		Revision 1.44  1992/02/19  17:21:06  lewis
 *		Support new accelerator format.
 *
 *		Revision 1.43  1992/02/15  06:37:53  lewis
 *		Got rid of unneeded includes, and added ensures on call to GetMenuIndex.
 *
 *		Revision 1.42  1992/02/15  00:39:00  lewis
 *		Moved everything but MenuOwner to seperate files (eg MenuBar, etc.).
 *
 *		Revision 1.41  1992/02/14  03:17:42  lewis
 *		Use GetMenuItemByCommand since redundent GetMenuItem is deleted.
 *
 *		Revision 1.40  1992/02/12  08:15:47  lewis
 *		Add #include oscontrols.
 *
 *		Revision 1.39  1992/02/12  07:07:03  lewis
 *		Change from AbstractMenu to Menu.
 *
 *		Revision 1.37  1992/02/11  01:15:06  lewis
 *		Change to use new Menu stuff (1 class).
 *
 *		Revision 1.34  1992/02/05  07:46:51  lewis
 *		Use new menu types, and get rid of bool param to Menu_MotifUI to say if it is popup - figure out
 *		automatically now.
 *
 *		Revision 1.33  1992/02/03  22:26:38  lewis
 *		General cleanups.
 *
 *		Revision 1.32  1992/02/01  19:34:59  lewis
 *		Call RealizeWidget () when making a menubar before it gets managed. That works around bug
 *		in managing code where menubar doesnt get set right size in main app window (not sure if
 *		this is motif bug, or just a feature).
 *
 *		Revision 1.31  1992/01/31  16:51:54  sterling
 *		temp hack to Motif Popup to setupthemenus, should do this to MenjOwners in general
 *
 *		Revision 1.30  1992/01/31  16:30:38  lewis
 *		Get rid of unneeded motif includes, and use gadgets where possible instead of widgets.
 *
 *		Revision 1.29  1992/01/27  05:57:41  sterling
 *		fixed when we build menu
 *
 *		Revision 1.28  1992/01/27  05:33:58  sterling
 *		fixed StTitle call for Motif Popup
 *
 *		Revision 1.26  1992/01/23  21:47:27  lewis
 *		Fixed so didnt call MotifOSControl::SetTitle/SetSensative () - no longer exist - more general methods.
 *
 *		Revision 1.25  1992/01/23  20:04:29  sterling
 *		support for PopUps in Motif
 *
 *		Revision 1.24  1992/01/23  04:29:59  sterling
 *		made PopUp use OSControl
 *
 *
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"MenuOwner.hh"



#if		!qRealTemplatesAvailable
	Implement (Iterator, MenuOwnerPtr);
	Implement (Collection, MenuOwnerPtr);
	Implement (AbSequence, MenuOwnerPtr);
	Implement (Array, MenuOwnerPtr);
	Implement (Sequence_Array, MenuOwnerPtr);
	Implement (Sequence, MenuOwnerPtr);

	Implement (Iterator, Accelerator);
	Implement (Collection, Accelerator);
	AbMappingImplement (CommandNumber, Accelerator);
	Mapping_HTImplement1 (CommandNumber, Accelerator);
	Mapping_HTImplement2 (CommandNumber, Accelerator);
	Mapping_HTImplement3 (CommandNumber, Accelerator);
#endif



#if		qRealTemplatesAvailable
Sequence<MenuOwner*>			MenuOwner::sInstalledMenus;
#else
Sequence(MenuOwnerPtr)		MenuOwner::sInstalledMenus;
#endif
const Accelerator							MenuOwner::kNoAccelerator	=	String ();
Boolean										MenuOwner::sMenusOutOfDate	=	True;




static	CollectionSize	CommandNumberHashFunction (const CommandNumber& key)
{
	return (key);
}

/*
 ********************************************************************************
 ********************************** MenuOwner ***********************************
 ********************************************************************************
 */
MenuOwner::MenuOwner ()
{
}

AbMapping(CommandNumber, Accelerator)&	MenuOwner::GetAccelerators ()
{
	static	Mapping_HashTable(CommandNumber, Accelerator)*	sAccelerators 
		= new Mapping_HashTable(CommandNumber, Accelerator) (&CommandNumberHashFunction, 53);
	return (*sAccelerators);
}

CollectionSize	MenuOwner::GetMenuIndex (Menu* menuItem)
{
	RequireNotNil (menuItem);
	CollectionSize	i	= GetMenuIndex_ (menuItem);
	Ensure (i != 0);
	Ensure (i == kBadSequenceIndex or (i <= GetLength ()));
	return (i);
}

#if		qSupportMneumonics	
void	MenuOwner::SetMneumonic (CommandNumber commandNumber, const String& mneumonic)
{
	SetMneumonic_ (commandNumber, mneumonic);
}
#endif	/*qSupportMneumonics*/		
		
void	MenuOwner::SetName (CommandNumber commandNumber, const String& name)
{
	SetName_ (commandNumber, name);
}

void	MenuOwner::SetOn (CommandNumber commandNumber, Boolean checked)
{
	SetOn_ (commandNumber, checked);
}

void	MenuOwner::BeginUpdate ()
{
	Require (sMenusOutOfDate);
	ForEach (MenuOwnerPtr, it, sInstalledMenus) {
		it.Current ()->BeginUpdate_ ();
	}
}

void	MenuOwner::EndUpdate ()
{
	Require (sMenusOutOfDate);
	ForEach (MenuOwnerPtr, it, sInstalledMenus) {
		it.Current ()->EndUpdate_ ();
	}
	sMenusOutOfDate = False;
}

void	MenuOwner::InstallMenu ()
{
	sInstalledMenus.Append (this);
	SetMenusOutOfDate ();
}

void	MenuOwner::DeinstallMenu ()
{
	sInstalledMenus.Remove (this);
}

SequenceIterator(MenuOwnerPtr)*	MenuOwner::InstalledMenusIterator (SequenceDirection d)
{
	return (sInstalledMenus.MakeSequenceIterator (d));
}

Boolean	MenuOwner::DoMenuSelection (const Point& startPt, MenuItem*& selected)
{
	if (DoMenuSelection_ (startPt, selected)) {
		AssertNotNil (selected);
		HandleMenuSelection (*selected);
		return (True);
	}
	return (False);
}

Boolean	MenuOwner::DoMenuSelection (Character keyPressed, MenuItem*& selected)
{
	ForEach (MenuOwnerPtr, it, sInstalledMenus) {
		if (it.Current ()->DoMenuSelection_ (keyPressed, selected)) {
			AssertNotNil (selected);
			it.Current ()->HandleMenuSelection (*selected);
			return (True);
		}
	}
	return (False);
}

#if		qXmToolkit
MenuItem*	MenuOwner::sSelectedMenuItem	=	Nil;

MenuItem*	MenuOwner::GetMenuItemSelected ()
{
	return (sSelectedMenuItem);
}

void	MenuOwner::SetMenuItemSelected (MenuItem* menuItem)
{
	sSelectedMenuItem = menuItem;
}
#endif

void	MenuOwner::HandleMenuSelection (const MenuItem& item)
{
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

