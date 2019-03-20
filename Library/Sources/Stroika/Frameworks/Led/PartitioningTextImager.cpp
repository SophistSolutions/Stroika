/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "PartitioningTextImager.h"

using namespace Stroika::Foundation;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

using PartitionMarker = Partition::PartitionMarker;

/*
 ********************************************************************************
 ************************************ Partition *********************************
 ********************************************************************************
 */
Partition::Partition (TextStore& textStore)
    :
#if qDebug
    fFinalConstructCalled (false)
    ,
#endif
    fTextStore (textStore)
    , fFindContainingPMCache (nullptr)
    , fPartitionWatchers ()
    , fMarkersToBeDeleted ()
    ,
#if qDebug
    fPartitionMarkerCount (0)
    ,
#endif
    fPartitionMarkerFirst (nullptr)
    , fPartitionMarkerLast (nullptr)
{
    fTextStore.AddMarkerOwner (this);
}

Partition::~Partition ()
{
    // This was commented out as of 20000119 - not sure why... If it causes trouble maybe
    // OK to just force empty here/now. But understand if so/why...
    Assert (fMarkersToBeDeleted.IsEmpty ()); // these better be deleted by now!

    /*
     *      We could simply loop and remove all markers with RemoveMarker().
     *  But using RemoveMarkers() is much faster. The trouble is we may
     *  not have enough memory to do the removeal, and an exception here
     *  would be very bad (deleting object shouldn't fail cuz not enuf memory!).
     *
     *      So we compromise, and use a small stack buffer to delete as much
     *  as we can at a time. For must cases, this will be plenty.
     *
     *      The alternative was to use a Led_Array - and if we ran out of memory,
     *  then to fall back to old slow strategy. That should work fine too, but I think
     *  this is slightly simpler.
     */
    for (PartitionMarker* cur = fPartitionMarkerFirst; cur != nullptr;) {
        Marker*      markersToRemoveAtATime[1000];
        const size_t kMaxBufMarkers = NEltsOf (markersToRemoveAtATime);
        size_t       i              = 0;
        for (; i < kMaxBufMarkers and cur != nullptr; i++, cur = cur->fNext) {
            markersToRemoveAtATime[i] = cur;
        }
        fTextStore.RemoveMarkers (markersToRemoveAtATime, i);
#if qDebug
        fPartitionMarkerCount -= i;
#endif
        for (; i != 0; i--) {
            delete (markersToRemoveAtATime[i - 1]);
        }
    }
    fTextStore.RemoveMarkerOwner (this);
    Ensure (fPartitionMarkerCount == 0);
}

/*
@METHOD:        Partition::FinalConstruct
@DESCRIPTION:   <p>Logically this should be part of the constructor. But since it needs to call a method
    (@'Partition::MakeNewPartitionMarker ()') which is virtual, and must be bound in the final complete
    class. So it must be done as a separate call. It is illegal to call this more than once, or to call any
    of the other class methods without having called it. These errors will be detected (in debug builds)
    where possible.</p>
 */
void Partition::FinalConstruct ()
{
#if qDebug
    Require (not fFinalConstructCalled);
    fFinalConstructCalled = true;
#endif
    Assert (fPartitionMarkerCount == 0);
    Assert (fPartitionMarkerFirst == nullptr);
    PartitionMarker* pm = MakeNewPartitionMarker (nullptr);
#if qDebug
    fPartitionMarkerCount++;
#endif
    fTextStore.AddMarker (pm, 0, fTextStore.GetLength () + 1, this); // include ALL text
    Assert (fPartitionMarkerFirst == pm);
    Assert (fPartitionMarkerLast == pm);
}

TextStore* Partition::PeekAtTextStore () const
{
    return &fTextStore;
}

/*
@METHOD:        Partition::GetPartitionMarkerContainingPosition
@DESCRIPTION:   <p>Finds the @'PartitioningTextImager::PartitionMarker' which contains the given character#.
    Note, the use of 'charPosition' rather than markerpos is to disambiguiate the case where we are at the boundary
    between two partition elements.</p>
*/
PartitionMarker* Partition::GetPartitionMarkerContainingPosition (size_t charPosition) const
{
    Require (fFinalConstructCalled);
    Require (charPosition <= GetEnd () + 1); // cuz last PM contains bogus char past end of buffer

    /*
     *  Based on cached value, either search forwards from there or back.
     */
    PartitionMarker* pm = fFindContainingPMCache;
    if (pm == nullptr) {
        pm = fPartitionMarkerFirst; // could be first time, or could have deleted cached value
    }
    AssertNotNull (pm);

    if (charPosition < 10) {
        pm = fPartitionMarkerFirst;
    }
    else if (charPosition + 10 > GetEnd ()) {
        pm = fPartitionMarkerLast;
    }

    bool loopForwards = pm->GetStart () <= charPosition;
    for (; pm != nullptr; pm = loopForwards ? pm->fNext : pm->fPrevious) {
        if (Contains (*pm, charPosition)) {
            if (pm != fPartitionMarkerFirst and pm != fPartitionMarkerLast) {
                fFindContainingPMCache = pm;
            }
            EnsureNotNull (pm);
            return (pm);
        }
    }
    AssertNotReached ();
    return (nullptr);
}

/*
@METHOD:        Partition::MakeNewPartitionMarker
@DESCRIPTION:   <p>Method which is called to construct new partition elements. Override this if you subclass
    @'PartitioningTextImager', and want to provide your own subtype of @'PartitioningTextImager::PartitionMarker'.</p>
*/
PartitionMarker* Partition::MakeNewPartitionMarker (PartitionMarker* insertAfterMe)
{
    Require (fFinalConstructCalled);
    return (new PartitionMarker (*this, insertAfterMe));
}

/*
@METHOD:        Partition::Split
@DESCRIPTION:   <p>Split the given PartitionMarker at position 'at' - which must fall within the partition element.
    This method always produces a new partition element, and inserts it appropriately into the partition, adjusting
    the sizes of surrounding elements appropriately.</p>
        <p>Ensure that the split happens so that PM continues to point to the beginning of the range and the new PMs created
    are at the end of the range</p>
        <p>This method is typically called by Partition subclasses OVERRIDE of @'Partition::UpdatePartitions'.</p>
*/
void Partition::Split (PartitionMarker* pm, size_t at)
{
    Require (fFinalConstructCalled);
    RequireNotNull (pm);
    Require (pm->GetStart () < at);
    Require (pm->GetEnd () > at);
#if qMultiByteCharacters
    Assert_CharPosDoesNotSplitCharacter (at);
#endif

    vector<void*> watcherInfos;
    DoAboutToSplitCalls (pm, at, &watcherInfos);

    size_t start = 0;
    size_t end   = 0;
    pm->GetRange (&start, &end);
    Assert (at >= start and at <= end);

    // try to cleanup as best as we can if we fail in the middle of adding a new PM.
    PartitionMarker* newPM = MakeNewPartitionMarker (pm);
    Assert (pm->GetNext () == newPM);
    Assert (newPM->GetPrevious () == pm);

    // NB: we set marker range before adding new marker for no good reason, except that
    // for quirky reasons I don't understand, reading in files is MUCH faster that
    // way - I really don't understand the idiocyncracies of the ChunckedArrayTextStore
    // performance... but luckily - no real need to...
    // LGP 960515
    fTextStore.SetMarkerEnd (pm, at);
    try {
        fTextStore.AddMarker (newPM, at, end - at, this);
    }
    catch (...) {
        // the only item above that can throw is the AddMarker () call. So at this point, all we need todo
        // is remove newPM from our linked list, and then delete it. Of course, our Partition elements won't be
        // quite right. But with no memory, this is the best we can do. At least we still have a partition!
        AssertNotNull (newPM);
        if (pm == nullptr) {
            fPartitionMarkerFirst = newPM->fNext;
        }
        else {
            pm->fNext = newPM->fNext;
        }
        // Now patch pm's old and new successor (before and after newPM breifly was it)
        if (newPM->fNext != nullptr) {
            Assert (newPM->fNext->fPrevious == newPM); // but thats getting deleted...
            newPM->fNext->fPrevious = pm;
        }
        if (newPM == fPartitionMarkerLast) {
            fPartitionMarkerLast = newPM->fPrevious;
        }

        // Since we set this BEFORE the AddMarker, we must set it back. Thats the only
        // cost in the above '960515' performance hack - and it only occurs EXTREMELY
        // RARELY and in a case I don't care about.
        // LGP 960515
        fTextStore.SetMarkerRange (pm, start, end);

        throw;
    }
#if qDebug
    fPartitionMarkerCount++;
#endif
    DoDidSplitCalls (watcherInfos);
}

/*
@METHOD:        Partition::Coalece
@DESCRIPTION:   <p>Coalece the given PartitionMarker with the one which follows it (if any exists). If coalesced
    with the following element, the given 'pm' is deleted. Frequently this doesn't happen immediately (mainly
    for performance reasons, but possibly also to avoid bugs where the marker is still in some list being iterated over
    and having DidUpdate() etc methods called on it). It is accumulated for later deletion using
    @'Partition::AccumulateMarkerForDeletion'.</p>
        <p>This method is typically called by Partition subclasses OVERRIDE of @'Partition::UpdatePartitions'.</p>
*/
void Partition::Coalece (PartitionMarker* pm)
{
    Require (fFinalConstructCalled);
    AssertNotNull (pm);
    vector<void*> watcherInfos;
    DoAboutToCoaleceCalls (pm, &watcherInfos);
    if (pm->fNext != nullptr) { // We don't do anything to coalesce the last item - nothing to coalesce with!
        size_t start = 0;
        size_t end   = 0;
        pm->GetRange (&start, &end);
        size_t lengthToAdd = end - start;

        PartitionMarker* successor = pm->fNext;
        AssertNotNull (successor);
        successor->GetRange (&start, &end);
        Assert (start >= lengthToAdd);
        fTextStore.SetMarkerStart (successor, start - lengthToAdd);

        Assert (successor->fPrevious == pm);
        successor->fPrevious = pm->fPrevious;
        Assert ((pm->fPrevious == nullptr) or (pm->fPrevious->fNext == pm));
        if (pm->fPrevious == nullptr) {
            Assert (fPartitionMarkerFirst == pm);
            fPartitionMarkerFirst = pm->fNext;
        }
        else {
            pm->fPrevious->fNext = pm->fNext;
        }
        if (pm->fNext == nullptr) {
            Assert (fPartitionMarkerLast == pm);
            fPartitionMarkerLast = pm->fPrevious;
        }
#if qDebug
        fPartitionMarkerCount--;
#endif
        AccumulateMarkerForDeletion (pm);
    }
    DoDidCoaleceCalls (watcherInfos);
}

/*
@METHOD:        Partition::AccumulateMarkerForDeletion
@DESCRIPTION:   <p>Wrap @'MarkerMortuary<MARKER>::AccumulateMarkerForDeletion' - and make sure our cache
    isn't pointing to a deleted marker.</p>
*/
void Partition::AccumulateMarkerForDeletion (PartitionMarker* m)
{
    Require (fFinalConstructCalled);
    AssertNotNull (m);
    Assert (&m->GetOwner () == this);
    fMarkersToBeDeleted.AccumulateMarkerForDeletion (m);
    if (fFindContainingPMCache == m) {
        fFindContainingPMCache = nullptr;
    }
}

void Partition::AboutToUpdateText (const UpdateInfo& updateInfo)
{
    Require (fFinalConstructCalled);
    Assert (fMarkersToBeDeleted.IsEmpty ()); // would be bad to do a replace with any of these not
    // yet finalized since they would then appear in the
    // CollectAllMarkersInRange() and get DidUpdate calls!
    Invariant ();
    inherited::AboutToUpdateText (updateInfo);
}

void Partition::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    Require (fFinalConstructCalled);
    fMarkersToBeDeleted.FinalizeMarkerDeletions ();
    inherited::DidUpdateText (updateInfo);
    Invariant ();
}

#if qDebug
void Partition::Invariant_ () const
{
    Require (fFinalConstructCalled);
    size_t lastCharDrawn = 0;
    Assert (fPartitionMarkerCount != 0);
    size_t realPMCount = 0;
    for (PartitionMarker* cur = fPartitionMarkerFirst; cur != nullptr; cur = cur->fNext) {
        PartitionMarker* pm = cur;
        AssertNotNull (pm);
        Assert (&pm->GetOwner () == this);
        size_t start = pm->GetStart ();
        size_t end   = pm->GetEnd ();
        size_t len   = end - start;

        Assert (start == lastCharDrawn);
        Assert (end <= GetEnd () + 1); // +1 for extra bogus space so we always get autoexpanded

        lastCharDrawn = end;

        if (end > GetEnd ()) {
            len--; // Last partition extends past end of text
        }
        realPMCount++;
        Assert (realPMCount <= fPartitionMarkerCount);
        Assert (static_cast<bool> (fPartitionMarkerLast == cur) == static_cast<bool> (cur->fNext == nullptr));
    }
    Assert (realPMCount == fPartitionMarkerCount);
    Assert (lastCharDrawn == GetEnd () + 1);
}
#endif

/*
 ********************************************************************************
 ****************************** PartitioningTextImager **************************
 ********************************************************************************
 */
PartitioningTextImager::PartitioningTextImager ()
    : fPartition (nullptr)
#if qCacheTextMeasurementsForPM
    , fMeasureTextCache ()
#endif
{
}

PartitioningTextImager::~PartitioningTextImager ()
{
    Require (fPartition.get () == nullptr);
}

/*
@METHOD:        PartitioningTextImager::SetPartition
@DESCRIPTION:   <p>Associates the given parition (@'PartitionPtr') with this @'PartitioningTextImager'.
    It can be nullptr.</p>
        <p>The method is virtual, in case you need to hook partition changes by updating
    other derived/cached information. But if overridding, be sure to always call the inherited version.</p>
*/
void PartitioningTextImager::SetPartition (const PartitionPtr& partitionPtr)
{
#if qCacheTextMeasurementsForPM
    fMeasureTextCache.reset ();
#endif
    fPartition = partitionPtr;
#if qCacheTextMeasurementsForPM
    if (partitionPtr.get () != nullptr) {
        fMeasureTextCache = unique_ptr<MeasureTextCache> (new MeasureTextCache (partitionPtr));
    }
#endif
}

#if qCacheTextMeasurementsForPM
/*
@METHOD:        PartitioningTextImager::InvalidateAllCaches
@DESCRIPTION:   <p>Hook the @'TextImager::InvalidateAllCaches' message to free some additional caches.</p>
*/
void PartitioningTextImager::InvalidateAllCaches ()
{
    inherited::InvalidateAllCaches ();
    if (fMeasureTextCache.get () != nullptr) {
        fMeasureTextCache->ClearAll ();
    }
}
#endif

/*
@METHOD:        PartitioningTextImager::GetPrimaryPartitionTextDirection
@DESCRIPTION:   <p>Return the primary text direction for the partition. This COULD be specified by some external
            user setting, or - by default - simply computed from the text direction of the first row of the
            paragraph.</p>
*/
TextDirection PartitioningTextImager::GetPrimaryPartitionTextDirection (size_t rowContainingCharPosition) const
{
    return GetTextDirection (GetStartOfPartitionContainingPosition (rowContainingCharPosition));
}

TextLayoutBlock_Copy PartitioningTextImager::GetTextLayoutBlock (size_t rowStart, size_t rowEnd) const
{
    if (rowStart == GetStartOfPartitionContainingPosition (rowStart)) {
        return inherited::GetTextLayoutBlock (rowStart, rowEnd);
    }
    else {
        size_t                              rowLen = rowEnd - rowStart;
        Memory::SmallStackBuffer<Led_tChar> rowBuf (rowLen);
        CopyOut (rowStart, rowLen, rowBuf);
        return TextLayoutBlock_Copy (TextLayoutBlock_Basic (rowBuf, rowBuf + rowLen, GetPrimaryPartitionTextDirection (rowStart)));
    }
}

/*
@METHOD:        PartitioningTextImager::GetTextDirection
@DESCRIPTION:   <p>Implementation of abstract interface @'TextImager::GetTextDirection'</p>
*/
TextDirection PartitioningTextImager::GetTextDirection (size_t charPosition) const
{
    size_t startOfRow = GetStartOfRowContainingPosition (charPosition);
    size_t endOfRow   = GetEndOfRowContainingPosition (startOfRow);
    if (charPosition == endOfRow) {
        return eLeftToRight; //??Not sure what todo here??
        // should probably grab from ambient setting for paragraph direction or something like that
    }
    else {
        return GetTextLayoutBlock (startOfRow, endOfRow).GetCharacterDirection (charPosition - startOfRow);
    }
}

/*
@METHOD:        PartitioningTextImager::CalcSegmentSize
@DESCRIPTION:   <p>Implementation of abstract interface @'TextImager::CalcSegmentSize'</p>
                <p>Note that internally - this must grab the ENTIRE ROW to measure (or nearly so).
            To assure ligatures are measured properly (to could be part of a ligature) we need the characters just past
            'to' to measure it properly. And - to measure tabs properly - we need to go before 'from' all the way to
            the start of the row. So - we just always goto the starts and ends of rows. Since
            @'PartitioningTextImager::CalcSegmentSize_CACHING' caches these values - this isn't a great cost.</p>
*/
Led_Distance PartitioningTextImager::CalcSegmentSize (size_t from, size_t to) const
{
#if !qCacheTextMeasurementsForPM || qDebug
    Led_Distance referenceValue = CalcSegmentSize_REFERENCE (from, to);
#endif

#if qCacheTextMeasurementsForPM
    Led_Distance value = CalcSegmentSize_CACHING (from, to);
    Assert (value == referenceValue);
    return value;
#else
    return referenceValue;
#endif
}

/*
@METHOD:        PartitioningTextImager::CalcSegmentSize_REFERENCE
@ACCESS:        private
@DESCRIPTION:   <p>Similar to @'PartitioningTextImager::CalcSegmentSize_CACHING' but recalculating the measurements
            each time as a check that the cache has not somehow (undetected) become invalid (say cuz a font changed
            and we weren't notified?).</p>
*/
Led_Distance PartitioningTextImager::CalcSegmentSize_REFERENCE (size_t from, size_t to) const
{
    Require (from <= to);

    if (from == to) {
        return 0;
    }
    else {
        size_t startOfRow = GetStartOfRowContainingPosition (from);
        size_t rowEnd     = GetEndOfRowContainingPosition (startOfRow);
        Require (startOfRow <= from); //  WE REQUIRE from/to be contained within a single row!!!
        Require (to <= rowEnd);       //  ''
        size_t                                 rowLen = rowEnd - startOfRow;
        Memory::SmallStackBuffer<Led_Distance> distanceVector (rowLen);
        CalcSegmentSize_FillIn (startOfRow, rowEnd, distanceVector);
        Assert (to > startOfRow);                                                                 // but from could be == startOfRow, so must be careful of that...
        Assert (to - startOfRow - 1 < (GetEndOfRowContainingPosition (startOfRow) - startOfRow)); // now buffer overflows!
        Led_Distance result = distanceVector[to - startOfRow - 1];
        if (from != startOfRow) {
            result -= distanceVector[from - startOfRow - 1];
        }
        return (result);
    }
}

#if qCacheTextMeasurementsForPM
/*
@METHOD:        PartitioningTextImager::CalcSegmentSize_CACHING
@ACCESS:        private
@DESCRIPTION:   <p>Caching implementation of @'PartitioningTextImager::CalcSegmentSize'. Values checked by
            calls to related @'PartitioningTextImager::CalcSegmentSize_REFERENCE'.</p>
*/
Led_Distance PartitioningTextImager::CalcSegmentSize_CACHING (size_t from, size_t to) const
{
    Require (from <= to);

    if (from == to) {
        return 0;
    }
    PartitionMarker* pm = GetPartitionMarkerContainingPosition (from);
    Require (pm->GetEnd () == to or pm == GetPartitionMarkerContainingPosition (to)); // since must be in same row, must be in same PM.

    size_t startOfRow = GetStartOfRowContainingPosition (from);
    Require (GetEndOfRowContainingPosition (startOfRow) >= to); //  WE REQUIRE from/to be contained within a single row!!!

    MeasureTextCache::CacheElt ce                = fMeasureTextCache->LookupValue (pm, startOfRow, [this](PartitionMarker* pm, size_t startOfRow) {
        MeasureTextCache::CacheElt newCE{MeasureTextCache::CacheElt::COMPARE_ITEM (pm, startOfRow)};
        size_t                     rowEnd = GetEndOfRowContainingPosition (startOfRow);
        size_t                     rowLen = rowEnd - startOfRow;
        newCE.fMeasurementsCache.GrowToSize (rowLen);
        CalcSegmentSize_FillIn (startOfRow, rowEnd, newCE.fMeasurementsCache);
        return newCE;
    });
    const Led_Distance*        measurementsCache = ce.fMeasurementsCache;

    Assert (to > startOfRow);                                                                 // but from could be == startOfRow, so must be careful of that...
    Assert (to - startOfRow - 1 < (GetEndOfRowContainingPosition (startOfRow) - startOfRow)); // now buffer overflows!
    Led_Distance result = measurementsCache[to - startOfRow - 1];
    if (from != startOfRow) {
        result -= measurementsCache[from - startOfRow - 1];
    }
    return (result);
}
#endif

/*
@METHOD:        PartitioningTextImager::CalcSegmentSize_FillIn
@ACCESS:        private
@DESCRIPTION:   <p>The 'rowStart' argument MUST start a row, and rowEnd must END that same row. 'distanceVector' must be a
            non-null array whose  size is set to at least (rowEnd-rowStart) elements.</p>
*/
void PartitioningTextImager::CalcSegmentSize_FillIn (size_t rowStart, size_t rowEnd, Led_Distance* distanceVector) const
{
    Require (rowStart == GetStartOfRowContainingPosition (rowStart)); // must already be a rowstart
    Require (rowEnd == GetEndOfRowContainingPosition (rowStart));     // ""
    RequireNotNull (distanceVector);
    Require (rowStart <= rowEnd);

    // we must re-snag the text to get the width/tab alignment of the initial segment (for reset tabstops)- a bit more complicated ...
    size_t len = rowEnd - rowStart;

    Memory::SmallStackBuffer<Led_tChar> fullRowTextBuf (len);
    CopyOut (rowStart, len, fullRowTextBuf);

    MeasureSegmentWidth (rowStart, rowEnd, fullRowTextBuf, distanceVector);
    (void)ResetTabStops (rowStart, fullRowTextBuf, len, distanceVector, 0);
}

/*
@METHOD:        PartitioningTextImager::GetRowRelativeCharLoc
@DESCRIPTION:   <p>Implementation of abstract interface @'TextImager::GetRowRelativeCharLoc'</p>
*/
void PartitioningTextImager::GetRowRelativeCharLoc (size_t charLoc, Led_Distance* lhs, Led_Distance* rhs) const
{
    Require (charLoc <= GetEnd ());
    RequireNotNull (lhs);
    RequireNotNull (rhs);

    /*
     *  Note that this algoritm assumes that TextImager::CalcSegmentSize () measures the VIRTUAL characters,
     *  including any mapped characters (mirroring) that correspond to the argument REAL character range.
     */
    size_t                  rowStart = GetStartOfRowContainingPosition (charLoc);
    size_t                  rowEnd   = GetEndOfRowContainingPosition (charLoc);
    [[maybe_unused]] size_t rowLen   = rowEnd - rowStart;
    TextLayoutBlock_Copy    rowText  = GetTextLayoutBlock (rowStart, rowEnd);

    using ScriptRunElt        = TextLayoutBlock::ScriptRunElt;
    vector<ScriptRunElt> runs = rowText.GetScriptRuns ();

    /*
     *  Walk through the runs in VIRTUAL order (screen left to right). Find the run whose charLoc is inside
     *  the "REAL" run span. Stop measuring there.
     */
    Assert (not runs.empty () or (rowLen == 0));
    if (runs.size () > 1) {
        // sort by virtual start
        sort (runs.begin (), runs.end (), TextLayoutBlock::LessThanVirtualStart ());
    }
    size_t       rowRelCharLoc = charLoc - rowStart;
    Led_Distance spannedSoFar  = 0;
    for (auto i = runs.begin (); i != runs.end (); ++i) {
        const ScriptRunElt& se        = *i;
        size_t              runLength = se.fRealEnd - se.fRealStart;
        // See if we are STRICTLY inside the run, or maybe at the last character of the last run
        if ((se.fRealStart <= rowRelCharLoc) and
            ((rowRelCharLoc < se.fRealEnd) or ((rowRelCharLoc == se.fRealEnd) and (i + 1 == runs.end ())))) {
            size_t absoluteSegStart = rowStart + se.fRealStart;
            size_t subSegLen        = rowRelCharLoc - se.fRealStart;
            Assert (subSegLen <= runLength);

            size_t nextPosition = FindNextCharacter (charLoc);
            bool   emptyChar    = (nextPosition == charLoc);
            if (not emptyChar) {
                Led_tChar lastChar = '\0';
                CopyOut (charLoc, 1, &lastChar);
                emptyChar = (RemoveMappedDisplayCharacters (&lastChar, 1) == 0);
            }

            // EXPLAIN LOGIC (Once I have it right) ;-)
            if (se.fDirection == eLeftToRight) {
                *lhs = spannedSoFar + CalcSegmentSize (absoluteSegStart, absoluteSegStart + subSegLen);
                if (emptyChar) {
                    *rhs = *lhs;
                }
                else {
                    *rhs = spannedSoFar + CalcSegmentSize (absoluteSegStart, FindNextCharacter (absoluteSegStart + subSegLen));
                }
            }
            else {
                Led_Coordinate segRHS = spannedSoFar + CalcSegmentSize (absoluteSegStart, absoluteSegStart + runLength);
                *rhs                  = segRHS - CalcSegmentSize (absoluteSegStart, absoluteSegStart + subSegLen);
                if (emptyChar) {
                    *lhs = *rhs;
                }
                else {
                    *lhs = segRHS - CalcSegmentSize (absoluteSegStart, FindNextCharacter (absoluteSegStart + subSegLen));
                }
            }
            break;
        }
        else {
            spannedSoFar += CalcSegmentSize (rowStart + se.fRealStart, rowStart + se.fRealEnd);
        }
    }
    Ensure (*lhs <= *rhs); // can be equal for case like 'RemoveMappedDisplayCharacters'
}

/*
@METHOD:        PartitioningTextImager::GetRowRelativeCharAtLoc
@DESCRIPTION:   <p>Implementation of abstract interface @'TextImager::GetRowRelativeCharAtLoc'</p>
*/
size_t PartitioningTextImager::GetRowRelativeCharAtLoc (Led_Coordinate hOffset, size_t rowStart) const
{
    Require (rowStart == GetStartOfRowContainingPosition (rowStart));

    /*
     *  Note that this algoritm assumes that TextImager::CalcSegmentSize () measures the VIRTUAL characters,
     *  including any mapped characters (mirroring) that correspond to the argument REAL character range.
     */
    size_t                  rowEnd  = GetEndOfRowContainingPosition (rowStart);
    [[maybe_unused]] size_t rowLen  = rowEnd - rowStart;
    TextLayoutBlock_Copy    rowText = GetTextLayoutBlock (rowStart, rowEnd);

    using ScriptRunElt        = TextLayoutBlock::ScriptRunElt;
    vector<ScriptRunElt> runs = rowText.GetScriptRuns ();

    /*
     *  Walk through the runs in VIRTUAL order (screen left to right). Find the run whose hOffset is inside
     *  Then find the character which hOffset resides in (within that run).
     */
    Assert (not runs.empty () or (rowLen == 0));
    if (runs.size () > 1) {
        // sort by virtual start
        sort (runs.begin (), runs.end (), TextLayoutBlock::LessThanVirtualStart ());
    }
    Led_Distance  spannedSoFar = 0;
    TextDirection lastRunDir   = eLeftToRight;
    for (auto i = runs.begin (); i != runs.end (); ++i) {
        const ScriptRunElt& se            = *i;
        Led_Distance        thisSpanWidth = CalcSegmentSize (rowStart + se.fRealStart, rowStart + se.fRealEnd);
        Led_Distance        segVisStart   = spannedSoFar;
        Led_Distance        segVisEnd     = segVisStart + thisSpanWidth;

        lastRunDir = se.fDirection;

        if (hOffset < static_cast<Led_Coordinate> (segVisEnd)) {
            /*
             *  Must be this segment. NB: this takes care of special case where mouseLoc is BEFORE first segment in which case
             *  we treat as at the start of that segment...
             *
             *  Now walk through the segment and see when we have enough chars gone by within the segment to
             *  get us past 'hOffset'
             */
            size_t absoluteSegStart = rowStart + se.fRealStart;
            //size_t segLen           = se.fRealEnd - se.fRealStart;

            size_t prevEnd = rowStart + se.fRealStart;
            size_t segEnd  = rowStart + se.fRealEnd;
            for (size_t curEnd = FindNextCharacter (prevEnd); curEnd < segEnd; (prevEnd = curEnd), (curEnd = FindNextCharacter (curEnd))) {
                Led_Distance hSize = CalcSegmentSize (absoluteSegStart, curEnd);
                if (se.fDirection == eLeftToRight) {
                    if (static_cast<Led_Coordinate> (hSize + spannedSoFar) > hOffset) {
#if qMultiByteCharacters
                        Assert_CharPosDoesNotSplitCharacter (prevEnd);
#endif
                        //DbgTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (LTR)", hOffset, prevEnd);
                        return (prevEnd);
                    }
                }
                else {
                    if (static_cast<Led_Coordinate> (segVisEnd) - static_cast<Led_Coordinate> (hSize) < hOffset) {
#if qMultiByteCharacters
                        Assert_CharPosDoesNotSplitCharacter (prevEnd);
#endif
                        //DbgTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (RTL)", hOffset, prevEnd);
                        return (prevEnd);
                    }
                }
            }
            //DbgTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (EOS)", hOffset, prevEnd);
            return (prevEnd);
        }

        spannedSoFar += thisSpanWidth;
    }

    Assert (hOffset > 0 or runs.size () == 0);
    if (lastRunDir == eLeftToRight) {
        //DbgTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (EOR-LTR)", hOffset, rowEnd);
        return rowEnd;
    }
    else {
        //DbgTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (EOR-RTL)", hOffset, rowStart);
        return rowStart;
    }
}

/*
@METHOD:        PartitioningTextImager::ResetTabStops
@DESCRIPTION:   <p>Patch the charLocations for tab-stop locations, now that we know our previous wrap point.
    Returns index of last tab found (allows for optimizations). Returns a number <= from if
    no tabs found.</p>
        <p>'from' defines where in the 'textStore' all these things are being measured relative to (typically the start of
    a partition).</p>
        <p>'text' is the text starting in the partition at position 'from', and extending 'nTChars' Led_tChars in length.</p>
        <p>'charLocations' is a vector - of the widths, measured from @'TextImager::MeasureSegmentWidth'. As such, it
    measures distances from 'from' to a position 'i' after from, with the distance of from..from being implied zero, so the first
    elt of the array left out (in other words to find distance from..i, you say charLocations[i-from-1]).</p>
        <p>'startSoFar' is an arg so you can only reset the tabstops for a given subset of the text - ignoring the initial portion.
    So, if startSoFar==0, then it is assumed we are starting at the beginning of the charLocations array, but if startSoFar != 0,
    we assume we can (and must) snag our starting width from what is already in the array at charLocations[startSoFar-1].</p>
*/
size_t PartitioningTextImager::ResetTabStops (size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const
{
    RequireNotNull (charLocations);
    size_t         lastTabIndex = 0;
    Led_Coordinate tabAdjust    = 0;
    Led_Distance   widthAtStart = (startSoFar == 0 ? 0 : charLocations[startSoFar - 1]);
    for (size_t i = startSoFar; i < startSoFar + nTChars; i++) {
        if (text[i] == '\t') {
            Led_Distance widthSoFar = (i == 0 ? 0 : charLocations[i - 1]);
            tabAdjust               = widthAtStart + GetTabStopList (from).ComputeTabStopAfterPosition (Tablet_Acquirer (this), widthSoFar - widthAtStart) - charLocations[i];
            lastTabIndex            = i;
        }
        charLocations[i] += tabAdjust;
    }
    return (lastTabIndex);
}

#if qDebug
void PartitioningTextImager::Invariant_ () const
{
    if (fPartition.get () != nullptr) {
        fPartition->Invariant ();
        Assert (fPartition->PeekAtTextStore () == PeekAtTextStore ());
    }
}
#endif

#if qCacheTextMeasurementsForPM
/*
 ********************************************************************************
 ************* PartitioningTextImager::MeasureTextCache *************************
 ********************************************************************************
 */
PartitioningTextImager::MeasureTextCache::MeasureTextCache (const PartitionPtr& partition)
    : fPartition (partition)
    , fCache (1)
{
    Assert (partition.get () != nullptr);
    fPartition->AddPartitionWatcher (this);
    TextStore& ts = partition->GetTextStore ();
    ts.AddMarkerOwner (this);
}

PartitioningTextImager::MeasureTextCache::~MeasureTextCache ()
{
    fPartition->RemovePartitionWatcher (this);
    TextStore& ts = fPartition->GetTextStore ();
    ts.RemoveMarkerOwner (this);
}

void PartitioningTextImager::MeasureTextCache::AboutToSplit (PartitionMarker* pm, size_t /*at*/, void** infoRecord) const noexcept
{
    *infoRecord = pm;
}

void PartitioningTextImager::MeasureTextCache::DidSplit (void* infoRecord) const noexcept
{
    PartitionMarker* pm = reinterpret_cast<PartitionMarker*> (infoRecord);
    fCache.clear ([pm](const CacheElt::COMPARE_ITEM& c) { return pm == c.fPM; });
}

void PartitioningTextImager::MeasureTextCache::AboutToCoalece (PartitionMarker* pm, void** infoRecord) const noexcept
{
    RequireNotNull (infoRecord);
    RequireNotNull (pm);
    *infoRecord = pm;
}

void PartitioningTextImager::MeasureTextCache::DidCoalece (void* infoRecord) const noexcept
{
    PartitionMarker* pm = reinterpret_cast<PartitionMarker*> (infoRecord);
    fCache.clear ([pm](const CacheElt::COMPARE_ITEM& c) { return pm == c.fPM; });
}

TextStore* PartitioningTextImager::MeasureTextCache::PeekAtTextStore () const
{
    return fPartition->PeekAtTextStore ();
}

void PartitioningTextImager::MeasureTextCache::EarlyDidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    {
        size_t       cacheSize   = 3;
        size_t       bufLen      = GetTextStore ().GetLength ();
        const size_t kBigThresh1_ = 1024;
        const size_t kBigThresh2_ = 10 * 1024;
        const size_t kBigThresh3_ = 20 * 1024;
        if (bufLen > kBigThresh1_) {
            cacheSize = 5;
            if (bufLen > kBigThresh2_) {
                cacheSize = 10;
                if (bufLen > kBigThresh3_) {
                    cacheSize = 75;
                }
            }
        }
        fCache.SetMaxCacheSize (cacheSize);
    }

    /*
     * iterate over all markers in the range updated, and for each one - see if they intersect with
     * any of the cache elements.
     */
    for (PartitionMarker* pm = fPartition->GetPartitionMarkerContainingPosition (FindPreviousCharacter (updateInfo.fReplaceFrom)); pm != nullptr; pm = pm->GetNext ()) {
        if (pm->GetStart () > updateInfo.GetResultingRHS ()) {
            break;
        }
        // Could optimize this further.... (MUCH)
        fCache.clear ([pm](const CacheElt::COMPARE_ITEM& c) { return pm == c.fPM; });
    }
    MarkerOwner::EarlyDidUpdateText (updateInfo);
}
#endif

/*
 ********************************************************************************
 ********************************** PartitionMarker *****************************
 ********************************************************************************
 */
void PartitionMarker::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    inherited::DidUpdateText (updateInfo);
    GetOwner ().UpdatePartitions (this, updateInfo);
}
