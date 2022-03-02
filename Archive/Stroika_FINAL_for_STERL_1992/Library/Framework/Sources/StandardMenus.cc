/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/StandardMenus.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *			Move code to hack MOTIF shell here for about box system menu item.....
 *
 * Changes:
 *	$Log: StandardMenus.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  21:29:31  sterling
 *		Use Bag instead of obsolete BagPtr.
 *
 *		Revision 1.3  1992/07/14  19:59:26  lewis
 *		Rename AddSeperatorMenuItem->AddSeparatorMenuItem.
 *
 *		Revision 1.2  1992/07/02  05:00:55  lewis
 *		Renamed Bag_DoublyLLOfPointers->BagPtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.8  1992/04/16  13:07:08  lewis
 *		Dont put print stuff in default file menu for motif - just temporarily til we support it.
 *
 *		Revision 1.6  1992/03/26  17:21:50  sterling
 *		added FrameworkCommandNamesBuilder
 *
 *		Revision 1.4  1992/03/06  02:36:32  lewis
 *		Added dtor to call DeleteMenuItems () - not really a very safe change, but all thats practical
 *		given sterls new change to View - not deleting subviews, and assuming removed from parent before
 *		thier deletion. I think this change is good, but must study its impact better.
 *
 *		Revision 1.2  1992/03/03  03:53:29  lewis
 *		Added Default File/Edit Menus.
 *
 *		Revision 1.1  1992/03/02  22:52:02  lewis
 *		Initial revision
 *
 *
 */


#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Menus.h>
#endif	/*qMacToolkit*/
#include	"OSRenamePost.hh"

#include	"Bag.hh"
#include	"StreamUtils.hh"

#include	"MenuOwner.hh"
#include	"SeparatorMenuItem.hh"
#include	"StringMenuItem.hh"

#include	"StandardMenus.hh"



#if		!qRealTemplatesAvailable
	typedef	class	SystemMenu*	SysMenuPtr;
	
	Declare (Iterator, SysMenuPtr);
	Declare (Collection, SysMenuPtr);
	Declare (AbBag, SysMenuPtr);
	Declare (Array, SysMenuPtr);
	Declare (Bag_Array, SysMenuPtr);
	Declare (Bag, SysMenuPtr);

	Implement (Iterator, SysMenuPtr);
	Implement (Collection, SysMenuPtr);
	Implement (AbBag, SysMenuPtr);
	Implement (Array, SysMenuPtr);
	Implement (Bag_Array, SysMenuPtr);
	Implement (Bag, SysMenuPtr);
#endif

static	Bag(SysMenuPtr)	sSystemMenus;







/*
 ********************************************************************************
 ************************ FrameworkCommandNamesBuilder **************************
 ********************************************************************************
 */
FrameworkCommandNamesBuilder::FrameworkCommandNamesBuilder ()
{
	const	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, String (String::eReadOnly, "About Application")),
		CmdNameTablePair (CommandHandler::eUndo, String (String::eReadOnly, "Undo")),
		CmdNameTablePair (CommandHandler::eKeyPress, String (String::eReadOnly, "Typing")),
		CmdNameTablePair (CommandHandler::eCut, String (String::eReadOnly, "Cut")),
		CmdNameTablePair (CommandHandler::eCopy, String (String::eReadOnly, "Copy")),
		CmdNameTablePair (CommandHandler::ePaste, String (String::eReadOnly, "Paste")),
		CmdNameTablePair (CommandHandler::eClear, String (String::eReadOnly, "Clear")),
		CmdNameTablePair (CommandHandler::eSelectAll, String (String::eReadOnly, "Select All")),
		CmdNameTablePair (CommandHandler::eDuplicate, String (String::eReadOnly, "Duplicate")),
		CmdNameTablePair (CommandHandler::eToggleClipboardShown, String (String::eReadOnly, "Toggle Clipboard Shown")),
		CmdNameTablePair (CommandHandler::eQuit, String (String::eReadOnly, "Quit")),
		CmdNameTablePair (CommandHandler::ePrint, String (String::eReadOnly, "Print")),
		CmdNameTablePair (CommandHandler::ePrintOne, String (String::eReadOnly, "Print One")),
		CmdNameTablePair (CommandHandler::ePageSetup, String (String::eReadOnly, "Page Setup")),
		CmdNameTablePair (CommandHandler::eNew, String (String::eReadOnly, "New")),
		CmdNameTablePair (CommandHandler::eOpen, String (String::eReadOnly, "Open")),
		CmdNameTablePair (CommandHandler::eClose, String (String::eReadOnly, "Close")),
		CmdNameTablePair (CommandHandler::eSave, String (String::eReadOnly, "Save")),
		CmdNameTablePair (CommandHandler::eSaveAs, String (String::eReadOnly, "Save As")),
		CmdNameTablePair (CommandHandler::eRevertToSaved, String (String::eReadOnly, "Revert To Saved")),
	};

	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));

	MenuOwner::GetAccelerators ().Enter (CommandHandler::eUndo, String ("%Z"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eCut, String ("%X"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eCopy, String ("%C"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::ePaste, String ("%V"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eSelectAll, String ("%A"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eDuplicate, String ("%D"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eQuit, String ("%Q"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::ePrint, String ("%P"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eNew, String ("%N"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eOpen, String ("%O"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eClose, String ("%W"));
	MenuOwner::GetAccelerators ().Enter (CommandHandler::eSave, String ("%S"));
}








/*
 ********************************************************************************
 ********************************* SystemMenu ***********************************
 ********************************************************************************
 */
SystemMenu::SystemMenu ()
{
	sSystemMenus.Add (this);
#if		qMacToolkit
	AddStringMenuItem (CommandHandler::eAboutApplication, True);
	::AddResMenu (GetOSRepresentation (), 'DRVR');
#endif
}

SystemMenu::~SystemMenu ()
{
	sSystemMenus.Remove (this);
}

String	SystemMenu::DefaultName ()
{
#if		qMacToolkit
	const String kAppleMenuTitle	=	String (String::eReadOnly, "\024");		// special char in system font for apple
	return (kAppleMenuTitle);
#else
	return (kEmptyString);	// ???
#endif
}

#if		qMacToolkit
void	SystemMenu::EnableDeskAccessories (Boolean enable)
{
	ForEach (SysMenuPtr, it, sSystemMenus) {
		it.Current ()->EnableDAs (enable);
	}
}

void	SystemMenu::BeginUpdate ()
{
	EnableDeskAccessories (False);
	Menu::BeginUpdate ();
}

Boolean	SystemMenu::GetEnabled () const
{
	AssertNotNil (GetOSRepresentation ());
	return (Boolean ((*GetOSRepresentation ())->enableFlags & 0x1));
}

void	SystemMenu::EnableDAs (Boolean enable)
{
	AssertNotNil (GetOSRepresentation ());
	CollectionSize	ourItems = GetLength ();
	for (CollectionSize count = ::CountMItems (GetOSRepresentation ()); count > ourItems; count--) {
		if (enable) {
			::osEnableItem (GetOSRepresentation (), count);
		}
		else {
			::DisableItem (GetOSRepresentation (), count);
		}
	}
}
#endif	/*qMacToolkit*/







/*
 ********************************************************************************
 ***************************** DefaultFileMenu **********************************
 ********************************************************************************
 */
DefaultFileMenu::DefaultFileMenu ():
	   Menu ()
{
	AddStringMenuItem (CommandHandler::eNew);
	AddStringMenuItem (CommandHandler::eOpen, True);
	AddSeparatorMenuItem ();
	AddStringMenuItem (CommandHandler::eClose);
	AddStringMenuItem (CommandHandler::eSave);
	AddStringMenuItem (CommandHandler::eSaveAs, True);
	AddStringMenuItem (CommandHandler::eRevertToSaved, True);
	AddSeparatorMenuItem ();
#if		!qXmToolkit
	// Dont put these in the menu under Xm, since we dont support them yet...
	AddStringMenuItem (CommandHandler::ePageSetup, True);
	AddStringMenuItem (CommandHandler::ePrint, True);
	AddStringMenuItem (CommandHandler::ePrintOne);
	AddSeparatorMenuItem ();
#endif
	AddStringMenuItem (CommandHandler::eQuit);
}

DefaultFileMenu::~DefaultFileMenu ()
{
	DeleteAllMenuItems ();
}

String	DefaultFileMenu::DefaultName ()
{
	const String kMenuTitle	=	String (String::eReadOnly, "File");
	return (kMenuTitle);
}








/*
 ********************************************************************************
 ******************************* DefaultEditMenu ********************************
 ********************************************************************************
 */
DefaultEditMenu::DefaultEditMenu ():
	   Menu ()
{
	AddStringMenuItem (CommandHandler::eUndo);
	AddSeparatorMenuItem ();
	AddStringMenuItem (CommandHandler::eCut);
	AddStringMenuItem (CommandHandler::eCopy);
	AddStringMenuItem (CommandHandler::ePaste);
	AddStringMenuItem (CommandHandler::eClear);
	AddSeparatorMenuItem ();
	AddStringMenuItem (CommandHandler::eSelectAll);
	AddCheckBoxMenuItem (CommandHandler::eToggleClipboardShown);
}

DefaultEditMenu::~DefaultEditMenu ()
{
	DeleteAllMenuItems ();
}

String	DefaultEditMenu::DefaultName ()
{
	const String kMenuTitle	=	String (String::eReadOnly, "Edit");
	return (kMenuTitle);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

