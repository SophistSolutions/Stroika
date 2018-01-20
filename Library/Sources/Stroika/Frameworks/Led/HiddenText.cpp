/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../../Foundation/StroikaPreComp.h"

#include <iterator>
#include <memory>

#include "Config.h"

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "../../Foundation/Traversal/Iterator.h"

#include "StandardStyledTextImager.h"

#include "HiddenText.h"

using namespace Stroika::Foundation;

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ****************************** HidableTextMarkerOwner **************************
 ********************************************************************************
 */
HidableTextMarkerOwner::HidableTextMarkerOwner (TextStore& textStore)
    : inherited ()
    , fInternalizer ()
    , fExternalizer ()
    , fTextStore (textStore)
    , fMarkersToBeDeleted ()
{
    SetInternalizer (shared_ptr<FlavorPackageInternalizer> ()); // sets default
    SetExternalizer (shared_ptr<FlavorPackageExternalizer> ()); // DITTO
    fTextStore.AddMarkerOwner (this);
}

HidableTextMarkerOwner::~HidableTextMarkerOwner ()
{
    Require (fMarkersToBeDeleted.IsEmpty ());
    try {
        MarkerList markers = CollectAllInRange (0, fTextStore.GetLength () + 1);
        if (not markers.empty ()) {
            {
                vector<Marker*> tmp;
                copy (markers.begin (), markers.end (), inserter (tmp, tmp.begin ()));
                // Note - this kookie &* stuff is to work around bugs in some STLs - that don't let you convert an iterator to a pointer.- SPR#0847
                GetTextStore ().RemoveMarkers (Traversal::Iterator2Pointer (tmp.begin ()), tmp.size ());
            }
            for (auto i = markers.begin (); i != markers.end (); ++i) {
                delete (*i);
            }
        }
        fTextStore.RemoveMarkerOwner (this);
    }
    catch (...) {
        Assert (false); // someday - handle exceptions here better - should cause no harm but memory leak (frequently - but sometimes worse),
        // and should be exceedingly rare - LGP 2000/03/30
    }
}

/*
@METHOD:        HidableTextMarkerOwner::HideAll
@DESCRIPTION:   <p>Tell all the existing 'hidden text' markers (optionally restricted to those intersecting
    the 'from' .. 'to' range) to hide themselves. This does <em>not</em> create any new @'HidableTextMarkerOwner::HidableTextMarker's.
    Call @'HidableTextMarkerOwner::MakeRegionHidable' for that.</p>
        <p>See @'HidableTextMarkerOwner::ShowAll' to re-show them.</p>
*/
void HidableTextMarkerOwner::HideAll ()
{
    HideAll (0, fTextStore.GetEnd () + 1);
}

void HidableTextMarkerOwner::HideAll (size_t from, size_t to)
{
    Invariant ();
    MarkerList markers = CollectAllInRange (from, to);
    sort (markers.begin (), markers.end (), LessThan<HidableTextMarker> ());
    // proceed in reverse direction - so that any markers being shown won't affect our text offsets
    for (auto i = markers.rbegin (); i != markers.rend (); ++i) {
        if (TextStore::Overlap (**i, from, to)) {
            (*i)->Hide ();
        }
    }
    Invariant ();
}

/*
@METHOD:        HidableTextMarkerOwner::ShowAll
@DESCRIPTION:   <p>Tell all the existing 'hidden text' markers (optionally restricted to those intersecting
    the 'from' .. 'to' range) to show themselves. This does <em>not</em> destroy any new @'HidableTextMarkerOwner::HidableTextMarker's.
    It merely re-installs their context into the document so that it can be seen and edited.</p>
        <p>See also @'HidableTextMarkerOwner::HideAll'.</p>
*/
void HidableTextMarkerOwner::ShowAll ()
{
    ShowAll (0, fTextStore.GetEnd () + 1);
}

void HidableTextMarkerOwner::ShowAll (size_t from, size_t to)
{
    Invariant ();
    MarkerList markers = CollectAllInRange (from, to);
    sort (markers.begin (), markers.end (), LessThan<HidableTextMarker> ());
    // proceed in reverse direction - so that any markers being shown won't affect our text offsets
    for (auto i = markers.rbegin (); i != markers.rend (); ++i) {
        if (TextStore::Overlap (**i, from, to)) {
#if qDebug
            HidableTextMarkerOwner::Invariant_ (); // don't make virtual call - cuz that might not be fully valid til end of routine...
#endif
            (*i)->Show ();
#if qDebug
            HidableTextMarkerOwner::Invariant_ (); // don't make virtual call - cuz that might not be fully valid til end of routine...
#endif
        }
    }
    Invariant ();
}

/*
@METHOD:        HidableTextMarkerOwner::MakeRegionHidable
@DESCRIPTION:   <p>Mark the region from 'from' to 'to' as hidable. This could involve coalescing adjacent hidden text markers
    (though even adjacent markers are sometimes <em>NOT</em> coalesced, if they differ in shown/hidden state, or if one is already
    hidden - cuz then it would be too hard to combine the two).</p>
        <p>This routine assures that after the call - all text in the given range is encapsulated by a hidden-text
    marker or markers.</p>
        <p>Note - this does <em>NOT</em> actually hide the text. You must then call @'HidableTextMarkerOwner::HideAll' and give
    it the same range given this function to get the text to actaully disapear from the screen.</p>
        <p>See also @'HidableTextMarkerOwner::MakeRegionUnHidable'</p>
*/
void HidableTextMarkerOwner::MakeRegionHidable (size_t from, size_t to)
{
    Require (from <= to);
    Invariant ();

    if (from == to) {
        return; // degenerate behavior
    }

    MarkerList hidableTextMarkersInRange = CollectAllInRange (from, to);

    // short circuit some code as a performance tweek
    if (hidableTextMarkersInRange.size () == 1 and
        hidableTextMarkersInRange[0]->GetStart () <= from and
        hidableTextMarkersInRange[0]->GetEnd () >= to) {
        return;
    }

    sort (hidableTextMarkersInRange.begin (), hidableTextMarkersInRange.end (), LessThan<HidableTextMarker> ());

    HidableTextMarker* prevNonEmptyMarker = nullptr; // used for coalescing...

    if (from > 0) {
        MarkerList tmp = CollectAllInRange (from - 1, from);
        // Can sometimes have more than one, if two different hidable region markers didn't get coalesced.
        for (auto i = tmp.begin (); i != tmp.end (); ++i) {
            if ((*i)->IsShown () and (*i)->GetEnd () == from) {
                prevNonEmptyMarker = *i;
                break;
            }
        }
    }

    // Update other markers and owners - since this change can affect the display
    {
        TextStore::SimpleUpdater updater (fTextStore, from, to);

        // iterate through markers, and eliminate all but one of them. The last one - if it exists - we'll enlarge.
        for (auto i = hidableTextMarkersInRange.begin (); i != hidableTextMarkersInRange.end (); ++i) {
            AssertNotNull (*i);
            if (prevNonEmptyMarker == nullptr) {
                Assert (i == hidableTextMarkersInRange.begin ()); // must have been first marker...
                prevNonEmptyMarker = *i;
                AssertNotNull (prevNonEmptyMarker);
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(suppress : 6011)
#endif
                if (from < prevNonEmptyMarker->GetStart ()) {
                    if (prevNonEmptyMarker->IsShown ()) {
                        fTextStore.SetMarkerStart (prevNonEmptyMarker, from);
                    }
                    else {
                        // cannot combine hidden and shown markers, so must create a new one
                        Assert (prevNonEmptyMarker->GetStart () > from);
                        GetTextStore ().AddMarker (MakeHidableTextMarker (), from, prevNonEmptyMarker->GetStart () - from, this);
                    }
                }
            }
            else if (prevNonEmptyMarker->IsShown ()) {
                // If its shown - we can coalesce it - so delete old one
                fMarkersToBeDeleted.AccumulateMarkerForDeletion (*i);
            }
        }
        if (prevNonEmptyMarker == nullptr) {
            GetTextStore ().AddMarker (MakeHidableTextMarker (), from, to - from, this);
        }
        else {
            if (prevNonEmptyMarker->GetEnd () < to) {
                if (prevNonEmptyMarker->IsShown ()) {
                    fTextStore.SetMarkerEnd (prevNonEmptyMarker, to);
                }
                else {
                    // cannot combine hidden and shown markers, so must create a new one
                    Assert (to > prevNonEmptyMarker->GetEnd ());
                    GetTextStore ().AddMarker (MakeHidableTextMarker (), prevNonEmptyMarker->GetEnd (), to - prevNonEmptyMarker->GetEnd (), this);
                }
            }
        }
    }

    Invariant ();
}

/*
@METHOD:        HidableTextMarkerOwner::MakeRegionUnHidable
@DESCRIPTION:   <p>Remove any hidable-text markers in the given range. The markers could be either hidden or shown
    at the time. This could involce splitting or coalescing adjacent markers.</p>
        <p>See also @'HidableTextMarkerOwner::MakeRegionHidable'.</p>
*/
void HidableTextMarkerOwner::MakeRegionUnHidable (size_t from, size_t to)
{
    Require (from <= to);
    Invariant ();

    if (from == to) {
        return; // degenerate behavior
    }

    MarkerList hidableTextMarkersInRange = CollectAllInRange (from, to);
    // short circuit some code as a performance tweek
    if (hidableTextMarkersInRange.empty ()) {
        return;
    }

    TempMarker pastSelMarker (GetTextStore (), to + 1, to + 1);

    // Update other markers and owners - since this change can affect the display
    {
        TextStore::SimpleUpdater updater (fTextStore, from, to);
        {
            sort (hidableTextMarkersInRange.begin (), hidableTextMarkersInRange.end (), LessThan<HidableTextMarker> ());

            // iterate through markers, and eliminate all of them, except maybe on the endpoints - if they have stuff outside
            // this range
            for (auto i = hidableTextMarkersInRange.begin (); i != hidableTextMarkersInRange.end (); ++i) {
                if (i == hidableTextMarkersInRange.begin () and (*i)->GetStart () < from) {
                    // merely adjust its end-point so not overlapping this region. Be careful if he
                    // used to extend past to, and cons up NEW marker for that part.
                    size_t oldEnd = (*i)->GetEnd ();
                    fTextStore.SetMarkerEnd (*i, from);
                    if (oldEnd > to) {
                        fTextStore.AddMarker (MakeHidableTextMarker (), to, oldEnd - to, this);
                    }
                }
                else if (i + 1 == hidableTextMarkersInRange.end () and (*i)->GetEnd () > to) {
                    // merely adjust its start-point so not overlapping this region
                    fTextStore.SetMarkerStart (*i, to);
                }
                else {
                    fMarkersToBeDeleted.AccumulateMarkerForDeletion (*i);
                }
            }
        }
    }

    /*
     *  If - as a result of the unhiding - we've expanded our text - we may not have done a 'DIDUPDATE' for enough markers.
     *  So catch the ones we've missed.
     *
     *  Note 100% sure this is a good enough test - but I hope so - LGP 2000/04/24
     */
    {
        TextStore::SimpleUpdater updater (fTextStore, to, pastSelMarker.GetEnd ());
    }

    Invariant ();
}

/*
@METHOD:        HidableTextMarkerOwner::GetHidableRegions
@DESCRIPTION:   <p>Return a @'DiscontiguousRun<DATA>' list (where DATA=void) of regions of hidable text.
    Regions returned are relative to offset 'from'. So - for example - if we have hidden text from
    5..8, and you call GetHidableRegions (2,8) you'll get back the list [[3,3]].</p>
        <p>NB: this routine only returns the hidable regions which are currently being SHOWN - not any invisible
    ones. This is because otherwise the run information would be useless, and not convey the
    actual sizes of the hidden text.</p>
        <p>See also @'HidableTextMarkerOwner::GetHidableRegionsWithData'.</p>
*/
DiscontiguousRun<bool> HidableTextMarkerOwner::GetHidableRegions (size_t from, size_t to) const
{
    Invariant ();
    DiscontiguousRun<bool> result;
    MarkerList             markers = CollectAllInRange (from, to);
    sort (markers.begin (), markers.end (), LessThan<HidableTextMarker> ());
    size_t relStartFrom = from;
    for (auto i = markers.begin (); i != markers.end (); ++i) {
        size_t mStart;
        size_t mEnd;
        (*i)->GetRange (&mStart, &mEnd);
        {
            mStart = max (mStart, relStartFrom); // ignore if marker goes back further than our start
            mEnd   = min (mEnd, to);             // ignore if end past requested end
            Assert (mStart < mEnd);
            result.push_back (DiscontiguousRunElement<bool> (mStart - relStartFrom, mEnd - mStart, (*i)->IsShown ()));
            relStartFrom = mEnd;
        }
    }
    return result;
}

DiscontiguousRun<bool> HidableTextMarkerOwner::GetHidableRegions () const
{
    return GetHidableRegions (0, fTextStore.GetEnd () + 1);
}

/*
@METHOD:        HidableTextMarkerOwner::GetHidableRegionsContiguous
@DESCRIPTION:   <p>If 'hidden' is true - then return true - iff the entire region from 'from' to 'to' is hidden.
    If 'hidden' is false, then return true iff the entire region from 'from' to 'to' contains no hidden elements.</p>
*/
bool HidableTextMarkerOwner::GetHidableRegionsContiguous (size_t from, size_t to, bool hidden) const
{
    Invariant ();
    // Sloppy, inefficient implementation. Can be MUCH faster - since we just need to know if there are ANY in this region!
    DiscontiguousRun<bool> tmpHack = GetHidableRegions (from, to);
    if (tmpHack.size () == 1) {
        return tmpHack[0].fData == hidden and
               tmpHack[0].fOffsetFromPrev == 0 and
               tmpHack[0].fElementLength >= to - from;
    }
    else {
        if (hidden) {
            return false;
        }
        else {
            return tmpHack.size () == 0;
        }
    }
}

/*
@METHOD:        HidableTextMarkerOwner::SetInternalizer
@DESCRIPTION:   <p>Sets the internalizer (@'FlavorPackageInternalizer' subclass). to be used with this class.
    It defaults to @'FlavorPackageInternalizer'.</p>
*/
void HidableTextMarkerOwner::SetInternalizer (const shared_ptr<FlavorPackageInternalizer>& i)
{
    fInternalizer = i;
    if (fInternalizer == nullptr) {
        fInternalizer = make_shared<FlavorPackageInternalizer> (GetTextStore ());
    }
}

/*
@METHOD:        HidableTextMarkerOwner::SetExternalizer
@DESCRIPTION:   <p>Sets the externalizer (@'FlavorPackageExternalizer' subclass). to be used with this class.
    It defaults to @'FlavorPackageExternalizer'.</p>
*/
void HidableTextMarkerOwner::SetExternalizer (const shared_ptr<FlavorPackageExternalizer>& e)
{
    fExternalizer = e;
    if (fExternalizer == nullptr) {
        fExternalizer = make_shared<FlavorPackageExternalizer> (GetTextStore ());
    }
}

/*
@METHOD:        HidableTextMarkerOwner::CollapseMarker
@DESCRIPTION:
*/
void HidableTextMarkerOwner::CollapseMarker (HidableTextMarker* m)
{
    RequireNotNull (m);
    Require (m->fShown);

    size_t start = 0;
    size_t end   = 0;
    m->GetRange (&start, &end);
    TextStore::SimpleUpdater updater (fTextStore, start, end, false);
    m->fShown = false;
}

/*
@METHOD:        HidableTextMarkerOwner::ReifyMarker
@DESCRIPTION:
*/
void HidableTextMarkerOwner::ReifyMarker (HidableTextMarker* m)
{
    RequireNotNull (m);
    Require (not m->fShown);

    {
        size_t start = 0;
        size_t end   = 0;
        m->GetRange (&start, &end);

        TextStore::SimpleUpdater updater (fTextStore, start, end, false);
        m->fShown = true;
    }
}

/*
@METHOD:        HidableTextMarkerOwner::MakeHidableTextMarker
@DESCRIPTION:   <p>This routine creates the actual marker objects to be used to hide text.</p>
                <p>By default - it
            creates @'HidableTextMarkerOwner::FontSpecHidableTextMarker' markers. You can OVERRIDE this to create different
            style markers, or to set different color etc attributes for use in your @'HidableTextMarkerOwner' subclass.</p>
*/
HidableTextMarkerOwner::HidableTextMarker* HidableTextMarkerOwner::MakeHidableTextMarker ()
{
    /*
     *  Some alternates you may want to consider in your overrides...
     *

                Led_IncrementalFontSpecification    fontSpec;
                fontSpec.SetTextColor (Led_Color::kRed);
                return new LightUnderlineHidableTextMarker (fontSpec);

        or:
                Led_IncrementalFontSpecification    fontSpec;
                fontSpec.SetTextColor (Led_Color::kRed);
                #if     qPlatform_Windows
                fontSpec.SetStyle_Strikeout (true);
                #endif
                return new FontSpecHidableTextMarker (fontSpec);

     */
    return new LightUnderlineHidableTextMarker ();
}

TextStore* HidableTextMarkerOwner::PeekAtTextStore () const
{
    return &fTextStore;
}

void HidableTextMarkerOwner::AboutToUpdateText (const UpdateInfo& updateInfo)
{
    Invariant ();
    Assert (fMarkersToBeDeleted.IsEmpty ());
    inherited::AboutToUpdateText (updateInfo);
}

void HidableTextMarkerOwner::DidUpdateText (const UpdateInfo& updateInfo) noexcept
{
    inherited::DidUpdateText (updateInfo);
    if (updateInfo.fTextModified) {
        // cull empty markers
        MarkerList markers = CollectAllInRange_OrSurroundings (updateInfo.fReplaceFrom, updateInfo.GetResultingRHS ());
        for (auto i = markers.begin (); i != markers.end (); ++i) {
            HidableTextMarker* m = *i;
            if (m->GetLength () == 0) {
                fMarkersToBeDeleted.AccumulateMarkerForDeletion (m);
            }
        }
    }
    fMarkersToBeDeleted.FinalizeMarkerDeletions ();
    Invariant ();
}

HidableTextMarkerOwner::MarkerList HidableTextMarkerOwner::CollectAllInRange (size_t from, size_t to) const
{
    MarkersOfATypeMarkerSink2Vector<HidableTextMarker> result;
    fTextStore.CollectAllMarkersInRangeInto (from, to, this, result);
    return result.fResult;
}

HidableTextMarkerOwner::MarkerList HidableTextMarkerOwner::CollectAllInRange_OrSurroundings (size_t from, size_t to) const
{
    MarkersOfATypeMarkerSink2Vector<HidableTextMarker> result;
    fTextStore.CollectAllMarkersInRangeInto_OrSurroundings (from, to, this, result);
    return result.fResult;
}

#if qDebug
void HidableTextMarkerOwner::Invariant_ () const
{
    MarkerList markers = CollectAllInRange (0, fTextStore.GetEnd () + 1);

    sort (markers.begin (), markers.end (), LessThan<HidableTextMarker> ());

    // Walk through - and see we are non-overlapping, and have no empties (unless hidden)
    // Note - we DON'T require adjacent ones be coalesced, though we try to arrange for that if possible.
    // We don't always coalece cuz if one already hidden, and we try to make a new one - its too hard to combine the adjacent
    // stored readwritepackages.
    size_t lastEnd = 0;
    for (size_t i = 0; i < markers.size (); i++) {
        HidableTextMarker* m = markers[i];
        Assert (m->GetLength () > 0);
        Assert (m->GetStart () >= lastEnd);
        lastEnd = m->GetEnd ();
    }
    Assert (lastEnd <= fTextStore.GetLength () + 1);
}
#endif

/*
 ********************************************************************************
 *********************** UniformHidableTextMarkerOwner **************************
 ********************************************************************************
 */
UniformHidableTextMarkerOwner::UniformHidableTextMarkerOwner (TextStore& textStore)
    : inherited (textStore)
    , fHidden (false)
{
}

void UniformHidableTextMarkerOwner::HideAll ()
{
    if (not fHidden) {
        inherited::HideAll ();
        fHidden = true;
    }
}

void UniformHidableTextMarkerOwner::ShowAll ()
{
    if (fHidden) {
        inherited::ShowAll ();
        fHidden = false;
    }
}

void UniformHidableTextMarkerOwner::MakeRegionHidable (size_t from, size_t to)
{
    Require (from <= to);

    //Not so great implemenation - could look at particular objects created - and make sure THEY have the hidden bit set...
    inherited::MakeRegionHidable (from, to);
    if (fHidden) {
        inherited::HideAll ();
    }
    else {
        inherited::ShowAll ();
    }
}

/*
 ********************************************************************************
 ************* HidableTextMarkerOwner::FontSpecHidableTextMarker ****************
 ********************************************************************************
 */
Led_FontSpecification HidableTextMarkerOwner::FontSpecHidableTextMarker::MakeFontSpec (const StyledTextImager* /*imager*/, const RunElement& runElement) const
{
    Led_FontSpecification fsp;
    for (auto i = runElement.fSupercededMarkers.begin (); i != runElement.fSupercededMarkers.end (); ++i) {
        if (StandardStyledTextImager::StandardStyleMarker* m = dynamic_cast<StandardStyledTextImager::StandardStyleMarker*> (*i)) {
            fsp.MergeIn (m->fFontSpecification);
        }
    }
    fsp.MergeIn (fFontSpecification); // give our fontSpec last dibs - so 'deletion' hilighting takes precedence
    return fsp;
}

/*
 ********************************************************************************
 ********* HidableTextMarkerOwner::LightUnderlineHidableTextMarker **************
 ********************************************************************************
 */
HidableTextMarkerOwner::LightUnderlineHidableTextMarker::LightUnderlineHidableTextMarker (const Led_IncrementalFontSpecification& fsp)
{
    fFontSpecification = fsp;
}

Led_Color HidableTextMarkerOwner::LightUnderlineHidableTextMarker::GetUnderlineBaseColor () const
{
    if (fFontSpecification.GetTextColor_Valid ()) {
        return fFontSpecification.GetTextColor ();
    }
    else {
        return inherited::GetUnderlineBaseColor ();
    }
}

/*
 ********************************************************************************
 ******************* ColoredUniformHidableTextMarkerOwner ***********************
 ********************************************************************************
 */
void ColoredUniformHidableTextMarkerOwner::FixupSubMarkers ()
{
    // Now walk all existing markers, and set their fColor field!!!
    MarkerList markers = CollectAllInRange_OrSurroundings (0, GetTextStore ().GetEnd () + 1);
    for (auto i = markers.begin (); i != markers.end (); ++i) {
        LightUnderlineHidableTextMarker* m = dynamic_cast<LightUnderlineHidableTextMarker*> (*i);
        AssertNotNull (m);
        if (fColored) {
            m->fFontSpecification.SetTextColor (fColor);
        }
        else {
            m->fFontSpecification.InvalidateTextColor ();
        }
    }
}

ColoredUniformHidableTextMarkerOwner::HidableTextMarker* ColoredUniformHidableTextMarkerOwner::MakeHidableTextMarker ()
{
    Led_IncrementalFontSpecification fontSpec;
    if (fColored) {
        fontSpec.SetTextColor (fColor);
    }
    return new LightUnderlineHidableTextMarker (fontSpec);
}
