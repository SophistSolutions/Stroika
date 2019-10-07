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

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MarkerCover.inl"

#endif /*_Stroika_Frameworks_Led_MarkerCover_h_*/
