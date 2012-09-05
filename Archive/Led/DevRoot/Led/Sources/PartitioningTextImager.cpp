/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/PartitioningTextImager.cpp,v 2.63 2004/01/27 03:36:32 lewis Exp $
 *
 * Changes:
 *	$Log: PartitioningTextImager.cpp,v $
 *	Revision 2.63  2004/01/27 03:36:32  lewis
 *	SPR#1597: Got rid of old 'fix' for SPR#1232 (it was erronious) in PartitioningTextImager::GetRowRelativeCharAtLoc. Real fix is in TextInteractor::GetCharAtClickLocation ()
 *	
 *	Revision 2.62  2003/11/27 20:42:21  lewis
 *	cleanups (lost ifdefed out stuff)
 *	
 *	Revision 2.61  2003/11/27 20:39:04  lewis
 *	doccomments, code cleanups, and make PartitioningTextImager::MeasureTextCache : private Partition::PartitionWatcher
 *	
 *	Revision 2.60  2003/11/03 20:05:44  lewis
 *	add new TextImager::GetTextLayoutBlock () method - which in subclasses - can take into account subclasses to use an intiail dir from first row in subsequent rows. Related - added PartitioningTextImager::GetPrimaryPartitionTextDirection ()
 *	
 *	Revision 2.59  2003/03/31 20:17:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2003/03/21 14:51:59  lewis
 *	added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 2.57  2003/03/06 14:02:06  lewis
 *	SPR#1325 - dont BREAK on looking for PMs to inval cache just cuz you found ONE with a
 *	given PM - could be multiple (diff rows in same para)
 *	
 *	Revision 2.56  2003/03/04 21:32:51  lewis
 *	SPR#1315 - redo the qCacheTextMeasurementsForPM cache using the new LRUCache<> template.
 *	Dynamically set cache size (based on TextStore size)
 *	
 *	Revision 2.55  2003/02/13 21:51:16  lewis
 *	SPR#1134- Cleanup/improve qCacheTextMeasurementsForPM/MeasureTextCache code. Now keep a
 *	primitive multi-element cache. This cache is crummy - and could be substantially improved,
 *	but wait til I profile to see how important that is
 *	
 *	Revision 2.54  2003/01/29 18:12:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.53  2003/01/28 22:46:39  lewis
 *	slight speed tweek in PartitioningTextImager::MeasureTextCache::EarlyDidUpdateText
 *	
 *	Revision 2.52  2003/01/11 19:28:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.51  2003/01/11 18:14:06  lewis
 *	Small fix to PartitioningTextImager::GetRowRelativeCharAtLoc to fix SPR#1232 (click on end of wrapped row)
 *	
 *	Revision 2.50  2003/01/10 22:18:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.49  2003/01/08 15:40:42  lewis
 *	SPR#1225 - PartitioningTextImager::MeasureTextCache::EarlyDidUpdateText needed to look
 *	PartitioningTextImager::MeasureTextCache::EarlyDidUpdateText needed to look
 *	
 *	Revision 2.48  2003/01/06 16:11:40  lewis
 *	SPR#1219- small fixes to PartitioningTextImager::GetRowRelativeCharAtLoc() for mixed BIDI text case
 *	(multiple runs). And disable the LedDebugTrace calls
 *	
 *	Revision 2.47  2002/12/21 03:00:11  lewis
 *	SPR#1217. Added virtual TextImager::InvalidateAllCaches and made SimpleTextImager/MRTI versions
 *	just overrides. No longer need SimpleTextImager::TabletChangedMetrics overrides etc
 *	since now TextImager::TabletChangedMetrics calls TextImager::InvalidateAllCaches.
 *	MultiRowTextImager::PurgeUnneededMemory () no longer needed since TextImager version
 *	now calls InvalidateAllCaches().
 *	
 *	Revision 2.46  2002/12/19 16:50:25  lewis
 *	SPR#1214- fix edge conditions in PartitioningTextImager::GetRowRelativeCharAtLoc - end / start of rows
 *	
 *	Revision 2.45  2002/12/19 13:28:29  lewis
 *	SPR#1212- fix qCacheTextMeasurementsForPM / PartitioningTextImager::CalcSegmentSize code.
 *	Need to measure whole row!
 *	
 *	Revision 2.44  2002/12/17 23:44:07  lewis
 *	SPR#1211 - fix mouse selection for BIDI - PartitioningTextImager::GetRowRelativeCharAtLoc
 *	
 *	Revision 2.43  2002/12/14 03:42:21  lewis
 *	hack PartitioningTextImager::GetTextDirection() support when qCacheTextMeasurementsForPM
 *	true(spr#1205 entered to call for fix
 *	
 *	Revision 2.42  2002/12/13 22:06:49  lewis
 *	For SPR#1189 - major BIDI rewrite of PartitioningTextImager::GetRowRelativeCharLoc.
 *	For SPR#1204 - added TextLayoutBlock::GetCharacterDirection (BIDI SUP).
 *	
 *	Revision 2.41  2002/12/07 18:05:21  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.40  2002/12/06 17:48:49  lewis
 *	prelim work on SPR#1189/GetRowRelativeCharLoc bidi support. 'qBidiImpl' defined 0 for now til
 *	I have other things debugged and can come back to this
 *	
 *	Revision 2.39  2002/12/06 15:33:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.38  2002/12/06 15:15:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.37  2002/12/06 15:13:40  lewis
 *	SPR#1198 - added 2 new APIs to TextImager, implemented them in PartitioningTextImager,
 *	and used  them in MultiRowTextImager and SimpleTextImager:
 *		GetRowRelativeCharLoc ()
 *		GetRowRelativeCharAtLoc ().
 *	
 *	Revision 2.36  2002/12/04 22:27:58  lewis
 *	more cleanups of the new CalcSegmentSize code: delete MeasureTextCache::AboutToUpdateText checker.
 *	Fix DidUpdate checker to only inval if that area changed. Delete CalcSegmentSizeWithImmediateText.
 *	Doccomments.
 *	
 *	Revision 2.35  2002/12/04 16:45:03  lewis
 *	turn qCacheTextMeasurementsForPM on (was accidentally left off so I never really tested
 *	1193 change - but now OK)
 *	
 *	Revision 2.34  2002/12/04 15:45:51  lewis
 *	SPR#1193 - added PartitioningTextImager::MeasureTextCache to implement qCacheTextMeasurementsForPM and
 *	use CalcSegmentSize instead of CalcSegmentSizeWithImmediateText
 *	
 *	Revision 2.33  2002/11/18 21:12:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.32  2002/10/14 23:37:48  lewis
 *	SPR#1126 - LOTS of work (very ugly right now and MAYBE still buggy) to support new partition code so
 *	that tables apepar in other own partition element. Added PartHELPER<> and WordProcessor::WPPartition
 *	and XXWPPartition. ALSO - fixed SPR#1127- UndoableContextHelper for insert embedding
 *	
 *	Revision 2.31  2002/10/11 01:42:55  lewis
 *	SPR#1125 fix small bug with WPPartition code (assert in invariant). Call Invariant
 *	(in Partition code) more often.
 *	
 *	Revision 2.30  2002/05/06 21:33:49  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.29  2001/11/27 00:29:54  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.28  2001/10/17 20:42:59  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.27  2001/09/29 16:23:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.26  2001/09/29 16:21:57  lewis
 *	SPR#1052- added fPartitionMarkerLast field and use that in
 *	Partition::GetPartitionMarkerContainingPosition as a speed tweek
 *	
 *	Revision 2.25  2001/08/29 23:36:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.24  2001/08/28 18:43:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.23  2001/01/03 14:42:29  Lewis
 *	qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__. And lose asserts that some
 *	unsigned ints were >= 0
 *	
 *	Revision 2.22  2000/06/13 15:31:44  lewis
 *	SPR#0767- convert tabstop support to store TWIPS internally
 *	
 *	Revision 2.21  2000/04/16 14:39:37  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.20  2000/04/14 22:40:25  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.19  2000/01/19 19:26:25  lewis
 *	SPR#0699- added more assertions that I'm properly deleted all markers from the partition -
 *	better updating/cehcking of fParitionMarkerCounter
 *	
 *	Revision 2.18  1999/12/14 18:09:13  lewis
 *	DocComments - and use Led_NEltsOf ()
 *	
 *	Revision 2.17  1999/11/13 16:32:22  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.16  1999/06/28 15:08:18  lewis
 *	fixed bug in Partition::AccumulateMarkerForDeletion () where it was ALWAYS
 *	clearing cache. Dangerous to fix now. So test well!
 *	
 *	Revision 2.15  1999/06/28 15:05:28  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.14  1999/05/03 22:05:13  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.13  1998/10/30 14:40:11  lewis
 *	cleanups?
 *	
 *	Revision 2.12  1998/07/24  01:11:37  lewis
 *	Changed ResteTabStop() code - howfully for last time.
 *	Now (hopefully) well documented how it works.
 *
 *	Revision 2.11  1998/04/25  01:31:56  lewis
 *	Cleanup, and only keep track of fPartitionMarkerCount if qDebug -
 *	since never used except for asserts.
 *	(and would interfere with GetPrev/GetNext() dynamic allocation of
 *	PMs trick REMO has been advised to use.
 *
 *	Revision 2.10  1998/04/09  01:31:34  lewis
 *	Use new MarkerMortuary<> templated helper class - instead of old
 *	manual impmentaiton.
 *	No semantic changes (I believe)
 *
 *	Revision 2.9  1998/04/08  01:49:18  lewis
 *	Major redesign. Introduced a separable, sharable Partition class,
 *	which is owned by the PartitioningTextImager.
 *
 *	Revision 2.8  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/09/29  15:09:48  lewis
 *	Lose qLedFirstIndex supprot.
 *	Lose fOwner field from PM. Instead grab/cast inherited one from
 *	Marker base class.
 *	Renamed CalcSegmentSize/3 to CalcSegmentSizeWithImmediateText
 *	to avoid ambiguities.
 *
 *	Revision 2.6  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.5  1997/07/23  23:10:00  lewis
 *	Docs.
 *
 *	Revision 2.4  1997/07/15  05:26:36  lewis
 *	AutoDoc content.
 *
 *	Revision 2.3  1997/07/14  00:45:19  lewis
 *	Support qLedFirstIndex = 0.
 *	REnamed TextImager_ to TextImager, and TextStore_ to TextStore, etc.
 *
 *	Revision 2.2  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1997/06/18  03:01:22  lewis
 *	Moved ResetTabStops here from TextImager, and added PartitionMarker* arg.
 *	Call GetTabStopList () with start of PM.
 *	Moved TextImager::CalcSegmentSize () here.
 *
 *	Revision 2.0  1997/03/04  20:11:11  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== CODE MOVED HERE FROM LineBasedTextImager.cpp ==========>
 *
 */

#include	"PartitioningTextImager.h"





#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif








typedef	Partition::PartitionMarker	PartitionMarker;





/*
 ********************************************************************************
 ************************************ Partition *********************************
 ********************************************************************************
 */
Partition::Partition (TextStore& textStore):
	inherited (),
	#if		qDebug
		fFinalConstructCalled (false),
	#endif
	fTextStore (textStore),
	fFindContainingPMCache (NULL),
	fPartitionWatchers (),
	fMarkersToBeDeleted (),
	#if		qDebug
		fPartitionMarkerCount (0),
	#endif
	fPartitionMarkerFirst (NULL),
	fPartitionMarkerLast (NULL)
{
	fTextStore.AddMarkerOwner (this);
}

Partition::~Partition ()
{
	// This was commented out as of 20000119 - not sure why... If it causes trouble maybe
	// OK to just force empty here/now. But understand if so/why...
	Led_Assert (fMarkersToBeDeleted.IsEmpty ());		// these better be deleted by now!

	/*
	 *		We could simply loop and remove all markers with RemoveMarker().
	 *	But using RemoveMarkers() is much faster. The trouble is we may
	 *	not have enough memory to do the removeal, and an exception here
	 *	would be very bad (deleting object shouldn't fail cuz not enuf memory!).
	 *
	 *		So we compromise, and use a small stack buffer to delete as much
	 *	as we can at a time. For must cases, this will be plenty.
	 *
	 *		The alternative was to use a Led_Array - and if we ran out of memory,
	 *	then to fall back to old slow strategy. That should work fine too, but I think
	 *	this is slightly simpler.
	 */
	for (PartitionMarker* cur = fPartitionMarkerFirst; cur != NULL;) {
		Marker*	markersToRemoveAtATime[1000];
		const	size_t	kMaxBufMarkers	=	Led_NEltsOf (markersToRemoveAtATime);
		size_t	i = 0;
		for (; i < kMaxBufMarkers and cur != NULL; i++, cur = cur->fNext) {
			markersToRemoveAtATime[i] = cur;
		}
		fTextStore.RemoveMarkers (markersToRemoveAtATime, i);
		#if		qDebug
			fPartitionMarkerCount -= i;
		#endif
		for (; i != 0; i--) {
			delete (markersToRemoveAtATime[i-1]);
		}
	}
	fTextStore.RemoveMarkerOwner (this);
	Led_Ensure (fPartitionMarkerCount == 0);
}

/*
@METHOD:		Partition::FinalConstruct
@DESCRIPTION:	<p>Logically this should be part of the constructor. But since it needs to call a method
	(@'Partition::MakeNewPartitionMarker ()') which is virtual, and must be bound in the final complete
	class. So it must be done as a separate call. It is illegal to call this more than once, or to call any
	of the other class methods without having called it. These errors will be detected (in debug builds)
	where possible.</p>
 */
void	Partition::FinalConstruct ()
{
	#if		qDebug
		Led_Require (not fFinalConstructCalled);
		fFinalConstructCalled = true;
	#endif
	Led_Assert (fPartitionMarkerCount == 0);
	Led_Assert (fPartitionMarkerFirst == NULL);
	PartitionMarker*	pm	=	MakeNewPartitionMarker (NULL);
	#if		qDebug
		fPartitionMarkerCount++;
	#endif
	fTextStore.AddMarker (pm, 0, fTextStore.GetLength ()+1, this);	// include ALL text
	Led_Assert (fPartitionMarkerFirst == pm);
	Led_Assert (fPartitionMarkerLast == pm);
}

TextStore*	Partition::PeekAtTextStore () const
{
	return &fTextStore;
}

/*
@METHOD:		Partition::GetPartitionMarkerContainingPosition
@DESCRIPTION:	<p>Finds the @'PartitioningTextImager::PartitionMarker' which contains the given character#.
	Note, the use of 'charPosition' rather than markerpos is to disambiguiate the case where we are at the boundary
	between two partition elements.</p>
*/
PartitionMarker*		Partition::GetPartitionMarkerContainingPosition (size_t charPosition) const
{
	Led_Require (fFinalConstructCalled);
	Led_Require (charPosition <= GetEnd () + 1);	// cuz last PM contains bogus char past end of buffer

	/*
	 *	Based on cached value, either search forwards from there or back.
	 */
	PartitionMarker* pm	=	fFindContainingPMCache;
	if (pm == NULL) {
		pm = fPartitionMarkerFirst;		// could be first time, or could have deleted cached value
	}
	Led_AssertNotNil (pm);

	if (charPosition < 10) {
		pm = fPartitionMarkerFirst;
	}
	else if (charPosition + 10 > GetEnd ()) {
		pm = fPartitionMarkerLast;
	}

	bool	loopForwards	=	pm->GetStart () <= charPosition;
	for (; pm != NULL; pm = loopForwards? pm->fNext: pm->fPrevious) {
		if (Contains (*pm, charPosition)) {
			if (pm != fPartitionMarkerFirst and pm != fPartitionMarkerLast) {
				fFindContainingPMCache = pm;
			}
			Led_EnsureNotNil (pm);
			return (pm);
		}
	}
	#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
		#pragma push
		#pragma warn -8008
	#endif
		Led_Assert (false);		return (NULL);
	#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
		#pragma pop
	#endif
}

/*
@METHOD:		Partition::MakeNewPartitionMarker
@DESCRIPTION:	<p>Method which is called to construct new partition elements. Override this if you subclass
	@'PartitioningTextImager', and want to provide your own subtype of @'PartitioningTextImager::PartitionMarker'.</p>
*/
PartitionMarker*	Partition::MakeNewPartitionMarker (PartitionMarker* insertAfterMe)
{
	Led_Require (fFinalConstructCalled);
	return (new PartitionMarker (*this, insertAfterMe));
}

/*
@METHOD:		Partition::Split
@DESCRIPTION:	<p>Split the given PartitionMarker at position 'at' - which must fall within the partition element.
	This method always produces a new partition element, and inserts it appropriately into the partition, adjusting
	the sizes of surrounding elements appropriately.</p>
		<p>Ensure that the split happens so that PM continues to point to the beginning of the range and the new PMs created
	are at the end of the range</p>
		<p>This method is typically called by Partition subclasses override of @'Partition::UpdatePartitions'.</p>
*/
void	Partition::Split (PartitionMarker* pm, size_t at)
{
	Led_Require (fFinalConstructCalled);
	Led_RequireNotNil (pm);
	Led_Require (pm->GetStart () < at);
	Led_Require (pm->GetEnd () > at);
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (at);
#endif

	vector<void*>	watcherInfos;
	DoAboutToSplitCalls (pm, at, &watcherInfos);

	size_t	start	=	0;
	size_t	end		=	0;
	pm->GetRange (&start, &end);
	Led_Assert (at >= start and at <= end);

	// try to cleanup as best as we can if we fail in the middle of adding a new PM.
	PartitionMarker*	newPM	=	MakeNewPartitionMarker (pm);
	Led_Assert (pm->GetNext () == newPM);
	Led_Assert (newPM->GetPrevious () == pm);

	// NB: we set marker range before adding new marker for no good reason, except that
	// for quirky reasons I don't understand, reading in files is MUCH faster that
	// way - I really don't understand the idiocyncracies of the ChunckedArrayTextStore
	// performance... but luckily - no real need to...
	// LGP 960515
	fTextStore.SetMarkerEnd (pm, at);
	try {
		fTextStore.AddMarker (newPM, at, end-at, this);
	}
	catch (...) {
		// the only item above that can throw is the AddMarker () call. So at this point, all we need todo
		// is remove newPM from our linked list, and then delete it. Of course, our Partition elements won't be
		// quite right. But with no memory, this is the best we can do. At least we still have a partition!
		Led_AssertNotNil (newPM);
		if (pm == NULL) {
			fPartitionMarkerFirst = newPM->fNext;
		}
		else {
			pm->fNext = newPM->fNext;
		}
		// Now patch pm's old and new successor (before and after newPM breifly was it)
		if (newPM->fNext != NULL) {
			Led_Assert (newPM->fNext->fPrevious == newPM);	// but thats getting deleted...
			newPM->fNext->fPrevious = pm;
		}
		if (newPM == fPartitionMarkerLast) {
			fPartitionMarkerLast = newPM->fPrevious;
		}

		// Since we set this BEFORE the AddMarker, we must set it back. Thats the only
		// cost in the above '960515' performance hack - and it only occurs EXTREMELY
		// RARELY and in a case I don't care about.
		// LGP 960515
		fTextStore.SetMarkerRange (pm, start, end);

		throw;
	}
	#if		qDebug
		fPartitionMarkerCount++;
	#endif
	DoDidSplitCalls (watcherInfos);
}

/*
@METHOD:		Partition::Coalece
@DESCRIPTION:	<p>Coalece the given PartitionMarker with the one which follows it (if any exists). If coalesced
	with the following element, the given 'pm' is deleted. Frequently this doesn't happen immediately (mainly
	for performance reasons, but possibly also to avoid bugs where the marker is still in some list being iterated over
	and having DidUpdate() etc methods called on it). It is accumulated for later deletion using
	@'Partition::AccumulateMarkerForDeletion'.</p>
		<p>This method is typically called by Partition subclasses override of @'Partition::UpdatePartitions'.</p>
*/
void	Partition::Coalece (PartitionMarker* pm)
{
	Led_Require (fFinalConstructCalled);
	Led_AssertNotNil (pm);
	vector<void*>	watcherInfos;
	DoAboutToCoaleceCalls (pm, &watcherInfos);
	if (pm->fNext != NULL) {		// We don't do anything to coalesce the last item - nothing to coalesce with!
		size_t	start	=	0;
		size_t	end		=	0;
		pm->GetRange (&start, &end);
		size_t	lengthToAdd	=	end-start;

		PartitionMarker*	successor	=	pm->fNext;
		Led_AssertNotNil (successor);
		successor->GetRange (&start, &end);
		Led_Assert (start >= lengthToAdd);
		fTextStore.SetMarkerStart (successor, start - lengthToAdd);

		Led_Assert (successor->fPrevious == pm);
		successor->fPrevious = pm->fPrevious;
		Led_Assert ((pm->fPrevious == NULL) or (pm->fPrevious->fNext == pm));
		if (pm->fPrevious == NULL) {
			Led_Assert (fPartitionMarkerFirst == pm);
			fPartitionMarkerFirst = pm->fNext;
		}
		else {
			pm->fPrevious->fNext = pm->fNext;
		}
		if (pm->fNext == NULL) {
			Led_Assert (fPartitionMarkerLast == pm);
			fPartitionMarkerLast = pm->fPrevious;
		}
		#if		qDebug
			fPartitionMarkerCount--;
		#endif
		AccumulateMarkerForDeletion (pm);
	}
	DoDidCoaleceCalls (watcherInfos);
}

/*
@METHOD:		Partition::AccumulateMarkerForDeletion
@DESCRIPTION:	<p>Wrap @'MarkerMortuary<MARKER>::AccumulateMarkerForDeletion' - and make sure our cache
	isn't pointing to a deleted marker.</p>
*/
void	Partition::AccumulateMarkerForDeletion (PartitionMarker* m)
{
	Led_Require (fFinalConstructCalled);
	Led_AssertNotNil (m);
	Led_Assert (&m->GetOwner () == this);
	fMarkersToBeDeleted.AccumulateMarkerForDeletion (m);
	if (fFindContainingPMCache == m) {
		fFindContainingPMCache = NULL;
	}
}

void	Partition::AboutToUpdateText (const UpdateInfo& updateInfo)
{
	Led_Require (fFinalConstructCalled);
	Led_Assert (fMarkersToBeDeleted.IsEmpty ());		// would be bad to do a replace with any of these not
														// yet finalized since they would then appear in the
														// CollectAllMarkersInRange() and get DidUpdate calls!
	Invariant ();
	inherited::AboutToUpdateText (updateInfo);
}

void	Partition::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	Led_Require (fFinalConstructCalled);
	fMarkersToBeDeleted.FinalizeMarkerDeletions ();
	inherited::DidUpdateText (updateInfo);
	Invariant ();
}

#if		qDebug
void	Partition::Invariant_ () const
{
	Led_Require (fFinalConstructCalled);
	size_t	lastCharDrawn	=	0;
	Led_Assert (fPartitionMarkerCount != 0);
	size_t	realPMCount	=	0;
	for (PartitionMarker* cur = fPartitionMarkerFirst; cur != NULL; cur = cur->fNext) {
		PartitionMarker*	pm	=	cur;
		Led_AssertNotNil (pm);
		Led_Assert (&pm->GetOwner () == this);
		size_t	start	=	pm->GetStart ();
		size_t	end		=	pm->GetEnd ();
		size_t	len		=	end-start;

		Led_Assert (start == lastCharDrawn);
		Led_Assert (end <= GetEnd () + 1);	// +1 for extra bogus space so we always get autoexpanded

		lastCharDrawn = end;

		if (end > GetEnd ()) {
			len--;	// Last partition extends past end of text
		}
		realPMCount++;
		Led_Assert (realPMCount <= fPartitionMarkerCount);
		Led_Assert (static_cast<bool> (fPartitionMarkerLast == cur) == static_cast<bool> (cur->fNext == NULL));
	}
	Led_Assert (realPMCount == fPartitionMarkerCount);
	Led_Assert (lastCharDrawn == GetEnd () + 1);
}
#endif








/*
 ********************************************************************************
 ****************************** PartitioningTextImager **************************
 ********************************************************************************
 */
PartitioningTextImager::PartitioningTextImager ():
	inherited (),
	fPartition (NULL)
	#if		qCacheTextMeasurementsForPM
	,fMeasureTextCache ()
	#endif
{
}

PartitioningTextImager::~PartitioningTextImager ()
{
	Led_Require (fPartition.IsNull ());
}

/*
@METHOD:		PartitioningTextImager::SetPartition
@DESCRIPTION:	<p>Associates the given parition (@'PartitionPtr') with this @'PartitioningTextImager'.
	It can be NULL.</p>
		<p>The method is virtual, in case you need to hook partition changes by updating
	other derived/cached information. But if overridding, be sure to always call the inherited version.</p>
*/
void	PartitioningTextImager::SetPartition (const PartitionPtr& partitionPtr)
{
	#if		qCacheTextMeasurementsForPM
		#if		qAutoPtr_NoResetMethodBug
			fMeasureTextCache = auto_ptr<MeasureTextCache> (NULL);
		#else
			fMeasureTextCache.reset ();
		#endif
	#endif
	fPartition = partitionPtr;
	#if		qCacheTextMeasurementsForPM
		if (not partitionPtr.IsNull ()) {
			#if		qAutoPtrBrokenBug
				fMeasureTextCache = Led_RefCntPtr<MeasureTextCache> (new MeasureTextCache (partitionPtr));
			#else
				fMeasureTextCache = auto_ptr<MeasureTextCache> (new MeasureTextCache (partitionPtr));
			#endif
		}
	#endif
}

#if		qCacheTextMeasurementsForPM
/*
@METHOD:		PartitioningTextImager::InvalidateAllCaches
@DESCRIPTION:	<p>Hook the @'TextImager::InvalidateAllCaches' message to free some additional caches.</p>
*/
void		PartitioningTextImager::InvalidateAllCaches ()
{
	inherited::InvalidateAllCaches ();
	if (fMeasureTextCache.get () != NULL) {
		fMeasureTextCache->ClearAll ();
	}
}
#endif

/*
@METHOD:		PartitioningTextImager::GetPrimaryPartitionTextDirection
@DESCRIPTION:	<p>Return the primary text direction for the partition. This COULD be specified by some external
			user setting, or - by default - simply computed from the text direction of the first row of the
			paragraph.</p>
*/
TextDirection	PartitioningTextImager::GetPrimaryPartitionTextDirection (size_t rowContainingCharPosition) const
{
	return GetTextDirection (GetStartOfPartitionContainingPosition (rowContainingCharPosition));
}

TextLayoutBlock_Copy	PartitioningTextImager::GetTextLayoutBlock (size_t rowStart, size_t rowEnd)	const
{
	if (rowStart == GetStartOfPartitionContainingPosition (rowStart)) {
		return inherited::GetTextLayoutBlock (rowStart, rowEnd);
	}
	else {
		size_t							rowLen		=	rowEnd-rowStart;
		Led_SmallStackBuffer<Led_tChar>	rowBuf (rowLen);
		CopyOut (rowStart, rowLen, rowBuf);
		return TextLayoutBlock_Copy (TextLayoutBlock_Basic	(rowBuf, rowBuf + rowLen, GetPrimaryPartitionTextDirection (rowStart)));
	}
}

/*
@METHOD:		PartitioningTextImager::GetTextDirection
@DESCRIPTION:	<p>Implementation of abstract interface @'TextImager::GetTextDirection'</p>
*/
TextDirection	PartitioningTextImager::GetTextDirection (size_t charPosition)	const
{
	size_t	startOfRow	=	GetStartOfRowContainingPosition (charPosition);
	size_t	endOfRow	=	GetEndOfRowContainingPosition (startOfRow);
	if (charPosition == endOfRow) {
		return eLeftToRight;	//??Not sure what todo here??
								// should probably grab from ambient setting for paragraph direction or something like that
	}
	else {
		return GetTextLayoutBlock (startOfRow, endOfRow).GetCharacterDirection (charPosition-startOfRow);
	}
}

/*
@METHOD:		PartitioningTextImager::CalcSegmentSize
@DESCRIPTION:	<p>Implementation of abstract interface @'TextImager::CalcSegmentSize'</p>
				<p>Note that internally - this must grab the ENTIRE ROW to measure (or nearly so).
			To assure ligatures are measured properly (to could be part of a ligature) we need the characters just past
			'to' to measure it properly. And - to measure tabs properly - we need to go before 'from' all the way to
			the start of the row. So - we just always goto the starts and ends of rows. Since
			@'PartitioningTextImager::CalcSegmentSize_CACHING' caches these values - this isn't a great cost.</p>
*/
Led_Distance	PartitioningTextImager::CalcSegmentSize (size_t from, size_t to) const
{
	#if		!qCacheTextMeasurementsForPM || qDebug
		size_t	referenceValue	=	CalcSegmentSize_REFERENCE (from, to);
	#endif

	#if		qCacheTextMeasurementsForPM
		size_t	value			=	CalcSegmentSize_CACHING (from, to);
		Led_Assert (value == referenceValue);
		return value;
	#else
		return referenceValue;
	#endif
}

/*
@METHOD:		PartitioningTextImager::CalcSegmentSize_REFERENCE
@ACCESS:		private
@DESCRIPTION:	<p>Similar to @'PartitioningTextImager::CalcSegmentSize_CACHING' but recalculating the measurements
			each time as a check that the cache has not somehow (undetected) become invalid (say cuz a font changed
			and we weren't notified?).</p>
*/
Led_Distance	PartitioningTextImager::CalcSegmentSize_REFERENCE (size_t from, size_t to) const
{
	Led_Require (from <= to);

	if (from == to) {
		return 0;
	}
	else {
		size_t	startOfRow	=	GetStartOfRowContainingPosition (from);
		size_t	rowEnd		=	GetEndOfRowContainingPosition (startOfRow);
		Led_Require (startOfRow <= from);		//	WE REQUIRE from/to be contained within a single row!!!
		Led_Require (to <= rowEnd);				//	''
		size_t	rowLen		=	rowEnd - startOfRow;
		Led_SmallStackBuffer<Led_Distance>	distanceVector (rowLen);
		CalcSegmentSize_FillIn (startOfRow, rowEnd, distanceVector);
		Led_Assert (to > startOfRow);															// but from could be == startOfRow, so must be careful of that...
		Led_Assert (to-startOfRow-1 < (GetEndOfRowContainingPosition (startOfRow)-startOfRow));	// now buffer overflows!
		Led_Distance	result	=	distanceVector[to-startOfRow-1];
		if (from != startOfRow) {
			result -= distanceVector[from-startOfRow-1];
		}
		return (result);
	}
}

#if		qCacheTextMeasurementsForPM
/*
@METHOD:		PartitioningTextImager::CalcSegmentSize_CACHING
@ACCESS:		private
@DESCRIPTION:	<p>Caching implementation of @'PartitioningTextImager::CalcSegmentSize'. Values checked by
			calls to related @'PartitioningTextImager::CalcSegmentSize_REFERENCE'.</p>
*/
Led_Distance	PartitioningTextImager::CalcSegmentSize_CACHING (size_t from, size_t to) const
{
	Led_Require (from <= to);

	if (from == to) {
		return 0;
	}
	PartitionMarker*	pm	=	GetPartitionMarkerContainingPosition (from);
	Led_Require (pm->GetEnd () == to or pm == GetPartitionMarkerContainingPosition (to));	// since must be in same row, must be in same PM.

	size_t	startOfRow	=	GetStartOfRowContainingPosition (from);
	Led_Require (GetEndOfRowContainingPosition (startOfRow) >= to);		//	WE REQUIRE from/to be contained within a single row!!!

	const MeasureTextCache::CacheElt*	ce	=	fMeasureTextCache->LookupPM (pm, startOfRow);
	if (ce == NULL) {
		MeasureTextCache::CacheElt*	updateCE = fMeasureTextCache->PrepareCacheUpdate (pm);
		size_t	rowEnd		=	GetEndOfRowContainingPosition (startOfRow);
		size_t	rowLen		=	rowEnd - startOfRow;
		updateCE->fMeasurementsCache.GrowToSize (rowLen);
		CalcSegmentSize_FillIn (startOfRow, rowEnd, updateCE->fMeasurementsCache);
		ce = fMeasureTextCache->CompleteCacheUpdate (updateCE, pm, startOfRow);
		Led_Assert (ce == fMeasureTextCache->LookupPM (pm, startOfRow));
	}
	Led_AssertNotNil (ce);
	const	Led_Distance*	measurementsCache	=	ce->fMeasurementsCache;

	Led_Assert (to > startOfRow);															// but from could be == startOfRow, so must be careful of that...
	Led_Assert (to-startOfRow-1 < (GetEndOfRowContainingPosition (startOfRow)-startOfRow));	// now buffer overflows!
	Led_Distance	result	=	measurementsCache[to-startOfRow-1];
	if (from != startOfRow) {
		result -= measurementsCache[from-startOfRow-1];
	}
	return (result);
}
#endif

/*
@METHOD:		PartitioningTextImager::CalcSegmentSize_FillIn
@ACCESS:		private
@DESCRIPTION:	<p>The 'rowStart' argument MUST start a row, and rowEnd must END that same row. 'distanceVector' must be a
			non-null array whose  size is set to at least (rowEnd-rowStart) elements.</p>
*/
void	PartitioningTextImager::CalcSegmentSize_FillIn (size_t rowStart, size_t rowEnd, Led_Distance* distanceVector) const
{
	Led_Require (rowStart == GetStartOfRowContainingPosition (rowStart));	// must already be a rowstart
	Led_Require (rowEnd == GetEndOfRowContainingPosition (rowStart));		// ""
	Led_RequireNotNil (distanceVector);
	Led_Require (rowStart <= rowEnd);

	// we must re-snag the text to get the width/tab alignment of the initial segment (for reset tabstops)- a bit more complicated ...
	size_t	len	=	rowEnd-rowStart;

	Led_SmallStackBuffer<Led_tChar>	fullRowTextBuf (len);
	CopyOut (rowStart, len, fullRowTextBuf);

	MeasureSegmentWidth (rowStart, rowEnd, fullRowTextBuf, distanceVector);
	(void)ResetTabStops (rowStart, fullRowTextBuf, len, distanceVector, 0);
}

/*
@METHOD:		PartitioningTextImager::GetRowRelativeCharLoc
@DESCRIPTION:	<p>Implementation of abstract interface @'TextImager::GetRowRelativeCharLoc'</p>
*/
void	PartitioningTextImager::GetRowRelativeCharLoc (size_t charLoc, Led_Distance* lhs, Led_Distance* rhs) const
{
	Led_Require (charLoc <= GetEnd ());
	Led_RequireNotNil (lhs);
	Led_RequireNotNil (rhs);

	/*
	 *	Note that this algoritm assumes that TextImager::CalcSegmentSize () measures the VIRTUAL characters,
	 *	including any mapped characters (mirroring) that correspond to the argument REAL character range.
	 */
	size_t	rowStart	=	GetStartOfRowContainingPosition (charLoc);
	size_t	rowEnd		=	GetEndOfRowContainingPosition (charLoc);
	size_t	rowLen		=	rowEnd-rowStart;
	TextLayoutBlock_Copy	rowText = GetTextLayoutBlock (rowStart, rowEnd);

	typedef	TextLayoutBlock::ScriptRunElt	ScriptRunElt;
	vector<ScriptRunElt> runs	=	rowText.GetScriptRuns ();

	/*
	 *	Walk through the runs in VIRTUAL order (screen left to right). Find the run whose charLoc is inside
	 *	the "REAL" run span. Stop measuring there.
	 */
	Led_Assert (not runs.empty () or (rowLen == 0));
	if (runs.size () > 1) {
		// sort by virtual start
		sort (runs.begin (), runs.end (), TextLayoutBlock::LessThanVirtualStart ());
	}
	size_t			rowRelCharLoc	=	charLoc - rowStart;
	Led_Distance	spannedSoFar	=	0;
	for (vector<ScriptRunElt>::const_iterator i = runs.begin (); i != runs.end (); ++i) {
		const ScriptRunElt&	se	=	*i;
		size_t	runLength	=	se.fRealEnd - se.fRealStart;
		// See if we are STRICTLY inside the run, or maybe at the last character of the last run
		if ((se.fRealStart <= rowRelCharLoc) and 
			((rowRelCharLoc < se.fRealEnd) or ((rowRelCharLoc == se.fRealEnd) and (i+1 == runs.end ())))
			) {
			size_t	absoluteSegStart	=	rowStart + se.fRealStart;
			size_t	subSegLen			=	rowRelCharLoc-se.fRealStart;
			Led_Assert (subSegLen <= runLength);
			
			size_t	nextPosition	=	FindNextCharacter (charLoc);
			bool	emptyChar		=	(nextPosition == charLoc);
			if (not emptyChar) {
				Led_tChar	lastChar	=	'\0';
				CopyOut (charLoc, 1, &lastChar);
				emptyChar = (RemoveMappedDisplayCharacters (&lastChar, 1) == 0);
			}

// EXPLAIN LOGIC (Once I have it right) ;-)
			if (se.fDirection == eLeftToRight) {
				*lhs = spannedSoFar + CalcSegmentSize (absoluteSegStart, absoluteSegStart + subSegLen);
				if (emptyChar) {
					*rhs = *lhs;
				}
				else {
					*rhs = spannedSoFar + CalcSegmentSize (absoluteSegStart, FindNextCharacter (absoluteSegStart + subSegLen));
				}
			}
			else {
				Led_Coordinate	segRHS	=	spannedSoFar + CalcSegmentSize (absoluteSegStart, absoluteSegStart + runLength);
				*rhs = segRHS - CalcSegmentSize (absoluteSegStart, absoluteSegStart + subSegLen);
				if (emptyChar) {
					*lhs = *rhs;
				}
				else {
					*lhs = segRHS - CalcSegmentSize (absoluteSegStart, FindNextCharacter (absoluteSegStart + subSegLen));
				}
			}
			break;
		}
		else {
			spannedSoFar += CalcSegmentSize (rowStart + se.fRealStart, rowStart + se.fRealEnd);
		}
	}
	Led_Ensure (*lhs <= *rhs);	// can be equal for case like 'RemoveMappedDisplayCharacters'
}

/*
@METHOD:		PartitioningTextImager::GetRowRelativeCharAtLoc
@DESCRIPTION:	<p>Implementation of abstract interface @'TextImager::GetRowRelativeCharAtLoc'</p>
*/
size_t	PartitioningTextImager::GetRowRelativeCharAtLoc (Led_Coordinate hOffset, size_t rowStart) const
{
	Led_Require (rowStart == GetStartOfRowContainingPosition (rowStart));

	/*
	 *	Note that this algoritm assumes that TextImager::CalcSegmentSize () measures the VIRTUAL characters,
	 *	including any mapped characters (mirroring) that correspond to the argument REAL character range.
	 */
	size_t	rowEnd		=	GetEndOfRowContainingPosition (rowStart);
	size_t	rowLen		=	rowEnd-rowStart;
	TextLayoutBlock_Copy	rowText = GetTextLayoutBlock (rowStart, rowEnd);

	typedef	TextLayoutBlock::ScriptRunElt	ScriptRunElt;
	vector<ScriptRunElt> runs	=	rowText.GetScriptRuns ();

	/*
	 *	Walk through the runs in VIRTUAL order (screen left to right). Find the run whose hOffset is inside
	 *	Then find the character which hOffset resides in (within that run).
	 */
	Led_Assert (not runs.empty () or (rowLen == 0));
	if (runs.size () > 1) {
		// sort by virtual start
		sort (runs.begin (), runs.end (), TextLayoutBlock::LessThanVirtualStart ());
	}
	Led_Distance	spannedSoFar	=	0;
	TextDirection	lastRunDir		=	eLeftToRight;
	for (vector<ScriptRunElt>::const_iterator i = runs.begin (); i != runs.end (); ++i) {
		const ScriptRunElt&	se				=	*i;
		Led_Distance		thisSpanWidth	=	CalcSegmentSize (rowStart + se.fRealStart, rowStart + se.fRealEnd);
		Led_Distance		segVisStart		=	spannedSoFar;
		Led_Distance		segVisEnd		=	segVisStart + thisSpanWidth;

		lastRunDir = se.fDirection;

		if (hOffset < static_cast<Led_Coordinate> (segVisEnd)) {
			/*
			 *	Must be this segment. NB: this takes care of special case where mouseLoc is BEFORE first segment in which case
			 *	we treat as at the start of that segment...
			 *
			 *	Now walk through the segment and see when we have enough chars gone by within the segment to
			 *	get us past 'hOffset'
			 */
			size_t	absoluteSegStart	=	rowStart + se.fRealStart;
			size_t	segLen				=	se.fRealEnd-se.fRealStart;

			size_t	prevEnd	=	rowStart + se.fRealStart;
			size_t	segEnd	=	rowStart + se.fRealEnd;
			for (size_t curEnd = FindNextCharacter (prevEnd); curEnd < segEnd; (prevEnd = curEnd), (curEnd = FindNextCharacter (curEnd))) {
				Led_Distance	hSize	=	CalcSegmentSize (absoluteSegStart, curEnd);
				if (se.fDirection == eLeftToRight) {
					if (static_cast<Led_Coordinate> (hSize + spannedSoFar) > hOffset) {
						#if		qMultiByteCharacters
							Assert_CharPosDoesNotSplitCharacter (prevEnd);
						#endif
						//LedDebugTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (LTR)", hOffset, prevEnd);
						return (prevEnd);
					}
				}
				else {
					if (static_cast<Led_Coordinate> (segVisEnd) - static_cast<Led_Coordinate> (hSize) < hOffset) {
						#if		qMultiByteCharacters
							Assert_CharPosDoesNotSplitCharacter (prevEnd);
						#endif
						//LedDebugTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (RTL)", hOffset, prevEnd);
						return (prevEnd);
					}
				}
			}
			//LedDebugTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (EOS)", hOffset, prevEnd);
			return (prevEnd);
		}

		spannedSoFar += thisSpanWidth;
	}

	Led_Assert (hOffset > 0 or runs.size () == 0);
	if (lastRunDir == eLeftToRight) {
		//LedDebugTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (EOR-LTR)", hOffset, rowEnd);
		return rowEnd;
	}
	else {
		//LedDebugTrace ("PartitioningTextImager::GetRowRelativeCharAtLoc (offset=%d,...) returning %d (EOR-RTL)", hOffset, rowStart);
		return rowStart;
	}
}

/*
@METHOD:		PartitioningTextImager::ResetTabStops
@DESCRIPTION:	<p>Patch the charLocations for tab-stop locations, now that we know our previous wrap point.
	Returns index of last tab found (allows for optimizations). Returns a number <= from if
	no tabs found.</p>
		<p>'from' defines where in the 'textStore' all these things are being measured relative to (typically the start of
	a partition).</p>
		<p>'text' is the text starting in the partition at position 'from', and extending 'nTChars' Led_tChars in length.</p>
		<p>'charLocations' is a vector - of the widths, measured from @'TextImager::MeasureSegmentWidth'. As such, it
	measures distances from 'from' to a position 'i' after from, with the distance of from..from being implied zero, so the first
	elt of the array left out (in other words to find distance from..i, you say charLocations[i-from-1]).</p>
		<p>'startSoFar' is an arg so you can only reset the tabstops for a given subset of the text - ignoring the initial portion.
	So, if startSoFar==0, then it is assumed we are starting at the beginning of the charLocations array, but if startSoFar != 0,
	we assume we can (and must) snag our starting width from what is already in the array at charLocations[startSoFar-1].</p>
*/
size_t	PartitioningTextImager::ResetTabStops (size_t from, const Led_tChar* text, size_t nTChars, Led_Distance* charLocations, size_t startSoFar) const
{
	Led_RequireNotNil (charLocations);
	size_t			lastTabIndex	=	0;
	Led_Coordinate	tabAdjust		=	0;
	Led_Distance	widthAtStart	=	(startSoFar==0?0:charLocations[startSoFar-1]);
	for (size_t i = startSoFar; i < startSoFar+nTChars; i++) {
		if (text[i] == '\t') {
			Led_Distance	widthSoFar	=	(i==0?0:charLocations[i-1]);
			tabAdjust = widthAtStart + GetTabStopList (from).ComputeTabStopAfterPosition (Tablet_Acquirer (this), widthSoFar - widthAtStart) - charLocations[i];
			lastTabIndex = i;
		}
		charLocations[i] += tabAdjust;
	}
	return (lastTabIndex);
}

#if		qDebug
void	PartitioningTextImager::Invariant_ () const
{
	if (not fPartition.IsNull ()) {
		fPartition->Invariant ();
		Led_Assert (fPartition->PeekAtTextStore () == PeekAtTextStore ());
	}
}
#endif





#if		qCacheTextMeasurementsForPM
/*
 ********************************************************************************
 ************* PartitioningTextImager::MeasureTextCache *************************
 ********************************************************************************
 */
PartitioningTextImager::MeasureTextCache::MeasureTextCache (const PartitionPtr& partition):
	fPartition (partition),
	fCache (1)
{
	Led_Assert (not partition.IsNull ());
	fPartition->AddPartitionWatcher (this);
	TextStore&	ts	=	partition->GetTextStore ();
	ts.AddMarkerOwner (this);
}

PartitioningTextImager::MeasureTextCache::~MeasureTextCache ()
{
	fPartition->RemovePartitionWatcher (this);
	TextStore&	ts	=	fPartition->GetTextStore ();
	ts.RemoveMarkerOwner (this);
}

const PartitioningTextImager::MeasureTextCache::CacheElt*	PartitioningTextImager::MeasureTextCache::CompleteCacheUpdate (CacheElt* cacheElt, PartitionMarker* pm, size_t rowStart) const
{
	Led_RequireNotNil (cacheElt);
	cacheElt->fValidFor.fPM = pm;
	cacheElt->fValidFor.fRowStartingAt = rowStart;
	return cacheElt;
}

void	PartitioningTextImager::MeasureTextCache::AboutToSplit (PartitionMarker* pm, size_t /*at*/, void** infoRecord) const throw ()
{
	*infoRecord = pm;
}

void	PartitioningTextImager::MeasureTextCache::DidSplit (void* infoRecord) const throw ()
{
	PartitionMarker*	pm	=	reinterpret_cast<PartitionMarker*> (infoRecord);
	for (LRUCache<CacheElt>::CacheIterator i = fCache.begin (); i != fCache.end (); ++i) {
		if ((*i).fValidFor.fPM == pm) {
			(*i).Clear ();
		}
	}
}

void	PartitioningTextImager::MeasureTextCache::AboutToCoalece (PartitionMarker* pm, void** infoRecord) const throw ()
{
	Led_RequireNotNil (infoRecord);
	Led_RequireNotNil (pm);
	*infoRecord = pm;
}

void	PartitioningTextImager::MeasureTextCache::DidCoalece (void* infoRecord) const throw ()
{
	PartitionMarker*	pm	=	reinterpret_cast<PartitionMarker*> (infoRecord);
	for (LRUCache<CacheElt>::CacheIterator i = fCache.begin (); i != fCache.end (); ++i) {
		if ((*i).fValidFor.fPM == pm) {
			(*i).Clear ();
		}
	}
}

TextStore*	PartitioningTextImager::MeasureTextCache::PeekAtTextStore () const
{
	return fPartition->PeekAtTextStore ();
}

void	PartitioningTextImager::MeasureTextCache::EarlyDidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	{
		size_t	cacheSize	=	1;
		size_t	bufLen		=	GetTextStore ().GetLength ();
		const	size_t	kBigThresh1	=	1024;
		const	size_t	kBigThresh2	=	10*1024;
		const	size_t	kBigThresh3	=	20*1024;
		if (bufLen > kBigThresh1) {
			cacheSize = 3;
			if (bufLen > kBigThresh2) {
				cacheSize = 10;
				if (bufLen > kBigThresh3) {
					cacheSize = 75;
				}
			}
		}
		fCache.SetMaxCacheSize (cacheSize);
	}

	/*
	 * iterate over all markers in the range updated, and for each one - see if they intersect with
	 * any of the cache elements.
	 */
	for (PartitionMarker* pm = fPartition->GetPartitionMarkerContainingPosition (FindPreviousCharacter (updateInfo.fReplaceFrom)); pm != NULL; pm = pm->GetNext ()) {
		if (pm->GetStart () > updateInfo.GetResultingRHS ()) {
			break;
		}
// Could optimize this further.... (MUCH)
		for (LRUCache<CacheElt>::CacheIterator i = fCache.begin (); i != fCache.end (); ++i) {
			if ((*i).fValidFor.fPM == pm) {
				(*i).Clear ();
			}
		}
	}
	MarkerOwner::EarlyDidUpdateText (updateInfo);
}
#endif







/*
 ********************************************************************************
 ********************************** PartitionMarker *****************************
 ********************************************************************************
 */
void	PartitionMarker::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	inherited::DidUpdateText (updateInfo);
	GetOwner ().UpdatePartitions (this, updateInfo);
}




#if		qLedUsesNamespaces
}
#endif



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


