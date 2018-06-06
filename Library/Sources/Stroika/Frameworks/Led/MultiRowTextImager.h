/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_MultiRowTextImager_h_
#define _Stroika_Frameworks_Led_MultiRowTextImager_h_ 1

#include "../../Foundation/StroikaPreComp.h"

/*
@MODULE:    MultiRowTextImager
@DESCRIPTION:
        <p>MultiRowTextImager is a @'TextImager' which supports having partition elements made up of multirow text.</p>

 */

#include <climits> // for UINT_MAX
#include <cstring>

#include "LineBasedPartition.h"
#include "PartitioningTextImager.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {

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
                struct Rep {
                public:
                    Rep ();
                    ~Rep ();

                public:
                    Led_Distance fInterlineSpace;
                    Led_Distance fPixelHeightCache;
                    size_t       fRowCountCache;
                    RowStart_*   fRowStartArray;
                    RowHeight_*  fRowHeightArray;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep);

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

            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */
            // defined out of order cuz used earlier
            inline MultiRowTextImager::RowReference::RowReference (PartitionMarker* partitionMarker, size_t subRow)
                : fPartitionMarker (partitionMarker)
                , fSubRow (subRow)
            {
            }
            inline void MultiRowTextImager::InvalidateTotalRowsInWindow ()
            {
                fTotalRowsInWindow = 0; // zero is sentinal meaning invalid
            }
            inline MultiRowTextImager::RowReference MultiRowTextImager::GetTopRowReferenceInWindow () const
            {
                RequireNotNull (PeekAtTextStore ()); //  Must associate textstore before we can ask for row-references
                EnsureNotNull (fTopLinePartitionMarkerInWindow);
                const_cast<MultiRowTextImager*> (this)->ReValidateSubRowInTopLineInWindow ();
                return (RowReference (fTopLinePartitionMarkerInWindow, fSubRowInTopLineInWindow));
            }
            inline size_t MultiRowTextImager::GetTotalRowsInWindow_ () const
            {
                if (fTotalRowsInWindow == 0) { // cached value invalid
                    fTotalRowsInWindow = ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ());
                }
                Assert (fTotalRowsInWindow >= 1); // always have at least one row...
                Assert (fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));
                return (fTotalRowsInWindow);
            }

            //  class   MultiRowTextImager::PartitionElementCacheInfo::Rep
            inline MultiRowTextImager::PartitionElementCacheInfo::Rep::Rep ()
                : fPixelHeightCache (Led_Distance (-1))
                , fInterlineSpace (0)
                , fRowCountCache (0)
                , fRowStartArray (nullptr)
                , fRowHeightArray (nullptr)
            {
            }
            inline MultiRowTextImager::PartitionElementCacheInfo::Rep::~Rep ()
            {
                if (fRowCountCache > kPackRowStartCount + 1) {
                    delete[] fRowStartArray;
                }
                if (fRowCountCache > kPackRowHeightCount) {
                    delete[] fRowHeightArray;
                }
            }

            //  class   MultiRowTextImager::PartitionElementCacheInfo
            inline MultiRowTextImager::PartitionElementCacheInfo::PartitionElementCacheInfo ()
                : fRep (make_shared<Rep> ())
            {
            }
            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::GetInterLineSpace
            @DESCRIPTION:   <p>Get the interline space associated with a partition. Each partition can
                have a unique interline space. These are computed by overriding the
                MultiRowTextImager::CalculateInterLineSpace () method. They are not set directly.
                </p>
            */
            inline Led_Distance MultiRowTextImager::PartitionElementCacheInfo::GetInterLineSpace () const
            {
                Assert (fRep->fInterlineSpace != Led_Distance (-1));
                return (fRep->fInterlineSpace);
            }
            inline void MultiRowTextImager::PartitionElementCacheInfo::SetInterLineSpace (Led_Distance interlineSpace)
            {
                Assert (interlineSpace != Led_Distance (-1));
                fRep->fInterlineSpace = interlineSpace;
            }

            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::GetPixelHeight
            @DESCRIPTION:   <p>Return the cached height of the given partition element. This is the sum of the
                pixel height computed in FillCache () for the actual text, and the interline space (see GetInterLineSpace ()).</p>
            */
            inline Led_Distance MultiRowTextImager::PartitionElementCacheInfo::GetPixelHeight () const
            {
                return (fRep->fPixelHeightCache + GetInterLineSpace ());
            }
            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::GetRowCount
            @DESCRIPTION:   <p>Return the cached number of rows in this partition element. This is computed in
                MultiRowTextImager::FillCache (), and specified indirectly via calls (from inside FillCache)
                to MultiRowTextImager::MultiRowPartitionMarker::IncrementRowCountAndFixCacheBuffers ().</p>
            */
            inline size_t MultiRowTextImager::PartitionElementCacheInfo::GetRowCount () const
            {
                Assert (fRep->fRowCountCache >= 1); // even for empty lines we have 1 row (by definition)
                return (fRep->fRowCountCache);
            }
            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::PeekRowCount
            @DESCRIPTION:   <p>Return the cached number of rows in this partition element. This is computed in
                MultiRowTextImager::FillCache (), and specified indirectly via calls (from inside FillCache)
                to MultiRowTextImager::MultiRowPartitionMarker::IncrementRowCountAndFixCacheBuffers ().</p>
            */
            inline size_t MultiRowTextImager::PartitionElementCacheInfo::PeekRowCount () const
            {
                return (fRep->fRowCountCache);
            }
            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::GetLastRow
            @DESCRIPTION:   <p>Return the last valid row index (may invoke FillCache if cached result not
                already available).</p>
            */
            inline size_t MultiRowTextImager::PartitionElementCacheInfo::GetLastRow () const
            {
                return GetRowCount () - 1;
            }
            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowHeight
            @DESCRIPTION:   <p>Return the height - (in whatever unit the GDI is using, but typically pixels) of the given row.
                The internal representation of these things is somewhat obscure for data size reasons, so there is some
                unpacking to be done.</p>
            */
            inline Led_Distance MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowHeight (size_t i) const
            {
                Assert (i < fRep->fRowCountCache); // MFC Hint - when this assert fails, look closely at your
                // stack-trace - often its cuz some other assert failed in the context
                // of a FillCache, and so the cache info isn't completely filled in
                // yet...
                /*
                 *  A bit of trickery. --- XPLAIN
                 */
                if (fRep->fRowCountCache <= kPackRowHeightCount) {
                    // Then we use the pointer to the array as the actual array
                    const RowHeight_* theArray = reinterpret_cast<const RowHeight_*> (&fRep->fRowHeightArray);
                    return (theArray[i]);
                }
                AssertNotNull (fRep->fRowHeightArray);
                return (fRep->fRowHeightArray[i]);
            }
            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::SetRowHeight
            @DESCRIPTION:   <p>Set the height of a given row. This is typically just done within FillCache().</p>
            */
            inline void MultiRowTextImager::PartitionElementCacheInfo::SetRowHeight (size_t i, Led_Distance rowHeight)
            {
                Assert (i < fRep->fRowCountCache);
                Assert (sizeof (RowHeight_) > 1 or rowHeight <= 0xff);   // be sure value fits..
                Assert (sizeof (RowHeight_) > 2 or rowHeight <= 0xffff); // be sure value fits.
                /*
                 *  A bit of trickery. --- XPLAIN
                 */
                if (fRep->fRowCountCache <= kPackRowHeightCount) {
                    // Then we use the pointer to the array as the actual array
                    RowHeight_* theArray = reinterpret_cast<RowHeight_*> (&fRep->fRowHeightArray);
                    theArray[i]          = RowHeight_ (rowHeight);
                }
                else {
                    AssertNotNull (fRep->fRowHeightArray);
                    fRep->fRowHeightArray[i] = RowHeight_ (rowHeight);
                }
            }
            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowStart
            @DESCRIPTION:   <p>Return the partition element relative offset of the start of a given row. So for the
                first row, this is always zero.</p>
            */
            inline size_t MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowStart (size_t i) const
            {
                Assert (i < fRep->fRowCountCache);

                if (i == 0) {
                    return (0);
                }
                else {
                    /*
                     *  A bit of trickery. --- XPLAIN
                     */
                    Assert (i >= 1);
                    if (fRep->fRowCountCache <= kPackRowStartCount + 1) {
                        // Then we use the pointer to the array as the actual array
                        const RowStart_* theArray = reinterpret_cast<const RowStart_*> (&fRep->fRowStartArray);
                        return (theArray[i - 1]);
                    }
                    AssertNotNull (fRep->fRowStartArray);
                    return (fRep->fRowStartArray[i - 1]);
                }
            }
            /*
            @METHOD:        MultiRowTextImager::PartitionElementCacheInfo::SetRowStart
            @DESCRIPTION:   <p>Set the partition element relative offset of the start of a given row. So for the
                first row, this is MUST BE zero. This is typically just called during FillCache ().</p>
            */
            inline void MultiRowTextImager::PartitionElementCacheInfo::SetRowStart (size_t i, size_t rowStart)
            {
                Assert (i < fRep->fRowCountCache);

                if (i == 0) {
                    Assert (rowStart == 0);
                }
                else {
                    /*
                     *  A bit of trickery. --- XPLAIN
                     */
                    Assert (i >= 1);
                    Assert (sizeof (RowStart_) > 1 or rowStart <= 0xff);   // be sure value fits..
                    Assert (sizeof (RowStart_) > 2 or rowStart <= 0xffff); // be sure value fits.
                    if (fRep->fRowCountCache <= kPackRowStartCount + 1) {
                        // Then we use the pointer to the array as the actual array
                        RowStart_* theArray = reinterpret_cast<RowStart_*> (&fRep->fRowStartArray);
                        theArray[i - 1]     = RowStart_ (rowStart);
                    }
                    else {
                        AssertNotNull (fRep->fRowStartArray);
                        fRep->fRowStartArray[i - 1] = RowStart_ (rowStart);
                    }
                }
            }
            inline size_t MultiRowTextImager::PartitionElementCacheInfo::GetLineRelativeRowStartPosition (size_t ithRow) const
            {
                return (PeekAtRowStart (ithRow));
            }
            inline Led_Distance MultiRowTextImager::PartitionElementCacheInfo::GetRowHeight (size_t ithRow) const
            {
                return (PeekAtRowHeight (ithRow));
            }
            inline size_t MultiRowTextImager::PartitionElementCacheInfo::LineRelativePositionInWhichRow (size_t charPos) const
            {
                //  ZERO based charPos - ie zero is just before first byte in first row
                //  Require (charPos >= 0); // yes I know this is a degenerate test - just for doc purposes...
                //  Assert (charPos < OURLENGTH);
                for (size_t row = fRep->fRowCountCache; row >= 1; row--) {
                    if (charPos >= PeekAtRowStart (row - 1)) {
                        return (row - 1);
                    }
                }
                Assert (false);
                return (0); // if we get here - must have been before our line...
            }

            //  class   MultiRowTextImager::RowReference
            inline MultiRowTextImager::RowReference::RowReference (const RowReference& from)
                : fPartitionMarker (from.fPartitionMarker)
                , fSubRow (from.fSubRow)
            {
            }
            inline MultiRowTextImager::RowReference& MultiRowTextImager::RowReference::operator= (const MultiRowTextImager::RowReference& rhs)
            {
                fPartitionMarker = rhs.fPartitionMarker;
                fSubRow          = rhs.fSubRow;
                return (*this);
            }
            inline MultiRowTextImager::PartitionMarker* MultiRowTextImager::RowReference::GetPartitionMarker () const
            {
                return (fPartitionMarker);
            }
            inline size_t MultiRowTextImager::RowReference::GetSubRow () const
            {
                return (fSubRow);
            }

            //  class   MultiRowTextImager
            /*
            @METHOD:        MultiRowTextImager::GetNextRowReference
            @DESCRIPTION:   <p>Advance the given row reference argument to the next row. Return true if there
                is a valid next row. And false if <code>adjustMeInPlace</code> was already on the last row.</p>
                    <p>See also @'MultiRowTextImager::GetPreviousRowReference'.</p>
            */
            inline bool MultiRowTextImager::GetNextRowReference (RowReference* adjustMeInPlace) const
            {
                RequireNotNull (adjustMeInPlace);
                PartitionMarker*          cur         = adjustMeInPlace->GetPartitionMarker ();
                size_t                    subRow      = adjustMeInPlace->GetSubRow ();
                PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (cur);
                if (subRow + 1 < pmCacheInfo.GetRowCount ()) {
                    subRow++;
                    *adjustMeInPlace = RowReference (cur, subRow);
                    return true;
                }
                else {
                    if (cur->GetNext () == nullptr) {
                        return false;
                    }
                    else {
                        cur              = cur->GetNext ();
                        subRow           = 0;
                        *adjustMeInPlace = RowReference (cur, subRow);
                        return true;
                    }
                }
            }
            /*
            @METHOD:        MultiRowTextImager::GetPreviousRowReference
            @DESCRIPTION:   <p>Retreat the given row reference argument to the previous row. Return true if there
                is a valid previous row. And false if <code>adjustMeInPlace</code> was already on the first row.</p>
                    <p>See also @'MultiRowTextImager::GetNextRowReference'.</p>
            */
            inline bool MultiRowTextImager::GetPreviousRowReference (RowReference* adjustMeInPlace) const
            {
                AssertNotNull (adjustMeInPlace);
                PartitionMarker* cur    = adjustMeInPlace->GetPartitionMarker ();
                size_t           subRow = adjustMeInPlace->GetSubRow ();
                if (subRow > 0) {
                    subRow--;
                    *adjustMeInPlace = RowReference (cur, subRow);
                    return true;
                }
                else {
                    if (cur->GetPrevious () == nullptr) {
                        return false;
                    }
                    else {
                        cur                                   = cur->GetPrevious ();
                        PartitionElementCacheInfo pmCacheInfo = GetPartitionElementCacheInfo (cur);
                        subRow                                = pmCacheInfo.GetRowCount () - 1;
                        *adjustMeInPlace                      = RowReference (cur, subRow);
                        return true;
                    }
                }
            }
            /*
            @METHOD:        MultiRowTextImager::GetIthRowReferenceFromHere
            @DESCRIPTION:   <p>Adjust <code>fromHere</code> by <code>ith</code> rows. If <code>ith</code> is zero, then
                this returns <code>fromHere</code>. It asserts there are at least ith rows from the given one to retrieve.
                It calls @'MultiRowTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
                    <p>See also @'MultiRowTextImager::GetNextRowReference', @'MultiRowTextImager::GetPreviousRowReference'.</p>
            */
            inline MultiRowTextImager::RowReference MultiRowTextImager::GetIthRowReferenceFromHere (RowReference fromHere, ptrdiff_t ith) const
            {
                [[maybe_unused]] bool result = GetIthRowReferenceFromHere (&fromHere, ith);
                Assert (result);
                return fromHere;
            }
            /*
            @METHOD:        MultiRowTextImager::GetIthRowReference
            @DESCRIPTION:   <p>Get the <code>ith</code> row reference in the document. Asserts value <code>ith</code> refers to
                a valid row number.</p>
                    <p>It calls @'MultiRowTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
            */
            inline MultiRowTextImager::RowReference MultiRowTextImager::GetIthRowReference (size_t ith) const
            {
                RowReference fromHere (GetFirstPartitionMarker (), 0);
                bool         result = GetIthRowReferenceFromHere (&fromHere, ith);
                Assert (result);
                return (fromHere);
            }
            /*
            @METHOD:        MultiRowTextImager::GetRowLength
            @DESCRIPTION:   <p>Gets the length of the given row (in @'Led_tChar's).</p>
                    <p>See also  @'MultiRowTextImager::GetStartOfRow' and @'MultiRowTextImager::GetEndOfRow'.</p>
            */
            inline size_t MultiRowTextImager::GetRowLength (RowReference row) const
            {
                return (GetEndOfRow (row) - GetStartOfRow (row));
            }
            /*
            @METHOD:        MultiRowTextImager::GetLastRowReferenceInWindow
            @DESCRIPTION:   <p>Returns the last row-reference in the window (end of window).</p>
            */
            inline MultiRowTextImager::RowReference MultiRowTextImager::GetLastRowReferenceInWindow () const
            {
                RowReference row = GetTopRowReferenceInWindow ();
                Assert (GetTotalRowsInWindow_ () >= 1);
                (void)GetIthRowReferenceFromHere (&row, GetTotalRowsInWindow_ () - 1);
                return (row);
            }
            inline void MultiRowTextImager::SetTopRowInWindow_ (RowReference row)
            {
                fTopLinePartitionMarkerInWindow = row.GetPartitionMarker ();
                fSubRowInTopLineInWindow        = row.GetSubRow ();
                AssertNotNull (fTopLinePartitionMarkerInWindow);
                InvalidateTotalRowsInWindow ();
            }
            /*
            @METHOD:        MultiRowTextImager::GetRowHeight
            @DESCRIPTION:   <p>Returns the height (in standard GDI units, usually pixels) of the given row reference.</p>
            */
            inline Led_Distance MultiRowTextImager::GetRowHeight (RowReference row) const
            {
                AssertNotNull (row.GetPartitionMarker ());
                return GetPartitionElementCacheInfo (row.GetPartitionMarker ()).GetRowHeight (row.GetSubRow ());
            }

            inline bool operator== (MultiRowTextImager::RowReference lhs, MultiRowTextImager::RowReference rhs)
            {
                return (lhs.GetPartitionMarker () == rhs.GetPartitionMarker () and
                        lhs.GetSubRow () == rhs.GetSubRow ());
            }
            inline bool operator!= (MultiRowTextImager::RowReference lhs, MultiRowTextImager::RowReference rhs)
            {
                return (lhs.GetPartitionMarker () != rhs.GetPartitionMarker () or
                        lhs.GetSubRow () != rhs.GetSubRow ());
            }
        }
    }
}

#endif /*_Stroika_Frameworks_Led_MultiRowTextImager_h_*/
