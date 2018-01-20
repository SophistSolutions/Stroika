/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "../../Foundation/Traversal/Iterator.h"

#include "StyledTextEmbeddedObjects.h"

#include "StyledTextIO/StyledTextIO_HTML.h"
#include "StyledTextIO/StyledTextIO_RTF.h"

#include "StandardStyledTextInteractor.h"

#if qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
#include "StyledTextIO/Led_StyledTextIO_LedNative.h"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::StyledTextIO;

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(disable : 4786) //qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif

#if qPlatform_MacOS
const Led_ClipFormat Led::kLedPrivateClipFormat = 'LedP';
const Led_ClipFormat Led::kRTFClipFormat        = 'RTF ';
const Led_ClipFormat Led::kHTMLClipFormat       = 'HTML';
#elif qPlatform_Windows
const TCHAR                                kLedPrivateClipTypeName[]                   = _T ("Led Rich Text Format");
const Led_ClipFormat                       Led::kLedPrivateClipFormat                  = ::RegisterClipboardFormat (kLedPrivateClipTypeName);
const TCHAR                                kRTFClipTypeName[]                          = _T ("Rich Text Format");
const Led_ClipFormat                       Led::kRTFClipFormat                         = ::RegisterClipboardFormat (kRTFClipTypeName);
const TCHAR                                kHTMLClipTypeName[]                         = _T ("HTML"); /// MAYBE A BAD NAME - SEE IF ANY WINDOWS STANDARD NAME???
const Led_ClipFormat                       Led::kHTMLClipFormat                        = ::RegisterClipboardFormat (kHTMLClipTypeName);
#elif qXWindows
// Toolkit-specific code (e.g. Led_Gtk<>) must reset these to good values. Cannot be constants
// and cannot be filled in here, cuz we require a DISPLAY object to register the contants on.
Led_ClipFormat kLedPrivateClipFormat = 0;
Led_ClipFormat kRTFClipFormat        = 0;
Led_ClipFormat kHTMLClipFormat       = 0;
#endif

/*
 ********************************************************************************
 *************************** StandardStyledTextInteractor ***********************
 ********************************************************************************
 */
#if qRuntimeCrashMaybeCodeGenBugWithAppStartupBug
StandardStyledTextInteractor::CommandNames& StandardStyledTextInteractor::sCommandNames ()
{
    static CommandNames commandNames = MakeDefaultCommandNames ();
    return commandNames;
}
#else
StandardStyledTextInteractor::CommandNames StandardStyledTextInteractor::sCommandNames = StandardStyledTextInteractor::MakeDefaultCommandNames ();
#endif

StandardStyledTextInteractor::StandardStyledTextInteractor ()
    : TextInteractor ()
    , StandardStyledTextImager ()
    , fEmptySelectionStyleSuppressMode (false)
    , fEmptySelectionStyle (GetStaticDefaultFont ())
{
}

StandardStyledTextInteractor::~StandardStyledTextInteractor ()
{
}

StandardStyledTextInteractor::CommandNames StandardStyledTextInteractor::MakeDefaultCommandNames ()
{
    StandardStyledTextInteractor::CommandNames cmdNames;
    cmdNames.fFontChangeCommandName = Led_SDK_TCHAROF ("Font Change");
    return cmdNames;
}

void StandardStyledTextInteractor::HookLosingTextStore ()
{
    TextInteractor::HookLosingTextStore ();
    StandardStyledTextImager::HookLosingTextStore ();
    HookLosingTextStore_ ();
}

void StandardStyledTextInteractor::HookLosingTextStore_ ()
{
    // Remove all embeddings...
    vector<SimpleEmbeddedObjectStyleMarker*> embeddings = CollectAllEmbeddingMarkersInRange (0, GetLength ());
    for (size_t i = 0; i < embeddings.size (); i++) {
        SimpleEmbeddedObjectStyleMarker* e = embeddings[i];
        AssertNotNull (e); // all embeddings returned must be non-null
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(suppress : 28182)
#endif
        if (e->GetOwner () == this) {
            PeekAtTextStore ()->RemoveMarker (e);
            delete e;
        }
    }
}

void StandardStyledTextInteractor::HookGainedNewTextStore ()
{
    TextInteractor::HookGainedNewTextStore ();
    StandardStyledTextImager::HookGainedNewTextStore ();
    HookGainedNewTextStore_ ();
}

void StandardStyledTextInteractor::HookGainedNewTextStore_ ()
{
}

/*
@METHOD:        StandardStyledTextInteractor::SetDefaultFont
@DESCRIPTION:   <p>Override @'TextImager::SetDefaultFont' to provide a moderately sensible interpretation
            of that method.</p>
                <p>Note that this definition is significantly different than in Led 3.0. To get
            the Led 3.0 behavior, you should call InteractiveSetFont ()</p>
 */
void StandardStyledTextInteractor::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont)
{
    size_t selStart = GetSelectionStart ();
    size_t selEnd   = GetSelectionEnd ();
    if (selStart == selEnd) {
        size_t                selLength              = selEnd - selStart;
        Led_FontSpecification newEmptySelectionStyle = fEmptySelectionStyle;
        newEmptySelectionStyle.MergeIn (defaultFont);
        SetStyleInfo (selStart, selLength, defaultFont);
        SetEmptySelectionStyle (newEmptySelectionStyle);
        Refresh ();
    }
    StandardStyledTextImager::SetDefaultFont (defaultFont);
}

/*
@METHOD:        StandardStyledTextInteractor::InteractiveSetFont
@DESCRIPTION:   <p>Applies the given incremental font
            specification to the current selection. If the selection is empty - it stores the specification in
            a special variable used to represent the font of the empty selection. Then when the user types, the
            newly typed characters are in that font.</p>
                <p>This function is meant to implement the underlying semantics of the standard word-processor
            font/style selection menu.</p>
                <p>As such, it is also entered by name in the command UNDO list. If you do <em>not</em> want your
            font changes treated that way, use @'StandardStyledTextImager::SetStyleInfo'.</p>
                <p>Note that this functionality USED to be provided by @'StandardStyledTextInteractor::SetDefaultFont'
            (before Led 3.1a4) - but that is now obsolete. @'TextImager::SetDefaultFont' just sets a default font
            object associated with the imager, and that has little or no effect when used with this class.
 */
void StandardStyledTextInteractor::InteractiveSetFont (const Led_IncrementalFontSpecification& defaultFont)
{
    InteractiveModeUpdater iuMode (*this);
    RequireNotNull (PeekAtTextStore ()); // Must specify TextStore before calling this, or any routine that calls it.

    BreakInGroupedCommands ();

    UndoableContextHelper undoContext (*this, GetCommandNames ().fFontChangeCommandName, false);
    {
        /*
         *  NB: The SetStyleInfo() call will notify markers via AboutTo/DidUpdate, so long as that code
         *  percieves there is any change. But if the selection is empty, no style runs will change!
         *
         *  Similarly, if there is no selection, SetEmptySelectionStyle () will take care of the notification
         *  of change.
         */
        size_t                selLength              = undoContext.GetUndoRegionEnd () - undoContext.GetUndoRegionStart ();
        Led_FontSpecification newEmptySelectionStyle = fEmptySelectionStyle;
        newEmptySelectionStyle.MergeIn (defaultFont);
        SetStyleInfo (undoContext.GetUndoRegionStart (), selLength, defaultFont);
        SetEmptySelectionStyle (newEmptySelectionStyle);
        Refresh ();
    }
    undoContext.CommandComplete ();
}

Led_IncrementalFontSpecification StandardStyledTextInteractor::GetContinuousStyleInfo (size_t from, size_t nTChars) const
{
    if (nTChars == 0 and from == GetSelectionStart ()) {
        vector<InfoSummaryRecord> summaryInfo;
        summaryInfo.push_back (InfoSummaryRecord (fEmptySelectionStyle, 0));
        return (GetContinuousStyleInfo_ (summaryInfo));
    }
    else {
        return StandardStyledTextImager::GetContinuousStyleInfo (from, nTChars);
    }
}

void StandardStyledTextInteractor::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    TextInteractor::DidUpdateText (updateInfo);
    StandardStyledTextImager::DidUpdateText (updateInfo);
    /*
     *  SPR#1171 - note that we cannot call SetEmptySelectionStyle () here because it is TOO aggressive.
     *  changes occur all the time (including setting the font in an empty selection to italics or something
     *  which trigger a DidUpdate call. Easier and safer to just do the SetEmptySelectionStyle () in the few
     *  places where I notice its needed.
     */
}

bool StandardStyledTextInteractor::ShouldEnablePasteCommand () const
{
    if (TextInteractor::ShouldEnablePasteCommand ()) {
        return true;
    }
    if (Led_ClipboardObjectAcquire::FormatAvailable (kLedPrivateClipFormat)) {
        return true;
    }
    if (Led_ClipboardObjectAcquire::FormatAvailable (kRTFClipFormat)) {
        return true;
    }

    const vector<EmbeddedObjectCreatorRegistry::Assoc>& types = EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
    for (size_t i = 0; i < types.size (); i++) {
        EmbeddedObjectCreatorRegistry::Assoc assoc           = types[i];
        bool                                 clipAvailForAll = true;
        for (size_t j = 0; j < assoc.fFormatTagCount; j++) {
            if (not Led_ClipboardObjectAcquire::FormatAvailable (assoc.GetIthFormat (j))) {
                clipAvailForAll = false;
                break;
            }
        }
        if (clipAvailForAll) {
            return true;
        }
    }
    return false;
}

bool StandardStyledTextInteractor::CanAcceptFlavor (Led_ClipFormat clipFormat) const
{
    if (TextInteractor::CanAcceptFlavor (clipFormat)) {
        return true;
    }
    if (clipFormat == kLedPrivateClipFormat) {
        return true;
    }
    if (clipFormat == kRTFClipFormat) {
        return true;
    }

    const vector<EmbeddedObjectCreatorRegistry::Assoc>& types = EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
    for (size_t i = 0; i < types.size (); i++) {
        EmbeddedObjectCreatorRegistry::Assoc assoc = types[i];

        // This may sometimes false-posative - since we may (in priciple) require several other formats at the
        // same time. But this will at least return true whenever we CAN accept the format...
        // Maybe we should redesign/reimplement this API for a future release? LGP 960416
        for (size_t j = 0; j < assoc.fFormatTagCount; j++) {
            if (assoc.GetIthFormat (j) == clipFormat) {
                return true;
            }
        }
    }
    return false;
}

void StandardStyledTextInteractor::HookStyleDatabaseChanged ()
{
    StandardStyledTextImager::HookStyleDatabaseChanged ();
    if (PeekAtTextStore () != nullptr) {
        // if no TextStore - no problem - TextInteractor::HookGainedTextStore () will recreate these.
        SetExternalizer (MakeDefaultExternalizer ());
        SetInternalizer (MakeDefaultInternalizer ());
    }
}

shared_ptr<FlavorPackageInternalizer> StandardStyledTextInteractor::MakeDefaultInternalizer ()
{
    return make_shared<StyledTextFlavorPackageInternalizer> (GetTextStore (), GetStyleDatabase ());
}

shared_ptr<FlavorPackageExternalizer> StandardStyledTextInteractor::MakeDefaultExternalizer ()
{
    return make_shared<StyledTextFlavorPackageExternalizer> (GetTextStore (), GetStyleDatabase ());
}

/*
@METHOD:        StandardStyledTextInteractor::ProcessSimpleClick
@ACCESS:        protected
@DESCRIPTION:   <p>Override @'TextInteractor::ProcessSimpleClick' to handle embeddings.</p>
*/
bool StandardStyledTextInteractor::ProcessSimpleClick (Led_Point clickedAt, unsigned clickCount, bool extendSelection, size_t* dragAnchor)
{
    RequireNotNull (dragAnchor);
    size_t   clickedOnChar = GetCharAtWindowLocation (clickedAt);
    Led_Rect charRect      = GetCharWindowLocation (clickedOnChar);

    // Only if click is on an embedding character cell, and fully within it (not in case just past it as at when at
    // end of line) - then we look at if it needs special processing
    //
    // Actually - better to check that the click isn't too near the edges of the embedding,
    // cuz then its hard to click and make an insertion point in between two embeddings.
    // So only do this click-selects somewhere near the middle of the embedding.
    Led_Rect           tstClickRect = charRect;
    const Led_Distance kHMargin     = 3;
    tstClickRect.left += kHMargin;
    tstClickRect.right -= kHMargin;

    if (tstClickRect.Contains (clickedAt)) {
        vector<SimpleEmbeddedObjectStyleMarker*> embeddingList = CollectAllEmbeddingMarkersInRange (clickedOnChar, clickedOnChar + 1);
        Assert (embeddingList.size () == 0 or embeddingList.size () == 1);

        if (embeddingList.size () == 1) {
            SimpleEmbeddedObjectStyleMarker* embedding = embeddingList[0];
            AssertMember (embedding, SimpleEmbeddedObjectStyleMarker);
            switch (clickCount) {
                case 1: {
                    if (not extendSelection) {
                        SetSelection (clickedOnChar, clickedOnChar + 1);
                        // select the entire embedding, and then process the rest as usual...
                        if (clickCount == 1) {
                            // In this case - it really doesn't matter if we pick the LHS or RHS of the embedding
                            // as the drag anchor...
                            *dragAnchor = clickedOnChar;
                        }
                        return embedding->HandleClick (clickedAt, 1);
                    }
                } break;

                case 2: {
                    //  If we dbl-click on an embedding, then be sure it is still selected, and then try to open it.
                    if (not extendSelection) {
                        SetSelection (clickedOnChar, clickedOnChar + 1);
                        // DO OPEN? Anyhow - even if no open, the treating the object as a word makes good sense...
                        if (clickCount == 1) {
                            // In this case - it really doesn't matter if we pick the LHS or RHS of the embedding
                            // as the drag anchor...
                            *dragAnchor = clickedOnChar;
                        }
                        return embedding->HandleClick (clickedAt, 2);
                    }
                } break;

                default: {
                    // others are ignored
                } break;
            }
        }
    }
    return TextInteractor::ProcessSimpleClick (clickedAt, clickCount, extendSelection, dragAnchor);
}

void StandardStyledTextInteractor::WhileSimpleMouseTracking (Led_Point newMousePos, size_t dragAnchor)
{
    size_t clickedOnChar = GetCharAtWindowLocation (newMousePos);
    size_t oldSelStart   = GetSelectionStart ();
    size_t oldSelEnd     = GetSelectionEnd ();

    /*
     *  If the drag anchor is coincident with the LHS or RHS of the clicked on character and the selection length
     *  is one (we clicked on an embedding) - then just eat that mousetracking - and prevent the selection from
     *  changing.
     */
    if ((clickedOnChar == dragAnchor or clickedOnChar + 1 == dragAnchor) and (oldSelEnd - oldSelStart == 1)) {
        vector<SimpleEmbeddedObjectStyleMarker*> embeddingList = CollectAllEmbeddingMarkersInRange (clickedOnChar, clickedOnChar + 1);
        if (embeddingList.size () == 1) {
            SimpleEmbeddedObjectStyleMarker* embedding = embeddingList[0];
            AssertMember (embedding, SimpleEmbeddedObjectStyleMarker);
            return;
        }
    }
    TextInteractor::WhileSimpleMouseTracking (newMousePos, dragAnchor);
}

void StandardStyledTextInteractor::InteractiveReplace (const Led_tChar* withWhat, size_t withWhatCharCount, UpdateMode updateMode)
{
    UpdateMode useUpdateMode = updateMode == eImmediateUpdate ? eDelayedUpdate : updateMode;
    Assert (not fEmptySelectionStyleSuppressMode);
    fEmptySelectionStyleSuppressMode = true;
    try {
        TextInteractor::InteractiveReplace (withWhat, withWhatCharCount, useUpdateMode);
        fEmptySelectionStyleSuppressMode = false;
    }
    catch (...) {
        fEmptySelectionStyleSuppressMode = false;
        throw;
    }
    if (updateMode == eImmediateUpdate) {
        Update ();
    }
}

void StandardStyledTextInteractor::SetSelection (size_t start, size_t end)
{
    bool changed = (GetSelectionStart () != start) or (GetSelectionEnd () != end);
    TextInteractor::SetSelection (start, end);
    if (changed) {
        StandardStyledTextInteractor::SetSelection_ (start, end);
    }
}

void StandardStyledTextInteractor::SetSelection_ (size_t start, size_t end)
{
    Led_Arg_Unused (start);
    Led_Arg_Unused (end);
    // SetEmptySelectionStyle () assumes selection already set - uses set one - assure that we're called
    // at the right time and that it already HAS been set
    Require (start == GetSelectionStart ());
    Require (end == GetSelectionEnd ());
    if (not fEmptySelectionStyleSuppressMode) {
        SetEmptySelectionStyle ();
    }
}

/*
@METHOD:        StandardStyledTextInteractor::GetEmptySelectionStyle
@DESCRIPTION:   <p>Return the style applied to newly typed text (or interactively entered text) at the current
    selection. This font defaults to the same as the surrounding text. But can be changed under user-control,
    in order to implement the usual semantics of a font / style menu.</p>
        <p>See @'StandardStyledTextInteractor::SetEmptySelectionStyle'.</p>
*/
Led_FontSpecification StandardStyledTextInteractor::GetEmptySelectionStyle () const
{
    return fEmptySelectionStyle;
}

/*
@METHOD:        StandardStyledTextInteractor::SetEmptySelectionStyle_OVLD
@DESCRIPTION:   <p>Same as @'StandardStyledTextInteractor::SetEmptySelectionStyle' but always grabs style info
            from the surrounding text (called from @'StandardStyledTextInteractor::SetSelection_').</p>
        <p>See @'StandardStyledTextInteractor::GetEmptySelectionStyle'.</p>
*/
void StandardStyledTextInteractor::SetEmptySelectionStyle ()
{
    size_t start = 0;
    size_t end   = 0;
    GetSelection (&start, &end);
    if (fLeftSideOfSelectionInteresting) {
        if (start < GetTextStore ().GetEnd ()) {
            fEmptySelectionStyle = GetStyleInfo (start);
        }
    }
    else {
        fEmptySelectionStyle = GetStyleInfo (FindPreviousCharacter (end));
    }
    if (end == GetEnd ()) {
        SetStyleInfo (GetEnd (), 1, fEmptySelectionStyle);
    }
}

/*
@METHOD:        StandardStyledTextInteractor::SetEmptySelectionStyle
@DESCRIPTION:   <p>Set the @'Led_FontSpecification' applied to newly typed text (or interactively entered text) at the current
    selection. This font defaults to the same as the surrounding text. But can be changed under user-control,
    in order to implement the usual semantics of a font / style menu.</p>
        <p>Note: if the selection is currently empty, this routine will make appropriate AboutToUpdate/DidUpdate calls to
    notifie anyone interested of the change (so - for example - the cached font metrics of text can change).</p>
        <p>See @'StandardStyledTextInteractor::GetEmptySelectionStyle'.</p>
*/
void StandardStyledTextInteractor::SetEmptySelectionStyle (Led_FontSpecification newEmptyFontSpec)
{
    if (fEmptySelectionStyle != newEmptyFontSpec) {
        /*
         *  If we change the empty style selection, this change can affect menus etc (since they show
         *  the currently selected font which - with an empty selection is really just 'fEmptySelectionStyle').
         *
         *  Also - this can change the size of the current row of text (when you change the font of the empty style to make
         *  it big - even if you type no characters - the row immediately gets bigger).
         */
        size_t selStart = 0;
        size_t selEnd   = 0;
        GetSelection (&selStart, &selEnd);
        if (selStart == selEnd) {
            {
                // not sure if we really need this scoping operator - just added to assure preserving semeantics - for now - LGP 2003-03-16
                TextStore::SimpleUpdater updater (GetTextStore (), selStart, selEnd, false);
                fEmptySelectionStyle = newEmptyFontSpec;
            }
            if (selEnd == GetEnd ()) {
                SetStyleInfo (GetEnd (), 1, fEmptySelectionStyle);
            }
        }
    }
}

bool StandardStyledTextInteractor::InteractiveReplaceEarlyPostReplaceHook (size_t withWhatCharCount)
{
    Assert (GetSelectionStart () >= withWhatCharCount);
    if (withWhatCharCount == 1) {
        // If we just typed a single extra char - apply our fEmptySelectionStyle to that extra char typed. Return true iff
        // that caused a font-style change.
        size_t                charAt    = FindPreviousCharacter (GetSelectionStart ()); // start of char we just added
        Led_FontSpecification prevStyle = GetStyleInfo (charAt);
        if (prevStyle != fEmptySelectionStyle) {
            SetStyleInfo (charAt, withWhatCharCount, Led_IncrementalFontSpecification (fEmptySelectionStyle));
            return true;
        }
    }
    return false;
}

vector<SimpleEmbeddedObjectStyleMarker*> StandardStyledTextInteractor::CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const
{
    /*
     *  Walk through all the markers in existence, and throw away all but our
     *  SimpleEmbeddedObjectStyleMarker markers. This is an inefficient approach. It would be far
     *  faster to keep a linked, or doubly linked list of all these guys.
     *  But this approach saves a bit of memory, and til we see this as a problem, lets just
     *  live with it.
     */
    MarkersOfATypeMarkerSink2Vector<SimpleEmbeddedObjectStyleMarker> result;
    GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, result);
    return result.fResult;
}

InteractiveReplaceCommand::SavedTextRep* StandardStyledTextInteractor::InteractiveUndoHelperMakeTextRep (size_t regionStart, size_t regionEnd, size_t selStart, size_t selEnd)
{
    if (regionStart == regionEnd) {
        // optimization, cuz these are smaller
        return new EmptySelStyleTextRep (this, selStart, selEnd);
    }
    else {
        return TextInteractor::InteractiveUndoHelperMakeTextRep (regionStart, regionEnd, selStart, selEnd);
    }
}

/*
 ********************************************************************************
 ************************* StandardStyledTextIOSinkStream ***********************
 ********************************************************************************
 */
using StandardStyledTextIOSinkStream = StandardStyledTextInteractor::StandardStyledTextIOSinkStream;
StandardStyledTextIOSinkStream::StandardStyledTextIOSinkStream (
    TextStore*                                        textStore,
    const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
    size_t                                            insertionStart)
    : inherited ()
    , fSavedContexts ()
    , fTextStore (textStore)
    , fStyleRunDatabase (textStyleDatabase)
    , fOriginalStart (insertionStart)
    , fInsertionStart (insertionStart)
    , fSavedStyleInfo ()
    , fCachedText ()
{
    RequireNotNull (textStore);
    Require (textStyleDatabase.get () != nullptr);
}

StandardStyledTextIOSinkStream::~StandardStyledTextIOSinkStream ()
{
    try {
        Flush ();
    }
    catch (...) {
        // ignore, cuz cannot fail out of DTOR
    }
}

size_t StandardStyledTextIOSinkStream::current_offset () const
{
    return (fInsertionStart - fOriginalStart);
}

void StandardStyledTextIOSinkStream::AppendText (const Led_tChar* text, size_t nTChars, const Led_FontSpecification* fontSpec)
{
    RequireNotNull (text);
    AssertNotNull (fTextStore);

    //  If caching, append the text to an array. Coun't on the array to have efficient
    //  growing properties (does for MSVC50 - grows by factor of two, so log-n append times).
    fCachedText.insert (fCachedText.end (), text, text + nTChars);

    /*
     * Keep track of the style changes that need to be applied later. If there is NO spec specified, use the one from
     * the previous section. Check for 'nTChars == 0' - don't append empty info-summary records.
     */
    if (nTChars != 0) {
        if (fontSpec == nullptr) {
            if (fSavedStyleInfo.size () == 0) {
                fSavedStyleInfo.push_back (StandardStyledTextImager::InfoSummaryRecord (fStyleRunDatabase->GetStyleInfo (fOriginalStart, 0)[0], nTChars));
            }
            else {
                fSavedStyleInfo.back ().fLength += nTChars;
            }
        }
        else {
            fSavedStyleInfo.push_back (StandardStyledTextImager::InfoSummaryRecord (*fontSpec, nTChars));
        }
    }
    fInsertionStart += nTChars;
}

void StandardStyledTextIOSinkStream::ApplyStyle (size_t from, size_t to, const vector<StandardStyledTextImager::InfoSummaryRecord>& styleRuns)
{
    Require (from <= to);
    if (GetCachedTextSize () != 0) {
        Flush ();
    }
    fStyleRunDatabase->SetStyleInfo (fOriginalStart + from, to - from, styleRuns);
}

Led_FontSpecification StandardStyledTextIOSinkStream::GetDefaultFontSpec () const
{
    return TextImager::GetStaticDefaultFont ();
}

void StandardStyledTextIOSinkStream::InsertEmbeddingForExistingSentinal (SimpleEmbeddedObjectStyleMarker* embedding, size_t at)
{
    RequireNotNull (embedding);
    if (GetCachedTextSize () != 0) {
        Flush ();
    }
    size_t    effectiveFrom = fOriginalStart + at;
    Led_tChar testSentinal;
    AssertNotNull (fTextStore);
    fTextStore->CopyOut (effectiveFrom, 1, &testSentinal);
    if (testSentinal != kEmbeddingSentinalChar) {
        Led_ThrowBadFormatDataException ();
    }
    Stroika::Frameworks::Led::InsertEmbeddingForExistingSentinal (embedding, *fTextStore, effectiveFrom, fStyleRunDatabase.get ());
}

void StandardStyledTextIOSinkStream::AppendEmbedding (SimpleEmbeddedObjectStyleMarker* embedding)
{
    RequireNotNull (embedding);
    AssertNotNull (fTextStore);
    if (GetCachedTextSize () != 0) {
        Flush ();
    }
    AddEmbedding (embedding, *fTextStore, fInsertionStart, fStyleRunDatabase.get ());
    fInsertionStart++;
}

void StandardStyledTextIOSinkStream::AppendSoftLineBreak ()
{
    // Bogus implementation - usually overriden...
    AppendText (LED_TCHAR_OF ("\n"), 1, nullptr);
}

void StandardStyledTextIOSinkStream::InsertMarker (Marker* m, size_t at, size_t length, MarkerOwner* markerOwner)
{
    Require (at <= current_offset ());
    RequireNotNull (m);
    RequireNotNull (markerOwner);
    AssertNotNull (fTextStore);
    if (GetCachedTextSize () != 0) {
        Flush ();
    }
    {
        TextStore::SimpleUpdater u (*fTextStore, fOriginalStart + at, fOriginalStart + at + length);
        fTextStore->AddMarker (m, fOriginalStart + at, length, markerOwner);
    }
}

void StandardStyledTextIOSinkStream::Flush ()
{
    if (GetCachedTextSize () != 0) {
        AssertNotNull (fTextStore);
        size_t dataSize      = fCachedText.size ();
        size_t whereToInsert = fInsertionStart - dataSize;
        fTextStore->Replace (whereToInsert, whereToInsert, Traversal::Iterator2Pointer (fCachedText.begin ()), dataSize);
        fCachedText.clear ();

        // Flush the cached style info
        fStyleRunDatabase->SetStyleInfo (whereToInsert, dataSize, fSavedStyleInfo.size (), Traversal::Iterator2Pointer (fSavedStyleInfo.begin ()));
        fSavedStyleInfo.clear ();
    }
    Ensure (fSavedStyleInfo.size () == 0);
}

void StandardStyledTextIOSinkStream::PushContext (TextStore*                                        ts,
                                                  const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
                                                  size_t                                            insertionStart)
{
    Require (GetCachedTextSize () == 0); // must flush before setting/popping context

    Context c;
    c.fTextStore        = fTextStore;
    c.fStyleRunDatabase = fStyleRunDatabase;
    c.fInsertionStart   = fInsertionStart;
    c.fOriginalStart    = fOriginalStart;
    fSavedContexts.push_back (c);
    fTextStore        = ts;
    fStyleRunDatabase = textStyleDatabase;
    fInsertionStart   = insertionStart;
    fOriginalStart    = insertionStart;
}

void StandardStyledTextIOSinkStream::PopContext ()
{
    Require (GetCachedTextSize () == 0); // must flush before setting/popping context
    Require (not fSavedContexts.empty ());
    fTextStore        = fSavedContexts.back ().fTextStore;
    fStyleRunDatabase = fSavedContexts.back ().fStyleRunDatabase;
    fInsertionStart   = fSavedContexts.back ().fInsertionStart;
    fOriginalStart    = fSavedContexts.back ().fOriginalStart;
    fSavedContexts.pop_back ();
}

/*
 ********************************************************************************
 **************************** StandardStyledTextIOSrcStream *********************
 ********************************************************************************
 */
using StandardStyledTextIOSrcStream = StandardStyledTextInteractor::StandardStyledTextIOSrcStream;
StandardStyledTextIOSrcStream::StandardStyledTextIOSrcStream (
    TextStore*                                        textStore,
    const StandardStyledTextImager::StyleDatabasePtr& textStyleDatabase,
    size_t selectionStart, size_t selectionEnd)
    : inherited ()
    , fTextStore (textStore)
    , fStyleRunDatabase (textStyleDatabase)
    , fCurOffset (selectionStart)
    , fSelStart (selectionStart)
    , fSelEnd (selectionEnd)
{
    RequireNotNull (textStore);
    Require (textStyleDatabase.get () != nullptr);
    Require (fSelStart >= 0);
    Require (fSelEnd >= 0);
    fSelEnd = min (fSelEnd, textStore->GetEnd ());
}

StandardStyledTextIOSrcStream::StandardStyledTextIOSrcStream (StandardStyledTextImager* textImager, size_t selectionStart, size_t selectionEnd)
    : inherited ()
    , fTextStore (textImager->PeekAtTextStore ())
    , fStyleRunDatabase (textImager->GetStyleDatabase ())
    , fCurOffset (selectionStart)
    , fSelStart (selectionStart)
    , fSelEnd (selectionEnd)
{
    RequireNotNull (textImager);
    RequireNotNull (fTextStore);
    Require (fStyleRunDatabase.get () != nullptr);
    Require (fSelStart >= 0);
    Require (fSelEnd >= 0);
    fSelEnd = min (fSelEnd, fTextStore->GetEnd ());
}

size_t StandardStyledTextIOSrcStream::readNTChars (Led_tChar* intoBuf, size_t maxTChars)
{
    AssertNotNull (intoBuf);
    size_t bytesToRead = min (maxTChars, fSelEnd - fCurOffset);
    Assert (bytesToRead <= maxTChars);
    fTextStore->CopyOut (fCurOffset, bytesToRead, intoBuf);
    fCurOffset += bytesToRead;
    return (bytesToRead);
}

size_t StandardStyledTextIOSrcStream::current_offset () const
{
    return (fCurOffset - fSelStart);
}

void StandardStyledTextIOSrcStream::seek_to (size_t to)
{
    Require (to >= 0);
    to += fSelStart;
    to         = min (to, fSelEnd);
    fCurOffset = to;
    Ensure (fCurOffset >= fSelStart);
    Ensure (fCurOffset <= fSelEnd);
}

size_t StandardStyledTextIOSrcStream::GetTotalTextLength () const
{
    Assert (fSelEnd >= fSelStart);
    return (fSelEnd - fSelStart);
}

vector<StandardStyledTextImager::InfoSummaryRecord> StandardStyledTextIOSrcStream::GetStyleInfo (size_t from, size_t len) const
{
    size_t effectiveFrom = from + fSelStart;
#if qDebug
    size_t effectiveTo = effectiveFrom + len;
#endif
    Require (effectiveFrom >= fSelStart);
    Require (effectiveFrom <= fSelEnd);
    Require (effectiveTo >= fSelStart);
    Require (effectiveTo <= fSelEnd);
    return (fStyleRunDatabase->GetStyleInfo (effectiveFrom, len));
}

vector<SimpleEmbeddedObjectStyleMarker*> StandardStyledTextIOSrcStream::CollectAllEmbeddingMarkersInRange (size_t from, size_t to) const
{
    size_t effectiveFrom = from + fSelStart;
    size_t effectiveTo   = to + fSelStart;
    Require (effectiveFrom >= fSelStart);
    Require (effectiveFrom <= fSelEnd);
    Require (effectiveTo >= fSelStart);
    Require (effectiveTo <= fSelEnd);

    MarkersOfATypeMarkerSink2Vector<SimpleEmbeddedObjectStyleMarker> result;
    AssertNotNull (fTextStore);
    fTextStore->CollectAllMarkersInRangeInto (effectiveFrom, effectiveTo, TextStore::kAnyMarkerOwner, result);
    return result.fResult;
}

StandardStyledTextIOSrcStream::Table* StandardStyledTextIOSrcStream::GetTableAt (size_t /*at*/) const
{
    return nullptr;
}

void StandardStyledTextIOSrcStream::SummarizeFontAndColorTable (set<Led_SDK_String>* fontNames, set<Led_Color>* colorsUsed) const
{
    using InfoSummaryRecord = StandardStyledTextImager::InfoSummaryRecord;
    if (fontNames != nullptr or colorsUsed != nullptr) {
        size_t                    totalTextLength = GetTotalTextLength ();
        vector<InfoSummaryRecord> styleRuns;
        if (totalTextLength != 0) {
            styleRuns = GetStyleInfo (0, totalTextLength);
        }
        for (auto i = styleRuns.begin (); i != styleRuns.end (); ++i) {
            if (fontNames != nullptr) {
                fontNames->insert ((*i).GetFontName ());
            }
            if (colorsUsed != nullptr) {
                colorsUsed->insert ((*i).GetTextColor ());
            }
        }
    }
}

size_t StandardStyledTextIOSrcStream::GetEmbeddingMarkerPosOffset () const
{
    return (fSelStart);
}

/*
 ********************************************************************************
 ************************* StyledTextFlavorPackageInternalizer ******************
 ********************************************************************************
 */
using StyledTextFlavorPackageInternalizer = StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer;
StyledTextFlavorPackageInternalizer::StyledTextFlavorPackageInternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase)
    : inherited (ts)
    , fStyleDatabase (styleDatabase)
{
}

void StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromName (
#if qPlatform_MacOS
    const FSSpec* fileName,
#elif qPlatform_Windows || qXWindows
    const Led_SDK_Char* fileName,
#endif
    Led_ClipFormat* suggestedClipFormat,
    CodePage*       suggestedCodePage)
{
    inherited::InternalizeFlavor_FILEGuessFormatsFromName (fileName, suggestedClipFormat, suggestedCodePage);

#if qPlatform_MacOS
// Should add code here to grab file-type from OS. If called from XXX - then thats already done, but in case
// called from elsewhere...
#elif qPlatform_Windows
    if (suggestedClipFormat != nullptr and *suggestedClipFormat == kBadClipFormat) {
        TCHAR drive[_MAX_DRIVE];
        TCHAR dir[_MAX_DIR];
        TCHAR fname[_MAX_FNAME];
        TCHAR ext[_MAX_EXT];
        ::_tsplitpath_s (fileName, drive, dir, fname, ext);
        if (::_tcsicmp (ext, Led_SDK_TCHAROF (".rtf")) == 0) {
            *suggestedClipFormat = kRTFClipFormat;
        }
        else if (::_tcsicmp (ext, Led_SDK_TCHAROF (".htm")) == 0) {
            *suggestedClipFormat = kHTMLClipFormat;
        }
        else if (::_tcsicmp (ext, Led_SDK_TCHAROF (".html")) == 0) {
            *suggestedClipFormat = kHTMLClipFormat;
        }
        else if (::_tcsicmp (ext, Led_SDK_TCHAROF (".led")) == 0) {
            *suggestedClipFormat = kLedPrivateClipFormat;
        }
    }
#endif
}

void StyledTextFlavorPackageInternalizer::InternalizeFlavor_FILEGuessFormatsFromStartOfData (
    Led_ClipFormat* suggestedClipFormat,
    CodePage*       suggestedCodePage,
    const Byte* fileStart, const Byte* fileEnd)

{
    inherited::InternalizeFlavor_FILEGuessFormatsFromStartOfData (suggestedClipFormat, suggestedCodePage, fileStart, fileEnd);
    if (suggestedClipFormat != nullptr) {
        if (*suggestedClipFormat == kBadClipFormat) {
            {
                StyledTextIOSrcStream_Memory source (fileStart, fileEnd - fileStart);
                StyledTextIOReader_RTF       reader (&source, nullptr);
                if (reader.QuickLookAppearsToBeRightFormat ()) {
                    *suggestedClipFormat = kRTFClipFormat;
                    return;
                }
            }

            {
                StyledTextIOSrcStream_Memory source (fileStart, fileEnd - fileStart);
                StyledTextIOReader_HTML      reader (&source, nullptr);
                if (reader.QuickLookAppearsToBeRightFormat ()) {
                    *suggestedClipFormat = kHTMLClipFormat;
                    return;
                }
            }

#if qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
            {
                StyledTextIOSrcStream_Memory           source (fileStart, fileEnd - fileStart);
                StyledTextIOReader_LedNativeFileFormat reader (&source, nullptr);
                if (reader.QuickLookAppearsToBeRightFormat ()) {
                    *suggestedClipFormat = kLedPrivateClipFormat;
                    return;
                }
            }
#endif
        }
    }
}

bool StyledTextFlavorPackageInternalizer::InternalizeBestFlavor (ReaderFlavorPackage& flavorPackage,
                                                                 size_t from, size_t to)
{
    Require (from <= GetTextStore ().GetEnd ());
    Require (to <= GetTextStore ().GetEnd ());
    Require (from <= to);

    if (InternalizeFlavor_RTF (flavorPackage, from, to)) {
        return true;
    }
    else if (InternalizeFlavor_HTML (flavorPackage, from, to)) {
        return true;
    }
#if qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
    else if (InternalizeFlavor_Native (flavorPackage, from, to)) {
        return true;
    }
#endif
#if qPlatform_Windows
    // A bit of a hack. MSIE 3.0 generates both FILE and DIB objects on the clip
    // for when we drag out pictures. This allows us to grab the dibs in that case.
    // I just hope it doesn't cause too much trouble for other cases. For Led 2.2, we
    // must completely rewrite this code and find a better way to choose what to
    // grab out of a clip package...
    // LGP 961101
    else if (flavorPackage.GetFlavorAvailable (CF_DIB) and InternalizeFlavor_OtherRegisteredEmbedding (flavorPackage, from, to)) {
        return true;
    }
#endif
    else if (InternalizeFlavor_FILE (flavorPackage, from, to)) {
        return true;
    }
    else if (InternalizeFlavor_OtherRegisteredEmbedding (flavorPackage, from, to)) {
        return true;
    }
#if qPlatform_MacOS
    else if (InternalizeFlavor_STYLAndTEXT (flavorPackage, from, to)) {
        return true;
    }
#endif
    else if (InternalizeFlavor_TEXT (flavorPackage, from, to)) {
        return true;
    }
    return false;
}

#if qPlatform_MacOS
bool StyledTextFlavorPackageInternalizer::InternalizeFlavor_STYLAndTEXT (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
    size_t pasteStart = from;
    size_t pasteEnd   = to;
    Assert (pasteEnd >= pasteStart);

    TempMarker newSel (GetTextStore (), pasteStart + 1, pasteStart + 1);
    if (inherited::InternalizeFlavor_TEXT (flavorPackage, pasteStart, pasteEnd)) {
        if (flavorPackage.GetFlavorAvailable ('styl')) {
            size_t                         length = flavorPackage.GetFlavorSize ('styl');
            Memory::SmallStackBuffer<char> buf (length);
            length = flavorPackage.ReadFlavorData ('styl', length, buf);
            Assert (newSel.GetStart () >= pasteStart + 1);
            size_t pasteEndXXX = newSel.GetStart () - 1;
            Assert (pasteEndXXX >= pasteStart);
            StScrpRec*                styleRecords = reinterpret_cast<StScrpRec*> (static_cast<char*> (buf));
            vector<InfoSummaryRecord> ledStyleInfo = StandardStyledTextImager::Convert (styleRecords->scrpStyleTab, styleRecords->scrpNStyles);
            fStyleDatabase->SetStyleInfo (pasteStart, pasteEndXXX - pasteStart, ledStyleInfo);
        }

        // Even if we have no STYL info, we did already paste the text in, and that would be next
        // on our list to try anyhow...
        return true;
    }
    return false;
}
#endif

#if qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
bool StyledTextFlavorPackageInternalizer::InternalizeFlavor_Native (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
    if (flavorPackage.GetFlavorAvailable (kLedPrivateClipFormat)) {
        size_t                         length = flavorPackage.GetFlavorSize (kLedPrivateClipFormat);
        Memory::SmallStackBuffer<char> buf (length);
        length = flavorPackage.ReadFlavorData (kLedPrivateClipFormat, length, buf);

        size_t start = from;
        size_t end   = to;
        Assert (end >= start);

        GetTextStore ().Replace (start, end, LED_TCHAR_OF (""), 0); // clear current selection before insert
        {
            // Be sure these guys in scope like this so caches get flusehd before we update cursor position
            StyledTextIOSrcStream_Memory               source (buf, length);
            unique_ptr<StandardStyledTextIOSinkStream> sink (mkStandardStyledTextIOSinkStream (start));
            StyledTextIOReader_LedNativeFileFormat     textReader (&source, sink.get ());
            textReader.Read ();
            sink->Flush (); // would be called implcitly in DTOR, but call like this so exceptions get propagates...
        }
        return true;
    }
    else {
        return false;
    }
}
#endif

bool StyledTextFlavorPackageInternalizer::InternalizeFlavor_RTF (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
    if (flavorPackage.GetFlavorAvailable (kRTFClipFormat)) {
        size_t length = flavorPackage.GetFlavorSize (kRTFClipFormat);
        if (length == 0) {
            // Bizare - but Word2000 seems to sometimes return a zero-length RTF entity when you insert a BitMap ActiveX control into the WP, and
            // try to copy it... LGP 2000/04/26
            return false;
        }
        Memory::SmallStackBuffer<char> buf (length);
        length = flavorPackage.ReadFlavorData (kRTFClipFormat, length, buf);

        size_t start = from;
        size_t end   = to;
        Assert (end >= start);

        GetTextStore ().Replace (start, end, LED_TCHAR_OF (""), 0); // clear current selection before insert
        {
            // Be sure these guys in scope like this so caches get flusehd before we update cursor position
            StyledTextIOSrcStream_Memory               source (buf, length);
            unique_ptr<StandardStyledTextIOSinkStream> sink (mkStandardStyledTextIOSinkStream (start));
            StyledTextIOReader_RTF                     textReader (&source, sink.get ());
            textReader.Read ();
            sink->Flush (); // would be called implcitly in DTOR, but call like this so exceptions get propagates...
        }
        return true;
    }
    else {
        return false;
    }
}

bool StyledTextFlavorPackageInternalizer::InternalizeFlavor_HTML (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
    if (flavorPackage.GetFlavorAvailable (kHTMLClipFormat)) {
        size_t                         length = flavorPackage.GetFlavorSize (kHTMLClipFormat);
        Memory::SmallStackBuffer<char> buf (length);
        length = flavorPackage.ReadFlavorData (kHTMLClipFormat, length, buf);

        size_t start = from;
        size_t end   = to;
        Assert (end >= start);

        GetTextStore ().Replace (start, end, LED_TCHAR_OF (""), 0); // clear current selection before insert
        {
            // Be sure these guys in scope like this so caches get flusehd before we update cursor position
            StyledTextIOSrcStream_Memory               source (buf, length);
            unique_ptr<StandardStyledTextIOSinkStream> sink (mkStandardStyledTextIOSinkStream (start));
            StyledTextIOReader_HTML                    textReader (&source, sink.get ());
            textReader.Read ();
            sink->Flush (); // would be called implcitly in DTOR, but call like this so exceptions get propagates...
        }
        return true;
    }
    else {
        return false;
    }
}

bool StyledTextFlavorPackageInternalizer::InternalizeFlavor_OtherRegisteredEmbedding (ReaderFlavorPackage& flavorPackage, size_t from, size_t to)
{
    const vector<EmbeddedObjectCreatorRegistry::Assoc>& types = EmbeddedObjectCreatorRegistry::Get ().GetAssocList ();
    for (size_t i = 0; i < types.size (); i++) {
        EmbeddedObjectCreatorRegistry::Assoc assoc           = types[i];
        bool                                 clipAvailForAll = (assoc.fFormatTagCount != 0);
        for (size_t j = 0; j < assoc.fFormatTagCount; j++) {
            if (not flavorPackage.GetFlavorAvailable (assoc.GetIthFormat (j))) {
                clipAvailForAll = false;
                break;
            }
        }
        if (clipAvailForAll) {
            SimpleEmbeddedObjectStyleMarker* objMarker = (assoc.fReadFromFlavorPackage) (flavorPackage);
            {
                size_t pasteStart = from;
                size_t pasteEnd   = to;
                Assert (pasteEnd >= pasteStart);

                GetTextStore ().Replace (pasteStart, pasteEnd, &kEmbeddingSentinalChar, 1); // clear current selection and put in embedding character

                {
                    // add marker, and do DID_UPDATE stuff so cached metrics and rowheights get refreshed...
                    TextStore::SimpleUpdater updater (GetTextStore (), pasteStart, pasteStart + 1);
                    GetTextStore ().AddMarker (objMarker, pasteStart, 1, fStyleDatabase.get ());
                }
            }

            return true;
        }
    }
    return false;
}

/*
@METHOD:        StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer::mkStandardStyledTextIOSinkStream
@DESCRIPTION:   <p>Hook function so that the various Externalize_XXX methods in
            @'StandardStyledTextInteractor::StyledTextFlavorPackageInternalizer' can use a dynamicly typed
            SinkStream. So - for example - the externalize methods include paragraph info.</p>
*/
StandardStyledTextInteractor::StandardStyledTextIOSinkStream* StyledTextFlavorPackageInternalizer::mkStandardStyledTextIOSinkStream (size_t insertionStart)
{
    return new StandardStyledTextIOSinkStream (PeekAtTextStore (), fStyleDatabase, insertionStart);
}

/*
 ********************************************************************************
 ************************* StyledTextFlavorPackageExternalizer ******************
 ********************************************************************************
 */
using StyledTextFlavorPackageExternalizer = StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer;
StyledTextFlavorPackageExternalizer::StyledTextFlavorPackageExternalizer (TextStore& ts, const StandardStyledTextImager::StyleDatabasePtr& styleDatabase)
    : inherited (ts)
    , fStyleDatabase (styleDatabase)
{
}

void StyledTextFlavorPackageExternalizer::ExternalizeFlavors (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
    size_t start = from;
    size_t end   = to;
    Require (start >= 0);
    Require (end <= GetTextStore ().GetEnd ());
    Require (start <= end);

    /*
     * Enumerate in fidelity order flavors to copy...
     */

    // Directly write out singly selected embedding
    {
        MarkersOfATypeMarkerSink2Vector<SimpleEmbeddedObjectStyleMarker> embeddings;
        GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, embeddings);
        if ((embeddings.fResult.size () == 1) and (start + 1 == end)) {
            /*
             *  If we have some embedding like a pict or whatever selected, no reason
             *  to copy to clip anything else.
             */
            ExternalizeFlavor_SingleSelectedEmbedding (flavorPackage, embeddings.fResult[0]);
#if 0
            // WELL, this trick (of returning here) is ALMOST right.
            // At least for the time being, some 'unknown' embeddings get externalized in
            // some format NOBODY - including LED will recognize (eg rtf pictures).
            // This isn't fatal, except
            // in a few rare cases. For example, when we try to
            return;
#endif
        }
    }

    // not sure we should do if single selected???? -LGP 961014
    // If NOT, we must be careful about case of unknown RTF embeddings!!! - See SPR# 0397
    ExternalizeFlavor_RTF (flavorPackage, start, end);

    ExternalizeFlavor_TEXT (flavorPackage, start, end);

#if qPlatform_MacOS
    ExternalizeFlavor_STYL (flavorPackage, start, end);
#endif
}

void StyledTextFlavorPackageExternalizer::ExternalizeBestFlavor (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
    ExternalizeFlavor_RTF (flavorPackage, from, to);
}

#if qPlatform_MacOS
void StyledTextFlavorPackageExternalizer::ExternalizeFlavor_STYL (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
    Require (from <= to);
    Require (to <= GetTextStore ().GetEnd ());
    size_t length = to - from;

    vector<InfoSummaryRecord> ledStyleRuns = fStyleDatabase->GetStyleInfo (from, length);
    size_t                    nStyleRuns   = ledStyleRuns.size ();

    Assert (offsetof (StScrpRec, scrpStyleTab) == sizeof (short)); // thats why we add sizeof (short)

    size_t                         nBytes = sizeof (short) + nStyleRuns * sizeof (ScrpSTElement);
    Memory::SmallStackBuffer<char> buf (nBytes);
    StScrpPtr                      stylePtr = (StScrpPtr) (char*)buf;

    stylePtr->scrpNStyles = nStyleRuns;
    StandardStyledTextImager::Convert (ledStyleRuns, stylePtr->scrpStyleTab);
    flavorPackage.AddFlavorData ('styl', nBytes, stylePtr);
}
#endif

#if qIncludeLedNativeFileFormatSupportInStandardStyledTextInteractor
void StyledTextFlavorPackageExternalizer::ExternalizeFlavor_Native (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
    Require (from <= to);
    Require (to <= GetTextStore ().GetEnd ());
    unique_ptr<StandardStyledTextIOSrcStream> source (mkStandardStyledTextIOSrcStream (from, to));
    StyledTextIOWriterSinkStream_Memory       sink;
    StyledTextIOWriter_LedNativeFileFormat    textWriter (source.get (), &sink);
    textWriter.Write ();
    flavorPackage.AddFlavorData (kLedPrivateClipFormat, sink.GetLength (), sink.PeekAtData ());
}
#endif

void StyledTextFlavorPackageExternalizer::ExternalizeFlavor_RTF (WriterFlavorPackage& flavorPackage, size_t from, size_t to)
{
    Require (from <= to);
    Require (to <= GetTextStore ().GetEnd ());
    unique_ptr<StandardStyledTextIOSrcStream> source (mkStandardStyledTextIOSrcStream (from, to));
    StyledTextIOWriterSinkStream_Memory       sink;
    StyledTextIOWriter_RTF                    textWriter (source.get (), &sink);
    textWriter.Write ();
    flavorPackage.AddFlavorData (kRTFClipFormat, sink.GetLength (), sink.PeekAtData ());
}

void StyledTextFlavorPackageExternalizer::ExternalizeFlavor_SingleSelectedEmbedding (WriterFlavorPackage& flavorPackage, SimpleEmbeddedObjectStyleMarker* embedding)
{
    RequireNotNull (embedding);
    embedding->ExternalizeFlavors (flavorPackage);
}

/*
@METHOD:        StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer::mkStandardStyledTextIOSrcStream
@DESCRIPTION:   <p>Hook function so that the various Internalize_XXX methods in
    @'StandardStyledTextInteractor::StyledTextFlavorPackageExternalizer' can use a dynamicly typed
    SinkStream. So - for example - the internalize methods include paragraph info.</p>
*/
StandardStyledTextInteractor::StandardStyledTextIOSrcStream* StyledTextFlavorPackageExternalizer::mkStandardStyledTextIOSrcStream (size_t selectionStart, size_t selectionEnd)
{
    return new StandardStyledTextIOSrcStream (PeekAtTextStore (), fStyleDatabase, selectionStart, selectionEnd);
}

/*
 ********************************************************************************
 ***************************** EmptySelStyleTextRep *****************************
 ********************************************************************************
 */
using EmptySelStyleTextRep = StandardStyledTextInteractor::EmptySelStyleTextRep;

EmptySelStyleTextRep::EmptySelStyleTextRep (StandardStyledTextInteractor* interactor, size_t selStart, size_t selEnd)
    : inherited (selStart, selEnd)
    , fSavedStyle (interactor->fEmptySelectionStyle)
{
}

size_t EmptySelStyleTextRep::GetLength () const
{
    return 0;
}

void EmptySelStyleTextRep::InsertSelf (TextInteractor* interactor, size_t at, size_t nBytesToOverwrite)
{
    RequireNotNull (interactor);
    interactor->Replace (at, at + nBytesToOverwrite, LED_TCHAR_OF (""), 0);

    StandardStyledTextInteractor* si = dynamic_cast<StandardStyledTextInteractor*> (interactor);
    RequireNotNull (si); // cannot DO with one type, and UNDO with another!

    si->SetEmptySelectionStyle (fSavedStyle);
}
