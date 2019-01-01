/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_TextInteractor_h_
#define _Stroika_Frameworks_Led_TextInteractor_h_ 1

#include "../../Foundation/StroikaPreComp.h"

#include "../../Foundation/Time/Realtime.h"

/*
@MODULE:    TextInteractor
@DESCRIPTION:
        <p>This module defines the @'TextInteractor' class, which subclasses from @'TextImager'
    and introduces user-interaction
    behavior - such as keyclicks, mouse tracking, and the notion of @'TextInteractor::UpdateMode' s.</p>
 */

#include "Command.h"
#include "FlavorPackage.h"
#include "SpellCheckEngine.h"
#include "TextImager.h"

namespace Stroika::Frameworks::Led {

// Private Led Macro utility to define command numbers (cannot use Math::RoundUpTo use used to init enums - at least on msvc2k17)
#define RoundUpToNearest_CMDNUMs_MACRO(x, n) (((x + 1) & ~(n - 1)) + n)

    /*
    @CLASS:         TextInteractor
    @BASES:         virtual @'TextImager'
    @DESCRIPTION:   <p>TextInteractors are special @'TextImager's which respond to events
                (such as keyclicks), handle mouse tracking,
                and are designed to work with some sort of windowing system. They have a
                concept of Update events, and having their content data out of sync with
                that which is displayed in the window (inval/validate region). They also
                provide support for things like a selection, and cursoring through the
                selection, and word selection, etc.</p>

                    <p>Note that TextInteractors are still abstract classes, and know nothing
                about particular windowing systems, or class libraries. They do what can
                be done genericly, without having yet made a choice about these things.
                TextInteractor mainly serves to collect common code/functionality which
                can be shared accross (for example) MFC/OpenDoc/TCL/PowerPlant/PowerPlant, etc.
                See @'Led_PPView', @'Led_MacOS_Helper<BASE_INTERACTOR>', @'Led_MFC', @'Led_Win32_Helper<BASE_INTERACTOR>'
                @'Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>', etc for more information on class-library-specific
                integration.</p>
    */
    class TextInteractor : public virtual TextImager {
    protected:
        TextInteractor ();

    public:
        virtual ~TextInteractor ();

        /*
         *  By default we have none. If you set one, it will be notified/used for undo
         *  support. It is callers responsability to free the command handler, and must be
         *  done BEFORE we are destroyed, and we must be notified (via SetCommandHandler(nullptr))
         *  BEFORE CommandHandler is destroyed.
         */
    public:
        nonvirtual CommandHandler* GetCommandHandler () const;
        virtual void               SetCommandHandler (CommandHandler* commandHandler);

    private:
        CommandHandler* fCommandHandler;

    public:
        nonvirtual void BreakInGroupedCommands ();
        nonvirtual void BreakInGroupedCommandsIfDifferentCommand (const Led_SDK_String& cmdName);

    public:
        class SuppressCommandBreaksContext;

    private:
        bool fSuppressCommandBreaksContext;

    public:
        struct CommandNames;

        // This class builds commands with command names. The UI may wish to change these
        // names (eg. to customize for particular languages, etc)
        // Just patch these strings here, and commands will be created with these names.
        // (These names appear in text of undo menu item)
    public:
        static const CommandNames& GetCommandNames ();
        static void                SetCommandNames (const CommandNames& cmdNames);
        static CommandNames        MakeDefaultCommandNames ();

    private:
        static CommandNames sCommandNames;

    public:
        using CommandNumber = int;

    public:
#if qCannotStaticlyInitializeConstantsDataMembersInClassDeclaration
        enum {kNoCommand_CmdID = 0};
#else
        static const CommandNumber kNoCommand_CmdID = 0;
#endif

    public:
        enum {
            kTextInteractorCommand_First = 0x100,

            kUndo_CmdID = kTextInteractorCommand_First,
            kRedo_CmdID,
            kSelectAll_CmdID,
            kCut_CmdID,
            kCopy_CmdID,
            kPaste_CmdID,
            kClear_CmdID,

            kSelectWord_CmdID,
            kSelectTextRow_CmdID,
            kSelectParagraph_CmdID,

            kFind_CmdID,
            kFindAgain_CmdID,
            kEnterFindString_CmdID,
            kReplace_CmdID,
            kReplaceAgain_CmdID,

            kSpellCheck_CmdID,

            kTextInteractorCommand_Last = kTextInteractorCommand_First + 0xff
        };

    public:
        class CommandUpdater;

    public:
        class DialogSupport;
        static DialogSupport& GetDialogSupport ();
        static void           SetDialogSupport (DialogSupport* ds);

    private:
        static DialogSupport* sDialogSupport;

    public:
        virtual bool OnUpdateCommand (CommandUpdater* enabler);
        virtual bool OnPerformCommand (CommandNumber commandNumber);

    public:
        virtual void OnUpdateCutCopyClearCommand (CommandUpdater* enabler);
        virtual void OnUpdatePasteCommand (CommandUpdater* enabler);
        virtual void OnUpdateUndoRedoCommand (CommandUpdater* enabler);

    public:
        virtual void OnUpdateSelectTextCommand (CommandUpdater* enabler);
        virtual void OnPerformSelectTextCommand (CommandNumber commandNumber);

        // Find related commands
    public:
        class SearchParameters;
        class ReplaceParameters;

    public:
        virtual void OnFindCommand ();
        virtual void OnFindAgainCommand ();
        virtual void OnEnterFindString ();
        virtual void OnReplaceCommand ();
        virtual void OnReplaceAgainCommand ();
        virtual void OnDoReplaceCommand (const SearchParameters& searchFor, const Led_tString& replaceWith);
        virtual void OnDoReplaceAllCommand (const SearchParameters& searchFor, const Led_tString& replaceWith);
        virtual void OnDoReplaceAllInSelectionCommand (const SearchParameters& searchFor, const Led_tString& replaceWith);
        virtual void OnUpdateFindCommands (CommandUpdater* enabler);

    private:
        static SearchParameters sSearchParameters;

    public:
        virtual SearchParameters GetSearchParameters () const;
        virtual void             SetSearchParameters (const SearchParameters& sp);

    private:
        static ReplaceParameters sReplaceParameters;

    public:
        virtual ReplaceParameters GetReplaceParameters () const;
        virtual void              SetReplaceParameters (const ReplaceParameters& rp);

    protected:
        virtual vector<Led_tString> MergeRecentFindStrings (const Led_tString& s, const vector<Led_tString>& oldRecents);

    public:
        virtual void OnSpellCheckCommand ();
        virtual void OnUpdateSpellCheckCommand (CommandUpdater* enabler);

    public:
        nonvirtual SpellCheckEngine* GetSpellCheckEngine () const;
        nonvirtual void              SetSpellCheckEngine (SpellCheckEngine* spellCheckEngine);

    private:
        SpellCheckEngine* fSpellCheckEngine;

    protected:
        class UndoableContextHelper;

        friend class UndoableContextHelper;

    public:
        /*
        @CLASS:         TextInteractor::UpdateMode
        @DESCRIPTION:   <p>Support for controlling how/when the window is updated. This is a key difference between
            a @'TextImager' and a @'TextInteractor'. A @'TextInteractor' introduces the concept of a real GUI
            window that can get out of data, and needs to be updated. These updateModes control
            the basic updating/timing strategies for redrawing portions of the textimager.</p>
        */
        enum UpdateMode { eDelayedUpdate,
                          eImmediateUpdate,
                          eNoUpdate,
                          eDefaultUpdate };

        /*
            #if 0
            *  Sometimes its convenient to globally (perhaps modally is a better word) disable
            *  updates to a TextImager. This is convenient in code-reuse, occasionally (where
            *  you want to re-use a version of a routine with an UpdateMode, but you will handle
            *  the updating yourself, and you cannot call the routine with the Updatemode directly).
            *
            *  To avoid the possability (likelihood) of accidentally leaving updates disabled, you cannot set this
            *  directly, but only via a stack-based helper class. Just instantiate a TemporarilySetUpdateMode
            *  to disable updates.
            #endif
            */
    public:
        nonvirtual UpdateMode GetDefaultUpdateMode () const;
        nonvirtual UpdateMode RealUpdateMode (UpdateMode updateMode) const;

    protected:
        virtual void SetDefaultUpdateMode (UpdateMode defaultUpdateMode);

    private:
        UpdateMode fDefaultUpdateMode;

    public:
        class TemporarilySetUpdateMode;
        friend class TemporarilySetUpdateMode;

    public:
        nonvirtual void Refresh (UpdateMode updateMode = eDefaultUpdate) const;
        nonvirtual void RefreshWindowRect (const Led_Rect& windowRectArea, UpdateMode updateMode = eDefaultUpdate) const;
        nonvirtual void Refresh (size_t from, size_t to, UpdateMode updateMode = eDefaultUpdate) const;
        nonvirtual void Refresh (const Marker* range, UpdateMode updateMode = eDefaultUpdate) const;

    protected:
        /*
        @METHOD:        TextInteractor::RefreshWindowRect_
        @DESCRIPTION:   <p>pure virtual method called by @'TextInteractor::RefreshWindowRect_'. Generally
                    OVERRIDE in SDK-specific wrapper subclass.</p>
                        <p>The given 'windowRectArea' is given in the same coordinates as the window rect
                    specified by @'TextImager::SetWindowRect': it is not relative to that rectangle.</p>
        */
        virtual void RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const = 0;

        /*
            *  When doing "Cut" (and related commands like drag), remove extra surrounding whitespace.
            *  And when doing "Paste" (and related commands like drop), add in any appropriate surrounding whitespace.
            */
    public:
        nonvirtual bool GetSmartCutAndPasteMode () const;
        nonvirtual void SetSmartCutAndPasteMode (bool smartCutAndPasteMode);

    private:
        bool fSmartCutAndPasteMode;

    protected:
        struct SmartCNPInfo {
            SmartCNPInfo ();

            bool fWordBreakAtSelStart;
            bool fWordBreakAtSelEnd;
        };

    protected:
        nonvirtual bool LooksLikeSmartPastableText (const Led_tChar* text, size_t nTextTChars, SmartCNPInfo* smartCNPInfo) const;
        nonvirtual void OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (size_t selStart, const SmartCNPInfo& smartCNPInfo);
        nonvirtual void OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (size_t* selStart, size_t* selEnd);

    public:
        virtual void SetSelectionShown (bool shown) override;
        virtual void SetSelectionShown (bool shown, UpdateMode updateMode);

    public:
        nonvirtual void ScrollToSelection (UpdateMode updateMode = eDefaultUpdate, bool forceShowSelectionEndpoint = false);

    protected:
        virtual void    HookLosingTextStore () override;
        nonvirtual void HookLosingTextStore_ ();
        virtual void    HookGainedNewTextStore () override;
        nonvirtual void HookGainedNewTextStore_ ();

        /*
     *  Mouse clicks.
     */
    protected:
        virtual bool ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor);

    public:
        nonvirtual unsigned GetCurClickCount () const;
        nonvirtual void     SetCurClickCount (unsigned curClickCount, Foundation::Time::DurationSecondsType lastClickAt);

    protected:
        nonvirtual void IncrementCurClickCount (Foundation::Time::DurationSecondsType lastClickAt);
        nonvirtual void UpdateClickCount (Foundation::Time::DurationSecondsType clickAtTime, const Led_Point& clickAtLocation);
        virtual bool    ClickTimesAreCloseForDoubleClick (Foundation::Time::DurationSecondsType thisClick);
        virtual bool    PointsAreCloseForDoubleClick (const Led_Point& p);

    private:
        unsigned                              fClickCount;
        Foundation::Time::DurationSecondsType fLastClickedAt;
        Led_Point                             fLastMouseDownAt;

    protected:
        virtual void WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor);

    protected:
        virtual void WhileTrackingConstrainSelection (size_t* selStart, size_t* selEnd);
        virtual void WhileTrackingConstrainSelection_ForWholeWords (size_t* selStart, size_t* selEnd);
        virtual void WhileTrackingConstrainSelection_ForWholeRows (size_t* selStart, size_t* selEnd);

    public:
        virtual size_t GetCharAtClickLocation (const Led_Point& where) const;

    public:
        nonvirtual bool IsWholeWindowInvalid () const;

        virtual void Draw (const Led_Rect& subsetToDraw, bool printing) override;

    protected:
        nonvirtual void NoteWholeWindowIsInvalid (); // be very careful calling this - be sure its really true!
        nonvirtual void NoteWindowPartiallyUpdated ();

    private:
        bool fWholeWindowInvalid;

    public:
        /*
        @CLASS:         TextInteractor::CannotUpdateNow
        @DESCRIPTION:   <p>Can be thrown indirectly by @'TextInteractor::Update' or @'TextInteractor::UpdateWindowRect'
                    calls if updates are for some reason (perhaps temporarily) unavailable. Can usually be safely
                    ignored.</p>
        */
        class CannotUpdateNow {
        };
        nonvirtual void Update (bool ignoreCannotUpdateNowErrors = true) const;
        nonvirtual void UpdateWindowRect (const Led_Rect& windowRectArea, bool ignoreCannotUpdateNowErrors = true) const;

    protected:
        // These are to be overriden in the actual class library mixin to hook into its
        // update mechanism.
        virtual void UpdateWindowRect_ (const Led_Rect& windowRectArea) const = 0;

    public:
        nonvirtual bool GetUseSecondaryHilight () const;
        nonvirtual void SetUseSecondaryHilight (bool useSecondaryHilight);

    private:
        bool fUseSecondaryHilight;

    protected:
        virtual void DrawBefore (const Led_Rect& subsetToDraw, bool printing);
        virtual void DrawAfter (const Led_Rect& subsetToDraw, bool printing);

        /*
         *  Overrides to add optional UpdateMode argument.
         */
    public:
        virtual void    SetSelection (size_t start, size_t end) override;
        nonvirtual void SetSelection (size_t start, size_t end, UpdateMode updateMode);
        nonvirtual void SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode = eDefaultUpdate);
        nonvirtual void ScrollByIfRoom (ptrdiff_t downBy, UpdateMode updateMode = eDefaultUpdate);
        nonvirtual void ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0, UpdateMode updateMode = eDefaultUpdate);
        nonvirtual void SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont, UpdateMode updateMode = eDefaultUpdate);
        nonvirtual void SetWindowRect (const Led_Rect& windowRect, UpdateMode updateMode = eDefaultUpdate);
        virtual void    SetHScrollPos (Led_Coordinate hScrollPos) override;
        nonvirtual void SetHScrollPos (Led_Coordinate hScrollPos, UpdateMode updateMode);

        /*
         *  Same as SetTopRowInWindow, but uses a marker position instead of a row#. This can be MUCH
         *  more efficient, since it doesn't force word-wrapping in subclasses which support that.
         */
    public:
        virtual void SetTopRowInWindowByMarkerPosition (size_t markerPos, UpdateMode updateMode = eDefaultUpdate) = 0;

    public:
        nonvirtual bool GetUseBitmapScrollingOptimization () const;
        nonvirtual void SetUseBitmapScrollingOptimization (bool useBitmapScrollingOptimization);

    private:
        bool fUseBitmapScrollingOptimization;

    public:
        nonvirtual bool GetSuppressTypedControlCharacters () const;
        nonvirtual void SetSuppressTypedControlCharacters (bool suppressTypedControlCharacters);

    private:
        bool fSuppressTypedControlCharacters;

    public:
        /*
        @CLASS:         TextInteractor::InteractiveUpdadeMode
        @DESCRIPTION:   <p>Update modes used with @'TextInteractor::InteractiveModeUpdater' and
                    @'TextInteractor::CheckIfCurrentUpdateIsInteractive' to see if a given update is
                    interactive. This can be used to distinguish user text changes (which might be disallowed)
                    from code-based text changes (which a users logic might want to allow).</p>
        */
        enum InteractiveUpdadeMode {
            eInteractiveUpdateMode,
            eNonInteractiveUpdateMode,
            eIndeterminateInteractiveUpdateMode
        };
        nonvirtual InteractiveUpdadeMode GetInteractiveUpdateMode () const;
        nonvirtual void                  SetInteractiveUpdateMode (InteractiveUpdadeMode interactive);
        nonvirtual bool                  CheckIfCurrentUpdateIsInteractive () const;

    private:
        InteractiveUpdadeMode fInteractiveUpdadeMode;

    public:
        class InteractiveModeUpdater;

    protected:
        class PreScrollInfo;
        nonvirtual void PreScrollHelper (UpdateMode updateMode, PreScrollInfo* preScrollInfo);
        nonvirtual void PostScrollHelper (PreScrollInfo preScrollInfo);

    public:
        virtual void Replace (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode = eDefaultUpdate);

    protected:
        class PreReplaceInfo;
        virtual void PreReplace (size_t from, size_t to, size_t withWhatCharCount, UpdateMode updateMode, PreReplaceInfo* preReplaceInfo);
        virtual void PostReplace (PreReplaceInfo& preReplaceInfo);
        virtual void AbortReplace (PreReplaceInfo& preReplaceInfo);
        virtual void ExpandedFromAndToInPostReplace (size_t from, size_t newTo,
                                                     size_t stableTypingRegionStart, size_t stableTypingRegionEnd,
                                                     size_t startPositionOfRowWhereReplaceBegins, size_t startPositionOfRowAfterReplaceEnds,
                                                     size_t* expandedFrom, size_t* expandedTo);

    protected:
        // NB: PreReplaceInfo declared here instead of outside TextInteractor class cuz an instance is
        // a data member of TextInteractor.
        class PreReplaceInfo {
        public:
            PreReplaceInfo ();
            ~PreReplaceInfo (); // must get destroyed before owned fTextInteractor, and must either call
            // PostReplace or AbortReplace or ~PreReplaceInfo before ever losing
            // fTextInteractor's TextStore...

        public:
            nonvirtual UpdateMode GetUpdateMode () const;
            nonvirtual size_t GetFrom () const;
            nonvirtual size_t GetTo () const;

        private:
            TextInteractor* fTextInteractor;
            UpdateMode      fUpdateMode;
            size_t          fFrom;
            size_t          fTo;
            size_t          fWithWhatCharCount;
            Marker          fBoundingUpdateMarker;
            Led_Distance    fBoundingUpdateHeight;
            Led_Distance    fStableTypingRegionHeight;

        private:
            friend void TextInteractor::PreReplace (size_t from, size_t to, size_t withWhatCharCount, UpdateMode updateMode, PreReplaceInfo* preReplaceInfo);
            friend void TextInteractor::PostReplace (PreReplaceInfo& preReplaceInfo);
            friend void TextInteractor::AbortReplace (PreReplaceInfo& preReplaceInfo);
        };

        /*
            *  Simply calls 'Replace ()'. Only purpose is that it is sometimes convenient to OVERRIDE in just
            *  one place to filter user typing/text updates. And not have to worry about if the particular
            *  bottleneck routine also gets used for 'programming-sourced' text updates. So all typing, and
            *  user-insertions from things like paste (things you might want to validate) go through this
            *  procedure. And you neededn't OVERRIDE 'Replace' itself, unless your refinements are intended to
            *  apply even to programming-based text updates. NB: text updates stemming from apple-events/OLE
            *  automation are considered 'user-updates' - and so vector through here. This is because they
            *  would likely want to be validated.
        ** DOCS INVALID - MEANING OF InteractiveReplace() REVISED
            */
    public:
        virtual void InteractiveReplace (const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode = eDefaultUpdate);

    protected:
        nonvirtual void InteractiveReplace_ (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, bool updateCursorPosition = true, bool validateTextForCharsetConformance = true, UpdateMode updateMode = eDefaultUpdate);

    protected:
        virtual bool InteractiveReplaceEarlyPostReplaceHook (size_t withWhatCharCount);

        // utilities to help with undo
    public:
        virtual void PreInteractiveUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd);
        virtual void PostInteractiveUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t startOfInsert, size_t endOfInsert, const Led_SDK_String& cmdName);
        virtual void PostInteractiveSimpleCharInsertUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t startOfInsert, size_t endOfInsert, const Led_SDK_String& cmdName);
        virtual void PostInteractiveUndoPostHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, InteractiveReplaceCommand::SavedTextRep** afterRep, size_t startOfInsert, const Led_SDK_String& cmdName);

    protected:
        virtual InteractiveReplaceCommand::SavedTextRep* InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd);

    protected:
        nonvirtual void OnUndoCommand ();
        nonvirtual void OnRedoCommand ();

        // Utility routine for lots of editing cases - like backspace, cursor arrows, etc...
    public:
        enum CursorMovementAction { eCursorDestroying,
                                    eCursorMoving,
                                    eCursorExtendingSelection };

    protected:
        virtual void DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
                                             UpdateMode updateMode = eDefaultUpdate, bool scrollToSelection = true);

    public:
        virtual void AboutToUpdateText (const UpdateInfo& updateInfo) override;
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    private:
        PreReplaceInfo  fTmpPreReplaceInfo;
        TextInteractor* fDoingUpdateModeReplaceOn;

        // Clipboard commands
    public:
        virtual void OnCutCommand ();
        virtual void OnCopyCommand ();
        virtual void OnPasteCommand ();
        virtual void OnClearCommand ();

        // helper function for implementing OnCopyCommand
    protected:
#if qAccessCheckAcrossInstancesSometimesWrong
    public:
#endif
        virtual bool OnCopyCommand_Before ();
        virtual void OnCopyCommand_After ();
        virtual void OnCopyCommand_CopyFlavors ();

        // helper function, to share code between this and subclasses which need different paste behavior
    protected:
#if qAccessCheckAcrossInstancesSometimesWrong
    public:
#endif
        virtual bool ShouldEnablePasteCommand () const;
        virtual bool OnPasteCommand_Before ();
        virtual void OnPasteCommand_After ();
        virtual void OnPasteCommand_PasteBestFlavor ();
        virtual void OnPasteCommand_PasteFlavor_Specific (Led_ClipFormat format);

    protected:
        nonvirtual bool PasteLooksLikeSmartCNP (SmartCNPInfo* scnpInfo) const;

    public:
        virtual void OnSelectAllCommand ();

        // Drag & Drop support
    protected:
        virtual bool CanAcceptFlavor (Led_ClipFormat clipFormat) const;

        // Internalizing
    public:
        virtual void InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
                                            bool updateCursorPosition = true, bool autoScroll = true, UpdateMode updateMode = eDefaultUpdate);
        virtual void InternalizeFlavor_Specific (ReaderFlavorPackage& flavorPackage, Led_ClipFormat format,
                                                 bool updateCursorPosition = true, bool autoScroll = true, UpdateMode updateMode = eDefaultUpdate);

    public:
        nonvirtual shared_ptr<FlavorPackageInternalizer> GetInternalizer () const;
        nonvirtual void                                  SetInternalizer (const shared_ptr<FlavorPackageInternalizer>& i);

    private:
        shared_ptr<FlavorPackageInternalizer> fInternalizer;

    protected:
        virtual shared_ptr<FlavorPackageInternalizer> MakeDefaultInternalizer ();
        virtual void                                  HookInternalizerChanged ();

        // Externalizing
    public:
        virtual void ExternalizeFlavors (WriterFlavorPackage& flavorPackage);
        virtual void ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage);

    public:
        nonvirtual shared_ptr<FlavorPackageExternalizer> GetExternalizer () const;
        nonvirtual void                                  SetExternalizer (const shared_ptr<FlavorPackageExternalizer>& e);

    private:
        shared_ptr<FlavorPackageExternalizer> fExternalizer;

    protected:
        virtual shared_ptr<FlavorPackageExternalizer> MakeDefaultExternalizer ();
        virtual void                                  HookExternalizerChanged ();

        // Notifications
    public:
        class BadUserInput;
        virtual void OnBadUserInput (); // called - for example - when user types bad SJIS - default is to sysbeep()

        // Scrollbar support
    public:
        enum VHSelect { v,
                        h,
                        eFirst = v,
                        eLast  = h };
        enum ScrollBarType { eScrollBarNever,
                             eScrollBarAsNeeded,
                             eScrollBarAlways };
        nonvirtual ScrollBarType GetScrollBarType (VHSelect vh) const;
        virtual void             SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType);

    protected:
        nonvirtual void SetScrollBarType_ (VHSelect vh, ScrollBarType scrollBarType);

    private:
        ScrollBarType fScrollBarType[2];

        /*
            * Fancy support for delayed scrollbar updatating...
            */
    public:
        virtual void InvalidateScrollBarParameters () override;

    protected:
        nonvirtual void InvalidateScrollBarParameters_ ();
        virtual void    UpdateScrollBars ();
        nonvirtual void UpdateScrollBars_ (); // must be called cuz marks sbar parameters valid
    private:
        bool fScrollBarParamsValid;

        /*
            *  Helper for implementing scrolling.
            */
    public:
        virtual bool DelaySomeForScrollBarClick ();

    private:
        Foundation::Time::DurationSecondsType fLastScrolledAt;

    public:
        nonvirtual bool GetCaretShown () const;
        nonvirtual void SetCaretShown (bool shown);

    private:
        bool fCaretShown; // caret is active (though possibly not REALLY shown since drawn in
        // off state...

    public:
        virtual bool GetCaretShownSituation () const;

    public:
        nonvirtual bool GetCaretShownAfterPos () const;
        nonvirtual void SetCaretShownAfterPos (bool shownAfterPos);

    protected:
        virtual Led_Rect CalculateCaretRect () const;

        // Can be overriden to update other things like IME position
    protected:
        virtual void InvalidateCaretState ();

    protected:
        bool fLeftSideOfSelectionInteresting; // Occasionally (like in ScrollToSelection ()) it is helpful to know
        // which end of the selection the user is most interseted in seeing.
        // Also - we might - for some UI's take this into account in other
        // places like if we extend one side of a selection, or reduce the
        // other. Also used in cursoring for extended selection. Must be updated
        // in subclasses for things like mouse dragging...
    private:
        bool fCaretShownAfterPos; // If true show after, else before

    public:
        virtual void OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

#if qMultiByteCharacters
    protected:
        char fMultiByteInputCharBuf[2]; // the 'multi' in 'multibyte' better be 2!!

        virtual bool HandledMByteCharTyping (char theChar);
#endif

        /*
         *  Query of the windowing system if there are any pending keystrokes.
         */
    protected:
        virtual bool QueryInputKeyStrokesPending () const = 0;

    protected:
        nonvirtual void UpdateIfNoKeysPending (); // utility

#if qPlatform_MacOS || qXWindows
    public:
        static float GetTickCountBetweenBlinks ();
#endif

    private:
        friend class SuppressCommandBreaksContext;
    };

    /*
    @CLASS:         TextInteractor::SuppressCommandBreaksContext
    @ACCESS:        public
    @DESCRIPTION:   <p>When one of these is created on the stack for the @'TextInteractor' ti, then
                commands on that TextInteractor to @'TextInteractor::BreakInGroupedCommands ()' are
                ignored.</p>
    */
    class TextInteractor::SuppressCommandBreaksContext {
    public:
        SuppressCommandBreaksContext (TextInteractor& ti);
        ~SuppressCommandBreaksContext ();

    private:
        TextInteractor& fTextInteractor;
        bool            fOldVal;
    };

    /*
    @CLASS:         TextInteractor::CommandNames
    @DESCRIPTION:   <p>Command names for each of the user-visible commands produced by the @'TextInteractor' module.
                This name is used used in the constructed Undo command name, as
                in, "Undo Paste". You can replace this name with whatever you like.
                You change this value with @'TextInteractor::SetCommandNames'.</p>
                    <p> The point of this is to allow for different UI-language localizations,
                without having to change Led itself.</p>
                    <p>See also @'TextInteractor::GetCommandNames'.</p>
                    <p>See also @'WordProcessor::CommandNames'.</p>
    */
    struct TextInteractor::CommandNames {
        Led_SDK_String fTypingCommandName;
        Led_SDK_String fCutCommandName;
        Led_SDK_String fClearCommandName;
        Led_SDK_String fPasteCommandName;
        Led_SDK_String fUndoFormatString;
        Led_SDK_String fRedoFormatString;
        Led_SDK_String fReplaceCommandName;
        Led_SDK_String fReplaceAllCommandName;
        Led_SDK_String fReplaceAllInSelectionCommandName;
    };

    /*
    @CLASS:         TextInteractor::CommandUpdater
    @DESCRIPTION:   <p></p>
    */
    class TextInteractor::CommandUpdater {
    public:
        using CommandNumber = TextInteractor::CommandNumber;

    public:
        virtual CommandNumber GetCmdID () const                  = 0;
        virtual bool          GetEnabled () const                = 0;
        virtual void          SetEnabled (bool enabled)          = 0;
        virtual void          SetChecked (bool checked)          = 0;
        virtual void          SetText (const Led_SDK_Char* text) = 0;
    };

    /*
    @CLASS:         TextInteractor::DialogSupport
    @DESCRIPTION:   <p></p>
    */
    class TextInteractor::DialogSupport {
    public:
        using CommandNumber = TextInteractor::CommandNumber;

    public:
        virtual void DisplayFindDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative, bool* pressedOK);

    public:
        enum ReplaceButtonPressed { eReplaceButton_Cancel,
                                    eReplaceButton_Find,
                                    eReplaceButton_Replace,
                                    eReplaceButton_ReplaceAll,
                                    eReplaceButton_ReplaceAllInSelection };
        virtual ReplaceButtonPressed DisplayReplaceDialog (Led_tString* findText, const vector<Led_tString>& recentFindSuggestions, Led_tString* replaceText, bool* wrapSearch, bool* wholeWordSearch, bool* caseSensative);

    public:
        class SpellCheckDialogCallback;
        virtual void DisplaySpellCheckDialog (SpellCheckDialogCallback& callback);
    };

    /*
    @CLASS:         TextInteractor::DialogSupport::SpellCheckDialogCallback
    @DESCRIPTION:   <p>This interface is called by the actual spellcheck dialog implematation back to the implementer
                of the real spellchecking functionality.</p>
    */
    class TextInteractor::DialogSupport::SpellCheckDialogCallback {
    public:
        struct MisspellingInfo {
            Led_tString         fUndefinedWord;
            vector<Led_tString> fSuggestions;
        };

    public:
        virtual MisspellingInfo* GetNextMisspelling () = 0;

    public:
        virtual void DoIgnore ()                                  = 0;
        virtual void DoIgnoreAll ()                               = 0;
        virtual void DoChange (const Led_tString& changeTo)       = 0;
        virtual void DoChangeAll (const Led_tString& changeTo)    = 0;
        virtual bool AddToDictionaryEnabled () const              = 0;
        virtual void AddToDictionary (const Led_tString& newWord) = 0;
        virtual void LookupOnWeb (const Led_tString& word)        = 0;
        virtual bool OptionsDialogEnabled () const                = 0;
        virtual void OptionsDialog ()                             = 0;
    };

    /*
    @CLASS:         TextInteractor::SearchParameters
    @DESCRIPTION:   <p>
                    </p>
    */
    class TextInteractor::SearchParameters : public TextStore::SearchParameters {
    private:
        using inherited = TextStore::SearchParameters;

    public:
        SearchParameters ();

    public:
        vector<Led_tString> fRecentFindStrings;
    };

    /*
    @CLASS:         TextInteractor::ReplaceParameters
    @DESCRIPTION:   <p>
                    </p>
    */
    class TextInteractor::ReplaceParameters {
    public:
        ReplaceParameters ();

    public:
        Led_tString fReplaceWith;
    };

    /*
    @CLASS:         TextInteractor::UndoableContextHelper
    @DESCRIPTION:   <p>Use this helper class in a context where you are creating/doing an undoable
                command. This class is a simpler wrapper/helper for @'TextInteractor::PreInteractiveUndoHelper'
                and @'TextInteractor::PostInteractiveUndoHelper'.</p>
                    <p>A simple example usage would be:<br>
                <code><pre>
                    UndoableContextHelper   undoContext (*this, GetCommandNames ().fClearCommandName, true);
                        {
                            //Actual command guts
                            InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0);
                        }
                    undoContext.CommandComplete ();
                </pre></code>
                    </p>
                    <p>Note - the 'allowSmartCNPExpansion' should generally be true if the undoable action could
                have invoked smartcutandpaste space additions/removealls. Specifing true expands the region we capture to
                take this possability into account.
                    </p>
    */
    class TextInteractor::UndoableContextHelper {
    public:
        using SavedTextRep = InteractiveReplaceCommand::SavedTextRep;

    public:
        UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, bool allowSmartCNPExpansion);
        UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, size_t regionAndSelStart, size_t regionAndSelEnd, bool allowSmartCNPExpansion);
        UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd, bool allowSmartCNPExpansion);
        ~UndoableContextHelper ();

    public:
        nonvirtual void CommandComplete ();
        nonvirtual void CommandComplete (size_t endOfInsert);

    public:
        nonvirtual size_t GetUndoRegionStart () const;
        nonvirtual size_t GetUndoRegionEnd () const;

    public:
        nonvirtual bool GetSimplePlainTextInsertOptimization () const;
        nonvirtual void SetSimplePlainTextInsertOptimization (bool simplePlainTextInsertOptimizationFlag);

    private:
        bool fSimplePlainTextInsertOptimization;

    private:
        TextInteractor& fTextInteractor;
        Led_SDK_String  fCmdName;
        size_t          fSelStart;
        size_t          fSelEnd;
        SavedTextRep*   fBefore;
        bool            fCommandComplete;
    };

    /*
    @CLASS:         TextInteractor::TemporarilySetUpdateMode
    @DESCRIPTION:   <p>See @'TextInteractor::GetDefaultUpdateMode'.</p>
    */
    class TextInteractor::TemporarilySetUpdateMode {
    public:
        TemporarilySetUpdateMode (TextInteractor& ti, TextInteractor::UpdateMode tmpUpdateMode);
        ~TemporarilySetUpdateMode ();

    private:
        TextInteractor& fTextInteractor;
        UpdateMode      fOldValue;

    private:
        TemporarilySetUpdateMode (const TemporarilySetUpdateMode&) = delete;
        TemporarilySetUpdateMode& operator= (const TemporarilySetUpdateMode&) = delete;
        static void*              operator new (size_t); // DONT
    };

    /*
    @CLASS:         TextInteractor::InteractiveModeUpdater
    @DESCRIPTION:   <p>Calls @'TextInteractor::SetInteractiveUpdateMode' to save/restore the interactive update
                mode for</p>
    */
    class TextInteractor::InteractiveModeUpdater {
    public:
        InteractiveModeUpdater (TextInteractor& ti, bool interactive = true);
        ~InteractiveModeUpdater ();

    private:
        TextInteractor&       fTextInteractor;
        InteractiveUpdadeMode fSavedMode;
    };

    /*
    @CLASS:         TextInteractor::PreScrollInfo
    @DESCRIPTION:   <p></p>
    */
    class TextInteractor::PreScrollInfo {
    public:
        PreScrollInfo ();

    private:
        UpdateMode     fUpdateMode;
        size_t         fOldWindowStart;
        Led_Coordinate fOldHScrollPos;
        bool           fTryTodoScrollbits;
        size_t         fOldLastRowStart;

    private:
        friend void TextInteractor::PreScrollHelper (UpdateMode updateMode, PreScrollInfo* preScrollInfo);
        friend void TextInteractor::PostScrollHelper (PreScrollInfo preScrollInfo);
    };

    /*
    @CLASS:         TextInteractor::BadUserInput
    @DESCRIPTION:   <p>See @'TextInteractor::OnBadUserInput'.</p>
    */
    class TextInteractor::BadUserInput {
    };

    /*
    @CLASS:         CommandNumberMapping<TARGET_COMMAND_NUMBER>
    @DESCRIPTION:   <p></p>
    */
    template <typename TARGET_COMMAND_NUMBER>
    class CommandNumberMapping {
    public:
        using CommandNumber = TextInteractor::CommandNumber;

    public:
        CommandNumberMapping ();
        ~CommandNumberMapping ();

    public:
        static CommandNumberMapping<TARGET_COMMAND_NUMBER>& Get ();

    public:
        nonvirtual void               AddAssociation (TARGET_COMMAND_NUMBER externalCommandNumber, CommandNumber internalCommandNumber);
        nonvirtual void               AddRangeAssociation (TARGET_COMMAND_NUMBER externalRangeStart, TARGET_COMMAND_NUMBER externalRangeEnd, CommandNumber internalRangeStart, CommandNumber internalRangeEnd);
        virtual CommandNumber         Lookup (TARGET_COMMAND_NUMBER tcn) const;
        virtual TARGET_COMMAND_NUMBER ReverseLookup (CommandNumber cmdNum) const;

    private:
        using MAP_TYPE = map<TARGET_COMMAND_NUMBER, CommandNumber>;
        MAP_TYPE fMap;
        struct RangeElt {
            pair<TARGET_COMMAND_NUMBER, TARGET_COMMAND_NUMBER> fExternalCmds;
            pair<CommandNumber, CommandNumber>                 fInternalCmds;
        };
        using RANGE_VEC_TYPE = vector<RangeElt>;
        RANGE_VEC_TYPE fRanges;

    private:
        static CommandNumberMapping<TARGET_COMMAND_NUMBER>* sThe;
    };

    /*
    @CLASS:         SimpleCommandUpdater
    @BASES:         TextInteractor::CommandUpdater
    @DESCRIPTION:   <p></p>
    */
    class SimpleCommandUpdater : public TextInteractor::CommandUpdater {
    public:
        SimpleCommandUpdater (CommandNumber cmdNum);

    public:
        virtual CommandNumber GetCmdID () const override;
        virtual bool          GetEnabled () const override;
        virtual void          SetEnabled (bool enabled) override;
        virtual void          SetChecked (bool checked) override;
        virtual void          SetText (const Led_SDK_Char* text) override;

    public:
        CommandNumber  fCommandNumber;
        bool           fEnabled;
        bool           fChecked;
        Led_SDK_String fText;
    };

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif
    /*
    @CLASS:         TrivialImager_Interactor<TEXTSTORE,IMAGER>
    @DESCRIPTION:   <p>Handy little template, if you want to use the power of Led, but just to wrap a particular imager,
        in a localized, one-time fasion, say todo printing, or some such. Not for interactors.</p>
            <p>The usuage can be as simple as:
            <code>
                void    SomeAppDrawCall (Led_Tablet t, const Led_Rect& r)
                {
                    TrivialImager_Interactor<ChunkedArrayTextStore, WordProcessor> (t, r, LED_TCHAR_OF ("Hi mom")).Draw ();
                }
            </code>
            </p>
            <p>See also @'TrivialImager<TEXTSTORE,IMAGER>', and @'TrivialWordWrappedImager<TEXTSTORE,IMAGER>'.</p>
        */
    template <typename TEXTSTORE, typename IMAGER>
    class TrivialImager_Interactor : public TrivialImager<TEXTSTORE, IMAGER> {
    private:
        using inherited = TrivialImager<TEXTSTORE, IMAGER>;

    public:
        TrivialImager_Interactor (Led_Tablet t)
            : inherited (t)
        {
        }
        TrivialImager_Interactor (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText = LED_TCHAR_OF (""))
            : inherited (t)
        {
            // Kooky I cannot just call base class CTOR that does all this - but then it invokes calls to RefreshWindowRect_ etc, before
            // THIS CTOR is done, so they call pure-virtual version!
            SnagAttributesFromTablet ();
            SetWindowRect (bounds);
            GetTextStore ().Replace (0, 0, initialText.c_str (), initialText.length ());
        }

        // In case the imager is a TextInteractor - provide dummy implementations...
    public:
        using UpdateMode = typename IMAGER::UpdateMode;
        virtual bool QueryInputKeyStrokesPending () const override { return false; };
        virtual void RefreshWindowRect_ ([[maybe_unused]] const Led_Rect& windowRectArea, [[maybe_unused]] UpdateMode updateMode) const override {}
        virtual void UpdateWindowRect_ ([[maybe_unused]] const Led_Rect& windowRectArea) const override {}
    };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

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
        Require (int(externalRangeEnd - externalRangeStart) == int(internalRangeEnd - internalRangeStart)); // ranges same length.

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

#endif /*_Stroika_Frameworks_Led_TextInteractor_h_*/
