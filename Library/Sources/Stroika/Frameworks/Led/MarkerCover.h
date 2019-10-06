/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_MarkerCover_h_
#define _Stroika_Frameworks_Led_MarkerCover_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    MarkerCover
@DESCRIPTION:
        <p>A MarkerCover is a templated class which keeps track of a collection of markers which "cover" the text.
    Here the term "cover" is very similar to the mathematical term, meaning that the union of all the elements
    of the collection contains the set of valid marker positions, and yet none of the MarkerCover elements
    intersect (contain the same character position).</p>
        <p>So - for example - a common use for a MarkerCover would be to keep track of all the style information
    associated with a document in a standard implementation of styleruns (as in @'StandardStyledTextImager').</p>
 */

#include <algorithm>
#include <list>
#include <vector>

#include "Marker.h"
#include "TextStore.h"

namespace Stroika::Frameworks::Led {

    /*
    @CLASS:         MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>
    @BASES:         virtual @'MarkerOwner'
    @DESCRIPTION:   <p>MarkerCover is a template used to simplify implementation of a cover of markers. You define
        some marker type (MARKER), and be sure it has a GetInfo and SetInfo () methods (which take type MARKERINFO).
        The 'MARKER' class also must have a no-arg constructor, and one which takes a MARKERINFO.
        Be sure type MARKERINFO has an operator== method to compare if the info stored in two markers should be considered
        equal.</p>
            <p>Then this template will keep track of a bunch of these markers for you. As you set the 'info' attribute
        on any range of marker positions, this code will automatically split, and merge adjacent markers, and assign them
        this MARKERINFO attribute.</p>
            <p>This code is ideal for keeping track of something like style runs, where you have a (in this case font)
        attribute to be applied to all the text.</p>
            <p>NB: Although this class is very similar to a @'Partition' - it is different enuf that these to classes share
        no common implementation. The reason for this is that there are different performance considerations operative, and
        this is not an abstract class; but rather a utility template class to help implement covers.</p>
            <p>The thing a @'Partition' and a @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>' share is that they both
        implement a partition - in the mathematical sense - of the legal/valid marker-positions associated with a @'TextStore'.</p>
    */
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    class MarkerCover : public virtual MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        using MarkerVector = vector<MARKER*>;

    public:
        MarkerCover (TextStore& useTextStore, MARKERINFO defaultInfo = MARKERINFO ());
        ~MarkerCover ();

        // prevent accidental copying
    private:
        MarkerCover (const MarkerCover&);
        void operator= (const MarkerCover&);

    public:
        virtual TextStore* PeekAtTextStore () const override;

    private:
        TextStore& fTextStore;

    public:
        nonvirtual MarkerVector CollectAllInRange (size_t from, size_t to) const;
        nonvirtual MarkerVector CollectAllInRange_OrSurroundings (size_t from, size_t to) const;

    protected:
        nonvirtual MarkerVector CollectAllNonEmptyInRange (size_t from, size_t to) const;
        nonvirtual MarkerVector CollectAllNonEmptyInRange_OrSurroundings (size_t from, size_t to) const;

    public:
        nonvirtual const MARKERINFO& GetInfo (size_t charAfterPos) const;
        nonvirtual MarkerVector GetInfoMarkers (size_t charAfterPos, size_t nTCharsFollowing) const;
        nonvirtual vector<pair<MARKERINFO, size_t>> GetInfo (size_t charAfterPos, size_t nTCharsFollowing) const;
        virtual void                                SetInfo (size_t charAfterPos, size_t nTCharsFollowing, const INCREMENTALMARKERINFO& infoForMarkers);
        virtual void                                SetInfos (size_t charAfterPos, const vector<pair<INCREMENTALMARKERINFO, size_t>>& infoForMarkers);
        nonvirtual void                             SetInfos2 (size_t charAfterPos, const vector<pair<MARKERINFO, size_t>>& infoForMarkers);

    protected:
        nonvirtual void SetInfoInnerLoop (
            size_t                       from,
            size_t                       to,
            const INCREMENTALMARKERINFO& infoForMarkers,
            const UpdateInfo&            allMarkersUpdateInfo,
            TextStore::SimpleUpdater**   updater);

    protected:
        virtual void    ConstrainSetInfoArgs (size_t* charAfterPos, size_t* nTCharsFollowing);
        nonvirtual void NoteCoverRangeDirtied (size_t from, size_t to);
        virtual void    NoteCoverRangeDirtied (size_t from, size_t to, const MarkerVector& rangeAndSurroundingsMarkers);

    public:
        virtual void AboutToUpdateText (const UpdateInfo& updateInfo) override;
        virtual void EarlyDidUpdateText (const UpdateInfo& updateInfo) noexcept override;
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    protected:
        nonvirtual void CullZerod (size_t around) noexcept;
        nonvirtual void CullZerod (const MarkerVector& rangeAndSurroundingsMarkers) noexcept;
        nonvirtual void CheckForMerges (size_t around) noexcept;
        nonvirtual void CheckForMerges (const MarkerVector& rangeAndSurroundingsMarkers) noexcept;

    protected:
        mutable MarkerMortuary<MARKER> fMarkersToBeDeleted;

    private:
        nonvirtual void HandleCallBeforeDidUpdateComplete () const noexcept;
        virtual void    HandleCallBeforeDidUpdateComplete_ () const noexcept;

    private:
        mutable bool fNeedExtraUpdateCheck; // flag so we can tell if someone queries our state after an 'AboutToUpdateText' but BEFORE we've gotten our 'DidUpdateText'
        mutable bool fEarlyDidUpdateCalled; // See docs for @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::HandleCallBeforeDidUpdateComplete'
        UpdateInfo   fNeedExtraUpdateCheck_UpdateInfo;

    protected:
        class NonEmptyOnes : public MarkersOfATypeMarkerSink2Vector<MARKER> {
        private:
            using inherited = MarkersOfATypeMarkerSink2Vector<MARKER>;

        public:
            NonEmptyOnes ()
                : inherited ()
            {
            }

        public:
            virtual void Append (Marker* m) override
            {
                if (m->GetLength () != 0) {
                    inherited::Append (m);
                }
            }
        };

        // Debug support
    public:
        nonvirtual void Invariant () const;
#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    };

    /*
        ********************************************************************************
        ***************************** Implementation Details ***************************
        ********************************************************************************
        */
    //  class   MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::MarkerCover (TextStore& useTextStore, MARKERINFO defaultInfo)
        : MarkerOwner ()
        , fTextStore (useTextStore)
        , fNeedExtraUpdateCheck (false)
        , fEarlyDidUpdateCalled (false)
        , fNeedExtraUpdateCheck_UpdateInfo (0, 0, nullptr, 0, false, false)
    {
        /*
         *  Add ourselves to the TextStore (for callbacks), and add one marker to span
         *  the entire buffer length (plus one so catches anything at the end).
         */
        fTextStore.AddMarkerOwner (this);
        fTextStore.AddMarker (new MARKER (defaultInfo), 0, fTextStore.GetEnd () + 1, this);
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::~MarkerCover ()
    {
        try {
            //  NB: We are collecting ALL markers we own here, not just markers we added!
            //  This is because it is common practice to add other markers - such as embeddings - to a textstore
            //  using this dbase as the marker owner.
            MarkersOfATypeMarkerSink2Vector<Marker> result;
            fTextStore.CollectAllMarkersInRangeInto_OrSurroundings (0, fTextStore.GetLength () + 1, this, result);
            vector<Marker*>::const_iterator end = result.fResult.end ();
            for (auto i = result.fResult.begin (); i != end; ++i) {
                fTextStore.RemoveMarker (*i);
                delete *i;
            }
        }
        catch (...) {
            Assert (false); // NYI - must come up with some safe way to delete all these markers
            // without using any memory! - maybe do the above in small pieces?
            // One cell at a time? That would be painfully slow, but if we only did
            // it on caught excpetions, that might be OK...
            // LGP 960427
            // not right, but til we fix this, eat the exception... LGP 960427
        }
        Assert (CollectAllInRange_OrSurroundings (0, fTextStore.GetLength () + 1).size () == 0);
        fTextStore.RemoveMarkerOwner (this);
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    TextStore* MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::PeekAtTextStore () const
    {
        return &fTextStore;
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    inline typename MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::MarkerVector MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::CollectAllInRange (size_t from, size_t to) const
    {
        /*
         *  Walk through all the markers in existence (in this range), and throw away all
         *  but our standard style markers.
         *  This is an inefficient approach. It would be far
         *  faster to keep a linked, or doubly linked list of all these guys.
         *  But this approach saves a bit of memory, and til we see this as a problem, lets just
         *  live with it.
         */
        MarkersOfATypeMarkerSink2Vector<MARKER> result;
        fTextStore.CollectAllMarkersInRangeInto (from, to, this, result);
        return result.fResult;
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    inline typename MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::MarkerVector MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::CollectAllInRange_OrSurroundings (size_t from, size_t to) const
    {
        /*
         *  Walk through all the markers in existence (in this range), and throw away all
         *  but our standard style markers.
         *  This is an inefficeint approach. It would be far
         *  faster to keep a linked, or doubly linked list of all these guys.
         *  But this approach saves a bit of memory, and til we see this as a problem, lets just
         *  live with it.
         */
        MarkersOfATypeMarkerSink2Vector<MARKER> result;
        fTextStore.CollectAllMarkersInRangeInto_OrSurroundings (from, to, this, result);
        return result.fResult;
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    inline typename MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::MarkerVector MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::CollectAllNonEmptyInRange (size_t from, size_t to) const
    {
        /*
         *  See CollectAllInRange - but with extra test to eliminate zero-len markers...
         */
        NonEmptyOnes result;
        fTextStore.CollectAllMarkersInRangeInto (from, to, this, result);
        return result.fResult;
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    inline typename MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::MarkerVector MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::CollectAllNonEmptyInRange_OrSurroundings (size_t from, size_t to) const
    {
        /*
         *  See CollectAllInRange - but with extra test to eliminate zero-len markers...
         */
        NonEmptyOnes result;
        fTextStore.CollectAllMarkersInRangeInto_OrSurroundings (from, to, this, result);
        return result.fResult;
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    inline const MARKERINFO& MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::GetInfo (size_t charAfterPos) const
    {
        HandleCallBeforeDidUpdateComplete ();
        Invariant ();
        MarkerOfATypeMarkerSink<MARKER> result;
        fTextStore.CollectAllMarkersInRangeInto (charAfterPos, charAfterPos + 1, this, result);
        AssertNotNull (result.fResult);
#if qDebug
        {
            MarkerVector markers = CollectAllInRange (charAfterPos, charAfterPos + 1);
            Assert (markers.size () == 1);
            Assert (result.fResult == markers[0]);
        }
#endif
        return result.fResult->GetInfo ();
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::GetInfoMarkers
    @DESCRIPTION:
            <p>Returns the marker objects (of type 'MARKER') which contain the marker-info (of type 'MARKERINFO').
        which are found in the range from charAfterPos and up to nTCharsFollowing. If nTCharsFollowing == 0, it is
        assumed to really be at least one.</p>
            <p>The marker list is returned sorted, and always contains at least one element.</p>
    */
    inline typename MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::MarkerVector
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::GetInfoMarkers (size_t charAfterPos, size_t nTCharsFollowing) const
    {
        HandleCallBeforeDidUpdateComplete ();
        Invariant ();
        if (nTCharsFollowing == 0) {
            nTCharsFollowing = 1;
        }
        MarkerVector markers = CollectAllInRange (charAfterPos, charAfterPos + nTCharsFollowing);
        sort (markers.begin (), markers.end (), LessThan<MARKER> ());
        Ensure (markers.size () >= 1);
        return markers;
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::GetInfo
    @DESCRIPTION:
            <p>Returns the a vector of pairs: MARKERINFO and length (pair<MARKERINFO,size_t>). These are returned in
        order. These are the info which is found in the range from charAfterPos and up to nTCharsFollowing.
        If nTCharsFollowing == 0, it is assumed to really be at least one.</p>
            <p>The list is returned sorted, and always contains at least one element.</p>
            <p>See also @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::SetInfo',
        and @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::SetInfos'</p>
    */
    inline vector<pair<MARKERINFO, size_t>>
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::GetInfo (size_t charAfterPos, size_t nTCharsFollowing) const
    {
        MarkerVector                     markers = GetInfoMarkers (charAfterPos, nTCharsFollowing);
        vector<pair<MARKERINFO, size_t>> result;
        for (auto i = markers.begin (); i != markers.end (); ++i) {
            // Simply append makerinfo's, but be careful on the endpoints. We have have cut one or both endpoints off slightly
            if (i == markers.begin ()) {
                result.push_back (pair<MARKERINFO, size_t> ((*i)->GetInfo (), min ((*i)->GetEnd () - charAfterPos, nTCharsFollowing)));
            }
            else if (i + 1 == markers.end ()) {
                result.push_back (pair<MARKERINFO, size_t> ((*i)->GetInfo (), charAfterPos + nTCharsFollowing - (*i)->GetStart ()));
            }
            else {
                result.push_back (pair<MARKERINFO, size_t> ((*i)->GetInfo (), (*i)->GetLength ()));
            }
        }
        Ensure (result.size () >= 1);
        return result;
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::SetInfo
    @DESCRIPTION:
            <p>Apply the given INCREMENTALMARKERINFO to the range of 'nTCharsFollowing' chars starting at charAfterPos.
        This could have no effect (if nTCharsFollowing is zero, and the substypes ConstrainSetInfoArgs () doesn't modify the start/end,
        or if the INCREMENTALMARKERINFO specifies no change).</p>
            <p>At any rate - the method will assure the given INCREMENTALMARKERINFO is applied to the entire range from 'charAfterPos;
        to 'charAfterPos + nTCharsFollowing', and any splitting or coalescing
        of adjacent style runs will be handled automatically.</p>
            <p>See also @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::GetInfo', and
        @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::SetInfos'</p>
    */
    void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::SetInfo (size_t charAfterPos, size_t nTCharsFollowing, const INCREMENTALMARKERINFO& infoForMarkers)
    {
        HandleCallBeforeDidUpdateComplete ();
        Invariant ();

        ConstrainSetInfoArgs (&charAfterPos, &nTCharsFollowing);

        if (nTCharsFollowing == 0) {
            return;
        }

        size_t     from = charAfterPos;
        size_t     to   = from + nTCharsFollowing;
        UpdateInfo allMarkersUpdateInfo (charAfterPos, charAfterPos + nTCharsFollowing, LED_TCHAR_OF (""), 0, false, true);

        TextStore::SimpleUpdater* updater = nullptr;
        try {
            SetInfoInnerLoop (from, to, infoForMarkers, allMarkersUpdateInfo, &updater);
            if (updater != nullptr) {
                NoteCoverRangeDirtied (allMarkersUpdateInfo.fReplaceFrom, allMarkersUpdateInfo.fReplaceTo);
                delete updater; // forces DidUpdate calls
            }
        }
        catch (...) {
            if (updater != nullptr) {
                updater->Cancel ();
            }
            delete updater;
            throw;
        }
        Invariant ();
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::SetInfos
    @DESCRIPTION:
            <p>DOC LATER. SHOULD TIS TAKE A VECTOR OF INCREMENTALMARKERINFO???? INSTEAD OF MARKERINFO</p>
    */
    void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::SetInfos (size_t charAfterPos, const vector<pair<INCREMENTALMARKERINFO, size_t>>& infoForMarkers)
    {
        HandleCallBeforeDidUpdateComplete ();

        Invariant ();

        size_t totalStart = 0;
        size_t totalEnd   = 0;
        {
            /*
             *  We must compute the entire range of the affected area. This is very nearly trivial - except for the ConstrainSetInfoArgs ()
             *  effect. If the ConstrainSetInfoArgs () call expands the nCharsFollowing count past the end of what would have been the sum
             *  of the '(*i).second' values - then we must use the larger value - so that we do an AboutToUpdate/DidUpdate on the entire
             *  affected area.
             *
             *  If the ConstrainSetInfoArgs () turn out to be expensive - then we could probably optimize this so that we only called
             *  the ConstrainSetInfoArgs () on the last few markers in the list.
             */
            totalStart    = charAfterPos;
            size_t curPos = charAfterPos;
            for (auto i = infoForMarkers.begin (); i != infoForMarkers.end (); ++i) {
                size_t from = curPos;
                size_t to   = from;
                {
                    size_t nCharsFollowing = (*i).second;
                    ConstrainSetInfoArgs (&from, &nCharsFollowing);
                    to = from + nCharsFollowing;
                }

                curPos += (*i).second;

                totalStart = min (totalStart, from);
                totalEnd   = max (to, curPos);
            }
        }

        if (totalStart == totalEnd) {
            return;
        }

        UpdateInfo                allMarkersUpdateInfo (totalStart, totalEnd, LED_TCHAR_OF (""), 0, false, true);
        TextStore::SimpleUpdater* updater = nullptr;
        try {
            {
                size_t curPos = charAfterPos;
                for (auto i = infoForMarkers.begin (); i != infoForMarkers.end (); ++i) {
                    size_t from = curPos;
                    size_t to   = from;
                    {
                        size_t nCharsFollowing = (*i).second;
                        ConstrainSetInfoArgs (&from, &nCharsFollowing);
                        to = from + nCharsFollowing;
                    }

                    /*
                     *   Assure region we are updating is contained within the 'total' update region.
                     */
                    Assert (totalStart <= from);
                    Assert (from <= to);
                    Assert (to <= totalEnd);

                    SetInfoInnerLoop (from, to, (*i).first, allMarkersUpdateInfo, &updater);
                    curPos += (*i).second;
                }
            }

            if (updater != nullptr) {
                NoteCoverRangeDirtied (allMarkersUpdateInfo.fReplaceFrom, allMarkersUpdateInfo.fReplaceTo);
                delete updater; // forces DidUpdate calls
            }
        }
        catch (...) {
            if (updater != nullptr) {
                updater->Cancel ();
            }
            delete updater;
            throw;
        }

        Invariant ();
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::SetInfos2
    @DESCRIPTION:
    */
    void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::SetInfos2 (size_t charAfterPos, const vector<pair<MARKERINFO, size_t>>& infoForMarkers)
    {
        vector<pair<INCREMENTALMARKERINFO, size_t>> tmp;
        tmp.reserve (infoForMarkers.size ());
        for (auto i = infoForMarkers.begin (); i != infoForMarkers.end (); ++i) {
            tmp.push_back (pair<INCREMENTALMARKERINFO, size_t> (INCREMENTALMARKERINFO ((*i).first), (*i).second));
        }
        SetInfos (charAfterPos, tmp);
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::SetInfoInnerLoop
    @ACCESS:        protected
    @DESCRIPTION:
            <p>Internal helper routine. You probably should not use/call this directly.</p>
            <p>Apply the given 'INCREMENTALMARKERINFO' to the 'from' to 'to' range of text. Keep track of some
        outer scope, passed in variables that help the overall update process.</p>
            <p>Only update 'allMarkersInRange' - calling aboutToUpdate - if the passed in argument is non-nullptr.
            Only update 'changedAnything' - flag if its pointer is non-nullptr. 'allMarkersInRange' can be null if-and-only-if
            'changedAnything' is null (pointer).</p>.
    */
    void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::SetInfoInnerLoop (
        size_t                       from,
        size_t                       to,
        const INCREMENTALMARKERINFO& infoForMarkers,
        const UpdateInfo&            allMarkersUpdateInfo,
        TextStore::SimpleUpdater**   updater)
    {
        // Gather all style markers and sumarize them for the region which overlaps my change
        // Sort the markers to make the coalesce code below simpler, and more efficient...
        MarkerVector markers            = CollectAllNonEmptyInRange (from > 0 ? from - 1 : from, to);
        MARKER*      prevNonEmptyMarker = nullptr; // used for coalescing...
        sort (markers.begin (), markers.end (), LessThan<MARKER> ());
        typename MarkerVector::const_iterator miStart = markers.begin ();
        typename MarkerVector::const_iterator miEnd   = markers.end ();
        if (from > 0) {
#if qDebug
            {
                MarkerVector tmp = CollectAllNonEmptyInRange (from - 1, from);
                Assert (tmp.size () == 1);
                Assert (tmp[0] == markers[0]);
            }
#endif
            prevNonEmptyMarker = markers[0];
            if (prevNonEmptyMarker->GetEnd () > from) { // else it would be the FIRST in our marker list!
                Assert (markers[0] == prevNonEmptyMarker);
                prevNonEmptyMarker = nullptr;
            }
            else {
                miStart++;
                Assert (miStart != miEnd);
            }
        }

        bool changedAnythingHERE = false;
        // iterate through markers, and create an ordered list of the MARKERs which overlap our change
        for (auto mi = miStart; mi != miEnd; mi++) {
            MARKER* m = *mi;
            AssertMember (m, MARKER);

            // Be sure prev marker lines up with next in list...
            Assert (prevNonEmptyMarker != nullptr or mi == miStart); // if mi > miStart, we must have had at least ONE non-empty marker!
            Assert (mi == miStart or (prevNonEmptyMarker->GetEnd () == m->GetStart ()));

            // First see if we need make any change at all
            MARKERINFO fsp = m->GetInfo ();
            fsp.MergeIn (infoForMarkers);
            if (fsp != m->GetInfo ()) {
                if (updater != nullptr and *updater == nullptr) {
                    *updater = new TextStore::SimpleUpdater (fTextStore, allMarkersUpdateInfo);
                }
                changedAnythingHERE = true;

                /*
                 *  First apply the actual change, splitting any markers as needed along the way.
                 */
                if (m->GetStart () < from) {
                    // WE MUST SPLIT
                    MARKERINFO origFSP   = m->GetInfo ();
                    size_t     itsOldEnd = m->GetEnd ();
                    fTextStore.SetMarkerEnd (m, from);

                    size_t  lenLeftToFixup = itsOldEnd - from;
                    size_t  ourNewLen      = min (lenLeftToFixup, (to - from));
                    MARKER* newMarker      = new MARKER (fsp);
                    fTextStore.AddMarker (newMarker, from, ourNewLen, this);

                    Assert (m->GetLength () != 0);
                    prevNonEmptyMarker = m;         // update prevNonEmptyMarker
                    m                  = newMarker; // be sure m points to LAST marker of results for this range

                    // Now do we need to clone the old marker to go after ours?
                    lenLeftToFixup -= ourNewLen;
                    if (lenLeftToFixup != 0) {
                        MARKER* nm = new MARKER (origFSP);
                        fTextStore.AddMarker (nm, from + ourNewLen, lenLeftToFixup, this);
                        Assert (m->GetLength () != 0);
                        prevNonEmptyMarker = m;  // update prevNonEmptyMarker
                        m                  = nm; // be sure m points to LAST marker of results for this range
                    }
                }
                else if (m->GetEnd () > to) {
                    // WE MUST SPLIT
                    MARKER* newMarker   = new MARKER (fsp);
                    size_t  itsOldStart = m->GetStart ();
                    fTextStore.SetMarkerStart (m, to);
                    size_t newMarkerLen = to - itsOldStart;
                    Assert (newMarkerLen > 0);
                    fTextStore.AddMarker (newMarker, itsOldStart, newMarkerLen, this);
                    Assert (newMarker->GetLength () != 0);

                    m = newMarker; // so we attempt to merge 'm' (really newMarker) onto end of prevNonEmptyMarker
                }
                else {
                    Assert (m->GetStart () >= from);
                    Assert (m->GetEnd () <= to);
                    m->SetInfo (fsp);
                    // NB: we don't need to update 'm' cuz no markers created
                    // and we didn't change prev either!
                }
            }

            /*
             *  Now for each marker - as a result of this merged in style - may now be
             *  EQUAL to its predecesor marker, or its following marker.
             *
             *  Since all our markers are now ordered (sorted above), checking the
             *  predecessor is easy (except for the first maker). And we can avoid
             *  the redundancy of checking the following markers (not to mention
             *  possible futility since they are about to change) by simply arranging to
             *  check the very LAST marker, and see if it can be merged with the
             *  one following it.
             *
             *  Note: we do this OUTSIDE the test for (fsp != m->GetInfo ())
             *  cuz even if they were the same, we could have had PREVIOUS differences,
             *  and so needed to merge together all those blocks.
             */
            if (changedAnythingHERE and prevNonEmptyMarker != nullptr and prevNonEmptyMarker != m) {
                AssertMember (prevNonEmptyMarker, MARKER);
                if (prevNonEmptyMarker->GetInfo () == m->GetInfo ()) {
                    // simply zero out the size of 'm', and put that size into
                    // prevMarker. Zero'd out markers will automagically be deleted
                    // in the DidUpdate () call below...
                    Assert (prevNonEmptyMarker->GetEnd () == m->GetStart ());
                    fTextStore.SetMarkerEnd (prevNonEmptyMarker, m->GetEnd ());
                    fTextStore.SetMarkerEnd (m, m->GetStart ()); // note we zero out by setting END to start - rather than
                    // the other way cuz of the special case where 'm' is the
                    // last marker, and otherwise it won't get found to 'cull'
                }
            }
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
// Asserts above make clear m not null
#pragma warning(suppress : 28182)
#endif
            if (m->GetLength () != 0) {
                prevNonEmptyMarker = m;
            }
        }
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::ConstrainSetInfoArgs
    @ACCESS:        protected
    @DESCRIPTION:
            <p>Override this to apply any special constraints on the boundaries of regions in the MarkerCover. For example, for
        a ParagraphDatabase, this makes sure the boundaries on lookups/changes fall on partition element boundaries. By default,
        this routine does nothing.</p>
    */
    void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::ConstrainSetInfoArgs (size_t* /*charAfterPos*/, size_t* /*nTCharsFollowing*/)
    {
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::NoteCoverRangeDirtied
    @ACCESS:        protected
    @DESCRIPTION:
            <p>Internal utility routine, used to react to a region of text having had some of its MARKERINFO changed. This code
        will walk the affected region, and make sure all contraints (like no zero-length markers) are observed. It calls the
        virtual @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::NoteCoverRangeDirtied' to handle the real gruntwork.</p>
    */
    void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::NoteCoverRangeDirtied (size_t from, size_t to)
    {
        Require (from <= to);
        Require (to <= fTextStore.GetEnd () + 1);
        MarkerVector markers = CollectAllInRange_OrSurroundings (from, to);
        sort (markers.begin (), markers.end (), LessThan<MARKER> ());
        NoteCoverRangeDirtied (from, to, markers);
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::NoteCoverRangeDirtied ([[maybe_unused]] size_t from, [[maybe_unused]] size_t to, const MarkerVector& rangeAndSurroundingsMarkers)
    {
        Require (from <= to);
        Require (to <= fTextStore.GetEnd () + 1);
        CullZerod (rangeAndSurroundingsMarkers);
        CheckForMerges (rangeAndSurroundingsMarkers);
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::AboutToUpdateText (const UpdateInfo& updateInfo)
    {
        fNeedExtraUpdateCheck = false; // If we set true before this call - must be because an 'AboutToUpdate' was aborted.
        Assert (fMarkersToBeDeleted.IsEmpty ());
        Invariant ();
        inherited::AboutToUpdateText (updateInfo);
        if (updateInfo.fTextModified) {
            fNeedExtraUpdateCheck            = true;
            fEarlyDidUpdateCalled            = false;
            fNeedExtraUpdateCheck_UpdateInfo = updateInfo;
        }
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::EarlyDidUpdateText (const UpdateInfo& /*updateInfo*/) noexcept
    {
        // See docs on @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::HandleCallBeforeDidUpdateComplete' for an
        // explanation of this code.
        fEarlyDidUpdateCalled = true;
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::DidUpdateText
    @DESCRIPTION:
            <p>Override the standard @'MarkerOwner::DidUpdateText' to handle any updates which maybe needed to the MarkerCover.
        Calls @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::NoteCoverRangeDirtied'. Also checks the Invariant ()
        after its called (invariant could fail before DidUpdate() call).</p>
    */
    void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::DidUpdateText (const UpdateInfo& updateInfo) noexcept
    {
        fNeedExtraUpdateCheck = false;
        if (updateInfo.fTextModified) {
            NoteCoverRangeDirtied (updateInfo.fReplaceFrom, updateInfo.GetResultingRHS ());
        }
        inherited::DidUpdateText (updateInfo);
        fMarkersToBeDeleted.FinalizeMarkerDeletions ();
        Invariant ();
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::CullZerod
    @ACCESS:        protected
    @DESCRIPTION:
            <p>Internal utility routine, used to check for (and safely delete) zero-width cover elements.</p>
    */
    void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::CullZerod (size_t around) noexcept
    {
        CullZerod (CollectAllInRange_OrSurroundings (around, around));
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::CullZerod (const MarkerVector& rangeAndSurroundingsMarkers) noexcept
    {
        // all effected text is diff if we did a replace or not - if no, then from-to,
        // else from to from+textInserted (cuz from-to deleted)
        for (auto i = rangeAndSurroundingsMarkers.begin (); i != rangeAndSurroundingsMarkers.end (); ++i) {
            MARKER* m = *i;
            if (m->GetLength () == 0) {
                fMarkersToBeDeleted.SafeAccumulateMarkerForDeletion (m);
            }
        }
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::CheckForMerges (size_t around) noexcept
    {
        // Gather all style markers and sumarize them for the region which overlaps my change
        MarkerVector markers = CollectAllNonEmptyInRange_OrSurroundings (around, around);
        Assert (markers.size () != 0);

        if (markers.size () > 1) {
            Assert (markers.size () == 2); // since two character range, can be at most two style markers!
            MARKER* m1 = markers[0];
            MARKER* m2 = markers[1];
            Assert (m1->GetLength () != 0);
            Assert (m2->GetLength () != 0);
            if (m1->GetInfo () == m2->GetInfo ()) {
                // We then must merge one out. Doesn't matter which. Arbitrarily chose to keep rightmost one
                MARKER* deleteMe = (m1->GetStart () < m2->GetStart ()) ? m1 : m2;
                MARKER* keepMe   = (deleteMe == m1) ? m2 : m1;
                Assert (keepMe->GetStart () == deleteMe->GetEnd ());
                fTextStore.SetMarkerStart (keepMe, deleteMe->GetStart ());
                fTextStore.SetMarkerLength (deleteMe, 0); // so won't be refered to again once accumulated for deletion
                fMarkersToBeDeleted.AccumulateMarkerForDeletion (deleteMe);
            }
        }
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::CheckForMerges (const MarkerVector& rangeAndSurroundingsMarkers) noexcept
    {
        /*
         *  Argument markers must be pre-sorted.
         *
         *  Walk the list - and if any adjacent pairs are identical, merge one out.
         */
        if (rangeAndSurroundingsMarkers.size () >= 2) {
            typename MarkerVector::const_iterator i          = rangeAndSurroundingsMarkers.begin ();
            typename MarkerVector::const_iterator end        = rangeAndSurroundingsMarkers.end ();
            MARKER*                               prevMarker = *i;
            i++;
            for (; i != end; ++i) {
                MARKER* m1 = prevMarker;
                MARKER* m2 = *i;

                // handle tricky cases of zero-length markers...
                {
                    if (m1->GetLength () == 0) {
                        // if prev empty - skip ahead one...
                        prevMarker = m2;
                        continue;
                    }
                    if (m2->GetLength () == 0) {
                        // prev is set to the last non-empty marker, but we're currently looking at an empty one.
                        // Just skip it
                        continue;
                    }
                }

                Assert (m1->GetLength () != 0);
                Assert (m2->GetLength () != 0);
                if (m1->GetInfo () == m2->GetInfo ()) {
                    // We then must merge one out. Doesn't matter which. Arbitrarily chose to keep rightmost one
                    // (though the choice is arbitrary - it is assumed above - eariler on in this routine - when we
                    // handle zero-length markers).
                    Assert (m1->GetStart () < m2->GetStart ());
                    MARKER* deleteMe = m1;
                    MARKER* keepMe   = m2;
                    Assert (keepMe->GetStart () == deleteMe->GetEnd ());
                    fTextStore.SetMarkerStart (keepMe, deleteMe->GetStart ());
                    fTextStore.SetMarkerLength (deleteMe, 0); // so won't be refered to again once accumulated for deletion
                    fMarkersToBeDeleted.AccumulateMarkerForDeletion (deleteMe);
                }
                prevMarker = m2;
            }
        }
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    /*
    @METHOD:        MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::HandleCallBeforeDidUpdateComplete
    @ACCESS:        private
    @DESCRIPTION:
            <p>For the most part, we can call @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>::NoteCoverRangeDirtied' to cleanup
        any changes made to the text in this region, either through a 'SetInfo' call, or because of a plain text update.
        We hook the @'MarkerOwner::DidUpdateText' call to perform this validation of our MarkerCover.</p>
            <p>However, what happens if someone makes a change to the text, and after our 'AboutToUpdate' hook happens, but BEFORE
        our 'DidUpdate' hook - someone tries to query some info about the MarkerCover<>? See SPR#0599 for an example of this
        problem.</p>
            <p>Anyhow - we must protect our database so that it remains consistent, even if someone queries information after our
        'AboutToUpdate' hook, but before our 'DidUpdate' hook. Thats what this routine, together with a few flags, does.</p>
            <p>There is one more twist: Just what area (in HandleCallBeforeDidUpdateComplete) to we check if its after our
        'AboutToUpdate' hook and before our 'DidUpdate' hook? The answer is - it depends? If we are called from another AboutToUpdate ()
        handler, then the text hasn't changed, so we have no work todo. If we are called from another 'DidUpdate' hook, then we MUST process
        the NoteCoverRangeDirtied (). But we cannot BLINDLY do this. We need to know if the actual text modification has taken place (so we know
        which buffer offsets to use). It is for this reason that we have the 'fEarlyDidUpdateCalled' variable.</p>
    */
    inline void
    MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::HandleCallBeforeDidUpdateComplete () const noexcept
    {
        if (fNeedExtraUpdateCheck) {
            HandleCallBeforeDidUpdateComplete_ ();
        }
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::HandleCallBeforeDidUpdateComplete_ () const noexcept
    {
        /*
            *  If some 'GetInfo' routine is called - AFTER our AboutToUpdate () call but before our EarlyDidUpdate call, then
            *  the text hasn't truely been updated - so we cannot yet do our checks (and nor do we need to).
            */
        Assert (fNeedExtraUpdateCheck);
        if (fEarlyDidUpdateCalled) {
            const_cast<MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>*> (this)->NoteCoverRangeDirtied (fNeedExtraUpdateCheck_UpdateInfo.fReplaceFrom, fNeedExtraUpdateCheck_UpdateInfo.GetResultingRHS ());
            fMarkersToBeDeleted.FinalizeMarkerDeletions ();
            fNeedExtraUpdateCheck = false;
        }
    }
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    inline void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::Invariant () const
    {
#if qDebug && qHeavyDebugging
        Invariant_ ();
#endif
    }
#if qDebug
    template <typename MARKER, typename MARKERINFO, typename INCREMENTALMARKERINFO>
    void MarkerCover<MARKER, MARKERINFO, INCREMENTALMARKERINFO>::Invariant_ () const
    {
        MarkerVector markers = CollectAllInRange (0, fTextStore.GetEnd () + 1);
        sort (markers.begin (), markers.end (), LessThan<MARKER> ());

        // Walk through - and see we have a cover, and non-overlapping...
        Assert (markers.size () > 0);
        size_t lastEnd = 0;
        for (auto i = markers.begin (); i != markers.end (); i++) {
            MARKER* m = *i;
            AssertMember (m, MARKER);
            Assert (m->GetLength () > 0); // we should eliminate all zero-length markers...
            if (i == markers.begin ()) {
                Assert (m->GetStart () == 0);
            }
            if (i == markers.end () - 1) {
                Assert (m->GetStart () == lastEnd);
                Assert (m->GetEnd () == fTextStore.GetLength () + 1);
            }
            Assert (m->GetStart () == lastEnd);
            if (i != markers.begin ()) {
                MARKER* prevMarker = *(i - 1);
                AssertMember (prevMarker, MARKER);
                Assert (prevMarker->GetInfo () != m->GetInfo ()); // otherwise they should have been merged together
            }
            lastEnd = m->GetEnd ();
        }
        Assert (lastEnd == fTextStore.GetLength () + 1);
    }
#endif

}

#endif /*_Stroika_Frameworks_Led_MarkerCover_h_*/
