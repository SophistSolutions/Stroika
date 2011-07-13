/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DebugMenu.cc,v 1.4 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: DebugMenu.cc,v $
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/07/14  19:51:33  lewis
 *		Call AddSeparatorMenuItem() instead of AddSeperatorMenuItem().
 *
 *		Revision 1.17  1992/03/06  02:37:05  lewis
 *		Call DeleteAllMenuItems from dtor - reaction to sterls change to View - no longer deleting subviews
 *		on destruction. I think this change is for the best, but we must study its consequences more carefully.
 *
 *		Revision 1.16  1992/03/05  18:23:18  lewis
 *		Allow for multiple debug menus, and get rid of base class MenuCommandHandler, and
 *		instead provide static members to provide the menusetup, and execution handling.
 *
 *		Revision 1.15  1992/02/14  03:08:04  lewis
 *		Add print region statictics command, and cleaned up other menu items.
 *
 *		Revision 1.14  1992/02/12  08:15:10  lewis
 *		Add menu item for toggle validate heap storage.
 *
 *		Revision 1.13  1992/02/12  06:50:17  lewis
 *		Added eToggleValidateHeapStorageOnAllocs, and switch to Menu, from AbstractMenu.
 *
 *		Revision 1.12  1992/02/04  17:05:44  lewis
 *		No longer reference Menu_MacUI_Standard.
 *
 *		Revision 1.10  1992/01/10  03:21:19  lewis
 *		Moved CommandNameTablePairs here from Command.cc (the onces specific to this menu), and
 *		added one for XGDI - ToggleSyncMode.
 *
 *
 *
 *
 */



#include	"Memory.hh"

#include	"Region.hh"
#include	"Tablet.hh"

#include	"MenuCommandHandler.hh"

#include	"DebugMenu.hh"








#if		qDebug

/*
 ********************************************************************************
 ************************************ DebugMenu *********************************
 ********************************************************************************
 */

DebugMenu::DebugMenu ():
	Menu ()
{
#if		!qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG
	static	const
#endif
	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (ePrintMemoryStatistics, String (String::eReadOnly, "Print Memory Statistics")),
		CmdNameTablePair (eValidateHeapStorage, String (String::eReadOnly, "Validate Heap Storage")),
		CmdNameTablePair (eToggleValidateHeapStorageOnAllocs, String (String::eReadOnly, "Validate Heap Storage On Allocs")),
		CmdNameTablePair (eTogglePrintIncomingEvents, String (String::eReadOnly, "Print Incoming Events")),
#if		qKeepRegionAllocStatistics
		CmdNameTablePair (ePrintRegionStatistics, String (String::eReadOnly, "Print Region Statistics")),
#endif
#if		qXGDI 
		CmdNameTablePair (eToggleSynchronizedMode, String (String::eReadOnly, "Syncronize Server Communications")),
#endif
	};

	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));

	AddStringMenuItem (ePrintMemoryStatistics, True);
	AddStringMenuItem (eValidateHeapStorage, True);
	AddCheckBoxMenuItem (eToggleValidateHeapStorageOnAllocs);
	AddSeparatorMenuItem ();
	AddCheckBoxMenuItem (eTogglePrintIncomingEvents);
#if		qKeepRegionAllocStatistics
	AddSeparatorMenuItem ();
	AddStringMenuItem (ePrintRegionStatistics, True);
#endif
#if		qXGDI 
	AddSeparatorMenuItem ();
	AddCheckBoxMenuItem (eToggleSynchronizedMode);
#endif
}

DebugMenu::~DebugMenu ()
{
	DeleteAllMenuItems ();
}

void	DebugMenu::DoSetupsForDebugMenus ()
{
	MenuCommandHandler::EnableCommand (ePrintMemoryStatistics, True);
	MenuCommandHandler::EnableCommand (eValidateHeapStorage, True);
	MenuCommandHandler::EnableCommand (eToggleValidateHeapStorageOnAllocs, True);
	MenuCommandHandler::SetOn (eToggleValidateHeapStorageOnAllocs, GetValidateOnAlloc ());
	MenuCommandHandler::EnableCommand (eTogglePrintIncomingEvents, True);
	MenuCommandHandler::SetOn (eTogglePrintIncomingEvents, EventManager::GetPrintIncommingEventsFlag ());
#if		qKeepRegionAllocStatistics
	MenuCommandHandler::EnableCommand (ePrintRegionStatistics, True);
#endif
#if		qXGDI 
	MenuCommandHandler::EnableCommand (eToggleSynchronizedMode, True);
	MenuCommandHandler::SetOn (eToggleSynchronizedMode, Tablet::GetSyncMode ());
#endif
}

Boolean		DebugMenu::DoCommandDebugMenu (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case	ePrintMemoryStatistics: {
			PrintMemoryStatistics ();
			return (True);
		}
		break;
		case	eValidateHeapStorage: {
			ValidateHeapStorage (True);
			return (True);
		}
		break;
		case	eToggleValidateHeapStorageOnAllocs: {
			SetValidateOnAlloc (not GetValidateOnAlloc ());
			return (True);
		}
		break;
		case	eTogglePrintIncomingEvents: {
			EventManager::SetPrintIncommingEventsFlag (not EventManager::GetPrintIncommingEventsFlag ());
			return (True);
		}
		break;
#if		qKeepRegionAllocStatistics
		case	ePrintRegionStatistics: {
			Region::PrintStatictics ();
			return (True);
		}
		break;
#endif
#if		qXGDI
		case	eToggleSynchronizedMode: {
			Tablet::SetSyncMode (not Tablet::GetSyncMode ());
			return (True);
		}
		break;
#endif
		default: {
			return (False);
		}
		break;
	}
}

String	DebugMenu::DefaultName ()
{
	return (String (String::eReadOnly, "Debug"));
}

#endif	/*qDebug*/





// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

