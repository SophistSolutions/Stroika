/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_MultiRowTextImager_h_
#define _Stroika_Frameworks_Led_MultiRowTextImager_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    MultiRowTextImager
@DESCRIPTION:
        <p>MultiRowTextImager is a @'TextImager' which supports having partition elements made up of multirow text.</p>

 */

#include <climits> // for UINT_MAX
#include <cstring>

#include "LineBasedPartition.h"
#include "PartitioningTextImager.h"

namespace Stroika::Frameworks::Led {

    /*
    @CLASS:         MultiRowTextImager
    @BASES:         @'PartitioningTextImager'
    @DESCRIPTION:
            <p>A @'PartitioningTextImager' which supports the notion of having multiple rows of text per line.</p>
            <p>NB: Although this class properly supports the APIs for @'PartitioningTextImager', it has a
        limitation, based on its historical implementation. It assumes that there is a one-to-one
        correspondance between its Partition and the TextImager, and that this Partition is (or subclasses from
        @'MultiRowTextImager::MultiRowPartition').</p>
            <p>This limitation is partly (largely) based on the fact that we use the trick of piggybacking row-wrap
        information on the partition markers themselves. A future version of this class might keep the per-row information
        in a MarkerCover, or some such other data structure.</p>
    */
    class MultiRowTextImager : public PartitioningTextImager {
    protected:
        MultiRowTextImager ();
        virtual ~MultiRowTextImager ();

    private:
        MultiRowTextImager (const MultiRowTextImager&);        // don't call. not implemented
        nonvirtual void operator= (const MultiRowTextImager&); // don't call. not implemented

    private:
        using inherited = PartitioningTextImager;

    protected:
        virtual void    HookLosingTextStore () override;
        nonvirtual void HookLosingTextStore_ ();
        virtual void    HookGainedNewTextStore () override;
        nonvirtual void HookGainedNewTextStore_ ();

    public:
        class PartitionElementCacheInfo;

    protected:
        virtual void SetPartition (const PartitionPtr& partitionPtr) override;

    public:
        virtual PartitionPtr MakeDefaultPartition () const override;

    public:
        /*
        @CLASS:         MultiRowTextImager::RowReference
        @DESCRIPTION:
                <p>A utility class to represent a row. It is a struct with a parition marker, and a row number.
                These things are NOT long-lived. And shouldn't be saved anyplace, as no attempt is made to keep
                them automaticlly up to date as the text is modified.
                They are just a convenient, short-hand way to navigate through rows of text.</p>
        */
        class RowReference {
        public:
            RowReference (const RowReference& from);
            RowReference (PartitionMarker* partitionMarker, size_t subRow);

        private:
            RowReference (); // left undefined to assure never called...
        public:
            nonvirtual RowReference& operator= (const RowReference& rhs);

        public:
            nonvirtual PartitionMarker* GetPartitionMarker () const;
            nonvirtual size_t GetSubRow () const;

        private:
            PartitionMarker* fPartitionMarker;
            size_t           fSubRow;
        };
        friend class RowReference;
        friend bool operator== (RowReference lhs, RowReference rhs);
        friend bool operator!= (RowReference lhs, RowReference rhs);

    public:
        nonvirtual PartitionElementCacheInfo GetPartitionElementCacheInfo (Partition::PartitionMarker* pm) const;
        nonvirtual PartitionElementCacheInfo GetPartitionElementCacheInfo (RowReference row) const;

    private:
        class PMInfoCacheMgr;
        unique_ptr<PMInfoCacheMgr> fPMCacheMgr;

        // Row Reference support routines...
    public:
        nonvirtual bool GetNextRowReference (RowReference* adjustMeInPlace) const;     // return true if there is a next, and false if at end
        nonvirtual bool GetPreviousRowReference (RowReference* adjustMeInPlace) const; // return true if there is a previous, and false if at the beginning

        // NB: if ith==1, that means do NOTHING - for convenience...
        nonvirtual bool GetIthRowReferenceFromHere (RowReference* adjustMeInPlace, ptrdiff_t ith) const; // return true if there is an ith, and false if we run off end... (ith==0 implies no change, < 0 means go back)
        nonvirtual RowReference GetIthRowReferenceFromHere (RowReference fromHere, ptrdiff_t ith) const; // ERROR if ith doesn't exist... (ith==0 implies no change, < 0 means go back)
        nonvirtual RowReference GetIthRowReference (size_t ith) const;                                   // ERROR if ith doesn't exist...(1 th is first row)

        nonvirtual size_t GetRowNumber (RowReference rowRef) const; // Use of row numbers is discouraged, but this routine
        // can be helpful in implementing those APIs anyhow

        nonvirtual size_t CountRowDifference (RowReference lhs, RowReference rhs) const;
        nonvirtual size_t CountRowDifferenceLimited (RowReference lhs, RowReference rhs, size_t limit) const;

        /*
         *  Window/Scrolling support.
         */
    public:
        virtual size_t    GetTopRowInWindow () const override;
        virtual size_t    GetTotalRowsInWindow () const override;
        virtual size_t    GetLastRowInWindow () const override;
        virtual void      SetTopRowInWindow (size_t newTopRow) override;
        virtual size_t    GetMarkerPositionOfStartOfWindow () const override;
        virtual size_t    GetMarkerPositionOfEndOfWindow () const override;
        virtual size_t    GetMarkerPositionOfStartOfLastRowOfWindow () const override;
        virtual ptrdiff_t CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const override;
        virtual ptrdiff_t CalculateCharDeltaFromRowDeltaFromTopOfWindow (ptrdiff_t deltaRows) const override;
        virtual void      ScrollByIfRoom (ptrdiff_t downByRows); // if downBy negative then up
        // OK to ask to scroll further
        // than allowed - return true
        // if any scrolling (not necesarily
        // same amont requested) done
    public:
        virtual void ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0) override;

    protected:
        nonvirtual RowReference GetTopRowReferenceInWindow () const;
        nonvirtual RowReference GetLastRowReferenceInWindow () const;
        virtual void            SetTopRowInWindow (RowReference row);

    protected:
        nonvirtual void SetTopRowInWindow_ (RowReference row); // just sets the fields without any hook functions
        // getting called. This is important sometimes when
        // it would be unsafe for subclasses to get a chance
        // to call methods while our data structures are not
        // not completely up-to-date.

    protected:
        virtual void AssureWholeWindowUsedIfNeeded () override;

    public:
        virtual Led_Rect GetCharLocation (size_t afterPosition) const override;
        virtual size_t   GetCharAtLocation (const Led_Point& where) const override;
        virtual Led_Rect GetCharWindowLocation (size_t afterPosition) const override;
        virtual size_t   GetCharAtWindowLocation (const Led_Point& where) const override;

        virtual size_t   GetStartOfRow (size_t rowNumber) const override;
        virtual size_t   GetStartOfRowContainingPosition (size_t charPosition) const override;
        virtual size_t   GetEndOfRow (size_t rowNumber) const override;
        virtual size_t   GetEndOfRowContainingPosition (size_t charPosition) const override;
        virtual size_t   GetRealEndOfRow (size_t rowNumber) const override;
        virtual size_t   GetRealEndOfRowContainingPosition (size_t charPosition) const override;
        virtual size_t   GetRowContainingPosition (size_t charPosition) const override;
        virtual size_t   GetRowCount () const override;
        virtual Led_Rect GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck) const override;

    public:
        nonvirtual size_t GetStartOfRow (RowReference row) const;
        nonvirtual size_t GetEndOfRow (RowReference row) const;
        nonvirtual size_t GetRealEndOfRow (RowReference row) const;
        nonvirtual RowReference GetRowReferenceContainingPosition (size_t charPosition) const;
        nonvirtual size_t GetRowLength (RowReference row) const;

    public:
        virtual Led_Distance GetRowHeight (size_t rowNumber) const override;
        nonvirtual Led_Distance GetRowHeight (RowReference row) const;

    public:
        virtual Led_Distance GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition) const override;

    public:
        nonvirtual Led_Distance GetHeightOfRows (size_t startingRow, size_t rowCount) const;
        nonvirtual Led_Distance GetHeightOfRows (RowReference startingRow, size_t rowCount) const;

    public:
        virtual void GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
                                                               size_t* expandedFromMarkerPos, size_t* expandedToMarkerPos) const;

    public:
        virtual void Draw (const Led_Rect& subsetToDraw, bool printing);

    public:
        virtual void DrawPartitionElement (PartitionMarker* pm, size_t startSubRow, size_t maxSubRow, Led_Tablet tablet, OffscreenTablet* offscreenTablet, bool printing, const Led_Rect& subsetToDraw, Led_Rect* remainingDrawArea, size_t* rowsDrawn);

    protected:
        virtual Led_Rect GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck = UINT_MAX) const;
        virtual size_t   GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck = UINT_MAX) const;

    protected:
        virtual void         FillCache (PartitionMarker* pm, PartitionElementCacheInfo& cacheInfo) = 0;
        virtual Led_Distance CalculateInterLineSpace (const PartitionMarker* pm) const;

    protected:
        virtual bool   ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const;
        virtual size_t RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const;

#if 0
        // Hook to invalidate cached info based on fontmetrics
    public:
        virtual     void    SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont);
    protected:
        nonvirtual  void    SetDefaultFont_ (const Led_IncrementalFontSpecification& /*defaultFont*/);  // Merely invalidates font metrics
#endif

        // To assure our top-line scroll info not left corrupt...
    protected:
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept;

        // override to invalidate caches.
    public:
        virtual void SetWindowRect (const Led_Rect& windowRect) override;

    protected:
        virtual void InvalidateAllCaches () override;

    private:
        nonvirtual RowReference AdjustPotentialTopRowReferenceSoWholeWindowUsed (const RowReference& potentialTopRow);
        nonvirtual bool         PositionWouldFitInWindowWithThisTopRow (size_t markerPos, const RowReference& newTopRow);

    private:
        PartitionMarker* fTopLinePartitionMarkerInWindow;
        size_t           fSubRowInTopLineInWindow;

    private:
        nonvirtual void ReValidateSubRowInTopLineInWindow ();

        // Support for GetTotalRowsInWindow
        //
        // Override ComputeRowsThatWouldFitInWindowWithTopRow () to change the policy of how we
        // pack rows into a window
    private:
        mutable size_t fTotalRowsInWindow; // zero means invalid cached - fill cache on call to GetTotalRowsInWindow
    protected:
        nonvirtual size_t GetTotalRowsInWindow_ () const;
        nonvirtual void   InvalidateTotalRowsInWindow ();
        virtual size_t    ComputeRowsThatWouldFitInWindowWithTopRow (const RowReference& newTopRow) const;

    private:
        friend class PMInfoCacheMgr;
    };

    /*
    @CLASS:         MultiRowTextImager::PartitionElementCacheInfo
    @DESCRIPTION:
            <p></p>
    */
    class MultiRowTextImager::PartitionElementCacheInfo {
    public:
        PartitionElementCacheInfo ();

    public:
        nonvirtual Led_Distance GetPixelHeight () const;
        nonvirtual size_t GetRowCount () const;
        nonvirtual size_t PeekRowCount () const;
        nonvirtual size_t GetLastRow () const;
        nonvirtual Led_Distance GetRowHeight (size_t ithRow) const;
        nonvirtual size_t GetLineRelativeRowStartPosition (size_t ithRow) const;
        nonvirtual size_t LineRelativePositionInWhichRow (size_t charPos) const; // ZERO based charPos - ie zero is just before first byte in first row

    public:
        nonvirtual Led_Distance GetInterLineSpace () const;
        nonvirtual void         SetInterLineSpace (Led_Distance interlineSpace);

        /*
     *  Word wrapping helper routine.
     */
    public:
        nonvirtual void Clear ();
        nonvirtual void IncrementRowCountAndFixCacheBuffers (size_t newStart, Led_Distance newRowsHeight);

        // These should only be modifed in the FillCache () routine (or its overrides in subclasses)....
    public:
        // Note - calling these routines we assert i >= 0, <= fRowCountCache - to increase size of cache
        // call IncrementRowCountAndFixCacheBuffers ()
        nonvirtual Led_Distance PeekAtRowHeight (size_t ithRow) const;
        nonvirtual void         SetRowHeight (size_t i, Led_Distance rowHeight);

        nonvirtual size_t PeekAtRowStart (size_t i) const;
        nonvirtual void   SetRowStart (size_t i, size_t rowStart); // NB: rowStart[1] MUST BE ZERO!!!!

    private:
#if qAllowRowsThatAreLongerThan255
        using RowHeight_ = unsigned short;
#else
        using RowHeight_ = unsigned char;
#endif
        // Don't yet support packed RowStart_ ONLY because we keep array of starts, not nChars in row.
        // If we switch to that - then we can use unsigned char for this as with the rowHeight guy!!!
        // LGP 950519
        using RowStart_ = size_t;
        enum { kPackRowStartCount = sizeof (RowStart_*) / sizeof (RowStart_) };
        enum { kPackRowHeightCount = sizeof (RowHeight_*) / sizeof (RowHeight_) };

    private:
        struct Rep : public Foundation::Memory::UseBlockAllocationIfAppropriate<Rep> {
        public:
            Rep ();
            ~Rep ();

        public:
            Led_Distance fInterlineSpace;
            Led_Distance fPixelHeightCache;
            size_t       fRowCountCache;
            RowStart_*   fRowStartArray;
            RowHeight_*  fRowHeightArray;

        private:
            Rep (const Rep&);            //  intentionally not defined
            void operator= (const Rep&); //  ditto
        };

    private:
        shared_ptr<Rep> fRep;

    private:
        friend struct Rep;
        friend class shared_ptr<Rep>;
    };

    /*
    @CLASS:         MultiRowTextImager::PMInfoCacheMgr
    @BASES:         @'Partition::PartitionWatcher'
    @DESCRIPTION:
            <p></p>
    */
    class MultiRowTextImager::PMInfoCacheMgr : public Partition::PartitionWatcher {
    private:
        using inherited = Partition::PartitionWatcher;

    public:
        PMInfoCacheMgr (MultiRowTextImager& imager);
        ~PMInfoCacheMgr ();

    public:
        nonvirtual MultiRowTextImager::PartitionElementCacheInfo GetPartitionElementCacheInfo (Partition::PartitionMarker* pm) const;

    public:
        nonvirtual void ClearCache ();

    public:
        virtual void AboutToSplit (PartitionMarker* pm, size_t at, void** infoRecord) const noexcept override;
        virtual void DidSplit (void* infoRecord) const noexcept override;
        virtual void AboutToCoalece (PartitionMarker* pm, void** infoRecord) const noexcept override;
        virtual void DidCoalece (void* infoRecord) const noexcept override;

    private:
        nonvirtual void MyMarkerDidUpdateCallback ();

    private:
        class MyMarker;
        mutable map<PartitionMarker*, PartitionElementCacheInfo> fPMCache;
        mutable PartitionMarker*                                 fCurFillCachePM;
        mutable PartitionElementCacheInfo                        fCurFillCacheInfo;
        MultiRowTextImager&                                      fImager;
        unique_ptr<MyMarker>                                     fMyMarker;

    private:
        friend class MyMarker;
    };

    /*
    @CLASS:         MultiRowTextImager::PMInfoCacheMgr::MyMarker
    @CLASS:         Marker
    @ACCESS:        private
    @DESCRIPTION:
            <p></p>
    */
    class MultiRowTextImager::PMInfoCacheMgr::MyMarker : public Marker {
    private:
        using inherited = Marker;

    public:
        MyMarker (PMInfoCacheMgr& pmInfoCacheMgr);

    protected:
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    private:
        PMInfoCacheMgr& fPMInfoCacheMgr;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MultiRowTextImager.inl"

#endif /*_Stroika_Frameworks_Led_MultiRowTextImager_h_*/
