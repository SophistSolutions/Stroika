/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Menu.cc,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Menu.cc,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.5  1992/07/14  19:53:58  lewis
 *		Rename Seperator->Separator.
 *
 *		Revision 1.4  1992/07/08  03:19:58  lewis
 *		Use Contains instead of PointInside (renamed).
 *
 *		Revision 1.3  1992/07/03  01:01:09  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:10:50  sterling
 *		Add DeleteAllMenuItems to DTOR for Menu. Rename CalcDefaultSize to CalcDefaultSize_.
 *		AddButtonMenuItem now takes Toggle not Button (should change names!).
 *
 *		Revision 1.81  92/05/18  16:08:06  16:08:06  lewis (Lewis Pringle)
 *		Changed osMenuInfo to osMenu.
 *		
 *		Revision 1.80  92/04/24  09:00:32  09:00:32  lewis (Lewis Pringle)
 *		Misc futzing with realize, etc, under motif.
 *		
 *		Revision 1.79  92/04/20  14:25:11  14:25:11  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *		
 *		Revision 1.78  92/04/17  19:52:33  19:52:33  lewis (Lewis Pringle)
 *		Hack for popups for motif.
 *		
 *		Revision 1.77  92/04/17  13:37:11  13:37:11  sterling (Sterling Wight)
 *		made setdefaultname also set current name
 *		
 *		Revision 1.76  92/04/15  17:34:16  17:34:16  lewis (Lewis Pringle)
 *		When removing a Menu Item, check if it is currently selected, and deselect it - Should this have
 *		been an assertion failure instead???
 *		
 *		Revision 1.75  92/04/15  13:57:51  13:57:51  lewis (Lewis Pringle)
 *		Added overloaded Menu::AddCheckBoxMenuItem () to take string argument, similar to the StringMenuItem case.
 *		
 *		Revision 1.74  92/04/14  19:38:38  19:38:38  lewis (Lewis Pringle)
 *		Got rid of commented out calls to XmCreatePopupMenu. Documentation somewhat unclear, but I think we
 *		will never need it. And if we do, its easy enuf to put back. Also, added new require based on new docs
 *		in View.hh when Realize can get called.
 *		
 *		Revision 1.73  92/04/13  00:55:06  00:55:06  lewis (Lewis Pringle)
 *		Create PullDown menus always - instead of sometimes PopupMenus - used to check if parent was
 *		RowColumn.
 *		
 *		Revision 1.72  92/04/09  16:45:08  16:45:08  lewis (Lewis Pringle)
 *		Wrap calls to Flush/Data/Instruction caches on mac with checks of OSConfig to see if the traps were available. That was the
 *		only problem trying to get all the Stroika apps running on system 6/macplus.
 *		
 *		Revision 1.71  92/04/07  13:23:58  13:23:58  lewis (Lewis Pringle)
 *		Dont use Add - use Enter for the hash table.
 *		
 *		Revision 1.70  92/04/07  10:23:59  10:23:59  lewis (Lewis Pringle)
 *		Added hash table to mac command numbers to MenuItem*. This was in response to profiling on the mac. Very big performance win.
 *		
 *		Revision 1.69  92/04/02  17:42:15  17:42:15  lewis (Lewis Pringle)
 *		Added a couple of Requires that the menu being associated with the cascadeMenuItem is not already
 *		a subview someplace. That would like come up if it was accidentally added twice as a cascadeMenuItem menu.
 *		
 *		Revision 1.68  92/04/02  13:07:23  13:07:23  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.67  92/03/26  09:47:15  09:47:15  lewis (Lewis Pringle)
 *		Made GetVisibleArea () const method.
 *		Got rid of oldVisible arg to EffectiveVisibiltyChanged.
 *		
 *		Revision 1.65  1992/03/22  21:29:17  lewis
 *		Motif GetOSRep call now const, and also changed both mac and motif versions to be inline.
 *		Also, Menu::SetEnabled () now calls menuItem->SetEnabled (..,eNoUpdate) rather than ddefaulting to eDelayedUpdate.
 *
 *		Revision 1.64  1992/03/19  16:47:34  lewis
 *		Get rid of seperate BuildWidget () function for Menu. Expand inline in Realize.
 *
 *		Revision 1.62  1992/03/10  01:26:06  lewis
 *		Fix bug with ReorderMenuItems, and Add/RemoveMenuItems. They could not properly
 *		patch the fIndex field of menu items, since they would have had to patch
 *		all the succeeding ones. The reason we got away with this so long, is cuz all we ever did was append. So now, do
 *		the patch in BeginUpdate (), when were iterating over each guy anyhow.
 *
 *		Revision 1.61  1992/03/09  23:44:07  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.60  1992/03/06  02:31:44  lewis
 *		Had to add DeleteMenuItem(), DeleteAllMenuItems () in reaction to sterls change to View - not
 *		deleting subviews - probably for the best, but impact of change needs more thought with respect
 *		to menus (and related classes).
 *
 *		Revision 1.58  1992/03/05  18:31:26  lewis
 *		Workaround View bug in Menu override of View::GetVisibleArea () - must call inherited.
 *		Also, in Menu::~Menu () remember to call ::DisposeMenu (fOSMenu); on mac.
 *
 *		Revision 1.57  1992/03/02  22:48:05  lewis
 *		Moved SystemMenu to StandardMenus.cc
 *
 *		Revision 1.55  1992/02/28  22:23:03  lewis
 *		Get rid of GetParentPanel () override.
 *		Make View unrealize work properly when we have a nil widget (reacting to new defintion of how View::UnRealize () works).
 *
 *		Revision 1.54  1992/02/27  22:01:19  lewis
 *		rocs more portable to motif
 *		and also, to use view mechanism more since we want menus to be in views, and be views!!!
 *
 *		Revision 1.53  1992/02/17  05:12:10  lewis
 *		Add motif specific calls to set origin/size of menu- not done right nor at the right time, but needed
 *		some hack so native menus would draw at all (though they still draw in the wrong place!). This will
 *		be eventaully cleaned up as we cleanup menuowners to be views, and behave within the view framework.
 *
 *		Revision 1.52  1992/02/15  06:43:20  sterling
 *		elmiminated fOldActive
 *
 *		Revision 1.51  1992/02/15  00:34:50  lewis
 *		Put in some temporary hacks to work around fact that we are sometimes a subview and sometimes not (always
 *		should be!).
 *
 *		Revision 1.49  1992/02/14  03:10:03  lewis
 *		Lots of work on this class simplifying it, and clarifying semantics of the various
 *		methods. Cleaned up interface to make more portable. Got rid of unncessary
 *		params to virtual protected methods that were defined by MDEF messages. Now defined so they can be
 *		implemented with the same semantics under MOTIF - I Hope. Moved view magic
 *		out of them, and into MDEF itself that called the methods. Worked on notion
 *		of selection - though I think that still needs a little work - maybe.
 *
 *		Revision 1.48  1992/02/12  07:57:37  lewis
 *		Finally got rid of seperate menu implementation classes - all merged into one, and renamed to Menu (from AbstractMenu).
 *
 *		Revision 1.46  1992/02/11  01:10:53  lewis
 *		Got rid of a bunch of classes and merged functionality into AbstractMenu.
 *		Got custom menus working on mac again (at least partially).
 *
 *		Revision 1.44  1992/02/05  07:44:38  lewis
 *		Got rid of Menu_MotifUI_Custom and _Standard, and got rid of got rid of popup flag in Menu_MotifUI
 *		and otherwise better supported new view based realize stuff, and moved closer to having just 1
 *		menu class.
 *
 *		Revision 1.43  1992/02/04  22:49:52  lewis
 *		Working on realization stuff with menus - working towards getting them integrated into the View
 *		hiearchy.
 *
 *		Revision 1.42  1992/02/04  16:58:16  lewis
 *		Get rid of Menu_MacUI_Standard since trying to pair this file down, and it did nothing. Added
 *		CanUseNativeMDEF() procedure to AbstractMenu for MacToolkit so we can tell later if we can use the
 *		native mdef or not.
 *
 *		Revision 1.41  1992/02/04  06:00:58  lewis
 *		Did addsubview on add of menuitems, and remove, and reorder since abstractmenu is now a
 *		view.
 *
 *		Revision 1.40  1992/02/03  23:33:54  lewis
 *		Return abstract classes froma adder methods, and now include various types of menu items files (used to just
 *		all be in MenuItem.hh).
 *
 *		Revision 1.39  1992/02/03  22:25:07  lewis
 *		General cleanups.
 *
 *		Revision 1.38  1992/01/31  18:06:11  sterling
 *		added MenuOwner include
 *
 *		Revision 1.37  1992/01/31  16:45:01  sterling
 *		added SetUpTheMenus call the AddMenuItem -- temp hack
 *
 *		Revision 1.36  1992/01/31  16:30:19  lewis
 *		Get rid of unneeded includes.
 *
 *		Revision 1.34  1992/01/23  20:04:29  sterling
 *		support for PopUps in Motif
 *
 *		Revision 1.26  1991/12/27  19:16:36  lewis
 *		merged in sterls changes.
 *
 *		Revision 1.24  1991/12/18  18:27:19  lewis
 *		Fixed minor motif bugs with XtSetArg
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Memory.h>				// for HLock
#include	<Menus.h>
#include	<Resources.h>			// to snag default mdef
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
#undef	Min
#undef	Max
#endif
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"OSConfiguration.hh"
#include	"StreamUtils.hh"

#include	"Application.hh"
#include	"ButtonMenuItem.hh"
#include	"CascadeMenuItem.hh"
#include	"CheckBoxMenuItem.hh"
#include	"DeskTop.hh"
#include	"MenuOwner.hh"
#include	"SeparatorMenuItem.hh"
#include	"StringMenuItem.hh"

#include	"Menu.hh"





#if		!qRealTemplatesAvailable
Implement (HashTable, MenuItemPtr);
#endif






#if 	qMacToolkit
/*
 ********************************************************************************
 ****************************** MENUPROC STUFF **********************************
 ********************************************************************************
 */



/*
 * Format of the MDEF resource that we create.
 */
struct	_PrivateMDEFRecord {
	/*
	 * Note the trick here is that we layout the structure as sequence of instructions
	 * (really one JMP instruction).  Format is short instruction prefix, followed by
	 * proc address.  Rest of stuff after that, is just handle back to proper instance.
	 */
	short			JMP;				// JMP instruction
										// TO OUR STATIC MEMBER
	pascal	void	(*defProc) (short, struct osMenu**, struct osRect*, long, short*);
	Menu*	itsMenu;			// Back pointer to our object
};


/*
 * a Nil menuProc that is used to inhibit re-calculating the menu's size after each
 * call to EnableItem, CheckItem, etc.
 */
static	pascal	void	NilMenuProc (short, osMenu** aMenuHandle, osRect*, long, short*)
{
	// flag so we can tell need to recalcmenusize
	(*aMenuHandle)->menuWidth = 0;
}

static	osHandle	mkNilMenuProc ()
	{
		struct	JmpRecord {
			/*
			 * Note the trick here is that we layout the structure as sequence of instructions
			 * (really one JMP instruction).  Format is short instruction prefix, followed by
			 * proc address.  Rest of stuff after that, is just handle back to proper instance.
			 */
			short			JMP;				// JMP instruction
												// TO OUR STATIC MEMBER
			pascal	void	(*defProc) (short, struct osMenu**, struct osRect*, long, short*);
		};

		JmpRecord**	menuProc	=	(JmpRecord**)::NewHandle (sizeof (JmpRecord));
		(*menuProc)->JMP		=	0x4ef9;		// 68000 JMP instruction
		(*menuProc)->defProc	=	&NilMenuProc;

		// not sure MachineSupports32BitMode is right test, but close enuf...
		if (OSConfiguration ().MachineSupports32BitMode ()) {
			/*
			 * Be sure we dont get stale instructions on '040...
			 * These would seem to be OK to call on any machine since they are not traps, hopefully they
			 * check availabilty of priv instructions first..???
			 */
			::FlushInstructionCache (); 
			::FlushDataCache (); 
		}

		return (osHandle (menuProc));
	}

static	osHandle	pHNilMenuProc = mkNilMenuProc ();	//	Handle to Nil menu proc
#endif	/*qMacToolkit*/








/*
 ********************************************************************************
 **************************************** Menu **********************************
 ********************************************************************************
 */
#if		qMacToolkit
short	Menu::sLastMenuID = 1;
#endif



// Hash and compare functions for our hash table of menu items indexed by command number...
static	UInt32 MyHashFunction (const MenuItem*& menuItem)
{
	RequireNotNil (menuItem);
	return (menuItem->GetCommandNumber ());
}

Menu::Menu ():
	fCurrentlySelected (Nil),
	fItems (),
	fItemsHashTable (MyHashFunction),
#if		qMacToolkit
	fOSMenu (Nil),
	fSavedDefProc (Nil),
	fRealDefProc (Nil)
#elif	qXtToolkit
	fWidget (Nil)
#endif
{
#if		qMacToolkit
	fOSMenu = ::NewMenu (sLastMenuID++, "\p");
	fRealDefProc = (*fOSMenu)->menuProc;
	PatchMenuProc ();
#endif
}

Menu::~Menu ()
{
	DeleteAllMenuItems ();

#if		qXtToolkit
	Assert (fWidget == Nil);
#endif
#if		qMacToolkit
	if (fOSMenu != Nil) {
		::DisposeMenu (fOSMenu);
	}
#endif
}
		
AbstractStringMenuItem*		Menu::AddStringMenuItem (CommandNumber commandNumber)
{
	AbstractStringMenuItem*	menuItem = new StringMenuItem (commandNumber);
	AddMenuItem (menuItem);
	return (menuItem);
}

AbstractStringMenuItem*		Menu::AddStringMenuItem (CommandNumber commandNumber, Boolean extendedName)
{
	AbstractStringMenuItem*	menuItem = new StringMenuItem (commandNumber);
	menuItem->SetExtended (extendedName);
	AddMenuItem (menuItem);
	return (menuItem);
}

AbstractStringMenuItem*		Menu::AddStringMenuItem (CommandNumber commandNumber, const String& title)
{
	AbstractStringMenuItem*	menuItem = new StringMenuItem (commandNumber);
	menuItem->SetDefaultName (title);
	menuItem->SetName (title);
	AddMenuItem (menuItem);
	return (menuItem);
}

AbstractCheckBoxMenuItem*	Menu::AddCheckBoxMenuItem (CommandNumber commandNumber)
{
	AbstractCheckBoxMenuItem*	menuItem = new CheckBoxMenuItem (commandNumber);
	AddMenuItem (menuItem);
	return (menuItem);
}

AbstractCheckBoxMenuItem*		Menu::AddCheckBoxMenuItem (CommandNumber commandNumber, const String& title)
{
	AbstractCheckBoxMenuItem*	menuItem = new CheckBoxMenuItem (commandNumber);
	menuItem->SetDefaultName (title);
	AddMenuItem (menuItem);
	return (menuItem);
}

AbstractCascadeMenuItem*	Menu::AddCascadeMenuItem (Menu* cascadeMenu, const String& title)
{
	RequireNotNil (cascadeMenu);
	Require (cascadeMenu->GetParentView () == Nil);		// not already installed someplaces...
	AbstractCascadeMenuItem*	menuItem = new CascadeMenuItem (cascadeMenu);
	menuItem->SetDefaultName (title);
	AddMenuItem (menuItem);
	return (menuItem);
}

void	Menu::AddSeparatorMenuItem ()
{
	AddMenuItem (new SeparatorMenuItem ());
}

AbstractButtonMenuItem*		Menu::AddButtonMenuItem (Toggle* button, CommandNumber commandNumber)
{
	AbstractButtonMenuItem*	item = new ButtonMenuItem (button, commandNumber);
	AddMenuItem (item);
	return (item);
}

CollectionSize	Menu::GetLength () const
{
	return (fItems.GetLength ());
}

CollectionSize	Menu::GetMenuItemIndex (MenuItem* menuItem)
{
	return (fItems.IndexOf (menuItem));
}

MenuItem*	Menu::GetMenuItemByIndex (CollectionSize index)
{
	return (fItems[index]);
}

MenuItem*	Menu::GetMenuItemByCommand (CommandNumber commandNumber)
{
	/*
	 * Use a hash table keyed by command number to make this lookup fast.
	 */
	for (register HashTableElement(MenuItemPtr)* curT = fItemsHashTable.GetIthTableHead ((commandNumber	% fItemsHashTable.GetTableSize ())+1);
		 curT != Nil; curT = curT->fNext) {
		AssertNotNil (curT);
		AssertNotNil (curT->fElement);
		if (curT->fElement->GetCommandNumber () == commandNumber) {
			return (curT->fElement);
		}
	}
	return (Nil);
}

void	Menu::DeleteMenuItem (MenuItem* menuItem)
{
	RequireNotNil (menuItem);
	RemoveMenuItem (menuItem);
	delete (menuItem);
}

void	Menu::DeleteAllMenuItems ()
{
	ForEach (MenuItemPtr, it, MakeMenuItemIterator (eSequenceBackward)) {
		DeleteMenuItem (it.Current ());
	}
}

Boolean	Menu::GetEnabled () const
{
	ForEach (MenuItemPtr, it, fItems) {
		if (it.Current ()->GetEnabled ()) {
			return (True);
		}
	}
	return (False);
}

void	Menu::SetEnabled (CommandNumber commandNumber, Boolean enabled)
{
	/*
	 * Use a hash table keyed by command number to make this lookup fast.
	 */
	for (register HashTableElement(MenuItemPtr)* curT =
		 fItemsHashTable.GetIthTableHead ((commandNumber	% fItemsHashTable.GetTableSize ())+1);
		 curT != Nil; curT = curT->fNext) {
		AssertNotNil (curT);
		AssertNotNil (curT->fElement);
		if (curT->fElement->GetCommandNumber () == commandNumber) {
			curT->fElement->SetEnabled (enabled, eNoUpdate);
		}
	}
}

void	Menu::SetName (CommandNumber commandNumber, const String& name)
{
	ForEach (MenuItemPtr, it, fItems) {
		if (it.Current ()->GetCommandNumber () == commandNumber) {
			it.Current ()->SetName (name);
		}
	}
}

void	Menu::SetOn (CommandNumber commandNumber, Boolean on)
{
	ForEach (MenuItemPtr, it, fItems) {
		if (it.Current ()->GetCommandNumber () == commandNumber) {
			it.Current ()->SetOn (on);
		}
	}
}
		 		
#if		qSupportMneumonics		
void	Menu::SetMneumonic (CommandNumber commandNumber, const String& mneumonic)
{
	ForEach (MenuItemPtr, it, fItems) {
		if (it.Current ()->GetCommandNumber () == commandNumber) {
			it.Current ()->SetMneumonic (mneumonic);
		}
	}
}
#endif	/*qSupportMneumonics*/	
		
void	Menu::BeginUpdate ()
{
#if		qMacToolkit
	fSavedDefProc = (*GetOSRepresentation ())->menuProc;

	// blast menu def proc, for speed - hack from macapp 2.0b9
	// patch back at the endUpdate...
	(*GetOSRepresentation ())->menuProc = pHNilMenuProc;
#endif
	fOldEnabled = GetEnabled ();
	CollectionSize index = 1;
	ForEach (MenuItemPtr, it, fItems) {
		MenuItem*	menuItem	=	it.Current ();
		AssertNotNil (menuItem);

		// patch indexes here, rather than in Add/Remove/ReorderMenuItem, since we must also patch all the ones
		// past the one we've changes, and easier to do here...
		// Some question of whether its safe to wait til now???? LGP Mar 9, 1992
		menuItem->fIndex = index;
		index++;

		menuItem->Reset ();
	}
}

Boolean	Menu::EndUpdate ()
{
// not quite sure when the best time to do this is, but now seems reasonable...
ProcessLayout ();

	ForEach (MenuItemPtr, it, fItems) {
		if (it.Current ()->GetDirty ()) {
			it.Current ()->UpdateOSRep ();
		}
	}
	Boolean changed = Boolean (GetEnabled () != fOldEnabled);

// Note: On Xt we must do something similar here to set the menu title itself to be enabled/disabled???
// Maybe this code to munge the enableFlags should go into the menu title stuff/?
#if		qMacToolkit
	if (changed) {
		if (GetEnabled ()) {
			(*GetOSRepresentation ())->enableFlags |= 0x1;
		}
		else {
			(*GetOSRepresentation ())->enableFlags &= ~0x1;
		}
	}
#endif	/*qMacToolkit*/


// per haps under Xt we should do something similar - call SetSize (CalcDefaultSize???).
#if		qMacToolkit
	/*
	 * Blast back saved menuDefProc, and recalc menu size.
	 */
	(*GetOSRepresentation ())->menuProc = fSavedDefProc;
	::CalcMenuSize (GetOSRepresentation ());
#endif	/*qMacToolkit*/

#if		qXmToolkit
	SetOrigin (Point (GetOSRepresentation ()->core.y, GetOSRepresentation ()->core.x), eNoUpdate);
	SetSize (CalcDefaultSize (GetSize ()), eNoUpdate);
#endif

	return (changed);
}

void	Menu::AddMenuItem (MenuItem* menuItem, CollectionSize index)
{
	RequireNotNil (menuItem);
	if (index == kBadSequenceIndex) {
		index = GetLength () + 1;
	}
	AddMenuItem_ (menuItem, index);
}

void	Menu::AddMenuItem (MenuItem* menuItem, MenuItem* neighbor, AddMode addMode)	
{
	RequireNotNil (menuItem);
	RequireNotNil (neighbor);

	CollectionSize	index = GetMenuItemIndex (neighbor);
	Require (index != kBadSequenceIndex);
	AddMenuItem_ (menuItem, (addMode == eAppend) ? index + 1 : index);
}

void	Menu::RemoveMenuItem (MenuItem* menuItem)		
{
	RequireNotNil (menuItem);
	Require (GetMenuItemIndex (menuItem) != kBadSequenceIndex);
	RemoveMenuItem_ (menuItem);
}

void	Menu::RemoveAllMenuItems ()
{
	ForEach (MenuItemPtr, it, MakeMenuItemIterator (eSequenceBackward)) {
		RemoveMenuItem (it.Current ());
	}
	Ensure (GetLength () == 0);
}

void	Menu::ReorderMenuItem (MenuItem* menuItem, CollectionSize index)
{
	RequireNotNil (menuItem);
	Require (index != kBadSequenceIndex);
	Require (index <= GetLength ());
	
	CollectionSize	oldIndex = GetMenuItemIndex (menuItem);
	Require (oldIndex != kBadSequenceIndex);
	if (oldIndex != index) {
		ReorderMenuItem_ (menuItem, index);
	}
}
		
void	Menu::ReorderMenuItem (MenuItem* menuItem, MenuItem* neighbor, AddMode addMode)
{
	RequireNotNil (menuItem);
	RequireNotNil (neighbor);

	CollectionSize	index = GetMenuItemIndex (neighbor);
	Require (index != kBadSequenceIndex);
	
	CollectionSize	oldIndex = GetMenuItemIndex (menuItem);
	Require (oldIndex != kBadSequenceIndex);
	if (oldIndex != index) {
		ReorderMenuItem_ (menuItem, (addMode == eAppend) ? index + 1 : index);
	}
}

void	Menu::AddMenuItem_ (MenuItem* menuItem, CollectionSize index)
{
	RequireNotNil (menuItem);

	// should do this much better, based on whether of not am installed, also done
	// for removes, reorders, default name changes of menuitems, etc.
	MenuOwner::SetMenusOutOfDate ();	

	fItems.InsertAt (menuItem, index);
	fItemsHashTable.Enter (menuItem);
	Assert (menuItem->fOwner == Nil);		// not currently installed in another menu, I hope...
	menuItem->fOwner = this;
	AddSubView (menuItem, index);
#if		qMacToolkit
	::InsMenuItem (GetOSRepresentation (), "\pMissing Name", short (index-1));
#endif
}

void	Menu::RemoveMenuItem_ (MenuItem* menuItem)
{
	RequireNotNil (menuItem);
	Require (menuItem->fOwner == this);

	// *should this be an assertion error, or just change selection?
	if (fCurrentlySelected == menuItem) {
		SelectItem (Nil, eNoUpdate);
	}

#if		qMacToolkit
	CollectionSize	index = GetMenuItemIndex (menuItem);
	::DelMenuItem (GetOSRepresentation (), short (index));
#endif
	RemoveSubView (menuItem);
	menuItem->fOwner = Nil;
	menuItem->fIndex = kBadSequenceIndex;
	fItems.Remove (menuItem);

	/*
	 * While we could remove items more efficiently, the current hash table API doesnt give us a good way.
	 * The obvious thing of Remove () doesn't quite do what we want since it will remove the first entry in the
	 * table with the same command number. Also, it takes a MenuItem*, not a CommandNumber. This is not quite what we want.
	 * This will do for now.
	 */
	fItemsHashTable.Clear ();
	ForEach (MenuItemPtr, it, fItems) {
		fItemsHashTable.Enter (it.Current ());
	}
}

void	Menu::ReorderMenuItem_ (MenuItem* menuItem, CollectionSize index)
{
	RequireNotNil (menuItem);
	Require (menuItem->fOwner == this);

	// should do this much better, based on whether of not am installed, also done
	// for removes, reorders, default name changes of menuitems, etc.
	MenuOwner::SetMenusOutOfDate ();	

	// no need to do this since all items look alike anyhow - we re-set any interesting info in
	// updateOSRep ()
	fItems.Remove (menuItem);
	fItems.InsertAt (menuItem, index);
	ReorderSubView (menuItem, index);
}
		
#if		qMacToolkit
Boolean		Menu::CanUseNativeMDEF ()
{
	ForEach (MenuItemPtr, it, fItems) {
		if (not it.Current ()->IsNativeItem ()) {
			return (False);
		}
	}
	return (True);		// if each item is native, we can use native MDEF, by default...
}
#endif

#if		qXtToolkit
osWidget*	Menu::GetWidget () const
{
	return (fWidget);
}

void	Menu::Realize (osWidget* parent)
{
	RequireNotNil (parent);
	Require (fWidget == Nil or XtParent (fWidget) == parent);

//	if (fWidget != Nil) {
#if		qXmToolkit
extern Boolean gDoPopup;
//extern	Arg gPopupArgs[1];
//extern	int gPopupArgCount;

		if (gDoPopup) {
//gDebugStream << "creating popup with argcount = " << gPopupArgCount << newline;
//		fWidget = ::XmCreatePopupMenu (parent, "Popup Menu", gPopupArgs, gPopupArgCount);
			fWidget = ::XmCreatePopupMenu (parent, "Popup Menu", Nil, 0);
		}
		else {
			fWidget = ::XmCreatePulldownMenu (parent, "Pulldown Menu", Nil, 0);
		}
#endif

		AssertNotNil (fWidget);
		View::Realize (fWidget);
//	}
}

void	Menu::UnRealize ()
{
	View::UnRealize ();
	if (fWidget != Nil) {
		::XtDestroyWidget (fWidget);
		fWidget = Nil;
	}
}
#endif

#if		qMacToolkit
void	Menu::PatchMenuProc ()
{
	/*
	 * NB: dont call any menu functions for this menu til this instance is fully constructed,
	 * since otherwise the wrong virtual functions may be called.  For example, the documentation
	 * (IM 1-362) recomends calling ::CalcMenuSize () after changing the mdef proc, but that
	 * would be unwise, since our current binding for that function is a pure_virtual.
	 *
	 * Also, note that we dont bother HLocking this guy since we never do anything to move memory while
	 * running in it, and never do a JSR in it, so our return address are never pointing internal to it.
	 */
	fMDEFRecord = (_PrivateMDEFRecord**)::NewHandle (sizeof (_PrivateMDEFRecord));
	(*fMDEFRecord)->JMP = 0x4ef9;			// 68000 JMP instruction
	(*fMDEFRecord)->defProc = &MDEFProc;
	(*fMDEFRecord)->itsMenu = this;
	(*GetOSRepresentation ())->menuProc = osHandle (fMDEFRecord);

	// not sure MachineSupports32BitMode is right test, but close enuf...
	if (OSConfiguration ().MachineSupports32BitMode ()) {
		/*
		 * Be sure we dont get stale instructions on '040...
		 * These would seem to be OK to call on any machine since they are not traps, hopefully they
		 * check availabilty of priv instructions first..???
		 */
		::FlushInstructionCache (); 
		::FlushDataCache (); 
	}
}

void	Menu::PlacePopup (const Point& /*hitPt*/, CollectionSize /*whichItem*/, Rect* popupRectangle, CollectionSize* topOfMenu)
{
AssertNotReached ();		// not right - see IM V-248...
	*popupRectangle = kZeroRect;
	*topOfMenu = 1;
}

void	Menu::ProcessMessage (short theMessage, osMenu** theOSMenu, osRect* menuRect, long hitPt, short* whichItem)
{
	if (CanUseNativeMDEF ()) {
		/*
		 * Not clear if it is necessary to lock down apples mdef - not for anything we do, certainly, but
		 * they may do something internally that moves memory, and may have counted on someone somewhere
		 * along the way HLocking this guy?
		 */
		RequireNotNil (fRealDefProc);
		char		oldState	=	::HGetState (fRealDefProc);		
		::HLock (fRealDefProc);
		AssertNotNil (*fRealDefProc);
		((pascal void (*) (short, osMenu**, osRect*, long, short*)) (*fRealDefProc)) (theMessage, theOSMenu, menuRect, hitPt, whichItem);
		::HSetState (fRealDefProc, oldState);
	}
	else {
		switch (theMessage) {
			case mDrawMsg: {

// is it right to set the origin here, and the tablet, should we save and restore or assert they are right????

// maybe just override GetTablet to return differnt one when we are drawn from here???

				osGrafPort*	savedPort	=	Tablet::GetCurrentPort ();
				SetOrigin (os_cvt (*menuRect).GetOrigin (), eNoUpdate);
				SetTablet (DeskTop::Get ().GetTablet ());
				Assert (os_cvt (*menuRect).GetSize () == GetSize ());
				/*
				 * This processlayout call is a bit funny here, but the reason is that the above
				 * SetTablet call invalidates the views layout (and all its subviews).  So alas, we must
				 * re-lay them out.  This can be inefficent, but I can think of no clean solution.
				 * In practice, displayed views can override the EffectiveFontChanged method to make
				 * SetTablet calls inexpensive.
				 */
				ProcessLayout ();
				Render (GetVisibleArea ());
				Tablet::xDoSetPort (savedPort);
			}
			break;
			case mChooseMsg: {
				osGrafPort*	savedPort	=	Tablet::GetCurrentPort ();
				SetOrigin (os_cvt (*menuRect).GetOrigin (), eNoUpdate);
				SetTablet (DeskTop::Get ().GetTablet ());
				Assert (os_cvt (*menuRect).GetSize () == GetSize ());
				Point			theHitPoint		=	os_cvt (*((osPoint*)&hitPt));
				CollectionSize	myWhichItem 	=	*whichItem;
				myWhichItem = ChooseItem (AncestorToLocal (theHitPoint, &DeskTop::Get ()), myWhichItem);
				Assert ((short)myWhichItem == myWhichItem);		// assure no roundoff error
				*whichItem = (short)myWhichItem;
				Tablet::xDoSetPort (savedPort);
			}
			break;
			case mSizeMsg: {
				osGrafPort*	savedPort	=	Tablet::GetCurrentPort ();
				SetTablet (DeskTop::Get ().GetTablet ());
				Point	size =	CalcDefaultSize (GetSize ());
				Ensure (size.GetV () >= 0);
				Ensure (size.GetH () >= 0);
				Assert ((short)size.GetH () == size.GetH ());		// assure no roundoff error
				Assert ((short)size.GetV () == size.GetV ());		// assure no roundoff error
				(*GetOSRepresentation ())->menuWidth = (short)size.GetH ();
				(*GetOSRepresentation ())->menuHeight = (short)size.GetV ();
				SetSize (size, eNoUpdate);					// perhpas the setting of osMenu width and heigfht should be done in override of SetSize_???
				Tablet::xDoSetPort (savedPort);
			}
			break;
			case mPopUpMsg: {
				RequireNotNil (whichItem);
				RequireNotNil (menuRect);
				osGrafPort*	savedPort	=	Tablet::GetCurrentPort ();
				SetOrigin (os_cvt (*menuRect).GetOrigin (), eNoUpdate);
				SetTablet (DeskTop::Get ().GetTablet ());
				Assert (os_cvt (*menuRect).GetSize () == GetSize ());
				Point			theHitPoint		=	os_cvt (*((osPoint*)&hitPt));
				CollectionSize	myWhichItem 	=	*whichItem;
				Rect			theMenuRect		=	os_cvt (*menuRect);
				CollectionSize	topOfMenu		=	0;
				PlacePopup (theHitPoint, myWhichItem, &theMenuRect, &topOfMenu);
				Assert (topOfMenu == (short)topOfMenu);	// no roundoff
				RequireNotNil (whichItem);
				RequireNotNil (menuRect);
				*whichItem = topOfMenu;
				os_cvt (theMenuRect, *menuRect);
				Tablet::xDoSetPort (savedPort);
			}
			break;
			default: {
#if		qDebug
				gDebugStream << "Got new unknown message in MDEF: " << int (theMessage) << newline;
#endif
				// See above comment about doing HLock here...
				RequireNotNil (fRealDefProc);
				char		oldState	=	::HGetState (fRealDefProc);		
				::HLock (fRealDefProc);
				AssertNotNil (*fRealDefProc);
				((pascal void (*) (short, struct osMenu**, osRect*, long, short*)) (*fRealDefProc)) (theMessage, theOSMenu, menuRect, hitPt, whichItem);
				::HSetState (fRealDefProc, oldState);
			}
			break;
		}
	}
}

pascal	void	Menu::MDEFProc (short theMessage, struct osMenu** theOSMenu, struct osRect* menuRect, long hitPt, short* whichItem)
{
	/*
	 * Believe it or not, apple sometimes calls this routine with nil pointers!
	 *
	 * Arg!!! Worse than that - they call it with either uninitialized pointers, or not enuf args,
	 * cuz on draw messages, whichItem sometimes points into lala land. So we can not simply check
	 * if Nil, we must check the message kind to see if its safe to dereference!!!
	 *
	 * This poses problems for extensibility - if apple adds a new event to their protocol, we cannot process it in
	 * our typesafe method since we dont know if they are good pointers. For that reason, we only do the type
	 * conversion in the switch statement in our virtual function, since there we know which ones should
	 * be valid.
	 */
	RequireNotNil (theOSMenu);
	_PrivateMDEFRecord**	theMDEF		=	(_PrivateMDEFRecord**) (*theOSMenu)->menuProc;
	RequireNotNil (theMDEF);
	Menu*					theMenu		=	(*theMDEF)->itsMenu;
	AssertNotNil (theMenu);
	Assert ((*theMenu->GetOSRepresentation ())->menuProc == (osHandle)theMDEF);		// be sure its ours, and all appears in sync...

	theMenu->ProcessMessage (theMessage, theOSMenu, menuRect, hitPt, whichItem);
}
#endif	/*qMacToolkit*/

Point	Menu::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	/*
	 * This CalcDefaultSize () assumes all of its buttons are aranged in a single column, and all
	 * have the same width (the max of each of their default widths) and each have a height that
	 * they individually define.
	 */
	Point	size	 = kZeroPoint;
	ForEach (MenuItemPtr, it, fItems) {
		MenuItem* button	=	it.Current ();
		AssertNotNil (button);
		Point	s	=	button->CalcDefaultSize (button->GetSize ());
		if (s.GetH () > size.GetH ()) {
			size.SetH (s.GetH ());
		}
		size += Point (s.GetV (), 0);
	}
	return (size);
}

CollectionSize	Menu::ChooseItem (const Point& hitPt, CollectionSize whichItem)
{
Assert (GetLive ());
Assert (GetEffectiveLive ());

	ForEach (MenuItemPtr, it, fItems) {
		register MenuItem* button	=	it.Current ();
		AssertNotNil (button);
		if (button->Contains (hitPt) and button->GetLive ()) {
			SelectItem (button, eImmediateUpdate);
			Ensure (fCurrentlySelected == button);
			return (GetMenuItemIndex (fCurrentlySelected));
		}
	}
	SelectItem (Nil, eImmediateUpdate);		// nothing selected if we got here...
	return (0);
}

Boolean	Menu::TrackPress (const MousePressInfo& mouseInfo)
{
	/*
	 * Button tracking does not behave properly by default, and so we don't let them handle the
	 * TrackPress () message.
	 */
	CollectionSize	currentItem	=	0;		// By default nothing selected when we start tracking from a press
											// Subclasses like pallets can override this behavior
	(void)ChooseItem (mouseInfo.fPressAt, currentItem);
	return (True);	
}

void	Menu::SelectItem (MenuItem* item, Panel::UpdateMode updateMode)
{
	if (fCurrentlySelected != item) {
		if (fCurrentlySelected != Nil) {
			fCurrentlySelected->SetOn (False, updateMode);
		}
		fCurrentlySelected = item;
		if (fCurrentlySelected != Nil) {
			fCurrentlySelected->SetOn (True, updateMode);
		}
	}
	Ensure (fCurrentlySelected == item);
}

void	Menu::Layout ()
{
	/*
	 * By default, lay menu items out one on top of the other, setting their size to its default (not sure thats a good idea).
	 */
	Point	itemOrigin	=	kZeroPoint;
	ForEach (MenuItemPtr, it, fItems) {
		register MenuItem* button	=	it.Current ();
		AssertNotNil (button);
		button->SetOrigin (itemOrigin);
		button->SetSize (button->CalcDefaultSize (button->GetSize ()));
		itemOrigin += Point (button->GetSize ().GetV (), 0);
	}
	View::Layout ();
}

Region	Menu::GetVisibleArea () const
{
	/*
	 * I think we must call this to work around bug in view - trouble is that they count of setting
	 * fCurVisibleArea as a sort of flag - not really designed to support further subclassing
	 * and overrides of GetVisibleArea () - admittedly, a somewhat funny thing to do
	 * but if its not allowed, we must document, and find an alternative for what I'm doing
	 * here....
	 */
	// Workaround to a workaround!!! Should cast to (void) return value but for MPW Compiler bug - ... LGP March 4, 1992
	View::GetVisibleArea ();

	// never clipped by siblings, parents, or whatever....
	if (GetEffectiveVisibility ()) {
		return (GetLocalExtent ());
	}
	else {
		return (kEmptyRegion);
	}
}

void	Menu::EffectiveVisibilityChanged (Boolean newVisible, Panel::UpdateMode updateMode)
{
	// not well thgouht out if we should call arm/disarm before or after View::EffectiveVisibilityChanged!!!
	if (newVisible) {
		Armed ();
	}
	else {
		Disarmed ();
	}
	View::EffectiveVisibilityChanged (newVisible, updateMode);
}

void	Menu::Armed ()
{
}

void	Menu::Disarmed ()
{
}

Boolean	Menu::GetEffectiveLive () const
{
	return (GetLive ());
}









// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

