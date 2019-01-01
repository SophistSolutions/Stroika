/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "StandardStyledTextImager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

using StyleMarker       = StyledTextImager::StyleMarker;
using StyleDatabaseRep  = StandardStyledTextImager::StyleDatabaseRep;
using InfoSummaryRecord = StandardStyledTextImager::InfoSummaryRecord;

/*
 ********************************************************************************
 *************** StandardStyledTextImager::StandardStyleMarker ******************
 ********************************************************************************
 */
using StandardStyleMarker = StandardStyledTextImager::StandardStyleMarker;

void StandardStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
                                       size_t from, size_t to, const TextLayoutBlock& text,
                                       const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    RequireNotNull (imager);
    imager->DrawSegment_ (tablet, fFontSpecification, from, to, text, drawInto, useBaseLine, pixelsDrawn);
}

void StandardStyleMarker::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to,
                                               const Led_tChar* text,
                                               Led_Distance*    distanceResults) const
{
    RequireNotNull (imager);
    imager->MeasureSegmentWidth_ (fFontSpecification, from, to, text, distanceResults);
}

Led_Distance StandardStyleMarker::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to) const
{
    RequireNotNull (imager);
    return (imager->MeasureSegmentHeight_ (fFontSpecification, from, to));
}

Led_Distance StandardStyleMarker::MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to) const
{
    RequireNotNull (imager);
    return (imager->MeasureSegmentBaseLine_ (fFontSpecification, from, to));
}

/*
 ********************************************************************************
 ****************************** StandardStyledTextImager ************************
 ********************************************************************************
 */
StandardStyledTextImager::StandardStyledTextImager ()
    : fStyleDatabase (nullptr)
    , fICreatedDatabase (false)
{
}

void StandardStyledTextImager::HookLosingTextStore ()
{
    inherited::HookLosingTextStore ();
    HookLosingTextStore_ ();
}

void StandardStyledTextImager::HookLosingTextStore_ ()
{
    // Only if we created the styledb should we delete it. If it was set by SetStyleDatabase(), don't unset it here.
    if (fICreatedDatabase) {
        fICreatedDatabase = false;
        if (fStyleDatabase.get () != nullptr) {
            fStyleDatabase = StyleDatabasePtr ();
            HookStyleDatabaseChanged ();
        }
    }
}

void StandardStyledTextImager::HookGainedNewTextStore ()
{
    inherited::HookGainedNewTextStore ();
    HookGainedNewTextStore_ ();
}

void StandardStyledTextImager::HookGainedNewTextStore_ ()
{
    if (fStyleDatabase == nullptr) {
        fStyleDatabase    = make_shared<StyleDatabaseRep> (GetTextStore ());
        fICreatedDatabase = true;
        HookStyleDatabaseChanged ();
    }
}

void StandardStyledTextImager::SetStyleDatabase (const StyleDatabasePtr& styleDatabase)
{
    fStyleDatabase    = styleDatabase;
    fICreatedDatabase = false;
    if (fStyleDatabase == nullptr and PeekAtTextStore () != nullptr) {
        fStyleDatabase    = make_shared<StyleDatabaseRep> (GetTextStore ());
        fICreatedDatabase = true;
    }
    HookStyleDatabaseChanged ();
}

/*
@METHOD:        StandardStyledTextImager::HookStyleDatabaseChanged
@DESCRIPTION:   <p>Called whenever the @'StandardStyledTextImager::StyleDatabasePtr' associated with this @'StandardStyledTextImager'
    is changed. This means when a new one is provided, created, or disassociated. It does NOT mean that its called when any of the
    data in the style database changes.</p>
*/
void StandardStyledTextImager::HookStyleDatabaseChanged ()
{
}

Led_FontMetrics StandardStyledTextImager::GetFontMetricsAt (size_t charAfterPos) const
{
    Tablet_Acquirer tablet (this);
    AssertNotNull (static_cast<Led_Tablet> (tablet));

#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (charAfterPos);
#endif

    FontCacheInfoUpdater fontCacheUpdater (this, tablet, GetStyleInfo (charAfterPos));
    return (fontCacheUpdater.GetMetrics ());
}

/*
@METHOD:        StandardStyledTextImager::GetDefaultSelectionFont
@DESCRIPTION:   <p>Override @'TextImager::GetDefaultSelectionFont'.</p>
*/
Led_FontSpecification StandardStyledTextImager::GetDefaultSelectionFont () const
{
    vector<InfoSummaryRecord> summaryInfo = GetStyleInfo (GetSelectionEnd (), 0);
    Assert (summaryInfo.size () == 1);
    return summaryInfo[0];
}

/*
@METHOD:        StandardStyledTextImager::GetContinuousStyleInfo
@DESCRIPTION:   <p>Create a @'Led_IncrementalFontSpecification' with set as valid all attributes which apply to all of the text from
    <code>'from'</code> for <code>'nTChars'</code>.</p>
        <p>So for example - if all text in that range has the same face, but different font sizes, then the face attribute will be
    valid (and set to that common face) and the font size attribute will be set invalid.</p>
        <p>This is useful for setting menus checked or unchecked in a typical word processor font menu.</p>
*/
Led_IncrementalFontSpecification StandardStyledTextImager::GetContinuousStyleInfo (size_t from, size_t nTChars) const
{
    vector<InfoSummaryRecord> summaryInfo = GetStyleInfo (from, nTChars);
    return (GetContinuousStyleInfo_ (summaryInfo));
}

Led_IncrementalFontSpecification StandardStyledTextImager::GetContinuousStyleInfo_ (const vector<InfoSummaryRecord>& summaryInfo) const
{
    Led_IncrementalFontSpecification fontSpec;

    // There are only a certain number of font attributes which can be shared among these InfoSummaryRecords.
    // Each time we note one which cannot be shared - we decrement this count. That way - when we know there can be
    // no shared values - we stop comparing.
    //
    // countOfValidThings is a hack to see if we can skip out of for-loop early without a lot of expensive, and
    // redundant tests.
    //
    // Note - we COULD have simply checked at the end of each loop count a bunch of 'IsValid' booleans. That would have
    // been simpler. But it would have been more costly (performance).
    int countOfValidThings =
        7 +
#if qPlatform_MacOS
        4
#elif qPlatform_Windows
        1
#elif qXWindows
        0 //  X-TMP-HACK-LGP991213    -- Not quite a hack - but revisit when we have REAL X-Font support
#endif
        ;

    for (size_t i = 0; i < summaryInfo.size (); i++) {
        if (i == 0) {
            fontSpec = Led_IncrementalFontSpecification (summaryInfo[0]);
        }
        else {
            // check each attribute (if not already different) and see if NOW different...

            InfoSummaryRecord isr = summaryInfo[i];

            // Font ID
            if (fontSpec.GetFontNameSpecifier_Valid () and fontSpec.GetFontNameSpecifier () != isr.GetFontNameSpecifier ()) {
                fontSpec.InvalidateFontNameSpecifier ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }

            // Style Info
            if (fontSpec.GetStyle_Bold_Valid () and fontSpec.GetStyle_Bold () != isr.GetStyle_Bold ()) {
                fontSpec.InvalidateStyle_Bold ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
            if (fontSpec.GetStyle_Italic_Valid () and fontSpec.GetStyle_Italic () != isr.GetStyle_Italic ()) {
                fontSpec.InvalidateStyle_Italic ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
            if (fontSpec.GetStyle_Underline_Valid () and fontSpec.GetStyle_Underline () != isr.GetStyle_Underline ()) {
                fontSpec.InvalidateStyle_Underline ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
            if (fontSpec.GetStyle_SubOrSuperScript_Valid () and fontSpec.GetStyle_SubOrSuperScript () != isr.GetStyle_SubOrSuperScript ()) {
                fontSpec.InvalidateStyle_SubOrSuperScript ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
#if qPlatform_MacOS
            if (fontSpec.GetStyle_Outline_Valid () and fontSpec.GetStyle_Outline () != isr.GetStyle_Outline ()) {
                fontSpec.InvalidateStyle_Outline ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
            if (fontSpec.GetStyle_Shadow_Valid () and fontSpec.GetStyle_Shadow () != isr.GetStyle_Shadow ()) {
                fontSpec.InvalidateStyle_Shadow ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
            if (fontSpec.GetStyle_Condensed_Valid () and fontSpec.GetStyle_Condensed () != isr.GetStyle_Condensed ()) {
                fontSpec.InvalidateStyle_Condensed ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
            if (fontSpec.GetStyle_Extended_Valid () and fontSpec.GetStyle_Extended () != isr.GetStyle_Extended ()) {
                fontSpec.InvalidateStyle_Extended ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
#elif qPlatform_Windows
            if (fontSpec.GetStyle_Strikeout_Valid () and fontSpec.GetStyle_Strikeout () != isr.GetStyle_Strikeout ()) {
                fontSpec.InvalidateStyle_Strikeout ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
#endif

            // Font Size
            if (fontSpec.GetPointSize_Valid () and fontSpec.GetPointSize () != isr.GetPointSize ()) {
                fontSpec.InvalidatePointSize ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }

            // Font Color
            if (fontSpec.GetTextColor_Valid () and fontSpec.GetTextColor () != isr.GetTextColor ()) {
                fontSpec.InvalidateTextColor ();
                if (--countOfValidThings == 0) {
                    break;
                }
            }
        }
    }

    return (fontSpec);
}

#if qPlatform_MacOS
bool StandardStyledTextImager::DoContinuousStyle_Mac (size_t from, size_t nTChars, short* mode, TextStyle* theStyle)
{
    //  Require ((*mode & doColor) == 0);   // NB: we currently don't support   doColor,  doAll , addSize
    // Just silently ignore doColor for now since done from TCL - and we just return NO for that style...
    Require ((*mode & addSize) == 0);
    RequireNotNull (theStyle);

    unsigned int                     resultMode = *mode;
    Led_IncrementalFontSpecification resultSpec = GetContinuousStyleInfo (from, nTChars);
    if (resultMode & doFont) {
        resultSpec.GetOSRep (&theStyle->tsFont, nullptr, nullptr);
    }
    if (resultMode & doFace) {
        resultSpec.GetOSRep (nullptr, nullptr, &theStyle->tsFace);
    }
    if (resultMode & doSize) {
        resultSpec.GetOSRep (nullptr, &theStyle->tsSize, nullptr);
    }

    bool result = (resultMode != *mode);
    *mode       = resultMode;
    return (result);
}

vector<StandardStyledTextImager::InfoSummaryRecord> StandardStyledTextImager::Convert (const ScrpSTElement* teScrapFmt, size_t nElts)
{
    vector<InfoSummaryRecord> result;
    for (size_t i = 0; i < nElts; i++) {
        Led_IncrementalFontSpecification fsp;
        fsp.SetOSRep (teScrapFmt[i].scrpFont, teScrapFmt[i].scrpSize, teScrapFmt[i].scrpFace);
        size_t            length = (i < (nElts - 1)) ? (teScrapFmt[i + 1].scrpStartChar - teScrapFmt[i].scrpStartChar) : 9999999;
        InfoSummaryRecord isr (fsp, length);
        result.push_back (isr);
    }
    return (result);
}

void StandardStyledTextImager::Convert (const vector<InfoSummaryRecord>& fromLedStyleRuns, ScrpSTElement* teScrapFmt)
{
    size_t nElts     = fromLedStyleRuns.size ();
    size_t startChar = 0;

    GrafPtr oldPort = Led_GetCurrentGDIPort ();
#if TARGET_CARBON
    CGrafPtr tmpPort = ::CreateNewPort ();
    ::SetPort (tmpPort);
#else
    CGrafPort tmpPort;
    ::OpenCPort (&tmpPort);
#endif

    for (size_t i = 0; i < nElts; i++) {
        InfoSummaryRecord isr = fromLedStyleRuns[i];

        (void)::memset (&teScrapFmt[i], 0, sizeof (teScrapFmt[i]));
        teScrapFmt[i].scrpStartChar = startChar;

        isr.GetOSRep (&teScrapFmt[i].scrpFont, &teScrapFmt[i].scrpSize, &teScrapFmt[i].scrpFace);

        ::TextFont (teScrapFmt[i].scrpFont);
        ::TextFace (teScrapFmt[i].scrpFace);
        ::TextSize (teScrapFmt[i].scrpSize);

        FontInfo info;
        ::GetFontInfo (&info);

        teScrapFmt[i].scrpHeight = info.ascent + info.descent + info.leading;
        teScrapFmt[i].scrpAscent = info.ascent;

        startChar += isr.fLength;
    }

#if TARGET_CARBON
    ::DisposePort (tmpPort);
#else
    ::CloseCPort (&tmpPort);
#endif
    ::SetPort (oldPort);
}
#endif

#if qDebug
void StandardStyledTextImager::Invariant_ () const
{
    StyledTextImager::Invariant_ ();
    if (fStyleDatabase.get () != nullptr) {
        fStyleDatabase->Invariant ();
    }
}
#endif

/*
 ********************************************************************************
 ************** StandardStyledTextImager::AbstractStyleDatabaseRep **************
 ********************************************************************************
 */
#if qDebug
void StandardStyledTextImager::AbstractStyleDatabaseRep::Invariant_ () const
{
}
#endif

/*
 ********************************************************************************
 **************** StandardStyledTextImager::StyleDatabaseRep ********************
 ********************************************************************************
 */
StyleDatabaseRep::StyleDatabaseRep (TextStore& textStore)
    : inheritedMC (textStore, TextImager::GetStaticDefaultFont ())
{
}

vector<StandardStyledTextImager::InfoSummaryRecord> StyleDatabaseRep::GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const
{
    MarkerVector standardStyleMarkers = GetInfoMarkers (charAfterPos, nTCharsFollowing);

    vector<InfoSummaryRecord> result;
    size_t                    tCharsSoFar           = 0;
    size_t                    nStandardStyleMarkers = standardStyleMarkers.size ();
    for (size_t i = 0; i < nStandardStyleMarkers; i++) {
        StandardStyleMarker* marker = standardStyleMarkers[i];
        AssertNotNull (marker);
        size_t markerStart;
        size_t markerEnd;
        marker->GetRange (&markerStart, &markerEnd);

        // for i==START and END, we may have to include only partial lengths of the
        // markers - for the INTERNAL markers, use their whole length
        size_t length = markerEnd - markerStart;
        if (i == 0) {
            Assert (charAfterPos >= markerStart);
            Assert (charAfterPos - markerStart < length);
            length -= (charAfterPos - markerStart);
        }
        if (i == nStandardStyleMarkers - 1) {
            Assert (length >= nTCharsFollowing - tCharsSoFar); // must be preserving, or shortening...
            length = nTCharsFollowing - tCharsSoFar;
        }
        Assert (length > 0 or nTCharsFollowing == 0);
        Assert (length <= nTCharsFollowing);
        result.push_back (InfoSummaryRecord (marker->fFontSpecification, length));
        tCharsSoFar += length;
    }
    Assert (tCharsSoFar == nTCharsFollowing);
    return result;
}

void StyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo)
{
    SetInfo (charAfterPos, nTCharsFollowing, styleInfo);
}
#if 0
void    StyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos)
{
    SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfos.size (), &styleInfos.front ());
}
#endif
void StyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos)
{
    size_t setAt           = charAfterPos;
    size_t lengthUsedSoFar = 0;
    for (size_t i = 0; i < nStyleInfos and lengthUsedSoFar < nTCharsFollowing; i++) {
        InfoSummaryRecord isr    = styleInfos[i];
        size_t            length = isr.fLength;
        Assert (nTCharsFollowing >= lengthUsedSoFar);
        length = min (nTCharsFollowing - lengthUsedSoFar, length);
        SetStyleInfo (setAt, length, Led_IncrementalFontSpecification (isr));
        setAt += length;
        lengthUsedSoFar += length;
    }
}

#if qDebug
void StyleDatabaseRep::Invariant_ () const
{
    inheritedMC::Invariant_ ();
}
#endif
