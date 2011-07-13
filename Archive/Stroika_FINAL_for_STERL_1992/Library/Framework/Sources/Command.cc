/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Command.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Command.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.2  1992/07/02  04:49:47  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.15  92/03/26  17:19:52  17:19:52  sterling (Sterling Wight)
 *		moved commandtable stuff to frameworkcommandnamebuilder
 *		
 *		Revision 1.13  1992/03/05  18:20:39  lewis
 *		Made strings in CommandNameTable -nodelete.
 *		Got rid of Clear method, and moved its implematnion into dtor.
 *		Made some ctor/dtors inline.
 *
 *		Revision 1.10  1992/01/10  03:10:03  lewis
 *		Moved debug menu commandtablepairs to DebugMenu.cc. This invalidates Revert () method - delete it.
 *
 *
 *
 */



#include	"StreamUtils.hh"

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"Menu.hh"

#include	"Command.hh"







/*
 ********************************************************************************
 ************************************* Command **********************************
 ********************************************************************************
 */

const	Boolean	Command::kUndoable 	  = True;
const	Boolean	Command::kNotUndoable = False;

Command::Command (CommandNumber commandNumber, Boolean undoable):
	fCommandNumber (CommandHandler::eNoCommand),
	fUndoable (undoable),
	fDone (False),
	fName (kEmptyString)
{
	SetCommandNumber (commandNumber);
}

void	Command::DoIt ()
{
	SetDone (True);
}

void	Command::UnDoIt ()
{
	Require (GetDone ());
	Require (GetUndoable ());
	SetDone (False);
}

void	Command::ReDoIt ()
{
	Require (not (GetDone ()));
	DoIt ();
}

CommandNumber	Command::GetCommandNumber () const
{
	return (fCommandNumber);
}

void	Command::SetCommandNumber (CommandNumber commandNumber)
{
	fCommandNumber = commandNumber;

	String	name;
	if ((commandNumber != CommandHandler::eNoCommand) and
		CommandNameTable::Get ().Lookup (commandNumber, &name)) {
		SetName (name);
	}
}

Boolean		Command::GetUndoable () const
{
	return (fUndoable);
}

void	Command::SetUndoable (Boolean undoable)
{
	fUndoable = undoable;
}

Boolean		Command::GetDone () const
{
	return (fDone);
}

void	Command::SetDone (Boolean done)
{
	fDone = done;
}

String		Command::GetName () const
{
	return (fName);
}

void	Command::SetName (const String& name)
{
	fName = name;
}

Boolean	Command::CommitsLastCommand () const
{
	return (GetUndoable ());		// really want other things - like closing windows to
									// change this...
}









/*
 ********************************************************************************
 *********************************** CommandNameTable ***************************
 ********************************************************************************
 */



CmdNameTablePair::CmdNameTablePair ():
		fCommandNumber (CommandHandler::eNoCommand), fName (kEmptyString) {}


CmdNameTablePair::CmdNameTablePair (CommandNumber commandNumber, const String& name):
		fCommandNumber (commandNumber), fName (name) {}


CmdNameTablePair::~CmdNameTablePair ()
{
}

Boolean	operator== (const CmdNameTablePair& lhs, const CmdNameTablePair& rhs)
{
	return Boolean ((lhs.fCommandNumber == rhs.fCommandNumber) and (lhs.fName == rhs.fName));
}

Boolean	operator!= (const CmdNameTablePair& lhs, const CmdNameTablePair& rhs)
{
	return Boolean ((lhs.fCommandNumber != rhs.fCommandNumber) or (lhs.fName != rhs.fName));
}



#if		!qRealTemplatesAvailable
	Implement (Iterator, CmdNameTablePairPtr);
	Implement (Collection, CmdNameTablePairPtr);
	Implement (AbSequence, CmdNameTablePairPtr);
	Implement (Array, CmdNameTablePairPtr);
	Implement (Sequence_Array, CmdNameTablePairPtr);
	Implement (Sequence, CmdNameTablePairPtr);
#endif


CommandNameTable::CommandNameTable ()
{
}

CommandNameTable::~CommandNameTable ()
{
	ForEach (CmdNameTablePairPtr, it, fList) {
		CmdNameTablePair*	fred = it.Current ();
		AssertNotNil (fred);
		delete fred;
	}
}

void	CommandNameTable::AddPair (CommandNumber commandNumber, const String& name)
{
	AddPair (CmdNameTablePair (commandNumber, name));
}

void	CommandNameTable::AddPair (const CmdNameTablePair& pair)
{
	fList.Prepend (new CmdNameTablePair (pair));		// prepend so newer entries override old...
}

void	CommandNameTable::AddPairs (const CmdNameTablePair* pairs, CollectionSize count)
{
	RequireNotNil (pairs);
	for (CollectionSize i = 0; i < count; i++) {
		AddPair (pairs [i]);
	}
}

CommandNameTable&	CommandNameTable::Get ()
{
	/*
	 * Putting construction here avoids any possible races with building static objects, and
	 * also allows someone else to build their own subclass of CommandNameTable - they
	 * just must find a way to do it before anyone needs it (that is - if we had a setter function!!!).
	 */
	static	CommandNameTable&	sThe	=	*new CommandNameTable ();
	return (sThe);
}

Boolean	CommandNameTable::Lookup (CommandNumber commandNumber, String* name)
{
	RequireNotNil (name);
	ForEach (CmdNameTablePairPtr, it, fList) {
		AssertNotNil (it.Current ());
		if (it.Current ()->fCommandNumber == commandNumber) {
			*name = it.Current ()->fName;
			return (True);
		}
	}
	return (False);
}

Boolean	CommandNameTable::Lookup (String name, CommandNumber* commandNumber)
{
	RequireNotNil (commandNumber);
	ForEach (CmdNameTablePairPtr, it, fList) {
		if (it.Current ()->fName == name) {
			*commandNumber = it.Current ()->fCommandNumber;
			return (True);
		}
	}
	return (False);
}






/*
 ********************************************************************************
 *********************************** CompositeCommand ***************************
 ********************************************************************************
 */
CompositeCommand::CompositeCommand (Command* command, Command* next):
	Command (CommandHandler::eNoCommand),
	fCommand (command),
	fNextCommand (next)
{
	RequireNotNil (command);
}

CompositeCommand::~CompositeCommand ()
{
	if (fNextCommand != Nil) {
		delete fNextCommand;
	}
	delete fCommand;
}
		
void	CompositeCommand::DoIt ()
{
	if ((fNextCommand != Nil) and (not (fNextCommand->GetDone ()))) {
		fNextCommand->DoIt ();
	}
	AssertNotNil (fCommand);
	if (not (fCommand->GetDone ())) {
		fCommand->DoIt ();
	}
}

void	CompositeCommand::UnDoIt ()
{
	if ((fCommand->GetDone ()) and (fCommand->GetUndoable ())) {
		fCommand->UnDoIt ();
	}
	AssertNotNil (fCommand);
	if ((fNextCommand != Nil) and (fNextCommand->GetUndoable ()) and (fNextCommand->GetDone ())) {
		fNextCommand->UnDoIt ();
	}
}

CommandNumber	CompositeCommand::GetCommandNumber () const
{
	if (Command::GetCommandNumber () != CommandHandler::eNoCommand) {
		return (Command::GetCommandNumber ());
	}
	else if (fNextCommand != Nil) {
		return (fNextCommand->GetCommandNumber ());
	}
	else {
		return (fCommand->GetCommandNumber ());
	}
	AssertNotReached ();	return (CommandHandler::eNoCommand);
}

Boolean	CompositeCommand::GetUndoable () const
{
	if ((fNextCommand != Nil) and (not (fNextCommand->GetUndoable ()))) {
		return (False);
	}
	else {
		AssertNotNil (fCommand);
		return (fCommand->GetUndoable ());
	}
	AssertNotReached (); return (False);
}

String	CompositeCommand::GetName () const
{
	if (Command::GetName () != kEmptyString) {
		return (Command::GetName ());
	}
	else if (fNextCommand != Nil) {
		return (fNextCommand->GetName ());
	}
	else {
		AssertNotNil (fCommand);
		return (fCommand->GetName ());
	}
	AssertNotReached ();	return (kEmptyString);
}

Boolean	CompositeCommand::GetDone () const
{
	if ((fNextCommand != Nil) and (not (fNextCommand->GetDone ()))) {
		return (False);
	}
	else {
		AssertNotNil (fCommand);
		return (fCommand->GetDone ());
	}
	AssertNotReached (); return (False);
}







/*
 ********************************************************************************
 ************************************ CommandHandler ****************************
 ********************************************************************************
 */

void	CommandHandler::PostCommand (Command* newCommand)
{
	RequireNotNil (newCommand);
	Application::Get ().PostCommand (newCommand);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

