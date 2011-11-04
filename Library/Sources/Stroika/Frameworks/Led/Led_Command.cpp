/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../Foundation/StroikaPreComp.h"

#include	"Led_IdleManager.h"
#include	"Led_TextInteractor.h"

#include	"Led_Command.h"






#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {










/*
 ********************************************************************************
 *************************************** Command ********************************
 ********************************************************************************
 */
/*
@METHOD:		Command::GetName
@DESCRIPTION:	<p>Returns the name associated with a command. This is used for UI purposes in constructing the
	text of the Undo command name.</p>
*/
const Led_SDK_Char*	Command::GetName () const
{
	return Led_SDK_TCHAROF ("");
}

bool	Command::UpdateSimpleTextInsert (size_t /*insertAt*/, Led_tChar /*c*/)
{
	return false;
}





/*
 ********************************************************************************
 ******************************** CommandHandler ********************************
 ********************************************************************************
 */
CommandHandler::CommandHandler ()
{
}

bool	CommandHandler::PostUpdateSimpleTextInsert (size_t /*insertAt*/, Led_tChar /*c*/)
{
	IdleManager::NonIdleContext	nonIdleContext;
	return false;
}

size_t	CommandHandler::GetUndoRedoWhatMessageText (char* buf, size_t bufSize)
{
	const	char	kCantUndo[]	=	"Can't Undo";
	const	char	kUndo[]	=		"Undo";
	const	char	kReUndo[]	=	"Redo";
	if (CanUndo ()) {
		bufSize = Led_Min (bufSize, strlen (kUndo));
		memcpy (buf, kUndo, bufSize);
	}
	else if (CanRedo ()) {
		bufSize = Led_Min (bufSize, strlen (kReUndo));
		memcpy (buf, kReUndo, bufSize);
	}
	else {
		bufSize = Led_Min (bufSize, strlen (kCantUndo));
		memcpy (buf, kCantUndo, bufSize);
	}
	return bufSize;
}






/*
 ********************************************************************************
 ************************* SingleUndoCommandHandler *****************************
 ********************************************************************************
 */
SingleUndoCommandHandler::SingleUndoCommandHandler ():
	CommandHandler (),
	fLastCmd (NULL)
	#if		qDebug
	,fDoingCommands (false)
	#endif
{
}

void	SingleUndoCommandHandler::Post (Command* newCommand)
{
	Led_Require (not fDoingCommands);
	IdleManager::NonIdleContext	nonIdleContext;
	delete fLastCmd;
	fLastCmd = newCommand;
}

bool	SingleUndoCommandHandler::PostUpdateSimpleTextInsert (size_t insertAt, Led_tChar c)
{
	IdleManager::NonIdleContext	nonIdleContext;
	if (fLastCmd != NULL) {
		return fLastCmd->UpdateSimpleTextInsert (insertAt, c);
	}
	return false;
}

void	SingleUndoCommandHandler::BreakInGroupedCommands ()
{
}

void	SingleUndoCommandHandler::BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& /*cmdName*/)
{
}

void	SingleUndoCommandHandler::DoUndo (TextInteractor& interactor)
{
	Led_Require (CanUndo ());

	Led_RequireNotNil (fLastCmd);
	Led_Require (GetDone ());

	IdleManager::NonIdleContext	nonIdleContext;

	#if		qDebug
	Led_Require (not fDoingCommands);
	fDoingCommands = true;
	try {
	#endif
		fLastCmd->UnDo (interactor);
	#if		qDebug
	}
	catch (...) {
		fDoingCommands = false;
		throw;
	}
	fDoingCommands = false;
	#endif
}

void	SingleUndoCommandHandler::DoRedo (TextInteractor& interactor)
{
	Led_Require (CanRedo ());
	Led_RequireNotNil (fLastCmd);
	Led_Require (not GetDone ());

	IdleManager::NonIdleContext	nonIdleContext;

	#if		qDebug
	Led_Require (not fDoingCommands);
	fDoingCommands = true;
	try {
	#endif
		fLastCmd->ReDo (interactor);
	#if		qDebug
	}
	catch (...) {
		fDoingCommands = false;
		throw;
	}
	fDoingCommands = false;
	#endif
}

void	SingleUndoCommandHandler::Commit ()
{
	delete fLastCmd;
	fLastCmd = NULL;
}

bool	SingleUndoCommandHandler::CanUndo ()
{
	return fLastCmd != NULL and GetDone ();
}

bool	SingleUndoCommandHandler::CanRedo ()
{
	return fLastCmd != NULL and not GetDone ();
}

bool	SingleUndoCommandHandler::GetDone () const
{
	return (fLastCmd != NULL and fLastCmd->GetDone ());
}

const Led_SDK_Char*	SingleUndoCommandHandler::GetUndoCmdName ()
{
	if (CanUndo ()) {
		return fLastCmd->GetName ();
	}
	else {
		return Led_SDK_TCHAROF ("");
	}
}

const Led_SDK_Char*	SingleUndoCommandHandler::GetRedoCmdName ()
{
	if (CanRedo ()) {
		return fLastCmd->GetName ();
	}
	else {
		return Led_SDK_TCHAROF ("");
	}
}






/*
 ********************************************************************************
 ***************************** MultiLevelUndoCommandHandler *********************
 ********************************************************************************
 */
MultiLevelUndoCommandHandler::MultiLevelUndoCommandHandler (size_t maxUndoLevels, size_t maxCmdsPerLevel):
	CommandHandler (),
	fMaxUndoLevels (maxUndoLevels),
	fMaxCmdsPerLevel (maxCmdsPerLevel),
	fUndoCursor (0),
	fCommands (),
	fCommandGroupCount (0),
	fUndoneGroupCount (0)
	#if		qDebug
	,fDoingCommands (false)
	#endif
{
}

MultiLevelUndoCommandHandler::~MultiLevelUndoCommandHandler ()
{
	Commit ();	// just to avoid memory leak...
}

void	MultiLevelUndoCommandHandler::Post (Command* newCommand)
{
	Led_RequireNotNil (newCommand);
	Led_Require (not fDoingCommands);

	IdleManager::NonIdleContext	nonIdleContext;

	// When we've undone some things, and DO another, commit those UNDONE commands. We have no
	// UI for keeping multiple threads of undos alive...
	Led_Assert (fUndoCursor <= fCommands.size ());
	Commit_After (fUndoCursor);
	Led_Assert (fCommandGroupCount >= fUndoneGroupCount);
	fCommandGroupCount -= fUndoneGroupCount;
	fUndoneGroupCount = 0;

	Led_Assert (fCommandGroupCount <= fMaxUndoLevels);

	if (fMaxUndoLevels == 0) {
		// prevent memory leak/crash if no undo allowed
		delete newCommand;
		return;
	}

	bool	partOfNewCommand	=	(fCommands.size () == 0 or (fCommands.back () == NULL));

	// If prev item was a NULL, then we are starting new cmd group
	// cleanp/commit and old ones if we've hit the limit...
	if (partOfNewCommand) {
		if (fCommandGroupCount == fMaxUndoLevels) {
			size_t lastItemInFirstGroup = 0;
			for (; lastItemInFirstGroup <= fUndoCursor; lastItemInFirstGroup++) {
				if (fCommands[lastItemInFirstGroup] == NULL) {
					Led_Assert (lastItemInFirstGroup != 0);	// cannot have break here!
					// must be a break in here someplace - delete back from here...
					break;
				}
			}
			Led_Assert (lastItemInFirstGroup <= fUndoCursor);	// didn't fall through loop
			Commit_Before (lastItemInFirstGroup);
			fUndoCursor = fCommands.size ();
		}
		else {
			fCommandGroupCount++;
		}
	}
	else {
		Led_AssertNotNil (fCommands.back ());
	}


	// at this point, we may have added more commands in this group than we should have (fMaxCmdsPerLevel)...
	// maybe we'll ignore this for now - sooner or later I will implement merging commands together as
	// a speed hack!!!
	// LGP 960119
	// won't be as important once we implement merging adjacent typing commands which can be merged...
	// LGP 960328




	try {
		fCommands.push_back (newCommand);
	}
	catch (...) {
		// if we don't have enuf memory to add this to our list, we'd better dispose of the
		// command. Having the command Posted() makes it our responsability to free
		delete newCommand;

		// Actually - at this point, fCommandGroupCount maybe wrong (if partOfNewCommand)
		// rather than fixing it, since we are so low on memory, we may as well just commit
		// all our commands
		Commit ();

		throw;
	}
	fUndoCursor = fCommands.size ();
}

bool	MultiLevelUndoCommandHandler::PostUpdateSimpleTextInsert (size_t insertAt, Led_tChar c)
{
	IdleManager::NonIdleContext	nonIdleContext;
	if (fUndoCursor != fCommands.size ()) {
		// cannot update last command with undo info if we've undone anything on stack
		return false;
	}
	Command*	lastCmd	=	(fCommands.size () == 0)? NULL: fCommands.back ();
	if (lastCmd != NULL) {
		return lastCmd->UpdateSimpleTextInsert (insertAt, c);
	}
	return false;
}

void	MultiLevelUndoCommandHandler::BreakInGroupedCommands ()
{
	size_t	commandListLen	=	fCommands.size ();
	/*
	 *	Unless we are at the end of a sequence of commands, don't do a break (cuz it would
	 *	cause a committing of commands needlessly - and there must already be a break
	 *	here).
	 */
	if (commandListLen != 0 and (fUndoCursor == commandListLen)) {
		if (fCommands[fUndoCursor-1] != NULL) {	// last actual command in array
			fCommands.push_back (NULL);
			fUndoCursor = fCommands.size ();
		}
	}
}

void	MultiLevelUndoCommandHandler::BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName)
{
	if (fCommands.size () != 0 and fCommands.back () != NULL and fCommands.back ()->GetName () != cmdName) {
		BreakInGroupedCommands ();
	}
}

void	MultiLevelUndoCommandHandler::DoUndo (TextInteractor& interactor)
{
	Led_Require (CanUndo ());

	IdleManager::NonIdleContext	nonIdleContext;

	BreakInGroupedCommands ();

	size_t	start;
	size_t	end;
	bool	result	=	GetLastCmdRangeBefore (&start, &end);
	Led_Assert (result);

	#if		qDebug
	Led_Require (not fDoingCommands);
	fDoingCommands = true;
	try {
	#endif
		for (long i = end; i >= long (start); i--) {
			fCommands[i]->UnDo (interactor);
		}
	#if		qDebug
	}
	catch (...) {
		fDoingCommands = false;
		throw;
	}
	fDoingCommands = false;
	#endif

	fUndoCursor = start;
	fUndoneGroupCount++;
	Led_Assert (fUndoneGroupCount <= fCommandGroupCount);
}

void	MultiLevelUndoCommandHandler::DoRedo (TextInteractor& interactor)
{
	Led_Require (CanRedo ());

	IdleManager::NonIdleContext	nonIdleContext;

	size_t	start;
	size_t	end;
	bool	result	=	GetLastCmdRangeAfter (&start, &end);
	Led_Assert (result);

	#if		qDebug
	Led_Require (not fDoingCommands);
	fDoingCommands = true;
	try {
	#endif
		for (size_t i = start; i <= end; i++) {
			fCommands[i]->ReDo (interactor);
		}
	#if		qDebug
	}
	catch (...) {
		fDoingCommands = false;
		throw;
	}
	fDoingCommands = false;
	#endif

	fUndoCursor = end + 1;	// point AFTER last cmd
	Led_Assert (fUndoCursor <= fCommands.size ());
	if (fCommands[fUndoCursor] == NULL) {
		// if pointing to breaker, then point just past it, so new posted commands
		// come after that
		fUndoCursor++;
		Led_Assert (fUndoCursor <= fCommands.size ());
	}
	Led_Assert (fUndoneGroupCount <= fCommandGroupCount);
	Led_Assert (fUndoneGroupCount >= 1);
	fUndoneGroupCount--;
}

void	MultiLevelUndoCommandHandler::Commit ()
{
	for (size_t i = 0; i < fCommands.size (); i++) {
		delete fCommands[i];
	}
	fCommands.clear ();
	fUndoCursor = 0;
	fUndoneGroupCount = 0;
	fCommandGroupCount = 0;
}

bool	MultiLevelUndoCommandHandler::CanUndo ()
{
	return (fCommandGroupCount > fUndoneGroupCount);
}

bool	MultiLevelUndoCommandHandler::CanRedo ()
{
	return (fUndoneGroupCount > 0);
}

const Led_SDK_Char*	MultiLevelUndoCommandHandler::GetUndoCmdName ()
{
	if (CanUndo ()) {
		size_t	start;
		size_t	end;
		bool	result	=	GetLastCmdRangeBefore (&start, &end);
		Led_Assert (result);
		// arbitrarily pick name from any of the commands in group
		return fCommands[start]->GetName ();
	}
	else {
		return Led_SDK_TCHAROF ("");
	}
}

const Led_SDK_Char*	MultiLevelUndoCommandHandler::GetRedoCmdName ()
{
	if (CanRedo ()) {
		size_t	start;
		size_t	end;
		bool	result	=	GetLastCmdRangeAfter (&start, &end);
		Led_Assert (result);
		// arbitrarily pick name from any of the commands in group
		return fCommands[start]->GetName ();
	}
	else {
		return Led_SDK_TCHAROF ("");
	}
}

/*
@METHOD:		MultiLevelUndoCommandHandler::SetMaxUnDoLevels
@DESCRIPTION:	<p>See @'MultiLevelUndoCommandHandler::GetMaxUnDoLevels'</p>
	
*/
void	MultiLevelUndoCommandHandler::SetMaxUnDoLevels (size_t maxUndoLevels)
{
	if (fCommandGroupCount >= maxUndoLevels) {
		Commit ();
	}
	fMaxUndoLevels = maxUndoLevels;
}

bool	MultiLevelUndoCommandHandler::GetLastCmdRangeBefore (size_t* startIdx, size_t* endIdx) const
{
	Led_RequireNotNil (startIdx);
	Led_RequireNotNil (endIdx);

	*startIdx = 0;
	*endIdx = 0;

	size_t	commandListLen	=	fCommands.size ();

	if (commandListLen == 0) {
		return false;
	}
	Led_Assert (fUndoCursor >= 0);
	if (fUndoCursor == 0) {
		return false;
	}

	size_t i = fUndoCursor - 1;	// start looking at item just BEFORE cursor
	if (fCommands[i] == NULL) {
		if (i == 0) {
			return false;
		}
		i--;
	}
	*endIdx = i;

	for (; i > 0; i--) {
		if (fCommands[i] == NULL) {
			*startIdx = i+1;
			return true;
		}
	}
	return true;
}

bool	MultiLevelUndoCommandHandler::GetLastCmdRangeAfter (size_t* startIdx, size_t* endIdx) const
{
	Led_RequireNotNil (startIdx);
	Led_RequireNotNil (endIdx);

	*startIdx = 0;
	*endIdx = 0;

	Led_Assert (fUndoCursor != kBadIndex);	// if triggered must do some fixing...

	size_t	commandListLen		=	fCommands.size ();
	size_t	listPastEnd			=	commandListLen;
	Led_Assert (fUndoCursor <= listPastEnd);
	if (fUndoCursor == listPastEnd) {
		return false;
	}
	else {
		size_t i = fUndoCursor;
		if (fCommands[i] == NULL) {
			i++;
		}
		*startIdx = i;

		for (; i < listPastEnd; i++) {
			if (fCommands[i] == NULL) {
				*endIdx = i-1;
				return true;
			}
		}
		Led_Assert (listPastEnd > 0);
		*endIdx = listPastEnd-1;
		return true;
	}
}

void	MultiLevelUndoCommandHandler::Commit_After (size_t after)
{
	Led_Require (after >= 0);
	size_t	commandsLen	=	fCommands.size ();
	if (commandsLen != 0) {
		for (long i = commandsLen-1; i >= long (after); i--) {
			Led_Assert (i >= 0);
			delete fCommands[i];
			fCommands.erase (fCommands.begin () + i);
		}
	}
}

void	MultiLevelUndoCommandHandler::Commit_Before (size_t before)
{
	// delete items before 'before' - and INCLUDING THAT ITEM. So this MUST BE CALLED ON
	// A NON_EMPTY LIST!
	Led_Require (before >= 0);
	Led_Require (before <= fCommands.size ());
	size_t	countToCommit	=	(before) + 1;
	while (countToCommit != 0) {
		delete fCommands[0];
		fCommands.erase (fCommands.begin ());
		countToCommit--;
	}
}






/*
 ********************************************************************************
 **************************** SnoopingCommandHandler ****************************
 ********************************************************************************
 */

SnoopingCommandHandler::SnoopingCommandHandler (CommandHandler* realHandler):
	fRealHandler (realHandler)
{
}

void	SnoopingCommandHandler::Post (Command* newCommand)
{
	IdleManager::NonIdleContext	nonIdleContext;
	Snoop (newCommand);
	if (fRealHandler != NULL) {
		fRealHandler->Post (newCommand);
	}
}

void	SnoopingCommandHandler::BreakInGroupedCommands ()
{
	if (fRealHandler != NULL) {
		fRealHandler->BreakInGroupedCommands ();
	}
}

void	SnoopingCommandHandler::BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName)
{
	if (fRealHandler != NULL) {
		fRealHandler->BreakInGroupedCommandsIfDifferentCommand (cmdName);
	}
}

void	SnoopingCommandHandler::DoUndo (TextInteractor& interactor)
{
	IdleManager::NonIdleContext	nonIdleContext;

	if (fRealHandler != NULL) {
		fRealHandler->DoUndo (interactor);
	}
}

void	SnoopingCommandHandler::DoRedo (TextInteractor& interactor)
{
	IdleManager::NonIdleContext	nonIdleContext;

	if (fRealHandler != NULL) {
		fRealHandler->DoRedo (interactor);
	}
}

void	SnoopingCommandHandler::Commit ()
{
	if (fRealHandler != NULL) {
		fRealHandler->Commit ();
	}
}

bool	SnoopingCommandHandler::CanUndo ()
{
	if (fRealHandler == NULL) {
		return false;
	}
	else {
		return fRealHandler->CanUndo ();
	}
}

bool	SnoopingCommandHandler::CanRedo ()
{
	if (fRealHandler == NULL) {
		return false;
	}
	else {
		return fRealHandler->CanRedo ();
	}
}

const Led_SDK_Char*	SnoopingCommandHandler::GetUndoCmdName ()
{
	if (fRealHandler == NULL) {
		return Led_SDK_TCHAROF ("");
	}
	else {
		return fRealHandler->GetUndoCmdName ();
	}
}

const Led_SDK_Char*	SnoopingCommandHandler::GetRedoCmdName ()
{
	if (fRealHandler == NULL) {
		return Led_SDK_TCHAROF ("");
	}
	else {
		return fRealHandler->GetRedoCmdName ();
	}
}








/*
 ********************************************************************************
 ************************* InteractiveReplaceCommand ****************************
 ********************************************************************************
 */

/*
@METHOD:		InteractiveReplaceCommand::InteractiveReplaceCommand
@DESCRIPTION:	<p>This constructor takes an @'InteractiveReplaceCommand::SavedTextRep'
	for BEFORE and AFTER. It also takes as argument - an 'at' parameter - specifying where the NEW/OLD text (for DO and UNDO) will get inserted.
	For the length overwritten - we use the size from the SavedTextRep itself. This CTOR also takes a command name to be saved with the
	command (not used intenrally - but saved as an attribute so menu handling code can report what is to be UNDONE).</p>
*/
InteractiveReplaceCommand::InteractiveReplaceCommand (SavedTextRep* beforeRegion,
														SavedTextRep* afterRegion, size_t at, const Led_SDK_String& cmdName
													):
	inherited (true),
	fBeforeRegion (beforeRegion),
	fAfterRegion (afterRegion),
	fAt (at),
	fCmdName (cmdName)
{
	Led_RequireNotNil (fBeforeRegion);
	Led_RequireNotNil (fAfterRegion);
}

InteractiveReplaceCommand::~InteractiveReplaceCommand ()
{
	delete fBeforeRegion;
	delete fAfterRegion;
}

void	InteractiveReplaceCommand::Do (TextInteractor& /*interactor*/)
{
	Led_Assert (false);	// illegal to call - command must be PRE-DONE
}

void	InteractiveReplaceCommand::UnDo (TextInteractor& interactor)
{
	Led_AssertNotNil (fBeforeRegion);
	Led_AssertNotNil (fAfterRegion);
	fBeforeRegion->InsertSelf (&interactor, fAt, fAfterRegion->GetLength ());
	fBeforeRegion->ApplySelection (&interactor);

	inherited::UnDo (interactor);
}

void	InteractiveReplaceCommand::ReDo (TextInteractor& interactor)
{
	Led_AssertNotNil (fBeforeRegion);
	Led_AssertNotNil (fAfterRegion);
	fAfterRegion->InsertSelf (&interactor, fAt, fBeforeRegion->GetLength ());
	fAfterRegion->ApplySelection (&interactor);

	inherited::ReDo (interactor);
}

bool	InteractiveReplaceCommand::UpdateSimpleTextInsert (size_t insertAt, Led_tChar c)
{
	Led_AssertNotNil (fBeforeRegion);
	Led_AssertNotNil (fAfterRegion);

	PlainTextRep*	afterPTR	=	dynamic_cast<PlainTextRep*> (fAfterRegion);
	if (afterPTR != NULL) {
		return afterPTR->AppendCharToRep (insertAt, c);
	}

	return false;
}

const Led_SDK_Char*	InteractiveReplaceCommand::GetName () const
{
	return fCmdName.c_str ();
}






/*
 ********************************************************************************
 ******************** InteractiveReplaceCommand::SavedTextRep *******************
 ********************************************************************************
 */
void	InteractiveReplaceCommand::SavedTextRep::ApplySelection (TextInteractor* imager)
{
	Led_RequireNotNil (imager);
	imager->SetSelection (fSelStart, fSelEnd);
}







/*
 ********************************************************************************
 ******************** InteractiveReplaceCommand::PlainTextRep *******************
 ********************************************************************************
 */
InteractiveReplaceCommand::PlainTextRep::PlainTextRep (size_t selStart, size_t selEnd, const Led_tChar* text, size_t textLen):
	inherited (selStart, selEnd),
	fText (NULL),
	fTextLength (textLen)
{
	if (textLen != 0) {
		fText = new Led_tChar [textLen];
		memcpy (fText, text, textLen*sizeof (Led_tChar));
	}
}

InteractiveReplaceCommand::PlainTextRep::~PlainTextRep ()
{
	delete[] fText;
}

size_t	InteractiveReplaceCommand::PlainTextRep::GetLength () const
{
	return fTextLength;
}

void	InteractiveReplaceCommand::PlainTextRep::InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite)
{
	Led_RequireNotNil (interactor);
	interactor->Replace (at, at + nBytesToOverwrite, fText, fTextLength);
}

/*
@METHOD:		InteractiveReplaceCommand::PlainTextRep::AppendCharToRep
@DESCRIPTION:	<p>Utility used internally to implement optimized undo code.</p>
*/
bool	InteractiveReplaceCommand::PlainTextRep::AppendCharToRep (size_t insertAt, Led_tChar c)
{
	if (fSelStart == insertAt and fSelEnd == insertAt) {
		// could be more efficient and avoid copy - but this is already a big improvemnt over old algorithm - so lets not complain just yet...
		Led_tChar*	newText = new Led_tChar [fTextLength + 1];
		(void)::memcpy (newText, fText, fTextLength * sizeof (Led_tChar));
		newText[fTextLength] = c;
		fTextLength++;
		delete[] fText;
		fText = newText;
		++fSelStart;
		fSelEnd = fSelStart;
		return true;
	}
	else {
		return false;
	}
}






		}
	}
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


