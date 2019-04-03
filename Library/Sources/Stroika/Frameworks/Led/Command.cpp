/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "IdleManager.h"
#include "TextInteractor.h"

#include "Command.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 *************************************** Command ********************************
 ********************************************************************************
 */
/*
@METHOD:        Command::GetName
@DESCRIPTION:   <p>Returns the name associated with a command. This is used for UI purposes in constructing the
    text of the Undo command name.</p>
*/
const Led_SDK_Char* Command::GetName () const
{
    return Led_SDK_TCHAROF ("");
}

bool Command::UpdateSimpleTextInsert (size_t /*insertAt*/, Led_tChar /*c*/)
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

bool CommandHandler::PostUpdateSimpleTextInsert (size_t /*insertAt*/, Led_tChar /*c*/)
{
    IdleManager::NonIdleContext nonIdleContext;
    return false;
}

size_t CommandHandler::GetUndoRedoWhatMessageText (char* buf, size_t bufSize)
{
    const char kCantUndo[] = "Can't Undo";
    const char kUndo[]     = "Undo";
    const char kReUndo[]   = "Redo";
    if (CanUndo ()) {
        bufSize = min (bufSize, strlen (kUndo));
        memcpy (buf, kUndo, bufSize);
    }
    else if (CanRedo ()) {
        bufSize = min (bufSize, strlen (kReUndo));
        memcpy (buf, kReUndo, bufSize);
    }
    else {
        bufSize = min (bufSize, strlen (kCantUndo));
        memcpy (buf, kCantUndo, bufSize);
    }
    return bufSize;
}

/*
 ********************************************************************************
 ************************* SingleUndoCommandHandler *****************************
 ********************************************************************************
 */
SingleUndoCommandHandler::SingleUndoCommandHandler ()
    : CommandHandler ()
    , fLastCmd (nullptr)
#if qDebug
    , fDoingCommands (false)
#endif
{
}

void SingleUndoCommandHandler::Post (Command* newCommand)
{
    Require (not fDoingCommands);
    IdleManager::NonIdleContext nonIdleContext;
    delete fLastCmd;
    fLastCmd = newCommand;
}

bool SingleUndoCommandHandler::PostUpdateSimpleTextInsert (size_t insertAt, Led_tChar c)
{
    IdleManager::NonIdleContext nonIdleContext;
    if (fLastCmd != nullptr) {
        return fLastCmd->UpdateSimpleTextInsert (insertAt, c);
    }
    return false;
}

void SingleUndoCommandHandler::BreakInGroupedCommands ()
{
}

void SingleUndoCommandHandler::BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& /*cmdName*/)
{
}

void SingleUndoCommandHandler::DoUndo (TextInteractor& interactor)
{
    Require (CanUndo ());

    RequireNotNull (fLastCmd);
    Require (GetDone ());

    IdleManager::NonIdleContext nonIdleContext;

#if qDebug
    Require (not fDoingCommands);
    fDoingCommands = true;
    try {
#endif
        fLastCmd->UnDo (interactor);
#if qDebug
    }
    catch (...) {
        fDoingCommands = false;
        throw;
    }
    fDoingCommands = false;
#endif
}

void SingleUndoCommandHandler::DoRedo (TextInteractor& interactor)
{
    Require (CanRedo ());
    RequireNotNull (fLastCmd);
    Require (not GetDone ());

    IdleManager::NonIdleContext nonIdleContext;

#if qDebug
    Require (not fDoingCommands);
    fDoingCommands = true;
    try {
#endif
        fLastCmd->ReDo (interactor);
#if qDebug
    }
    catch (...) {
        fDoingCommands = false;
        throw;
    }
    fDoingCommands = false;
#endif
}

void SingleUndoCommandHandler::Commit ()
{
    delete fLastCmd;
    fLastCmd = nullptr;
}

bool SingleUndoCommandHandler::CanUndo ()
{
    return fLastCmd != nullptr and GetDone ();
}

bool SingleUndoCommandHandler::CanRedo ()
{
    return fLastCmd != nullptr and not GetDone ();
}

bool SingleUndoCommandHandler::GetDone () const
{
    return (fLastCmd != nullptr and fLastCmd->GetDone ());
}

const Led_SDK_Char* SingleUndoCommandHandler::GetUndoCmdName ()
{
    if (CanUndo ()) {
        return fLastCmd->GetName ();
    }
    else {
        return Led_SDK_TCHAROF ("");
    }
}

const Led_SDK_Char* SingleUndoCommandHandler::GetRedoCmdName ()
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
MultiLevelUndoCommandHandler::MultiLevelUndoCommandHandler (size_t maxUndoLevels, size_t maxCmdsPerLevel)
    : CommandHandler ()
    , fMaxUndoLevels (maxUndoLevels)
    , fMaxCmdsPerLevel (maxCmdsPerLevel)
    , fUndoCursor (0)
    , fCommands ()
    , fCommandGroupCount (0)
    , fUndoneGroupCount (0)
#if qDebug
    , fDoingCommands (false)
#endif
{
}

MultiLevelUndoCommandHandler::~MultiLevelUndoCommandHandler ()
{
    Commit (); // just to avoid memory leak...
}

void MultiLevelUndoCommandHandler::Post (Command* newCommand)
{
    RequireNotNull (newCommand);
    Require (not fDoingCommands);

    IdleManager::NonIdleContext nonIdleContext;

    // When we've undone some things, and DO another, commit those UNDONE commands. We have no
    // UI for keeping multiple threads of undos alive...
    Assert (fUndoCursor <= fCommands.size ());
    Commit_After (fUndoCursor);
    Assert (fCommandGroupCount >= fUndoneGroupCount);
    fCommandGroupCount -= fUndoneGroupCount;
    fUndoneGroupCount = 0;

    Assert (fCommandGroupCount <= fMaxUndoLevels);

    if (fMaxUndoLevels == 0) {
        // prevent memory leak/crash if no undo allowed
        delete newCommand;
        return;
    }

    bool partOfNewCommand = (fCommands.size () == 0 or (fCommands.back () == nullptr));

    // If prev item was a nullptr, then we are starting new cmd group
    // cleanp/commit and old ones if we've hit the limit...
    if (partOfNewCommand) {
        if (fCommandGroupCount == fMaxUndoLevels) {
            size_t lastItemInFirstGroup = 0;
            for (; lastItemInFirstGroup <= fUndoCursor; lastItemInFirstGroup++) {
                if (fCommands[lastItemInFirstGroup] == nullptr) {
                    Assert (lastItemInFirstGroup != 0); // cannot have break here!
                    // must be a break in here someplace - delete back from here...
                    break;
                }
            }
            Assert (lastItemInFirstGroup <= fUndoCursor); // didn't fall through loop
            Commit_Before (lastItemInFirstGroup);
            fUndoCursor = fCommands.size ();
        }
        else {
            fCommandGroupCount++;
        }
    }
    else {
        AssertNotNull (fCommands.back ());
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

bool MultiLevelUndoCommandHandler::PostUpdateSimpleTextInsert (size_t insertAt, Led_tChar c)
{
    IdleManager::NonIdleContext nonIdleContext;
    if (fUndoCursor != fCommands.size ()) {
        // cannot update last command with undo info if we've undone anything on stack
        return false;
    }
    Command* lastCmd = (fCommands.size () == 0) ? nullptr : fCommands.back ();
    if (lastCmd != nullptr) {
        return lastCmd->UpdateSimpleTextInsert (insertAt, c);
    }
    return false;
}

void MultiLevelUndoCommandHandler::BreakInGroupedCommands ()
{
    size_t commandListLen = fCommands.size ();
    /*
     *  Unless we are at the end of a sequence of commands, don't do a break (cuz it would
     *  cause a committing of commands needlessly - and there must already be a break
     *  here).
     */
    if (commandListLen != 0 and (fUndoCursor == commandListLen)) {
        if (fCommands[fUndoCursor - 1] != nullptr) { // last actual command in array
            fCommands.push_back (nullptr);
            fUndoCursor = fCommands.size ();
        }
    }
}

void MultiLevelUndoCommandHandler::BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName)
{
    if (fCommands.size () != 0 and fCommands.back () != nullptr and fCommands.back ()->GetName () != cmdName) {
        BreakInGroupedCommands ();
    }
}

void MultiLevelUndoCommandHandler::DoUndo (TextInteractor& interactor)
{
    Require (CanUndo ());

    IdleManager::NonIdleContext nonIdleContext;

    BreakInGroupedCommands ();

    size_t                start;
    size_t                end;
    [[maybe_unused]] bool result = GetLastCmdRangeBefore (&start, &end);
    Assert (result);

#if qDebug
    Require (not fDoingCommands);
    fDoingCommands = true;
    try {
#endif
        for (int i = static_cast<int> (end); i >= static_cast<int> (start); i--) {
            fCommands[i]->UnDo (interactor);
        }
#if qDebug
    }
    catch (...) {
        fDoingCommands = false;
        throw;
    }
    fDoingCommands = false;
#endif

    fUndoCursor = start;
    fUndoneGroupCount++;
    Assert (fUndoneGroupCount <= fCommandGroupCount);
}

void MultiLevelUndoCommandHandler::DoRedo (TextInteractor& interactor)
{
    Require (CanRedo ());

    IdleManager::NonIdleContext nonIdleContext;

    size_t                start;
    size_t                end;
    [[maybe_unused]] bool result = GetLastCmdRangeAfter (&start, &end);
    Assert (result);

#if qDebug
    Require (not fDoingCommands);
    fDoingCommands = true;
    try {
#endif
        for (size_t i = start; i <= end; i++) {
            fCommands[i]->ReDo (interactor);
        }
#if qDebug
    }
    catch (...) {
        fDoingCommands = false;
        throw;
    }
    fDoingCommands = false;
#endif

    fUndoCursor = end + 1; // point AFTER last cmd
    Assert (fUndoCursor <= fCommands.size ());
    if (fCommands[fUndoCursor] == nullptr) {
        // if pointing to breaker, then point just past it, so new posted commands
        // come after that
        fUndoCursor++;
        Assert (fUndoCursor <= fCommands.size ());
    }
    Assert (fUndoneGroupCount <= fCommandGroupCount);
    Assert (fUndoneGroupCount >= 1);
    fUndoneGroupCount--;
}

void MultiLevelUndoCommandHandler::Commit ()
{
    for (size_t i = 0; i < fCommands.size (); i++) {
        delete fCommands[i];
    }
    fCommands.clear ();
    fUndoCursor        = 0;
    fUndoneGroupCount  = 0;
    fCommandGroupCount = 0;
}

bool MultiLevelUndoCommandHandler::CanUndo ()
{
    return (fCommandGroupCount > fUndoneGroupCount);
}

bool MultiLevelUndoCommandHandler::CanRedo ()
{
    return (fUndoneGroupCount > 0);
}

const Led_SDK_Char* MultiLevelUndoCommandHandler::GetUndoCmdName ()
{
    if (CanUndo ()) {
        size_t                start;
        size_t                end;
        [[maybe_unused]] bool result = GetLastCmdRangeBefore (&start, &end);
        Assert (result);
        // arbitrarily pick name from any of the commands in group
        return fCommands[start]->GetName ();
    }
    else {
        return Led_SDK_TCHAROF ("");
    }
}

const Led_SDK_Char* MultiLevelUndoCommandHandler::GetRedoCmdName ()
{
    if (CanRedo ()) {
        size_t                start;
        size_t                end;
        [[maybe_unused]] bool result = GetLastCmdRangeAfter (&start, &end);
        Assert (result);
        // arbitrarily pick name from any of the commands in group
        return fCommands[start]->GetName ();
    }
    else {
        return Led_SDK_TCHAROF ("");
    }
}

/*
@METHOD:        MultiLevelUndoCommandHandler::SetMaxUnDoLevels
@DESCRIPTION:   <p>See @'MultiLevelUndoCommandHandler::GetMaxUnDoLevels'</p>

*/
void MultiLevelUndoCommandHandler::SetMaxUnDoLevels (size_t maxUndoLevels)
{
    if (fCommandGroupCount >= maxUndoLevels) {
        Commit ();
    }
    fMaxUndoLevels = maxUndoLevels;
}

bool MultiLevelUndoCommandHandler::GetLastCmdRangeBefore (size_t* startIdx, size_t* endIdx) const
{
    RequireNotNull (startIdx);
    RequireNotNull (endIdx);

    *startIdx = 0;
    *endIdx   = 0;

    size_t commandListLen = fCommands.size ();

    if (commandListLen == 0) {
        return false;
    }
    Assert (fUndoCursor >= 0);
    if (fUndoCursor == 0) {
        return false;
    }

    size_t i = fUndoCursor - 1; // start looking at item just BEFORE cursor
    if (fCommands[i] == nullptr) {
        if (i == 0) {
            return false;
        }
        i--;
    }
    *endIdx = i;

    for (; i > 0; i--) {
        if (fCommands[i] == nullptr) {
            *startIdx = i + 1;
            return true;
        }
    }
    return true;
}

bool MultiLevelUndoCommandHandler::GetLastCmdRangeAfter (size_t* startIdx, size_t* endIdx) const
{
    RequireNotNull (startIdx);
    RequireNotNull (endIdx);

    *startIdx = 0;
    *endIdx   = 0;

    Assert (fUndoCursor != kBadIndex); // if triggered must do some fixing...

    size_t commandListLen = fCommands.size ();
    size_t listPastEnd    = commandListLen;
    Assert (fUndoCursor <= listPastEnd);
    if (fUndoCursor == listPastEnd) {
        return false;
    }
    else {
        size_t i = fUndoCursor;
        if (fCommands[i] == nullptr) {
            i++;
        }
        *startIdx = i;

        for (; i < listPastEnd; i++) {
            if (fCommands[i] == nullptr) {
                *endIdx = i - 1;
                return true;
            }
        }
        Assert (listPastEnd > 0);
        *endIdx = listPastEnd - 1;
        return true;
    }
}

void MultiLevelUndoCommandHandler::Commit_After (size_t after)
{
    Require (after >= 0);
    size_t commandsLen = fCommands.size ();
    if (commandsLen != 0) {
        for (long i = static_cast<long> (commandsLen) - 1; i >= long (after); i--) {
            Assert (i >= 0);
            delete fCommands[i];
            fCommands.erase (fCommands.begin () + i);
        }
    }
}

void MultiLevelUndoCommandHandler::Commit_Before (size_t before)
{
    // delete items before 'before' - and INCLUDING THAT ITEM. So this MUST BE CALLED ON
    // A NON_EMPTY LIST!
    Require (before >= 0);
    Require (before <= fCommands.size ());
    size_t countToCommit = (before) + 1;
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

SnoopingCommandHandler::SnoopingCommandHandler (CommandHandler* realHandler)
    : fRealHandler (realHandler)
{
}

void SnoopingCommandHandler::Post (Command* newCommand)
{
    IdleManager::NonIdleContext nonIdleContext;
    Snoop (newCommand);
    if (fRealHandler != nullptr) {
        fRealHandler->Post (newCommand);
    }
}

void SnoopingCommandHandler::BreakInGroupedCommands ()
{
    if (fRealHandler != nullptr) {
        fRealHandler->BreakInGroupedCommands ();
    }
}

void SnoopingCommandHandler::BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName)
{
    if (fRealHandler != nullptr) {
        fRealHandler->BreakInGroupedCommandsIfDifferentCommand (cmdName);
    }
}

void SnoopingCommandHandler::DoUndo (TextInteractor& interactor)
{
    IdleManager::NonIdleContext nonIdleContext;

    if (fRealHandler != nullptr) {
        fRealHandler->DoUndo (interactor);
    }
}

void SnoopingCommandHandler::DoRedo (TextInteractor& interactor)
{
    IdleManager::NonIdleContext nonIdleContext;

    if (fRealHandler != nullptr) {
        fRealHandler->DoRedo (interactor);
    }
}

void SnoopingCommandHandler::Commit ()
{
    if (fRealHandler != nullptr) {
        fRealHandler->Commit ();
    }
}

bool SnoopingCommandHandler::CanUndo ()
{
    if (fRealHandler == nullptr) {
        return false;
    }
    else {
        return fRealHandler->CanUndo ();
    }
}

bool SnoopingCommandHandler::CanRedo ()
{
    if (fRealHandler == nullptr) {
        return false;
    }
    else {
        return fRealHandler->CanRedo ();
    }
}

const Led_SDK_Char* SnoopingCommandHandler::GetUndoCmdName ()
{
    if (fRealHandler == nullptr) {
        return Led_SDK_TCHAROF ("");
    }
    else {
        return fRealHandler->GetUndoCmdName ();
    }
}

const Led_SDK_Char* SnoopingCommandHandler::GetRedoCmdName ()
{
    if (fRealHandler == nullptr) {
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
@METHOD:        InteractiveReplaceCommand::InteractiveReplaceCommand
@DESCRIPTION:   <p>This constructor takes an @'InteractiveReplaceCommand::SavedTextRep'
    for BEFORE and AFTER. It also takes as argument - an 'at' parameter - specifying where the NEW/OLD text (for DO and UNDO) will get inserted.
    For the length overwritten - we use the size from the SavedTextRep itself. This CTOR also takes a command name to be saved with the
    command (not used intenrally - but saved as an attribute so menu handling code can report what is to be UNDONE).</p>
*/
InteractiveReplaceCommand::InteractiveReplaceCommand (SavedTextRep* beforeRegion,
                                                      SavedTextRep* afterRegion, size_t at, const Led_SDK_String& cmdName)
    : inherited (true)
    , fBeforeRegion (beforeRegion)
    , fAfterRegion (afterRegion)
    , fAt (at)
    , fCmdName (cmdName)
{
    RequireNotNull (fBeforeRegion);
    RequireNotNull (fAfterRegion);
}

InteractiveReplaceCommand::~InteractiveReplaceCommand ()
{
    delete fBeforeRegion;
    delete fAfterRegion;
}

void InteractiveReplaceCommand::Do (TextInteractor& /*interactor*/)
{
    Assert (false); // illegal to call - command must be PRE-DONE
}

void InteractiveReplaceCommand::UnDo (TextInteractor& interactor)
{
    AssertNotNull (fBeforeRegion);
    AssertNotNull (fAfterRegion);
    fBeforeRegion->InsertSelf (&interactor, fAt, fAfterRegion->GetLength ());
    fBeforeRegion->ApplySelection (&interactor);

    inherited::UnDo (interactor);
}

void InteractiveReplaceCommand::ReDo (TextInteractor& interactor)
{
    AssertNotNull (fBeforeRegion);
    AssertNotNull (fAfterRegion);
    fAfterRegion->InsertSelf (&interactor, fAt, fBeforeRegion->GetLength ());
    fAfterRegion->ApplySelection (&interactor);

    inherited::ReDo (interactor);
}

bool InteractiveReplaceCommand::UpdateSimpleTextInsert (size_t insertAt, Led_tChar c)
{
    AssertNotNull (fBeforeRegion);
    AssertNotNull (fAfterRegion);

    PlainTextRep* afterPTR = dynamic_cast<PlainTextRep*> (fAfterRegion);
    if (afterPTR != nullptr) {
        return afterPTR->AppendCharToRep (insertAt, c);
    }

    return false;
}

const Led_SDK_Char* InteractiveReplaceCommand::GetName () const
{
    return fCmdName.c_str ();
}

/*
 ********************************************************************************
 ******************** InteractiveReplaceCommand::SavedTextRep *******************
 ********************************************************************************
 */
void InteractiveReplaceCommand::SavedTextRep::ApplySelection (TextInteractor* imager)
{
    RequireNotNull (imager);
    imager->SetSelection (fSelStart, fSelEnd);
}

/*
 ********************************************************************************
 ******************** InteractiveReplaceCommand::PlainTextRep *******************
 ********************************************************************************
 */
InteractiveReplaceCommand::PlainTextRep::PlainTextRep (size_t selStart, size_t selEnd, const Led_tChar* text, size_t textLen)
    : inherited (selStart, selEnd)
    , fText (nullptr)
    , fTextLength (textLen)
{
    if (textLen != 0) {
        fText = new Led_tChar[textLen];
        memcpy (fText, text, textLen * sizeof (Led_tChar));
    }
}

InteractiveReplaceCommand::PlainTextRep::~PlainTextRep ()
{
    delete[] fText;
}

size_t InteractiveReplaceCommand::PlainTextRep::GetLength () const
{
    return fTextLength;
}

void InteractiveReplaceCommand::PlainTextRep::InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite)
{
    RequireNotNull (interactor);
    interactor->Replace (at, at + nBytesToOverwrite, fText, fTextLength);
}

/*
@METHOD:        InteractiveReplaceCommand::PlainTextRep::AppendCharToRep
@DESCRIPTION:   <p>Utility used internally to implement optimized undo code.</p>
*/
bool InteractiveReplaceCommand::PlainTextRep::AppendCharToRep (size_t insertAt, Led_tChar c)
{
    if (fSelStart == insertAt and fSelEnd == insertAt) {
        // could be more efficient and avoid copy - but this is already a big improvemnt over old algorithm - so lets not complain just yet...
        Led_tChar* newText = new Led_tChar[fTextLength + 1];
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
