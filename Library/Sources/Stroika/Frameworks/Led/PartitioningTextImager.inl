/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_PartitioningTextImager_inl_
#define _Stroika_Framework_Led_PartitioningTextImager_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

    /*
     ********************************************************************************
     ************************** Partition::PartitionMarker **************************
     ********************************************************************************
     */
    inline Partition::PartitionMarker::PartitionMarker (Partition& owner, PartitionMarker* insertAfterMe)
        : inherited ()
        , fPrevious (insertAfterMe)
        , fNext (insertAfterMe == nullptr ? nullptr : insertAfterMe->fNext)
    {
        if (insertAfterMe == nullptr) {
            owner.fPartitionMarkerFirst = this;
        }
        else {
            Assert (insertAfterMe->fNext == fNext); // I just updated MY next to be what HIS next used to be
            insertAfterMe->fNext       = this;      // Now HIS next should be me since I'm AFTER him
            PartitionMarker* following = fNext;
            if (following != nullptr) {
                Assert (following->fPrevious == insertAfterMe); // thats who he USED to point to
                following->fPrevious = this;
            }
        }
        if (fNext == nullptr) {
            owner.fPartitionMarkerLast = this;
        }
    }
    /*
    @METHOD:        Partition::PartitionMarker::GetOwner ()
    @DESCRIPTION:   <p>Return a reference to the partition markers owner. Note - since this returns a refernece,
        we assert that the owner is non-null. Also note that this routine intentionally hides the base-class
        @'Marker::GetOwner()' method. Use that if the owner could be nullptr. But note that the owner will never
        be nullptr if the marker has been added (and not yet removed) from the TextStore.</p>
    */
    inline Partition& Partition::PartitionMarker::GetOwner () const
    {
        EnsureNotNull (inherited::GetOwner ());
        EnsureMember (inherited::GetOwner (), Partition);
        return *(dynamic_cast<Partition*> (inherited::GetOwner ()));
    }
    inline Partition::PartitionMarker* Partition::PartitionMarker::GetPrevious () const
    {
        return fPrevious;
    }
    inline Partition::PartitionMarker* Partition::PartitionMarker::GetNext () const
    {
        return fNext;
    }
    inline void Partition::Invariant () const
    {
#if qDebug && qHeavyDebugging
        Invariant_ ();
#endif
    }

    //  class   Partition;
    /*
    @METHOD:        Partition::GetEnd
    @DESCRIPTION:   <p>Return the associated @'TextStore::GetEnd' ().</p>
    */
    inline size_t Partition::GetEnd () const
    {
        return (fTextStore.GetEnd ());
    }
    /*
    @METHOD:        Partition::CopyOut
    @DESCRIPTION:   <p>Calls the associated @'TextStore::CopyOut' ().</p>
    */
    inline void Partition::CopyOut (size_t from, size_t byteCount, Led_tChar* buffer) const
    {
        GetTextStore ().CopyOut (from, byteCount, buffer);
    }
    inline Partition::PartitionMarker* Partition::GetFirstPartitionMarker () const
    {
        RequireNotNull (fPartitionMarkerFirst);
        return fPartitionMarkerFirst;
    }
    inline Partition::PartitionMarker* Partition::GetLastPartitionMarker () const
    {
        RequireNotNull (fPartitionMarkerLast);
        return fPartitionMarkerLast;
    }
    /*
    @METHOD:        Partition::AddPartitionWatcher
    @DESCRIPTION:   <p>Associate the given @'Partition::PartitionWatcher' with this Partition. Use
        @'Partition::RemovePartitionWatcher' to remove the callback / association.</p>
    */
    inline void Partition::AddPartitionWatcher (PartitionWatcher* watcher)
    {
        fPartitionWatchers.push_back (watcher);
    }
    /*
    @METHOD:        Partition::RemovePartitionWatcher
    @DESCRIPTION:   <p>Remove the given @'Partition::PartitionWatcher' from being associated with this @'Partition'.
        Balances calls from @'Partition::AddPartitionWatcher'.</p>
    */
    inline void Partition::RemovePartitionWatcher (PartitionWatcher* watcher)
    {
        vector<PartitionWatcher*>::iterator it = find (fPartitionWatchers.begin (), fPartitionWatchers.end (), watcher);
        Assert (it != fPartitionWatchers.end ()); // Be forgiving about not finding in list, in light of exception handling - but give warning...
        if (it != fPartitionWatchers.end ()) {
            fPartitionWatchers.erase (it);
        }
    }
    inline void Partition::DoAboutToSplitCalls (PartitionMarker* pm, size_t at, vector<void*>* infos) const noexcept
    {
        for (auto it = fPartitionWatchers.begin (); it != fPartitionWatchers.end (); ++it) {
            void* info;
            (*it)->AboutToSplit (pm, at, &info);
            infos->push_back (info);
        }
    }
    inline void Partition::DoDidSplitCalls (const vector<void*>& infos) const noexcept
    {
        Assert (infos.size () == fPartitionWatchers.size ());
        vector<void*>::const_iterator infoIt = infos.begin ();
        for (auto it = fPartitionWatchers.begin (); it != fPartitionWatchers.end (); ++it) {
            (*it)->DidSplit (*infoIt);
            ++infoIt;
        }
    }
    inline void Partition::DoAboutToCoaleceCalls (PartitionMarker* pm, vector<void*>* infos) const noexcept
    {
        for (auto it = fPartitionWatchers.begin (); it != fPartitionWatchers.end (); ++it) {
            void* info;
            (*it)->AboutToCoalece (pm, &info);
            infos->push_back (info);
        }
    }
    inline void Partition::DoDidCoaleceCalls (const vector<void*>& infos) const noexcept
    {
        Assert (infos.size () == fPartitionWatchers.size ());
        vector<void*>::const_iterator infoIt = infos.begin ();
        for (auto it = fPartitionWatchers.begin (); it != fPartitionWatchers.end (); ++it) {
            (*it)->DidCoalece (*infoIt);
            ++infoIt;
        }
    }

    //  class   PartitioningTextImager;
    inline PartitioningTextImager::PartitionPtr PartitioningTextImager::GetPartition () const
    {
        return fPartition;
    }
    inline Partition::PartitionMarker* PartitioningTextImager::GetFirstPartitionMarker () const
    {
        Require (fPartition.get () != nullptr); // perhaps you've forgotten to call SpecifyTextStore or SetPartition ()?
        return fPartition->GetFirstPartitionMarker ();
    }
    /*
    @METHOD:        PartitioningTextImager::GetPartitionMarkerContainingPosition
    @DESCRIPTION:   <p>Finds the @'PartitioningTextImager::PartitionMarker' which contains the given character#.
        Note, the use of 'charPosition' rather than markerpos is to disambiguiate the case where we are at the boundary
        between two partition elements.</p>
    */
    inline Partition::PartitionMarker* PartitioningTextImager::GetPartitionMarkerContainingPosition (size_t charPosition) const
    {
        return fPartition->GetPartitionMarkerContainingPosition (charPosition);
    }
    /*
    @METHOD:        PartitioningTextImager::GetStartOfPartitionContainingPosition
    @DESCRIPTION:   <p>Simple wrapper on @'PartitioningTextImager::GetPartitionMarkerContainingPosition'.</p>
    */
    inline size_t PartitioningTextImager::GetStartOfPartitionContainingPosition (size_t charPosition) const
    {
        return GetPartitionMarkerContainingPosition (charPosition)->GetStart ();
    }
    /*
    @METHOD:        PartitioningTextImager::GetEndOfPartitionContainingPosition
    @DESCRIPTION:   <p>Simple wrapper on @'PartitioningTextImager::GetPartitionMarkerContainingPosition'. Returns marker position
        AFTER end of partition (same as START position of following partition - if any).</p>
    */
    inline size_t PartitioningTextImager::GetEndOfPartitionContainingPosition (size_t charPosition) const
    {
        return GetPartitionMarkerContainingPosition (charPosition)->GetEnd ();
    }
    inline void PartitioningTextImager::Invariant () const
    {
#if qDebug && qHeavyDebugging
        Invariant_ ();
#endif
    }

#if qCacheTextMeasurementsForPM
    //  class   PartitioningTextImager::MeasureTextCache
    inline void PartitioningTextImager::MeasureTextCache::ClearAll ()
    {
        fCache.clear ();
    }
    inline PartitioningTextImager::MeasureTextCache::CacheElt PartitioningTextImager::MeasureTextCache::LookupValue (PartitionMarker* pm, size_t rowStart, const function<CacheElt (PartitionMarker*, size_t)>& valueFetcher)
    {
        RequireNotNull (pm);
        using CacheElt = PartitioningTextImager::MeasureTextCache::CacheElt;
        return fCache.LookupValue (CacheElt::COMPARE_ITEM (pm, rowStart), [valueFetcher] (const CacheElt::COMPARE_ITEM& c) { return valueFetcher (c.fPM, c.fRowStartingAt); });
    }

    //  class   PartitioningTextImager::MeasureTextCache::CacheElt
    inline PartitioningTextImager::MeasureTextCache::CacheElt::CacheElt ()
        : fValidFor (nullptr, 0)
        , fMeasurementsCache (0)
    {
    }
    inline PartitioningTextImager::MeasureTextCache::CacheElt::CacheElt (const COMPARE_ITEM& ci)
        : fValidFor{ci}
        , fMeasurementsCache (0)
    {
    }
#endif

}

#endif /*_Stroika_Framework_Led_PartitioningTextImager_inl_*/
