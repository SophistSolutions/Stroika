/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "StandardStyledTextImager.h"

using namespace Stroika::Foundation;

using Memory::MakeSharedPtr;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ************************** AbstractStyleDatabaseRep ****************************
 ********************************************************************************
 */
#if qStroika_Foundation_Debug_AssertionsChecked
void AbstractStyleDatabaseRep::Invariant_ () const
{
}
#endif

/*
 ********************************************************************************
 *************** StandardStyledTextImager::StandardStyleMarker ******************
 ********************************************************************************
 */
#if qStroika_Frameworks_Led_SupportGDI
void StandardStyleMarker::DrawSegment (const StyledTextImager* imager, const StyleRunElement& /*runElement*/, Tablet* tablet, size_t from,
                                       size_t to, const TextLayoutBlock& text, const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/,
                                       CoordinateType useBaseLine, DistanceType* pixelsDrawn)
{
    RequireNotNull (imager);
    imager->DrawSegment_ (tablet, fFontSpecification, from, to, text, drawInto, useBaseLine, pixelsDrawn);
}

void StandardStyleMarker::MeasureSegmentWidth (const StyledTextImager* imager, const StyleRunElement& /*runElement*/, size_t from,
                                               size_t to, const Led_tChar* text, DistanceType* distanceResults) const
{
    RequireNotNull (imager);
    imager->MeasureSegmentWidth_ (fFontSpecification, from, to, text, distanceResults);
}

DistanceType StandardStyleMarker::MeasureSegmentHeight (const StyledTextImager* imager, const StyleRunElement& /*runElement*/, size_t from, size_t to) const
{
    RequireNotNull (imager);
    return (imager->MeasureSegmentHeight_ (fFontSpecification, from, to));
}

DistanceType StandardStyleMarker::MeasureSegmentBaseLine (const StyledTextImager* imager, const StyleRunElement& /*runElement*/, size_t from, size_t to) const
{
    RequireNotNull (imager);
    return (imager->MeasureSegmentBaseLine_ (fFontSpecification, from, to));
}
#endif

/*
 ********************************************************************************
 **************** StandardStyledTextImager::StyleDatabaseRep ********************
 ********************************************************************************
 */
StyleDatabaseRep::StyleDatabaseRep (TextStore& textStore)
    : inheritedMC{textStore, GetStaticDefaultFont ()}
{
}

vector<StyledInfoSummaryRecord> StyleDatabaseRep::GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const
{
    MarkerVector standardStyleMarkers = GetInfoMarkers (charAfterPos, nTCharsFollowing);

    vector<StyledInfoSummaryRecord> result;
    size_t                          tCharsSoFar           = 0;
    size_t                          nStandardStyleMarkers = standardStyleMarkers.size ();
    for (size_t i = 0; i < nStandardStyleMarkers; ++i) {
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
        result.push_back (StyledInfoSummaryRecord (marker->fFontSpecification, length));
        tCharsSoFar += length;
    }
    Assert (tCharsSoFar == nTCharsFollowing);
    return result;
}

void StyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const IncrementalFontSpecification& styleInfo)
{
    SetInfo (charAfterPos, nTCharsFollowing, styleInfo);
}

void StyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const StyledInfoSummaryRecord* styleInfos)
{
    size_t setAt           = charAfterPos;
    size_t lengthUsedSoFar = 0;
    for (size_t i = 0; i < nStyleInfos and lengthUsedSoFar < nTCharsFollowing; ++i) {
        StyledInfoSummaryRecord isr    = styleInfos[i];
        size_t                  length = isr.fLength;
        Assert (nTCharsFollowing >= lengthUsedSoFar);
        length = min (nTCharsFollowing - lengthUsedSoFar, length);
        SetStyleInfo (setAt, length, IncrementalFontSpecification{isr});
        setAt += length;
        lengthUsedSoFar += length;
    }
}

#if qStroika_Foundation_Debug_AssertionsChecked
void StyleDatabaseRep::Invariant_ () const
{
    inheritedMC::Invariant_ ();
}
#endif

#if qStroika_Frameworks_Led_SupportGDI
/*
 ********************************************************************************
 ****************************** StandardStyledTextImager ************************
 ********************************************************************************
 */
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
            fStyleDatabase = nullptr;
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
        fStyleDatabase    = MakeSharedPtr<StyleDatabaseRep> (GetTextStore ());
        fICreatedDatabase = true;
        HookStyleDatabaseChanged ();
    }
}

void StandardStyledTextImager::SetStyleDatabase (const shared_ptr<AbstractStyleDatabaseRep>& styleDatabase)
{
    fStyleDatabase    = styleDatabase;
    fICreatedDatabase = false;
    if (fStyleDatabase == nullptr and PeekAtTextStore () != nullptr) {
        fStyleDatabase    = MakeSharedPtr<StyleDatabaseRep> (GetTextStore ());
        fICreatedDatabase = true;
    }
    HookStyleDatabaseChanged ();
}

/*
@METHOD:        StandardStyledTextImager::HookStyleDatabaseChanged
@DESCRIPTION:   <p>Called whenever the @'StyleDatabasePtr' associated with this @'StandardStyledTextImager'
    is changed. This means when a new one is provided, created, or disassociated. It does NOT mean that its called when any of the
    data in the style database changes.</p>
*/
void StandardStyledTextImager::HookStyleDatabaseChanged ()
{
}

FontMetrics StandardStyledTextImager::GetFontMetricsAt (size_t charAfterPos) const
{
    Tablet_Acquirer tablet (this);
    AssertNotNull (static_cast<Tablet*> (tablet));

    FontCacheInfoUpdater fontCacheUpdater (this, tablet, GetStyleInfo (charAfterPos));
    return (fontCacheUpdater.GetMetrics ());
}

/*
@METHOD:        StandardStyledTextImager::GetDefaultSelectionFont
@DESCRIPTION:   <p>Override @'TextImager::GetDefaultSelectionFont'.</p>
*/
FontSpecification StandardStyledTextImager::GetDefaultSelectionFont () const
{
    vector<StyledInfoSummaryRecord> summaryInfo = GetStyleInfo (GetSelectionEnd (), 0);
    Assert (summaryInfo.size () == 1);
    return summaryInfo[0];
}

/*
@METHOD:        StandardStyledTextImager::GetContinuousStyleInfo
@DESCRIPTION:   <p>Create a @'IncrementalFontSpecification' with set as valid all attributes which apply to all of the text from
    <code>'from'</code> for <code>'nTChars'</code>.</p>
        <p>So for example - if all text in that range has the same face, but different font sizes, then the face attribute will be
    valid (and set to that common face) and the font size attribute will be set invalid.</p>
        <p>This is useful for setting menus checked or unchecked in a typical word processor font menu.</p>
*/
IncrementalFontSpecification StandardStyledTextImager::GetContinuousStyleInfo (size_t from, size_t nTChars) const
{
    vector<StyledInfoSummaryRecord> summaryInfo = GetStyleInfo (from, nTChars);
    return (GetContinuousStyleInfo_ (summaryInfo));
}

IncrementalFontSpecification StandardStyledTextImager::GetContinuousStyleInfo_ (const vector<StyledInfoSummaryRecord>& summaryInfo) const
{
    IncrementalFontSpecification fontSpec;

    // There are only a certain number of font attributes which can be shared among these InfoSummaryRecords.
    // Each time we note one which cannot be shared - we decrement this count. That way - when we know there can be
    // no shared values - we stop comparing.
    //
    // countOfValidThings is a hack to see if we can skip out of for-loop early without a lot of expensive, and
    // redundant tests.
    //
    // Note - we COULD have simply checked at the end of each loop count a bunch of 'IsValid' booleans. That would have
    // been simpler. But it would have been more costly (performance).
    int countOfValidThings = 7 +
#if qStroika_Platform_Windows
                             1
#endif
        ;

    for (size_t i = 0; i < summaryInfo.size (); ++i) {
        if (i == 0) {
            fontSpec = IncrementalFontSpecification{summaryInfo[0]};
        }
        else {
            // check each attribute (if not already different) and see if NOW different...

            StyledInfoSummaryRecord isr = summaryInfo[i];

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
#if qStroika_Platform_Windows
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

    return fontSpec;
}

#if qStroika_Foundation_Debug_AssertionsChecked
void StandardStyledTextImager::Invariant_ () const
{
    StyledTextImager::Invariant_ ();
    if (fStyleDatabase.get () != nullptr) {
        fStyleDatabase->Invariant ();
    }
}
#endif

#endif
