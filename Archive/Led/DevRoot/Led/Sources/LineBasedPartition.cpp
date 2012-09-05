/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/LineBasedPartition.cpp,v 2.35 2003/02/28 19:05:58 lewis Exp $
 *
 * Changes:
 *	$Log: LineBasedPartition.cpp,v $
 *	Revision 2.35  2003/02/28 19:05:58  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.34  2002/11/18 21:11:09  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.33  2002/10/14 23:37:46  lewis
 *	SPR#1126 - LOTS of work (very ugly right now and MAYBE still buggy) to support new partition
 *	code so that tables apepar in other own partition element. Added PartHELPER<> and WordProcessor::WPPartition
 *	and XXWPPartition. ALSO - fixed SPR#1127- UndoableContextHelper for insert embedding
 *	
 *	Revision 2.32  2002/10/08 16:38:05  lewis
 *	SPR#1119- Added CheckForSplits/NeedToCoalesce to LineBasedPartition. Add WordProcessor::WPPartition.
 *	Add / use MakeDefaultPartition () so we create the new partition for WPs.
 *	
 *	Revision 2.31  2002/05/06 21:33:47  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.30  2001/11/27 00:29:53  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.29  2001/10/17 20:42:58  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.28  2001/08/29 23:36:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.27  2001/08/28 18:43:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.26  2000/04/16 14:39:37  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.25  2000/04/14 22:40:25  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.24  1999/11/13 16:32:21  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.23  1999/05/03 22:05:11  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.22  1999/05/03 21:41:30  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.21  1998/04/08 01:47:13  lewis
 *	Major redesign - now partitions are separable objects, and this is a
 *	partition object, not a TextImager at all.
 *	
 *
 *	<<<Renamed LineBasedPartition.cpp from LineBasedTextImager.cpp>>>
 *
 *
 *	Revision 2.20  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1997/09/29  15:05:55  lewis
 *	Lose qLedFirstIndex support.
 *
 *	Revision 2.18  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.17  1997/07/14  00:46:48  lewis
 *	Fixed support for qLedFirstIndex == 0.
 *	AutoDoc support.
 *
 *	Revision 2.16  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1997/06/18  02:58:58  lewis
 *	qIncludePrefixFile
 *
 *	Revision 2.14  1997/03/04  20:11:36  lewis
 *	Moved most of the implemantion of this into a new base class -
 *	PartiioningTextImager.
 *	-------------------------------------------------------------
 *	------------------------------------------------------------
 *
 *	Revision 2.13  1997/01/10  03:17:57  lewis
 *	AboutToUpdateText() returns void.
 *	throw specifiers.
 *
 *	Revision 2.12  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.11  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.9  1996/05/23  19:43:06  lewis
 *	Tweeks to LineBasedTextImager::Split()
 *	a few other cleanups/comments.
 *
 *	Revision 2.8  1996/03/16  18:54:39  lewis
 *	revised invariant to make better qLedFirstIndex aware.
 *	nvariant_ const.
 *	Make marker::Split() code more exception-friendly.
 *
 *	Revision 2.7  1996/02/26  22:29:37  lewis
 *	Use Led_Min/Led_Max intead of Min/Max (old names)
 *
 *	Revision 2.6  1996/02/05  04:20:01  lewis
 *	Use new inherited typedef.
 *	Get rid of SpecifyTextStore () override, and use HookGainedNewTextStore_
 *	etc instead.
 *
 *	Revision 2.5  1995/12/09  05:39:17  lewis
 *	Use new PeekAtTextStore_, partly for efficiency, but mostly to work around
 *	MSVC 4.0 (and earlier) bugs with vtables before object complete.
 *
 *	Revision 2.4  1995/11/02  22:30:46  lewis
 *	qLedFirstIndex fix
 *
 *	Revision 2.3  1995/10/19  22:23:38  lewis
 *	Some one-based index cleanups (MUCH MORE TODO).
 *	Check updateInfo.fTextModified field in DidUpdate method.
 *
 *	Revision 2.2  1995/10/09  22:35:21  lewis
 *	Fix SpeciyfTextStore code to rebuild the partition elemenbt list properly
 *	when newly added.
 *	Use new DidUpdateText etc hooks from TextStore instead of old
 *	PrePost  replace.
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.12  1995/06/08  05:16:27  lewis
 *	Code cleanups
 *
 *	Revision 1.11  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.10  1995/05/20  04:59:33  lewis
 *	Fixed spelling of Invariant
 *	Added GetPartitionMarkerContainingPosition (size_t charPosition) const
 *	keeping cached last time value as optimization. See SPR 0127.
 *
 *	Revision 1.9  1995/05/09  23:39:43  lewis
 *	Lose unused withWhat parameter to Pre/PostReplace - SPR 0240.
 *
 *	Revision 1.8  1995/05/06  19:53:00  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *	Lose obsolete code for qCallingBaseClassMemberFunctionWithNestedClassesSymCPlusPlusBug workaround
 *	cuz we dropped compat with OLD symC++ compiler.
 *
 *	Revision 1.7  1995/05/05  19:50:33  lewis
 *	Led_SmallStackBuffer now templated class.
 *
 *	Revision 1.6  1995/04/18  00:43:29  lewis
 *	Cleanup
 *
 *	Revision 1.5  1995/04/18  00:16:08  lewis
 *	Cleanups.
 *	Fix arg to LineBasedTextImager::AccumulateMarkerForDeletion.
 *	Temporarily added workaround to bug with replace calling CollectAllMarkersinRange
 *	while we still had some guys in the list to be finalized. THis workarond
 *	worked - but isn't as good as just asserting this never happens
 *	and then making sure it doesn't - at least I think so.
 *	I'm pretty sure its always safe to call finalize early.
 *
 *	Revision 1.4  1995/04/16  19:33:27  lewis
 *	Added override of PostReplace to call FinalizeMarkersForDeletion
 *	and added AccumulateMarkerforDleteion and FinalizeMarkersForDeltion
 *	instead of removing and deleting markers directly in coalesce. Instead
 *	can use RemoveMarkers (plural) which is MUCH faster for large deletions.
 *	See SPR#0207.
 *
 *	Revision 1.3  1995/03/13  03:22:48  lewis
 *	Added Assert_CharPosDoesNotSplitCharacter() to LineBasedTextImager::Split().
 *
 *	Revision 1.2  1995/03/02  05:46:10  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:46  lewis
 *	Initial revision
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"LineBasedPartition.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif








/*
 ********************************************************************************
 ****************************** LineBasedPartition ******************************
 ********************************************************************************
 */

LineBasedPartition::LineBasedPartition (TextStore& textStore):
	inherited (textStore)
{
	FinalConstruct ();
}

LineBasedPartition::LineBasedPartition (TextStore& textStore, SpecialHackToDisableInit /*hack*/):
	inherited (textStore)
{
}

void	LineBasedPartition::FinalConstruct ()
{
	inherited::FinalConstruct ();
	PartitionMarker*	pm	=	GetFirstPartitionMarker ();
	// Perform any splits of the first created PM into lines (in case text buffer starts with text in it)
	for (size_t i = GetTextStore ().GetLength (); i > 0 ; i--) {
		Led_tChar	c;
		GetTextStore ().CopyOut (i - 1, 1, &c);
		if (c == '\n') {
			Split (pm, i);
		}
	}
}

/*
@METHOD:		LineBasedPartition::UpdatePartitions
@DESCRIPTION:	<p>Hook notification that the given partition was updated. See if any newlines were added
	or deleted, and update the partitions as appropriate. Do not call directly.</p>
				<p>Call @'LineBasedPartition::CheckForSplits' for each character to see if any splits are needed,
			and call @'LineBasedPartition::NeedToCoalesce' to see if the given PM needs coalescing because of the update.</p>
*/
void	LineBasedPartition::UpdatePartitions (PartitionMarker* pm, const UpdateInfo& updateInfo) throw ()
{
	Led_RequireNotNil (pm);

	if (updateInfo.fTextModified) {
		/*
		*	First check if we had any newlines inserted into our middle. If so, then
		*	we must split ourselves. Then check if we retain our trailing newline (or
		*	are the last partition element) and if not - coalece with the next element.
		*
		*	When checking for newlines inserted - we iterate backwards because when
		*	we do splits - we lose the ability to process the rest of the insertion, and
		*	there could be multiple newlines inserted.
		*/
		size_t	startOfInsert	=	Led_Max (updateInfo.fReplaceFrom, pm->GetStart ());
		size_t	endOfInsert		=	Led_Min (updateInfo.GetResultingRHS (), pm->GetEnd ());
		for (size_t i = endOfInsert; i > startOfInsert ; i--) {
			CheckForSplits (pm, updateInfo, i);
		}

		// See if after insertion of that text this PM needs to be coalesed with the next
		bool	coalesce	=	NeedToCoalesce (pm);
		if (coalesce) {
			Coalece (pm);		// 'pm' is DELETED BY THIS SO DO NOTHING to it AFTERWARDS!!!
		}
	}
}

/*
@METHOD:		LineBasedPartition::CheckForSplits
@ACCESS:		protected
@DESCRIPTION:	<p>Called by @'LineBasedPartition::UpdatePartitions' () to check if the given inserted text
			requires any partition elements	to be split. 'i' is the position at which we may want
			to do a split (e.g. right after the newline character).</p>
*/
void	LineBasedPartition::CheckForSplits (PartitionMarker* pm, const UpdateInfo& updateInfo, size_t i) throw ()
{
	Led_Require (updateInfo.fTextModified);	//so there is something in the fTextInserted area
	if (updateInfo.fTextInserted[i-updateInfo.fReplaceFrom-1] == '\n') {
		Split (pm, i);
	}
}

/*
@METHOD:		LineBasedPartition::NeedToCoalesce
@ACCESS:		protected
@DESCRIPTION:	<p>Called by @'LineBasedPartition::UpdatePartitions' () to check if the given inserted text requires this PM to be coalesed with its
			following one.</p>
*/
bool	LineBasedPartition::NeedToCoalesce (PartitionMarker* pm) throw ()
{
	// If after inserting a bunch of characters, and deleting some too, my
	// last character is no longer a newline - better Coalece...
	bool	coalesce	=	false;
	if (pm->GetLength () == 0) {
		coalesce = true;
	}
	else {
		if (pm->GetNext () != NULL) {
			size_t	end	=	pm->GetEnd ();
			Led_tChar	endChar	=	'\0';
			// Look at the character just BEFORE (rather than after) the end
			CopyOut (end-1, 1, &endChar);
			if (endChar != '\n') {
				coalesce = true;
			}
		}
	}
	return coalesce;
}

#if		qDebug
/*
@METHOD:		LineBasedPartition::Invariant_
@DESCRIPTION:	<p>Check internal consitency of data structures. Don't call this directly. Call Invariant instead.
	And only call at quiesent times; not in the midst of some update where data structures might not be fully consistent.</p>
*/
void	LineBasedPartition::Invariant_ () const
{
	inherited::Invariant_ ();
	for (PartitionMarker* cur = GetFirstPartitionMarker (); cur != NULL; cur = cur->GetNext ()) {
		Led_AssertNotNil (cur);
		size_t	start	=	cur->GetStart ();
		size_t	end		=	cur->GetEnd ();
		size_t	len		=	end-start;

		if (end > GetEnd ()) {
			len--;	// Last partition extends past end of text
		}
		Led_SmallStackBuffer<Led_tChar>	buf (len);
		CopyOut (start, len, buf);
		for (size_t i = 1; i < len; i++) {
			Led_Assert (buf[i-1] != '\n');
		}
		if (cur->GetNext () != NULL) {		// All but the last partition must be NL terminated...
			Led_Assert (buf[len-1] == '\n');
		}
	}
}
#endif



#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


