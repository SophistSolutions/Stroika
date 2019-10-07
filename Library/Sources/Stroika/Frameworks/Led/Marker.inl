/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_Marker_inl_
#define _Stroika_Framework_Led_Marker_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    //  class   MarkerOwner;
    /*
    @METHOD:        MarkerOwner::GetTextStore
    @DESCRIPTION:   <p>This is a trivial wrapper on @'MarkerOwner::PeekAtTextStore' which asserts that
        PeekAtTextStore () didn't return nullptr, and then returns its result dereferences. Use this for
        clarity sake when you are sure the markerowner must have an associated TextStore.</p>
    */
    inline TextStore& MarkerOwner::GetTextStore () const
    {
        TextStore* ts = PeekAtTextStore ();
        EnsureNotNull (ts);
        return *ts;
    }
    /*
    @METHOD:        MarkerOwner::AboutToUpdateText
    @DESCRIPTION:   <p>This method is called by a TextStore when text is being updated for all registered MarkerOwners
        (see @'TextStore::AddMarkerOwner'). You can throw an exception to prevent text from being updated.</p>
    */
    inline void MarkerOwner::AboutToUpdateText (const UpdateInfo& /*updateInfo*/)
    {
    }
    /*
    @METHOD:        MarkerOwner::EarlyDidUpdateText
    @DESCRIPTION:   <p>This method is called by a @'TextStore' <em>just</em>after a text update has taken place (and before the <em>DidUpdateText</em> calls take place).
        You can use this call just to set private data in your objects - but make no calls out (til the DidUpdateText). This is just so that
        in some rare cases - you can tell if your getting a callback between your AboutToUpdate() call and your DidUpdateText () call - so you can
        tell if the text is truely updated yet.</p>
    */
    inline void MarkerOwner::EarlyDidUpdateText (const UpdateInfo& /*updateInfo*/) noexcept
    {
    }
    /*
    @METHOD:        MarkerOwner::DidUpdateText
    @DESCRIPTION:   <p>This method is called by a @'TextStore' after a text update has taken place. You can use this to update
        some appropriate data structures based on the change. NB: an exception <em>cannot</em> be raised by this method, or
        any overrides of it.</p>
    */
    inline void MarkerOwner::DidUpdateText (const UpdateInfo& /*updateInfo*/) noexcept
    {
    }

    //  class   Marker;
    inline Marker::Marker ()
        : fTextStoreHook (nullptr)
    {
    }
    inline Marker::~Marker ()
    {
    }
    /*
    @METHOD:        Marker::AboutToUpdateText
    @DESCRIPTION:   <p>Notification callback which is called by a @'TextStore' when the text is updated. You override this
        if you want todo something special when the text marked by this particular marker is changed. You can throw
        from this method to prevent the update from actually taking place.</p>
    */
    inline void Marker::AboutToUpdateText (const UpdateInfo& /*updateInfo*/)
    {
    }
    /*
    @METHOD:        Marker::DidUpdateText
    @DESCRIPTION:   <p>Notification callback which is called by a @'TextStore' after the text is updated. You override this
        if you want todo something special when the text marked by this particular marker is changed. You cannot throw
        from this routine. Use @'Marker::AboutToUpdateText' to preflight, and assure any DidUpdateText method calls will
        work without a hitch.</p>
    */
    inline void Marker::DidUpdateText (const UpdateInfo& /*updateInfo*/) noexcept
    {
    }
    /*
    @METHOD:        Marker::GetStart
    @DESCRIPTION:   <p>Returns the marker position of the start (lhs) of the marker.</p>
            <p>It is illegal to call this
        if the marker is not currently added to a @'TextStore'.</p>
    */
    inline size_t Marker::GetStart () const
    {
        AssertNotNull (fTextStoreHook);
        return (fTextStoreHook->GetStart ());
    }
    /*
    @METHOD:        Marker::GetEnd
    @DESCRIPTION:   <p>Returns the marker position of the end (rhs) of the marker.</p>
            <p>It is illegal to call this
        if the marker is not currently added to a @'TextStore'.</p>
    */
    inline size_t Marker::GetEnd () const
    {
        AssertNotNull (fTextStoreHook);
        return (fTextStoreHook->GetEnd ());
    }
    /*
    @METHOD:        Marker::GetLength
    @DESCRIPTION:   <p>Return the length - in @'Led_tChar's - of the marker span. This value can be zero. But if it
        ever goes to zero, Led will never re-inflate the marker. You must reset its bounds manually via @'Marker::SetRange' ().</p>
    */
    inline size_t Marker::GetLength () const
    {
        AssertNotNull (fTextStoreHook);
        return (fTextStoreHook->GetLength ());
    }
    /*
    @METHOD:        Marker::GetOwner
    @DESCRIPTION:   <p>Return the current marker owner. Unlike most Marker methods, this <em>can</em> be called when the
        marker hasn't yet been added to a @'TextStore'. It just returns nullptr in that case. Note, it can return nullptr anyhow,
        as that is a valid value to specify in @'TextStore::AddMarker' ().</p>
    */
    inline MarkerOwner* Marker::GetOwner () const
    {
        // fTextStoreHook CAN be nullptr here if we don't yet have a marker owner!
        return (fTextStoreHook == nullptr ? nullptr : fTextStoreHook->GetOwner ());
    }
    /*
    @METHOD:        Marker::GetRange
    @DESCRIPTION:   <p>Return the start and end position of the marker. You must set its bounds
        via @'Marker::SetRange' ().</p>
    */
    inline void Marker::GetRange (size_t* start, size_t* end) const
    {
        RequireNotNull (start);
        RequireNotNull (end);
        RequireNotNull (fTextStoreHook);
        fTextStoreHook->GetStartEnd (start, end);
    }

    //  class   MarkerOwner::UpdateInfo;
    inline MarkerOwner::UpdateInfo::UpdateInfo (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCharCount, bool textModified, bool realContentUpdate)
        : fReplaceFrom (from)
        , fReplaceTo (to)
        , fTextInserted (withWhat)
        , fTextLength (withWhatCharCount)
        , fTextModified (textModified)
        , fRealContentUpdate (realContentUpdate)
    {
    }
    inline size_t MarkerOwner::UpdateInfo::GetResultingRHS () const
    {
        return fTextModified ? (fReplaceFrom + fTextLength) : fReplaceTo;
    }

    //  class   MarkerMortuary<MARKER>;
    template <typename MARKER>
    inline MarkerMortuary<MARKER>::~MarkerMortuary ()
    {
        Assert (fMarkersToBeDeleted.size () == 0); // these better be deleted by now!
    }
    template <typename MARKER>
    /*
    @METHOD:        MarkerMortuary<MARKER>::AccumulateMarkerForDeletion
    @DESCRIPTION:   <p>Since Led can remove large numbers of markers at a time much faster than it can
        remove a single one, we have this special interface to accumulate markers to be
        deleted, and delete them all at once. This doesn't ususally <em>NEED</em> to be used. But it sometimes
        does <em>Need</em> to be used to avoid the case where a marker is kept in a list (say back on the stack in
        some caller for example), and we decide it must be deleted, but we don't know for sure if it
        will be accessed from that stack-based list again.</p>
            <p>After a call to this routine, the marker is effectively dead and removed
        from OUR marker list, but it is not 'delete' d. And the call to RemoveMarker</p>
        is postponed til '@'MarkerMortuary<MARKER>::FinalizeMarkerDeletions ()'.
            <p>NB:  It is illegal to accumulate a marker for deletion twice (detected error). And greatly
        discouraged using it afterwards.</p>
    */
    inline void MarkerMortuary<MARKER>::AccumulateMarkerForDeletion (MARKER* m)
    {
        RequireNotNull (m);
        Require (IndexOf (fMarkersToBeDeleted, m) == kBadIndex);
#if qDebug
        if (fMarkersToBeDeleted.size () != 0) {
            RequireNotNull (static_cast<Marker*> (fMarkersToBeDeleted[0])->GetOwner ());
            RequireNotNull (static_cast<Marker*> (fMarkersToBeDeleted[0])->GetOwner ()->PeekAtTextStore ());
            RequireNotNull (static_cast<Marker*> (m)->GetOwner ());
            RequireNotNull (static_cast<Marker*> (m)->GetOwner ()->PeekAtTextStore ());
            Require (static_cast<Marker*> (fMarkersToBeDeleted[0])->GetOwner ()->PeekAtTextStore () == static_cast<Marker*> (m)->GetOwner ()->PeekAtTextStore ());
        }
#endif

        // Added for SPR#0822 - see for details
        static_cast<Marker*> (m)->GetOwner ()->GetTextStore ().PreRemoveMarker (m);

        // NB: fMarkersToBeDeleted SB a linked list, so we don't need todo any mem allocations
        // and don't need to worry about failing to allocate memory here!!!
        fMarkersToBeDeleted.push_back (m);
    }
    template <typename MARKER>
    /*
    @METHOD:        MarkerMortuary<MARKER>::SafeAccumulateMarkerForDeletion
    @DESCRIPTION:   <p>Like @'MarkerMortuary<MARKER>::AccumulateMarkerForDeletion', but its OK to add a marker more than once.</p>
    */
    void MarkerMortuary<MARKER>::SafeAccumulateMarkerForDeletion (MARKER* m)
    {
        RequireNotNull (m);
        if (IndexOf (fMarkersToBeDeleted, m) == kBadIndex) {
            AccumulateMarkerForDeletion (m);
        }
    }
    template <typename MARKER>
    /*
    @METHOD:        MarkerMortuary<MARKER>::FinalizeMarkerDeletions
    @DESCRIPTION:   <p>Call anytime, but preferably after we've finished doing a bunch of marker deletions.
        See @'MarkerMortuary<MARKER>::AccumulateMarkerForDeletion' for more information.</p>
    */
    void
    MarkerMortuary<MARKER>::FinalizeMarkerDeletions () noexcept
    {
        if (fMarkersToBeDeleted.size () != 0) {
            MarkerOwner* owner = static_cast<Marker*> (fMarkersToBeDeleted[0])->GetOwner ();
            AssertNotNull (owner);
            TextStore& textStore = owner->GetTextStore ();

            // NB: No exceptions can happen in any of this - all these deletes allocate no memory (LGP 950415)
            MARKER* const* markersToBeDeleted_ = &fMarkersToBeDeleted.front ();
            Marker* const* markersToBeDeleted  = (Marker* const*)markersToBeDeleted_; // need cast - but safe - cuz array of MARKER* and looking for Marker* - safe cuz const array!

            textStore.RemoveMarkers (markersToBeDeleted, fMarkersToBeDeleted.size ());
            for (size_t i = 0; i < fMarkersToBeDeleted.size (); i++) {
                delete fMarkersToBeDeleted[i];
            }
            fMarkersToBeDeleted.clear ();
        }
    }
    template <typename MARKER>
    /*
    @METHOD:        MarkerMortuary<MARKER>::IsEmpty
    @DESCRIPTION:   <p>Mostly used for assertions. Checks all accumulated markers have been finalized.</p>
    */
    inline bool MarkerMortuary<MARKER>::IsEmpty () const noexcept
    {
        return fMarkersToBeDeleted.size () == 0;
    }

    //  ::Contains;
    /*
    @METHOD:        Contains
    @DESCRIPTION:   <p>Four overloads. Basically this tests if the first marker is contained in the second.
                </p>
                    <p>Note that Led doesn't treat the beginning and ending of a marker symmetrically. It
                is considered legitimate containment if the marker start positions are equal, but <em>not</em>
                if the marker end positions are equal</p>
    */
    inline bool Contains (const Marker& containedMarker, const Marker& containerMarker)
    {
        size_t containerStart;
        size_t containerEnd;
        containerMarker.GetRange (&containerStart, &containerEnd);

        size_t containedStart;
        size_t containedEnd;
        containedMarker.GetRange (&containedStart, &containedEnd);
        return ((containedStart >= containerStart) and (containerEnd >= containedEnd));
    }
    inline bool Contains (size_t containedMarkerStart, size_t containedMarkerEnd, const Marker& containerMarker)
    {
        Assert (containedMarkerStart <= containedMarkerEnd);
        size_t containerStart;
        size_t containerEnd;
        containerMarker.GetRange (&containerStart, &containerEnd);
        return ((containedMarkerStart >= containerStart) and (containerEnd >= containedMarkerEnd));
    }
    inline bool Contains (const Marker& marker, size_t charPos)
    {
        size_t start;
        size_t end;
        marker.GetRange (&start, &end);
        return (charPos >= start and charPos < end);
    }
    inline bool Contains (size_t containedMarkerStart, size_t containedMarkerEnd, size_t charPos)
    {
        return (charPos >= containedMarkerStart and charPos < containedMarkerEnd);
    }

    //  class   TempMarker
    inline size_t TempMarker::GetStart () const
    {
        return fMarker.GetStart ();
    }
    inline size_t TempMarker::GetEnd () const
    {
        return fMarker.GetEnd ();
    }
    inline size_t TempMarker::GetLength () const
    {
        return fMarker.GetLength ();
    }
    inline void TempMarker::GetLocation (size_t* start, size_t* end) const
    {
        fMarker.GetRange (start, end);
    }

    //  class   TemporaryMarkerSlideDown<MARKER>
    template <typename MARKER>
    TemporaryMarkerSlideDown<MARKER>::TemporaryMarkerSlideDown (TextStore& ts, const vector<MARKER*>& m, ptrdiff_t slideBy)
        : fTextStore (ts)
        , fMarkers (m)
        , fSlideBy (slideBy)
    {
        for (auto i = fMarkers.begin (); i != fMarkers.end (); ++i) {
            size_t start = 0;
            size_t end   = 0;
            (*i)->GetRange (&start, &end);
            fTextStore.SetMarkerRange ((*i), start + fSlideBy, end + fSlideBy);
        }
    }
    template <typename MARKER>
    TemporaryMarkerSlideDown<MARKER>::~TemporaryMarkerSlideDown ()
    {
        for (auto i = fMarkers.begin (); i != fMarkers.end (); ++i) {
            size_t start = 0;
            size_t end   = 0;
            (*i)->GetRange (&start, &end);
            fTextStore.SetMarkerRange ((*i), start - fSlideBy, end - fSlideBy);
        }
    }

}

#endif /*_Stroika_Framework_Led_Marker_inl_*/
