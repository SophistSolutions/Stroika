/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <cctype>
#include <set>

#include "../../Foundation/Characters/CString/Utilities.h"
#include "../../Foundation/Characters/Character.h"
#include "../../Foundation/Characters/CodePage.h"
#include "../../Foundation/Characters/Format.h"

#include "Config.h"

#include "Command.h"
#include "IdleManager.h"
#include "Marker.h"
#include "TextInteractor.h"
#include "TextStore.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

using SavedTextRep = InteractiveReplaceCommand::SavedTextRep;

namespace {
    class FlavorSavorTextRep : public SavedTextRep {
    private:
        using inherited = SavedTextRep;

    public:
        FlavorSavorTextRep (TextInteractor* interactor, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
            : inherited (selStart, selEnd)
            , fSavedText ()
            , fTextLength (regionEnd - regionStart)
        {
#if !qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug
            RequireNotNull (interactor);
#endif
            interactor->GetExternalizer ()->ExternalizeBestFlavor (fSavedText, regionStart, regionEnd);
        }
        virtual size_t GetLength () const override
        {
            return fTextLength;
        }
        virtual void InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite) override
        {
#if !qFailToLookupFunctionNameWhenCompilingFunctionLocalClassMethodCompilerBug
            RequireNotNull (interactor);
#endif
            interactor->GetInternalizer ()->InternalizeBestFlavor (fSavedText, at, at + nBytesToOverwrite);
        }

    private:
        size_t                    fTextLength;
        ReadWriteMemBufferPackage fSavedText;
    };
}

namespace {
    // Only these chars count as whitespce for smart cut/n/paste
    inline bool IsSmartSpace (Led_tChar c)
    {
        return (c == ' ' or c == '\t');
    }
    inline bool IsShouldBeSepWithWhitespaceWordChar (Led_tChar c)
    {
        // iswalnum vectors to GetStringTypeW (CT_CTYPE1) and then checks for C1_ALPHA, which
        // matches all alphabetic, numeric, AND Idiogram characters. I THINK the right thing to use
        // is to directly call GetStringTypeW (CT_CTYPE2, and check for C2_SEGMENTSEPARATOR, or something
        // like that (will have to read up on UNICODE more, and/or experiemnet). Anyhow - none of that will happen
        // for this release, and this should be good enuf to prevent smart-copy-paste from happing with
        // idiogram characters
        return Character (c).IsAlphaNumeric () and c < 127;
    }
}

namespace {
    class MyCallback : public TextInteractor::DialogSupport::SpellCheckDialogCallback {
    private:
        using inherited = TextInteractor::DialogSupport::SpellCheckDialogCallback;

    public:
        MyCallback (TextInteractor& ti)
            : inherited ()
            , fTI (ti)
            , fIgnoredWords ()
        {
        }

    public:
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(suppress : 6262)
#endif
        virtual MisspellingInfo* GetNextMisspelling () override
        {
            SpellCheckEngine* sce = fTI.GetSpellCheckEngine ();
            if (sce != nullptr) {
                Led_tChar charBuf[10 * 1024]; // buffer size doesn't matter much - but just has to be larger than the largest undef word we ever want to find...
                bool      firstTry    = true;
                size_t    startRegion = fTI.GetSelectionEnd ();
            SecondTry : {
                // regardless of the startRegion - back up the search to the start of the interesected word. The only
                // exception is if the size of our charBuf isn't big enough to go past the startRegion position (cuz
                // then we'd be going backwards, and risk never moving forwards, in case of a large word)
                size_t wordStart = 0;
                size_t wordEnd   = 0;
                bool   wordReal  = false;
                fTI.GetTextStore ().FindWordBreaks (startRegion, &wordStart, &wordEnd, &wordReal, sce->PeekAtTextBreaksUsed ());
                if (wordReal and wordStart + NEltsOf (charBuf) > startRegion) {
                    startRegion = wordStart;
                }
            }
                size_t endRegion = min (startRegion + NEltsOf (charBuf), fTI.GetEnd ());
                fTI.CopyOut (startRegion, endRegion - startRegion, charBuf);
                const Led_tChar* cursor    = nullptr;
                const Led_tChar* wordStart = nullptr;
                const Led_tChar* wordEnd   = nullptr;
                if (sce->ScanForUndefinedWord (charBuf, charBuf + (endRegion - startRegion), &cursor, &wordStart, &wordEnd)) {
                    /*
                     * If the undefined word ends at the end of the buffer region we looked at - it might be an artifact of our
                     * chunking. Retry the word (being careful about the special case where the 'word' is as big as our buffer).
                     */
                    if (wordStart != charBuf and wordEnd == charBuf + (endRegion - startRegion)) {
                        startRegion += (wordStart - charBuf); // advance to the start of this word and try again
                        goto SecondTry;
                    }

                    Led_tString undefinedWord = Led_tString (wordStart, wordEnd);
                    if (fIgnoredWords.find (undefinedWord) != fIgnoredWords.end ()) {
                        // push startRegion a bit forward over this word, and try again, but don't
                        // set 'second try' flag cuz we haven't wrapped at the end of the document...
                        startRegion += (wordEnd - charBuf);
                        goto SecondTry;
                    }
                    MisspellingInfo* mi = new MisspellingInfo ();
                    mi->fUndefinedWord  = undefinedWord;
                    mi->fSuggestions    = sce->GenerateSuggestions (mi->fUndefinedWord);
                    size_t selStart     = startRegion + wordStart - charBuf;
                    size_t selEnd       = selStart + (wordEnd - wordStart);
                    fTI.SetSelection (selStart, selEnd);
                    fTI.ScrollToSelection ();
                    return mi;
                }
                else if (endRegion < fTI.GetEnd ()) {
                    // no undefined words in that region, so try the next chunk. Stopping at word boundaries taken
                    // care of with logic already above (for first case)
                    startRegion = endRegion;
                    goto SecondTry;
                }
                else if (firstTry) {
                    // Wrap around...
                    startRegion = 0;
                    firstTry    = false;
                    goto SecondTry;
                }
            }
            return nullptr;
        }
        virtual void DoIgnore () override
        {
            fTI.SetSelection (fTI.GetSelectionEnd (), fTI.GetSelectionEnd ());
            fTI.ScrollToSelection ();
        }
        virtual void DoIgnoreAll () override
        {
            {
                size_t                              origSelStart = fTI.GetSelectionStart ();
                size_t                              origSelEnd   = fTI.GetSelectionEnd ();
                Memory::SmallStackBuffer<Led_tChar> text (origSelEnd - origSelStart + 1);
                fTI.CopyOut (origSelStart, origSelEnd - origSelStart, text);
                Led_tString ignoredWord = Led_tString (text, origSelEnd - origSelStart);
                fIgnoredWords.insert (ignoredWord);
            }
            DoIgnore ();
        }
        virtual void DoChange (const Led_tString& changeTo) override
        {
            size_t                           origSelStart = fTI.GetSelectionStart ();
            size_t                           origSelEnd   = fTI.GetSelectionEnd ();
            TextInteractor::SearchParameters sp;
            {
                Memory::SmallStackBuffer<Led_tChar> text (origSelEnd - origSelStart + 1);
                fTI.CopyOut (origSelStart, origSelEnd - origSelStart, text);
                sp.fMatchString = Led_tString (text, origSelEnd - origSelStart);
            }
            fTI.SetSelection (origSelStart, origSelStart); // cuz OnDoReplaceCommand () looks from selectionEND
            fTI.OnDoReplaceCommand (sp, changeTo);
        }
        virtual void DoChangeAll (const Led_tString& changeTo) override
        {
            size_t                           origSelStart = fTI.GetSelectionStart ();
            size_t                           origSelEnd   = fTI.GetSelectionEnd ();
            TextInteractor::SearchParameters sp;
            {
                Memory::SmallStackBuffer<Led_tChar> text (origSelEnd - origSelStart + 1);
                fTI.CopyOut (origSelStart, origSelEnd - origSelStart, text);
                sp.fMatchString = Led_tString (text, origSelEnd - origSelStart);
            }
            fTI.OnDoReplaceAllCommand (sp, changeTo);
        }
        virtual bool AddToDictionaryEnabled () const override
        {
            SpellCheckEngine* sce = fTI.GetSpellCheckEngine ();
            if (sce != nullptr) {
                SpellCheckEngine::UDInterface* udi = sce->GetUDInterface ();
                if (udi != nullptr) {
                    return udi->AddWordToUserDictionarySupported ();
                }
            }
            return false;
        }
        virtual void AddToDictionary (const Led_tString& newWord) override
        {
            SpellCheckEngine* sce = fTI.GetSpellCheckEngine ();
            if (sce != nullptr) {
                SpellCheckEngine::UDInterface* udi = sce->GetUDInterface ();
                if (udi != nullptr) {
                    udi->AddWordToUserDictionary (newWord);
                }
            }

            fTI.SetSelection (fTI.GetSelectionEnd (), fTI.GetSelectionEnd ());
            fTI.ScrollToSelection ();
        }
        virtual void LookupOnWeb (const Led_tString& word) override
        {
            const char kURLBase[] = "http://dictionary.reference.com/search?q=";
            Led_URLManager::Get ().Open (kURLBase + Led_tString2ANSIString (word));
        }
        virtual bool OptionsDialogEnabled () const override
        {
            // cuz no implementation of OptionsDialog () callback...
            return false;
        }
        virtual void OptionsDialog () override
        {
            // NYI. When enabled - change result of OptionsDialogEnabled ()
        }

    private:
        TextInteractor&  fTI;
        set<Led_tString> fIgnoredWords; // note we intentionally dont keep this around so that the
        // ignored words list stays around (lives) as long as the dialog...
    };
}

/*
 ********************************************************************************
 ************************* TextInteractor::DialogSupport ************************
 ********************************************************************************
 */
void TextInteractor::DialogSupport::DisplayFindDialog (Led_tString* /*findText*/, const vector<Led_tString>& /*recentFindSuggestions*/, bool* /*wrapSearch*/, bool* /*wholeWordSearch*/, bool* /*caseSensative*/, bool* /*pressedOK*/)
{
    /*
     *  You may want to use code something like this in your OVERRIDE:
     *
        Led_StdDialogHelper_FindDialog  findDialog (::AfxGetResourceHandle (), nullptr);

        findDialog.fFindText = *findText;
        findDialog.fRecentFindTextStrings = recentFindSuggestions;
        findDialog.fWrapSearch = *wrapSearch;
        findDialog.fWholeWordSearch = *wholeWordSearch;
        findDialog.fCaseSensativeSearch = *caseSensative;

        findDialog.DoModal ();

        *findText = findDialog.fFindText;
        *wrapSearch = findDialog.fWrapSearch;
        *wholeWordSearch = findDialog.fWholeWordSearch;
        *caseSensative = findDialog.fCaseSensativeSearch;
        *pressOK = findDialog.fPressedOK;
    */
    Assert (false); // to use this - you must OVERRIDE this routine, and provide your own implementation, perhaps akin to the above.
}

TextInteractor::DialogSupport::ReplaceButtonPressed TextInteractor::DialogSupport::DisplayReplaceDialog (Led_tString* /*findText*/, const vector<Led_tString>& /*recentFindSuggestions*/, Led_tString* /*replaceText*/, bool* /*wrapSearch*/, bool* /*wholeWordSearch*/, bool* /*caseSensative*/)
{
    Assert (false); // to use this - you must OVERRIDE this routine, and provide your own implementation, perhaps akin to the above.
    return eReplaceButton_Cancel;
}

void TextInteractor::DialogSupport::DisplaySpellCheckDialog (SpellCheckDialogCallback& /*callback*/)
{
    Assert (false); // to use this - you must OVERRIDE this routine, and provide your own implementation, perhaps akin to the above.
}

/*
 ********************************************************************************
 ******************** TextInteractor::UndoableContextHelper *********************
 ********************************************************************************
 */
TextInteractor::UndoableContextHelper::UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, bool allowSmartCNPExpansion)
    : fSimplePlainTextInsertOptimization (false)
    , fTextInteractor (ti)
    , fCmdName (cmdName)
    , fSelStart (ti.GetSelectionStart ())
    , fSelEnd (ti.GetSelectionEnd ())
    , fBefore (nullptr)
    , fCommandComplete (false)
{
    if (allowSmartCNPExpansion) {
        ti.OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (&fSelStart, &fSelEnd);
    }
    if (ti.GetCommandHandler () != nullptr) {
        ti.PreInteractiveUndoHelper (&fBefore, fSelStart, fSelEnd, ti.GetSelectionStart (), ti.GetSelectionEnd ());
    }
}

TextInteractor::UndoableContextHelper::UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, size_t regionAndSelStart, size_t regionAndSelEnd, bool allowSmartCNPExpansion)
    : fSimplePlainTextInsertOptimization (false)
    , fTextInteractor (ti)
    , fCmdName (cmdName)
    , fSelStart (regionAndSelStart)
    , fSelEnd (regionAndSelEnd)
    , fBefore (nullptr)
    , fCommandComplete (false)
{
    if (allowSmartCNPExpansion) {
        ti.OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (&fSelStart, &fSelEnd);
    }
    if (ti.GetCommandHandler () != nullptr) {
        ti.PreInteractiveUndoHelper (&fBefore, fSelStart, fSelEnd, regionAndSelStart, regionAndSelEnd);
    }
}

TextInteractor::UndoableContextHelper::UndoableContextHelper (TextInteractor& ti, const Led_SDK_String& cmdName, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd, bool allowSmartCNPExpansion)
    : fSimplePlainTextInsertOptimization (false)
    , fTextInteractor (ti)
    , fCmdName (cmdName)
    , fSelStart (regionStart)
    , fSelEnd (regionEnd)
    , fBefore (nullptr)
    , fCommandComplete (false)
{
    if (allowSmartCNPExpansion) {
        ti.OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (&fSelStart, &fSelEnd);
    }
    if (ti.GetCommandHandler () != nullptr) {
        ti.PreInteractiveUndoHelper (&fBefore, fSelStart, fSelEnd, selStart, selEnd);
    }
}

TextInteractor::UndoableContextHelper::~UndoableContextHelper ()
{
    if (not fCommandComplete) {
        delete fBefore;
    }
}

/*
@METHOD:        TextInteractor::UndoableContextHelper::CommandComplete
@ACCESS:        public
@DESCRIPTION:   <p>There are two overloaded versions of this function. Both require knowledge of
            the END of the updated (inserted) region. The one with no arguments assumes the selectionEnd
            is the end of the inserted region. The second overload allows you to explicitly specify the
            end of the inserted region.
            </p>
                <p>Call this function when a command has been completed (See the @'TextInteractor::UndoableContextHelper' docs
            for how to structure the CTOR/CommandComplete calls). Failure to call this after constructing a UndoableContextHelper
            object results in the command not being recorded (as is appropriate if there is an exception thrown and the command
            is not performed).
            </p>
*/
void TextInteractor::UndoableContextHelper::CommandComplete ()
{
    fTextInteractor.ScrollToSelection ();
    if (fTextInteractor.GetCommandHandler () != nullptr) {
        if (GetSimplePlainTextInsertOptimization ()) {
            fTextInteractor.PostInteractiveSimpleCharInsertUndoHelper (&fBefore, fSelStart, fTextInteractor.GetSelectionEnd (), fCmdName);
        }
        else {
            fTextInteractor.PostInteractiveUndoHelper (&fBefore, fSelStart, fTextInteractor.GetSelectionEnd (), fCmdName);
        }
    }
    fCommandComplete = true;
}

void TextInteractor::UndoableContextHelper::CommandComplete (size_t endOfInsert)
{
    fTextInteractor.ScrollToSelection ();
    if (fTextInteractor.GetCommandHandler () != nullptr) {
        if (GetSimplePlainTextInsertOptimization ()) {
            fTextInteractor.PostInteractiveSimpleCharInsertUndoHelper (&fBefore, fSelStart, endOfInsert, fCmdName);
        }
        else {
            fTextInteractor.PostInteractiveUndoHelper (&fBefore, fSelStart, endOfInsert, fCmdName);
        }
    }
    fCommandComplete = true;
}

/*
 ********************************************************************************
 ************************* TextInteractor::PreReplaceInfo ***********************
 ********************************************************************************
 */
TextInteractor::PreReplaceInfo::PreReplaceInfo ()
    : fTextInteractor (nullptr)
    , fUpdateMode (TextInteractor::eNoUpdate)
    , fFrom (0)
    , fTo (0)
    , fWithWhatCharCount (0)
    , fBoundingUpdateMarker ()
    , fBoundingUpdateHeight (0)
    , fStableTypingRegionHeight (0)
{
}

TextInteractor::PreReplaceInfo::~PreReplaceInfo ()
{
    if (fTextInteractor != nullptr) {
        fTextInteractor->GetTextStore ().RemoveMarker (&fBoundingUpdateMarker);
        fTextInteractor = nullptr;
    }
}

TextInteractor::UpdateMode TextInteractor::PreReplaceInfo::GetUpdateMode () const
{
    return fUpdateMode;
}

size_t TextInteractor::PreReplaceInfo::GetFrom () const
{
    return fFrom;
}

size_t TextInteractor::PreReplaceInfo::GetTo () const
{
    return fTo;
}

/*
 ********************************************************************************
 ******************************** TextInteractor ********************************
 ********************************************************************************
 */

TextInteractor::CommandNames      TextInteractor::sCommandNames  = TextInteractor::MakeDefaultCommandNames ();
TextInteractor::DialogSupport*    TextInteractor::sDialogSupport = nullptr;
TextInteractor::SearchParameters  TextInteractor::sSearchParameters;
TextInteractor::ReplaceParameters TextInteractor::sReplaceParameters;

TextInteractor::TextInteractor ()
    : fCommandHandler (nullptr)
    , fSpellCheckEngine (nullptr)
    , fSuppressCommandBreaksContext (false)
    , fDefaultUpdateMode (eDelayedUpdate)
    , fSmartCutAndPasteMode (true)
    , fClickCount (0)
    , fLastClickedAt (0.0f)
    , fLastMouseDownAt (Led_Point (0, 0))
    , fWholeWindowInvalid (false)
    , fUseSecondaryHilight (false)
    , fUseBitmapScrollingOptimization (true)
    , fSuppressTypedControlCharacters (true)
    , fInteractiveUpdadeMode (eIndeterminateInteractiveUpdateMode)
    , fTmpPreReplaceInfo ()
    , fDoingUpdateModeReplaceOn (nullptr)
    , fCaretShown (false)
    , fLeftSideOfSelectionInteresting (false)
    , fCaretShownAfterPos (true)
    , fInternalizer ()
    , fExternalizer ()
    ,
    //fScrollBarType (),
    fScrollBarParamsValid (false)
    , fLastScrolledAt (0.0f)
#if qMultiByteCharacters
//fMultiByteInputCharBuf (),
#endif
{
#if qMultiByteCharacters
    fMultiByteInputCharBuf[0] = '\0';
#endif
    fScrollBarType[h] = eScrollBarNever;
    fScrollBarType[v] = eScrollBarNever;
}

TextInteractor::~TextInteractor ()
{
    Assert (fCommandHandler == nullptr); // must be set to nullptr before we are destroyed...
    // just sanity check - no real reason...
    Assert (fSpellCheckEngine == nullptr); // DITTO
}

TextInteractor::CommandNames TextInteractor::MakeDefaultCommandNames ()
{
    TextInteractor::CommandNames cmdNames;
    cmdNames.fTypingCommandName = Led_SDK_TCHAROF ("Typing");
    cmdNames.fCutCommandName    = Led_SDK_TCHAROF ("Cut");
    cmdNames.fClearCommandName  = Led_SDK_TCHAROF ("Clear");
    cmdNames.fPasteCommandName  = Led_SDK_TCHAROF ("Paste");
    cmdNames.fUndoFormatString  = Led_SDK_TCHAROF ("Undo %s");
    cmdNames.fRedoFormatString  = Led_SDK_TCHAROF ("ReDo %s");
#if qPlatform_Windows
    cmdNames.fUndoFormatString += Led_SDK_TCHAROF ("\tCtrl+Z");
    cmdNames.fRedoFormatString += Led_SDK_TCHAROF ("\tCtrl+Y");
#endif
    cmdNames.fReplaceCommandName               = Led_SDK_TCHAROF ("Replace");
    cmdNames.fReplaceAllCommandName            = Led_SDK_TCHAROF ("Replace All");
    cmdNames.fReplaceAllInSelectionCommandName = Led_SDK_TCHAROF ("Replace All In Selection");
    return cmdNames;
}

/*
@METHOD:        TextInteractor::OnUpdateCommand
@ACCESS:        public
@DESCRIPTION:   <p>
            </p>
*/
bool TextInteractor::OnUpdateCommand (CommandUpdater* enabler)
{
    RequireNotNull (enabler);
    switch (enabler->GetCmdID ()) {
        case kSelectAll_CmdID: {
            enabler->SetEnabled (true);
            return true;
        }
        case kCut_CmdID: {
            OnUpdateCutCopyClearCommand (enabler);
            return true;
        }
        case kCopy_CmdID: {
            OnUpdateCutCopyClearCommand (enabler);
            return true;
        }
        case kPaste_CmdID: {
            OnUpdatePasteCommand (enabler);
            return true;
        }
        case kClear_CmdID: {
            OnUpdateCutCopyClearCommand (enabler);
            return true;
        }
        case kUndo_CmdID: {
            OnUpdateUndoRedoCommand (enabler);
            return true;
        }
        case kRedo_CmdID: {
            OnUpdateUndoRedoCommand (enabler);
            return true;
        }
        case kFind_CmdID: {
            OnUpdateFindCommands (enabler);
            return true;
        }
        case kFindAgain_CmdID: {
            OnUpdateFindCommands (enabler);
            return true;
        }
        case kEnterFindString_CmdID: {
            OnUpdateFindCommands (enabler);
            return true;
        }
        case kReplace_CmdID: {
            OnUpdateFindCommands (enabler);
            return true;
        }
        case kReplaceAgain_CmdID: {
            OnUpdateFindCommands (enabler);
            return true;
        }
        case kSpellCheck_CmdID: {
            OnUpdateSpellCheckCommand (enabler);
            return true;
        }
        case kSelectWord_CmdID:
        case kSelectTextRow_CmdID:
        case kSelectParagraph_CmdID: {
            OnUpdateSelectTextCommand (enabler);
            return true;
        }
    }
    return false;
}

/*
@METHOD:        TextInteractor::OnPerformCommand
@ACCESS:        public
@DESCRIPTION:   <p>
            </p>
*/
bool TextInteractor::OnPerformCommand (CommandNumber commandNumber)
{
    switch (commandNumber) {
        case kSelectAll_CmdID: {
            OnSelectAllCommand ();
            return true;
        }
        case kCut_CmdID: {
            OnCutCommand ();
            return true;
        }
        case kCopy_CmdID: {
            OnCopyCommand ();
            return true;
        }
        case kPaste_CmdID: {
            OnPasteCommand ();
            return true;
        }
        case kClear_CmdID: {
            OnClearCommand ();
            return true;
        }
        case kUndo_CmdID: {
            OnUndoCommand ();
            return true;
        }
        case kRedo_CmdID: {
            OnRedoCommand ();
            return true;
        }
        case kFind_CmdID: {
            OnFindCommand ();
            return true;
        }
        case kFindAgain_CmdID: {
            OnFindAgainCommand ();
            return true;
        }
        case kEnterFindString_CmdID: {
            OnEnterFindString ();
            return true;
        }
        case kReplace_CmdID: {
            OnReplaceCommand ();
            return true;
        }
        case kReplaceAgain_CmdID: {
            OnReplaceAgainCommand ();
            return true;
        }
        case kSpellCheck_CmdID: {
            OnSpellCheckCommand ();
            return true;
        }
        case kSelectWord_CmdID:
        case kSelectTextRow_CmdID:
        case kSelectParagraph_CmdID: {
            OnPerformSelectTextCommand (commandNumber);
            return true;
        }
    }
    return false;
}

void TextInteractor::OnUpdateCutCopyClearCommand (CommandUpdater* enabler)
{
    RequireNotNull (enabler);
    size_t start;
    size_t end;
    static_cast<TextImager*> (this)->GetSelection (&start, &end);
    enabler->SetEnabled (start != end);
}

void TextInteractor::OnUpdatePasteCommand (CommandUpdater* enabler)
{
    RequireNotNull (enabler);
    //tmphack
    enabler->SetEnabled (true);
}

void TextInteractor::OnUpdateUndoRedoCommand (CommandUpdater* enabler)
{
    RequireNotNull (enabler);
    if (GetCommandHandler () == nullptr) {
        enabler->SetEnabled (false);
    }
    else {
        if (enabler->GetCmdID () == kUndo_CmdID) {
            enabler->SetEnabled (GetCommandHandler ()->CanUndo ());

            Led_SDK_String menuItemText = Characters::CString::Format (GetCommandNames ().fUndoFormatString.c_str (), GetCommandHandler ()->GetUndoCmdName ());
            enabler->SetText (menuItemText.c_str ());
        }
        else if (enabler->GetCmdID () == kRedo_CmdID) {
            enabler->SetEnabled (GetCommandHandler ()->CanRedo ());

            Led_SDK_String menuItemText = Characters::CString::Format (GetCommandNames ().fRedoFormatString.c_str (), GetCommandHandler ()->GetRedoCmdName ());
            enabler->SetText (menuItemText.c_str ());
        }
    }
}

void TextInteractor::OnUpdateSelectTextCommand (CommandUpdater* enabler)
{
    RequireNotNull (enabler);
    enabler->SetEnabled (true);
}

void TextInteractor::OnPerformSelectTextCommand (CommandNumber commandNumber)
{
    // Note: these are intentionally made not undoable, since they don't modify data
    size_t oldSelStart = GetSelectionStart ();
    size_t oldSelEnd   = GetSelectionEnd ();
    size_t newSelStart = oldSelStart;
    size_t newSelEnd   = oldSelEnd;
    switch (commandNumber) {
        case kSelectWord_CmdID: {
            size_t wordStart = 0;
            size_t wordEnd   = 0;
            bool   wordReal  = false;
            GetTextStore ().FindWordBreaks (oldSelStart, &wordStart, &wordEnd, &wordReal);
            if (wordReal) {
                Assert (wordStart <= oldSelStart);
                newSelStart = wordStart;
            }
            else {
                // See if we were just after a word
                GetTextStore ().FindWordBreaks (FindPreviousCharacter (oldSelStart), &wordStart, &wordEnd, &wordReal);
                if (wordReal) {
                    Assert (wordStart <= oldSelStart);
                    newSelStart = wordStart;
                }
            }

            GetTextStore ().FindWordBreaks (oldSelEnd, &wordStart, &wordEnd, &wordReal);
            if (wordReal) {
                newSelEnd = wordEnd;
            }
        } break;
        case kSelectTextRow_CmdID: {
            newSelStart = GetStartOfRowContainingPosition (oldSelStart);
            // check if oldSelEnd is ALREADY the end of a row - and if so - don't change it,
            // but otherwise, do so.
            if (oldSelStart == oldSelEnd or oldSelEnd != GetStartOfRowContainingPosition (oldSelEnd)) {
                newSelEnd = GetEndOfRowContainingPosition (oldSelEnd);
            }
        } break;
        case kSelectParagraph_CmdID: {
            newSelStart = GetTextStore ().GetStartOfLineContainingPosition (oldSelStart);

            // check if oldSelEnd is ALREADY the end of a paragraph - and if so - don't change it,
            // but otherwise, do so.
            if (oldSelStart == oldSelEnd or oldSelEnd != GetTextStore ().GetStartOfLineContainingPosition (oldSelEnd)) {
                newSelEnd = GetTextStore ().GetEndOfLineContainingPosition (oldSelEnd);
                // grab one past endofline - so we select the NEWLINE at the end...
                if (newSelEnd < GetEnd ()) {
                    Led_tChar c;
                    GetTextStore ().CopyOut (newSelEnd, 1, &c);
                    if (c == '\n') {
                        newSelEnd = FindNextCharacter (newSelEnd);
                    }
                }
            }
        } break;
    }
    SetSelection (newSelStart, newSelEnd);
}

namespace {
    vector<Led_tString> MergeRecentFindStrings (const Led_tString& s, const vector<Led_tString>& oldRecents);
}

void TextInteractor::OnFindCommand ()
{
    SearchParameters parameters = GetSearchParameters ();
    bool             pressedOK  = false;
    GetDialogSupport ().DisplayFindDialog (&parameters.fMatchString, parameters.fRecentFindStrings, &parameters.fWrapSearch, &parameters.fWholeWordSearch, &parameters.fCaseSensativeSearch, &pressedOK);
    parameters.fRecentFindStrings = MergeRecentFindStrings (parameters.fMatchString, parameters.fRecentFindStrings);
    SetSearchParameters (parameters);
    if (pressedOK) {
        OnFindAgainCommand ();
    }
}

void TextInteractor::OnReplaceCommand ()
{
    SearchParameters                    parameters  = GetSearchParameters ();
    ReplaceParameters                   rParameters = GetReplaceParameters ();
    DialogSupport::ReplaceButtonPressed pressed     = GetDialogSupport ().DisplayReplaceDialog (&parameters.fMatchString, parameters.fRecentFindStrings, &rParameters.fReplaceWith, &parameters.fWrapSearch, &parameters.fWholeWordSearch, &parameters.fCaseSensativeSearch);
    parameters.fRecentFindStrings                   = MergeRecentFindStrings (parameters.fMatchString, parameters.fRecentFindStrings);
    SetSearchParameters (parameters);
    SetReplaceParameters (rParameters);
    switch (pressed) {
        case TextInteractor::DialogSupport::eReplaceButton_Find:
            OnFindAgainCommand ();
            break;
        case TextInteractor::DialogSupport::eReplaceButton_Replace:
            OnDoReplaceCommand (parameters, rParameters.fReplaceWith);
            break;
        case TextInteractor::DialogSupport::eReplaceButton_ReplaceAll:
            OnDoReplaceAllCommand (parameters, rParameters.fReplaceWith);
            break;
        case TextInteractor::DialogSupport::eReplaceButton_ReplaceAllInSelection:
            OnDoReplaceAllInSelectionCommand (parameters, rParameters.fReplaceWith);
            break;
    }
}

void TextInteractor::OnReplaceAgainCommand ()
{
    SearchParameters  parameters  = GetSearchParameters ();
    ReplaceParameters rParameters = GetReplaceParameters ();
    OnDoReplaceCommand (parameters, rParameters.fReplaceWith);
}

void TextInteractor::OnDoReplaceCommand (const SearchParameters& searchFor, const Led_tString& replaceWith)
{
    BreakInGroupedCommands ();
    // search for last text entered into find dialog (could have been a find again).
    size_t origSelStart = GetSelectionStart ();
    size_t origSelEnd   = GetSelectionEnd ();
    size_t whereTo      = GetTextStore ().Find (searchFor, origSelEnd);
    if ((whereTo == kBadIndex) or (whereTo == origSelStart and whereTo + searchFor.fMatchString.length () == origSelEnd)) {
        Led_BeepNotify ();
    }
    else {
        InteractiveModeUpdater iuMode (*this);
        size_t                 replaceStart = whereTo;
        size_t                 replaceEnd   = whereTo + searchFor.fMatchString.length ();
        UndoableContextHelper  undoContext (*this, GetCommandNames ().fReplaceCommandName, replaceStart, replaceEnd, GetSelectionStart (), GetSelectionEnd (), false);
        {
            InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), replaceWith.c_str (), replaceWith.length ());
            SetSelection (whereTo, whereTo + replaceWith.length ());
        }
        undoContext.CommandComplete ();
        ScrollToSelection ();
    }
    BreakInGroupedCommands ();
}

void TextInteractor::OnDoReplaceAllCommand (const SearchParameters& searchFor, const Led_tString& replaceWith)
{
    InteractiveModeUpdater iuMode (*this);
    BreakInGroupedCommands ();
    size_t startAt = 0;
    while (true) {
        size_t whereTo = GetTextStore ().Find (searchFor, startAt, GetTextStore ().GetEnd ());
        if (whereTo == kBadIndex) {
            break;
        }
        else {
            size_t                replaceStart = whereTo;
            size_t                replaceEnd   = whereTo + searchFor.fMatchString.length ();
            UndoableContextHelper undoContext (*this, GetCommandNames ().fReplaceAllCommandName, replaceStart, replaceEnd, GetSelectionStart (), GetSelectionEnd (), false);
            {
                InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), replaceWith.c_str (), replaceWith.length ());
                SetSelection (whereTo, whereTo + replaceWith.length ());
                startAt = whereTo + replaceWith.length ();
            }
            undoContext.CommandComplete ();
        }
    }
    ScrollToSelection ();
    BreakInGroupedCommands ();
}

void TextInteractor::OnDoReplaceAllInSelectionCommand (const SearchParameters& searchFor, const Led_tString& replaceWith)
{
    InteractiveModeUpdater iuMode (*this);
    BreakInGroupedCommands ();
    size_t     startAt = GetSelectionStart ();
    TempMarker selectionRegion (GetTextStore (), startAt, GetSelectionEnd ());
    while (true) {
        Assert (startAt <= selectionRegion.GetEnd ());
        size_t whereTo = GetTextStore ().Find (searchFor, startAt, selectionRegion.GetEnd ());
        if (whereTo == kBadIndex) {
            break;
        }
        else {
            size_t                replaceStart = whereTo;
            size_t                replaceEnd   = whereTo + searchFor.fMatchString.length ();
            UndoableContextHelper undoContext (*this, GetCommandNames ().fReplaceAllInSelectionCommandName, replaceStart, replaceEnd, GetSelectionStart (), GetSelectionEnd (), false);
            {
                InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), replaceWith.c_str (), replaceWith.length ());
                SetSelection (whereTo, whereTo + replaceWith.length ());
                startAt = whereTo + replaceWith.length ();
            }
            undoContext.CommandComplete ();
        }
    }
    ScrollToSelection ();
    BreakInGroupedCommands ();
}

void TextInteractor::OnFindAgainCommand ()
{
    TextStore::SearchParameters parameters = GetSearchParameters ();

    // search for last text entered into find dialog (could have been a find again).
    size_t origSelStart = GetSelectionStart ();
    size_t origSelEnd   = GetSelectionEnd ();
    size_t whereTo      = GetTextStore ().Find (parameters, origSelEnd);
    if ((whereTo == kBadIndex) or (whereTo == origSelStart and whereTo + parameters.fMatchString.length () == origSelEnd)) {
        Led_BeepNotify ();
    }
    else {
        SetSelection (whereTo, whereTo + parameters.fMatchString.length ());
        ScrollToSelection ();
    }
}

void TextInteractor::OnEnterFindString ()
{
    SearchParameters parameters = GetSearchParameters ();

    size_t selStart  = GetSelectionStart ();
    size_t selEnd    = GetSelectionEnd ();
    size_t selLength = selEnd - selStart;

    Memory::SmallStackBuffer<Led_tChar> buf (selLength);
    CopyOut (selStart, selLength, buf);
    parameters.fMatchString       = Led_tString (buf, selLength);
    parameters.fRecentFindStrings = MergeRecentFindStrings (parameters.fMatchString, parameters.fRecentFindStrings);
    SetSearchParameters (parameters);
}

void TextInteractor::OnUpdateFindCommands (CommandUpdater* enabler)
{
    RequireNotNull (enabler);
    if (enabler->GetCmdID () == kFind_CmdID) {
        enabler->SetEnabled (true);
    }
    else if (enabler->GetCmdID () == kFindAgain_CmdID) {
        enabler->SetEnabled (GetSearchParameters ().fMatchString.length () != 0);
    }
    else if (enabler->GetCmdID () == kEnterFindString_CmdID) {
        enabler->SetEnabled (GetSelectionStart () != GetSelectionEnd ());
    }
    else if (enabler->GetCmdID () == kReplace_CmdID) {
        enabler->SetEnabled (true);
    }
    else if (enabler->GetCmdID () == kReplaceAgain_CmdID) {
        enabler->SetEnabled (GetSearchParameters ().fMatchString.length () != 0);
    }
}

TextInteractor::SearchParameters TextInteractor::GetSearchParameters () const
{
    return sSearchParameters;
}

void TextInteractor::SetSearchParameters (const SearchParameters& sp)
{
    sSearchParameters = sp;
}

TextInteractor::ReplaceParameters TextInteractor::GetReplaceParameters () const
{
    return sReplaceParameters;
}

void TextInteractor::SetReplaceParameters (const ReplaceParameters& rp)
{
    sReplaceParameters = rp;
}

vector<Led_tString> TextInteractor::MergeRecentFindStrings (const Led_tString& s, const vector<Led_tString>& oldRecents)
{
    const unsigned int  kMaxEntries = 20;
    vector<Led_tString> result      = oldRecents;
    // See if the given string appears in the list. If so - erase it (so it only appears once). Prepend the new string
    // (most recent) to the top of the list. Truncate any entries beyond some magic #.
    vector<Led_tString>::iterator i = std::find (result.begin (), result.end (), s);
    if (i != result.end ()) {
        result.erase (i);
    }
    result.insert (result.begin (), s);
    if (result.size () > kMaxEntries) {
        result.erase (result.begin () + kMaxEntries, result.end ());
    }
    return result;
}

void TextInteractor::OnSpellCheckCommand ()
{
    if (GetSpellCheckEngine () == nullptr) {
        Led_BeepNotify ();
    }
    else {
        MyCallback cb (*this);
        GetDialogSupport ().DisplaySpellCheckDialog (cb);
    }
}

void TextInteractor::OnUpdateSpellCheckCommand (CommandUpdater* enabler)
{
    RequireNotNull (enabler);
    enabler->SetEnabled (GetSpellCheckEngine () != nullptr);
}

/*
@METHOD:        TextInteractor::SetDefaultUpdateMode
@ACCESS:        protected
@DESCRIPTION:   <p>TextInteractor's have an associated default UpdateMode. This is the update mode which is
            used by methods which used the eDefaultUpdateMode argument (most default UpdateMode args in Led are this value).</p>
                <p>This should <em>not</em> be set directly. Instead, instantiate a @'TextInteractor::TemporarilySetUpdateMode' object on
            the stack.</p>
*/
void TextInteractor::SetDefaultUpdateMode (UpdateMode defaultUpdateMode)
{
    if (defaultUpdateMode != eDefaultUpdate) { // setting update mode to 'eDefaultUpdate' is synonomous with a NO-OP
        fDefaultUpdateMode = defaultUpdateMode;
    }
}

/*
@METHOD:        TextInteractor::LooksLikeSmartPastableText
@ACCESS:        protected
@DESCRIPTION:   <p>This is called internally when text is pasted or dropped in a Led text buffer.
            Right now - it looks at the selection range to see if the text surrounding the selection boundaries
            has space characters and sets this information into @'TextInteractor::SmartCNPInfo'.
            </p>
                <p>This code is still far from perfect, but now (SPR#1286) appears to work pretty decently.
            It may still see significant revision at some point as part of SPR #1040.
            </p>
*/
bool TextInteractor::LooksLikeSmartPastableText ([[maybe_unused]] const Led_tChar* text, size_t /*nTextTChars*/, SmartCNPInfo* smartCNPInfo) const
{
    RequireNotNull (text);
    RequireNotNull (smartCNPInfo);
    if (GetSmartCutAndPasteMode ()) {
        size_t selStart = GetSelectionStart ();
        size_t selEnd   = GetSelectionEnd ();

        // If both current and prev chars where non-space, insert a space between.
        if (selStart == 0 or selStart >= GetEnd ()) {
            smartCNPInfo->fWordBreakAtSelStart = true;
        }
        else {
            size_t    prev = FindPreviousCharacter (selStart);
            Led_tChar prevC;
            CopyOut (prev, 1, &prevC);
            Led_tChar c;
            CopyOut (selStart, 1, &c);
            smartCNPInfo->fWordBreakAtSelStart = (IsShouldBeSepWithWhitespaceWordChar (c) != IsShouldBeSepWithWhitespaceWordChar (prevC));
        }

        if (selEnd == 0 or selEnd >= GetEnd ()) {
            smartCNPInfo->fWordBreakAtSelEnd = true;
        }
        else {
            size_t    prev = FindPreviousCharacter (selEnd);
            Led_tChar prevC;
            CopyOut (prev, 1, &prevC);
            Led_tChar c;
            CopyOut (selEnd, 1, &c);
            smartCNPInfo->fWordBreakAtSelEnd = (IsShouldBeSepWithWhitespaceWordChar (c) != IsShouldBeSepWithWhitespaceWordChar (prevC));
        }
    }
    return true;
}

/*
@METHOD:        TextInteractor::OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds
@ACCESS:        protected
@DESCRIPTION:   <p>See @'TextInteractor::GetSmartCutAndPasteMode'.</p>
                <p>The way in which which we handle smart cut-and-paste is twofold. We have sometimes add extra spaces
            (which is done in this routine), and sometimes we must delete extra spaces
            (which is done in @'TextInteractor::OptionallyExpandSelectionForSmartCutAndPasteModeDeletes').</p>
                <p>We must be careful todo our space insertion in such a way that it is friendly to the UNDO system (doesn't
            appear as a separate undo event etc).</p>
                <p>Note - this routine is called after the update has taken place. And it looks at the text
            at the LHS of where the insertion was done, and at the RHS of where it finihsed. The RHS of where
            it finished is always assumed to be the result of a call to @'TextImager::GetSelectionEnd'. And the start
            is given by the 'selStart' argument.</p>
*/
void TextInteractor::OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (size_t selStart, const SmartCNPInfo& smartCNPInfo)
{
    size_t selEnd = GetSelectionEnd ();

    Require (0 <= selStart);
    Require (selStart <= selEnd);
    Require (selEnd <= GetTextStore ().GetEnd ());

    if (GetSmartCutAndPasteMode ()) {
        if (selEnd > 0 and selEnd < GetTextStore ().GetEnd ()) {
            size_t    prev = FindPreviousCharacter (selEnd);
            Led_tChar prevC;
            CopyOut (prev, 1, &prevC);
            Led_tChar c;
            CopyOut (selEnd, 1, &c);
            if (smartCNPInfo.fWordBreakAtSelEnd and IsShouldBeSepWithWhitespaceWordChar (c) and IsShouldBeSepWithWhitespaceWordChar (prevC)) {
                InteractiveReplace_ (selEnd, selEnd, LED_TCHAR_OF (" "), 1, false);
                // Cannot pass 'updateSelection' flag in the above InteractiveReplace_ () call because
                // that code only works under restrictive circumstances (see require calls in that code).
                // Still - we MUST advance the end of the selection to take into account the space added. Not
                // just for cosmetic reasons, but because the current (as of Led 2.3b8) - and probably all future versions-
                // Undo code uses the selection end at the end of insertions to see how much text needs to be
                // saved for undoing purposes.
                // LGP990401
                selEnd++;
                SetSelection (selEnd, selEnd);
            }
        }
        // If both current and prev chars where non-space, insert a space between.
        if (selStart > 0 and selStart < GetTextStore ().GetEnd ()) {
            size_t    prev = FindPreviousCharacter (selStart);
            Led_tChar prevC;
            CopyOut (prev, 1, &prevC);
            Led_tChar c;
            CopyOut (selStart, 1, &c);
            if (smartCNPInfo.fWordBreakAtSelStart and IsShouldBeSepWithWhitespaceWordChar (c) and IsShouldBeSepWithWhitespaceWordChar (prevC)) {
                // Programming Confusion Note:
                //
                //  Not quite sure why we pass the updateCursorPosition=false param here. Seems like it should
                //  be true. But that seems to screw up the enclosing undo information.
                //  Well, I guess if it aint broke, don't fix it. Least not right now... --LGP 970315.
                InteractiveReplace_ (selStart, selStart, LED_TCHAR_OF (" "), 1, false);
            }
            else {
                if (IsSmartSpace (c) and IsSmartSpace (prevC)) {
                    InteractiveReplace_ (selStart, FindNextCharacter (selStart), nullptr, 0, false);
                }
            }
        }
    }
}

/*
@METHOD:        TextInteractor::OptionallyExpandSelectionForSmartCutAndPasteModeDeletes
@DESCRIPTION:   <p>See @'TextInteractor::GetSmartCutAndPasteMode'.</p>
*/
void TextInteractor::OptionallyExpandSelectionForSmartCutAndPasteModeDeletes (size_t* selStart, size_t* selEnd)
{
    RequireNotNull (selStart);
    RequireNotNull (selEnd);
    Require (0 <= *selStart);
    Require (*selStart <= *selEnd);
    Require (*selEnd <= GetTextStore ().GetEnd ());

    if (GetSmartCutAndPasteMode ()) {
        size_t realStart = *selStart;
        size_t realEnd   = *selEnd;
        size_t newStart  = realStart;
        size_t newEnd    = realEnd;

        // if selStart is in whitespace, or in the middle of a word,
        // or selSend is just after whitespace or in the middle of a word then don't be smart.
        // LGP 970315 (amended LGP990415)
        {
            if (realStart < GetEnd ()) {
                Led_tChar c;
                CopyOut (realStart, 1, &c);
                if (Character (c).IsWhitespace ()) {
                    return;
                }
                if (realStart > 0) {
                    CopyOut (FindPreviousCharacter (realStart), 1, &c);
                    if (IsShouldBeSepWithWhitespaceWordChar (c)) {
                        return;
                    }
                }
            }

            if (realStart < realEnd and realEnd < GetEnd ()) {
                Led_tChar c;
                CopyOut (FindPreviousCharacter (realEnd), 1, &c);
                if (Character (c).IsWhitespace ()) {
                    return;
                }
                if (realEnd < GetEnd ()) {
                    CopyOut (realEnd, 1, &c);
                    if (IsShouldBeSepWithWhitespaceWordChar (c)) {
                        return;
                    }
                }
            }
        }

        // Now see if at the cursor we have extra spaces - scan first backward, and then forward. Should be at most one.
        Led_tChar c;
        // back
        {
            if (newStart > 0) {
                size_t prev = FindPreviousCharacter (newStart);
                CopyOut (prev, 1, &c);
                size_t    prevprev = FindPreviousCharacter (prev);
                Led_tChar prevprevC;
                CopyOut (prevprev, 1, &prevprevC);
                if (prevprev != prev and IsSmartSpace (c) and not(IsSmartSpace (prevprevC) or prevprevC == '\n')) {
                    newStart = prev;
                }
            }
        }
        // and forth
        {
            if (newEnd < GetEnd ()) {
                CopyOut (newEnd, 1, &c);
                Led_tChar charBeforeStart = '\0';
                if (newStart != 0) {
                    CopyOut (FindPreviousCharacter (newStart), 1, &charBeforeStart);
                }
                if (IsSmartSpace (c)) {
                    Assert (newEnd < FindNextCharacter (newEnd)); // Assert not looping!
                    newEnd = FindNextCharacter (newEnd);
                    if (newEnd < GetEnd ()) {
                        // As a result of this change - don't put two chars together that should be separated by whitespace
                        Led_tChar nextChar = '\0';
                        CopyOut (newEnd, 1, &nextChar);
                        if (IsShouldBeSepWithWhitespaceWordChar (charBeforeStart) and IsShouldBeSepWithWhitespaceWordChar (nextChar)) {
                            newEnd = FindPreviousCharacter (newEnd);
                        }
                    }
                }
            }
        }

        *selStart = newStart;
        *selEnd   = newEnd;
    }
}

/*
@METHOD:        TextInteractor::SetSelectionShown
@DESCRIPTION:   <p>See TextInteractor::GetSelectionShown. Typically this method isn't called directly by user code, but
    from within the class library wrappers (e.g. Led_MFC) on gain/lose focus events.</p>
*/
void TextInteractor::SetSelectionShown (bool shown)
{
    SetSelectionShown (shown, eDefaultUpdate);
}

void TextInteractor::SetSelectionShown (bool shown, UpdateMode updateMode)
{
    if (GetSelectionShown () != shown) {
        TextImager::SetSelectionShown (shown);
        Refresh (GetSelectionStart (), GetSelectionEnd (), updateMode);
    }
}

/*
@METHOD:        TextInteractor::SetSelection
@DESCRIPTION:   <p>See @'TextInteractor::GetSelection'.</p>
*/
void TextInteractor::SetSelection (size_t start, size_t end)
{
    Assert (end <= GetEnd ());
#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (start);
    Assert_CharPosDoesNotSplitCharacter (end);
#endif

    UpdateMode updateMode = GetDefaultUpdateMode ();

    // This isn't quite perfect for the case of eImmediateUpdate- but should be close enough...
    if (start != GetSelectionStart () or end != GetSelectionEnd ()) {
        IdleManager::NonIdleContext nonIdleContext;

        size_t oldSelectionStart = GetSelectionStart ();
        size_t oldSelectionEnd   = GetSelectionEnd ();

        /*
         *  Update fLeftSideOfSelectionInteresting. if start changed, we are interested in LHS, and
         *  if end changed we are interested in RHS. But if BOTH changed then just assume interested
         *  in RHS.
         */
        if (start == GetSelectionStart ()) {
            fLeftSideOfSelectionInteresting = false;
        }
        else if (end == GetSelectionEnd ()) {
            fLeftSideOfSelectionInteresting = true;
        }
        else if ((start < GetSelectionStart ()) == (end < GetSelectionEnd ())) {
            fLeftSideOfSelectionInteresting = (start < GetSelectionStart ());
        }

        /*
         *  Note we must invalidate the caret state before and after changing the selction so
         *  that we erase the old position, and draw in the new.
         */
        InvalidateCaretState ();

        TextImager::SetSelection (start, end);

        if ((GetSelectionShown () or GetUseSecondaryHilight ()) and updateMode != eNoUpdate) {
            /*
             *  What we REALLY want to invalidate is the CHANGE in hilight region. Not the UNION.
             *
             *  The difference (invalid area) is Union (a,b)-Intersection(a,b). Now with the facilities at our
             *  disposal, there doesn't appear any obvious way to compute this. So we hack it out
             *  a bit.
             */
            UpdateMode useUpdateMode = (updateMode == eImmediateUpdate) ? eDelayedUpdate : updateMode;

            size_t lhsOuter = min (oldSelectionStart, GetSelectionStart ()); // left of ALL 4
            size_t rhsOuter = max (oldSelectionEnd, GetSelectionEnd ());     // right of ALL 4
            size_t lhsInner = max (oldSelectionStart, GetSelectionStart ()); // the two inner ones - lhsInner could be >=< rhsInner
            size_t rhsInner = min (oldSelectionEnd, GetSelectionEnd ());
            Assert (lhsOuter <= rhsOuter); // See!- left of ALL 4
            Assert (lhsOuter <= lhsInner);
            Assert (lhsOuter <= rhsInner);
            Assert (lhsOuter <= rhsOuter); // See!- right of ALL 4
            Assert (lhsInner <= rhsOuter);
            Assert (rhsInner <= rhsOuter);
            /*
             * SPR#0973 - added in the FindPrev/FindNext calls to expand the region we update slightly. Reason is cuz
             * with new hilight display code - expanding the selection slightly at the end can affect the other end by
             *  up to one character (rarely - but best to over-invalidate than under-invalidate).
             */
            Refresh (FindPreviousCharacter (lhsOuter), FindNextCharacter (lhsInner), useUpdateMode);
            Refresh (FindPreviousCharacter (rhsInner), FindNextCharacter (rhsOuter), useUpdateMode);
        }

        InvalidateCaretState ();
    }
    if (updateMode == eImmediateUpdate) {
        Update ();
    }
    RecomputeSelectionGoalColumn ();
}

void TextInteractor::SetSelection (size_t start, size_t end, UpdateMode updateMode)
{
    TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
    TextImager*              tim = this; // Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
    tim->SetSelection (start, end);
}

/*
@METHOD:        TextInteractor::ScrollToSelection
@DESCRIPTION:   <p>Scroll the current window, so that the selection is showing. Calls TextInteractor::ScrollSoShowing ().
    Since TextInteractor::ScrollSoShowing () requires two marker-positions to try to show, we use the instance variable
    fLeftSideOfSelectionInteresting to guess which side of the selection the user is probably most interested in.
    This routine also pays attention to @'TextImager::GetCaretShownAfterPos' so that it shows the right part of
    the screen in ambiguous cases.</p>
        <p>See TextImager::ScrollSoShowing for more details. But basicly this tries to show the entire selection, if
    possible. And uses the flag to pick which side of the selection to use if only part of it can be made visible in the
    current window at once.</p>
*/
void TextInteractor::ScrollToSelection (UpdateMode updateMode, bool forceShowSelectionEndpoint)
{
    size_t selStart = GetSelectionStart ();
    size_t selEnd   = GetSelectionEnd ();

    /*
     * Based on SPR#0940. If some of the selection is showing - its not totally clear the user would want us
     * to scroll the entire screen to show another part of the selection. Maybe yes - maybe no. But clearly - if the
     * ENTIRE SCREEN is selected (as in this SPR - then we don't want to scroll to show a different part of the selection.
     * I THINK so anyhow... LGP 2001-05-24
     *
     *  PLUS - fix SPR#1051 - added forceShowSelectionEndpoint flag, so when using cursors, we can force a scroll.
     */
    if (not forceShowSelectionEndpoint and selStart <= GetMarkerPositionOfStartOfWindow () and selEnd >= GetMarkerPositionOfEndOfWindow ()) {
        return;
    }

    /*
     *  Why is this so complicated? And is it right?
     *
     *  There are a couple of issues here. One is that the semantics of @'TextImager::ScrollSoShowing'
     *  are very precise, but perhaps slightly unintuitive. We try to show the CHARACTERS (not marker positions)
     *  given by the marker-positions specified.
     *
     *  But when you are cursoring around, what you want to see are the CHARACTERS - not the MARKER POSITIONS.
     *
     *  In these ambiguous cases (like at a wrap point), we want to make sure we get the interpretation of which
     *  character to show correct. And when the user must choose (cannot show whole selection) we want to make sure
     *  we AT LEAST show the IMPORTANT side of the selection.
     *
     *  The 'lastCharShown  =   FindPreviousCharacter (selEnd)' bit is cuz we want to show the character BEFORE that
     *  marker position. And for most APIs the marker pos names the character after that marker position.
     *
     *      --  LGP990212
     */
    size_t firstCharShown = selStart;
    size_t lastCharShown  = FindPreviousCharacter (selEnd);
    if (fLeftSideOfSelectionInteresting) {
        if (firstCharShown != lastCharShown and GetStartOfRowContainingPosition (firstCharShown) != GetStartOfRowContainingPosition (lastCharShown)) {
            lastCharShown = firstCharShown;
        }
        ScrollSoShowing (firstCharShown, lastCharShown, updateMode);
    }
    else {
        if (GetCaretShownAfterPos ()) {
            lastCharShown = FindNextCharacter (lastCharShown);
        }
        if (firstCharShown != lastCharShown and GetStartOfRowContainingPosition (firstCharShown) != GetStartOfRowContainingPosition (lastCharShown)) {
            firstCharShown = lastCharShown;
        }
        ScrollSoShowing (lastCharShown, firstCharShown, updateMode);
    }
}

void TextInteractor::HookLosingTextStore ()
{
    HookLosingTextStore_ ();
    TextImager::HookLosingTextStore ();
}

void TextInteractor::HookLosingTextStore_ ()
{
    AbortReplace (fTmpPreReplaceInfo);
    fExternalizer.reset ();
    fInternalizer.reset ();
}

void TextInteractor::HookGainedNewTextStore ()
{
    HookGainedNewTextStore_ ();
    TextImager::HookGainedNewTextStore ();
}

void TextInteractor::HookGainedNewTextStore_ ()
{
    if (fExternalizer.get () == nullptr) {
        SetExternalizer (MakeDefaultExternalizer ());
    }
    if (fInternalizer.get () == nullptr) {
        SetInternalizer (MakeDefaultInternalizer ());
    }
}

/*
@METHOD:        TextInteractor::ProcessSimpleClick
@ACCESS:        protected
@DESCRIPTION:   <p>Bad name. Basicly does a small portion of the click handling code. Vectored out
    not only for code sharing, but to provide a hook we can use in the StyledTextInteractor code
    to handle embeddings (without massive cut/n/paste duplication of code).</p>
        <p>What this does is to convert the click (or double or tripple click into an appropriate
    selection. The anchor only has meaning for double clicks with exteneded selection on.
        <p>This routine uses the existing selection, and just adjusts that selection.</p>
        <p>This routine returns true if all went well, and you should continue with interpretting
    click. It returns false if you should assume something clicked on swallowed up the click.</p>
        <p>This is not a GREAT line to have drawn for the API boundary. But its a start.</p>
        <p>LGP 960303</p>
*/
bool TextInteractor::ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor)
{
    RequireNotNull (dragAnchor);
    switch (clickCount) {
        case 1: {
            size_t newPos      = GetCharAtClickLocation (clickedAt);
            size_t newSelStart = newPos;
            size_t newSelEnd   = newPos;
            {
                WhileTrackingConstrainSelection (&newSelStart, &newSelEnd);
                newPos = newSelStart;
            }
            if (extendSelection) {
                newSelStart = min (newSelStart, GetSelectionStart ());
                newSelEnd   = max (newSelEnd, GetSelectionEnd ());
            }

            /*
                 *  Set the drag anchor:
                 *
                 *  Note - we use @'TextInteractor::GetCharAtClickLocation' instead of @'TextImager::GetCharAtWindowLocation'
                 *  so we get the right intended click location between two characters (for an anchor point for drag selection).
                 *
                 *  Code that is looking to see if the user clicked on an embedding itself will directly call
                 *  @'TextImager::GetCharAtWindowLocation'.
                 */
            if (extendSelection) {
                // if we're extending the selection, the anchor should be the OTHER end of the selection (SPR#1364)
                if (newPos == newSelStart) {
                    *dragAnchor = newSelEnd;
                }
                else {
                    *dragAnchor = newSelStart;
                }
            }
            else {
                *dragAnchor = newPos;
            }

            // Set flag for how to display caret based on where we clicked
            if (not extendSelection) {
                SetCaretShownAfterPos (GetCharWindowLocation (newPos).top <= clickedAt.v);
            }

            WhileTrackingConstrainSelection (&newSelStart, &newSelEnd);
            SetSelection (newSelStart, newSelEnd);
        } break;

        default: {
            // others are ignored
        } break;
    }
#if 0
    DbgTrace ("TextInteractor::ProcessSimpleClick (tickCount=%f, newMousePos=(%d,%d), clickCount=%d, extendSel=%d, newSelStart=%d, newSelEnd=%d)\n",
              Time::GetTickCount (), clickedAt.v, clickedAt.h, clickCount, extendSelection, GetSelectionStart (), GetSelectionEnd ()
             );
#endif
    return true;
}

/*
@METHOD:        TextInteractor::UpdateClickCount
@DESCRIPTION:   <p>Helper to implemented best feeling UI for double click detection.</p>
*/
void TextInteractor::UpdateClickCount (Time::DurationSecondsType clickAtTime, const Led_Point& clickAtLocation)
{
    if (ClickTimesAreCloseForDoubleClick (clickAtTime) and PointsAreCloseForDoubleClick (clickAtLocation)) {
        IncrementCurClickCount (clickAtTime);
    }
    else {
        SetCurClickCount (1, clickAtTime);
    }
    fLastMouseDownAt = clickAtLocation;
}

/*
@METHOD:        TextInteractor::ClickTimesAreCloseForDoubleClick
@DESCRIPTION:   <p>Helper to implemented best feeling UI for double click detection. See also @'TextInteractor::UpdateClickCount' ().</p>
*/
bool TextInteractor::ClickTimesAreCloseForDoubleClick (Time::DurationSecondsType thisClick)
{
    return (fLastClickedAt + Led_GetDoubleClickTime () >= thisClick);
}

/*
@METHOD:        TextInteractor::PointsAreCloseForDoubleClick
@DESCRIPTION:   <p>Helper to implemented best feeling UI for double click detection. See also @'TextInteractor::UpdateClickCount' ().</p>
*/
bool TextInteractor::PointsAreCloseForDoubleClick (const Led_Point& p)
{
    const Led_Coordinate kMultiClickDistance = 4;
    Led_Coordinate       hDelta              = p.h - fLastMouseDownAt.h;
    if (hDelta < 0) {
        hDelta = -hDelta;
    }
    Led_Coordinate vDelta = p.v - fLastMouseDownAt.v;
    if (vDelta < 0) {
        vDelta = -vDelta;
    }
    return ((hDelta <= kMultiClickDistance) and (vDelta <= kMultiClickDistance));
}

/*
@METHOD:        TextInteractor::WhileSimpleMouseTracking
@DESCRIPTION:   <p>Helper to share code among implementations. Call this while mouse is down to handle autoscrolling,
    and selection updating.</p>
*/
void TextInteractor::WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor)
{
#if qDynamiclyChooseAutoScrollIncrement
    Foundation::Time::DurationSecondsType        now              = Time::GetTickCount ();
    static Foundation::Time::DurationSecondsType sLastTimeThrough = 0.0f;
    const Foundation::Time::DurationSecondsType  kClickThreshold  = Led_GetDoubleClickTime () / 3;
    bool                                         firstClick       = (now - sLastTimeThrough > kClickThreshold);

    int increment = firstClick ? 1 : 2;
#else
    const int increment = 1;
#endif

    size_t rhsPos = GetCharAtClickLocation (newMousePos);
    {
        size_t ignored = rhsPos;
        WhileTrackingConstrainSelection (&rhsPos, &ignored);
    }

    /*
     *  Handle vertical autoscrolling, if necessary.
     */
    if (rhsPos < GetMarkerPositionOfStartOfWindow ()) {
        ScrollByIfRoom (-1, eImmediateUpdate);
        rhsPos = GetMarkerPositionOfStartOfWindow ();
    }
    else if (rhsPos > GetMarkerPositionOfEndOfWindow ()) {
        ScrollByIfRoom (1, eImmediateUpdate);
        rhsPos = FindNextCharacter (GetMarkerPositionOfEndOfWindow ()); // So we select past to end of window
    }

    /*
     *  And horizontal.
     */
    const int kHScrollIncrementFactor = 4;
    if (newMousePos.h < GetWindowRect ().left) {
        if (GetHScrollPos () > 0) {
            SetHScrollPos (max (0, int (GetHScrollPos ()) - increment * kHScrollIncrementFactor));
        }
    }
    else if (newMousePos.h > GetWindowRect ().right) {
        SetHScrollPos (min (static_cast<Led_Coordinate> (GetHScrollPos () + increment * kHScrollIncrementFactor), static_cast<Led_Coordinate> (ComputeMaxHScrollPos ())));
    }

    size_t newSelStart = min (rhsPos, dragAnchor);
    size_t newSelEnd   = max (rhsPos, dragAnchor);
    WhileTrackingConstrainSelection (&newSelStart, &newSelEnd);
    SetSelection (newSelStart, newSelEnd, eImmediateUpdate);

#if qDynamiclyChooseAutoScrollIncrement
    sLastTimeThrough = now;
#endif
#if 0
    DbgTrace ("TextInteractor::WhileSimpleMouseTracking (tickCount=%f, newMousePos=(%d,%d), dragAnchor=%d, newSelStart=%d, newSelEnd=%d)\n",
              Time::GetTickCount (), newMousePos.v, newMousePos.h, dragAnchor, newSelStart, newSelEnd
             );
#endif
}

/*
@METHOD:        TextInteractor::WhileTrackingConstrainSelection
@DESCRIPTION:   <p>Override this to provide unusual selection behavior during
        tracking. For example, if you want to only allow selection of whole lines when
        the caps-lock key is down (OK, silly, but its an example), you could check for that
        state in this routine, and assure selStart and selEnd are adjusted to meet those
        criteria.</p>
            <p>By default this calls calls @'TextInteractor::WhileTrackingConstrainSelection_ForWholeWords',
        for double clicks and @'TextInteractor::WhileTrackingConstrainSelection_ForWholeRows' for 3 or more clicks.</p>
*/
void TextInteractor::WhileTrackingConstrainSelection (size_t* selStart, size_t* selEnd)
{
    RequireNotNull (selStart);
    RequireNotNull (selEnd);
    Require (GetCurClickCount () > 0);
    switch (GetCurClickCount ()) {
        case 1: {
            // No constraints on a single click
        } break;

        case 2: {
            WhileTrackingConstrainSelection_ForWholeWords (selStart, selEnd);
        } break;

        default: {
            // any more than 3 and we constrain to whole rows
            WhileTrackingConstrainSelection_ForWholeRows (selStart, selEnd);
        } break;
    }
}

/*
@METHOD:        TextInteractor::WhileTrackingConstrainSelection_ForWholeWords
@DESCRIPTION:   <p>See @'TextInteractor::WhileTrackingConstrainSelection'</p>
*/
void TextInteractor::WhileTrackingConstrainSelection_ForWholeWords (size_t* selStart, size_t* selEnd)
{
    RequireNotNull (selStart);
    RequireNotNull (selEnd);

    size_t wordStart = 0;
    size_t wordEnd   = 0;
    bool   wordReal  = false;
    GetTextStore ().FindWordBreaks (*selStart, &wordStart, &wordEnd, &wordReal);
    *selStart = wordStart;

    GetTextStore ().FindWordBreaks (*selEnd, &wordStart, &wordEnd, &wordReal);
#if qDoubleClickSelectsSpaceAfterWord
    if (wordReal) {
        // select the space forward...
        size_t xWordStart = 0;
        size_t xWordEnd   = 0;
        bool   xWordReal  = false;
        GetTextStore ().FindWordBreaks (wordEnd, &xWordStart, &xWordEnd, &xWordReal);
        if (not xWordReal) {
            wordEnd = xWordEnd;
        }
    }
#endif
    *selEnd = wordEnd;
}

/*
@METHOD:        TextInteractor::WhileTrackingConstrainSelection_ForWholeRows
@DESCRIPTION:   <p>See @'TextInteractor::WhileTrackingConstrainSelection'</p>
*/
void TextInteractor::WhileTrackingConstrainSelection_ForWholeRows (size_t* selStart, size_t* selEnd)
{
    RequireNotNull (selStart);
    RequireNotNull (selEnd);
    Require (*selStart <= *selEnd);

    size_t origSelStart = *selStart;
    size_t origSelEnd   = *selEnd;

    *selStart = GetStartOfRowContainingPosition (origSelStart);
    *selEnd   = GetStartOfNextRowFromRowContainingPosition (*selEnd);
    // if at end of buffer, go to end of buffer. Else, select to start of next row
    if (*selEnd <= origSelEnd) {
        *selEnd = GetEndOfRowContainingPosition (origSelEnd);
    }
}

/*
@METHOD:        TextInteractor::GetCharAtClickLocation
@DESCRIPTION:   <p>Just like @'TextImager::GetCharAtWindowLocation'(), but doesn't try to
    find the character which encloses the particular point. Rather - we try to
    find the character the user was probably trying
    to click at to position the cursor. This is typically either the same, or the character
    just just following. If we are clicking towards the end of the character - we probably
    wanted to click just <em>before</em> the next character. Note this works for BIDI characters.</p>
*/
size_t TextInteractor::GetCharAtClickLocation (const Led_Point& where) const
{
    size_t clickedOnChar   = GetCharAtWindowLocation (where);
    size_t endOfClickedRow = GetEndOfRowContainingPosition (clickedOnChar);

    /*
     *  SPR#1597 (was originally SPR#1232). A click past the end of the wrap point in a wrapped
     *  line has a value EQUAL to the end of the row. However - its also EQUAL to the start of
     *  the following row. The trouble is that GetEndOfRowContainingPosition (THAT_POSITION)
     *  will return the end of the FOLLOWING row in that case.
     *
     *  Since in order for this to happen there must be 1 or more characters in the row (to be a wrap point),
     *  we can look at the PRECEEDING character, and compute ITS End-of-Row. If thats the same as the
     *  returned click-position - we must have clicked on the end-of-row, and so we correct
     *  the value of 'endOfClickedRow'.
     */
    if (GetEndOfRowContainingPosition (FindPreviousCharacter (clickedOnChar)) == clickedOnChar) {
        endOfClickedRow = clickedOnChar;
    }

    Assert (GetStartOfRowContainingPosition (clickedOnChar) <= clickedOnChar);
    Assert (clickedOnChar <= endOfClickedRow);

    if (clickedOnChar < endOfClickedRow) { // Don't wrap cuz click past end - LGP 950424
        Led_Rect charRect           = GetCharWindowLocation (clickedOnChar);
        bool     clickedToLHSOfChar = (where.h <= charRect.left + Led_Coordinate (charRect.GetWidth ()) / 2 and charRect.GetWidth () != 0);
        if (GetTextDirection (clickedOnChar) == eLeftToRight) {
            if (not clickedToLHSOfChar) {
                clickedOnChar = FindNextCharacter (clickedOnChar);
            }
        }
        else {
            if (clickedToLHSOfChar) {
                clickedOnChar = FindNextCharacter (clickedOnChar);
            }
        }
    }
    return (clickedOnChar);
}

void TextInteractor::Draw (const Led_Rect& /*subsetToDraw*/, bool /*printing*/)
{
    NoteWindowPartiallyUpdated ();
    // don't call cuz there is none defined - pure virtual...TextImager::Draw (subsetToDraw, printing);
}

void TextInteractor::DrawBefore (const Led_Rect& /*subsetToDraw*/, bool /*printing*/)
{
    // LGP 2003-03-20 - not 100% sure this is necessary or a good idea - but done to keep backward
    // compat with old behavior. Once new idle code in place - test without this. See SPR#1366.
    if (not fScrollBarParamsValid) {
        UpdateScrollBars ();
    }
}

void TextInteractor::DrawAfter (const Led_Rect& /*subsetToDraw*/, bool printing)
{
    if (GetUseSecondaryHilight () and not GetSelectionShown () and not printing) {
        Led_Region r;
        GetSelectionWindowRegion (&r, GetSelectionStart (), GetSelectionEnd ());
        Tablet_Acquirer tablet_ (this);
        Led_Tablet      tablet = tablet_;
        tablet->FrameRegion (r, Led_GetSelectedTextBackgroundColor ());
    }
}

void TextInteractor::SetTopRowInWindow (size_t newTopRow, UpdateMode updateMode)
{
    TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
    TextImager*              tim = this; // Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
    tim->SetTopRowInWindow (newTopRow);
}

void TextInteractor::ScrollByIfRoom (ptrdiff_t downBy, UpdateMode updateMode)
{
    TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
    TextImager*              tim = this; // Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
    tim->ScrollByIfRoom (downBy);
}

void TextInteractor::ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos, UpdateMode updateMode)
{
    TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
    TextImager*              tim = this; // Dynamicly bind to 2-arg version. Direct call would select overloaded version from this class!
    tim->ScrollSoShowing (markerPos, andTryToShowMarkerPos);
}

void TextInteractor::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont, UpdateMode updateMode)
{
    TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
    TextImager*              tim = this; // Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
    tim->SetDefaultFont (defaultFont);
    Refresh ();
}

void TextInteractor::SetWindowRect (const Led_Rect& windowRect, UpdateMode updateMode)
{
    TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
    TextImager*              tim = this; // Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
    tim->SetWindowRect (windowRect);
}

void TextInteractor::SetHScrollPos (Led_Coordinate hScrollPos)
{
    PreScrollInfo preScrollInfo;
    PreScrollHelper (eDefaultUpdate, &preScrollInfo);
    TextImager::SetHScrollPos (hScrollPos);
    PostScrollHelper (preScrollInfo);
}

void TextInteractor::SetHScrollPos (Led_Coordinate hScrollPos, UpdateMode updateMode)
{
    TemporarilySetUpdateMode updateModeSetter (*this, updateMode);
    TextImager*              tim = this; // Dynamicly bind to 1-arg version. Direct call would select overloaded version from this class!
    tim->SetHScrollPos (hScrollPos);
}

void TextInteractor::PreScrollHelper (UpdateMode updateMode, PreScrollInfo* preScrollInfo)
{
    UpdateMode realUpdateMode  = RealUpdateMode (updateMode);
    preScrollInfo->fUpdateMode = realUpdateMode;
    if (realUpdateMode != eNoUpdate) {
        preScrollInfo->fOldWindowStart    = GetMarkerPositionOfStartOfWindow ();
        preScrollInfo->fOldHScrollPos     = GetHScrollPos ();
        preScrollInfo->fTryTodoScrollbits = GetUseBitmapScrollingOptimization () and bool (realUpdateMode == eImmediateUpdate);
        if (preScrollInfo->fTryTodoScrollbits) {
            preScrollInfo->fOldLastRowStart = GetMarkerPositionOfStartOfLastRowOfWindow ();
            try {
                Update (false); // so the stuff we scroll-bits is up to date...
                // Wouldn't want to scroll stale bits :-)
            }
            catch (...) {
                preScrollInfo->fTryTodoScrollbits = false;
            }
        }
    }
}

void TextInteractor::PostScrollHelper (PreScrollInfo preScrollInfo)
{
    /*
     *  Only if we scrolled do we need to refresh screen.
     */
    size_t newStartOfWindow   = GetMarkerPositionOfStartOfWindow ();
    preScrollInfo.fUpdateMode = RealUpdateMode (preScrollInfo.fUpdateMode);
    if (preScrollInfo.fUpdateMode != eNoUpdate and
        ((preScrollInfo.fOldWindowStart != newStartOfWindow) or
         (preScrollInfo.fOldHScrollPos != GetHScrollPos ()))) {

        // Don't try this except if we only got vertical scrolling - at least for now...
        if (preScrollInfo.fTryTodoScrollbits and preScrollInfo.fOldHScrollPos == GetHScrollPos ()) {
            Led_Rect windowRect = GetWindowRect ();

            Tablet_Acquirer tablet_ (this);
            Led_Tablet      tablet = tablet_;
            if (preScrollInfo.fOldWindowStart > newStartOfWindow) {
                /*                                    *
                 *  Move text (bits) DOWN screen (UP /|\ ARROW).
                 *                                    |
                 *                                    |
                 */
                Led_Coordinate newPos = GetCharWindowLocation (GetStartOfRowContainingPosition (preScrollInfo.fOldWindowStart)).top;
                if (newPos > 0) {
                    try {
                        tablet->ScrollBitsAndInvalRevealed (windowRect, newPos - windowRect.top);
                    }
                    catch (...) {
                        // Ignore any errors - just don't do scrollbits then...
                        preScrollInfo.fTryTodoScrollbits = false;
                    }
                }
                else {
                    preScrollInfo.fTryTodoScrollbits = false; // can happen, for example, if only one row fits in window...
                }
                if (preScrollInfo.fTryTodoScrollbits) {
                    /*
                     *  Very subtle speed hack. It turns out, in this case, we may need to update a sliver on the top
                     *  of the window (exposed by scrollbits), and the bottom of the window (cuz we don't show partial
                     *  rows). So the BOUNDING RECTANLGE of the update region is the whole window. Why is this
                     *  a problem? In our update code, we use the bounding rectangle of the update region for
                     *  logical clipping. This means we end up drawing (though clipped out) every row of text.
                     *  This can make the scroll operation needlessly slow. By simply doing the update of the
                     *  top sliver first, we assure we always have a nice rectangular update region, so our
                     *  later optimizations work better. -- LGP 961030
                     */
                    Update ();

                    // Now we may not want to allow the partial line to be displayed. Leave that choice to the
                    // logic in the imager, and repaint the area past the end of the last row
                    Led_Coordinate lastRowBottom = GetCharWindowLocation (GetMarkerPositionOfStartOfLastRowOfWindow ()).bottom;
                    Led_Rect       eraser        = windowRect;
                    eraser.top                   = lastRowBottom;
                    RefreshWindowRect (eraser, preScrollInfo.fUpdateMode);
                }
            }
            else {
                /*
                 *  Move text (bits) UP screen (DOWN | ARROW).
                 *                                   |
                 *                                  \|/
                 *                                   *
                 */
                Led_Coordinate newPos = GetCharLocationRowRelativeByPosition (newStartOfWindow, preScrollInfo.fOldWindowStart, 5).top;

                if (newPos > 0) {
                    try {
                        tablet->ScrollBitsAndInvalRevealed (windowRect, -newPos);
                    }
                    catch (...) {
                        // Ignore any errors - just don't do scrollbits then...
                        preScrollInfo.fTryTodoScrollbits = false;
                    }
                }
                else {
                    preScrollInfo.fTryTodoScrollbits = false; // can happen, for example, if only one row fits in window...
                }

                if (preScrollInfo.fTryTodoScrollbits) {
                    // now refresh the exposed portion at the bottom. Note that much of it may have been exposed
                    // already by the InvalRgn/ScrollWindow calls. But we may have to invalidate even more cuz we don't
                    // show entire bottom lines.
                    Led_Coordinate lastRowBottom = GetCharWindowLocation (preScrollInfo.fOldLastRowStart).bottom;
                    Led_Rect       eraser        = windowRect;
                    eraser.top                   = lastRowBottom;
                    RefreshWindowRect (eraser, eDelayedUpdate);
                }

                if (preScrollInfo.fUpdateMode == eImmediateUpdate) {
                    Update ();
                }
            }

            // ScrollBitsing succeeded, so we can return now, and avoid the below REFRESH.
            // Otherwise, fall through, and handle things the old-fashioned way...
            if (preScrollInfo.fTryTodoScrollbits) {
                return;
            }
        }

        Refresh (preScrollInfo.fUpdateMode);
    }
}

void TextInteractor::Replace (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode)
{
    if (from != to or withWhatCharCount != 0) {
        Assert (fDoingUpdateModeReplaceOn == nullptr);
        fDoingUpdateModeReplaceOn = this;
        try {
            PreReplaceInfo preReplaceInfo;
            PreReplace (from, to, withWhatCharCount, updateMode, &preReplaceInfo);
            GetTextStore ().Replace (from, to, withWhat, withWhatCharCount);
            PostReplace (preReplaceInfo);
            Assert (fDoingUpdateModeReplaceOn == this);
        }
        catch (...) {
            Assert (fDoingUpdateModeReplaceOn == this);
            fDoingUpdateModeReplaceOn = nullptr;
            throw;
        }
        fDoingUpdateModeReplaceOn = nullptr;
    }
}

void TextInteractor::AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo)
{
    TextImager::AboutToUpdateText (updateInfo);
    if (fDoingUpdateModeReplaceOn != this) {
        //  Sometimes a textstore sends notifications about updates beyond the end of the text (e.g. when setting
        //  styles which apply to newly typed characters). We only pay attention up to the end of the text
        //  (since that is all we display)
        PreReplace (updateInfo.fReplaceFrom, min (updateInfo.fReplaceTo, GetEnd ()), updateInfo.fTextLength, eDefaultUpdate, &fTmpPreReplaceInfo);
    }
}

void TextInteractor::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    TextImager::DidUpdateText (updateInfo);
    if (fDoingUpdateModeReplaceOn != this) {
        try {
            PostReplace (fTmpPreReplaceInfo);
        }
        catch (...) {
            Refresh (); // shouldn't happen? But if it does - this is probably the best we can do to handle it...
        }
    }
}

void TextInteractor::PreReplace (size_t from, size_t to, size_t withWhatCharCount, UpdateMode updateMode, PreReplaceInfo* preReplaceInfo)
{
    RequireNotNull (preReplaceInfo);

    if (preReplaceInfo->fTextInteractor != nullptr) { // used as flag to indicate cleaned up state (marker removed)
        AbortReplace (*preReplaceInfo);
    }

    updateMode = RealUpdateMode (updateMode);

    preReplaceInfo->fFrom              = from;
    preReplaceInfo->fTo                = to;
    preReplaceInfo->fWithWhatCharCount = withWhatCharCount;
    preReplaceInfo->fUpdateMode        = updateMode;

    if (updateMode == eNoUpdate) {
        return;
    }

    if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
        preReplaceInfo->fUpdateMode = eNoUpdate;
        return;
    }

    try {
        /*
         *  Every once in a while, somebody will try todo an edit operation outside of
         *  the visible window. Though this is rare, when it DOES happen, there are frequently
         *  several of them (as in reading in a file). So we attempt to specially tweek this
         *  case. Do a preliminary and then secondary test so we don't slow needlessly waste time
         *  for the more common case.
         *
         *  Note - we could also just as reasonably tweek the case where we are inserting BEFORE
         *  the start of the window, but I've found no cases where that comes up, so I don't bother
         *  for now.
         *
         *  LGP 960515.
         */
        {
            size_t endOfWindow = GetMarkerPositionOfEndOfWindow ();
            if (from > endOfWindow) {
                /*
                 *  Adding text JUST after the end of the window could - in principle - affect
                 *  the word-breaks of the end of window, so we must be a little more careful.
                 */
                size_t endOfNextRow = GetEndOfRowContainingPosition (FindNextCharacter (endOfWindow));
                if (from > endOfNextRow) {
                    updateMode                  = eNoUpdate;
                    preReplaceInfo->fUpdateMode = eNoUpdate;
                    return;
                }
            }
        }

        Assert (updateMode != eNoUpdate);
        /*
         *  Grab a range that includes totally all the rows between the start selection and the end.
         *
         *  Save this region in a Marker (so it gets its bounds adjusted for the edit). Then compute
         *  its pixelBounds (really we just need its HEIGHT). We will use this later to see
         *  if the edit has caused a change in the pixel-height of the region, which would mean we
         *  have to draw not just that region, but all the way to the end of the window.
         *
         *  Also we will use it later to decide if the starting or ending row (word-breaks) have
         *  changed.
         */
        size_t startPositionOfRowWhereReplaceBegins = GetStartOfRowContainingPosition (from);
        size_t startPositionOfRowAfterReplaceEnds   = GetEndOfRowContainingPosition (to);
        if (startPositionOfRowAfterReplaceEnds < GetTextStore ().GetEnd ()) {
            startPositionOfRowAfterReplaceEnds = GetStartOfRowContainingPosition (FindNextCharacter (startPositionOfRowAfterReplaceEnds));
            Assert (GetEndOfRowContainingPosition (to) <= startPositionOfRowAfterReplaceEnds);
        }
        Assert (startPositionOfRowWhereReplaceBegins <= startPositionOfRowAfterReplaceEnds);

        preReplaceInfo->fBoundingUpdateHeight = GetTextWindowBoundingRect (startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds).GetHeight ();

        /*
         *  In case the above changes, we may be able to get away with only updating the stable
         *  typing region, assuming its height hasn't changed.
         */
        {
            size_t expandedFromMarkerPos = 0;
            size_t expandedToMarkerPos   = 0;
            GetStableTypingRegionContaingMarkerRange (from, to, &expandedFromMarkerPos, &expandedToMarkerPos);
            if (expandedFromMarkerPos == startPositionOfRowWhereReplaceBegins and expandedToMarkerPos == startPositionOfRowAfterReplaceEnds) {
                // Speed tweek. Avoid expensive call to GetTextWindowBoundingRect () - and use old value...
                preReplaceInfo->fStableTypingRegionHeight = preReplaceInfo->fBoundingUpdateHeight;
                Assert (preReplaceInfo->fStableTypingRegionHeight == GetTextWindowBoundingRect (expandedFromMarkerPos, expandedToMarkerPos).GetHeight ());
            }
            else {
                preReplaceInfo->fStableTypingRegionHeight = GetTextWindowBoundingRect (expandedFromMarkerPos, expandedToMarkerPos).GetHeight ();
            }
        }

        /*
         *  The marker is one past the end of the final row so any typing just after the end of the row
         *  gets included.
         */
        GetTextStore ().AddMarker (&preReplaceInfo->fBoundingUpdateMarker, startPositionOfRowWhereReplaceBegins, (startPositionOfRowAfterReplaceEnds - startPositionOfRowWhereReplaceBegins) + 1, this);
        preReplaceInfo->fTextInteractor = this; // assign after add, cuz this var serves as flag to indicate addMarker call done...
    }
    catch (NotFullyInitialized&) {
        // Fine - we can ignore that..
        preReplaceInfo->fUpdateMode = eNoUpdate;
        return;
    }
    catch (...) {
        throw;
    }
}

void TextInteractor::PostReplace (PreReplaceInfo& preReplaceInfo)
{
    UpdateMode updateMode = preReplaceInfo.fUpdateMode;
    if (updateMode != eNoUpdate) {
        size_t from              = preReplaceInfo.fFrom;
        size_t withWhatCharCount = preReplaceInfo.fWithWhatCharCount;

        size_t newTo = from + withWhatCharCount;

        // Subtract one from end cuz we added one earlier so chars appended would grow marker...
        size_t startPositionOfRowWhereReplaceBegins = preReplaceInfo.fBoundingUpdateMarker.GetStart ();
        size_t startPositionOfRowAfterReplaceEnds   = preReplaceInfo.fBoundingUpdateMarker.GetEnd () - 1;
        Assert (startPositionOfRowWhereReplaceBegins <= startPositionOfRowAfterReplaceEnds);

        size_t stableTypingRegionStart = 0;
        size_t stableTypingRegionEnd   = 0;
        GetStableTypingRegionContaingMarkerRange (from, newTo, &stableTypingRegionStart, &stableTypingRegionEnd);

        size_t expandedFromMarkerPos = 0;
        size_t expandedToMarkerPos   = 0;
        ExpandedFromAndToInPostReplace (from, newTo,
                                        stableTypingRegionStart, stableTypingRegionEnd,
                                        startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds,
                                        &expandedFromMarkerPos, &expandedToMarkerPos);

        Led_Rect windowRect               = GetWindowRect ();
        Led_Rect expandedFromToMarkerRect = GetTextWindowBoundingRect (expandedFromMarkerPos, expandedToMarkerPos);
        Led_Rect updateRect               = expandedFromToMarkerRect;

        // we must ALWAYS draw to the end of the row (including space after last character)
        updateRect.right = windowRect.right;

        // Now if we've changed the height of the bounding rows region, we need to repaint to end.
        {
            Led_Rect revisedRect = expandedFromToMarkerRect; // Speed tweek. Avoid expensive call to GetTextWindowBoundingRect () - and use old value...
            if (expandedFromMarkerPos != startPositionOfRowWhereReplaceBegins or expandedToMarkerPos != startPositionOfRowAfterReplaceEnds) {
                revisedRect = GetTextWindowBoundingRect (startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds);
            }
            Assert (revisedRect == GetTextWindowBoundingRect (startPositionOfRowWhereReplaceBegins, startPositionOfRowAfterReplaceEnds));

            if (preReplaceInfo.fBoundingUpdateHeight != revisedRect.GetHeight ()) {
                updateRect = Led_Rect (updateRect.top, windowRect.left, windowRect.bottom - updateRect.top, windowRect.GetWidth ());
            }
        }

        // Now if we've changed the height of the stable region, we need to repaint to end of screen.
        {
            Led_Rect revisedRect = expandedFromToMarkerRect; // Speed tweek. Avoid expensive call to GetTextWindowBoundingRect () - and use old value...
            if (expandedFromMarkerPos != stableTypingRegionStart or expandedToMarkerPos != stableTypingRegionEnd) {
                revisedRect = GetTextWindowBoundingRect (stableTypingRegionStart, stableTypingRegionEnd);
            }
            Assert (revisedRect == GetTextWindowBoundingRect (stableTypingRegionStart, stableTypingRegionEnd));
            if (preReplaceInfo.fStableTypingRegionHeight != revisedRect.GetHeight ()) {
                updateRect = Led_Rect (updateRect.top, windowRect.left, windowRect.bottom - updateRect.top, windowRect.GetWidth ());
            }
        }

        RefreshWindowRect (updateRect, updateMode);
    }
}

void TextInteractor::AbortReplace (PreReplaceInfo& preReplaceInfo)
{
    if (preReplaceInfo.fTextInteractor != nullptr) {
        // remove marker, and set to nullptr to indicate cleaned up.
        preReplaceInfo.fTextInteractor->GetTextStore ().RemoveMarker (&preReplaceInfo.fBoundingUpdateMarker);
        preReplaceInfo.fTextInteractor = nullptr;
    }
}

void TextInteractor::ExpandedFromAndToInPostReplace (size_t from, size_t newTo,
                                                     size_t stableTypingRegionStart, size_t stableTypingRegionEnd,
                                                     size_t startPositionOfRowWhereReplaceBegins, size_t startPositionOfRowAfterReplaceEnds,
                                                     size_t* expandedFrom, size_t* expandedTo)
{
    RequireNotNull (expandedFrom);
    RequireNotNull (expandedTo);

    // Edits in a row practically never change the word-break from the previous line, but they CAN - in
    // principle - (eg. in the second row there is a very long word, and you insert a space near the
    // beginning, the little word-let created might fit on the previous row, in which case you would
    // need to redisplay BOTH rows. But we only need to update previous rows or earlier text in this
    // row if the row-break point changes - thats what our marker-test here measures.
    size_t expandedFromMarkerPos = 0;
    if (GetStartOfRowContainingPosition (from) == startPositionOfRowWhereReplaceBegins) {
        expandedFromMarkerPos = from;

        // On windows - at least temporarily - editing one char can change the WIDTH/MeasureText
        // of the preceeding character, since we don't need to add on the overhang anymore.
        // So be sure to draw that previous character in that subtle case
        // - See SPR# 0340 - LGP 960516
        if (expandedFromMarkerPos > startPositionOfRowWhereReplaceBegins) {
            expandedFromMarkerPos = FindPreviousCharacter (expandedFromMarkerPos);
        }
    }
    else {
        // practically never happens....
        expandedFromMarkerPos = stableTypingRegionStart;
    }

    // Edits much more commonly (though still only a few percent of edits) can change the ending
    // row word-break line. A change here means we must redraw to the end of the stable-region
    // (typically line - aka paragraph).
    size_t expandedToMarkerPos = GetEndOfRowContainingPosition (newTo);
    {
        size_t nowStartOfNextRow = expandedToMarkerPos;
        if (nowStartOfNextRow < GetTextStore ().GetEnd ()) {
            nowStartOfNextRow = GetStartOfRowContainingPosition (FindNextCharacter (nowStartOfNextRow));
            Assert (expandedToMarkerPos <= nowStartOfNextRow);
        }

        if (nowStartOfNextRow != startPositionOfRowAfterReplaceEnds) {
            // then we changed word-breaks! Must go all the way to the end of the stable region.
            expandedToMarkerPos = stableTypingRegionEnd;
        }
    }

    *expandedFrom = expandedFromMarkerPos;
    *expandedTo   = expandedToMarkerPos;
}

void TextInteractor::InteractiveReplace (const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode)
{
    BreakInGroupedCommandsIfDifferentCommand (GetCommandNames ().fTypingCommandName);
    InteractiveModeUpdater iuMode (*this);
    UndoableContextHelper  undoContext (*this, GetCommandNames ().fTypingCommandName, withWhatCharCount == 0);
    {
        InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), withWhat, withWhatCharCount, true, true, updateMode);
        bool anyChanges = InteractiveReplaceEarlyPostReplaceHook (withWhatCharCount);
        if (withWhatCharCount == 1 and not anyChanges) {
            // need other tests as well???? Like same start location ?? Maybe done inside command-handler stuff????
            undoContext.SetSimplePlainTextInsertOptimization (true);
        }
    }
    undoContext.CommandComplete ();
}

/*
@METHOD:        TextInteractor::InteractiveReplace_
@DESCRIPTION:
*/
void TextInteractor::InteractiveReplace_ (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount,
                                          bool updateCursorPosition, bool validateTextForCharsetConformance,
                                          UpdateMode updateMode)
{
// Assert selection bounardaries valid Led_tChar boundaries
#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (from);
    Assert_CharPosDoesNotSplitCharacter (to);
#endif

    if (validateTextForCharsetConformance) {
        // Then check the GIVEN text - no assert here, just bad_input if text bad...
        if (not ValidateTextForCharsetConformance (withWhat, withWhatCharCount)) {
            OnBadUserInput ();
            return; // in case OnBadUserInput () doesn't throw
        }
    }

    TempMarker newSel (GetTextStore (), to + 1, to + 1); // NB: This marker pos MAY split a character - but that should cause
    // no problems.
    // We are only keeping this temporarily and we subtract one at the
    // end. Just want to make it past point where we do the insertion
    // so we get the right adjustment on

    Tablet_Acquirer performanceHackTablet (this); // sometimes acquiring/releaseing the tablet can be expensive,
    // and as a result of stuff later in this call, it happens
    // several times. By acquiring it here, we make the other calls
    // much cheaper (cuz its basicly free to acquire when already
    // acuired).

    SetCaretShownAfterPos (true); // by default show pos after - this is what works best for NL's and is what we
    // want most of the time...
    Replace (from, to, withWhat, withWhatCharCount, updateMode);
    if (updateCursorPosition) {
        size_t newSelection = newSel.GetStart ();
        if (newSelection > 0) {
            newSelection--;
        }

        SetSelection (newSelection, newSelection);
    }
}

/*
@METHOD:        TextInteractor::InteractiveReplaceEarlyPostReplaceHook
@DESCRIPTION:   <p>Hook function called AFTER the @'TextInteractor::InteractiveReplace_' in @'TextInteractor::InteractiveReplace', but
    <em>before</em> the @'TextInteractor::PreInteractiveUndoHelper' call. The need to get into this interval is if you need to update
    the text to <em>augment</em> the effect of the user typing, and what that effect to be reflected in the UNDO information.</p>
        <p>This happens, for example, with @'StandardStyledTextInteractor::InteractiveReplaceEarlyPostReplaceHook' where
    we want to set the font of the newly typed character according to the 'fEmptySelection' style. See spr#0604 for more details.</p>
        <p><em>OLD-CODE-NOTE</em>NB: This routine was changed in Led 3.1a8 to return a boolean result.</p>
        </p>The boolean return value must be true if any changes were made that could affect undo processing (i.e. that could affect
    what infomration must be saved for proper undo processing).</p>
*/
bool TextInteractor::InteractiveReplaceEarlyPostReplaceHook (size_t /*withWhatCharCount*/)
{
    return false;
}

/*
@METHOD:        TextInteractor::PreInteractiveUndoHelper
@DESCRIPTION:   <p>This is called early on - before any change happens - to preserve the contents of a region about to be updated - so
    that the region can be restored upon an UNDO command. The argument selStart/selEnd are NOT the actual selection regions - but
    rather the region (which maybe slightly larger) that needs to be preserved (perhaps the arg names should change?).</p>
        <p>Note - this can differ from the selection region because of SmartCutAndPaste - where we expand the selection slightly for
    a command to do funky stuff with whitespace.</p>
        <p>Note that the handed in selStart/selEnd arguments can be modified by this routine (due to a call to
    @'TextInteractor::PreInteractiveUndoHelperHook').</p>
        <p>We then preserve the ACTUAL selection at the time this was called in the resulting 'beforeRep' object.</p>
        <p>NB: new in Led 3.1a6 - we require that fCommandHandler != nullptr to call this.</p>
*/
void TextInteractor::PreInteractiveUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
{
    Require (regionStart <= regionEnd);
    Require (selStart <= selEnd);
    RequireNotNull (beforeRep);
    Require ((*beforeRep) == nullptr);
    RequireNotNull (fCommandHandler);

    try {
        (*beforeRep) = InteractiveUndoHelperMakeTextRep (regionStart, regionEnd, selStart, selEnd);
    }
    catch (...) {
        // any exceptions in here are cuz we don't have enuf memory to make this
        // command undoable. No matter. Proceed anyhow, ignoring the exception...
        // And commit any existing commands to make it more likely real code
        // succeeds, and cuz we don't want any funny undo behavior where some
        // commands in sequence might get skipped.
        AssertNotNull (fCommandHandler);
        fCommandHandler->Commit ();
    }
}

/*
@METHOD:        TextInteractor::PostInteractiveUndoHelper
@DESCRIPTION:   <p>This routine is called after a user action has taken place which is to be recorded for UNDOing.
            The 'startOfInsert' / 'endOfInsert' passed here refer to the region of text which must be preserved.
            The actual selection saved will be the currently selected text at the time this method is called.</p>
                <p>Note - the startOfInsert/endOfInsert can differ from the selection region because things like
            SmartCutAndPaste can expand the affected area of text to BEYOND what was actaully selected by the user.</p>
                <p>This method operatates by calling @'TextInteractor::PostInteractiveUndoPostHelper' with the
            beforeRep argument given this function and an afterRep computed herein.</p>
                <p>NB: As of Led 3.1a6 - we require that fCommandHandler != nullptr to call this.</p>
*/
void TextInteractor::PostInteractiveUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t startOfInsert, size_t endOfInsert, const Led_SDK_String& cmdName)
{
    RequireNotNull (beforeRep);
    RequireNotNull (*beforeRep); // This shouldn't be called if there was a problem creating beforeRep (exception)
    RequireNotNull (fCommandHandler);
    SavedTextRep* afterRep = nullptr;
    try {
        afterRep = InteractiveUndoHelperMakeTextRep (startOfInsert, endOfInsert, GetSelectionStart (), GetSelectionEnd ());
        PostInteractiveUndoPostHelper (beforeRep, &afterRep, startOfInsert, cmdName);
    }
    catch (...) {
        delete (*beforeRep);
        (*beforeRep) = nullptr;
        delete afterRep;
        afterRep = nullptr;
        throw; // safe at this point to throw - but perhaps better to silently eat the throw?
    }
}

/*
@METHOD:        TextInteractor::PostInteractiveSimpleCharInsertUndoHelper
@DESCRIPTION:   <p>Utility function for optimized undo support - keeping smaller objects in the undo buffer, and trying re-use/tweek
            an existing one in the common case of multiple consecutive characters typed.</p>
*/
void TextInteractor::PostInteractiveSimpleCharInsertUndoHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep, size_t startOfInsert, size_t endOfInsert, const Led_SDK_String& cmdName)
{
    RequireNotNull (beforeRep);
    RequireNotNull (*beforeRep); // This shouldn't be called if there was a problem creating beforeRep (exception)
    RequireNotNull (fCommandHandler);
    if (endOfInsert - startOfInsert == 1) {
        Led_tChar c;
        CopyOut (startOfInsert, 1, &c);
        if (fCommandHandler->PostUpdateSimpleTextInsert (startOfInsert, c)) {
            delete *beforeRep;
            *beforeRep = nullptr;
            return;
        }
        // at least create a plain-text guy if we cannot update current one...
        SavedTextRep* afterRep = new InteractiveReplaceCommand::PlainTextRep (GetSelectionStart (), GetSelectionEnd (), &c, 1);
        PostInteractiveUndoPostHelper (beforeRep, &afterRep, startOfInsert, cmdName);
        Assert (afterRep == nullptr); // cleared out by PostInteractiveUndoPostHelper ()
        return;
    }

    PostInteractiveUndoHelper (beforeRep, startOfInsert, endOfInsert, cmdName);
}

/*
@METHOD:        TextInteractor::PostInteractiveUndoPostHelper
@DESCRIPTION:   <p>This routine is called after a user action has taken place which is to be recorded for UNDOing.
            The routine simply posts an @'InteractiveReplaceCommand' (with the already saved before/after reps) to the current
            command handler.</p>
                <p>This method is typically called by @'TextInteractor::PostInteractiveUndoHelper'.</p>
                <p>NB: As of Led 3.1a6 - we require that fCommandHandler != nullptr to call this.</p>
*/
void TextInteractor::PostInteractiveUndoPostHelper (InteractiveReplaceCommand::SavedTextRep** beforeRep,
                                                    InteractiveReplaceCommand::SavedTextRep** afterRep,
                                                    size_t startOfInsert, const Led_SDK_String& cmdName)
{
    RequireNotNull (beforeRep);
    RequireNotNull (afterRep);
    RequireNotNull (fCommandHandler);
    try {
        if (*beforeRep != nullptr and *afterRep != nullptr) {
            // We declare temporaries here, and be careful to set things to nullptr at each stage to prevent double
            // deletes in the event of a badly timed exception
            InteractiveReplaceCommand* cmd = new InteractiveReplaceCommand (*beforeRep, *afterRep, startOfInsert, cmdName);
            *beforeRep                     = nullptr;
            *afterRep                      = nullptr;
            fCommandHandler->Post (cmd);
        }
    }
    catch (...) {
        delete *beforeRep;
        *beforeRep = nullptr;
        delete *afterRep;
        *afterRep = nullptr;
        throw; // safe at this point to throw - but perhaps better to silently eat the throw?
    }
}

InteractiveReplaceCommand::SavedTextRep* TextInteractor::InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
{
    if (regionStart == regionEnd) {
        // optimization, cuz these are smaller
        return new InteractiveReplaceCommand::PlainTextRep (selStart, selEnd, nullptr, 0);
    }
    else {
        return new FlavorSavorTextRep (this, regionStart, regionEnd, selStart, selEnd);
    }
}

void TextInteractor::OnUndoCommand ()
{
    InteractiveModeUpdater iuMode (*this);
    if (GetCommandHandler () != nullptr and GetCommandHandler ()->CanUndo ()) {
        GetCommandHandler ()->DoUndo (*this);
        ScrollToSelection ();
    }
    else {
        Led_BeepNotify ();
    }
}

void TextInteractor::OnRedoCommand ()
{
    InteractiveModeUpdater iuMode (*this);
    if (GetCommandHandler () != nullptr and GetCommandHandler ()->CanRedo ()) {
        GetCommandHandler ()->DoRedo (*this);
        ScrollToSelection ();
    }
    else {
        Led_BeepNotify ();
    }
}

void TextInteractor::Refresh (size_t from, size_t to, UpdateMode updateMode) const
{
    Require (from <= to);
    updateMode = RealUpdateMode (updateMode);
    if ((updateMode != eNoUpdate) and (from != to)) {
        if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
            return;
        }
        // we could be more precise - but no need. Just take the box bounding the two
        // endpoints.
        // I'd be more inclined to worry about optimizing this, but the most relevant plausible
        // usage of this routine - during typing - doesn't use it. So I believe we can get away with
        // being sloppy here - LGP 960516
        Led_Rect refreshRect = GetTextWindowBoundingRect (from, to);
        RefreshWindowRect_ (refreshRect, updateMode);
    }
}

void TextInteractor::Refresh (const Marker* range, UpdateMode updateMode) const
{
    RequireNotNull (range);
    updateMode = RealUpdateMode (updateMode);
    if (updateMode != eNoUpdate) {
        if (updateMode == eDelayedUpdate and IsWholeWindowInvalid ()) {
            return;
        }
        Refresh (range->GetStart (), range->GetEnd (), updateMode);
    }
}

/*
@METHOD:        TextInteractor::DoSingleCharCursorEdit
@ACCESS:        protected
@DESCRIPTION:   <p>Helper routine for handling cursoring done by user.</p>
 */
void TextInteractor::DoSingleCharCursorEdit (CursorMovementDirection direction, CursorMovementUnit movementUnit, CursorMovementAction action,
                                             UpdateMode updateMode, bool scrollToSelection)
{
    IdleManager::NonIdleContext nonIdleContext;

    size_t oldStartSel = GetSelectionStart ();
    size_t oldEndSel   = GetSelectionEnd ();
    Assert (GetSelectionEnd () <= GetLength () + 1);

    size_t newStartSel = oldStartSel;
    size_t newEndSel   = oldEndSel;

    UpdateMode useUpdateMode = (updateMode == eImmediateUpdate) ? eDelayedUpdate : updateMode;

    GoalColumnRecomputerControlContext skipRecompute (*this, action == eCursorMoving and movementUnit == eCursorByRow and (direction == eCursorBack or direction == eCursorForward));

    // In a couple of cases, we get burned by the ambiguity of location-specification at start and end
    // of row. This only matters when the users says navigate to start/end of row, and we happen to already
    // be there. In that special case, we will navigate to the start/end of the next row.
    //
    // This situation only occurs when we are at the end of a row which has been word-wrapped. This is because
    // in that special case, there is a flag - CaretShownAfterPos - which says if you show the caret before or
    // after the marker-pos of the selection.
    //
    // If we are in this situation, OVERRIDE the usual navigation logic.
    if (movementUnit == eCursorByRow and GetStartOfRowContainingPosition (newStartSel) == newStartSel) {
        if (GetCaretShownAfterPos ()) {
            if (direction == eCursorToEnd and GetEndOfRowContainingPosition (newStartSel) == newStartSel) {
                goto SkipNavigation;
            }
        }
        else {
            if (direction == eCursorToStart) {
                // fall through with usual 'to start of row' logic, but backup one characater within the row so
                // that code doesn't get fooled about which row we are on.
                newStartSel = FindPreviousCharacter (newStartSel);
            }
            if (direction == eCursorToEnd) {
                goto SkipNavigation;
            }
        }
    }

    //  Generally don't want to mess with the caret shown pos. But in these cases few cases,
    //  the user is indicating which he'd prefer.
    //  Perhaps there are more cases? Can we generalize/simplify?
    if (movementUnit == eCursorByChar) {
        SetCaretShownAfterPos (true);
    }
    if (movementUnit == eCursorByRow) {
        switch (direction) {
            case eCursorToStart:
                SetCaretShownAfterPos (true);
                break;
            case eCursorToEnd:
                SetCaretShownAfterPos (false);
                break;
        }
    }

    if (action != eCursorExtendingSelection or (oldStartSel == oldEndSel)) {
        fLeftSideOfSelectionInteresting = (direction == eCursorBack or direction == eCursorToStart);
    }
    if (fLeftSideOfSelectionInteresting) {
        newStartSel = ComputeRelativePosition (newStartSel, direction, movementUnit);
        if (action == eCursorMoving) { // only case where we do this - destroy/extend we keep track of start
            newEndSel = newStartSel;
        }
    }
    else {
        newEndSel = ComputeRelativePosition (newEndSel, direction, movementUnit);
        if (action == eCursorMoving) { // only case where we do this - destroy/extend we keep track of start
            newStartSel = newEndSel;
        }
    }

SkipNavigation:

    // The above can reverse start/end, so make sure that doesn't happen...
    if (newEndSel < newStartSel) {
        size_t tmp  = newStartSel;
        newStartSel = newEndSel;
        newEndSel   = tmp;
    }
    Assert (newStartSel <= newEndSel);

    /*
     *  Now that we know the new and old selection region, we can perform the action
     */
    switch (action) {
        case eCursorDestroying: {
            if (oldStartSel == oldEndSel) {
                /*
                     *  In this case, then the computations above for the NEW selection
                     *  can be considered valid.
                     */
                Assert (newEndSel >= newStartSel);
                size_t howMany = newEndSel - newStartSel;
                if (howMany >= 1) { // might be zero if we were backed up against the start of the buffer.
                    bool oldCutAndPaste = GetSmartCutAndPasteMode ();
                    try {
                        SetSmartCutAndPasteMode (false); // See SPR#1044 - when user hits backspace and has empty selection, dont use smart cut and paste...
                        {
                            BreakInGroupedCommandsIfDifferentCommand (GetCommandNames ().fClearCommandName);
                            InteractiveModeUpdater iuMode (*this);
                            UndoableContextHelper  undoContext (*this, GetCommandNames ().fClearCommandName, newStartSel, newEndSel, GetSelectionStart (), GetSelectionEnd (), true);
                            {
                                InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0, true, true, useUpdateMode);
                            }
                            undoContext.CommandComplete ();
                        }
                        // After the above deletion, we already end up the the selection adjustment being handled for us (and more correctly
                        // than we can easily due to smart cut and paste). So just grab the current (already correct) selection, to prevent
                        // our later call to SetSelection () from doing any harm.
                        GetSelection (&newStartSel, &newEndSel);
                        SetSmartCutAndPasteMode (oldCutAndPaste);
                    }
                    catch (...) {
                        SetSmartCutAndPasteMode (oldCutAndPaste);
                        throw;
                    }
                }
            }
            else {
                /*
                     *  Otherwise, if there WAS some selection, and we get any kind of
                     *  delete key, we REALLY just want to delete the selection, and
                     *  ignore whatever computation was done above for where to put
                     *  the new selection - it goes right to the old startSel.
                     */
                Assert (oldEndSel >= oldStartSel);
                Assert (oldStartSel == GetSelectionStart ());
                Assert (oldEndSel == GetSelectionEnd ());
                OnClearCommand ();
                // After the above deletion, we already end up the the selection adjustment being handled for us (and more correctly
                // than we can easily due to smart cut and paste). So just grab the current (already correct) selection, to prevent
                // our later call to SetSelection () from doing any harm.
                GetSelection (&newStartSel, &newEndSel);
            }
        } break;

        case eCursorMoving: {
            // Nothing todo (actual setting of selection done at the end)
        } break;

        case eCursorExtendingSelection: {
            // Nothing todo (actual setting of selection done at the end)
        } break;

        default:
            Assert (false); // bad direction argument
    }

    /*
     *  Buy this point, we've computed where we should be, and performed
     *  any actions on the text that needed to be taken (e.g. deleting).
     *  Nothing should have yet been redisplayed. Now we update the selection,
     *  perform any needed scrolling, and only then - display - if prescribed.
     */
    SetSelection (newStartSel, newEndSel, useUpdateMode);

    if (scrollToSelection) {
        ScrollToSelection (useUpdateMode, true);
    }

    if (updateMode == eImmediateUpdate) {
        Update ();
    }
}

void TextInteractor::OnCutCommand ()
{
    InteractiveModeUpdater iuMode (*this);
    BreakInGroupedCommands ();
    if (GetSelectionStart () != GetSelectionEnd ()) {
        OnCopyCommand ();
        UndoableContextHelper undoContext (*this, GetCommandNames ().fCutCommandName, true);
        {
            InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0);
        }
        undoContext.CommandComplete ();
    }
    BreakInGroupedCommands ();
}

void TextInteractor::OnCopyCommand ()
{
    size_t start = GetSelectionStart ();
    size_t end   = GetSelectionEnd ();
    Assert (start <= end);
    if (start < end) {
        BreakInGroupedCommands ();

        if (OnCopyCommand_Before ()) {
            try {
                OnCopyCommand_CopyFlavors ();
            }
            catch (...) {
                OnCopyCommand_After ();
                throw;
            }
            OnCopyCommand_After ();
        }
    }
}

void TextInteractor::OnPasteCommand ()
{
    InteractiveModeUpdater iuMode (*this);
    BreakInGroupedCommands ();

    if (OnPasteCommand_Before ()) {
        try {
            UndoableContextHelper undoContext (*this, GetCommandNames ().fPasteCommandName, false);
            {
                OnPasteCommand_PasteBestFlavor ();
            }
            undoContext.CommandComplete ();
        }
        catch (...) {
            OnPasteCommand_After ();
            throw;
        }
        OnPasteCommand_After ();
    }
    BreakInGroupedCommands ();
}

void TextInteractor::OnClearCommand ()
{
    InteractiveModeUpdater iuMode (*this);
    BreakInGroupedCommands ();
    UndoableContextHelper undoContext (*this, GetCommandNames ().fClearCommandName, true);
    {
        InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0);
    }
    undoContext.CommandComplete ();
    BreakInGroupedCommands ();
}

/*
@METHOD:        TextInteractor::OnCopyCommand_Before
@ACCESS:        protected
@DESCRIPTION:   <p>Hook overriden by SDK-specific classes or templates which does special things in the before
            a clipboard <em>copy</em> operation can begin (like opening a clipboard object). Generally should not be
            called directly or overridden, except when implementing new SDK wrappers.</p>
                <p>Return false or throw if fail</p>
                <p>See also @'TextInteractor::OnCopyCommand_After',
                @'TextInteractor::OnPasteCommand_Before',
                and
                @'TextInteractor::OnPasteCommand_After'
                .</p>
 */
bool TextInteractor::OnCopyCommand_Before ()
{
    return true;
}

/*
@METHOD:        TextInteractor::OnCopyCommand_After
@ACCESS:        protected
@DESCRIPTION:   <p>See also @'TextInteractor::OnCopyCommand_Before'.</p>
 */
void TextInteractor::OnCopyCommand_After ()
{
}

/*
@METHOD:        TextInteractor::OnCopyCommand_CopyFlavors
@ACCESS:        protected
@DESCRIPTION:   <p></p>
 */
void TextInteractor::OnCopyCommand_CopyFlavors ()
{
    WriterClipboardFlavorPackage writer;
    ExternalizeFlavors (writer);
}

bool TextInteractor::ShouldEnablePasteCommand () const
{
    return (Led_ClipboardObjectAcquire::FormatAvailable_TEXT ());
}

/*
@METHOD:        TextInteractor::OnPasteCommand_Before
@DESCRIPTION:   <p>Hook overriden by SDK-specific classes or templates which does special things in the before
            a clipboard <em>paste</em> operation can begin (like opening a clipboard object). Generally should not be
            called directly or overridden, except when implementing new SDK wrappers.</p>
                <p>Return false or throw if fail</p>
                <p>See also @'TextInteractor::OnPasteCommand_After',
                @'TextInteractor::OnCopyCommand_Before',
                and
                @'TextInteractor::OnCopyCommand_After'
                .</p>
 */
bool TextInteractor::OnPasteCommand_Before ()
{
    return true;
}

/*
@METHOD:        TextInteractor::OnPasteCommand_After
@ACCESS:        protected
@DESCRIPTION:   <p>See also @'TextInteractor::OnPasteCommand_Before'.</p>
 */
void TextInteractor::OnPasteCommand_After ()
{
}

/*
@METHOD:        TextInteractor::OnPasteCommand_PasteBestFlavor
@ACCESS:        protected
@DESCRIPTION:   <p></p>
 */
void TextInteractor::OnPasteCommand_PasteBestFlavor ()
{
#if qPlatform_Windows && 0
    // A little debugging hack for windows - sometimes helpful to turn this on
    // to peek in the debugger at what is on the clipboard - LGP 960430

    long clipFormat = 0;
    while ((clipFormat = ::EnumClipboardFormats (clipFormat)) != 0) {
        TCHAR buf[1024];
        int   nChars    = ::GetClipboardFormatName (clipFormat, buf, NEltsOf (buf));
        int   breakHere = 0;
    }
#endif

    ReaderClipboardFlavorPackage clipData;

    SmartCNPInfo smartCNPInfo;
    bool         doSmartCNP    = PasteLooksLikeSmartCNP (&smartCNPInfo);
    size_t       savedSelStart = GetSelectionStart (); // save cuz InternalizeBestFlavor () will tend to adjust selStart
    InternalizeBestFlavor (clipData);
    Assert (savedSelStart <= GetSelectionStart ()); //  InternalizeBestFlavor can only adjust it FORWARD - not backward...
    if (doSmartCNP) {
        OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (savedSelStart, smartCNPInfo);
    }
}

/*
@METHOD:        TextInteractor::OnPasteCommand_PasteFlavor_Specific
@ACCESS:        protected
@DESCRIPTION:   <p></p>
 */
void TextInteractor::OnPasteCommand_PasteFlavor_Specific (Led_ClipFormat format)
{
    ReaderClipboardFlavorPackage clipData;
    SmartCNPInfo                 smartCNPInfo;
    bool                         doSmartCNP    = PasteLooksLikeSmartCNP (&smartCNPInfo);
    size_t                       savedSelStart = GetSelectionStart (); // save cuz InternalizeBestFlavor () will tend to adjust selStart
    InternalizeFlavor_Specific (clipData, format);
    Assert (savedSelStart <= GetSelectionStart ()); //  InternalizeBestFlavor can only adjust it FORWARD - not backward...
    if (doSmartCNP) {
        OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (savedSelStart, smartCNPInfo);
    }
}

/*
@METHOD:        TextInteractor::PasteLooksLikeSmartCNP
@ACCESS:        protected
@DESCRIPTION:   <p></p>
 */
bool TextInteractor::PasteLooksLikeSmartCNP (SmartCNPInfo* scnpInfo) const
{
    RequireNotNull (scnpInfo);
    ReaderClipboardFlavorPackage clipData;
    bool                         doSmartCNP = GetSmartCutAndPasteMode () and clipData.GetFlavorAvailable_TEXT ();
    if (doSmartCNP) {
        /*
         *  Check if this REALLY looks like a good opportunity todo a smart-cut-and-paste whitespace adjustment.
         */
        size_t         length     = clipData.GetFlavorSize (kTEXTClipFormat);
        Led_ClipFormat textFormat = kTEXTClipFormat;

        Memory::SmallStackBuffer<char> buf (length); // could use bufsize=(len+1)/sizeof (Led_tChar)
        length = clipData.ReadFlavorData (textFormat, length, buf);
        if (doSmartCNP) {
            Led_tChar* buffp   = reinterpret_cast<Led_tChar*> (static_cast<char*> (buf));
            size_t     nTChars = length / sizeof (Led_tChar);
            doSmartCNP         = LooksLikeSmartPastableText (buffp, nTChars, scnpInfo);
        }
    }
    return doSmartCNP;
}

/*
@METHOD:        TextInteractor::OnSelectAllCommand
@DESCRIPTION:   <p>Command to implement the "Select All" UI. Trivial implementation, but nearly all UI's want it, so why
            write it each time?</p>
*/
void TextInteractor::OnSelectAllCommand ()
{
    SetSelection (0, GetLength ());
}

bool TextInteractor::CanAcceptFlavor (Led_ClipFormat clipFormat) const
{
    return (kTEXTClipFormat == clipFormat or kFILEClipFormat == clipFormat);
}

void TextInteractor::InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
                                            bool updateCursorPosition, bool autoScroll, UpdateMode updateMode)
{
    size_t start = GetSelectionStart ();
    size_t end   = GetSelectionEnd ();

    bool good = false;
    {
        TempMarker newSel (GetTextStore (), end + 1, end + 1); // NB: This marker pos MAY split a multibyte character - but that should cause
        // no problems.
        // We are only keeping this temporarily and we subtract one at the
        // end. Just want to make it past point where we do the insertion
        // so we get the right adjustment on
        good = fInternalizer->InternalizeBestFlavor (flavorPackage, start, end);
        if (good and updateCursorPosition) {
            SetCaretShownAfterPos (true); // by default show pos after - this is what works best for NL's and is what we
            // want most of the time...

            // We placed a marker one past the end of the selection. Then we replaced the given selection.
            // So the marker now points one past where it really should. Backward adjust it, and set the selection there.
            size_t newSelection = newSel.GetStart ();
            if (newSelection > start) {
                newSelection--;
            }

            SetSelection (newSelection, newSelection);
        }
    }

    if (good) {
        if (autoScroll) {
            ScrollToSelection ();
        }
        if (updateMode == eImmediateUpdate) {
            Update ();
        }
    }
    else {
        OnBadUserInput ();
    }
}

/*
@METHOD:        TextInteractor::InternalizeFlavor_Specific
@DESCRIPTION:   <p></p>
 */
void TextInteractor::InternalizeFlavor_Specific (ReaderFlavorPackage& flavorPackage, Led_ClipFormat format,
                                                 bool updateCursorPosition, bool autoScroll, UpdateMode updateMode)
{
    size_t start = GetSelectionStart ();
    size_t end   = GetSelectionEnd ();

    bool good = false;
    {
        TempMarker newSel (GetTextStore (), end + 1, end + 1); // NB: This marker pos MAY split a multibyte character - but that should cause
        // no problems.
        // We are only keeping this temporarily and we subtract one at the
        // end. Just want to make it past point where we do the insertion
        // so we get the right adjustment on

        if (format == kTEXTClipFormat) {
            good = fInternalizer->InternalizeFlavor_TEXT (flavorPackage, start, end);
        }
        else if (format == kFILEClipFormat) {
            good = fInternalizer->InternalizeFlavor_FILE (flavorPackage, start, end);
        }
        else {
            good = fInternalizer->InternalizeBestFlavor (flavorPackage, start, end);
        }

        if (good and updateCursorPosition) {
            SetCaretShownAfterPos (true); // by default show pos after - this is what works best for NL's and is what we
            // want most of the time...

            // We placed a marker one past the end of the selection. Then we replaced the given selection.
            // So the marker now points one past where it really should. Backward adjust it, and set the selection there.
            size_t newSelection = newSel.GetStart ();
            if (newSelection > start) {
                newSelection--;
            }

            SetSelection (newSelection, newSelection);
        }
    }

    if (good) {
        if (autoScroll) {
            ScrollToSelection ();
        }
        if (updateMode == eImmediateUpdate) {
            Update ();
        }
    }
    else {
        OnBadUserInput ();
    }
}

/*
@METHOD:        TextInteractor::MakeDefaultInternalizer
@DESCRIPTION:   <p>Make a @'FlavorPackageInternalizer' which is appropriate for this text interactor. Override this
    to make a different subclass, which supports different style and file formats from the (simple) default.</p>
        <p>By default, this creates a @'FlavorPackageInternalizer'.</p>
*/
shared_ptr<FlavorPackageInternalizer> TextInteractor::MakeDefaultInternalizer ()
{
    return make_shared<FlavorPackageInternalizer> (GetTextStore ());
}

/*
@METHOD:        TextInteractor::HookInternalizerChanged
@DESCRIPTION:   <p>Called by @'TextInteractor::SetInternalizer' whenever there is a new internalizer specified.</p>
*/
void TextInteractor::HookInternalizerChanged ()
{
}

/*
@METHOD:        TextInteractor::ExternalizeFlavors
@DESCRIPTION:   <p>Use the associated externalizer (see @'TextInteractor::SetExternalizer')  to call
            ExternalizeFlavors applied to the current selection.</p>
*/
void TextInteractor::ExternalizeFlavors (WriterFlavorPackage& flavorPackage)
{
    fExternalizer->ExternalizeFlavors (flavorPackage, GetSelectionStart (), GetSelectionEnd ());
}

/*
@METHOD:        TextInteractor::ExternalizeBestFlavor
@DESCRIPTION:   <p>Use the associated externalizer (see @'TextInteractor::SetExternalizer')  to call
            ExternalizeBestFlavor applied to the current selection.</p>
*/
void TextInteractor::ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage)
{
    fExternalizer->ExternalizeBestFlavor (flavorPackage, GetSelectionStart (), GetSelectionEnd ());
}

/*
@METHOD:        TextInteractor::MakeDefaultExternalizer
@DESCRIPTION:   <p>Make a @'FlavorPackageExternalize' which is appropriate for this text interactor. Override this
    to make a different subclass, which supports different style and file formats from the (simple) default.</p>
        <p>By default, this creates a @'FlavorPackageExternalizer'.</p>
*/
shared_ptr<FlavorPackageExternalizer> TextInteractor::MakeDefaultExternalizer ()
{
    return make_shared<FlavorPackageExternalizer> (GetTextStore ());
}

/*
@METHOD:        TextInteractor::HookExternalizerChanged
@DESCRIPTION:   <p>Called by @'TextInteractor::SetExternalizer' whenever there is a new externalizer specified.</p>
*/
void TextInteractor::HookExternalizerChanged ()
{
}

/*
@METHOD:        TextInteractor::OnBadUserInput
@DESCRIPTION:   <p>By default this throws @'TextInteractor::BadUserInput' but it can be overriden to
        NOT throw anything, and just beep or something. BEWARE then when calling this that it may or
        may not throw, and may or may not return.</p>
*/
void TextInteractor::OnBadUserInput ()
{
    // you may want to OVERRIDE this to do a staged alert, or to throw an exception???
    //Led_BeepNotify ();
    throw BadUserInput (); // default catcher should call Led_BeepNotify ()
}

/*
@METHOD:        TextInteractor::SetScrollBarType
@DESCRIPTION:   <p>Specify whether or not the interactor will display / manage scrollbars. This really is handled in OS/ClassLib specific subclasses.
            But the API is here to keep it uniform across the platforms.</p>
                <p>The default settings for each direction (v/h) are 'TextInteractor::eScrollBarNever'
            (except than for the Windows platform, @'Led_Win32_Helper<BASE_INTERACTOR>::OnCreate_Msg'
            and @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnCreate' will set the initial value according to
            the windows style passed into the WM_CREATE message).</p>
                <p>On windows I recall that it USED to be standard that an edit text would suddenly
            spawn a scrollbar when needed. This doesn't appear to be the case any longer.
            I always hated this behavior, but since I didn't know better, I implemented it.</p>
                <p>Even though this behavior is no longer particularly standard or common, it is sometimes
            desired, and so supported.</p>
                <p>NB: You need not turn on the WS_V/HSCROLL styles to make the scrollbars appear/disapear.
            When this is on, it is handled automagically.</p>
                <p>NB: This USED to be controlled by a compile-time variable @'qMakeWindowsScrollbarAppearOnlyIfNeeded',
            which is now obsolete.</p>
                <p>See also 'TextInteractor::GetScrollBarType' and @'TextInteractor::InvalidateScrollBarParameters'.</p>
*/
void TextInteractor::SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType)
{
    if (GetScrollBarType (vh) != scrollBarType) {
        InvalidateScrollBarParameters ();
        SetScrollBarType_ (vh, scrollBarType);
    }
}

/*
@METHOD:        TextInteractor::InvalidateScrollBarParameters
@DESCRIPTION:   <p>Mark the contents of the scrollbars as invalid. Someone, sometime later soon will call
    @'TextInteractor::UpdateScrollBars' to fix them up again.</p>
*/
void TextInteractor::InvalidateScrollBarParameters ()
{
    InvalidateScrollBarParameters_ ();
}

/*
@METHOD:        TextInteractor::UpdateScrollBars
@DESCRIPTION:   <p>Override this to handle any update of the scrollbars you may need to. Something has happened to
    invalidate what they now display (new text added, scrolled, or whatever - someone called @'TextInteractor::InvalidateScrollBarParameters').
    This is usually taken care of in any class library wrapper code, such as @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
    or @'Led_Win32_Helper<BASE_INTERACTOR>'.</p>
*/
void TextInteractor::UpdateScrollBars ()
{
    UpdateScrollBars_ ();
}

/*
@METHOD:        TextInteractor::SetCaretShown
@DESCRIPTION:   <p>See also @'TextInteractor::GetCaretShownSituation' and @'TextInteractor::GetCaretShown'.</p>
*/
void TextInteractor::SetCaretShown (bool shown)
{
    if (GetCaretShown () != shown) {
        fCaretShown = shown;
        InvalidateCaretState ();
#if qPlatform_MacOS
        // On the mac - when it was shown, and now is not - we MAY need to force an update to get it erased - and when not shown- the
        // InvalidateCaretState () method doesn't force an update.
        if (not shown) {
            RefreshWindowRect (CalculateCaretRect ());
        }
#endif
    }
}

/*
@METHOD:        TextInteractor::GetCaretShownSituation
@DESCRIPTION:   <p>To decide if its appropriate for the editor to display a blinking caret, we check an
            overall state variable @'TextInteractor::GetCaretShown' which is logically tied to
            whether or not the edit widget has the focus. But - thats not all that needs to be
            considered to decide if you should display the caret. You also - typically - need
            to have an empty selection. This virtual method does that particular check.</p>
*/
bool TextInteractor::GetCaretShownSituation () const
{
    size_t selStart = 0;
    size_t selEnd   = 0;
    GetSelection (&selStart, &selEnd);
    return (selStart == selEnd);
}

/*
@METHOD:        TextInteractor::SetCaretShownAfterPos
@DESCRIPTION:   <p>See @'TextInteractor::GetCaretShownAfterPos' for meaning of this flag.</p>
 */
void TextInteractor::SetCaretShownAfterPos (bool shownAfterPos)
{
    if (GetCaretShownAfterPos () != shownAfterPos) {
        InvalidateCaretState (); // before and after so we draw/erase in new and old places
        fCaretShownAfterPos = shownAfterPos;
        InvalidateCaretState ();
    }
}

/*
@METHOD:        TextInteractor::CalculateCaretRect
@ACCESS:        protected
@DESCRIPTION:   <p>This is complicated due to worrying about bidirectional editing, and due to the fact
            that two adjacent characters in logical order, maybe in very different parts of the screen
            (e.g. the marker position between the end of a row and the start of the next row maybe identical, or
            the marker position between one character and another WITHIN a row at a directional boundary may
            indicate two very different caret locations).</p>
                <p>The first issue to resolve is - <em>which character</em> to we care most about - the one <em>preceeding</em>
            the marker position or the one <em>following</em>. This question is arbitrated by the <em>@'TextInteractor::GetCaretShownAfterPos'</em>
            API.</p>
                <p>The second question - as to which side of the character to display the caret - is decided
            by the text direction. But WHICH text direction? The one of the preceeding or following character? Easy - we use
            the same choice as chosen by the first question.
                </p>
 */
Led_Rect TextInteractor::CalculateCaretRect () const
{
    size_t selEnd = GetSelectionEnd ();
    if (GetSelectionStart () == selEnd) {
        bool showAfter = GetCaretShownAfterPos ();
        if (selEnd == 0) {
            showAfter = true;
        }

        size_t        charAfterPos  = showAfter ? selEnd : FindPreviousCharacter (selEnd);
        TextDirection textDirection = GetTextDirection (charAfterPos);
        Led_Rect      caretRect     = GetCharWindowLocation (charAfterPos);

        if (caretRect.GetBottom () < GetWindowRect ().GetTop () or caretRect.GetTop () > GetWindowRect ().GetBottom ()) {
            return (Led_Rect (0, 0, 0, 0));
        }

        Led_Rect        origCaretRect = caretRect;
        Led_FontMetrics fontMetrics   = GetFontMetricsAt (charAfterPos);

        // WE NEED THE WHOLE ROW BASELINE!!! THEN CAN COMPUTE caretrect from that!!!
        Led_Distance   baseLineFromTop = GetRowRelativeBaselineOfRowContainingPosition (charAfterPos);
        Led_Coordinate realBaseLine    = baseLineFromTop + caretRect.top;

        // now adjust caretrect to be font-metrics from the base line
        caretRect.top    = realBaseLine - fontMetrics.GetAscent ();
        caretRect.bottom = realBaseLine + fontMetrics.GetDescent ();

        // Before pinning CaretRect to full rowRect, first try to adjust it so its inside.
        // It can come outside the rowRect if the baseLineFromTop is zero - for example - when
        // the preceeding text is hidden.
        if (caretRect.top < origCaretRect.top) {
            Led_Distance diff = origCaretRect.GetTop () - caretRect.GetTop ();
            caretRect += Led_Point (diff, 0);
        }

        // pin CaretRect to full rowRect
        caretRect.SetTop (max (caretRect.GetTop (), origCaretRect.GetTop ()));
        caretRect.bottom = min (caretRect.GetBottom (), origCaretRect.GetBottom ());

        if (textDirection == eLeftToRight) {
            if (not showAfter) {
                caretRect.left = caretRect.right;
            }
        }
        else {
            caretRect.left = caretRect.right;
        }

        const Led_Coordinate kCaretWidth = 1;
        // quickie hack to be sure caret doesn't go off right side of window!!!
        const Led_Coordinate kSluff = kCaretWidth + 1;
        if (caretRect.GetLeft () + kSluff > GetWindowRect ().GetRight ()) {
            caretRect.SetLeft (GetWindowRect ().GetRight () - kSluff);
        }
        caretRect.SetRight (caretRect.GetLeft () + kCaretWidth);

        Ensure (not caretRect.IsEmpty ());
        return (caretRect);
    }
    else {
        return (Led_Rect (0, 0, 0, 0));
    }
}

void TextInteractor::InvalidateCaretState ()
{
    if (IsWholeWindowInvalid ()) {
        return;
    }
    if (GetCaretShown () and (GetSelectionStart () == GetSelectionEnd ())) {
        RefreshWindowRect (CalculateCaretRect ());
    }
}

/*
@METHOD:        TextInteractor::OnTypedNormalCharacter
@DESCRIPTION:   <p>High level handling that does basically all of the portable support for a typed character.
            This is typically what you would call (the class library integration classes call this directly) to handle
            keyboard input. You might also plausibly OVERRIDE this method to provide special handling for
            particular key sequences (e.g. LedLineIt! overrides this to map shift-tab when there is a selection
            to an indent command rather than an insertion of text).</p>
 */
void TextInteractor::OnTypedNormalCharacter (Led_tChar theChar, bool /*optionPressed*/, bool /*shiftPressed*/, bool /*commandPressed*/, bool controlPressed, bool /*altKeyPressed*/)
{
    IdleManager::NonIdleContext nonIdleContext;

#if qMultiByteCharacters
    if (HandledMByteCharTyping (theChar)) {
        return;
    }
#endif

    Assert (GetSelectionEnd () <= GetLength () + 1);

    if (GetSuppressTypedControlCharacters ()) {
        bool controlChar = Character (theChar).IsControl ();
        if (controlChar &&
            (theChar == '\r' || theChar == '\n' || theChar == ' ' || theChar == '\t' || theChar == '\b')) {
            controlChar = false;
        }
        if (controlChar) {
            OnBadUserInput ();
            return;
        }
    }

    switch (theChar) {
        case '\b': {
            CursorMovementDirection dir    = eCursorBack;
            CursorMovementUnit      unit   = controlPressed ? eCursorByWord : eCursorByChar;
            CursorMovementAction    action = eCursorDestroying;
            DoSingleCharCursorEdit (dir, unit, action, eDefaultUpdate);
        } break;

        default: {
            if (theChar == '\n') {
                BreakInGroupedCommands ();
            }
            InteractiveReplace (&theChar, 1, eDefaultUpdate);
        } break;
    }

    ScrollToSelection ();
#if qPeekForMoreCharsOnUserTyping
    UpdateIfNoKeysPending ();
#else
    Update ();
#endif
}

#if qMultiByteCharacters
bool TextInteractor::HandledMByteCharTyping (char theChar)
{
    if (fMultiByteInputCharBuf[0] == '\0' and Led_IsLeadByte (theChar)) {
        /*
         *  If we get a first-byte - then don't process it yet. Just save it up for the
         *  next call.
         */
        fMultiByteInputCharBuf[0] = theChar;
        fMultiByteInputCharBuf[1] = '\0';
        return true; // done with processing the character...
    }
    else if (fMultiByteInputCharBuf[0] != '\0') {
        /*
         *  If we have a PENDING first-byte - then append this to our buffer, and pretend
         *  the user typed these two bytes. However - if we get a BAD second byte -
         *  then call BadUserInput() (basicly does a sysbeep) and drop it on the
         *  floor.
         */
        fMultiByteInputCharBuf[1] = theChar; // set it even if its bad so OnBadUserInput can peek()
        if (ValidateTextForCharsetConformance (fMultiByteInputCharBuf, 2)) {
            InteractiveReplace (fMultiByteInputCharBuf, 2);
            fMultiByteInputCharBuf[0] = '\0';
            size_t newSelection       = FindNextCharacter (GetSelectionStart ());
            SetSelection (newSelection, newSelection);
            ScrollToSelection ();
#if qPeekForMoreCharsOnUserTyping
            UpdateIfNoKeysPending ();
#else
            Update ();
#endif
        }
        else {
            OnBadUserInput ();
            fMultiByteInputCharBuf[0] = '\0';
        }
        return true; // done with processing the character...
    }
    return false; // We did nothing - handle character as usual
}
#endif

#if qPlatform_MacOS || qStroika_FeatureSupported_XWindows
float TextInteractor::GetTickCountBetweenBlinks ()
{
#if qPlatform_MacOS
    return (::GetCaretTime () / 60.0);
#elif qStroika_FeatureSupported_XWindows
    return (0.4f);
#endif
}
#endif

bool TextInteractor::DelaySomeForScrollBarClick ()
{
    const Time::DurationSecondsType kDelayAfterFirstTicks = 0.20f; // maybe should use ::GetDblClickTime()???
    const Time::DurationSecondsType kDelayAfterOtherTicks = 0.02f; // This delay is so on really fast computers, text doesn't scroll too quickly
    const int                       kTimesForFirstClick   = 2;
    const Time::DurationSecondsType kLongTime             = 1.0f; // any click after this time deemed we start again with first-tick
    static short                    sTimesThruBeforeReset;

    Foundation::Time::DurationSecondsType now = Time::GetTickCount ();
    if (fLastScrolledAt == 0 or fLastScrolledAt + kLongTime < now) {
        fLastScrolledAt       = now + kDelayAfterFirstTicks;
        sTimesThruBeforeReset = 1;
        return true; // first time through - handle click immediately
    }
    else if (fLastScrolledAt < now) {
        sTimesThruBeforeReset++;
        fLastScrolledAt = now + (sTimesThruBeforeReset <= kTimesForFirstClick ? kDelayAfterFirstTicks : kDelayAfterOtherTicks);
        return true; // enuf time has elapsed
    }
    else {
        return false; // not enough time has elapsed
    }
}
