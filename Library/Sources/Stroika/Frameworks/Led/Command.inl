/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_Command_inl_
#define _Stroika_Framework_Led_Command_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
    //  class   SnoopingCommandHandler;
    /*
    @METHOD:        Command::Command
    @DESCRIPTION:   <p>Construct a Command object. The constructor takes an optional bool arg <code>done</code> because
        sometimes commands are created already done (but this defaults to false).</p>
    */
    inline Command::Command (bool done)
        : fDone (done)
    {
    }
    /*
    @METHOD:        Command::Do
    @DESCRIPTION:   <p>Perform the actual commands action.</p>
    */
    inline void Command::Do (TextInteractor& /*interactor*/)
    {
        fDone = true;
    }
    /*
    @METHOD:        Command::UnDo
    @DESCRIPTION:   <p>Undo an already done command.</p>
    */
    inline void Command::UnDo (TextInteractor& /*interactor*/)
    {
        fDone = false;
    }
    /*
    @METHOD:        Command::ReDo
    @DESCRIPTION:   <p>Redo a command which has been done (or perhaps redone), and more recently undone.</p>
    */
    inline void Command::ReDo (TextInteractor& /*interactor*/)
    {
        fDone = true;
    }
    /*
    @METHOD:        Command::GetDone
    @DESCRIPTION:   <p>The done, and redone states are considered identical. This returns true if the command has either
        been done, or redone (not most recently undone).</p>
    */
    inline bool Command::GetDone () const
    {
        return fDone;
    }

    //  class   SnoopingCommandHandler;
    inline CommandHandler* SnoopingCommandHandler::GetRealHandler () const
    {
        return fRealHandler;
    }

    //  class   MultiLevelUndoCommandHandler;
    /*
    @METHOD:        MultiLevelUndoCommandHandler::GetMaxUnDoLevels
    @DESCRIPTION:   <p>Gets the maximum number of undo levels the @'MultiLevelUndoCommandHandler' will support.
        Zero is a valid # - disabling the keeping of undo information. One is a valid number - implying
        basicly the same behavior as @'SingleUndoCommandHandler' - except that Undo isn't treated as Redo
        after an Undo.</p>
            <p>See @'MultiLevelUndoCommandHandler::SetMaxUnDoLevels'</p>

    */
    inline size_t MultiLevelUndoCommandHandler::GetMaxUnDoLevels ()
    {
        return fMaxUndoLevels;
    }

    //  class   InteractiveReplaceCommand::SavedTextRep
    inline InteractiveReplaceCommand::SavedTextRep::SavedTextRep (size_t selStart, size_t selEnd)
        : fSelStart (selStart)
        , fSelEnd (selEnd)
    {
    }
    inline InteractiveReplaceCommand::SavedTextRep::~SavedTextRep ()
    {
    }

}

#endif /*_Stroika_Framework_Led_Command_inl_*/
