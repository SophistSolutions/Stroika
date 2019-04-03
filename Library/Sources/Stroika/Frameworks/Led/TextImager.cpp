/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#if qPlatform_MacOS
#include <Script.h>
#endif

#include "GDI.h"

#include "TextImager.h"

using namespace Stroika::Foundation;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

#if qPlatform_MacOS
inline void GDI_TextFont (short font)
{
#if TARGET_CARBON
    ::TextFont (font);
#else
    if (Led_GetCurrentGDIPort ()->txFont != font) {
        ::TextFont (font);
    }
#endif
}
inline void GDI_TextFace (short face)
{
#if TARGET_CARBON
    ::TextFace (face);
#else
    if (Led_GetCurrentGDIPort ()->txFace != face) {
        ::TextFace (face);
    }
#endif
}
inline void GDI_TextMode (short mode)
{
#if TARGET_CARBON
    ::TextMode (mode);
#else
    if (Led_GetCurrentGDIPort ()->txMode != mode) {
        ::TextMode (mode);
    }
#endif
}
inline void GDI_TextSize (short size)
{
#if TARGET_CARBON
    ::TextSize (size);
#else
    if (Led_GetCurrentGDIPort ()->txSize != size) {
        ::TextSize (size);
    }
#endif
}
#endif

/*
 ********************************************************************************
 *********************** TextImager::FontCacheInfoUpdater ***********************
 ********************************************************************************
 */
#if qPlatform_Windows
inline bool LogFontsEqual (LOGFONT lhs, LOGFONT rhs)
{
    size_t bytesToCompare = offsetof (LOGFONT, lfFaceName) + (::_tcslen (lhs.lfFaceName) + 1) * sizeof (Led_SDK_Char);
    Require (bytesToCompare <= sizeof (LOGFONT)); // else we were passed bogus LogFont (and we should validate them before here!)
    return ::memcmp (&lhs, &rhs, bytesToCompare) == 0;
}
inline bool LogFontsEqual (const Led_FontSpecification& lhs, const Led_FontSpecification& rhs)
{
    if (lhs.GetStyle_SubOrSuperScript () == rhs.GetStyle_SubOrSuperScript ()) {
        LOGFONT lhslf;
        lhs.GetOSRep (&lhslf);
        LOGFONT rhslf;
        rhs.GetOSRep (&rhslf);
        return LogFontsEqual (lhslf, rhslf);
    }
    else {
        return false;
    }
}
#endif
TextImager::FontCacheInfoUpdater::FontCacheInfoUpdater (const TextImager* imager, Led_Tablet tablet, const Led_FontSpecification& fontSpec)
    : fImager (imager)
#if qPlatform_Windows
    , fTablet (tablet)
    , fRestoreObject (nullptr)
    , fRestoreAttribObject (nullptr)
#endif
{
#if qPlatform_MacOS
    /*
     *  For MAC:
     *
     *      Just set the font using the font-spec, and assume any 'restore' will be taken care of
     *  at a higher level (really just ignore as long as I can, and if I need to, then
     *  probably best to take care of here!!! (modularity vs speed).
     */
    short fontID    = 0;
    short fontSize  = 0;
    Style fontStyle = 0;
    fontSpec.GetOSRep (&fontID, &fontSize, &fontStyle);
    tablet->SetPort ();
    GDI_TextFont (fontID);
    if (fontSpec.GetStyle_SubOrSuperScript () != Led_FontSpecification::eNoSubOrSuperscript) {
        // See SPR#1523- was 'max (fontSize/2, 1);'
        // Careful to sync this with TextImager::DrawSegment_ () 'drawTop' adjustment
        fontSize = max (fontSize * 2 / 3, 1);
    }
    GDI_TextSize (fontSize);
    GDI_TextFace (fontStyle);

    if (not imager->fCachedFontValid or fontSpec != imager->fCachedFontSpec) {
        imager->fCachedFontInfo  = tablet->GetFontMetrics ();
        imager->fCachedFontSpec  = fontSpec;
        imager->fCachedFontValid = true;
    }
#elif qPlatform_Windows
    /*
     *  For Windows:
     *
     *      Just set the font using the font-spec. Cache font, so we don't keep creating. But select it
     *  in to tablet each time. And on DTOR, restore old font into tablet.
     */
    bool changed = false;
    if (imager->fCachedFont == nullptr or !LogFontsEqual (fontSpec, imager->fCachedFontSpec)) {
        changed = true;
        delete imager->fCachedFont;
        imager->fCachedFont = nullptr;
        imager->fCachedFont = new Led_FontObject ();
        LOGFONT lf;
        fontSpec.GetOSRep (&lf);
        if (fontSpec.GetStyle_SubOrSuperScript () != Led_FontSpecification::eNoSubOrSuperscript) {
            // See SPR#1523- was 'lf.lfHeight /= 2'
            // Careful to sync this with TextImager::DrawSegment_ () 'drawTop' adjustment
            lf.lfHeight = lf.lfHeight * 2 / 3;
            if (lf.lfHeight == 0) {
                lf.lfHeight = 1;
            }
        }
        Verify (imager->fCachedFont->CreateFontIndirect (&lf));
        imager->fCachedFontSpec = fontSpec;
    }
    AssertNotNull (imager->fCachedFont);

    // See CDC::SelectObject for the logic..., but we do better than thiers and restore
    // right object to right DC!!!! - LGP 950525
    if (tablet->m_hDC != tablet->m_hAttribDC) {
        fRestoreObject = ::SelectObject (tablet->m_hDC, imager->fCachedFont->m_hObject);
        // At one point i had Asserts() here that fRestoreObject != nullptr - but at least for PrintDC's, apparently
        // fRestoreObject can be nullptr, and apparently it isn't a problem...
    }
    if (tablet->m_hAttribDC != nullptr) {
        fRestoreAttribObject = ::SelectObject (tablet->m_hAttribDC, imager->fCachedFont->m_hObject);
        // At one point i had Asserts() here that fRestoreAttribObject != nullptr - but at least for PrintDC's, apparently
        // fRestoreAttribObject can be nullptr, and apparently it isn't a problem...
    }
    if (changed) {
        imager->fCachedFontInfo = tablet->GetFontMetrics ();
    }
#elif qXWindows
    tablet->SetFont (fontSpec);
    imager->fCachedFontInfo = tablet->GetFontMetrics ();
#endif
}

/*
 ********************************************************************************
 ***************************** TextImager::HilightMarker ************************
 ********************************************************************************
 */
TextImager::HilightMarker::HilightMarker ()
    : Marker ()
{
}

/*
 ********************************************************************************
 ************************************** TextImager ******************************
 ********************************************************************************
 */
TextImager::TextImager ()
    : fTextStore (nullptr)
    , fDefaultFont (GetStaticDefaultFont ())
    , fForceAllRowsShowing (true)
    , fImageUsingOffscreenBitmaps (qUseOffscreenBitmapsToReduceFlicker)
    , fHScrollPos (0)
    , fSuppressGoalColumnRecompute (false)
    , fSelectionGoalColumn (Led_TWIPS (0))
    , fUseEOLBOLRowHilightStyle (true)
    , fSelectionShown (false)
    , fWindowRect (Led_Rect (0, 0, 0, 0))
    , fHiliteMarker (nullptr)
    , fWeAllocedHiliteMarker (false)
    ,
    //  fDefaultColorIndex (),
    fCachedFontSpec ()
    , fCachedFontInfo ()
    ,
#if qPlatform_Windows
    fCachedFont (nullptr)
#else
    fCachedFontValid (false)
#endif
{
    for (Led_Color** i = &fDefaultColorIndex[0]; i < &fDefaultColorIndex[eMaxDefaultColorIndex]; ++i) {
        *i = nullptr;
    }
}

TextImager::~TextImager ()
{
    Require (fTextStore == nullptr);
    Require (fHiliteMarker == nullptr);
    for (Led_Color** i = &fDefaultColorIndex[0]; i < &fDefaultColorIndex[eMaxDefaultColorIndex]; ++i) {
        delete *i;
        *i = nullptr;
    }
#if qPlatform_Windows
    delete fCachedFont;
#endif
}

TextStore* TextImager::PeekAtTextStore () const
{
    return fTextStore; // Can return nullptr if no markers owned
}

/*
@METHOD:        TextImager::SpecifyTextStore
@DESCRIPTION:   <p>Associate this TextImager with the given @'TextStore'.</p>
        <p>Note that this will - as a side-effect - call
    @'TextStore::AddMarkerOwner'. So be sure that this @'TextImager' has not yet been added as a @'MarkerOwner' for
    any other (or this given) TextStore.</p>
*/
void TextImager::SpecifyTextStore (TextStore* useTextStore)
{
    if (fTextStore != useTextStore) {
        if (fTextStore != nullptr) {
            HookLosingTextStore ();
        }
        fTextStore = useTextStore;
        if (fTextStore != nullptr) {
            HookGainedNewTextStore ();
        }
    }
}

void TextImager::HookLosingTextStore ()
{
    HookLosingTextStore_ ();
}

void TextImager::HookLosingTextStore_ ()
{
    // BE MORE CAREFUL HERE - NO NEED TO DELETE HILIGHT MARKER - JUST REMOVE AND RE_ADD!!!
    AssertNotNull (fTextStore);
    if (fHiliteMarker != nullptr) {
        Assert (fWeAllocedHiliteMarker); // otherwise setter better have unset!!!
        GetTextStore ().RemoveMarker (fHiliteMarker);
        delete fHiliteMarker;
        fHiliteMarker = nullptr;
        fTextStore->RemoveMarkerOwner (this);
    }
}

void TextImager::HookGainedNewTextStore ()
{
    HookGainedNewTextStore_ ();
}

void TextImager::HookGainedNewTextStore_ ()
{
    AssertNotNull (fTextStore);
    if (fHiliteMarker == nullptr) {
        fTextStore->AddMarkerOwner (this);
        SetHilightMarker (nullptr); // forces creation of default one...
    }
}

/*
@METHOD:        TextImager::PurgeUnneededMemory
@DESCRIPTION:   <p>Call when you want to have the text engine save a little bit of memory. Frees up data
            that isn't required. This doesn't NEED to be called, but can be called by memory-restricted applications.
            The default implementation calls @'TextImager::InvalidateAllCaches'.
            </p>
*/
void TextImager::PurgeUnneededMemory ()
{
    InvalidateAllCaches ();
}

/*
@METHOD:        TextImager::InvalidateAllCaches
@DESCRIPTION:   <p>This is called - for example - when you change the wrap width for the entire document, or font information
            for the entire document, or maybe something about the @'Led_Tablet' metrics you are imaging to.
            TextImager::InvalidateAllCaches is called automatically from @'MultiRowTextImager::TabletChangedMetrics ()'
            </p>
*/
void TextImager::InvalidateAllCaches ()
{
// Classes which cache font-based information must OVERRIDE and invalidate it...
#if qPlatform_Windows
    delete fCachedFont;
    fCachedFont = nullptr;
#else
    fCachedFontValid = false;
#endif
}

/*
@METHOD:        TextImager::SetDefaultFont
@DESCRIPTION:   <p>Sets the default font associated with the given imager. This is NOT necessarily the one you will
            see displayed, as the font displayed maybe overriden by more specific font information from
            @'StyledTextImager::StyleMarker' or @'StandardStyledTextImager'.
            </p>
                <p>Note that the semantics of SetDefaultFontChanged in 3.1a4. If you want to set the font
            of a specific range of styled text (e.g. in a word-processor class) - then you may want to
            use @'StandardStyledTextInteractor::InteractiveSetFont'.
            </p>
*/
void TextImager::SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont)
{
    SetDefaultFont_ (defaultFont);
}

void TextImager::SetDefaultFont_ (const Led_IncrementalFontSpecification& defaultFont)
{
    if (PeekAtTextStore () != nullptr) {
        TextStore::SimpleUpdater u (GetTextStore (), 0, GetTextStore ().GetEnd (), false); // update buffer so cached measurement values refreshed
        fDefaultFont.MergeIn (defaultFont);
        if (defaultFont.GetTextColor_Valid ()) {
            SetDefaultTextColor (eDefaultTextColor, defaultFont.GetTextColor ());
        }
    }
    else {
        fDefaultFont.MergeIn (defaultFont);
        if (defaultFont.GetTextColor_Valid ()) {
            SetDefaultTextColor (eDefaultTextColor, defaultFont.GetTextColor ());
        }
    }
}

#if qPlatform_Windows
struct FontSelectionInfo {
    FontSelectionInfo (BYTE desiredCharset)
        : fDesiredCharset (desiredCharset)
        , fUsesBestCharset (false)
        , fIsTT (false)
        , fIsANSI_VAR_Font (false)
        , fIsGoodBackupCharset (false)
        , fIsFavoriteForCharset (false)
        , fIsVariablePitch (false)
        , fStartsWithAt (false)
    {
        memset (&fBestFont, 0, sizeof (fBestFont));
    }
    BYTE       fDesiredCharset;
    LOGFONT    fBestFont;
    bool       fUsesBestCharset;
    bool       fIsTT;
    bool       fIsANSI_VAR_Font; // a good second choice if Arial not present
    bool       fIsGoodBackupCharset;
    bool       fIsFavoriteForCharset;
    bool       fIsVariablePitch;
    bool       fStartsWithAt;
    inline int Score () const
    {
        int score = 0;
        if (fUsesBestCharset) {
            score += 10; // underweight - because (at least with Win2K) - the fCharset field almost always set to zero - so cannot be used reliably
        }
        if (fIsTT) {
            score += 20;
        }
        if (fIsANSI_VAR_Font) {
            score += 4;
        }
        if (fIsGoodBackupCharset) {
            score += 2;
        }
        if (fIsFavoriteForCharset) {
            score += 25;
        }
        if (fIsVariablePitch) {
            //unsure if this is desirable or not - good for US - but not sure about Japan?
            score += 2;
        }
        if (fStartsWithAt) {
            score -= 25; // we don't really support vertical fonts
        }
        return score;
    }
};
static int FAR PASCAL EnumFontCallback (const LOGFONT* lplf, const TEXTMETRIC* /*lpntm*/, DWORD fontType, LPARAM arg)
{
    // Score each font choice, and pick the 'best' one. Pick randomly if several are 'best'.
    RequireNotNull (lplf);
    FontSelectionInfo& result = *(FontSelectionInfo*)arg;

    static LOGFONT theANSILogFont;
    if (theANSILogFont.lfFaceName[0] == '\0') {
        HFONT xxx = HFONT (::GetStockObject (ANSI_VAR_FONT));
        Verify (::GetObject (xxx, sizeof theANSILogFont, &theANSILogFont));
    }

    FontSelectionInfo potentialResult = result;
    memcpy (&potentialResult.fBestFont, lplf, sizeof (LOGFONT));

    if (potentialResult.fDesiredCharset == DEFAULT_CHARSET) {
        potentialResult.fUsesBestCharset = bool (lplf->lfCharSet == theANSILogFont.lfCharSet);
    }
    else {
        potentialResult.fUsesBestCharset = bool (lplf->lfCharSet == potentialResult.fDesiredCharset);
    }
    potentialResult.fIsTT                = bool (fontType & TRUETYPE_FONTTYPE);
    potentialResult.fIsANSI_VAR_Font     = ::_tcscmp (lplf->lfFaceName, theANSILogFont.lfFaceName) == 0;
    potentialResult.fIsGoodBackupCharset = (lplf->lfCharSet == DEFAULT_CHARSET or lplf->lfCharSet == theANSILogFont.lfCharSet);
    {
        switch (potentialResult.fDesiredCharset) {
            case SHIFTJIS_CHARSET: {
                if (
                    potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MS P Gothic")) or
                    potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MS Gothic")) or
                    potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MS PGothic"))) {
                    potentialResult.fIsFavoriteForCharset = true;
                }
            } break;
            case CHINESEBIG5_CHARSET: {
                if (
                    potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MS HEI")) or
                    potentialResult.fBestFont.lfFaceName == Led_SDK_String (Led_SDK_TCHAROF ("MingLiU"))) {
                    potentialResult.fIsFavoriteForCharset = true;
                }
            } break;
        }
    }
    potentialResult.fIsVariablePitch = lplf->lfPitchAndFamily & VARIABLE_PITCH;
    potentialResult.fStartsWithAt    = lplf->lfFaceName[0] == '@';

    if (potentialResult.Score () > result.Score ()) {
        result = potentialResult;
    }
    return 1;
}
#endif
Led_FontSpecification TextImager::GetStaticDefaultFont ()
{
    //  Since this can be called alot, and since its value shouldn't change during the lifetime
    //  of Led running, cache the result (and since on windows - at least - it is expensive to compute)
    static bool                  sDefaultFontValid = false;
    static Led_FontSpecification sDefaultFont;
    if (not sDefaultFontValid) {
#if qPlatform_MacOS
        sDefaultFont.SetFontNameSpecifier (::GetScriptVariable (smCurrentScript, smScriptAppFond));
        sDefaultFont.SetPointSize (::GetScriptVariable (smCurrentScript, smScriptAppFondSize));
        sDefaultFont.SetStyle_Plain ();
#elif qPlatform_Windows
        sDefaultFont = GetStaticDefaultFont (DEFAULT_CHARSET);
#elif qXWindows
        {
            sDefaultFont.SetFontNameSpecifier ("times");
            sDefaultFont.SetPointSize (12);
        }
#endif
        sDefaultFont.SetTextColor (Led_GetTextColor ());
        sDefaultFontValid = true;
    }
    return (sDefaultFont);
}

#if qPlatform_Windows
Led_FontSpecification TextImager::GetStaticDefaultFont (BYTE charSet)
{
    Led_FontSpecification defaultFont;
    //nb: import to go through the intermediary font so we don't set into the
    // LOGFONT lots of fields which are part of the chosen font but are
    // extraneous, and then mess up later choices to change to face name.
    //
    // Eg., if our default font is BOLD, that will result in a big weight# being part of
    // the logFont. But then if the user picks a different face name, we don't want it
    // to stay bold. Maybe BOLD is a bad example cuz that DOES show up in our UI (menu of
    // font attriobutes). But pick one attribute (width? or escarpment) which doesn't show
    // up in our choice lists, and yet gets caried along even after you change face names.
    //
    // See spr# 0426 for more details.
    Led_FontSpecification fooo;
    FontSelectionInfo     selectedFont (charSet);
    Led_WindowDC          screenDC (nullptr);
#if defined(STRICT)
    ::EnumFontFamilies (screenDC.m_hDC, nullptr, EnumFontCallback, reinterpret_cast<LPARAM> (&selectedFont));
#else
    ::EnumFontFamilies (screenDC.m_hDC, nullptr, reinterpret_cast<FONTENUMPROC> (EnumFontCallback), reinterpret_cast<LPARAM> (&selectedFont));
#endif
    fooo.LightSetOSRep (selectedFont.fBestFont);

    // EnumFontFamilies seems to pick very bad sizes. Not sure why. No biggie. This works
    // pretty well. LGP 970613
    {
        static LOGFONT theANSILogFont;
        if (theANSILogFont.lfFaceName[0] == '\0') {
            HFONT xxx = HFONT (::GetStockObject (ANSI_VAR_FONT));
            Verify (::GetObject (xxx, sizeof theANSILogFont, &theANSILogFont));
        }
        fooo.SetPointSize (min (max (Led_FontSpecification (theANSILogFont).GetPointSize (), static_cast<unsigned short> (8)), static_cast<unsigned short> (14)));
    }

    defaultFont.MergeIn (fooo);
    defaultFont.SetTextColor (Led_GetTextColor ());
    return (defaultFont);
}
#endif

/*
@METHOD:        TextImager::GetDefaultSelectionFont
@DESCRIPTION:   <p>Some applications - such as updating the font of the IME - need to know the
            current selections font. For some @'TextImager's this is easy - its just the font used
            for the entire widget (or default implementation). But for others - say an imager with
            multiple runs of fonts - it can be ambiguous. There is no strictly <em>right</em> answer
            for what to return if there are overlapping runs of font information. Just return the best
            fit possible.
                </p>
                <p>The default implementation just returns @'TextImager::GetDefaultFont', but
            should be subclassed by @'StandardStyledTextImager::GetDefaultSelectionFont' to reflect just
            the selection's current font.
            </p>
*/
Led_FontSpecification TextImager::GetDefaultSelectionFont () const
{
    return GetDefaultFont ();
}

void TextImager::SetSelectionShown (bool shown)
{
    fSelectionShown = shown;
}

/*
@METHOD:        TextImager::GetTabStopList
@DESCRIPTION:   <p>Return the tabstop list (@'TextImager::TabStopList') active at a particular position in the text. Typically this will
    apply to an entire region (often a paragraph/partitionmarker). And we only need specify
    one point in that range.</p>
        <p>Override this to provide a different tabstop list. The default is a @'TextImager::SimpleTabStopList' of width
    roughly 1/3 of an inch.</p>
*/
const TextImager::TabStopList& TextImager::GetTabStopList (size_t /*containingPos*/) const
{
    // 1/3 inch tabstops by default (roughly 4 chars wide on Mac in Courier 10-point)
    static SimpleTabStopList sDefaultTabStopList = SimpleTabStopList (Led_TWIPS (1440 / 3));
    return sDefaultTabStopList;
}

/*
@METHOD:        TextImager::SetWindowRect
@DESCRIPTION:   <p>See also @'TextImager::GetWindowRect'.
*/
void TextImager::SetWindowRect (const Led_Rect& windowRect)
{
    SetWindowRect_ (windowRect);
}

/*
@METHOD:        TextImager::ScrollSoShowingHHelper
@DESCRIPTION:   <p>You probably should NOT call this directly. This is a helper to share code in implementing
    @'TextImager::ScrollSoShowing' in subclasses.</p>
*/
void TextImager::ScrollSoShowingHHelper (size_t markerPos, size_t andTryToShowMarkerPos)
{
    Led_Coordinate maxHScrollPos = ComputeMaxHScrollPos ();
    Led_Coordinate hsp           = GetHScrollPos ();

    /*
     *  Speed tweek - avoid alot of computations which are unneeded if this is true.
     */
    if (maxHScrollPos == 0) {
        if (hsp != 0) {
            SetHScrollPos (0);
        }
        return;
    }

    Led_Rect windowRect = GetWindowRect ();

    {
        // Try to see if we can accomodate the 'andTryToShowMarkerPos'.
        Led_Rect       andTryRRR             = GetCharWindowLocation (andTryToShowMarkerPos);
        Led_Coordinate whereAtInGlobalCoords = windowRect.left - andTryRRR.left;
        if (andTryRRR.left < windowRect.left) {
            Assert (hsp >= whereAtInGlobalCoords);
            hsp -= whereAtInGlobalCoords;
        }
        else if (andTryRRR.right > windowRect.right) {
            Led_Coordinate howFarOffRight = andTryRRR.right - windowRect.right;
            hsp += howFarOffRight; // now the char should be just barely showing.
            hsp = min (hsp, maxHScrollPos);
        }
    }
    Assert (hsp >= 0);
    Assert (hsp <= maxHScrollPos);

    {
        // Make sure the 'markerPos' is shown. Do this second - in case there is a conflict between 'markerPos' and 'andTryToShowMarkerPos'
        Led_Rect rrr = GetCharWindowLocation (markerPos);

        {
            Led_Coordinate adjustRRRBy = GetHScrollPos () - hsp;
            rrr += Led_Point (0, adjustRRRBy);
        }

        Led_Coordinate whereAtInGlobalCoords = windowRect.GetLeft () - rrr.GetLeft ();
        if (rrr.GetLeft () < windowRect.GetLeft ()) {
            Assert (hsp >= whereAtInGlobalCoords);
            hsp -= whereAtInGlobalCoords;
        }
        else if (rrr.GetRight () > windowRect.GetRight ()) {
            Led_Coordinate howFarOffRight = rrr.GetRight () - windowRect.GetRight ();
            hsp += howFarOffRight; // now the char should be just barely showing.
            hsp = min (hsp, maxHScrollPos);
        }
    }

    Assert (hsp >= 0);
    Assert (hsp <= maxHScrollPos);
    SetHScrollPos (hsp);
}

void TextImager::SetHScrollPos (Led_Coordinate hScrollPos)
{
    if (hScrollPos != GetHScrollPos ()) {
        SetHScrollPos_ (hScrollPos);
        InvalidateScrollBarParameters ();
    }
}

/*
@METHOD:        TextImager::ComputeMaxHScrollPos
@DESCRIPTION:   <p>This routine is used for horizontal scrolling (though not from within this class).
            It is mainly called by @'TextInteractor' or @'Led_Win32' etc methods to handle display/positioning
            of the scrollbar. Subclasses can OVERRIDE this to implement whatever horizontal scrolling
            they might want to.
            </p>
                <p>A plausible OVERRIDE
            of this routine might return (roughly - taking care of min'ing out to zero)
            <code>CalculateLongestRowInWindowPixelWidth () - GetWindowRect ().GetWidth ()</code>
            or perhaps even better:
            <code>CalculateLongestRowInDocumentPixelWidth () - GetWindowRect ().GetWidth ()</code>
            </p>
                <p>Some subclasses - such as @'WordProcessor' - already contain their own implementation
            (@'WordProcessor::ComputeMaxHScrollPos').
            </p>
*/
Led_Distance TextImager::ComputeMaxHScrollPos () const
{
    return 0;
}

/*
@METHOD:        TextImager::CalculateLongestRowInWindowPixelWidth
@DESCRIPTION:   <p>This is a utility methods, very handy for implementing horizontal scrolling.
    It can (and should be) overriden in certain subclasses for efficiency. But the default implementation will work.</p>
*/
Led_Distance TextImager::CalculateLongestRowInWindowPixelWidth () const
{
    size_t startOfWindow = GetMarkerPositionOfStartOfWindow ();
    size_t endOfWindow   = GetMarkerPositionOfEndOfWindow ();

    Led_Distance longestRowWidth = 0;
    for (size_t curOffset = startOfWindow; curOffset < endOfWindow;) {
        Led_Distance thisRowWidth = CalcSegmentSize (curOffset, GetEndOfRowContainingPosition (curOffset));
        longestRowWidth           = max (longestRowWidth, thisRowWidth);
        {
            size_t newOffset = GetStartOfNextRowFromRowContainingPosition (curOffset);
            if (newOffset <= curOffset) {
                break; // can happen at end of doc...
            }
            curOffset = newOffset;
        }
    }
    return longestRowWidth;
}

/*
@METHOD:        TextImager::TabletChangedMetrics
@DESCRIPTION:   <p>Call this method when something external, Led cannot detect, has happened to the tablet which would
    invalidate any information the @'TextImager' has cached. This is called automaticly, internal to Led, by anything
    Led knows about which would change the metrics.</p>
*/
void TextImager::TabletChangedMetrics ()
{
    InvalidateAllCaches ();
}

void TextImager::SetSelection (size_t start, size_t end)
{
    Assert (start >= 0);
    Assert (end <= GetEnd ()); // char 1 between positions 1..2
#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (start);
    Assert_CharPosDoesNotSplitCharacter (end);
#endif

#if 0
    DbgTrace ("TextImager::SetSelection (this= 0x%x, this_class = %s, oldSelStart=%d, oldSelEnd=%d, newSelStart=%d, newSelEnd=%d)\n",
              this, typeid (*this).name (), GetSelectionStart (), GetSelectionEnd (), start, end
             );
#endif

    if (start != GetSelectionStart () or end != GetSelectionEnd ()) {
        SetSelection_ (start, end);
    }
}

size_t TextImager::GetSelectionStart () const
{
    RequireNotNull (PeekAtTextStore ()); // Must specify TextStore before calling this, or any routine that calls it.
#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (fHiliteMarker->GetStart ());
#endif
    return (fHiliteMarker->GetStart ());
}

size_t TextImager::GetSelectionEnd () const
{
    RequireNotNull (PeekAtTextStore ()); // Must specify TextStore before calling this, or any routine that calls it.
#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (fHiliteMarker->GetEnd ());
#endif
    return (fHiliteMarker->GetEnd ());
}

void TextImager::GetSelection (size_t* start, size_t* end) const
{
    RequireNotNull (PeekAtTextStore ()); // Must specify TextStore before calling this, or any routine that calls it.
    AssertNotNull (start);
    AssertNotNull (end);
    fHiliteMarker->GetRange (start, end);
#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (*start);
    Assert_CharPosDoesNotSplitCharacter (*end);
#endif
}

void TextImager::SetSelection_ (size_t start, size_t end)
{
    Require (start >= 0);
    Require (end <= GetEnd ());
    Require (start <= end);
    GetTextStore ().SetMarkerRange (fHiliteMarker, start, end);
}

void TextImager::SetHilightMarker (HilightMarker* newHilightMarker)
{
    size_t start = 0;
    size_t end   = 0;
    if (fHiliteMarker != nullptr) {
        fHiliteMarker->GetRange (&start, &end);
        GetTextStore ().RemoveMarker (fHiliteMarker);
        if (fWeAllocedHiliteMarker) {
            delete fHiliteMarker;
        }
    }
    fHiliteMarker          = newHilightMarker;
    fWeAllocedHiliteMarker = bool (fHiliteMarker == nullptr);
    if (fHiliteMarker == nullptr) {
        fHiliteMarker = new HilightMarker ();
    }
    AssertNotNull (fHiliteMarker);
    GetTextStore ().AddMarker (fHiliteMarker, start, end - start, this);
}

/*
@METHOD:        TextImager::RecomputeSelectionGoalColumn
@DESCRIPTION:   <p></p>
*/
void TextImager::RecomputeSelectionGoalColumn ()
{
    if (not fSuppressGoalColumnRecompute) {
        // We now maintain a goal-column-target using pixel offsets within the row, rather than
        // character offsets, cuz thats what LEC/Alan Pollack prefers, and I think most
        // Texteditors seem todo likewise - LedSPR#0315
        Led_Distance lhs = 0;
        Led_Distance rhs = 0;
        GetRowRelativeCharLoc (GetSelectionStart (), &lhs, &rhs);
        SetSelectionGoalColumn (Tablet_Acquirer (this)->CvtToTWIPSH (lhs + (rhs - lhs) / 2));
    }
}

/*
@METHOD:        TextImager::ComputeRelativePosition
@DESCRIPTION:   <p></p>
*/
size_t TextImager::ComputeRelativePosition (size_t fromPos, CursorMovementDirection direction, CursorMovementUnit movementUnit)
{
    /*
     *  Handle all the different cases of movement directions (back, forward etc) and units (by char, word etc).
     *  Take the given starting point, and produce no side effects - returning the new resulting position.
     */
    switch (direction) {
        case eCursorBack: {
            switch (movementUnit) {
                case eCursorByChar: {
                    return (FindPreviousCharacter (fromPos));
                } break;

                case eCursorByWord: {
                    return (GetTextStore ().FindFirstWordStartStrictlyBeforePosition (fromPos));
                } break;

                case eCursorByRow: {
                    size_t startOfStartRow = GetStartOfRowContainingPosition (fromPos);
                    size_t startOfPrevRow  = GetStartOfPrevRowFromRowContainingPosition (fromPos);
                    if (startOfStartRow == startOfPrevRow) {
                        // no change
                        return (fromPos);
                    }
                    else {
                        return GetRowRelativeCharAtLoc (Tablet_Acquirer (this)->CvtFromTWIPSH (GetSelectionGoalColumn ()), startOfPrevRow);
                    }
                } break;

                case eCursorByLine: {
                    size_t fromLine = GetTextStore ().GetLineContainingPosition (fromPos);
                    size_t newLine  = (fromLine > 0) ? fromLine - 1 : 0;
                    if (newLine == fromLine) {
                        // no change
                        return (fromPos);
                    }
                    else {
                        // try to maintain the same horizontal position across lines
                        size_t positionInLine = fromPos - GetTextStore ().GetStartOfLine (fromLine); // ZERO RELATIVE
                        Assert (positionInLine <= GetTextStore ().GetLineLength (fromLine));
                        positionInLine = min (positionInLine, GetTextStore ().GetLineLength (newLine)); // don't go past end of new line...
#if qMultiByteCharacters
                        // Don't split a mbyte character
                        Memory::SmallStackBuffer<Led_tChar> buf (positionInLine);
                        CopyOut (GetTextStore ().GetStartOfLine (newLine), positionInLine, buf);
                        if (Led_FindPrevOrEqualCharBoundary (buf, buf + positionInLine) != buf + positionInLine) {
                            Assert (positionInLine > 0);
                            positionInLine--;
                        }
#endif
                        return (GetTextStore ().GetStartOfLine (newLine) + positionInLine);
                    }
                } break;

                case eCursorByWindow: {
                    Assert (false); // nyi
                } break;

                case eCursorByBuffer: {
                    Assert (false); // makes no sense - use toStart...
                } break;

                default:
                    Assert (false);
            }
        } break;

        case eCursorForward: {
            switch (movementUnit) {
                case eCursorByChar: {
                    return (FindNextCharacter (fromPos));
                } break;

                case eCursorByWord: {
                    TextStore& ts = GetTextStore ();
                    return (ts.FindFirstWordStartAfterPosition (ts.FindNextCharacter (fromPos)));
                } break;

                case eCursorByRow: {
                    size_t startOfStartRow = GetStartOfRowContainingPosition (fromPos);
                    size_t startOfNextRow  = GetStartOfNextRowFromRowContainingPosition (fromPos);
                    if (startOfStartRow == startOfNextRow) {
                        // no change
                        return (fromPos);
                    }
                    else {
                        return GetRowRelativeCharAtLoc (Tablet_Acquirer (this)->CvtFromTWIPSH (GetSelectionGoalColumn ()), startOfNextRow);
                    }
                } break;

                case eCursorByLine: {
                    size_t fromLine = GetTextStore ().GetLineContainingPosition (fromPos);
                    size_t newLine  = (fromLine == GetTextStore ().GetLineCount () - 1) ? fromLine : (fromLine + 1);
                    Assert (newLine <= GetTextStore ().GetLineCount () - 1);
                    if (newLine == fromLine) {
                        // no change
                        return (fromPos);
                    }
                    else {
                        // try to maintain the same horizontal position across rows
                        size_t positionInLine = fromPos - GetTextStore ().GetStartOfLine (fromLine); // ZERO RELATIVE
                        Assert (positionInLine <= GetTextStore ().GetLineLength (fromLine));
                        positionInLine = min (positionInLine, GetTextStore ().GetLineLength (newLine)); // don't go past end of new line...
                        return (GetTextStore ().GetStartOfLine (newLine) + positionInLine);
                    }
                } break;

                case eCursorByWindow: {
                    Assert (false); // nyi
                } break;

                case eCursorByBuffer: {
                    Assert (false); // makes no sense - use eCursorToEnd...
                } break;

                default:
                    Assert (false);
            }
        } break;

        case eCursorToStart: {
            switch (movementUnit) {
                case eCursorByChar: {
                    Assert (false); // to start of character - does this make sense???
                } break;

                case eCursorByWord: {
                    return (GetTextStore ().FindFirstWordStartStrictlyBeforePosition (FindNextCharacter (fromPos)));
                } break;

                case eCursorByRow: {
                    size_t fromRow = GetRowContainingPosition (fromPos);
                    return (GetStartOfRow (fromRow));
                } break;

                case eCursorByLine: {
                    size_t result = GetTextStore ().GetStartOfLineContainingPosition (fromPos);
                    if (fromPos == result) {
                        result = GetTextStore ().GetStartOfLineContainingPosition (GetTextStore ().FindPreviousCharacter (result));
                    }
                    return result;
                } break;

                case eCursorByWindow: {
                    return (GetMarkerPositionOfStartOfWindow ());
                } break;

                case eCursorByBuffer: {
                    return (0);
                } break;

                default:
                    Assert (false);
            }
        } break;

        case eCursorToEnd: {
            switch (movementUnit) {
                case eCursorByChar: {
                    Assert (false); // to start of character - does this make sense???
                } break;

                case eCursorByWord: {
                    return (GetTextStore ().FindFirstWordEndAfterPosition (fromPos));
                } break;

                case eCursorByRow: {
                    size_t fromRow = GetRowContainingPosition (fromPos);
                    return (GetEndOfRow (fromRow));
                } break;

                case eCursorByLine: {
                    size_t result = GetTextStore ().GetEndOfLineContainingPosition (fromPos);
                    if (fromPos == result) {
                        result = GetTextStore ().GetEndOfLineContainingPosition (GetTextStore ().FindNextCharacter (result));
                    }
                    return result;
                } break;

                case eCursorByWindow: {
                    return (GetMarkerPositionOfEndOfWindow ());
                } break;

                case eCursorByBuffer: {
                    return (GetTextStore ().GetEnd ());
                } break;

                default:
                    Assert (false);
            }
        } break;

        default:
            Assert (false);
    }
    Assert (false); // not reached...
    return (fromPos);
}

/*
@METHOD:        TextImager::GetTextWindowBoundingRect
@DESCRIPTION:   <p>GetTextWindowBoundingRect () return a @'Led_Rect' which bounds
    the characters defined by the given marker positions. The will fit nicely around the
    characters if they all fit in one row (and one directional run), but may have sluff around the left/right sides
    if the range crosses row boundaries or directional runs (since the shape wouldn't be a rectangle, but a region -
    try GetSelectionWindowRegion () for that).</p>
        <p>This function operates on CharacterCells.</p>
        <p>This function operates in Window coordinates (ie window relative, calling @'TextImager::GetCharWindowLocation').</p>
        <p>Return value is pinned to be within the WindowRect.</p>
        <p>See also @'TextImager::GetIntraRowTextWindowBoundingRect'</p>
*/
Led_Rect TextImager::GetTextWindowBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const
{
    Require (fromMarkerPos <= toMarkerPos);

    Led_Rect windowRect = GetWindowRect ();
    Led_Rect r1         = GetCharWindowLocation (fromMarkerPos);
    Led_Rect r2         = r1;
    if (fromMarkerPos != toMarkerPos) {
        r2 = GetCharWindowLocation (FindPreviousCharacter (toMarkerPos));
    }

    size_t realEndOfRowOfFromMarkerPos = GetRealEndOfRowContainingPosition (fromMarkerPos);

    Led_Rect boundingRect;
    boundingRect.top    = r1.GetTop ();
    boundingRect.bottom = r2.GetBottom (); // too aggressive??? for case of end of row it is...

    if (realEndOfRowOfFromMarkerPos >= toMarkerPos) {
        /*
         *  One ROW case
         *
         *      This is pretty complicated. We must worry about multiple overlapping runs, and about the
         *  extensions from the start/end of the text to the window borders (depending on the
         *  segmentHilightedAtStart/segmentHilightedAtEnd flags).
         *
         *      I'm not terribly confident this code is all right, but it really doesn't need to be perfect (though
         *  it SHOULD be). Its just important that it returns a rectangle BIG ENOUGH to wrap ALL the releveant text.
         *  Being a little TOO big is only inelegant, and not tragic.
         *
         *      See SPR#1237 for some details (and a test case).
         */

        // A bit of a sloppy hack to make sure any drawing to the right or left of the text (up to the margin)
        // gets erased as well.
        size_t startOfRow              = GetStartOfRowContainingPosition (fromMarkerPos);
        size_t endOfRow                = GetEndOfRowContainingPosition (fromMarkerPos);
        bool   segmentHilightedAtStart = (fromMarkerPos == startOfRow);
        bool   segmentHilightedAtEnd   = endOfRow < toMarkerPos;

        boundingRect.left  = min (r1.GetLeft (), r2.GetLeft ());
        boundingRect.right = max (r1.GetRight (), r2.GetRight ());

        /*
         *
         *  Trouble is we could have something like:
         *      1 2 7 8 3 4 5 6 9
         *  and if I select from 6 to 9, I'll get just the right side of the row, and miss the
         *  characters from 7-8.
         *  Really I want all the way from the LHS of 7 to the RHS of 9 in this case.
         */
        TextLayoutBlock_Copy text = GetTextLayoutBlock (startOfRow, endOfRow);

        using ScriptRunElt        = TextLayoutBlock::ScriptRunElt;
        vector<ScriptRunElt> runs = text.GetScriptRuns ();
        if (runs.size () > 1) {
            // Only needed if there are nested runs...
            for (auto i = runs.begin (); i != runs.end (); ++i) {
                const ScriptRunElt& se = *i;
                if (TextStore::Overlap (startOfRow + se.fRealStart, startOfRow + se.fRealEnd, fromMarkerPos, toMarkerPos)) {
                    /*
                    *   OK - there is SOME overlap between this run and the [fromMarkerPos, toMarkerPos] range.
                    *
                    *   Now see if the run endpoints are in the from/to range, and if so, then extend the
                    *   bounding rectangle to accomodate them.
                    */
                    if (fromMarkerPos <= startOfRow + se.fRealStart and startOfRow + se.fRealStart <= toMarkerPos) {
                        Led_Rect t1        = GetCharWindowLocation (startOfRow + se.fRealStart);
                        boundingRect.left  = min (boundingRect.left, t1.GetLeft ());
                        boundingRect.right = max (boundingRect.right, t1.GetRight ());
                    }

                    if (fromMarkerPos <= startOfRow + se.fRealEnd and startOfRow + se.fRealEnd <= toMarkerPos) {
                        Led_Rect t2        = GetCharWindowLocation (FindPreviousCharacter (startOfRow + se.fRealEnd));
                        boundingRect.left  = min (boundingRect.left, t2.GetLeft ());
                        boundingRect.right = max (boundingRect.right, t2.GetRight ());
                    }
                }
            }
        }

        if (segmentHilightedAtStart) {
            boundingRect.left = windowRect.left;
        }
        if (segmentHilightedAtEnd) {
            boundingRect.right = windowRect.right;
        }
    }
    else {
        /*
         *  Two or more ROWS case
         */
        boundingRect.left  = windowRect.left;
        boundingRect.right = windowRect.right;
    }

    // pin the results to be within the boundingRect (left and right)
    boundingRect.left  = max (boundingRect.left, windowRect.left);
    boundingRect.right = min (boundingRect.right, windowRect.right);

    Ensure (boundingRect.right >= boundingRect.left);
    return (boundingRect);
}

/*
@METHOD:        TextImager::GetIntraRowTextWindowBoundingRect
@ACCESS:        public
@DESCRIPTION:   <p>GetIntraRowTextWindowBoundingRect () return a Rect which bounds
            the characters defined by the given marker positions. The marker positions are required to fall
            within a single row.
                <p>This function operates on CharacterCells.</p>
                <p>This function operates in Window coordinates (ie the same coordinates as the WindowRect,
            is specified in - calling @'TextImager::GetCharWindowLocation').</p>
                <p>See also @'TextImager::GetTextBoundingRect',
            @'TextImager::GetTextWindowBoundingRect' and @'TextImager::GetIntraRowTextWindowBoundingRect'</p>
                <p>You CAN call this function with any range of 'fromMarkerPos' to 'toMarkerPos' within a row, but it only REALLY
            makes sense if you call it within a directional segment.</p>
*/
Led_Rect TextImager::GetIntraRowTextWindowBoundingRect (size_t fromMarkerPos, size_t toMarkerPos) const
{
    Require (fromMarkerPos <= toMarkerPos); // and they must be within the same row!!! Assert later...

    Led_Rect windowRect = GetWindowRect ();

    /*
     *  Note that we could use one side of one character and the same side of the other (e.g. for LTR text
     *  the left side of the first and second charaters). The trouble with this is that the character at 'toMarkerPos'
     *  can sometimes be on another line - not RIGHT after the one we are interested in. Furthermore - now that
     *  we support BIDI text - it could be left or right or almost anywhere if it was the start of another run.
     *
     *  So - instead - we measure (taking the LTR case) from the LEFT side of the 'fromMarkerPos' character
     *  to the RIGHT side of the character PRECEDING the 'toMarkerPos' character (with the special case of when the two
     *  markerPos positions are the same.
     */
    Led_Rect r1 = GetCharWindowLocation (fromMarkerPos);
    Led_Rect r2 = r1;
    if (fromMarkerPos != toMarkerPos) {
        r2 = GetCharWindowLocation (FindPreviousCharacter (toMarkerPos));
    }

    Led_Rect boundingRect = r1;

    if (GetTextDirection (fromMarkerPos) == eLeftToRight) {
        boundingRect.left  = r1.GetLeft ();
        boundingRect.right = (fromMarkerPos == toMarkerPos) ? boundingRect.left : r2.GetRight ();
    }
    else {
        boundingRect.right = r1.GetRight ();
        boundingRect.left  = (fromMarkerPos == toMarkerPos) ? boundingRect.right : r2.GetLeft ();
    }
    Ensure (boundingRect.right >= boundingRect.left); // If this is triggered, its probably cuz your from/to crossed
    // a directional segment boundary?
    return (boundingRect);
}

/*
@METHOD:        TextImager::GetRowHilightRects
@ACCESS:        public
@DESCRIPTION:   <p>Compute the subregion of the row demarcated by 'rowStart' and 'rowEnd' which interesects with the given
            'hilightStart' and 'hilightEnd'. If the hilights extend past the row (either start or end) this is OK- and the
            hilight rectangle list is adjusted accordingly to display that extended hilgiht
            (if @'TextImager::GetUseSelectEOLBOLRowHilightStyle' is set).</p>
                <p>This routine is called by @'TextImager::GetSelectionWindowRegion' and @'TextImager::DrawRowHilight' to figure out what areas
            of the screen to hilight.</p>
*/
vector<Led_Rect> TextImager::GetRowHilightRects (const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, size_t hilightStart, size_t hilightEnd) const
{
    Require (rowEnd == GetEndOfRowContainingPosition (rowStart)); // passed in for performance reasons - so not computed multiple times

    vector<Led_Rect> result;

    size_t realEndOfRow     = GetRealEndOfRowContainingPosition (rowStart);
    bool   segmentHilighted = max (rowStart, hilightStart) < min (realEndOfRow, hilightEnd);
    if (segmentHilighted) {
        bool segmentHilightedAtStart = false;
        bool segmentHilightedAtEnd   = false;
        if (GetUseSelectEOLBOLRowHilightStyle ()) {
            segmentHilightedAtStart = (hilightStart < rowStart) or (hilightStart == 0);
            segmentHilightedAtEnd   = rowEnd < hilightEnd;
            if (segmentHilightedAtEnd and rowEnd >= GetEnd ()) {
                segmentHilightedAtEnd = false; // last row always contains no NL - so no invert off to the right...
            }
        }

        hilightStart = max (hilightStart, rowStart);

        using ScriptRunElt        = TextLayoutBlock::ScriptRunElt;
        vector<ScriptRunElt> runs = text.GetScriptRuns ();
        //NB: it doesn't matter what order we iterate over the blocks and draw their hilight
        for (auto i = runs.begin (); i != runs.end (); ++i) {
            const ScriptRunElt& se        = *i;
            size_t              hRunStart = max (se.fRealStart + rowStart, hilightStart);
            size_t              hRunEnd   = min (se.fRealEnd + rowStart, hilightEnd);
            if (hRunStart < hRunEnd) {
                Led_Rect hilightRect = GetIntraRowTextWindowBoundingRect (hRunStart, hRunEnd);
                Assert (hilightRect.GetWidth () >= 0);
                Assert (hilightRect.GetHeight () >= 0);
                if (not hilightRect.IsEmpty ()) { // don't add empty rectangles
                    result.push_back (hilightRect);
                }
            }
        }

        // Add extra rects before or after all the text to the start/end of the row, as needed
        if (segmentHilightedAtStart) {
            size_t realOffsetOfVirtualRowStart = rowStart;
            if (text.GetTextLength () != 0) {
                realOffsetOfVirtualRowStart += text.MapVirtualOffsetToReal (0);
            }
            // Make sure the 'segmentHilightAtStart is drawn even if the selection only comes up to just before first char in row
            Led_Rect hilightRect = GetCharWindowLocation (realOffsetOfVirtualRowStart);
            hilightRect.right    = hilightRect.left;
            hilightRect.left     = min (GetWindowRect ().GetLeft (), hilightRect.left);
            Assert (hilightRect.GetWidth () >= 0);
            Assert (hilightRect.GetHeight () >= 0);
            if (not hilightRect.IsEmpty ()) { // don't add empty rectangles
                result.push_back (hilightRect);
            }
        }
        if (segmentHilightedAtEnd) {
            size_t realOffsetOfVirtualRowEnd = rowStart;
            if (text.GetTextLength () != 0) {
                realOffsetOfVirtualRowEnd += text.MapVirtualOffsetToReal (text.GetTextLength () - 1);
            }
            // Make sure the 'segmentHilightAtStart is drawn even if the selection only comes up to just before first char in row
            Led_Rect hilightRect = GetCharWindowLocation (realOffsetOfVirtualRowEnd);
            hilightRect.left     = hilightRect.GetRight ();
            hilightRect.right    = max (hilightRect.right, GetWindowRect ().GetRight ());
            Assert (hilightRect.GetWidth () >= 0);
            Assert (hilightRect.GetHeight () >= 0);
            if (not hilightRect.IsEmpty ()) { // don't add empty rectangles
                result.push_back (hilightRect);
            }
        }
    }

#if qDebug
    {
        // Make sure rectangles don't overlap with one another (can share an edge) -- SPR#1226
        for (auto orit = result.begin (); orit != result.end (); ++orit) {
            Ensure ((*orit).GetWidth () > 0);
            Ensure ((*orit).GetHeight () > 0);
            for (auto irit = orit + 1; irit != result.end (); ++irit) {
                Led_Rect hr = *irit;
                Ensure (hr.GetWidth () > 0);
                Ensure (hr.GetHeight () > 0);
                Ensure (not Intersect (hr, *orit));
            }
        }
    }
#endif
    return result;
}

/*
@METHOD:        TextImager::GetTextLayoutBlock
@ACCESS:        public
@DESCRIPTION:   <p>REQUIRE that rowStart and rowEnd are valid rowstart/end values</p>
*/
TextLayoutBlock_Copy TextImager::GetTextLayoutBlock (size_t rowStart, size_t rowEnd) const
{
    size_t                              rowLen = rowEnd - rowStart;
    Memory::SmallStackBuffer<Led_tChar> rowBuf (rowLen);
    CopyOut (rowStart, rowLen, rowBuf);
    TextLayoutBlock_Basic text (rowBuf, rowBuf + rowLen);
    return TextLayoutBlock_Copy (text);
}

/*
@METHOD:        TextImager::GetSelectionWindowRects
@ACCESS:        public
@DESCRIPTION:   <p>Compute the window-relative region (list of rectangles) bounding the
            given segment of text. This is useful for displaying some sort of text hilight,
            in addition (or apart from) the standard hilighting of text.</p>
                <p>This function uses @'TextImager::GetRowHilightRects' to figure out what areas of the
            screen to hilight. This routine also tries to take into account interline space by
            extending the hilight from succeeding rows back to the bottom of the preceeding row.</p>
                <p>See also @'TextImager::GetSelectionWindowRegion'</p>
*/
vector<Led_Rect> TextImager::GetSelectionWindowRects (size_t from, size_t to) const
{
    Require (from <= to);

    vector<Led_Rect> result;

    from = max (from, GetMarkerPositionOfStartOfWindow ());
    to   = min (to, FindNextCharacter (GetMarkerPositionOfEndOfWindow ()));

    if (from >= to) {
        return result;
    }
    Assert (from < to);

    size_t topRow    = GetRowContainingPosition (from);
    size_t bottomRow = GetRowContainingPosition (to);
    Assert (topRow <= bottomRow);

    // If to is at the start of a row (remember - we wanted the to select char UP-TO that
    // MARKER POS) then we've gone one row too far
    if (GetStartOfRow (bottomRow) == to) {
        // then use end of previous row
        Assert (topRow < bottomRow);
        bottomRow--;
    }

    Led_Coordinate lastRowBottom = 0; // Keep track of last row's bottom for interline-space support
    for (size_t curRow = topRow;;) {
        size_t firstCharInRow = from;
        if (topRow != curRow) {
            firstCharInRow = GetStartOfRow (curRow);
        }

        size_t startOfRow = GetStartOfRowContainingPosition (firstCharInRow);
        size_t endOfRow   = GetEndOfRowContainingPosition (startOfRow);
#if 1
        TextLayoutBlock_Copy text = GetTextLayoutBlock (startOfRow, endOfRow);
#else
        size_t rowLen = endOfRow - startOfRow;
        Memory::SmallStackBuffer<Led_tChar> rowBuf (rowLen);
        CopyOut (startOfRow, rowLen, rowBuf);
        TextLayoutBlock_Basic text (rowBuf, rowBuf + rowLen);
#endif

        vector<Led_Rect> hilightRects = GetRowHilightRects (text, startOfRow, endOfRow, GetSelectionStart (), GetSelectionEnd ());
        Led_Coordinate   newMinTop    = lastRowBottom;
        Led_Coordinate   newMaxBottom = lastRowBottom;
        for (auto i = hilightRects.begin (); i != hilightRects.end (); ++i) {
            Led_Rect hilightRect = *i;
            Require (hilightRect.GetWidth () >= 0);
            Assert (hilightRect.GetHeight () > 0);
            if (not hilightRect.IsEmpty ()) {
                result.push_back (hilightRect);
            }
            newMinTop    = min (newMinTop, hilightRect.top);
            newMaxBottom = max (newMaxBottom, hilightRect.bottom);
        }

        /*
         *  Now that we've kept that lastRowBottom and this emitted rows minTop and maxBottom, we can
         *  see if there is any gap between the lastRowBottom and the newMinTop. If yes, then fill
         *  that gap.
         */
        if (lastRowBottom < newMinTop) {
            // Compute this hScrollAdjustedWR inside loop since it should happen extremely rarely, and doing it
            // outside would mean it gets called more (cuz we can only do the test inside).
            Led_Rect hScrollAdjustedWR = GetWindowRect () - Led_Point (0, GetHScrollPos ());
            result.push_back (Led_Rect (lastRowBottom, hScrollAdjustedWR.GetLeft (), newMinTop - lastRowBottom, hScrollAdjustedWR.GetWidth ()));
        }
        lastRowBottom = newMaxBottom;

        if (curRow == bottomRow) {
            break;
        }
        curRow++;
    }

#if qDebug
    {
        // Make sure rectangles don't overlap with one another (can share an edge) -- SPR#1226
        for (auto orit = result.begin (); orit != result.end (); ++orit) {
            Ensure ((*orit).GetWidth () > 0);
            Ensure ((*orit).GetHeight () > 0);
            for (auto irit = orit + 1; irit != result.end (); ++irit) {
                Led_Rect hr = *irit;
                Ensure (hr.GetWidth () > 0);
                Ensure (hr.GetHeight () > 0);
                Ensure (not Intersect (hr, *orit));
            }
        }
    }
#endif
    return result;
}

/*
@METHOD:        TextImager::GetSelectionWindowRegion
@ACCESS:        public
@DESCRIPTION:   <p>Figure the region bounding the given segment of text. Useful for displaying
            some sort of text hilight, in addition (or apart from) the standard hilighting
            of text. Note we use a VAR parameter for the region rather than returing it
            cuz MFC's CRgn class doesn't support being copied.</p>
                <p>This routine is a simple wrapper on @'TextImager::GetSelectionWindowRects'</p>
*/
void TextImager::GetSelectionWindowRegion (Led_Region* r, size_t from, size_t to) const
{
    RequireNotNull (r);
    vector<Led_Rect> selRects = GetSelectionWindowRects (from, to);
    for (auto i = selRects.begin (); i != selRects.end (); ++i) {
        AddRectangleToRegion (*i, r);
    }
}

/*
@METHOD:        TextImager::EraseBackground
@DESCRIPTION:   <p>EraseBackground () is called internally by TextImagers to get arbitrary subsets of the current
    window being drawn to erased. This erasure must only apply to the argument 'subsetToDraw'. Clipping is
    not used to gaurantee this. Failure to follow that rule can produce undesirably results where other bits of
    surrounding text get erased.</p>
        <p>The erasebackground call is made virtual so that you can use this as a hook to provide some sort
    of multi-media, picture or whatever as your background. Note that the is called during the draw
    process as a side-effect. So to have a changing background, you would need to both force periodic
    updates, and OVERRIDE this routine.</p>
        <p>NB: the argument 'subsetToDraw' can occasionally be outside of the WindowRect. If so - then DO draw where this
    function says to draw. This is because you maybe asked to erase window margins/borders outside the Led 'WindowRect' using
    the same color/algorithm as that used inside the Led WindowRect.</p>
        <p>By default - this simply calls @'Led_Tablet_::EraseBackground_SolidHelper' with @'Led_GetTextBackgroundColor' ().</p>
        <p>Note - typically when you OVERRIDE this - you will want to OVERRIDE @'TextImager::HilightARectangle' to change
    its implementation to specify a new background color (so inverting works properly). Perhaps see SPR#0708 for details.</p>
*/
void TextImager::EraseBackground (Led_Tablet tablet, const Led_Rect& subsetToDraw, bool printing)
{
    RequireNotNull (tablet);
    // Don't erase when printing - at least by default. Tends to screw up most print drivers.
    if (not printing) {
        tablet->EraseBackground_SolidHelper (subsetToDraw, GetEffectiveDefaultTextColor (TextImager::eDefaultBackgroundColor));
    }
}

/*
@METHOD:        TextImager::HilightArea
@DESCRIPTION:   <p>Hilight the given rectangle of the screen, after its been drawn. This is typically done via
    some sort of pixel or color invesion. The default implemtation uses @'Led_Tablet_::HilightArea_SolidHelper'.</p>
        <p>Override this mostly if you want different hilighting behavior, or if you want your hilighting behavior
    to remain in sync with other changes to the EraseBackground behavior.</p>
*/
void TextImager::HilightArea (Led_Tablet tablet, Led_Rect hiliteArea)
{
    RequireNotNull (tablet);
    tablet->HilightArea_SolidHelper (hiliteArea, GetEffectiveDefaultTextColor (eDefaultSelectedTextBackgroundColor), GetEffectiveDefaultTextColor (eDefaultSelectedTextColor), GetEffectiveDefaultTextColor (eDefaultBackgroundColor), GetEffectiveDefaultTextColor (eDefaultTextColor));
}

/*
@METHOD:        TextImager::HilightArea
@DESCRIPTION:   <p>Hilight the given region of the screen, after its been drawn. This is typically done via
    some sort of pixel or color invesion. The default implemtation uses @'Led_Tablet_::HilightArea_SolidHelper'.</p>
        <p>Override this mostly if you want different hilighting behavior, or if you want your hilighting behavior
    to remain in sync with other changes to the EraseBackground behavior.</p>
*/
void TextImager::HilightArea (Led_Tablet tablet, const Led_Region& hiliteArea)
{
    RequireNotNull (tablet);
    tablet->HilightArea_SolidHelper (hiliteArea, GetEffectiveDefaultTextColor (eDefaultSelectedTextBackgroundColor), GetEffectiveDefaultTextColor (eDefaultSelectedTextColor), GetEffectiveDefaultTextColor (eDefaultBackgroundColor), GetEffectiveDefaultTextColor (eDefaultTextColor));
}

/*
@METHOD:        TextImager::DrawRow
@DESCRIPTION:   <p>Draw the given row of text. Erase the background (by calling @'TextImager::EraseBackground'), and
            then draw the segments of the row by calling @'TextImager::DrawRowSegments'. Then draw any hilighting.
            This routine CAN cause flicker. Flicker is eliminated at a higher level (caller) by using offscreen bitmaps (see @'MultiRowTextImager::Draw'
            or @'TextImager::GetImageUsingOffscreenBitmaps').</p>
                <p>NB: This bit about flicker changed in Led 2.2. Earlier Led versions tried to elminate flicker at all levels
            and avoid offscreen bitmaps. But problems with kerning made this difficult.</p>
                <p>Note, only the invalidRect subset of currentRowRect need be drawn, though the rest CAN be.</p>
                <p>Renamed to @'TextImager::DrawRowSegments' from MutliRowTextImager::DrawRowSegments for Led 3.1a3 release.</p>
*/
void TextImager::DrawRow (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                          const TextLayoutBlock& text, size_t rowStart, size_t rowEnd, bool printing)
{
    RequireNotNull (tablet);
    Require (rowEnd == GetEndOfRowContainingPosition (rowStart)); // passed in for performance reasons - so not computed multiple times

    /*
     *  Could CONSIDER doing something like:
     *
     *      Led_Tablet_::ClipNarrowAndRestore   clipFurtherTo (tablet, currentRowRect);
     *
     *  here, but it might have too much of a performance cost. Perhaps I should test this. See SPR#?????
     */

    EraseBackground (tablet, currentRowRect, printing);

    DrawRowSegments (tablet, currentRowRect, invalidRowRect, text, rowStart, rowEnd);

    /*
     *  Only draw hilighting if we aren't printing, because this doesn't show up well on printers.
     */
    if (not printing) {
        DrawRowHilight (tablet, currentRowRect, invalidRowRect, text, rowStart, rowEnd);
    }
}

/*
@METHOD:        TextImager::DrawRowSegments
@DESCRIPTION:   <p>Called by @'TextImager::DrawRow' to draw (in or mode - don't worry about erasing) all the segments
            which make up the row of text. Here, we take care of any sort of justification, or indending (in subclasses which OVERRIDE
            this method). Its rowRect its called with represents the entire row. Subclasses can call DrawSegment () with adjusted row-rects
            taking into account indents, etc.</p>
                <p>Note, only the invalidRect subset of currentRowRect need be drawn, though the rest CAN be.</p>
                <p>Renamed to @'TextImager::DrawRowSegments' from MutliRowTextImager::DrawRowSegments for Led 3.1a3 release.</p>
*/
void TextImager::DrawRowSegments (Led_Tablet tablet, const Led_Rect& currentRowRect, const Led_Rect& invalidRowRect,
                                  const TextLayoutBlock& text, size_t rowStart, size_t rowEnd)
{
    RequireNotNull (tablet);

#if qDebug && 0
    // try to get this code enabled again - even here!!! LGP 2002-12-02
    {
        size_t startOfRow   = GetStartOfRow (row);
        size_t endOfRow     = GetEndOfRow (row);
        size_t realEndOfRow = GetRealEndOfRow (row);
        Assert (startOfRow == start);
        Assert (endOfRow <= end);
        Assert (end <= realEndOfRow);
    }
#endif

    /*
     *  We always want to draw all the characters in the row - including the character
     *  that terminates the row. Typically - this is a space in a word-wrap so it
     *  cannot be seen. Or a NEWLINE char. In the case of a NEWLINE char - these
     *  don't really display properly - so we skip drawing those.
     *
     *  An earlier attempt at this said - DONT BOTHER drawing the wrap character. The problem
     *  with this is that in SOME languages (e.g. Japanese) the character used as a wrap-char
     *  may be a real useful (Japanese) character!
     */
    size_t         segEnd   = rowEnd;
    Led_Coordinate baseLine = currentRowRect.top + MeasureSegmentBaseLine (rowStart, segEnd);

    /*
     *  Its OK for the baseline to be outside of the currentRowRect. But the text display of this
     *  seems to mimic MSWord better if you pin the baseLine inside the rowRect.
     */
    baseLine = min (baseLine, currentRowRect.bottom);

    DrawSegment (tablet, rowStart, segEnd, text, currentRowRect, invalidRowRect, baseLine, nullptr);
}

/*
@METHOD:        TextImager::DrawRowHilight
@DESCRIPTION:   <p>Called by @'TextImager::DrawRow' to draw any necessary hilighting for the current selection
            for the given row.</p>
                <p>Note, only the invalidRect subset of currentRowRect need be drawn, though the rest CAN be.</p>
                <p>Renamed/Moved to @'TextImager::DrawRowHilight' from MutliRowTextImager::DrawRowHilight for Led 3.1a3 release.</p>
*/
void TextImager::DrawRowHilight (Led_Tablet tablet, [[maybe_unused]] const Led_Rect& currentRowRect, const Led_Rect& /*invalidRowRect*/,
                                 const TextLayoutBlock& text, size_t rowStart, size_t rowEnd)
{
    Require (rowEnd == GetEndOfRowContainingPosition (rowStart)); // passed in for performance reasons - so not computed multiple times

    if (GetSelectionShown ()) {
        vector<Led_Rect> hilightRects = GetRowHilightRects (text, rowStart, rowEnd, GetSelectionStart (), GetSelectionEnd ());
        for (auto i = hilightRects.begin (); i != hilightRects.end (); ++i) {
            Led_Rect hilightRect = *i;
#if qDebug
            {
                // Funky test - see SPR# 0470 for details...
                if (Intersect (hilightRect, currentRowRect) or hilightRect.IsEmpty ()) {
                    Led_Rect x = hilightRect;
                    Led_Rect y = currentRowRect;
                    x.left     = y.left;
                    x.right    = y.right;
                    Assert (Intersect (x, y) or x.IsEmpty ());
                }
            }
#endif
            HilightArea (tablet, hilightRect);
        }
    }
}

/*
@METHOD:        TextImager::DrawInterLineSpace
@DESCRIPTION:   <p>Typically called by @'MultiRowTextImager::DrawPartitionElement' or @'SimpleTextImager::DrawPartitionElement' to draw space between paragraphs (lines).
            Typically this is nothing. But this hook can be used to draw various sorts of annotations on
            paragraphs (as in LECs LVEJ side-by-side mode).</p>
                <p>Renamed to @'TextImager::DrawInterLineSpace' from MutliRowTextImager::DrawInterLineSpace for Led 3.1a3 release.</p>
*/
void TextImager::DrawInterLineSpace (Led_Distance interlineSpace, Led_Tablet tablet, Led_Coordinate vPosOfTopOfInterlineSpace, bool segmentHilighted, bool printing)
{
    // This code not been checked/tested since I rewrote the erasing code etc.. Maybe wrong - probably wrong? No matter, anybody
    // ever using interline space would probably OVERRIDE this anyhow..
    // LGP 960516
    AssertNotNull (tablet);
    if (interlineSpace != 0) {
        Led_Rect fillRect = GetWindowRect ();
        fillRect.top      = vPosOfTopOfInterlineSpace;
        fillRect.bottom   = vPosOfTopOfInterlineSpace + interlineSpace;
        EraseBackground (tablet, fillRect, printing);
        if (segmentHilighted) {
            HilightArea (tablet, fillRect);
        }
    }
}

/*
@METHOD:        TextImager::ContainsMappedDisplayCharacters
@DESCRIPTION:   <p>Override this to specify if any of the given characters should be hidden, removed,
    or mapped to some other display character.
    You can change the behavior of this
    function at runtime (depending on user settings). But when its changed - you should invalidate all cached information,
    since cached font metrics will we invalid (often with @'MultiRowTextImager::InvalidateAllCaches ()').</p>
        <p>Note - this function doesn't REPLACE the given character from the text. Instead -
    it merely causes the @'TextImager::ReplaceMappedDisplayCharacters', @'TextImager::RemoveMappedDisplayCharacters',
    and @'TextImager::RemoveMappedDisplayCharacters' () methods to get called.</p>
        <p>Note also that this function takes an array of characters as a performance optimization (so it doesn't need to
    be called too many times, and to avoid copying buffers when nothing need be done). It could have been more logically
    (though less efficeintly) implemented as bool IsMappedDisplayCharacter (Led_tChar).</p>
        <p>If you OVERRIDE this - you may find it handy to call @'TextImager::ContainsMappedDisplayCharacters_HelperForChar'
    to do most of the work.</p>
*/
bool TextImager::ContainsMappedDisplayCharacters (const Led_tChar* /*text*/, size_t /*nTChars*/) const
{
    return false;
}

/*
@METHOD:        TextImager::ReplaceMappedDisplayCharacters
@DESCRIPTION:   <p>Override this to specify any characters which should be mapped to different values at the last minute for
    display purposes only. This can be used to give a (simple to implement) funny display for particular special characters.
    For example, this technique can be used to make a NEWLINE character display as a special end-of-paragraph marker.</p>
        <p>If you OVERRIDE this - you may find it handy to call @'TextImager::ReplaceMappedDisplayCharacters_HelperForChar'
    to do most of the work.</p>
        <p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
void TextImager::ReplaceMappedDisplayCharacters (const Led_tChar* srcText, Led_tChar* copyText, size_t nTChars) const
{
    // Default to none replaced- just plain copy...
    (void)::memcpy (copyText, srcText, nTChars * sizeof (Led_tChar));
}

/*
@METHOD:        TextImager::RemoveMappedDisplayCharacters
@DESCRIPTION:   <p>Override this to specify any characters which should be removed at the last minute for
    display purposes only. This can be used to give a (simple to implement) way to hide some special characters.
    For example, this technique can be used to implement SHIFT-RETURN soft line breaks (as is done in
    @'WordWrappedTextImager::RemoveMappedDisplayCharacters').</p>
        <p>If you OVERRIDE this - you may find it handy to call @'TextImager::RemoveMappedDisplayCharacters_HelperForChar'
    to do most of the work.</p>
        <p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
size_t TextImager::RemoveMappedDisplayCharacters (Led_tChar* /*copyText*/, size_t nTChars) const
{
    // Default to none removed
    return nTChars;
}

/*
@METHOD:        TextImager::PatchWidthRemoveMappedDisplayCharacters
@DESCRIPTION:   <p>Override this to specify any characters which should be removed at the last minute for
    display purposes only. This particular function patches the distanceResults to zero out the removed characters.
    This can be used to give a (simple to implement) way to hide some special characters.</p>
        <p>For example, this technique can be used to implement SHIFT-RETURN soft line breaks (as is done in
    @'WordWrappedTextImager::RemoveMappedDisplayCharacters').</p>
        <p>If you OVERRIDE this - you may find it handy to call @'TextImager::PatchWidthRemoveMappedDisplayCharacters_HelperForChar'
    to do most of the work.</p>
        <p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
void TextImager::PatchWidthRemoveMappedDisplayCharacters (const Led_tChar* /*srcText*/, Led_Distance* /*distanceResults*/, size_t /*nTChars*/) const
{
}

/*
@METHOD:        TextImager::ContainsMappedDisplayCharacters_HelperForChar
@DESCRIPTION:   <p>Simple implementation of See also @'TextImager::ContainsMappedDisplayCharacters' which is frequently applicable.
    Just specify the special character you are looking for.</p>
*/
bool TextImager::ContainsMappedDisplayCharacters_HelperForChar (const Led_tChar* text, size_t nTChars, Led_tChar charToMap)
{
    // 'charToMap' characters can appear anywhere in a segment of text (cuz this gets called to compute widths for an entire paragraph at a time).
    const Led_tChar* end = &text[nTChars];
    for (const Led_tChar* cur = text; cur < end; cur = Led_NextChar (cur)) {
        if (*cur == charToMap) {
            return true;
        }
    }
    return false;
}

/*
@METHOD:        TextImager::ReplaceMappedDisplayCharacters_HelperForChar
@DESCRIPTION:   <p>Simple implementation of See also @'TextImager::ReplaceMappedDisplayCharacters_HelperForChar' which is frequently applicable.
    Just specify the special character you are looking for, and the one you are mapping to.</p>
        <p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
void TextImager::ReplaceMappedDisplayCharacters_HelperForChar (Led_tChar* copyText, size_t nTChars, Led_tChar charToMap, Led_tChar charToMapTo)
{
    // 'charToMap' characters can appear anywhere in a segment of text (cuz this gets called to compute widths for an entire paragraph at a time).
    Led_tChar* end = &copyText[nTChars];
    for (Led_tChar* cur = copyText; cur < end; cur = Led_NextChar (cur)) {
        if (*cur == charToMap) {
            *cur = charToMapTo;
        }
    }
}

/*
@METHOD:        TextImager::RemoveMappedDisplayCharacters_HelperForChar
@DESCRIPTION:   <p>Simple implementation of See also @'TextImager::RemoveMappedDisplayCharacters' which is frequently applicable.
    Just specify the special character you are looking to remove.</p>
        <p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
size_t TextImager::RemoveMappedDisplayCharacters_HelperForChar (Led_tChar* copyText, size_t nTChars, Led_tChar charToRemove)
{
    // Trim out any kSoftLineBreakChar characters
    Led_tChar* outPtr = copyText;
    Led_tChar* end    = copyText + nTChars;
    for (const Led_tChar* cur = copyText; cur < end; cur = Led_NextChar (cur)) {
        if (*cur != charToRemove) {
            *outPtr = *cur;
            outPtr  = Led_NextChar (outPtr);
        }
    }
    size_t newLen = outPtr - copyText;
    Assert (newLen <= nTChars);
    return newLen;
}

/*
@METHOD:        TextImager::PatchWidthRemoveMappedDisplayCharacters_HelperForChar
@DESCRIPTION:   <p>Simple implementation of See also @'TextImager::PatchWidthRemoveMappedDisplayCharacters' which is frequently applicable.
    Just specify the special character you are looking to remove.</p>
        <p>See also @'TextImager::ContainsMappedDisplayCharacters'.</p>
*/
void TextImager::PatchWidthRemoveMappedDisplayCharacters_HelperForChar (const Led_tChar* srcText, Led_Distance* distanceResults, size_t nTChars, Led_tChar charToRemove)
{
    // Each of these kSoftLineBreakChar will be mapped to ZERO-WIDTH. So walk text (and distanceResults) and when
    // I see a softlinebreak - zero its size, and subtrace from start point total amount of zero-ed softlinebreaks.
    Led_Distance     cumSubtract = 0;
    const Led_tChar* end         = srcText + nTChars;
    for (const Led_tChar* cur = srcText; cur < end; cur = Led_NextChar (cur)) {
        size_t i = cur - srcText;
        Assert (i < nTChars);
        if (*cur == charToRemove) {
            Led_Distance thisSoftBreakWidth = i == 0 ? distanceResults[0] : (distanceResults[i] - distanceResults[i - 1]);
            cumSubtract                     = thisSoftBreakWidth;
        }
        distanceResults[i] -= cumSubtract;
    }
}

/*
@METHOD:        TextImager::DrawSegment
@DESCRIPTION:   <p>DrawSegment () is responsible for rendering the text within a segment (subset of a row).
            Note that because of bidirectional display, the 'from' and 'to' are LOGICAL offsets (what are
            used to lookup in @'TextStore'), but they may NOT be the same as the display-order offsets. That is to
            say - a character between offset 10-11 could be either to the right or left of one at offset 13-14.
            </p>
                <p>Note we REQUIRE that useBaseLine be contained within drawInto invalidRect specified the subset of the drawInto
            rect which really must be filled in. This can be ignored, or used for logical clipping.</p>
                <p>See also @'TextImager::DrawSegment_'. The variable 'pixelsDrawn' is OPTIONAL parameter
            (ie filled in if non-nullptr)</p>
                <p>See also @'TextImager::DrawSegment_'.</p>
*/
void TextImager::DrawSegment (Led_Tablet tablet,
                              size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                              Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    DrawSegment_ (tablet, GetDefaultFont (), from, to, text, drawInto, useBaseLine, pixelsDrawn);
}

/*
@METHOD:        TextImager::DrawSegment_
@DESCRIPTION:   <p>Helper function to access tablet text drawing. This function takes care of setting the background/foreground colors,
            and setting up a font object to be used in the Led_Tablet_::TabbedTextOut calls (maybe can go away if we do better integrating
            font code into Led_Tablet/LedGDI and out of TextImager).</p>
                <p>The 'from' marker position must be legit, since it is used to grab the tabstop list.
            The 'end' marker position is OK to fake (passing in other text), as it is just used to determine the string length. Note
            text in 'text' argument need not match the REAL text in the TextStore buffer.
        <p>See also @'TextImager::MeasureSegmentWidth_'.</p>
*/
void TextImager::DrawSegment_ (Led_Tablet tablet, const Led_FontSpecification& fontSpec,
                               size_t from, size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto,
                               Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn) const
{
    RequireNotNull (tablet);
    Assert (from <= to);

    /*
     *  In the presence of multiple markers, you might VERY plaisbly OVERRIDE this method and
     *  change how the layout of text is done within a segment.
     *
     *  I've thought long and hard about how to automatically combine the drawing effects
     *  of different markers which overlap, and have come up with noting reasonable. So its
     *  up to YOU by overriding this method todo what you want.
     */
    [[maybe_unused]] size_t length = to - from;

    /*
     *  Setup the colors to be drawn.
     */
    Led_Color foreColor = fontSpec.GetTextColor ();
    Led_Color backColor = GetEffectiveDefaultTextColor (eDefaultBackgroundColor);
    tablet->SetBackColor (backColor);
    tablet->SetForeColor (foreColor);

    FontCacheInfoUpdater fontCacheUpdater (this, tablet, fontSpec);

    Led_Distance ascent = fCachedFontInfo.GetAscent ();
    Assert (useBaseLine >= drawInto.top);

    //Assert (useBaseLine <= drawInto.bottom);      Now allowed... LGP 2000-06-12 - see SPR#0760 - and using EXACT-height of a small height, and use a large font
    Led_Coordinate drawCharTop = useBaseLine - ascent; // our PortableGDI_TabbedTextOut() assumes draw in topLeft
    //Require (drawCharTop >= drawInto.top);        // Same deal as for useBaseLine - LGP 2000-06-12

    if (fontSpec.GetStyle_SubOrSuperScript () == Led_FontSpecification::eSuperscript) {
        // See SPR#1523- was 'drawCharTop -= fCachedFontInfo.GetAscent ()'
        // Careful to sync this with FontCacheInfoUpdater::CTOR () height adjustment
        // get back to (roughly - round down) original ascent. If we did TIMES 2/3 now
        // mutliply by reciprocal to get back (again - rounding down so we don't go
        // up too high).
        drawCharTop = useBaseLine - ascent * 3 / 2;
    }
    else if (fontSpec.GetStyle_SubOrSuperScript () == Led_FontSpecification::eSubscript) {
        drawCharTop += fCachedFontInfo.GetDescent ();
    }

    using ScriptRunElt        = TextLayoutBlock::ScriptRunElt;
    vector<ScriptRunElt> runs = text.GetScriptRuns ();
    Assert (not runs.empty () or (length == 0));
    if (runs.size () > 1) {
        // sort by virtual start
        sort (runs.begin (), runs.end (), TextLayoutBlock::LessThanVirtualStart ());
    }

    const Led_tChar* fullVirtualText = text.PeekAtVirtualText ();
    Led_Point        outputAt        = Led_Point (drawCharTop, drawInto.left);
    if (pixelsDrawn != nullptr) {
        *pixelsDrawn = 0;
    }
    for (auto i = runs.begin (); i != runs.end (); ++i) {
        const ScriptRunElt& se        = *i;
        size_t              runLength = se.fVirtualEnd - se.fVirtualStart;

        /*
         *  Fill in the useVirtualText buffer with the text to draw.
         */
        Memory::SmallStackBuffer<Led_tChar> useVirtualText (runLength);
        (void)::memcpy (static_cast<Led_tChar*> (useVirtualText), &fullVirtualText[se.fVirtualStart], runLength * sizeof (Led_tChar));

        /*
         *  Process 'mapped display characters'
         */
        Led_tChar*                          drawText    = useVirtualText;
        size_t                              drawTextLen = runLength;
        Memory::SmallStackBuffer<Led_tChar> mappedDisplayBuf (1);
        if (ContainsMappedDisplayCharacters (drawText, drawTextLen)) {
            mappedDisplayBuf.GrowToSize (drawTextLen);
            ReplaceMappedDisplayCharacters (drawText, mappedDisplayBuf, drawTextLen);
            size_t newLength = RemoveMappedDisplayCharacters (mappedDisplayBuf, drawTextLen);
            Assert (newLength <= drawTextLen);
            drawText    = mappedDisplayBuf;
            drawTextLen = newLength;
        }

        /*
         *  Actually draw the text.
         */
        Led_Distance amountDrawn = 0;
        tablet->TabbedTextOut (fCachedFontInfo, drawText, drawTextLen, se.fDirection, outputAt, GetWindowRect ().left, GetTabStopList (from), &amountDrawn, GetHScrollPos ());
        outputAt.h += amountDrawn;
        if (pixelsDrawn != nullptr) {
            *pixelsDrawn += amountDrawn;
        }
    }
}

void TextImager::MeasureSegmentWidth (size_t from, size_t to, const Led_tChar* text,
                                      Led_Distance* distanceResults) const
{
    MeasureSegmentWidth_ (GetDefaultFont (), from, to, text, distanceResults);
}

/*
@METHOD:        TextImager::MeasureSegmentWidth_
@DESCRIPTION:   <p>Helper function to access tablet text measurement.</p>
                <p>The 'from' and 'to' marker positions are ignored, except to compute the width. There is no
            requirement that the 'text' argument refer to the same text as that stored in the TextStore object.
        <p>See also @'TextImager::DrawSegment_'.</p>
*/
void TextImager::MeasureSegmentWidth_ (const Led_FontSpecification& fontSpec, size_t from, size_t to,
                                       const Led_tChar* text,
                                       Led_Distance*    distanceResults) const
{
    Require (to > from);

    Tablet_Acquirer tablet (this);

    size_t length = to - from;
    Assert (length > 0);

#if qMultiByteCharacters
    Assert (Led_IsValidMultiByteString (text, length));
    Assert_CharPosDoesNotSplitCharacter (from);
    Assert_CharPosDoesNotSplitCharacter (to);
#endif

    FontCacheInfoUpdater fontCacheUpdater (this, tablet, fontSpec);

    if (ContainsMappedDisplayCharacters (text, length)) {
        Memory::SmallStackBuffer<Led_tChar> buf2 (length);
        ReplaceMappedDisplayCharacters (text, buf2, length);
        tablet->MeasureText (fCachedFontInfo, buf2, length, distanceResults);
        PatchWidthRemoveMappedDisplayCharacters (buf2, distanceResults, length);
    }
    else {
        tablet->MeasureText (fCachedFontInfo, text, length, distanceResults);
    }
}

Led_Distance TextImager::MeasureSegmentHeight (size_t from, size_t to) const
{
    return (MeasureSegmentHeight_ (GetDefaultFont (), from, to));
}

Led_Distance TextImager::MeasureSegmentHeight_ (const Led_FontSpecification& fontSpec, size_t /*from*/, size_t /*to*/) const
{
    Tablet_Acquirer tablet (this);
    AssertNotNull (static_cast<Led_Tablet> (tablet));
    FontCacheInfoUpdater fontCacheUpdater (this, tablet, fontSpec);
    return (fCachedFontInfo.GetLineHeight ());
}

Led_Distance TextImager::MeasureSegmentBaseLine (size_t from, size_t to) const
{
    return (MeasureSegmentBaseLine_ (GetDefaultFont (), from, to));
}

Led_Distance TextImager::MeasureSegmentBaseLine_ (const Led_FontSpecification& fontSpec, size_t /*from*/, size_t /*to*/) const
{
    Tablet_Acquirer tablet (this);
    AssertNotNull (static_cast<Led_Tablet> (tablet));
    FontCacheInfoUpdater fontCacheUpdater (this, tablet, fontSpec);
    return (fCachedFontInfo.GetAscent ());
}

Led_FontMetrics TextImager::GetFontMetricsAt (
#if qMultiByteCharacters
    size_t charAfterPos
#else
    size_t /*charAfterPos*/
#endif
    ) const
{
    Tablet_Acquirer tablet (this);
    AssertNotNull (static_cast<Led_Tablet> (tablet));

#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (charAfterPos);
#endif

    Led_FontSpecification fontSpec = GetDefaultFont ();

    FontCacheInfoUpdater fontCacheUpdater (this, tablet, fontSpec);
    return (fCachedFontInfo);
}
