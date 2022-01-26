/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MenuCommandHandler.cc,v 1.3 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MenuCommandHandler.cc,v $
 *		Revision 1.3  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.9  1992/05/02  11:41:20  lewis
 *		Add requirements that commandnumber in EnableCommand/DisableCommand, etc != eNoCommand.
 *
 *		Revision 1.6  1992/02/04  04:28:23  lewis
 *		General cleanups (deleting include files).
 *
 *		Revision 1.5  1992/02/03  22:25:27  lewis
 *		Moved CommandSelection here from MenuItem.cc
 *
 *
 */



#include	"Debug.hh"

#include	"MenuOwner.hh"

#include	"MenuCommandHandler.hh"








/*
 ********************************************************************************
 ****************************** CommandSelection ********************************
 ********************************************************************************
 */
CommandSelection::CommandSelection (CommandNumber commandNumber):
	fCommandNumber (commandNumber),
	fName (String (String::eReadOnly, "Unknown Command Selection"))
{
}

CommandSelection::CommandSelection (CommandNumber commandNumber, const String& name):
	fCommandNumber (commandNumber),
	fName (name)
{
}

CommandSelection::~CommandSelection ()
{
}

CommandNumber	CommandSelection::GetCommandNumber () const
{
	return (fCommandNumber);
}

void	CommandSelection::SetCommandNumber (CommandNumber commandNumber)
{
	if (commandNumber != GetCommandNumber ()) {
		SetCommandNumber_ (commandNumber);
	}
	Ensure (GetCommandNumber () == commandNumber);
}

String	CommandSelection::GetName () const
{
	return (fName);
}

void	CommandSelection::SetName (const String& name)
{
	if (GetName () != name) {
		SetName_ (name);
	}
	Ensure (GetName () == name);
}
		
CollectionSize	CommandSelection::GetIndex () const
{
	return (GetIndex_ ());
}

void	CommandSelection::SetCommandNumber_ (CommandNumber commandNumber)
{
	fCommandNumber = commandNumber;
}

void	CommandSelection::SetName_ (const String& name)
{
	fName = name;
}

CollectionSize	CommandSelection::GetIndex_ () const
{
	return (kBadSequenceIndex);
}







/*
 ********************************************************************************
 ******************************** MenuCommandHandler ****************************
 ********************************************************************************
 */
void	MenuCommandHandler::DoSetupMenus ()
{
	// should be pure virtual???
}

Boolean	MenuCommandHandler::DoCommand (const CommandSelection& /*selection*/)
{
	return (False);	// should be pure virtual???
}

MenuItem*	MenuCommandHandler::GetMenuItem (CommandNumber commandNumber)
{
	ForEach (MenuOwnerPtr, it, MenuOwner::InstalledMenusIterator ()) {
		MenuItem* item = it.Current ()->GetMenuItem (commandNumber);
		if (item != Nil) {
			return (item);
		}
	}
	return (Nil);
}

void	MenuCommandHandler::EnableCommand (CommandNumber commandNumber, Boolean enableIt)
{
	Require (commandNumber != eNoCommand);	//	eNoCommand cannot be enabled - menu items that cannot
											//	be enabled use this as their command number.
											//	Almost certainly programmer bug to get here anyhow...
	if (enableIt) {
		EnableCommand (commandNumber);
	}
}

void	MenuCommandHandler::EnableCommand (CommandNumber commandNumber)
{
	Require (commandNumber != eNoCommand);	//	eNoCommand cannot be enabled - menu items that cannot
											//	be enabled use this as their command number.
											//	Almost certainly programmer bug to get here anyhow...
	ForEach (MenuOwnerPtr, it, MenuOwner::InstalledMenusIterator ()) {
		it.Current ()->SetEnabled (commandNumber, True);
	}
}

void	MenuCommandHandler::DisableCommand (CommandNumber commandNumber)
{
	Require (commandNumber != eNoCommand);	//	eNoCommand cannot be enabled(or disabled - always disabled)
											//	see MenuCommandHandler::EnableCommand ()
	ForEach (MenuOwnerPtr, it, MenuOwner::InstalledMenusIterator ()) {
		it.Current ()->SetEnabled (commandNumber, False);
	}
}

void	MenuCommandHandler::SetCommandName (CommandNumber commandNumber, const String& newName)
{
	Require (commandNumber != eNoCommand);	//	eNoCommand cannot have its name set. This is a specail command #.
											//	Almost certainly programmer bug to get here anyhow...
	ForEach (MenuOwnerPtr, it, MenuOwner::InstalledMenusIterator ()) {
		it.Current ()->SetName (commandNumber, newName);
	}
}

void	MenuCommandHandler::SetOn (CommandNumber commandNumber, Boolean on)
{
	Require (commandNumber != eNoCommand);	//	eNoCommand cannot be turned on/off. This is a specail command #.
											//	Almost certainly programmer bug to get here anyhow...
	ForEach (MenuOwnerPtr, it, MenuOwner::InstalledMenusIterator ()) {
		it.Current ()->SetOn (commandNumber, on);
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



