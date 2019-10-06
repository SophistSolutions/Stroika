/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_PartitioningTextImager_h_
#define _Stroika_Frameworks_Led_PartitioningTextImager_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Cache/LRUCache.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

/*
@MODULE:    PartitioningTextImager
@DESCRIPTION:
        <p>@'PartitioningTextImager' is based on the idea that a entire text buffer can be layed out
    by first breaking the text into special chunks, and then laying out those chunks.</p>
        <p>We use @'Marker's (or more specificly @'Partition::PartitionMarker') to delimit
    the chunks (refered to as partition elements).</p>
        <p>Note that it doesn't really matter much what the rule is for breaking text
    into chunks, except that the rule should be chosen to make the drawing of those chunks easier,
    and to fit in well with logical clipping.</p>
        <p>So for example, the most common rule for breaking text into partitions would be to look
    for LF chraracters, and consider each "line" to be a separate partition element.</p>
*/

#include "TextImager.h"

namespace Stroika::Frameworks::Led {

    /*
    @CONFIGVAR:     qCacheTextMeasurementsForPM
    @DESCRIPTION:   <p>A fairly simple, but effective performance hack. Defaults ON</p>
        */
#ifndef qCacheTextMeasurementsForPM
#define qCacheTextMeasurementsForPM 1
#endif

    /*
    @CLASS:         Partition
    @BASES:         virtual @'MarkerOwner'
    @DESCRIPTION:   <p>PartitioningTextImager is a TextImager which implements imaging by partitioning the text into stable
        regions, called Partitions.</p>
            <p>A partition is logically very similar to a @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>'. But we
        don't directly use that class because it is not an abstract class, but rather a utility template. And that utility
        has very different performance considerations than we require. Also, that class is used to associate a MARKERINFO
        with a particular partition element. There is nothing we here associate with the partition element.</p>
            <p>For more information, see <a href=../DesignManual/TextImagers.html>DesignManual/TextImagers.html</a>
        (NB: This code has changed ALOT between Led 2.2 and 2.3, and so this doc really needs to be rewritten!).</p>
    */
    class Partition : public virtual MarkerOwner {
    private:
        using inherited = MarkerOwner;

    public:
        Partition (TextStore& textStore);
        virtual ~Partition ();

    protected:
        virtual void FinalConstruct ();
#if qDebug
    private:
        bool fFinalConstructCalled;
#endif

    public:
        class PartitionMarker;

    public:
        virtual TextStore* PeekAtTextStore () const override;

    private:
        TextStore& fTextStore;

    protected:
        virtual void UpdatePartitions (PartitionMarker* pm, const UpdateInfo& updateInfo) noexcept = 0;

    public:
        virtual PartitionMarker* GetPartitionMarkerContainingPosition (size_t charPosition) const;

    private:
        mutable PartitionMarker* fFindContainingPMCache;

    protected:
        virtual PartitionMarker* MakeNewPartitionMarker (PartitionMarker* insertAfterMe); // if insertAfter==nullptr then prepend

    public:
        class PartitionWatcher;
        nonvirtual void AddPartitionWatcher (PartitionWatcher* watcher);
        nonvirtual void RemovePartitionWatcher (PartitionWatcher* watcher);

    private:
        vector<PartitionWatcher*> fPartitionWatchers;

    protected:
        virtual void Split (PartitionMarker* pm, size_t at);
        virtual void Coalece (PartitionMarker* pm); // call when pm loses its trialing NL (may do nothing)
    private:
        nonvirtual void DoAboutToSplitCalls (PartitionMarker* pm, size_t at, vector<void*>* infos) const noexcept;
        nonvirtual void DoDidSplitCalls (const vector<void*>& infos) const noexcept;
        nonvirtual void DoAboutToCoaleceCalls (PartitionMarker* pm, vector<void*>* infos) const noexcept;
        nonvirtual void DoDidCoaleceCalls (const vector<void*>& infos) const noexcept;

    protected:
        nonvirtual void                 AccumulateMarkerForDeletion (PartitionMarker* m);
        MarkerMortuary<PartitionMarker> fMarkersToBeDeleted;

    protected:
        virtual void AboutToUpdateText (const UpdateInfo& updateInfo) override;
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

        // Trivial TextStore wrappers...
    public:
        nonvirtual size_t GetEnd () const;
        nonvirtual void   CopyOut (size_t from, size_t count, Led_tChar* buffer) const;

    public:
        nonvirtual PartitionMarker* GetFirstPartitionMarker () const;
        nonvirtual PartitionMarker* GetLastPartitionMarker () const;

    private:
#if qDebug
        size_t fPartitionMarkerCount;
#endif
        PartitionMarker* fPartitionMarkerFirst;
        PartitionMarker* fPartitionMarkerLast;

        // Debug support
    public:
        nonvirtual void Invariant () const;
#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    private:
        friend class PartitionMarker; //  For UpdatePartitions () call
    };

    /*
    @CLASS:         Partition::PartitionMarker
    @BASES:         @'Marker'
    @DESCRIPTION:   <p>This class is the building block of a partition. It is used to keep track of a single partition element.</p>
                    <p><em>Subclassing Caution</em>: Since we provide operator new/delete overrides, you must be careful
                building any subclasses which have a different size than this class. You must provide your own overrides
                of operator new/delete todo so.</p>
    */
    class Partition::PartitionMarker : public Marker, public Foundation::Memory::UseBlockAllocationIfAppropriate<PartitionMarker> {
    private:
        using inherited = Marker;

    public:
        PartitionMarker (Partition& owner, PartitionMarker* insertAfterMe); // if insertAfter==nullptr then prepend

        // Calls Partition::UpdatePartitions ()
    public:
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

        // These markers are all kept in a doubly-linked list, headed by "GetOwner ()"
    public:
        virtual PartitionMarker* GetPrevious () const;
        virtual PartitionMarker* GetNext () const;

    private:
        PartitionMarker* fPrevious;
        PartitionMarker* fNext;

    public:
        nonvirtual Partition& GetOwner () const;

    private:
        friend class Partition;
    };

    /*
    @CLASS:         Partition::PartitionWatcher
    @DESCRIPTION:   <p>Some programs may need to keep track of when a partition changes. For example, an Imager will likely want to redisplay
        itself. Perhaps even do some more elaborate computation. Things like paragraph-based marker-covers (@'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>')
        may need to adjust their boundaries to correspond to the new paragraph boundaries
        (e.g. @'WordProcessor::ParagraphDatabaseRep').</p>
            <p>You get notified just before, and after partition markers are split, and/or coalesced. The untyped 'info' records are used to
        pass arbitrary bits of info from the 'about' call to the 'did' call.</p>
    */
    class Partition::PartitionWatcher {
    public:
        virtual void AboutToSplit (PartitionMarker* pm, size_t at, void** infoRecord) const noexcept = 0;
        virtual void DidSplit (void* infoRecord) const noexcept                                      = 0;
        virtual void AboutToCoalece (PartitionMarker* pm, void** infoRecord) const noexcept          = 0;
        virtual void DidCoalece (void* infoRecord) const noexcept                                    = 0;
    };

    /*
    @CLASS:         PartitioningTextImager
    @BASES:         virtual @'TextImager'
    @DESCRIPTION:   <p>PartitioningTextImager is a TextImager which implements imaging by partitioning the text into stable
        regions, called Partitions.</p>
            <p>For more information, see <a href=../DesignManual/TextImagers.html>DesignManual/TextImagers.html</a>.</p>
    */
    class PartitioningTextImager : public virtual TextImager {
    private:
        using inherited = TextImager;

    protected:
        PartitioningTextImager ();
        virtual ~PartitioningTextImager ();

    public:
        using PartitionPtr = shared_ptr<Partition>;
        nonvirtual PartitionPtr GetPartition () const;

    protected:
        virtual void SetPartition (const PartitionPtr& partitionPtr);

    private:
        PartitionPtr fPartition;

    public:
        virtual PartitionPtr MakeDefaultPartition () const = 0;

#if qCacheTextMeasurementsForPM
    private:
        class MeasureTextCache;
        unique_ptr<MeasureTextCache> fMeasureTextCache;

    protected:
        virtual void InvalidateAllCaches () override;
#endif

    public:
        using PartitionMarker = Partition::PartitionMarker;

        // Simple wrappers on the Partition object
    protected:
        nonvirtual PartitionMarker* GetFirstPartitionMarker () const;

    public:
        nonvirtual size_t GetStartOfPartitionContainingPosition (size_t charPosition) const;
        nonvirtual size_t GetEndOfPartitionContainingPosition (size_t charPosition) const;
        nonvirtual PartitionMarker* GetPartitionMarkerContainingPosition (size_t charPosition) const;

    public:
        virtual TextDirection GetPrimaryPartitionTextDirection (size_t rowContainingCharPosition) const;

    public:
        virtual TextLayoutBlock_Copy GetTextLayoutBlock (size_t rowStart, size_t rowEnd) const override;

    public:
        virtual TextDirection GetTextDirection (size_t charPosition) const override;

    public:
        virtual Led_Distance CalcSegmentSize (size_t from, size_t to) const override;

    public:
        virtual void   GetRowRelativeCharLoc (size_t charLoc, Led_Distance* lhs, Led_Distance* rhs) const override;
        virtual size_t GetRowRelativeCharAtLoc (Led_Coordinate hOffset, size_t rowStart) const override;

    private:
        nonvirtual Led_Distance CalcSegmentSize_REFERENCE (size_t from, size_t to) const;
#if qCacheTextMeasurementsForPM
        nonvirtual Led_Distance CalcSegmentSize_CACHING (size_t from, size_t to) const;
#endif

    private:
        nonvirtual void CalcSegmentSize_FillIn (size_t rowStart, size_t rowEnd, Led_Distance* distanceVector) const;

    protected:
        virtual size_t ResetTabStops (size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const;

        // Debug support
    public:
        nonvirtual void Invariant () const;
#if qDebug
    protected:
        virtual void Invariant_ () const;
#endif
    };

#if qCacheTextMeasurementsForPM
    /*
    @CLASS:         PartitioningTextImager::MeasureTextCache
    @BASES:         private @'Partition::PartitionWatcher', @'MarkerOwner'
    @DESCRIPTION:   <p>A helper class to implement the @'qCacheTextMeasurementsForPM' caching code.</p>
    */
    class PartitioningTextImager::MeasureTextCache : private Partition::PartitionWatcher, public MarkerOwner {
    private:
        using inherited = void*; // so any references to inherited generate an error...

    public:
        MeasureTextCache (const PartitionPtr& partition);
        ~MeasureTextCache ();

    public:
        virtual void AboutToSplit (PartitionMarker* pm, size_t at, void** infoRecord) const noexcept override;
        virtual void DidSplit (void* infoRecord) const noexcept override;
        virtual void AboutToCoalece (PartitionMarker* pm, void** infoRecord) const noexcept override;
        virtual void DidCoalece (void* infoRecord) const noexcept override;

    public:
        virtual TextStore* PeekAtTextStore () const override;

    protected:
        virtual void EarlyDidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    public:
        struct CacheEltLRUCacheTraits;
        class CacheElt {
        public:
            struct COMPARE_ITEM {
                COMPARE_ITEM (PartitionMarker* pm, size_t startingRowAt)
                    : fPM (pm)
                    , fRowStartingAt (startingRowAt)
                {
                }
                PartitionMarker* fPM;
                size_t           fRowStartingAt;
            };

        public:
            CacheElt ();
            CacheElt (const COMPARE_ITEM& ci);
            CacheElt (const CacheElt&) = default;

        private:
            COMPARE_ITEM fValidFor;

        public:
            Foundation::Memory::SmallStackBuffer<Led_Distance> fMeasurementsCache; // for just the given PM

        private:
            friend struct CacheEltLRUCacheTraits;
            friend class PartitioningTextImager::MeasureTextCache;
        };

    private:
        struct CacheElt_COMPARE_ITEM_KeyEqualsCompareFunctionType_ {
            bool operator() (const CacheElt::COMPARE_ITEM& lhs, const CacheElt::COMPARE_ITEM& rhs) const
            {
                return lhs.fPM == rhs.fPM and lhs.fRowStartingAt == rhs.fRowStartingAt;
            };
        };
        mutable Foundation::Cache::LRUCache<CacheElt::COMPARE_ITEM, CacheElt, CacheElt_COMPARE_ITEM_KeyEqualsCompareFunctionType_> fCache;

    public:
        nonvirtual void ClearAll ();

    public:
        nonvirtual CacheElt LookupValue (PartitionMarker* pm, size_t rowStart, const function<CacheElt (PartitionMarker*, size_t)>& valueFetcher);

    private:
        PartitionPtr fPartition;
    };
#endif

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    //  class   Partition::PartitionMarker;
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

#endif /*_Stroika_Frameworks_Led_PartitioningTextImager_h_*/
