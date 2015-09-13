/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ChunkedArrayTextStore_h__
#define	__ChunkedArrayTextStore_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/ChunkedArrayTextStore.h,v 2.39 2003/03/04 21:30:26 lewis Exp $
 */


/*
@MODULE:	ChunkedArrayTextStore
@DESCRIPTION:
		<p>@'ChunkedArrayTextStore' is a chunked-array based implementation of the
	TextStore class, together with a hierarchical marker storage representation.</p>
 */


/*
 * Changes:
 *	$Log: ChunkedArrayTextStore.h,v $
 *	Revision 2.39  2003/03/04 21:30:26  lewis
 *	SPR#1315 - In ChunkedArrayTextStore - use new LRUCache<ELEMENT> to keep track of CollectAllMarkersEtc... cache instead of private LRU cache implementation.
 *	
 *	Revision 2.38  2002/11/17 15:55:30  lewis
 *	SPR#1173- Replace/ReplaceWithoutUpdate change
 *	
 *	Revision 2.37  2002/09/13 15:29:18  lewis
 *	Add TextStore::ConstructNewTextStore () support
 *	
 *	Revision 2.36  2002/05/06 21:33:23  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.35  2001/11/27 00:29:37  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.34  2001/10/19 20:47:12  lewis
 *	DocComments
 *	
 *	Revision 2.33  2001/10/17 20:42:47  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.32  2001/08/29 23:00:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.31  2001/08/28 18:43:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.30  2000/08/13 03:30:14  lewis
 *	SPR#0822- added TextStore::PreRemoveMarker
 *	
 *	Revision 2.29  2000/07/26 05:46:35  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner
 *	arg to CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table of speedups.
 *	
 *	Revision 2.28  2000/04/15 14:32:33  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.27  2000/04/14 22:40:19  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.26  1999/12/11 18:33:38  lewis
 *	qUseLRUCacheForRecentlyLookedUpMarkers support as part of SPR#0652
 *	
 *	Revision 2.25  1999/11/13 16:32:16  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.24  1999/06/28 14:57:24  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.23  1999/05/03 22:04:49  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.22  1998/10/30 14:10:16  lewis
 *	eliminate some obsolete BWAs - like nested class / vector stuff
 *	
 *	Revision 2.21  1998/07/24  00:54:49  lewis
 *	cleanup some names that were 'byte' and should have been 'tchar'.
 *
 *	Revision 2.20  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 2.19  1998/04/25  01:16:31  lewis
 *	Fix memcpy/memmoves to take into account that Led_tChar could be bigger than a char (UNICODE support).
 *	And docs
 *
 *	Revision 2.18  1998/03/04  20:18:28  lewis
 *	*** empty log message ***
 *
 *	Revision 2.17  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.16  1997/09/29  14:27:31  lewis
 *	Lose alot of 21Compat stuff.
 *	Lose qLedFirstIndex support.
 *	Use vector<> instead of Led_Array.
 *	more.
 *
 *	Revision 2.15  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.14  1997/07/27  14:59:48  lewis
 *	edit docs
 *
 *	Revision 2.13  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.12  1997/07/12  19:55:03  lewis
 *	TextStore_ renamed -> TextStore.
 *	qLedFirstIndex fixed.
 *	AutoDoc.
 *
 *	Revision 2.11  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/06/18  02:41:59  lewis
 *	react to changes in TextStore_ CollectAllMarkersInRangeInto API.
 *
 *	Revision 2.9  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.8  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.6  1996/08/05  05:59:14  lewis
 *	Change param names to fit new programmer docs.
 *
 *	Revision 2.5  1996/05/23  19:15:31  lewis
 *	Mostly just added throw specifiers, but also deleted obsolete GetMarkerRange
 *	routine
 *
 *	Revision 2.4  1996/03/16  18:31:52  lewis
 *	Invariant/WalkSubTreeAndCheckInvariants now const.
 *
 *	Revision 2.3  1996/02/26  18:30:54  lewis
 *	No longer need to override RemoveMarker.
 *
 *	Revision 2.2  1995/10/19  22:00:27  lewis
 *	Make CollectAllMarkersInRangeIntoAndBelow private.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.14  1995/05/29  23:44:47  lewis
 *	Changed emacs-mode string to be c++ from Text
 *
 *	Revision 1.13  1995/05/29  23:34:35  lewis
 *	Lots of changes
 *	Got rid of RootMarker class since now Marker concrete class.
 *	Added AtomicAddChunk () utility to fix exception handling problems.
 *	More work to the marker code, speedups/cleanups, SPR#0296,0292,0293,
 *	0294,0290.
 *
 *	Revision 1.12  1995/05/26  06:40:44  lewis
 *	AddMarker1 now takes canAddHackMarker flag.
 *	RemoveMarker1 no longer needs insideMarker arg (cuz we can use parent
 *	field) and add new utility method LoseIfUselessHackMarker().
 *
 *	Revision 1.11  1995/05/20  04:40:38  lewis
 *	Fixed misspelling of invarient
 *
 *	Revision 1.10  1995/05/18  08:03:56  lewis
 *	Lose old bug woCW5.* bug workarounds (SPR#0249).
 *
 *	Revision 1.9  1995/05/06  19:20:29  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *
 *	Revision 1.8  1995/05/05  19:39:02  lewis
 *	Use Led_tChar typedef to facilitate later UNICODE support.
 *
 *	Revision 1.7  1995/04/18  00:41:38  lewis
 *	Cleanups
 *
 *	Revision 1.6  1995/04/18  00:06:17  lewis
 *	Fix constness of args to removeMarkers.
 *
 *	Revision 1.5  1995/04/16  18:59:37  lewis
 *	Added RemoveMarkers () override (much faster to remove a bunch at
 *	once than one at a time (SPR#0207). Worked on speeding up
 *	hackmarker stuff quite a bit.
 *
 *	Revision 1.4  1995/03/29  21:01:40  lewis
 *	Got rid of unused Split() method (SPR#0187).
 *
 *	Revision 1.3  1995/03/13  03:14:27  lewis
 *	Renamed Array<T> to Led_Array<T>
 *
 *	Revision 1.2  1995/03/02  05:45:35  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:22  lewis
 *	Initial revision
 *
 *
 *
 */

#include	<cstring>
#include	<vector>

#include	"TextStore.h"


#if		qLedUsesNamespaces
namespace	Led {
#endif



/*
@CONFIGVAR:		qKeepChunkedArrayStatistics
@DESCRIPTION:	<p>Slight debugging aid. Tells you how many existing markers are out there, and a few other statistics.
	This just keeps track of the given variables. To see them - you must peek in the debugger at the ChunkedArrayTextStore's
	instance variables.</p>
		<p>Turn ON iff @'qDebug' - by default.</p>
 */
#ifndef	qKeepChunkedArrayStatistics
	#define	qKeepChunkedArrayStatistics	qDebug
#endif

 
 
 
 
/*
 *	Reduce the amount of memory needed per Marker*, by limiting the number of Led_tChars
 *	in the buffer to 16Megs, and the number of differnt MarkerOwner*'s associated with
 *	a particular text-store to 256. Doing this can save (??? roughly) 8 bytes per
 *	Marker. (OH, and also can do similar trick with next/prev/parent poiinters. They
 *	all occur multiple times. Use table (array) in textstore obj, and store indexes
 *	into it here? maybe too slow on adding/removing markers? - oh well)
 *
 *	WARNING - NOT YET IMPLEMENTED - THIS IS STILL IGNORED, BUT TOTALLY LOCAL TO CHUNKEDDARRAYTEXTSTORE.CPP.
 */
#ifndef	qSkrunchDataSizeByImposingLimits
	#define	qSkrunchDataSizeByImposingLimits	1
#endif

 
 
 
 
/*
@CONFIGVAR:		qUseLRUCacheForRecentlyLookedUpMarkers
@DESCRIPTION:	<p>Small speed tweek. Conditionally compiled cuz I'm not sure its always a speed tweek. In the
	case of a problem sent to me (SPR#0652) - this had a 20% speedup. And in other cases I tested (e.g. reading RTF 1.4 RTF doc) - no
	noticable difference. Maybe a 5% speedup on Cut/Paste/Paste operation after opening a file with 3X RTF 1.4 RTF.</p>
		<p>Turn ON by default</p>
 */
#ifndef	qUseLRUCacheForRecentlyLookedUpMarkers
#define	qUseLRUCacheForRecentlyLookedUpMarkers	1
#endif








/*
@CLASS:			ChunkedArrayTextStore
@BASES:			@'TextStore'
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
class	ChunkedArrayTextStore : public TextStore {
	private:
		typedef	TextStore	inherited;

	public:
		ChunkedArrayTextStore ();
		virtual ~ChunkedArrayTextStore ();

	public:
		override	TextStore*	ConstructNewTextStore () const;

	public:
		override	void	AddMarkerOwner (MarkerOwner* owner);
		override	void	RemoveMarkerOwner (MarkerOwner* owner);

	public:
		override	size_t	GetLength () const throw ();
		override	void	CopyOut (size_t from, size_t count, Led_tChar* buffer) const throw ();
		override	void	ReplaceWithoutUpdate (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount);

	public:
		override	void	AddMarker (Marker* marker, size_t lhs, size_t length, MarkerOwner* owner);
		override	void	RemoveMarkers (Marker*const markerArray[], size_t markerCount);
		override	void	PreRemoveMarker (Marker* marker);
		override	void	SetMarkerRange (Marker* m, size_t start, size_t end) throw ();
		override	void	CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const;

	#if		qUseLRUCacheForRecentlyLookedUpMarkers
	public:
		struct	CollectLookupCacheElt;
	#endif
	private:
		nonvirtual	void	CollectAllMarkersInRangeInto_Helper_MO (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output
										#if		qUseLRUCacheForRecentlyLookedUpMarkers
											, CollectLookupCacheElt* fillingCache = NULL
										#endif
									) const;

	private:
		class	TextChunk;
		class	ChunkAndOffset {
			public:
				ChunkAndOffset (size_t chunk, size_t offset);

				size_t	fChunk;
				size_t	fOffset;
		};

	private:
		nonvirtual	void	AddMarker1 (Marker* marker, Marker* insideMarker, bool canAddHackMarkers);
		// NB: PossiblyAddHackMarkers () cannot throw - if no memory for add, then just don't add hackmarkers
		nonvirtual	void	PossiblyAddHackMarkers (Marker* insideMarker);
		nonvirtual	void	RemoveMarker1 (Marker* marker);
		nonvirtual	Marker*	AddHackMarkerHelper_ (Marker* insideMarker, size_t start, size_t length);
		nonvirtual	void	LoseIfUselessHackMarker (Marker* potentiallyUselessHackMarker);

	private:
		nonvirtual	void		InsertAfter_ (const Led_tChar* what, size_t howMany, size_t after);
		nonvirtual	void		DeleteAfter_ (size_t howMany, size_t after);
		nonvirtual	TextChunk*	AtomicAddChunk (size_t atArrayPos);		// does 2 mem allocs, so make sure if second fails, first cleaned up!

	private:
		nonvirtual	ChunkAndOffset	FindChunkIndex (size_t charPos) const;		// search if need be - but cache most recent...
																				// This is inline checking the cache - underbar version
																				// does out-of-line searching...
																				// charPos refers to character AFTER that POS. And
																				// it refers to where the actual character is. If it
																				// is on the first byte of a new chunk - then it could
																				// be we insert there, or at the END of the PREVIOUS
																				// chunk
		nonvirtual	ChunkAndOffset	FindChunkIndex_ (size_t charPos) const;		// Do the searching


	// chunk start cacing (to make FindChunkIndex() faster)
	// note - these are ALWAYS valid values - we simply reset them to the start of the chunked array if
	// anything before hand changes...
	// Note - this tecnique works best if we ask for FindChunkIndex() on roughly the same chunk each time.
	// If we really bounce all over the place - this optimization could be a slight any-tweek.
	private:
		mutable	size_t	fCachedChunkIndex;
		mutable	size_t	fCachedChunkIndexesOffset;

		nonvirtual	void	InvalCachedChunkIndexes ();		// reset them to the beginning which is always safe...

	private:
		nonvirtual	void	AdjustMarkersForInsertAfter (size_t after, size_t howMany);
		nonvirtual	void	AdjustMarkersForInsertAfter1 (size_t after, size_t howMany, Marker* startAt);
		nonvirtual	void	AdjustMarkersForDeleteAfter (size_t after, size_t howMany);
		nonvirtual	void	AdjustMarkersForDeleteAfter1 (size_t after, size_t howMany, Marker* startAt);

	private:
		static		bool	AllHackMarkers (const Marker* m);
		static		bool	AllSubMarkersAreHackMarkerTrees (const Marker* m);



	private:
		size_t				fLength;
		vector<TextChunk*>	fTextChunks;


#if		qDebug
	protected:
		override	void	Invariant_ () const;
		nonvirtual	void	WalkSubTreeAndCheckInvariants (const Marker* m) const;
#endif
};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	ChunkedArrayTextStore::ChunkAndOffset
	inline	ChunkedArrayTextStore::ChunkAndOffset::ChunkAndOffset (size_t chunk, size_t offset):
		fChunk (chunk),
		fOffset (offset)
		{
		}







//	class	ChunkedArrayTextStore
	inline	size_t	ChunkedArrayTextStore::GetLength () const throw ()
		{
			return (fLength);
		}
	inline	ChunkedArrayTextStore::ChunkAndOffset	ChunkedArrayTextStore::FindChunkIndex (size_t charPos) const
		{
			// No cache implemented yet - later check cache - and only call _ version if need be...
			return (FindChunkIndex_ (charPos));
		}
	inline	void	ChunkedArrayTextStore::InvalCachedChunkIndexes ()
		{
			fCachedChunkIndex = 0;
			fCachedChunkIndexesOffset = 0;
		}



#if		qLedUsesNamespaces
}
#endif



#endif	/*__ChunkedArrayTextStore_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

