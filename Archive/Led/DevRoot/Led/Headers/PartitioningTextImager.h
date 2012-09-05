/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__PartitioningTextImager_h__
#define	__PartitioningTextImager_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/PartitioningTextImager.h,v 1.55 2003/11/27 20:39:04 lewis Exp $
 */


/*
@MODULE:	PartitioningTextImager
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


/*
 * Changes:
 *	$Log: PartitioningTextImager.h,v $
 *	Revision 1.55  2003/11/27 20:39:04  lewis
 *	doccomments, code cleanups, and make PartitioningTextImager::MeasureTextCache : private Partition::PartitionWatcher
 *	
 *	Revision 1.54  2003/11/26 16:19:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.53  2003/11/03 20:05:37  lewis
 *	add new TextImager::GetTextLayoutBlock () method - which in subclasses - can take into account subclasses to use an intiail dir from first row in subsequent rows. Related - added PartitioningTextImager::GetPrimaryPartitionTextDirection ()
 *	
 *	Revision 1.52  2003/10/26 22:24:21  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.51  2003/03/11 19:33:50  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.50  2003/03/04 21:32:50  lewis
 *	SPR#1315 - redo the qCacheTextMeasurementsForPM cache using the new LRUCache<> template. Dynamically set cache size (based on TextStore size)
 *	
 *	Revision 1.49  2003/02/13 21:51:14  lewis
 *	SPR#1134- Cleanup/improve qCacheTextMeasurementsForPM/MeasureTextCache code. Now keep
 *	a primitive multi-element cache. This cache is crummy - and could be substantially improved,
 *	but wait til I profile to see how important that is
 *	
 *	Revision 1.48  2002/12/21 03:00:00  lewis
 *	SPR#1217. Added virtual TextImager::InvalidateAllCaches and made SimpleTextImager/MRTI versions
 *	just overrides. No longer need SimpleTextImager::TabletChangedMetrics overrides etc
 *	since now TextImager::TabletChangedMetrics calls TextImager::InvalidateAllCaches.
 *	MultiRowTextImager::PurgeUnneededMemory () no longer needed since TextImager version
 *	now calls InvalidateAllCaches().
 *	
 *	Revision 1.47  2002/12/19 13:28:28  lewis
 *	SPR#1212- fix qCacheTextMeasurementsForPM / PartitioningTextImager::CalcSegmentSize code.
 *	Need to measure whole row!
 *	
 *	Revision 1.46  2002/12/13 18:41:15  lewis
 *	SPR#1204- added TextImager::GetTextDirection () API support
 *	
 *	Revision 1.45  2002/12/06 15:15:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.44  2002/12/06 15:13:01  lewis
 *	SPR#1198 - added 2 new APIs to TextImager, implemented them in PartitioningTextImager,
 *	and used  them in MultiRowTextImager and SimpleTextImager:
 *		GetRowRelativeCharLoc ()
 *		GetRowRelativeCharAtLoc ().
 *	
 *	Revision 1.43  2002/12/04 22:27:56  lewis
 *	more cleanups of the new CalcSegmentSize code: delete MeasureTextCache::AboutToUpdateText checker.
 *	Fix DidUpdate checker to only inval if that area changed. Delete CalcSegmentSizeWithImmediateText.
 *	Doccomments.
 *	
 *	Revision 1.42  2002/12/04 16:45:02  lewis
 *	turn qCacheTextMeasurementsForPM on (was accidentally left off so I never really tested 1193 change - but now OK)
 *	
 *	Revision 1.41  2002/12/04 15:45:44  lewis
 *	SPR#1193 - added PartitioningTextImager::MeasureTextCache to implement qCacheTextMeasurementsForPM
 *	and use CalcSegmentSize instead of CalcSegmentSizeWithImmediateText
 *	
 *	Revision 1.40  2002/10/14 23:23:40  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.39  2002/10/08 16:37:55  lewis
 *	SPR#1119- Added CheckForSplits/NeedToCoalesce to LineBasedPartition. Add WordProcessor::WPPartition.
 *	Add / use MakeDefaultPartition () so we create the new partition for WPs.
 *	
 *	Revision 1.38  2002/05/06 21:33:32  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.37  2001/11/27 00:29:44  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.36  2001/10/17 20:42:52  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 1.35  2001/09/29 16:23:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.34  2001/09/29 16:21:56  lewis
 *	SPR#1052- added fPartitionMarkerLast field and use that in
 *	Partition::GetPartitionMarkerContainingPosition as a speed tweek
 *
 *	Revision 1.33  2001/08/29 23:00:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.32  2001/08/28 18:43:28  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.31  2001/04/26 16:39:28  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 1.30  2000/08/13 05:21:45  lewis
 *	SPR#0824- LED_DECLARE_USE_BLOCK_ALLOCATION instead of DECLARE_USE_BLOCK_ALLOCATION
 *	
 *	Revision 1.29  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 1.28  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 1.27  2000/03/31 16:59:13  lewis
 *	SPR#0721- use DECLARE_USE_BLOCK_ALLOCATION macro
 *	
 *	Revision 1.26  1999/12/14 18:03:24  lewis
 *	Only call invariant if qHeavyDebugging
 *	
 *	Revision 1.25  1999/11/13 22:33:01  lewis
 *	lose qSupportLed22CompatAPI support
 *	
 *	Revision 1.24  1999/11/13 16:32:18  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 1.23  1999/07/21 14:37:09  lewis
 *	make fields I had made private, now public again, if qSupportLed22CompatAPI
 *	
 *	Revision 1.22  1999/07/13 22:39:12  lewis
 *	Support CWPro5, and lose support for CWPro2,CWPro3 - and the various
 *	BugWorkArounds that were only needed for these old compilers
 *	
 *	Revision 1.21  1999/06/28 15:05:25  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 1.20  1999/05/03 22:04:57  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 1.19  1999/05/03 21:41:24  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 1.18  1999/02/12 23:30:56  lewis
 *	make Partition::GetPartitionMarkerContainingPosition virtual, at REMO's request
 *	
 *	Revision 1.17  1998/07/24 01:00:32  lewis
 *	Revised ResetTabStops() API (INCOMPATABLY).
 *	
 *	Revision 1.16  1998/06/03  01:30:43  lewis
 *	make PartitioningTextImager::SetPartition virtual.
 *
 *	Revision 1.15  1998/05/05  00:29:36  lewis
 *	support MWERKSCWPro3 - and lose old bug workarounds.
 *
 *	Revision 1.14  1998/04/25  01:22:21  lewis
 *	GetPrev/Next() methods of ParitionMarker now virtual, so can more easily implement 'delayed partitioning' or
 *	'on demand paritioning' Partition subclass (Remo).
 *
 *	Revision 1.13  1998/04/09  01:28:00  lewis
 *	Use new MarkerMortuary<> template instead of manual fMarkersToBeDeleted array implementation
 *
 *	Revision 1.12  1998/04/08  01:34:40  lewis
 *	Major new design. Added new Partition class. Moved PartitionMarker class to be under that instead.
 *	And PartitioningTextImager now much simpler, and just uses a Partition object.
 *	Can associate a single partition shared among multiple imagers now (as requested by REMO).
 *	And more importantly - can specify the details of the partition much more easily separated
 *	from the rest of the editing details.
 *
 *	Revision 1.11  1998/03/04  20:17:37  lewis
 *	*** empty log message ***
 *
 *	Revision 1.10  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 1.9  1997/09/29  14:33:42  lewis
 *	Lose explicit fOwner filed in partitionmarkers. Instead use the one inheirted (casted)
 *	from class Marker.
 *	Renamed one variant of CalcSegmentSize to CalcSegmentSizeWithImmediateText to
 *	avoid confusion with mixins.
 *
 *	Revision 1.8  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 1.7  1997/07/27  15:01:13  lewis
 *	Edit docs
 *
 *	Revision 1.6  1997/07/23  23:04:17  lewis
 *	doc changes
 *
 *	Revision 1.5  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 1.4  1997/07/12  20:02:16  lewis
 *	AutoDoc support.
 *	TextImager_ renamed TextImager
 *
 *	Revision 1.3  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1997/06/18  02:43:53  lewis
 *	Moved CalcSegmentSize override here from TextImager and same for ResetTabStops (so we
 *	can add PartitionMarker* arg to it).
 *
 *	Revision 1.1  1997/03/04  20:05:53  lewis
 *	Initial revision
 *
 *
 *
 *	<========== CODE MOVED HERE FROM LineBasedTextImager.h ==========>
 *
 *
 *
 */

#include	"TextImager.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif



/*
@CONFIGVAR:		qCacheTextMeasurementsForPM
@DESCRIPTION:	<p>A fairly simple, but effective performance hack. Defaults ON</p>
 */
#ifndef	qCacheTextMeasurementsForPM
	#define	qCacheTextMeasurementsForPM			1
#endif






/*
@CLASS:			Partition
@BASES:			virtual @'MarkerOwner'
@DESCRIPTION:	<p>PartitioningTextImager is a TextImager which implements imaging by partitioning the text into stable
	regions, called Partitions.</p>
		<p>A partition is logically very similar to a @'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>'. But we
	don't directly use that class because it is not an abstract class, but rather a utility template. And that utility
	has very different performance considerations than we require. Also, that class is used to associate a MARKERINFO
	with a particular partition element. There is nothing we here associate with the partition element.</p>
		<p>For more information, see <a href=../DesignManual/TextImagers.html>DesignManual/TextImagers.html</a>
	(NB: This code has changed ALOT between Led 2.2 and 2.3, and so this doc really needs to be rewritten!).</p>
*/
class	Partition : public virtual MarkerOwner {
	private:
		typedef	MarkerOwner	inherited;
	public:
		Partition (TextStore& textStore);
		virtual ~Partition ();

	protected:
		virtual		void	FinalConstruct ();
	#if		qDebug
	private:
		bool	fFinalConstructCalled;
	#endif

	public:
		class	PartitionMarker;

	public:
		override	TextStore*	PeekAtTextStore () const;
	private:
		TextStore&	fTextStore;

	protected:
		virtual	void	UpdatePartitions (PartitionMarker* pm, const UpdateInfo& updateInfo) throw ()	=	0;

	public:
		virtual	PartitionMarker*	GetPartitionMarkerContainingPosition (size_t charPosition) const;
	private:
		mutable	PartitionMarker*	fFindContainingPMCache;


	protected:
		virtual	PartitionMarker*	MakeNewPartitionMarker (PartitionMarker* insertAfterMe);	// if insertAfter==NULL then prepend


	public:
		class	PartitionWatcher;
		nonvirtual	void	AddPartitionWatcher (PartitionWatcher* watcher);
		nonvirtual	void	RemovePartitionWatcher (PartitionWatcher* watcher);
	private:
		vector<PartitionWatcher*>	fPartitionWatchers;

	protected:
		virtual		void	Split (PartitionMarker* pm, size_t at);
		virtual		void	Coalece (PartitionMarker* pm);	// call when pm loses its trialing NL (may do nothing)
	private:
		nonvirtual		void	DoAboutToSplitCalls (PartitionMarker* pm, size_t at, vector<void*>* infos) const throw ();
		nonvirtual		void	DoDidSplitCalls (const vector<void*>& infos) const throw ();
		nonvirtual		void	DoAboutToCoaleceCalls (PartitionMarker* pm, vector<void*>* infos) const throw ();
		nonvirtual		void	DoDidCoaleceCalls (const vector<void*>& infos) const throw ();

	protected:
		nonvirtual	void	AccumulateMarkerForDeletion (PartitionMarker* m);
		MarkerMortuary<PartitionMarker>	fMarkersToBeDeleted;

	protected:
		override	void	AboutToUpdateText (const UpdateInfo& updateInfo);
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();


	// Trivial TextStore wrappers...
	public:
		nonvirtual	size_t	GetEnd () const;
		nonvirtual	void	CopyOut (size_t from, size_t count, Led_tChar* buffer) const;


	public:
		nonvirtual	PartitionMarker*	GetFirstPartitionMarker () const;
		nonvirtual	PartitionMarker*	GetLastPartitionMarker () const;

	private:
		#if		qDebug
		size_t				fPartitionMarkerCount;
		#endif
		PartitionMarker*	fPartitionMarkerFirst;
		PartitionMarker*	fPartitionMarkerLast;

	// Debug support
	public:
		nonvirtual	void	Invariant () const;
#if		qDebug
	protected:
		virtual		void	Invariant_ () const;
#endif
	private:
		friend	class	PartitionMarker;	//	For UpdatePartitions () call
};




/*
@CLASS:			Partition::PartitionMarker
@BASES:			@'Marker'
@DESCRIPTION:	<p>This class is the building block of a partition. It is used to keep track of a single partition element.</p>
				<p><em>Subclassing Caution</em>: Since we provide operator new/delete overrides, you must be careful
			building any subclasses which have a different size than this class. You must provide your own overrides
			of operator new/delete todo so.</p>
*/
class	Partition::PartitionMarker : public Marker {
	private:
		typedef	Marker	inherited;
	public:
		PartitionMarker (Partition& owner, PartitionMarker* insertAfterMe);	// if insertAfter==NULL then prepend

	public:
		LED_DECLARE_USE_BLOCK_ALLOCATION (PartitionMarker);

	// Calls Partition::UpdatePartitions ()
	public:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	// These markers are all kept in a doubly-linked list, headed by "GetOwner ()"
	public:
		virtual	PartitionMarker*	GetPrevious () const;
		virtual	PartitionMarker*	GetNext () const;

	private:
		PartitionMarker*	fPrevious;
		PartitionMarker*	fNext;

	public:
		nonvirtual	Partition&	GetOwner () const;

	private:
		STLDefCTORDeclare_BWA(PartitionMarker)
	private:
		friend	class	Partition;
};






/*
@CLASS:			Partition::PartitionWatcher
@DESCRIPTION:	<p>Some programs may need to keep track of when a partition changes. For example, an Imager will likely want to redisplay
	itself. Perhaps even do some more elaborate computation. Things like paragraph-based marker-covers (@'MarkerCover<MARKER,MARKERINFO,INCREMENTALMARKERINFO>')
	may need to adjust their boundaries to correspond to the new paragraph boundaries
	(e.g. @'WordProcessor::ParagraphDatabaseRep').</p>
		<p>You get notified just before, and after partition markers are split, and/or coalesced. The untyped 'info' records are used to
	pass arbitrary bits of info from the 'about' call to the 'did' call.</p>
*/
class	Partition::PartitionWatcher {
	public:
		virtual		void	AboutToSplit (PartitionMarker* pm, size_t at, void** infoRecord) const throw ()	=	0;
		virtual		void	DidSplit (void* infoRecord) const throw ()										=	0;
		virtual		void	AboutToCoalece (PartitionMarker* pm, void** infoRecord) const throw ()			=	0;
		virtual		void	DidCoalece (void* infoRecord) const throw ()									=	0;
};



/*
@CLASS:			PartitioningTextImager
@BASES:			virtual @'TextImager'
@DESCRIPTION:	<p>PartitioningTextImager is a TextImager which implements imaging by partitioning the text into stable
	regions, called Partitions.</p>
		<p>For more information, see <a href=../DesignManual/TextImagers.html>DesignManual/TextImagers.html</a>.</p>
*/
class	PartitioningTextImager : public virtual TextImager {
	private:
		typedef	TextImager	inherited;

	protected:
		PartitioningTextImager ();
		virtual ~PartitioningTextImager ();

	public:
		typedef	Led_RefCntPtr<Partition>	PartitionPtr;
		nonvirtual	PartitionPtr	GetPartition () const;
	protected:
		virtual		void			SetPartition (const PartitionPtr& partitionPtr);
	private:
		PartitionPtr	fPartition;

	public:
		virtual	PartitionPtr	MakeDefaultPartition () const		=	0;

	#if		qCacheTextMeasurementsForPM
	private:
		class	MeasureTextCache;
		#if		qAutoPtrBrokenBug
			Led_RefCntPtr<MeasureTextCache>	fMeasureTextCache;
		#else
			auto_ptr<MeasureTextCache>		fMeasureTextCache;
		#endif
	protected:
		override	void		InvalidateAllCaches ();
	#endif

	public:
		typedef	Partition::PartitionMarker	PartitionMarker;

	// Simple wrappers on the Partition object
	protected:
		nonvirtual	PartitionMarker*	GetFirstPartitionMarker () const;
	public:
		nonvirtual	size_t				GetStartOfPartitionContainingPosition (size_t charPosition) const;
		nonvirtual	size_t				GetEndOfPartitionContainingPosition (size_t charPosition) const;
		nonvirtual	PartitionMarker*	GetPartitionMarkerContainingPosition (size_t charPosition) const;

	public:
		virtual	TextDirection	GetPrimaryPartitionTextDirection (size_t rowContainingCharPosition) const;

	public:
		override	TextLayoutBlock_Copy	GetTextLayoutBlock (size_t rowStart, size_t rowEnd)	const;

	public:
		override		TextDirection	GetTextDirection (size_t charPosition)	const;

	public:
		override	Led_Distance	CalcSegmentSize (size_t from, size_t to) const;
	public:
		override	void			GetRowRelativeCharLoc (size_t charLoc, Led_Distance* lhs, Led_Distance* rhs) const;
		override	size_t			GetRowRelativeCharAtLoc (Led_Coordinate hOffset, size_t rowStart) const;

	private:
		nonvirtual	Led_Distance	CalcSegmentSize_REFERENCE (size_t from, size_t to) const;
	#if		qCacheTextMeasurementsForPM
		nonvirtual	Led_Distance	CalcSegmentSize_CACHING (size_t from, size_t to) const;
	#endif

	private:
		nonvirtual	void			CalcSegmentSize_FillIn (size_t rowStart, size_t rowEnd, Led_Distance* distanceVector) const;

	protected:
		virtual	size_t	ResetTabStops (size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const;


	// Debug support
	public:
		nonvirtual	void	Invariant () const;
#if		qDebug
	protected:
		virtual		void	Invariant_ () const;
#endif
};



#if		qCacheTextMeasurementsForPM
/*
@CLASS:			PartitioningTextImager::MeasureTextCache
@BASES:			private @'Partition::PartitionWatcher', @'MarkerOwner'
@DESCRIPTION:	<p>A helper class to implement the @'qCacheTextMeasurementsForPM' caching code.</p>
*/
class	PartitioningTextImager::MeasureTextCache : private Partition::PartitionWatcher, public MarkerOwner {
	private:
		typedef	void*	inherited;	// so any references to inherited generate an error...

	public:
		MeasureTextCache (const PartitionPtr& partition);
		~MeasureTextCache ();

	public:
		override	void	AboutToSplit (PartitionMarker* pm, size_t at, void** infoRecord) const throw ();
		override	void	DidSplit (void* infoRecord) const throw ();
		override	void	AboutToCoalece (PartitionMarker* pm, void** infoRecord) const throw ();
		override	void	DidCoalece (void* infoRecord) const throw ();

	public:
		override	TextStore*	PeekAtTextStore () const;

	protected:
		override	void	EarlyDidUpdateText (const UpdateInfo& updateInfo) throw ();

	public:
		class	CacheElt {
			public:
				CacheElt ();

			public:
				struct	COMPARE_ITEM {
					COMPARE_ITEM (PartitionMarker* pm, size_t startingRowAt): fPM (pm), fRowStartingAt (startingRowAt) {}

					PartitionMarker*	fPM;
					size_t				fRowStartingAt;
				};
			private:
				COMPARE_ITEM	fValidFor;

			// For LRUCache<> class
			public:
				nonvirtual	void	Clear ()
					{
						fValidFor.fPM = NULL;
					}
				static	bool	Equal (const CacheElt& lhs, const COMPARE_ITEM& rhs)
					{
						return lhs.fValidFor.fPM == rhs.fPM and lhs.fValidFor.fRowStartingAt == rhs.fRowStartingAt;
					}

			public:
				Led_SmallStackBuffer<Led_Distance>	fMeasurementsCache;		// for just the given PM
	
			private:
				friend	class	PartitioningTextImager::MeasureTextCache;
		};

	private:
		mutable LRUCache<CacheElt>	fCache;

	public:
		nonvirtual	void			ClearAll ();
		nonvirtual	const CacheElt*	LookupPM (PartitionMarker* pm, size_t rowStart) const;

	public:
		nonvirtual	CacheElt*		PrepareCacheUpdate (PartitionMarker* pm) const;
		nonvirtual	const CacheElt*	CompleteCacheUpdate (CacheElt* cacheElt, PartitionMarker* pm, size_t rowStart) const;

	private:
		PartitionPtr	fPartition;
};
#endif






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//	class	Partition::PartitionMarker;
	inline	Partition::PartitionMarker::PartitionMarker (Partition& owner, PartitionMarker* insertAfterMe):
		inherited (),
		fPrevious (insertAfterMe),
		fNext (insertAfterMe==NULL? NULL: insertAfterMe->fNext)
			{
				if (insertAfterMe == NULL) {
					owner.fPartitionMarkerFirst = this;
				}
				else {
					Led_Assert (insertAfterMe->fNext == fNext);		// I just updated MY next to be what HIS next used to be
					insertAfterMe->fNext = this;				// Now HIS next should be me since I'm AFTER him
					PartitionMarker*	following	=	fNext;
					if (following != NULL) {
						Led_Assert (following->fPrevious == insertAfterMe);	// thats who he USED to point to
						following->fPrevious = this;
					}
				}
				if (fNext == NULL) {
					owner.fPartitionMarkerLast = this;
				}
			}
	/*
	@METHOD:		Partition::PartitionMarker::GetOwner ()
	@DESCRIPTION:	<p>Return a reference to the partition markers owner. Note - since this returns a refernece,
		we assert that the owner is non-null. Also note that this routine intentionally hides the base-class
		@'Marker::GetOwner()' method. Use that if the owner could be NULL. But note that the owner will never
		be NULL if the marker has been added (and not yet removed) from the TextStore.</p>
	*/
	inline	Partition&	Partition::PartitionMarker::GetOwner () const
		{
			Led_EnsureNotNil (inherited::GetOwner ());
			Led_EnsureMember (inherited::GetOwner (), Partition);
			return *(dynamic_cast<Partition*> (inherited::GetOwner ()));
		}
	inline	Partition::PartitionMarker*	Partition::PartitionMarker::GetPrevious () const
		{
			return fPrevious;
		}
	inline	Partition::PartitionMarker*	Partition::PartitionMarker::GetNext () const
		{
			return fNext;
		}
	inline	void	Partition::Invariant () const
		{
			#if		qDebug && qHeavyDebugging
				Invariant_ ();
			#endif
		}




//	class	Partition;
	/*
	@METHOD:		Partition::GetEnd
	@DESCRIPTION:	<p>Return the associated @'TextStore::GetEnd' ().</p>
	*/
	inline	size_t	Partition::GetEnd () const
		{
			return (fTextStore.GetEnd ());
		}
	/*
	@METHOD:		Partition::CopyOut
	@DESCRIPTION:	<p>Calls the associated @'TextStore::CopyOut' ().</p>
	*/
	inline	void	Partition::CopyOut (size_t from, size_t byteCount, Led_tChar* buffer) const
		{
			GetTextStore ().CopyOut (from, byteCount, buffer);
		}
	inline	Partition::PartitionMarker*	Partition::GetFirstPartitionMarker () const
		{
			Led_RequireNotNil (fPartitionMarkerFirst);
			return fPartitionMarkerFirst;
		}
	inline	Partition::PartitionMarker*	Partition::GetLastPartitionMarker () const
		{
			Led_RequireNotNil (fPartitionMarkerLast);
			return fPartitionMarkerLast;
		}
	/*
	@METHOD:		Partition::AddPartitionWatcher
	@DESCRIPTION:	<p>Associate the given @'Partition::PartitionWatcher' with this Partition. Use
		@'Partition::RemovePartitionWatcher' to remove the callback / association.</p>
	*/
	inline	void	Partition::AddPartitionWatcher (PartitionWatcher* watcher)
		{
			fPartitionWatchers.push_back (watcher);
		}
	/*
	@METHOD:		Partition::RemovePartitionWatcher
	@DESCRIPTION:	<p>Remove the given @'Partition::PartitionWatcher' from being associated with this @'Partition'.
		Balances calls from @'Partition::AddPartitionWatcher'.</p>
	*/
	inline	void	Partition::RemovePartitionWatcher (PartitionWatcher* watcher)
		{
			vector<PartitionWatcher*>::iterator	it	=	find (fPartitionWatchers.begin (), fPartitionWatchers.end (), watcher);
			Led_Assert (it != fPartitionWatchers.end ());	// Be forgiving about not finding in list, in light of exception handling - but give warning...
			if (it != fPartitionWatchers.end ()) {
				fPartitionWatchers.erase (it);
			}
		}
	inline	void	Partition::DoAboutToSplitCalls (PartitionMarker* pm, size_t at, vector<void*>* infos) const throw ()
		{
			for (vector<PartitionWatcher*>::const_iterator it = fPartitionWatchers.begin (); it != fPartitionWatchers.end (); ++it) {
				void*	info;
				(*it)->AboutToSplit (pm, at, &info);
				infos->push_back (info);
			}
		}
	inline	void	Partition::DoDidSplitCalls (const vector<void*>& infos) const throw ()
		{
			Led_Assert (infos.size () == fPartitionWatchers.size ());
			vector<void*>::const_iterator infoIt = infos.begin ();
			for (vector<PartitionWatcher*>::const_iterator it = fPartitionWatchers.begin (); it != fPartitionWatchers.end (); ++it) {
				(*it)->DidSplit (*infoIt);
				++infoIt;
			}
		}
	inline	void	Partition::DoAboutToCoaleceCalls (PartitionMarker* pm, vector<void*>* infos) const throw ()
		{
			for (vector<PartitionWatcher*>::const_iterator it = fPartitionWatchers.begin (); it != fPartitionWatchers.end (); ++it) {
				void*	info;
				(*it)->AboutToCoalece (pm, &info);
				infos->push_back (info);
			}
		}
	inline	void	Partition::DoDidCoaleceCalls (const vector<void*>& infos) const throw ()
		{
			Led_Assert (infos.size () == fPartitionWatchers.size ());
			vector<void*>::const_iterator infoIt = infos.begin ();
			for (vector<PartitionWatcher*>::const_iterator it = fPartitionWatchers.begin (); it != fPartitionWatchers.end (); ++it) {
				(*it)->DidCoalece (*infoIt);
				++infoIt;
			}
		}




//	class	PartitioningTextImager;
	inline	PartitioningTextImager::PartitionPtr	PartitioningTextImager::GetPartition () const
		{
			return fPartition;
		}
	inline	Partition::PartitionMarker*	PartitioningTextImager::GetFirstPartitionMarker () const
		{
			Led_Require (not fPartition.IsNull ());		// perhaps you've forgotten to call SpecifyTextStore or SetPartition ()?
			return fPartition->GetFirstPartitionMarker ();
		}
	/*
	@METHOD:		PartitioningTextImager::GetPartitionMarkerContainingPosition
	@DESCRIPTION:	<p>Finds the @'PartitioningTextImager::PartitionMarker' which contains the given character#.
		Note, the use of 'charPosition' rather than markerpos is to disambiguiate the case where we are at the boundary
		between two partition elements.</p>
	*/
	inline	Partition::PartitionMarker*		PartitioningTextImager::GetPartitionMarkerContainingPosition (size_t charPosition) const
		{
			return fPartition->GetPartitionMarkerContainingPosition (charPosition);
		}
	/*
	@METHOD:		PartitioningTextImager::GetStartOfPartitionContainingPosition
	@DESCRIPTION:	<p>Simple wrapper on @'PartitioningTextImager::GetPartitionMarkerContainingPosition'.</p>
	*/
	inline	size_t		PartitioningTextImager::GetStartOfPartitionContainingPosition (size_t charPosition) const
		{
			return GetPartitionMarkerContainingPosition (charPosition)->GetStart ();
		}
	/*
	@METHOD:		PartitioningTextImager::GetEndOfPartitionContainingPosition
	@DESCRIPTION:	<p>Simple wrapper on @'PartitioningTextImager::GetPartitionMarkerContainingPosition'. Returns marker position
		AFTER end of partition (same as START position of following partition - if any).</p>
	*/
	inline	size_t		PartitioningTextImager::GetEndOfPartitionContainingPosition (size_t charPosition) const
		{
			return GetPartitionMarkerContainingPosition (charPosition)->GetEnd ();
		}
	inline	void	PartitioningTextImager::Invariant () const
		{
			#if		qDebug && qHeavyDebugging
				Invariant_ ();
			#endif
		}



#if		qCacheTextMeasurementsForPM
//	class	PartitioningTextImager::MeasureTextCache
	inline	void	PartitioningTextImager::MeasureTextCache::ClearAll ()
		{
			fCache.ClearCache ();
		}
	inline	const PartitioningTextImager::MeasureTextCache::CacheElt*	PartitioningTextImager::MeasureTextCache::LookupPM (PartitionMarker* pm, size_t rowStart) const
		{
			Led_RequireNotNil (pm);
			return fCache.LookupElement (CacheElt::COMPARE_ITEM (pm, rowStart));
		}
	inline	PartitioningTextImager::MeasureTextCache::CacheElt*	PartitioningTextImager::MeasureTextCache::PrepareCacheUpdate (PartitionMarker* pm) const
		{
			Led_RequireNotNil (pm);
			Led_Arg_Unused (pm);
			return fCache.AddNew ();
		}


//	class	PartitioningTextImager::MeasureTextCache::CacheElt
	inline	PartitioningTextImager::MeasureTextCache::CacheElt::CacheElt ():
			fValidFor (NULL, 0),
			fMeasurementsCache (0)
		{
		}
#endif




#if		qLedUsesNamespaces
}
#endif

#endif	/*__PartitioningTextImager_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
