/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_TextInteractor_inl_
#define _Stroika_Framework_Led_TextInteractor_inl_ 1

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

    //  class   TextInteractor::SuppressCommandBreaksContext
    inline TextInteractor::SuppressCommandBreaksContext::SuppressCommandBreaksContext (TextInteractor& ti)
        : fTextInteractor (ti)
        , fOldVal (ti.fSuppressCommandBreaksContext)
    {
        ti.fSuppressCommandBreaksContext = true;
    }
    inline TextInteractor::SuppressCommandBreaksContext::~SuppressCommandBreaksContext ()
    {
        fTextInteractor.fSuppressCommandBreaksContext = fOldVal;
    }

    //  class   TextInteractor::PreScrollInfo
    inline TextInteractor::PreScrollInfo::PreScrollInfo ()
        : fUpdateMode (eDefaultUpdate)
        , fOldWindowStart (0)
        , fOldHScrollPos (0)
        , fTryTodoScrollbits (false)
        , fOldLastRowStart (0)
    {
    }

    //  class   CommandNumberMapping<TARGET_COMMAND_NUMBER>
    template <typename TARGET_COMMAND_NUMBER>
    CommandNumberMapping<TARGET_COMMAND_NUMBER>* CommandNumberMapping<TARGET_COMMAND_NUMBER>::sThe = nullptr;
    template <typename TARGET_COMMAND_NUMBER>
    CommandNumberMapping<TARGET_COMMAND_NUMBER>::CommandNumberMapping ()
        : fMap ()
        , fRanges ()
    {
        Assert (sThe == nullptr);
        sThe = this;
    }
    template <typename TARGET_COMMAND_NUMBER>
    CommandNumberMapping<TARGET_COMMAND_NUMBER>::~CommandNumberMapping ()
    {
        Assert (sThe == this);
        sThe = nullptr;
    }
    template <typename TARGET_COMMAND_NUMBER>
    inline CommandNumberMapping<TARGET_COMMAND_NUMBER>& CommandNumberMapping<TARGET_COMMAND_NUMBER>::Get ()
    {
        AssertNotNull (sThe); //  Applications using Led_MFC must instantiate a subclass of MFC_CommandNumberMapping
        //  BEFORE any access to this class
        return *sThe;
    }
    template <typename TARGET_COMMAND_NUMBER>
    inline void CommandNumberMapping<TARGET_COMMAND_NUMBER>::AddAssociation (TARGET_COMMAND_NUMBER externalCommandNumber, CommandNumber internalCommandNumber)
    {
        fMap.insert (MAP_TYPE::value_type (externalCommandNumber, internalCommandNumber));
    }
    template <typename TARGET_COMMAND_NUMBER>
    inline void CommandNumberMapping<TARGET_COMMAND_NUMBER>::AddRangeAssociation (TARGET_COMMAND_NUMBER externalRangeStart, TARGET_COMMAND_NUMBER externalRangeEnd, CommandNumber internalRangeStart, CommandNumber internalRangeEnd)
    {
        Require (int (externalRangeEnd - externalRangeStart) == int (internalRangeEnd - internalRangeStart)); // ranges same length.

        RangeElt re;
        re.fExternalCmds.first  = externalRangeStart;
        re.fExternalCmds.second = externalRangeEnd;
        re.fInternalCmds.first  = internalRangeStart;
        re.fInternalCmds.second = internalRangeEnd;
        fRanges.push_back (re);
    }
    template <typename TARGET_COMMAND_NUMBER>
    TextInteractor::CommandNumber CommandNumberMapping<TARGET_COMMAND_NUMBER>::Lookup (TARGET_COMMAND_NUMBER externalCommandNumber) const
    {
        typename MAP_TYPE::const_iterator i = fMap.find (externalCommandNumber);
        if (i == fMap.end ()) {
            for (typename RANGE_VEC_TYPE::const_iterator j = fRanges.begin (); j != fRanges.end (); ++j) {
                const RangeElt& re = *j;
                if (re.fExternalCmds.first <= externalCommandNumber and externalCommandNumber <= re.fExternalCmds.second) {
                    TARGET_COMMAND_NUMBER         offset = externalCommandNumber - re.fExternalCmds.first;
                    TextInteractor::CommandNumber cmdNum = re.fInternalCmds.first + offset;
                    Assert (cmdNum <= re.fInternalCmds.second);
                    return cmdNum;
                }
            }
            return TextInteractor::kNoCommand_CmdID;
        }
        else {
            return (*i).second;
        }
    }
    template <typename TARGET_COMMAND_NUMBER>
    TARGET_COMMAND_NUMBER CommandNumberMapping<TARGET_COMMAND_NUMBER>::ReverseLookup (CommandNumber cmdNum) const
    {
        for (typename MAP_TYPE::const_iterator i = fMap.begin (); i != fMap.end (); ++i) {
            if ((*i).second == cmdNum) {
                return (*i).first;
            }
        }
        for (auto j = fRanges.begin (); j != fRanges.end (); ++j) {
            const RangeElt& re = *j;
            if (re.fInternalCmds.first <= cmdNum and cmdNum <= re.fInternalCmds.second) {
                size_t                offset = cmdNum - re.fInternalCmds.first;
                TARGET_COMMAND_NUMBER tarCmd = static_cast<TARGET_COMMAND_NUMBER> (re.fExternalCmds.first + offset);
                Assert (tarCmd <= re.fExternalCmds.second);
                return tarCmd;
            }
        }
        // Treat a command which hasn't been registered as the special value 0 indicating 'no command'
        return 0;
    }

    //  class   SimpleCommandUpdater
    inline SimpleCommandUpdater::SimpleCommandUpdater (CommandNumber cmdNum)
        : fCommandNumber (cmdNum)
        , fEnabled (false)
        , fChecked (false)
        , fText ()
    {
    }
    inline SimpleCommandUpdater::CommandNumber SimpleCommandUpdater::GetCmdID () const
    {
        return fCommandNumber;
    }
    inline bool SimpleCommandUpdater::GetEnabled () const
    {
        return fEnabled;
    }
    inline void SimpleCommandUpdater::SetEnabled (bool enabled)
    {
        fEnabled = enabled;
    }
    inline void SimpleCommandUpdater::SetChecked (bool checked)
    {
        fChecked = checked;
    }
    inline void SimpleCommandUpdater::SetText (const Led_SDK_Char* text)
    {
        RequireNotNull (text);
        fText = text;
    }

    //  class   TextInteractor::InteractiveModeUpdater
    inline TextInteractor::InteractiveModeUpdater::InteractiveModeUpdater (TextInteractor& ti, bool interactive)
        : fTextInteractor (ti)
        , fSavedMode (ti.GetInteractiveUpdateMode ())
    {
        InteractiveUpdadeMode newMode = interactive ? eInteractiveUpdateMode : eNonInteractiveUpdateMode;
        if (ti.GetInteractiveUpdateMode () == eIndeterminateInteractiveUpdateMode) {
            ti.SetInteractiveUpdateMode (newMode);
        }
        else {
            /*
                *  If the original mode was already set - make sure our new mode agrees.
                */
            Assert (ti.GetInteractiveUpdateMode () == newMode);
        }
    }
    inline TextInteractor::InteractiveModeUpdater::~InteractiveModeUpdater ()
    {
        fTextInteractor.SetInteractiveUpdateMode (fSavedMode);
    }

    //  class   TextInteractor::SmartCNPInfo
    inline TextInteractor::SmartCNPInfo::SmartCNPInfo ()
        : fWordBreakAtSelStart (false)
        , fWordBreakAtSelEnd (false)
    {
    }

    //  class   TextInteractor
    /*
    @METHOD:        TextInteractor::GetCommandHandler
    @DESCRIPTION:   <p>TextInteractor's have associated an optional @'CommandHandler', used for maintaining UNDO
        information. This can return nullptr, if no CommandHandler is currently associated (meaning no Undo is avialable).</p>
    */
    inline CommandHandler* TextInteractor::GetCommandHandler () const
    {
        return fCommandHandler;
    }
    /*
    @METHOD:        TextInteractor::SetCommandHandler
    @DESCRIPTION:   <p>See @'TextInteractor::GetCommandHandler'.</p>
    */
    inline void TextInteractor::SetCommandHandler (CommandHandler* commandHandler)
    {
        fCommandHandler = commandHandler;
    }
    /*
    @METHOD:        TextInteractor::BreakInGroupedCommands
    @DESCRIPTION:   <p>Trivial helper - delegates to @'CommandHandler::BreakInGroupedCommands' if
                @'TextInteractor::GetCommandHandler' returns non-nullptr.
                See also @'TextInteractor::SuppressCommandBreaksContext'</p>
    */
    inline void TextInteractor::BreakInGroupedCommands ()
    {
        if (fCommandHandler != nullptr and not fSuppressCommandBreaksContext) {
            fCommandHandler->BreakInGroupedCommands ();
        }
    }
    /*
    @METHOD:        TextInteractor::BreakInGroupedCommandsIfDifferentCommand
    @DESCRIPTION:   <p>Trivial helper - delegates to @'CommandHandler::BreakInGroupedCommandsIfDifferentCommand' if
                @'TextInteractor::GetCommandHandler' returns non-nullptr.
                See also @'TextInteractor::SuppressCommandBreaksContext'</p>
    */
    inline void TextInteractor::BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName)
    {
        if (fCommandHandler != nullptr and not fSuppressCommandBreaksContext) {
            fCommandHandler->BreakInGroupedCommandsIfDifferentCommand (cmdName);
        }
    }
    /*
    @METHOD:        TextInteractor::GetDialogSupport
    @DESCRIPTION:   <p></p>
    */
    inline TextInteractor::DialogSupport& TextInteractor::GetDialogSupport ()
    {
        if (sDialogSupport == nullptr) {
            static DialogSupport sDefSup;
            sDialogSupport = &sDefSup;
        }
        return *sDialogSupport;
    }
    /*
    @METHOD:        TextInteractor::SetDialogSupport
    @DESCRIPTION:   <p></p>
    */
    inline void TextInteractor::SetDialogSupport (DialogSupport* ds)
    {
        sDialogSupport = ds;
    }
    /*
    @METHOD:        TextInteractor::GetCommandNames
    @DESCRIPTION:   <p>Returns command name for each of the user-visible commands produced by this module.
        This name is used used in the constructed Undo command name, as
        in, "Undo Paste". You can replace this name with whatever you like.You change this value with
        WordProcessor::SetCommandNames.</p>
            <p> The point of this is to allow for different UI-language localizations,
                without having to change Led itself.</p>
            <p>See also @'TextInteractor::CommandNames'.</p>
    */
    inline const TextInteractor::CommandNames& TextInteractor::GetCommandNames ()
    {
        return sCommandNames;
    }
    /*
    @METHOD:        TextInteractor::SetCommandNames
    @DESCRIPTION:   <p>See @'TextInteractor::GetCommandNames'.</p>
    */
    inline void TextInteractor::SetCommandNames (const TextInteractor::CommandNames& cmdNames)
    {
        sCommandNames = cmdNames;
    }
    /*
    @METHOD:        TextInteractor::GetSpellCheckEngine
    @DESCRIPTION:   <p>Return a pointer to the current @'SpellCheckEngine' if there is one. It CAN BE nullptr - meaning
                that spellchecking is disabled. See @'TextInteractor::SetSpellCheckEngine'.</p>
    */
    inline SpellCheckEngine* TextInteractor::GetSpellCheckEngine () const
    {
        return fSpellCheckEngine;
    }
    /*
    @METHOD:        TextInteractor::SetSpellCheckEngine
    @DESCRIPTION:   <p>This is typically called by an application or document to associate a spellcheck engine with the
                @'TextInteractor'. That caller still 'owns' the object and must manage its lifetime, and to reset this
                value (in this class) to nullptr before destorying the @'SpellCheckEngine'. If you have an option to turn on/off
                spellchecking, when you turn it off, you would typically just call this method with nullptr to disable
                spellchecking command funcitonality. Be sure to set this property to nullptr before destruction.</p>
                    <p>See @'TextInteractor::GetSpellCheckEngine'.</p>
    */
    inline void TextInteractor::SetSpellCheckEngine (SpellCheckEngine* spellCheckEngine)
    {
        fSpellCheckEngine = spellCheckEngine;
    }
    /*
    @METHOD:        TextInteractor::GetDefaultUpdateMode
    @DESCRIPTION:   <p>TextInteractor's have an associated default UpdateMode. This is the update mode which is
                used by methods which used the eDefaultUpdateMode argument (most default UpdateMode args in Led are this value).</p>
                    <p>This value should <em>not</em> be set directly. Instead, instantiate a
                @'TextInteractor::TemporarilySetUpdateMode' object on
                the stack to temporarily set the default update mode.</p>
    */
    inline TextInteractor::UpdateMode TextInteractor::GetDefaultUpdateMode () const
    {
        Ensure (fDefaultUpdateMode != eDefaultUpdate);
        return fDefaultUpdateMode;
    }
    /*
    @METHOD:        TextInteractor::RealUpdateMode
    @DESCRIPTION:   <p>TextInteractor's have an associated default UpdateMode. This utility method takes an arbitrary
                UpdateMode, and returns either its argument, or if it was eDefaultUpdateMode, it returns the current value
                of the default update mode.</p>
    */
    inline TextInteractor::UpdateMode TextInteractor::RealUpdateMode (UpdateMode updateMode) const
    {
        return updateMode == eDefaultUpdate ? GetDefaultUpdateMode () : updateMode;
    }
    /*
    @METHOD:        TextInteractor::GetSmartCutAndPasteMode
    @DESCRIPTION:   <p>TextInteractor's support a UI feature called "smart cut & paste". Basicly, Led tries to guess
                what a user 'really meant todo' with surrounding whitespace when dragging, and cutting and pasting text.</p>
                    <p> This can sometimes be extremely helpful. Othertimes, it can be extremely anoying. You decide. Thats
                why its an option.</p>
                    <p>And see @'TextInteractor::OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds' and
                @'TextInteractor::OptionallyExpandSelectionForSmartCutAndPasteModeDeletes' for the implementation of
                smart cut and paste.</p>
    */
    inline bool TextInteractor::GetSmartCutAndPasteMode () const
    {
        return fSmartCutAndPasteMode;
    }
    /*
    @METHOD:        TextInteractor::SetSmartCutAndPasteMode
    @DESCRIPTION:   <p>Set the 'smart cut and paste' mode. See TextInteractor::GetSmartCutAndPasteMode for more details.</p>
    */
    inline void TextInteractor::SetSmartCutAndPasteMode (bool smartCutAndPasteMode)
    {
        fSmartCutAndPasteMode = smartCutAndPasteMode;
    }
    /*
    @METHOD:        TextInteractor::GetCurClickCount
    @DESCRIPTION:   <p>Used to keep track of the number of clicks the user has done very recently. Used for determining
                double and triple clicks. If the user clicks, and then clicks a long time later - it wont be considered
                a double click - and so this value will remain one.</p>
    */
    inline unsigned TextInteractor::GetCurClickCount () const
    {
        return fClickCount;
    }
    /*
    @METHOD:        TextInteractor::SetCurClickCount
    @DESCRIPTION:   <p>See also @'TextInteractor::GetCurClickCount' (). This is seldom called directly - except perhaps to RESET
                the count to zero or one. This is typically just called internally through @'TextInteractor::UpdateClickCount'.</p>
    */
    inline void TextInteractor::SetCurClickCount (unsigned curClickCount, Foundation::Time::DurationSecondsType lastClickAt)
    {
        fClickCount    = curClickCount;
        fLastClickedAt = lastClickAt;
    }
    /*
    @METHOD:        TextInteractor::IncrementCurClickCount
    @DESCRIPTION:   <p>See also @'TextInteractor::GetCurClickCount' () and @'TextInteractor::SetCurClickCount' ().
                This is typically just called internally through @'TextInteractor::UpdateClickCount'.</p>
    */
    inline void TextInteractor::IncrementCurClickCount (Foundation::Time::DurationSecondsType lastClickAt)
    {
        fClickCount++;
        fLastClickedAt = lastClickAt;
    }
    /*
    @METHOD:        TextInteractor::IsWholeWindowInvalid
    @DESCRIPTION:   <p>Sometimes we can go through alot of computation to see just what minimal area of the window needs
                to be updated. Sometimes this can be quite helpful, and allow things to run quickly. But other times,
                like when we are making many changes to the text before ever showing anything, it would be just faster
                to mark the whole screen as dirty, and to avoid the computation about what region needs updating.
                Thats what these APIs here are about.</p>
                    <p>You can tell the @'TextInteractor' that the whole window is dirty explicitly by simply doing a Refresh()
                with no arguments (really anything which causes whole window rect to become invalid).
                You can query if the whole window is invalid. This only returns true if the whole window is KNOWN
                to be invalid. It maybe that there is some complex update region which happens to cover the whole window
                or that something has happened that we don't know about to make the entire window invalid. We will
                just return false in those cases. But you can count on the fact that if IsWholeWindowInvalid() returns
                true - this means you need not do any bookkeeping/work to compute just what region you will be
                updating.</p>
                    <p>We OVERRIDE the Draw (const Led_Rect& subsetToDraw, bool printing) method to call
                    @'TextInteractor::NoteWindowPartiallyUpdated' ().
                (clearning the fWholeWindowInvalid bool). If subclassers who OVERRIDE Draw(), or otherwise update the
                screen (calling ValidRect ()) - should be sure to call NoteWindowPartiallyUpdated ();</p>
                    <p>Led supports a redraw optimization, whereby complex calculations about what region of the screen
                needs to be invalidated can be avoided, if we keep track of, and cache the notion that the whole screen is
                already fully invalidated. This method is called internally to avoid complex calculations. You may <em>rarely</em>
                want to call it as well, if profiling yields a hot-spot in your code, in a calculation of something that need not
                be done if the screen has already been marked as invalid.</p>
    */
    inline bool TextInteractor::IsWholeWindowInvalid () const
    {
        return fWholeWindowInvalid;
    }
    /*
    @METHOD:        TextInteractor::NoteWholeWindowIsInvalid
    @DESCRIPTION:   <p>See @'TextInteractor::IsWholeWindowInvalid'. This method marks the screen as
                already having been invalidated. Called internally by Led. Only call this with extreme
                caution. Should very rarely be appropriate, outside of class library wrappers.</p>
    */
    inline void TextInteractor::NoteWholeWindowIsInvalid ()
    {
        fWholeWindowInvalid = true;
    }
    /*
    @METHOD:        TextInteractor::NoteWindowPartiallyUpdated
    @DESCRIPTION:   <p>See @'TextInteractor::IsWholeWindowInvalid'. This method marks the screen as having been
                at least partially redrawn, so the 'IsWholeWindowInvalid' no longer apply.</p>
                    <p>Called internally by Led. Only call this with extreme caution. Should very rarely be
                appropriate, outside of class library wrappers.</p>
    */
    inline void TextInteractor::NoteWindowPartiallyUpdated ()
    {
        fWholeWindowInvalid = false;
    }
    /*
    @METHOD:        TextInteractor::Refresh
    @DESCRIPTION:   <p>Invalidate the entire window rectangle (@'TextImager::GetWindowRect') in a
                manner dependingon the 'updateMode' (see @'TextInteractor::UpdateMode'). Delegates
                implemantion to @'TextInteractor::Refresh_', which is generally overriden in
                SDK-specific wrapper subclass.</p>
    */
    inline void TextInteractor::Refresh (UpdateMode updateMode) const
    {
        updateMode = RealUpdateMode (updateMode);
        if (updateMode != eNoUpdate) {
            if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
                return;
            }
            RefreshWindowRect_ (GetWindowRect (), updateMode);
            // wrong - cuz above may not have invaled anything if ??? Hmm.. maybe under some circomstances???
            if (updateMode == eDelayedUpdate) {
                const_cast<TextInteractor*> (this)->NoteWholeWindowIsInvalid ();
            }
        }
    }
    /*
    @METHOD:        TextInteractor::RefreshWindowRect_
    @DESCRIPTION:   <p>Cause the given region of the edit window to be updated. The given 'windowRectArea'
                is given in the same coordinates as the window rect
                specified by @'TextImager::SetWindowRect': it is not relative to that rectangle.</p>
                    <p>This simply calls @'TextInteractor::RefreshWindowRect_' to delegate its implementation.</p>
    */
    inline void TextInteractor::RefreshWindowRect (const Led_Rect& windowRectArea, UpdateMode updateMode) const
    {
        updateMode = RealUpdateMode (updateMode);
        if (updateMode != eNoUpdate) {
            if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
                return;
            }
            RefreshWindowRect_ (windowRectArea, updateMode);
            if (updateMode == eDelayedUpdate and windowRectArea.Contains (GetWindowRect ())) {
                const_cast<TextInteractor*> (this)->NoteWholeWindowIsInvalid ();
            }
        }
    }
    /*
    @METHOD:        TextInteractor::Update
    @DESCRIPTION:   <p>Calls @'TextInteractor::UpdateWindowRect' on the entire windowrect.</p>
    */
    inline void TextInteractor::Update (bool ignoreCannotUpdateNowErrors) const
    {
        try {
            UpdateWindowRect_ (GetWindowRect ());
        }
        catch (CannotUpdateNow&) {
            if (not ignoreCannotUpdateNowErrors) {
                throw;
            }
        }
        catch (...) {
            throw;
        }
    }
    /*
    @METHOD:        TextInteractor::UpdateWindowRect
    @DESCRIPTION:   <p>Makes sure the given section of the window rect is updated (redisplayed). This MAY
                not be possible at a given time, in whcih case, @'TextInteractor::CannotUpdateNow' maybe
                thrown instead.</p>
    */
    inline void TextInteractor::UpdateWindowRect (const Led_Rect& windowRectArea, bool ignoreCannotUpdateNowErrors) const
    {
        try {
            UpdateWindowRect_ (windowRectArea);
        }
        catch (CannotUpdateNow&) {
            if (not ignoreCannotUpdateNowErrors) {
                throw;
            }
        }
        catch (...) {
            throw;
        }
    }
    /*
    @METHOD:        TextInteractor::GetUseSecondaryHilight
    @DESCRIPTION:   <p>See @'TextInteractor::SetUseSecondaryHilight'.</p>
    */
    inline bool TextInteractor::GetUseSecondaryHilight () const
    {
        return fUseSecondaryHilight;
    }
    /*
    @METHOD:        TextInteractor::SetUseSecondaryHilight
    @DESCRIPTION:   <p>If this is set true (false by default) then when a window is deactivated, it will draw
                its selection hilight as an outline - rather than just not displaying it.</p>
                    <p>See also @'TextInteractor::GetUseSecondaryHilight'.</p>
    */
    inline void TextInteractor::SetUseSecondaryHilight (bool useSecondaryHilight)
    {
        if (fUseSecondaryHilight != useSecondaryHilight) {
            Refresh ();
            fUseSecondaryHilight = useSecondaryHilight;
        }
    }
    inline bool TextInteractor::GetUseBitmapScrollingOptimization () const
    {
        return fUseBitmapScrollingOptimization;
    }
    inline void TextInteractor::SetUseBitmapScrollingOptimization (bool useBitmapScrollingOptimization)
    {
        fUseBitmapScrollingOptimization = useBitmapScrollingOptimization;
    }
    inline bool TextInteractor::GetSuppressTypedControlCharacters () const
    {
        return fSuppressTypedControlCharacters;
    }
    inline void TextInteractor::SetSuppressTypedControlCharacters (bool suppressTypedControlCharacters)
    {
        fSuppressTypedControlCharacters = suppressTypedControlCharacters;
    }
    /*
    @METHOD:        TextInteractor::GetInteractiveUpdateMode
    @DESCRIPTION:   <p>See also @'TextInteractor::CheckIfCurrentUpdateIsInteractive'.
    */
    inline TextInteractor::InteractiveUpdadeMode TextInteractor::GetInteractiveUpdateMode () const
    {
        return fInteractiveUpdadeMode;
    }
    /*
    @METHOD:        TextInteractor::SetInteractiveUpdateMode
    @DESCRIPTION:   <p>See also @'TextInteractor::CheckIfCurrentUpdateIsInteractive'.
    */
    inline void TextInteractor::SetInteractiveUpdateMode (InteractiveUpdadeMode interactive)
    {
        fInteractiveUpdadeMode = interactive;
    }
    /*
    @METHOD:        TextInteractor::CheckIfCurrentUpdateIsInteractive
    @DESCRIPTION:   <p>See also @'TextInteractor::InteractiveUpdadeMode' and @'TextInteractor::SetInteractiveUpdateMode' ().
    */
    inline bool TextInteractor::CheckIfCurrentUpdateIsInteractive () const
    {
        // Interpret 'indeterminate' as NOT being interactive mode
        return fInteractiveUpdadeMode == eInteractiveUpdateMode;
    }
    /*
    @METHOD:        TextInteractor::GetInternalizer
    @DESCRIPTION:
    */
    inline shared_ptr<FlavorPackageInternalizer> TextInteractor::GetInternalizer () const
    {
        return fInternalizer;
    }
    /*
    @METHOD:        TextInteractor::SetInternalizer
    @DESCRIPTION:   <p>Calls @'TextInteractor::HookInternalizerChanged' whenever the internalizer changes. Can be a nullptr externalizer.</p>
    */
    inline void TextInteractor::SetInternalizer (const shared_ptr<FlavorPackageInternalizer>& i)
    {
        fInternalizer = i;
        HookInternalizerChanged ();
    }
    /*
    @METHOD:        TextInteractor::GetExternalizer
    @DESCRIPTION:
    */
    inline shared_ptr<FlavorPackageExternalizer> TextInteractor::GetExternalizer () const
    {
        return fExternalizer;
    }
    /*
    @METHOD:        TextInteractor::SetExternalizer
    @DESCRIPTION:   <p>Calls @'TextInteractor::HookExternalizerChanged' whenever the externalizer changes. Can be a nullptr externalizer.</p>
    */
    inline void TextInteractor::SetExternalizer (const shared_ptr<FlavorPackageExternalizer>& e)
    {
        fExternalizer = e;
        HookExternalizerChanged ();
    }
    inline void TextInteractor::InvalidateScrollBarParameters_ ()
    {
        fScrollBarParamsValid = false;
    }
    inline void TextInteractor::UpdateScrollBars_ ()
    {
        fScrollBarParamsValid = true;
    }
    /*
    @METHOD:        TextInteractor::GetCaretShown
    @DESCRIPTION:   <p>Flag to control whether or not blinking caret is shown for this editor.
                Typically it will be turned on when this widget gets the focus, and off when it
                loses it. But that is the responsability of higher level software (subclasses).
                </p>
                    <p>See also @'TextInteractor::GetCaretShownSituation' and @'TextInteractor::SetCaretShown'.</p>
    */
    inline bool TextInteractor::GetCaretShown () const
    {
        return (fCaretShown);
    }
    /*
    @METHOD:        TextInteractor::GetCaretShownAfterPos
    @DESCRIPTION:   <p>Given an empty selection, it seems pretty obvious where to place
                the caret - right? NOT!!!</p>
                    <p>Remember that a selection position falls BETWEEN two characters. One
                plausible definition is to use the RIGHT edge of the preceeding character.
                Yet - ANOTHER plausible definition is to use the LEFT edge of the following
                character. For some particular positions (notable the beginning and ends of
                the text) or or the other of these definitions might not quite make sense.
                But there are obvious definitions to take care of these special cases.</p>
                    <p>And - luckily - for the most part - these definitions coincide. But there
                is at least one place which comes up (in normal editor usage - more in
                fancy Led subclasses like LVEJ's side-by-side mode). In particular, when
                the preceeding and following characters fall on different rows. In this
                case, a hint is helpful in deciding which of the two approaches to use.</p>
                    <p>Many editors simply pick one approach or the other. Led did this for a while.
                It always picked the leading edge of the following character. But now Led
                has a option. When users change the selection by clicking, this option is
                is automaticly set. And similarly when they use the arrow keys, or do other
                editing operations. The set-method for this field is virtual, so subclassers
                who prefer one definition over the other can OVERRIDE the set method to simply
                force a particular value.</p>
                    <p>If GetCaretShownAfterPos is true, use the use the LHS of following character,
                and if false, then the RHS of the preceeding character. So the after refers to which
                character to use - the one before or after the given marker POS. The side of the charater is the opposite
                of this value.</p>
        */
    inline bool TextInteractor::GetCaretShownAfterPos () const
    {
        return (fCaretShownAfterPos);
    }
    /*
    @METHOD:        TextInteractor::GetScrollBarType
    @DESCRIPTION:
                    <p>See also 'TextInteractor::SetScrollBarType'</p>
    */
    inline TextInteractor::ScrollBarType TextInteractor::GetScrollBarType (VHSelect vh) const
    {
        return fScrollBarType[vh];
    }
    inline void TextInteractor::SetScrollBarType_ (VHSelect vh, ScrollBarType scrollBarType)
    {
        fScrollBarType[vh] = scrollBarType;
    }
    inline void TextInteractor::UpdateIfNoKeysPending ()
    {
        if (QueryInputKeyStrokesPending ()) {
            // Note - this is NOT strictly necesary todo. But rather - its a performance hack. This call takes
            // very little time, and it allows LATER optimizations (see TextInteractor::IsWholeWindowInvalid ());
            // This was done in response to SPR#0659. I never measured exactly how much of a speedup it produced,
            // but it eliminated stuff from my profiles so I went on to bigger things... LGP 991214
            Refresh ();
        }
        else {
            Update ();
        }
    }

    //  class   TextInteractor::SearchParameters
    inline TextInteractor::SearchParameters::SearchParameters ()
        : inherited ()
        , fRecentFindStrings ()
    {
    }

    //  class   TextInteractor::ReplaceParameters
    inline TextInteractor::ReplaceParameters::ReplaceParameters ()
        : fReplaceWith ()
    {
    }

    //  class   TextInteractor::UndoableContextHelper
    /*
    @METHOD:        TextInteractor::UndoableContextHelper::GetUndoRegionStart
    @DESCRIPTION:   <p>This method was formerly called 'GetSelStart'. However - this value - though originally based on the selection -
                can be slightly different - especially in light of smart-cut-and-paste. It is the area that is being changed,
                based on the users original selection.</p>
    */
    inline size_t TextInteractor::UndoableContextHelper::GetUndoRegionStart () const
    {
        return fSelStart;
    }
    /*
    @METHOD:        TextInteractor::UndoableContextHelper::GetUndoRegionEnd
    @DESCRIPTION:   <p>@'See @'TextInteractor::UndoableContextHelper::GetUndoRegionStart'</p>
    */
    inline size_t TextInteractor::UndoableContextHelper::GetUndoRegionEnd () const
    {
        return fSelEnd;
    }
    /*
    @METHOD:        TextInteractor::UndoableContextHelper::GetSimplePlainTextInsertOptimization
    @DESCRIPTION:   <p>@'See @'TextInteractor::UndoableContextHelper::SetSimplePlainTextInsertOptimization'</p>
    */
    inline bool TextInteractor::UndoableContextHelper::GetSimplePlainTextInsertOptimization () const
    {
        return fSimplePlainTextInsertOptimization;
    }
    /*
    @METHOD:        TextInteractor::UndoableContextHelper::SetSimplePlainTextInsertOptimization
    @DESCRIPTION:   <p>This should rarely be called. Its called internally just in the context where we are doing a special
                form of text insert to create an optimized representation for undo information (in a common context).
                </p>
                    <p>If this is to be called - it <em>must</em> be called before calling @'TextInteractor::UndoableContextHelper::CommandComplete'.</p>
                    <p>@'See also @'TextInteractor::UndoableContextHelper::GetSimplePlainTextInsertOptimization'</p>
    */
    inline void TextInteractor::UndoableContextHelper::SetSimplePlainTextInsertOptimization (bool simplePlainTextInsertOptimizationFlag)
    {
        fSimplePlainTextInsertOptimization = simplePlainTextInsertOptimizationFlag;
    }

    //  class   TextInteractor::TemporarilySetUpdateMode
    inline TextInteractor::TemporarilySetUpdateMode::TemporarilySetUpdateMode (TextInteractor& ti, UpdateMode tmpUpdateMode)
        : fTextInteractor (ti)
        , fOldValue (ti.GetDefaultUpdateMode ())
    {
        fTextInteractor.SetDefaultUpdateMode (tmpUpdateMode);
    }
    inline TextInteractor::TemporarilySetUpdateMode::~TemporarilySetUpdateMode ()
    {
        fTextInteractor.SetDefaultUpdateMode (fOldValue);
    }

}

#endif /*_Stroika_Framework_Led_TextInteractor_inl_*/
