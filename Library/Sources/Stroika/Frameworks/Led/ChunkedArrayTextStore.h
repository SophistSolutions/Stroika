/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_ChunkedArrayTextStore_h_
#define _Stroika_Framework_Led_ChunkedArrayTextStore_h_ 1

#include "../StroikaPreComp.h"

/*
@MODULE:    ChunkedArrayTextStore
@DESCRIPTION:
        <p>@'ChunkedArrayTextStore' is a chunked-array based implementation of the
    TextStore class, together with a hierarchical marker storage representation.</p>
 */

#include <cstring>
#include <vector>

#include "../../Foundation/Cache/LRUCache.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"

#include "TextStore.h"

namespace Stroika::Frameworks::Led {

/*
    @CONFIGVAR:     qKeepChunkedArrayStatistics
    @DESCRIPTION:   <p>Slight debugging aid. Tells you how many existing markers are out there, and a few other statistics.
        This just keeps track of the given variables. To see them - you must peek in the debugger at the ChunkedArrayTextStore's
        instance variables.</p>
            <p>Turn ON iff @'qDebug' - by default.</p>
*/
#ifndef qKeepChunkedArrayStatistics
#define qKeepChunkedArrayStatistics qDebug
#endif

/*
 *  Reduce the amount of memory needed per Marker*, by limiting the number of Led_tChars
 *  in the buffer to 16Megs, and the number of different MarkerOwner*'s associated with
 *  a particular text-store to 256. Doing this can save (??? roughly) 8 bytes per
 *  Marker. (OH, and also can do similar trick with next/prev/parent poiinters. They
 *  all occur multiple times. Use table (array) in textstore obj, and store indexes
 *  into it here? maybe too slow on adding/removing markers? - oh well)
 *
 *  WARNING - NOT YET IMPLEMENTED - THIS IS STILL IGNORED, BUT TOTALLY LOCAL TO CHUNKEDDARRAYTEXTSTORE.CPP.
 */
#ifndef qSkrunchDataSizeByImposingLimits
#define qSkrunchDataSizeByImposingLimits 1
#endif

/*
    @CONFIGVAR:     qUseLRUCacheForRecentlyLookedUpMarkers
    @DESCRIPTION:   <p>Small speed tweek. Conditionally compiled cuz I'm not sure its always a speed tweek. In the
        case of a problem sent to me (SPR#0652) - this had a 20% speedup. And in other cases I tested (e.g. reading RTF 1.4 RTF doc) - no
        noticable difference. Maybe a 5% speedup on Cut/Paste/Paste operation after opening a file with 3X RTF 1.4 RTF.</p>
            <p>Turn ON by default</p>
        */
#ifndef qUseLRUCacheForRecentlyLookedUpMarkers
// a good idea but not compiling and at this stage of developemnt (2012-09-10 - getting quickly imported into Stroika) - ignore for now...)
#define qUseLRUCacheForRecentlyLookedUpMarkers 0
//#define qUseLRUCacheForRecentlyLookedUpMarkers  1
#endif

    /*
    @CLASS:         ChunkedArrayTextStore
    @BASES:         @'TextStore'
    @DESCRIPTION:
            <p>For most purposes, this is the most efficient @'TextStore' Led provides. It
        uses a chunked array implementation
        to keep track of the text, and a tree-like structure for keeping track of markers.</p>

            <p>The implementation is far more complex than that of @'SimpleTextStore'.
            so for understanding purposes, or as a starting point for a different sort of TextStore, this is probably <em>not</em>
            a good choice.</p>

            <p>A "chunked array" is a data structure that mixes an array with a linked
        list, to get most of the performance benefits of the two. It is a linked list of fixed-length array of Led_tChars (the chunks).
        Insertions don't have the usual problem with arrays, because you only have to shuffle bytes down up to a fixed maximum distance
        (to the end of the chunk). But lookups of a region of text typically are as quick as with arrays, since most operations you do
        on text are localized within a particular chunk.</p>

            <p>The representation of the markers however, is more novel, and clever. Basically,
        we use a tree-structure, where the organization of the tree naturally mimics the
        container/contains relationship among the markers. This allows for rapidly adjusting
        marker offsets during text updates, and rapidly finding all the markers in a particular
        subrange of the buffer.</p>
    */
    class ChunkedArrayTextStore : public TextStore {
    private:
        using inherited = TextStore;

    public:
        ChunkedArrayTextStore ();
        virtual ~ChunkedArrayTextStore ();

    public:
        virtual TextStore* ConstructNewTextStore () const override;

    public:
        virtual void AddMarkerOwner (MarkerOwner* owner) override;
        virtual void RemoveMarkerOwner (MarkerOwner* owner) override;

    public:
        virtual size_t GetLength () const noexcept override;
        virtual void   CopyOut (size_t from, size_t count, Led_tChar* buffer) const noexcept override;
        virtual void   ReplaceWithoutUpdate (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount) override;

    public:
        virtual void AddMarker (Marker* marker, size_t lhs, size_t length, MarkerOwner* owner) override;
        virtual void RemoveMarkers (Marker* const markerArray[], size_t markerCount) override;
        virtual void PreRemoveMarker (Marker* marker) override;
        virtual void SetMarkerRange (Marker* m, size_t start, size_t end) noexcept override;
        virtual void CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const override;

#if qUseLRUCacheForRecentlyLookedUpMarkers
    public:
        struct CollectLookupCacheElt;
#endif
    private:
        nonvirtual void CollectAllMarkersInRangeInto_Helper_MO (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output
#if qUseLRUCacheForRecentlyLookedUpMarkers
                                                                ,
                                                                CollectLookupCacheElt* fillingCache = NULL
#endif
                                                                ) const;

    private:
        class TextChunk;
        class ChunkAndOffset {
        public:
            ChunkAndOffset (size_t chunk, size_t offset);

            size_t fChunk;
            size_t fOffset;
        };

    private:
        nonvirtual void AddMarker1 (Marker* marker, Marker* insideMarker, bool canAddHackMarkers);
        // NB: PossiblyAddHackMarkers () cannot throw - if no memory for add, then just don't add hackmarkers
        nonvirtual void PossiblyAddHackMarkers (Marker* insideMarker);
        nonvirtual void RemoveMarker1 (Marker* marker);
        nonvirtual Marker* AddHackMarkerHelper_ (Marker* insideMarker, size_t start, size_t length);
        nonvirtual void    LoseIfUselessHackMarker (Marker* potentiallyUselessHackMarker);

    private:
        nonvirtual void InsertAfter_ (const Led_tChar* what, size_t howMany, size_t after);
        nonvirtual void DeleteAfter_ (size_t howMany, size_t after);
        nonvirtual TextChunk* AtomicAddChunk (size_t atArrayPos); // does 2 mem allocs, so make sure if second fails, first cleaned up!

    private:
        nonvirtual ChunkAndOffset FindChunkIndex (size_t charPos) const; // search if need be - but cache most recent...
        // This is inline checking the cache - underbar version
        // does out-of-line searching...
        // charPos refers to character AFTER that POS. And
        // it refers to where the actual character is. If it
        // is on the first byte of a new chunk - then it could
        // be we insert there, or at the END of the PREVIOUS
        // chunk
        nonvirtual ChunkAndOffset FindChunkIndex_ (size_t charPos) const; // Do the searching

        // chunk start cacing (to make FindChunkIndex() faster)
        // note - these are ALWAYS valid values - we simply reset them to the start of the chunked array if
        // anything before hand changes...
        // Note - this tecnique works best if we ask for FindChunkIndex() on roughly the same chunk each time.
        // If we really bounce all over the place - this optimization could be a slight any-tweek.
    private:
        mutable size_t fCachedChunkIndex;
        mutable size_t fCachedChunkIndexesOffset;

        nonvirtual void InvalCachedChunkIndexes (); // reset them to the beginning which is always safe...

    private:
        nonvirtual void AdjustMarkersForInsertAfter (size_t after, size_t howMany);
        nonvirtual void AdjustMarkersForInsertAfter1 (size_t after, size_t howMany, Marker* startAt);
        nonvirtual void AdjustMarkersForDeleteAfter (size_t after, size_t howMany);
        nonvirtual void AdjustMarkersForDeleteAfter1 (size_t after, size_t howMany, Marker* startAt);

    private:
        static bool AllHackMarkers (const Marker* m);
        static bool AllSubMarkersAreHackMarkerTrees (const Marker* m);

    private:
        size_t             fLength;
        vector<TextChunk*> fTextChunks;

#if qDebug
    protected:
        virtual void    Invariant_ () const override;
        nonvirtual void WalkSubTreeAndCheckInvariants (const Marker* m) const;
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ChunkedArrayTextStore.inl"

#endif /*_Stroika_Framework_Led_ChunkedArrayTextStore_h_*/
