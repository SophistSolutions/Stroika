/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include "../../Foundation/Traversal/Iterator.h"

#include "StyledTextImager.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

using StyleMarker            = StyledTextImager::StyleMarker;
using RunElement             = StyledTextImager::RunElement;
using StyleMarkerSummarySink = StyledTextImager::StyleMarkerSummarySink;

/*
 ********************************************************************************
 ********************************** StyleMarker *********************************
 ********************************************************************************
 */
/*
@METHOD:        StyledTextImager::StyleMarker::GetPriority
@DESCRIPTION:   <p>Since we can have style markers overlap, we need someway to deal with conflicting
    style information. Since some style elements can be arbitrary drawing code, like
    an OpenDoc part, or a picture, we cannot genericly write code to merge styles.
    So we invoke a somewhat hackish priority scheme, where the marker with the highest
    priority is what is asked todo the drawing.</p>
        <p>The priority of StandardStyleMarker is eBaselinePriority - ZERO - as a reference. So you can specify
    easily either markers that take precedence over, or are always superseded by the
    standard style markers. And this returns eBaselinePriority - ZERO - by default.</p>
 */
int StyleMarker::GetPriority () const
{
    return eBaselinePriority;
}

/*
 ********************************************************************************
 **************************** StyleMarkerSummarySink ****************************
 ********************************************************************************
 */
StyleMarkerSummarySink::StyleMarkerSummarySink (size_t from, size_t to)
    : inherited ()
    , fBuckets ()
    , fText (nullptr)
    , fFrom (from)
    , fTo (to)
{
    // See SPR#1293 - may want to get rid of this eventually
    Require (from <= to);
    if (from != to) {
        fBuckets.push_back (RunElement (nullptr, to - from));
    }
}

StyleMarkerSummarySink::StyleMarkerSummarySink (size_t from, size_t to, const TextLayoutBlock& text)
    : inherited ()
    , fBuckets ()
    , fText (&text)
    , fFrom (from)
    , fTo (to)
{
    Require (from <= to);
    if (from != to) {
        fBuckets.push_back (RunElement (nullptr, to - from));
    }
    using ScriptRunElt              = TextLayoutBlock::ScriptRunElt;
    vector<ScriptRunElt> scriptRuns = text.GetScriptRuns ();
    for (auto i = scriptRuns.begin (); i != scriptRuns.end (); ++i) {
        Assert ((*i).fRealEnd <= (to - from));
        SplitIfNeededAt (from + (*i).fRealEnd);
    }
}

void StyleMarkerSummarySink::Append (Marker* m)
{
    RequireNotNull (m);
    StyleMarker* styleMarker = dynamic_cast<StyleMarker*> (m);
    if (styleMarker != nullptr) {
        size_t start = max (styleMarker->GetStart (), fFrom);
        size_t end   = min (styleMarker->GetEnd (), fTo);

        /*
         *  Assure the marker were adding will fit neatly.
         */
        SplitIfNeededAt (start);
        SplitIfNeededAt (end);

        /*
         *  Now walk the buckets and fit the new marker into each bucket as appropriate.
         */
        size_t upTo = fFrom;
        for (auto i = fBuckets.begin (); i != fBuckets.end (); ++i) {
            if (start <= upTo and upTo + (*i).fLength <= end) {
                CombineElements (Traversal::Iterator2Pointer (i), styleMarker);
            }
            upTo += (*i).fLength;
        }
    }
}

/*
@METHOD:        StyledTextImager::StyleMarkerSummarySink::SplitIfNeededAt
@DESCRIPTION:   <p>Private routine to split the current list of buckets at a particular position. Called with
            the endpoints of a new marker.</p>
                <p>Note that this routine keeps the buckets in order sorted by their REAL OFFSET (not visual display)
            index. We don't even keep track of the index explicitly in the buckets: we compute it based on the fLength
            field in the buckets and their offset from the summary sink start ('this->fFrom').</p>
*/
void StyleMarkerSummarySink::SplitIfNeededAt (size_t markerPos)
{
    Require (markerPos >= fFrom);
    Require (markerPos <= fTo);
    size_t upTo = fFrom;
    for (auto i = fBuckets.begin (); i != fBuckets.end (); ++i) {
        size_t eltStart = upTo;
        size_t eltEnd   = upTo + (*i).fLength;
        if (markerPos >= eltStart and markerPos <= eltEnd and markerPos != eltStart and markerPos != eltEnd) {
#if qDebug
            size_t oldLength = (*i).fLength;
#endif
            // then we need a split at that position.
            RunElement newElt = *i;
            (*i).fLength      = markerPos - eltStart;
            newElt.fLength    = eltEnd - markerPos;
            Assert (oldLength == (*i).fLength + newElt.fLength);
            Assert ((*i).fLength != 0);
            Assert (newElt.fLength != 0);
            fBuckets.insert (i + 1, newElt);
            break;
        }
        upTo += (*i).fLength;
    }
}

/*
@METHOD:        StyledTextImager::StyleMarkerSummarySink::CombineElements
@DESCRIPTION:   <p>When two style markers overlap, which one gets todo the drawing? As part of the summarizing
    process (@'StyledTextImager::SummarizeStyleMarkers'), we must chose how to combine markers.</p>
        <p>This default algorithm simply chooses the one with the higher priority.</p>
        <p>Subclassers can OVERRIDE this behavior, and, for instance, restrict paying attention to only a particular
    subtype of '@StyleMarker's, or maybe to set particular values into one (chosen subtype marker to connote the overlap, and
    allow this to effect the draw. Or, perhaps, a subclass might ingnore markers with a particular owner value.</p>
        <p>Note that markers NOT used can be placed in the @'StyledTextImager::RunElement's 'fSupercededMarkers' array,
    so that the eventual marker which does the drawing <em>can</em> delegate or combine the drawing behaviors of
    different kinds of markers.</p>
        <p>Note also that this routine will somewhat randomly deal with ties. The first element of a given
    priority wins. But - because of how this is typcially called - as the result of a collection of markers
    from a TextStore - that results in random choices. That can cause trouble - so try to avoid ties
    without GOOD motivation.</p>
*/
void StyleMarkerSummarySink::CombineElements (StyledTextImager::RunElement* runElement, StyleMarker* newStyleMarker)
{
    RequireNotNull (runElement);
    RequireNotNull (newStyleMarker);

    if (runElement->fMarker == nullptr) {
        runElement->fMarker = newStyleMarker;
    }
    else {
        bool newEltStronger = runElement->fMarker->GetPriority () < newStyleMarker->GetPriority ();
#if qAssertWarningForEqualPriorityMarkers
        Assert (runElement->fMarker->GetPriority () != newStyleMarker->GetPriority ());
#endif
        if (newEltStronger) {
            runElement->fSupercededMarkers.push_back (runElement->fMarker);
            runElement->fMarker = newStyleMarker;
        }
        else {
            runElement->fSupercededMarkers.push_back (newStyleMarker);
        }
    }
}

/*
@METHOD:        StyledTextImager::StyleMarkerSummarySink::ProduceOutputSummary
@DESCRIPTION:   <p>Create a vector of @'StyledTextImager::RunElement's. Each of these contains a list of
            marker objects for the region and a length field. The elements are returned in VIRTUAL (LTR display)
            order - NOT logical (internal memory buffer) order. The elements are guarantied not to cross
            any directional boundaries (as returned from the @'TextLayoutBlock::GetScriptRuns' API)</p>
*/
vector<StyledTextImager::RunElement> StyledTextImager::StyleMarkerSummarySink::ProduceOutputSummary () const
{
    using ScriptRunElt = TextLayoutBlock::ScriptRunElt;
    // Soon fix to use fText as a REFERENCE. Then we probably should have this code assure its re-ordering is done only once and then cached,
    // LGP 2002-12-16
    if (fText != nullptr) {
        vector<RunElement>   runElements;
        vector<ScriptRunElt> scriptRuns = fText->GetScriptRuns ();
        if (scriptRuns.size () > 1) {
            // sort by virtual start
            sort (scriptRuns.begin (), scriptRuns.end (), TextLayoutBlock::LessThanVirtualStart ());
        }
        for (auto i = scriptRuns.begin (); i != scriptRuns.end (); ++i) {
            // Grab all StyledTextImager::RunElement elements from this run and copy them out
            const ScriptRunElt& se                 = *i;
            size_t              styleRunStart      = 0;
            size_t              runEltsBucketStart = runElements.size ();
            for (auto j = fBuckets.begin (); j != fBuckets.end (); ++j) {
                size_t styleRunEnd = styleRunStart + (*j).fLength;
                if (se.fRealStart <= styleRunStart and styleRunEnd <= se.fRealEnd) {
                    if (se.fDirection == eLeftToRight) {
                        runElements.push_back (*j);
                    }
                    else {
                        runElements.insert (runElements.begin () + runEltsBucketStart, *j);
                    }
                }
                styleRunStart = styleRunEnd;
            }
        }

        Ensure (runElements.size () == fBuckets.size ());
        return runElements;
    }
    return fBuckets;
}

/*
 ********************************************************************************
 ******************** StyleMarkerSummarySinkForSingleOwner **********************
 ********************************************************************************
 */
using StyleMarkerSummarySinkForSingleOwner = StyledTextImager::StyleMarkerSummarySinkForSingleOwner;

StyleMarkerSummarySinkForSingleOwner::StyleMarkerSummarySinkForSingleOwner (const MarkerOwner& owner, size_t from, size_t to)
    : inherited (from, to)
    , fOwner (owner)
{
}

StyleMarkerSummarySinkForSingleOwner::StyleMarkerSummarySinkForSingleOwner (const MarkerOwner& owner, size_t from, size_t to, const TextLayoutBlock& text)
    : inherited (from, to, text)
    , fOwner (owner)
{
}

/*
@METHOD:        StyledTextImager::StyleMarkerSummarySinkForSingleOwner::CombineElements
@DESCRIPTION:   <p>Like @'StyledTextImager::StyleMarkerSummarySink::CombineElements', except that matching
    the MarkerOwner is more important than the Marker Priority.</p>
*/
void StyleMarkerSummarySinkForSingleOwner::CombineElements (StyledTextImager::RunElement* runElement, StyleMarker* newStyleMarker)
{
    RequireNotNull (runElement);
    RequireNotNull (newStyleMarker);

    if (runElement->fMarker == nullptr) {
        runElement->fMarker = newStyleMarker;
    }
    else {
        bool newEltStronger  = runElement->fMarker->GetPriority () < newStyleMarker->GetPriority ();
        bool newMatchesOwner = newStyleMarker->GetOwner () == &fOwner;
        bool oldMatchesOwner = runElement->fMarker->GetOwner () == &fOwner;
        if (newMatchesOwner != oldMatchesOwner) {
            newEltStronger = newMatchesOwner;
        }
        if (newEltStronger) {
            runElement->fSupercededMarkers.push_back (runElement->fMarker);
            runElement->fMarker = newStyleMarker;
        }
        else {
            runElement->fSupercededMarkers.push_back (newStyleMarker);
        }
    }
}

/*
 ********************************************************************************
 ****************************** StyledTextImager ********************************
 ********************************************************************************
 */
/*
@METHOD:        StyledTextImager::SummarizeStyleMarkers
@DESCRIPTION:   <p>Create a summary of the style markers applied to a given range of text (by default using
    @'StyledTextImager::StyleMarkerSummarySink') into @'StyledTextImager::RunElement's.</p>
*/
vector<RunElement> StyledTextImager::SummarizeStyleMarkers (size_t from, size_t to) const
{
    // See SPR#1293 - may want to get rid of this eventually
    StyleMarkerSummarySink summary (from, to);
    GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, summary);
    return summary.ProduceOutputSummary ();
}

/*
@METHOD:        StyledTextImager::SummarizeStyleMarkers
@DESCRIPTION:   <p>Create a summary of the style markers applied to a given range of text (by default using
    @'StyledTextImager::StyleMarkerSummarySink') into @'StyledTextImager::RunElement's.</p>
*/
vector<RunElement> StyledTextImager::SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const
{
    StyleMarkerSummarySink summary (from, to, text);
    GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, summary);
    return summary.ProduceOutputSummary ();
}

/*
@METHOD:        StyledTextImager::DrawSegment
@DESCRIPTION:   <p>Override @'StyledTextImager::DrawSegment' to break the given segment into subsets based on
            what @'StyledTextImager::StyleMarker' are present in the text. This breakup is done by
            @'StyledTextImager::SummarizeStyleMarkers'.</p>
*/
void StyledTextImager::DrawSegment (Led_Tablet tablet,
                                    size_t from, size_t to, const TextLayoutBlock& text,
                                    const Led_Rect& drawInto, const Led_Rect& invalidRect,
                                    Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn)
{
    /*
     *  Note that SummarizeStyleMarkers assures 'outputSummary' comes out in VIRTUAL order.
     *  Must display text in LTR virtual display order.
     */
    vector<RunElement> outputSummary = SummarizeStyleMarkers (from, to, text);

    Led_Rect tmpDrawInto          = drawInto;
    size_t   outputSummaryLength  = outputSummary.size ();
    size_t   indexIntoText_VISUAL = 0;
    if (pixelsDrawn != nullptr) {
        *pixelsDrawn = 0;
    }

    for (size_t i = 0; i < outputSummaryLength; i++) {
        const RunElement& re       = outputSummary[i];
        size_t            reLength = re.fLength;
        size_t            reFrom   = text.MapVirtualOffsetToReal (indexIntoText_VISUAL) + from; // IN LOGICAL OFFSETS!!!
        size_t            reTo     = reFrom + reLength;                                         // ""
        Assert (indexIntoText_VISUAL <= to - from);
        Assert (reLength > 0);
        /*
         *  Do logical clipping across segments.
         *
         *      We could be more aggressive, and do logical clipping WITHIN segments, but that would have
         *  little potential benefit. The most important cases are embeddings which are one-char
         *  long and all-or-nothing draws. Also, todo so would impose greater complexity in
         *  dealing with the following:
         *
         *      We cannot even totally do logical clipping at the segment boundaries. This is because
         *  we allow a character to draw into an adjacent character cell (but we only allow it to
         *  overwrite the ONE ADJACENT CELL). This happens, for example, with italics, and ligatures,
         *  etc.
         *
         *      So in our logical clipping, we must not clip out segments which are only outside the
         *  logical clipping rect by a single character.
         *
         *
         *  NB: Doing this RIGHT will be easy if we have access to the whole measured-text. But for now we don't.
         *  So simply (as a temp hack) use some fixed number of pixels to optimize by.
         *
         */
        const Led_Coordinate kSluffToLeaveRoomForOverhangs = 20; // cannot imagine more pixel overhang than this,
        // and its a tmp hack anyhow - LGP 960516
        if (tmpDrawInto.left - GetHScrollPos () < invalidRect.right + kSluffToLeaveRoomForOverhangs) {
            Led_Distance pixelsDrawnHere = 0;

            /*
             *  This is a BIT of a kludge. No time to throughly clean this up right now. This is vaguely
             *  related to SPR#1210 and SPR#1108.
             *
             *  All our code REALLY pays attention to (for the most part) is the LHS of the rectangle. But occasionally
             *  we pay attention to the RHS. We have funky sloppy semantics with respect to the GetHScrollPos and this rectangle.
             *  The rectange - I believe - is supposed to be in WINDOW coordinates. But - its implicitly offset
             *  for drawing purposes by the GetHScrollPos (SOMEWHAT - not totally because of tabstops).
             *
             *  Anyhow - some code wants it one way - and other another way. CLEAN THIS UP in the future - probably when
             *  I support scaling - probably be RE-STRUCTRURING my coordinate systems (all floats? and inches or TWIPS?).
             *
             */
            tmpDrawInto.right = drawInto.right + GetHScrollPos ();
            if (tmpDrawInto.left < tmpDrawInto.right) {
                if (re.fMarker == nullptr) {
                    DrawSegment_ (tablet, GetDefaultFont (), reFrom, reTo, TextLayoutBlock_VirtualSubset (text, indexIntoText_VISUAL, indexIntoText_VISUAL + reLength),
                                  tmpDrawInto, useBaseLine, &pixelsDrawnHere);
                }
                else {
                    re.fMarker->DrawSegment (this, re, tablet, reFrom, reTo, TextLayoutBlock_VirtualSubset (text, indexIntoText_VISUAL, indexIntoText_VISUAL + reLength),
                                             tmpDrawInto, invalidRect, useBaseLine, &pixelsDrawnHere);
                }
            }
            if (pixelsDrawn != nullptr) {
                *pixelsDrawn += pixelsDrawnHere;
            }
            tmpDrawInto.left += pixelsDrawnHere;
        }
        indexIntoText_VISUAL += reLength;
    }
}

void StyledTextImager::MeasureSegmentWidth (size_t from, size_t to, const Led_tChar* text,
                                            Led_Distance* distanceResults) const
{
    // See SPR#1293 - may want to pass in TextLayoutBlock here - instead of just plain text...
    vector<RunElement> outputSummary = SummarizeStyleMarkers (from, to);

    size_t outputSummaryLength = outputSummary.size ();
    size_t indexIntoText       = 0;
    for (size_t i = 0; i < outputSummaryLength; i++) {
        const RunElement& re       = outputSummary[i];
        size_t            reFrom   = indexIntoText + from;
        size_t            reLength = re.fLength;
        size_t            reTo     = reFrom + reLength;
        Assert (indexIntoText <= to - from);
        if (re.fMarker == nullptr) {
            MeasureSegmentWidth_ (GetDefaultFont (), reFrom, reTo, &text[indexIntoText], &distanceResults[indexIntoText]);
        }
        else {
            re.fMarker->MeasureSegmentWidth (this, re, reFrom, reTo, &text[indexIntoText], &distanceResults[indexIntoText]);
        }
        if (indexIntoText != 0) {
            Led_Distance addX = distanceResults[indexIntoText - 1];
            for (size_t j = 0; j < reLength; j++) {
                distanceResults[indexIntoText + j] += addX;
            }
        }
        indexIntoText += reLength;
    }
}

Led_Distance StyledTextImager::MeasureSegmentHeight (size_t from, size_t to) const
{
    // See SPR#1293 - may want to pass in TextLayoutBlock here ... and then pass that to SummarizeStyleMarkers ()
    Require (from <= to);
    if (from == to) { // HACK/TMP? SO WE GET AT LEAST ONE SUMMARY RECORD?? LGP 951018
        to = from + 1;
    }

    vector<RunElement> outputSummary = SummarizeStyleMarkers (from, to);

    /*
     *  This is somewhat subtle.
     *
     *  If we have a mixture of pictures and text on the same line, we want to
     *  have the pictures resting on the baseline (aligned along the bottom edge
     *  even if the picts have different height).
     *
     *  We also want decenders (like the bottom of a g) to go BELOW the picture.
     *
     *  This behavior isn't anything I dreamed up. And its not what I implemented
     *  originally. I've copied the behavior of other editors. So presumably
     *  somebody put some thought into the reasons for this. They are lost
     *  on me :-) -- LGP 960314
     */
    size_t outputSummaryLength = outputSummary.size ();
    Assert (outputSummaryLength != 0);
    Led_Distance maxHeightAbove = 0;
    Led_Distance maxHeightBelow = 0;
    size_t       indexIntoText  = 0;
    for (size_t i = 0; i < outputSummaryLength; i++) {
        const RunElement& re       = outputSummary[i];
        size_t            reFrom   = indexIntoText + from;
        size_t            reLength = re.fLength;
        size_t            reTo     = reFrom + reLength;
        Assert (indexIntoText <= to - from);
        Led_Distance itsBaseline;
        Led_Distance itsHeight;
        if (re.fMarker == nullptr) {
            itsBaseline = MeasureSegmentBaseLine_ (GetDefaultFont (), reFrom, reTo);
            itsHeight   = MeasureSegmentHeight_ (GetDefaultFont (), reFrom, reTo);
        }
        else {
            itsBaseline = re.fMarker->MeasureSegmentBaseLine (this, re, reFrom, reTo);
            itsHeight   = re.fMarker->MeasureSegmentHeight (this, re, reFrom, reTo);
        }
        maxHeightAbove = max (maxHeightAbove, itsBaseline);
        maxHeightBelow = max (maxHeightBelow, (itsHeight - itsBaseline));
        indexIntoText += reLength;
    }
    return maxHeightAbove + maxHeightBelow;
}

Led_Distance StyledTextImager::MeasureSegmentBaseLine (size_t from, size_t to) const
{
    // See SPR#1293 - may want to pass in TextLayoutBlock here ... and then pass that to SummarizeStyleMarkers ()
    Require (from <= to);
    if (from == to) { // HACK/TMP? SO WE GET AT LEAST ONE SUMMARY RECORD?? LGP 951018
        to = from + 1;
    }

    vector<RunElement> outputSummary       = SummarizeStyleMarkers (from, to);
    size_t             outputSummaryLength = outputSummary.size ();
    Assert (outputSummaryLength != 0);
    Led_Distance maxHeight     = 0;
    size_t       indexIntoText = 0;
    for (size_t i = 0; i < outputSummaryLength; i++) {
        const RunElement& re       = outputSummary[i];
        size_t            reFrom   = indexIntoText + from;
        size_t            reLength = re.fLength;
        size_t            reTo     = reFrom + reLength;
        Assert (indexIntoText <= to - from);
        if (re.fMarker == nullptr) {
            maxHeight = max (maxHeight, MeasureSegmentBaseLine_ (GetDefaultFont (), reFrom, reTo));
        }
        else {
            maxHeight = max (maxHeight, re.fMarker->MeasureSegmentBaseLine (this, re, reFrom, reTo));
        }
        indexIntoText += reLength;
    }
    return maxHeight;
}

#if qDebug
void StyledTextImager::Invariant_ () const
{
}
#endif

/*
 ********************************************************************************
 ************************** TrivialFontSpecStyleMarker **************************
 ********************************************************************************
 */
int TrivialFontSpecStyleMarker::GetPriority () const
{
    return eBaselinePriority + 1;
}
