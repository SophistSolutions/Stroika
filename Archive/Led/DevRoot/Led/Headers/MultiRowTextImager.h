/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__MultiRowTextImager_h__
#define	__MultiRowTextImager_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/MultiRowTextImager.h,v 2.84 2004/01/26 16:42:04 lewis Exp $
 */


/*
@MODULE:	MultiRowTextImager
@DESCRIPTION:
		<p>MultiRowTextImager is a @'TextImager' which supports having partition elements made up of multirow text.</p>

 */


/*
 *
 * Changes:
 *	$Log: MultiRowTextImager.h,v $
 *	Revision 2.84  2004/01/26 16:42:04  lewis
 *	SPR#1604: Lose overrides of ComputeRelativePosition () since the speedtweek can be done directly in the TextImager::ComputeRelativePosition () version using GetStartOfNextRowFromRowContainingPosition () etc.
 *	
 *	Revision 2.83  2003/04/18 22:43:04  lewis
 *	Minor tweeks for MSVC80 (VC++ .Net 2003) compatability
 *	
 *	Revision 2.82  2003/04/18 17:19:17  lewis
 *	small code cleanups to things complained about by new gcc 2.96
 *	
 *	Revision 2.81  2003/01/29 17:59:46  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.80  2003/01/11 19:28:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.79  2002/12/21 02:59:59  lewis
 *	SPR#1217. Added virtual TextImager::InvalidateAllCaches and made SimpleTextImager/MRTI versions
 *	just overrides. No longer need SimpleTextImager::TabletChangedMetrics overrides etc
 *	since now TextImager::TabletChangedMetrics calls TextImager::InvalidateAllCaches.
 *	MultiRowTextImager::PurgeUnneededMemory () no longer needed since TextImager version
 *	now calls InvalidateAllCaches().
 *	
 *	Revision 2.78  2002/12/20 17:55:58  lewis
 *	SPR#1216- get rid of override of GetSelectionWindowRegion. New version in TextImager
 *	now properly handles BIDI, and interline space (interline space not tested,but
 *	not really used much anymore??).
 *	
 *	Revision 2.77  2002/12/04 15:43:58  lewis
 *	SPR#1193 - lose qCacheTextMeasurementsForOneRow hack (now in PartitioningTextImager) and
 *	use CalcSegmetnSize instead of CalcSegmentSizeWithImmediateText
 *	
 *	Revision 2.76  2002/12/02 15:57:48  lewis
 *	SPR#1185 - move DrawRow,DrawRowSegments,DrawRowHilight,DrawInterlineSpace from MRTI/SimpleTextImager to
 *	TextImager (share code. Changed their args slightly. For BIDI work.
 *	
 *	Revision 2.75  2002/12/02 00:51:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.74  2002/11/27 15:58:23  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 2.73  2002/11/27 15:13:44  lewis
 *	As part of SPR#1183 - BIDI project- used TextLayoutBlock class instead of Led_tChar* as
 *	arg to DrawRow and DrawRowSegments()
 *	routines. Much more of this sort to come! NB: NOT BACKWARD COMPAT!
 *	
 *	Revision 2.72  2002/11/27 13:06:53  lewis
 *	add DrawPartitionElement() method and break out some of the Draw() code into that as prelim for SPR#1183
 *	
 *	Revision 2.71  2002/10/23 20:47:46  lewis
 *	minor tweak to PMInfoCacheMgr to make it respect private access properly (detected on MSVC60)
 *	
 *	Revision 2.70  2002/10/23 20:01:23  lewis
 *	SPR#1144 - qAutoPtrBrokenBug workaround
 *	
 *	Revision 2.69  2002/10/23 18:00:22  lewis
 *	small tweeks to get working on MSVC60
 *	
 *	Revision 2.68  2002/10/21 20:27:33  lewis
 *	SPR#1134- Moved date in PartitionElementCacheInfo into a shared Rep object so copy CTOR/op=
 *	would be MUCH faster (at the cost of extra deref in use)
 *	
 *	Revision 2.67  2002/10/21 16:21:30  lewis
 *	SPR#1133 - more cleanups of new WPPartition code and PMCacheMgr etc. Still more todo
 *	
 *	Revision 2.66  2002/10/21 14:41:34  lewis
 *	SPR#1134 - partial cleanup of new PartitionElementCacheInfo and PMInfoCacheMgr code
 *	
 *	Revision 2.65  2002/10/20 19:38:05  lewis
 *	SPR#1128 and SPR#1129 - BIG change. Get rid of MultiRowPartitionMarker and MultiRowPartition. Still coded
 *	very slopily (with idefs) and needs major cleanups/tweeks. But - its now functional and testable
 *	
 *	Revision 2.64  2002/10/14 23:22:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.63  2002/10/08 16:37:52  lewis
 *	SPR#1119- Added CheckForSplits/NeedToCoalesce to LineBasedPartition. Add WordProcessor::WPPartition.
 *	Add / use MakeDefaultPartition () so we create the new partition for WPs.
 *	
 *	Revision 2.62  2002/09/20 15:06:01  lewis
 *	get rid of qTypedefsOfPrivateAccessNamesNotAllowedInSourceFile - wasn't really a bug
 *	
 *	Revision 2.61  2002/05/06 21:33:32  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.60  2001/11/27 00:29:43  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.59  2001/10/20 13:38:56  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.58  2001/10/17 20:42:51  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.57  2001/09/26 15:41:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.56  2001/09/24 16:28:59  lewis
 *	SPR#0993- made InvalidateAllCaches virtual and override in WordProcessorHScrollbarHelper<BASECLASS>
 *	as part of attempt to fix quirky mac sbar code on threshold between showing/hiding scrollbars
 *	
 *	Revision 2.55  2001/08/29 23:00:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.54  2001/08/28 18:43:28  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.53  2001/01/03 14:56:10  Lewis
 *	qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS. And delete pointless
 *	asserts that unsgined numbers >= 0.
 *	
 *	Revision 2.52  2000/08/13 05:21:45  lewis
 *	SPR#0824- LED_DECLARE_USE_BLOCK_ALLOCATION instead of DECLARE_USE_BLOCK_ALLOCATION
 *	
 *	Revision 2.51  2000/07/26 05:46:35  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner
 *	arg to CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table of speedups.
 *	
 *	Revision 2.50  2000/06/17 07:44:27  lewis
 *	SPR#0792- Add TextImager::GetRowRelativeBaselineOfRowContainingPosition() as part
 *	of fixing this. And impelmnt in MRTI. And added asserts to MRTI::DrawRowSegments()
 *	that the start/end are reasonable for that segment.
 *	
 *	Revision 2.49  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.48  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.47  2000/03/31 16:59:13  lewis
 *	SPR#0721- use DECLARE_USE_BLOCK_ALLOCATION macro
 *	
 *	Revision 2.46  2000/03/11 22:04:47  lewis
 *	SPR#0711 - new GetRealEndOfRow* () routines. And fixed PositionWouldFitInWindowWithThisTopRow ()
 *	to call it. And other small fixes to TextInteractor::ScrollToSelection ()  and Simple/MultiRow::ScrollSoShowing ()
 *	
 *	Revision 2.45  2000/03/09 04:40:29  lewis
 *	replace old MultiRowTextImager::HilightARectanle_ (and SimpleTextImager version) with
 *	TextImager::HilightARectanlge() ETC - SPR#0708
 *	
 *	Revision 2.44  1999/12/21 03:22:57  lewis
 *	PREV CHECKIN WAS PART OF SPR#0671.
 *	
 *	Revision 2.43  1999/12/21 03:22:14  lewis
 *	Lose all references to \n - except in new ContainsMappedDisplayCharacters/RemoveMappedDisplayCharacters functions.
 *	Use inherited instead of TextImager as base class a few places. Fixup help strings a few places.
 *	
 *	Revision 2.42  1999/11/13 16:32:18  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.41  1999/06/28 15:05:24  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.40  1999/06/28 14:52:35  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.39  1999/05/03 22:04:56  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.38  1999/05/03 21:41:23  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.37  1999/03/22 16:57:11  lewis
 *	weaken assertion check in MultiRowTextImager::SetTopRowInWindow_() to avoid assertion error
 *	in markercover code, due to 'race'.
 *	
 *	Revision 2.36  1999/02/05 01:41:05  lewis
 *	Call ReValidateSubRowInTopLineInWindow () from MultiRowTextImager::GetTopRowReferenceInWindow ()
 *	to avoid bugs that occasionally came up in LogoVista TA/SBSMode where these were out of sync.
 *	I tested a little in LedIt and there was no discernable performance impact.
 *	
 *	Revision 2.35  1998/10/30 14:13:40  lewis
 *	Add arg to DrawRowSegments() method (NB: virtual)
 *	
 *	Revision 2.34  1998/07/24  00:59:55  lewis
 *	fix longstanding (since qLedFirstIndex->ZERO conversion) bug with GetIthRowReference() - off by ONE ...
 *	DOn't know how it took so long to find!
 *
 *	Revision 2.33  1998/06/03  01:30:26  lewis
 *	override SetPartition to ajdust cached/dervied fields.
 *
 *	Revision 2.32  1998/04/25  13:04:42  lewis
 *	*** empty log message ***
 *
 *	Revision 2.31  1998/04/08  01:33:24  lewis
 *	Lots of changes to accomodate new PartitioningTextImager design, where Partitions are objects
 *	which can be externally specified, and associated with an Imager.
 *	Due to historical reasons, this class doesn't fully support that design (since it stores info in the
 *	partition elements which is imager-specific).
 *	A future version of this class should lift that limitation.
 *
 *	Revision 2.30  1998/03/04  20:17:42  lewis
 *	*** empty log message ***
 *
 *	Revision 2.29  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.28  1997/09/29  14:33:27  lewis
 *	Lose qLedFirstIndex support.
 *
 *	Revision 2.27  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.26  1997/07/27  15:01:05  lewis
 *	Edit Docs
 *
 *	Revision 2.25  1997/07/23  23:04:09  lewis
 *	doc changes
 *
 *	Revision 2.24  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.23  1997/07/12  20:01:45  lewis
 *	qLedFirstIndex = 0 support.
 *	AutoDoc support
 *
 *	Revision 2.22  1997/06/24  03:27:06  lewis
 *	No more SetLayoutWidth() to override (spr#0450). Instead added
 *	InvalidateAllCaches() helper to call when layout width changed.
 *
 *	Revision 2.21  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1997/03/22  13:47:19  lewis
 *	provide GetMarkerPositionOfStartOfLastRowOfWindow () override.
 *	Cleanups for new erase code strategy.
 *	GetTotalRowsInWindow_ () and SetLayoutWidth_ (Led_Distance width) helpers.
 *
 *	Revision 2.19  1997/03/04  20:05:14  lewis
 *	DrawInterLineSpace () takes bool param instead of Led_Color to support new virtual EraseBackground() method.
 *
 *	Revision 2.18  1997/01/20  05:19:50  lewis
 *	*** empty log message ***
 *
 *	Revision 2.17  1997/01/10  03:00:44  lewis
 *	throw specifiers.
 *	And moved ForceAllRowsShowing support down to TextImager.
 *
 *	Revision 2.16  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.15  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1996/09/30  14:16:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1996/09/03  15:03:51  lewis
 *	Lose obsolete qFriendDeclarationSeemsToPreventLaterInlineWithoutHelp bug workaround
 *
 *	Revision 2.12  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.11  1996/05/23  19:22:38  lewis
 *	Use new Led_IncrementalFontSpecifcation.
 *
 *	Revision 2.10  1996/05/14  20:19:30  lewis
 *	Added HilightARectangle_ ()
 *
 *	Revision 2.9  1996/04/18  15:04:34  lewis
 *	qCacheTextMeasurementsForOneRow.
 *	override new PurgeUnneededMemory.
 *
 *	Revision 2.8  1996/02/26  18:39:12  lewis
 *	Draw/DrawRow/DrawInterlineSpace now take bool printing parameter.
 *
 *	Revision 2.7  1996/02/05  04:10:52  lewis
 *	Get rif of TextSTore_ arg to CTOR, and override of SpecifyTextStore - instead
 *	we have overrides of HookLosting/Gaining TextStore.
 *	And typedef inherited.
 *
 *	Revision 2.6  1996/01/22  05:09:16  lewis
 *	override new SetMarkerPositionOfStartOfWindow
 *	and privide implementation.
 *	Override TabletChangedMetrics to clear caches.
 *	Comment on source of obscure assert error (PeekAtRowStart()) that comes
 *	up occasionally on MFC).
 *
 *	Revision 2.5  1996/01/11  08:15:03  lewis
 *	Now when qAllowRowsThatAreLongerThan255 enabled, use a USHORT, instead
 *	of ULONG, since we now DO define this by default.
 *
 *	Revision 2.4  1995/12/15  02:13:54  lewis
 *	DrawRow () now takes an invalidRect arg.
 *
 *	Revision 2.3  1995/12/06  01:31:03  lewis
 *	RowReference must be a friend.
 *
 *	Revision 2.2  1995/10/09  22:19:50  lewis
 *	No more postreplace, and added DidUpdateText () toeplace it.
 *	On getrowstart calls, special case if row == 1, then dont force
 *	word wrap.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.17  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.16  1995/05/20  04:45:55  lewis
 *	Many changes. Redid interline space support. No longer setter. Get/with cache
 *	and calc routine. SPR 0269.
 *	WindowRect/GetImageRect/GetImageWidth name/semantics change - SPR0263.
 *	Redo cahced data for PMs - save MUCHO size. SPR 0265.
 *
 *	Revision 1.15  1995/05/18  08:08:27  lewis
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *	Keep extra field in MultiRowTextImager::MultiRowPartitionMarker for fInterlineSpace. SPR#0250.
 *
 *	Revision 1.14  1995/05/16  06:38:39  lewis
 *	Added GetSelectionWindowRegion ().
 *
 *	Revision 1.13  1995/05/15  02:54:39  lewis
 *	On SetInterLineSpace call - be sure to do
 *	GetOwner()->InvalidateTotalRowsInWindow().
 *
 *	Revision 1.12  1995/05/09  23:22:44  lewis
 *	fPixelHeightCacheValid used instead of fRowHeightCacheValid - lose extra bool
 *	SPR 0241.
 *	Lose unused withWhat parameter to Pre/PostReplace - SPR 0240.
 *	MultiRowTextImager::MultiRowPartitionMarker::Inv
 *	alidateCachenow calls GetOwner ()->InvalidateTotalRowsInWindow ();
 *	SPR 0242.
 *
 *	Revision 1.11  1995/05/08  03:04:17  lewis
 *	ScrollByIfRoom, ScrollByIfRoom_, ScrollSoShowing etc no longer
 *	return bool.
 *	Added AssureWholeWindowUsedIfNeeded/MultiRowTextImager::InvalidateTotalRowsInWindow.
 *	These implement new separate caches instead of AdjustBoudns doing everyting.
 *	SetImageWidth/SetWindowRect now inval caches.
 *	For Led SPR#0236.
 *
 *	Revision 1.10  1995/05/06  19:25:02  lewis
 *	Use Led_tChar instead of char for UNICIDE support - SPR 0232.
 *
 *	Revision 1.9  1995/05/05  19:42:36  lewis
 *	Added Get/SetForceAllRowsShowing ().
 *	Added MeasureSegmentWidth/MeasureSegmentHeight () methods.
 *
 *	Revision 1.8  1995/05/03  19:20:19  lewis
 *	Make RowReference stuff public instead of protected.
 *
 *	Revision 1.7  1995/05/03  19:09:02  lewis
 *	Added new SetInterLineSpace() method for MultiRowPartitionMarker.
 *
 *	Revision 1.6  1995/04/18  00:08:52  lewis
 *	Add overrides for new TextImager methods GetStartOfRowContainingP
 *	and GetEndOfRowContainingPos
 *
 *	Revision 1.5  1995/03/29  22:44:14  lewis
 *	Added CountRowDifference ().
 *
 *	Revision 1.4  1995/03/15  01:03:24  lewis
 *	Added DrawRow() and DrawInterLineSpace() virtual methods to make
 *	refine draw behavior in subclasses easier.
 *
 *	Revision 1.3  1995/03/13  03:16:48  lewis
 *	Added MultiRowTextImager::MultiRowPartitionMarker::GetInterLineSpace () const
 *	Added FillAColoredRectangle_ ().
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

#include	<climits>		// for UINT_MAX
#include	<cstring>

#include	"PartitioningTextImager.h"
#include	"LineBasedPartition.h"



#if		qLedUsesNamespaces
namespace	Led {
#endif







/*
@CLASS:			MultiRowTextImager
@BASES:			@'PartitioningTextImager'
@DESCRIPTION:
	 <p>A @'PartitioningTextImager' which supports the notion of having multiple rows of text per line.</p>
	 <p>NB:	Although this class properly supports the APIs for @'PartitioningTextImager', it has a
	limitation, based on its historical implementation. It assumes that there is a one-to-one
	correspondance between its Partition and the TextImager, and that this Partition is (or subclasses from
	@'MultiRowTextImager::MultiRowPartition').</p>
		<p>This limitation is partly (largely) based on the fact that we use the trick of piggybacking row-wrap
	information on the partition markers themselves. A future version of this class might keep the per-row information
	in a MarkerCover, or some such other data structure.</p>
*/
class	MultiRowTextImager : public PartitioningTextImager {
	protected:
		MultiRowTextImager ();
		virtual ~MultiRowTextImager ();

	private:
		MultiRowTextImager (const MultiRowTextImager&);				// don't call. not implemented
		nonvirtual	void	operator= (const MultiRowTextImager&);	// don't call. not implemented

	private:
		typedef	PartitioningTextImager	inherited;

	protected:
		override	void	HookLosingTextStore ();
		nonvirtual	void	HookLosingTextStore_ ();
		override	void	HookGainedNewTextStore ();
		nonvirtual	void	HookGainedNewTextStore_ ();

	public:
		class	PartitionElementCacheInfo;

	protected:
		override	void	SetPartition (const PartitionPtr& partitionPtr);

	public:
		override	PartitionPtr	MakeDefaultPartition () const;

	public:
		/*
		@CLASS:			MultiRowTextImager::RowReference
		@DESCRIPTION:
			 <p>A utility class to represent a row. It is a struct with a parition marker, and a row number.
			 These things are NOT long-lived. And shouldn't be saved anyplace, as no attempt is made to keep
			 them automaticlly up to date as the text is modified.
			 They are just a convenient, short-hand way to navigate through rows of text.</p>
		*/
		class	RowReference {
			public:
				RowReference (const RowReference& from);
				RowReference (PartitionMarker* partitionMarker, size_t subRow);
			private:
				RowReference ();	// left undefined to assure never called...
			public:
				nonvirtual	RowReference&	operator= (const RowReference& rhs);

			public:
				nonvirtual	PartitionMarker*	GetPartitionMarker () const;
				nonvirtual	size_t				GetSubRow () const;
			private:
				PartitionMarker*	fPartitionMarker;
				size_t				fSubRow;
		};
		friend	class	RowReference;
		friend	bool	operator== (RowReference lhs, RowReference rhs);
		friend	bool	operator!= (RowReference lhs, RowReference rhs);

	public:
		nonvirtual	PartitionElementCacheInfo	GetPartitionElementCacheInfo (Partition::PartitionMarker* pm) const;
		nonvirtual	PartitionElementCacheInfo	GetPartitionElementCacheInfo (RowReference row) const;

	private:
		class	PMInfoCacheMgr;
		#if		qAutoPtrBrokenBug
			Led_RefCntPtr<PMInfoCacheMgr>	fPMCacheMgr;
		#else
			auto_ptr<PMInfoCacheMgr>		fPMCacheMgr;
		#endif


	// Row Reference support routines...
	public:
		nonvirtual	bool	GetNextRowReference (RowReference* adjustMeInPlace) const;		// return true if there is a next, and false if at end
		nonvirtual	bool	GetPreviousRowReference (RowReference* adjustMeInPlace) const;	// return true if there is a previous, and false if at the beginning

		// NB: if ith==1, that means do NOTHING - for convenience...
		nonvirtual	bool			GetIthRowReferenceFromHere (RowReference* adjustMeInPlace, long ith) const;		// return true if there is an ith, and false if we run off end... (ith==0 implies no change, < 0 means go back)
		nonvirtual	RowReference	GetIthRowReferenceFromHere (RowReference fromHere, long ith) const;				// ERROR if ith doesn't exist... (ith==0 implies no change, < 0 means go back)
		nonvirtual	RowReference	GetIthRowReference (size_t ith) const;												// ERROR if ith doesn't exist...(1 th is first row)

		nonvirtual	size_t	GetRowNumber (RowReference rowRef) const;	// Use of row numbers is discouraged, but this routine
																		// can be helpful in implementing those APIs anyhow

		nonvirtual	size_t	CountRowDifference (RowReference lhs, RowReference rhs) const;
		nonvirtual	size_t	CountRowDifferenceLimited (RowReference lhs, RowReference rhs, size_t limit) const;

	/*
	 *	Window/Scrolling support.
	 */
	public:
		override	size_t		GetTopRowInWindow () const;
		override	size_t		GetTotalRowsInWindow () const;
		override	size_t		GetLastRowInWindow () const;
		override	void		SetTopRowInWindow (size_t newTopRow);
		override	size_t		GetMarkerPositionOfStartOfWindow () const;
		override	size_t		GetMarkerPositionOfEndOfWindow () const;
		override	size_t		GetMarkerPositionOfStartOfLastRowOfWindow () const;
		override	long		CalculateRowDeltaFromCharDeltaFromTopOfWindow (long deltaChars) const;
		override	long		CalculateCharDeltaFromRowDeltaFromTopOfWindow (long deltaRows) const;
		override	void		ScrollByIfRoom (long downByRows);		// if downBy negative then up
																		// OK to ask to scroll further
																		// than allowed - return true
																		// if any scrolling (not necesarily
																		// same amont requested) done
	public:
		override	void		ScrollSoShowing (size_t markerPos, size_t andTryToShowMarkerPos = 0);

	protected:
		nonvirtual	RowReference	GetTopRowReferenceInWindow () const;
		nonvirtual	RowReference	GetLastRowReferenceInWindow () const;
		virtual		void			SetTopRowInWindow (RowReference row);

	protected:
		nonvirtual	void			SetTopRowInWindow_ (RowReference row);	// just sets the fields without any hook functions
																			// getting called. This is important sometimes when
																			// it would be unsafe for subclasses to get a chance
																			// to call methods while our data structures are not
																			// not completely up-to-date.

	protected:
		override	void		AssureWholeWindowUsedIfNeeded ();


	public:
		override	Led_Rect	GetCharLocation (size_t afterPosition)	const;
		override	size_t		GetCharAtLocation (const Led_Point& where) const;
		override	Led_Rect	GetCharWindowLocation (size_t afterPosition)	const;
		override	size_t		GetCharAtWindowLocation (const Led_Point& where) const;

		override	size_t			GetStartOfRow (size_t rowNumber) const;
		override	size_t			GetStartOfRowContainingPosition (size_t charPosition) const;
		override	size_t			GetEndOfRow (size_t rowNumber) const;
		override	size_t			GetEndOfRowContainingPosition (size_t charPosition) const;
		override	size_t			GetRealEndOfRow (size_t rowNumber) const;
		override	size_t			GetRealEndOfRowContainingPosition (size_t charPosition) const;
		override	size_t			GetRowContainingPosition (size_t charPosition) const;
		override	size_t			GetRowCount () const;
		override	Led_Rect		GetCharLocationRowRelativeByPosition (size_t afterPosition, size_t positionOfTopRow, size_t maxRowsToCheck)	const;

	public:
		nonvirtual	size_t			GetStartOfRow (RowReference row) const;
		nonvirtual	size_t			GetEndOfRow (RowReference row) const;
		nonvirtual	size_t			GetRealEndOfRow (RowReference row) const;
		nonvirtual	RowReference	GetRowReferenceContainingPosition (size_t charPosition) const;
		nonvirtual	size_t			GetRowLength (RowReference row) const;

	public:
		override	Led_Distance	GetRowHeight (size_t rowNumber)	const;
		nonvirtual	Led_Distance	GetRowHeight (RowReference row) const;

	public:
		override	Led_Distance	GetRowRelativeBaselineOfRowContainingPosition (size_t charPosition)	const;

	public:
		nonvirtual	Led_Distance	GetHeightOfRows (size_t startingRow, size_t rowCount)	const;
		nonvirtual	Led_Distance	GetHeightOfRows (RowReference startingRow, size_t rowCount)	const;

	public:
		override	void	GetStableTypingRegionContaingMarkerRange (size_t fromMarkerPos, size_t toMarkerPos,
								size_t* expandedFromMarkerPos, size_t * expandedToMarkerPos) const;

	public:
		override	void	Draw (const Led_Rect& subsetToDraw, bool printing);

	public:
		virtual		void	DrawPartitionElement (PartitionMarker* pm, size_t startSubRow, size_t maxSubRow, Led_Tablet tablet, OffscreenTablet* offscreenTablet, bool printing, const Led_Rect& subsetToDraw, Led_Rect* remainingDrawArea, size_t* rowsDrawn);

	protected:
		virtual	Led_Rect	GetCharLocationRowRelative (size_t afterPosition, RowReference topRow, size_t maxRowsToCheck = UINT_MAX)	const;
		virtual	size_t		GetCharAtLocationRowRelative (const Led_Point& where, RowReference topRow, size_t maxRowsToCheck = UINT_MAX) const;

	protected:
		virtual	void			FillCache (PartitionMarker* pm, PartitionElementCacheInfo& cacheInfo)									=	0;
		virtual	Led_Distance	CalculateInterLineSpace (const PartitionMarker* pm) const;


	protected:
		override	bool	ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const;
		override	size_t	RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const;

#if 0
	// Hook to invalidate cached info based on fontmetrics
	public:
		override	void	SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont);
	protected:
		nonvirtual	void	SetDefaultFont_ (const Led_IncrementalFontSpecification& /*defaultFont*/);	// Merely invalidates font metrics
#endif

	// To assure our top-line scroll info not left corrupt...
	protected:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();


	// override to invalidate caches.
	public:
		override	void		SetWindowRect (const Led_Rect& windowRect);

	protected:
		override	void		InvalidateAllCaches ();


	private:
		nonvirtual	RowReference	AdjustPotentialTopRowReferenceSoWholeWindowUsed (const RowReference& potentialTopRow);
		nonvirtual	bool			PositionWouldFitInWindowWithThisTopRow (size_t markerPos, const RowReference& newTopRow);

	private:
		PartitionMarker*	fTopLinePartitionMarkerInWindow;
		size_t				fSubRowInTopLineInWindow;
	private:
		nonvirtual	void	ReValidateSubRowInTopLineInWindow ();


	// Support for GetTotalRowsInWindow
	//
	// Override ComputeRowsThatWouldFitInWindowWithTopRow () to change the policy of how we
	// pack rows into a window
	private:
		mutable	size_t	fTotalRowsInWindow;		// zero means invalid cached - fill cache on call to GetTotalRowsInWindow
	protected:
		nonvirtual	size_t	GetTotalRowsInWindow_ () const;
		nonvirtual	void	InvalidateTotalRowsInWindow ();
		virtual		size_t	ComputeRowsThatWouldFitInWindowWithTopRow (const RowReference& newTopRow) const;

	#if		qSupportLed30CompatAPI
		protected:
			typedef	LineBasedPartition	MultiRowPartition;
		public:
			typedef	Partition::PartitionMarker	MultiRowPartitionMarker;
	#endif
	private:
		friend	class	PMInfoCacheMgr;
};




/*
@CLASS:			MultiRowTextImager::PartitionElementCacheInfo
@DESCRIPTION:
	 <p></p>
*/
class	MultiRowTextImager::PartitionElementCacheInfo {
	public:
		PartitionElementCacheInfo ();

	public:
		nonvirtual	Led_Distance	GetPixelHeight () const;
		nonvirtual	size_t			GetRowCount () const;
		nonvirtual	size_t			PeekRowCount () const;
		nonvirtual	size_t			GetLastRow () const;
		nonvirtual	Led_Distance	GetRowHeight (size_t ithRow) const;
		nonvirtual	size_t			GetLineRelativeRowStartPosition (size_t ithRow) const;
		nonvirtual	size_t			LineRelativePositionInWhichRow (size_t charPos) const;	// ZERO based charPos - ie zero is just before first byte in first row

	public:
		nonvirtual	Led_Distance	GetInterLineSpace () const;
		nonvirtual	void			SetInterLineSpace (Led_Distance interlineSpace);


	/*
 	 *	Word wrapping helper routine.
	 */
	public:
		nonvirtual	void	Clear ();
		nonvirtual	void	IncrementRowCountAndFixCacheBuffers (size_t newStart, Led_Distance newRowsHeight);


	// These should only be modifed in the FillCache () routine (or its overrides in subclasses)....
	public:
		// Note - calling these routines we assert i >= 0, <= fRowCountCache - to increase size of cache
		// call IncrementRowCountAndFixCacheBuffers ()
		nonvirtual	Led_Distance	PeekAtRowHeight (size_t ithRow) const;
		nonvirtual	void			SetRowHeight (size_t i, Led_Distance rowHeight);

		nonvirtual	size_t			PeekAtRowStart (size_t i) const;
		nonvirtual	void			SetRowStart (size_t i, size_t rowStart);		// NB: rowStart[1] MUST BE ZERO!!!!

	private:
		#if		qAllowRowsThatAreLongerThan255
			typedef	unsigned	short	RowHeight_;
		#else
			typedef	unsigned	char	RowHeight_;
		#endif
		// Don't yet support packed RowStart_ ONLY because we keep array of starts, not nChars in row.
		// If we switch to that - then we can use unsigned char for this as with the rowHeight guy!!!
		// LGP 950519
		typedef	size_t				RowStart_;
		enum	{ 	kPackRowStartCount	=	sizeof (RowStart_*)/sizeof (RowStart_)		};
		enum	{	kPackRowHeightCount	=	sizeof (RowHeight_*)/sizeof (RowHeight_)	};

	private:
		struct	Rep {
			public:
				Rep ();
				~Rep ();

			public:
				Led_Distance	fInterlineSpace;
				Led_Distance	fPixelHeightCache;
				size_t			fRowCountCache;
				RowStart_*		fRowStartArray;
				RowHeight_*		fRowHeightArray;

			public:
				LED_DECLARE_USE_BLOCK_ALLOCATION (Rep);

			private:
				Rep (const Rep&);				//	intentionally not defined
				void operator= (const Rep&);	//	ditto
		};
	private:
		Led_RefCntPtr<Rep>	fRep;

	private:
		friend	struct	Rep;
		friend	class	Led_RefCntPtr<Rep>;
};






/*
@CLASS:			MultiRowTextImager::PMInfoCacheMgr
@BASES:			@'Partition::PartitionWatcher'
@DESCRIPTION:
	 <p></p>
*/
class	MultiRowTextImager::PMInfoCacheMgr : public Partition::PartitionWatcher {
	private:
		typedef	Partition::PartitionWatcher	inherited;

	public:
		PMInfoCacheMgr (MultiRowTextImager& imager);
		~PMInfoCacheMgr ();

	public:
		nonvirtual	MultiRowTextImager::PartitionElementCacheInfo	GetPartitionElementCacheInfo (Partition::PartitionMarker* pm) const;

	public:
		nonvirtual	void	ClearCache ();

	public:
		override	void	AboutToSplit (PartitionMarker* pm, size_t at, void** infoRecord) const throw ();
		override	void	DidSplit (void* infoRecord) const throw ();
		override	void	AboutToCoalece (PartitionMarker* pm, void** infoRecord) const throw ();
		override	void	DidCoalece (void* infoRecord) const throw ();


	private:
		nonvirtual	void	MyMarkerDidUpdateCallback ();

	private:
		class	MyMarker;
		mutable	map<PartitionMarker*,PartitionElementCacheInfo>	fPMCache;
		mutable	PartitionMarker*								fCurFillCachePM;
		mutable	PartitionElementCacheInfo						fCurFillCacheInfo;
		MultiRowTextImager&										fImager;
		#if		qAutoPtrBrokenBug
			Led_RefCntPtr<MyMarker>								fMyMarker;
		#else
			auto_ptr<MyMarker>									fMyMarker;
		#endif
	private:
		friend	class	MyMarker;
};



/*
@CLASS:			MultiRowTextImager::PMInfoCacheMgr::MyMarker
@CLASS:			Marker
@ACCESS:		private
@DESCRIPTION:
	 <p></p>
*/
class	MultiRowTextImager::PMInfoCacheMgr::MyMarker : public Marker {
	private:
		typedef	Marker	inherited;

	public:
		MyMarker (PMInfoCacheMgr& pmInfoCacheMgr);

	protected:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	private:
		PMInfoCacheMgr&		fPMInfoCacheMgr;
};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
// defined out of order cuz used earlier
	inline	MultiRowTextImager::RowReference::RowReference (PartitionMarker* partitionMarker, size_t subRow):
		fPartitionMarker (partitionMarker),
		fSubRow (subRow)
		{
		}
	inline	void	MultiRowTextImager::InvalidateTotalRowsInWindow ()
		{
			fTotalRowsInWindow = 0;	// zero is sentinal meaning invalid
		}
	inline	MultiRowTextImager::RowReference	MultiRowTextImager::GetTopRowReferenceInWindow () const
		{
			Led_RequireNotNil (PeekAtTextStore ());	//	Must associate textstore before we can ask for row-references
			Led_EnsureNotNil (fTopLinePartitionMarkerInWindow);
			const_cast<MultiRowTextImager*>(this)->ReValidateSubRowInTopLineInWindow ();
			return (RowReference (fTopLinePartitionMarkerInWindow, fSubRowInTopLineInWindow));
		}
	inline	size_t	MultiRowTextImager::GetTotalRowsInWindow_ () const
		{
			if (fTotalRowsInWindow == 0) {	// cached value invalid
				fTotalRowsInWindow = ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ());
			}
			Led_Assert (fTotalRowsInWindow >= 1);	// always have at least one row...
			Led_Assert (fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));
			return (fTotalRowsInWindow);
		}




//	class	MultiRowTextImager::PartitionElementCacheInfo::Rep
	inline	MultiRowTextImager::PartitionElementCacheInfo::Rep::Rep ():
		fPixelHeightCache (Led_Distance (-1)),
		fInterlineSpace (0),
		fRowCountCache (0),
		fRowStartArray (NULL),
		fRowHeightArray (NULL)
		{
		}
	inline	MultiRowTextImager::PartitionElementCacheInfo::Rep::~Rep ()
		{
			if (fRowCountCache > kPackRowStartCount + 1) {
				delete[] fRowStartArray;
			}
			if (fRowCountCache > kPackRowHeightCount) {
				delete[] fRowHeightArray;
			}
		}


//	class	MultiRowTextImager::PartitionElementCacheInfo
	inline	MultiRowTextImager::PartitionElementCacheInfo::PartitionElementCacheInfo ():
		fRep (new Rep ())
		{
		}
	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::GetInterLineSpace
	@DESCRIPTION:	<p>Get the interline space associated with a partition. Each partition can
		have a unique interline space. These are computed by overriding the
		MultiRowTextImager::CalculateInterLineSpace () method. They are not set directly.
		</p>
	*/
	inline	Led_Distance	MultiRowTextImager::PartitionElementCacheInfo::GetInterLineSpace () const
		{
			Led_Assert (fRep->fInterlineSpace != Led_Distance (-1));
			return (fRep->fInterlineSpace);
		}
	inline	void	MultiRowTextImager::PartitionElementCacheInfo::SetInterLineSpace (Led_Distance interlineSpace)
		{
			Led_Assert (interlineSpace != Led_Distance (-1));
			fRep->fInterlineSpace = interlineSpace;
		}

	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::GetPixelHeight
	@DESCRIPTION:	<p>Return the cached height of the given partition element. This is the sum of the
		pixel height computed in FillCache () for the actual text, and the interline space (see GetInterLineSpace ()).</p>
	*/
	inline	Led_Distance	MultiRowTextImager::PartitionElementCacheInfo::GetPixelHeight () const
		{
			return (fRep->fPixelHeightCache + GetInterLineSpace ());
		}
	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::GetRowCount
	@DESCRIPTION:	<p>Return the cached number of rows in this partition element. This is computed in
		MultiRowTextImager::FillCache (), and specified indirectly via calls (from inside FillCache)
		to MultiRowTextImager::MultiRowPartitionMarker::IncrementRowCountAndFixCacheBuffers ().</p>
	*/
	inline	size_t	MultiRowTextImager::PartitionElementCacheInfo::GetRowCount () const
		{
			Led_Assert (fRep->fRowCountCache >= 1);	// even for empty lines we have 1 row (by definition)
			return (fRep->fRowCountCache);
		}
	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::PeekRowCount
	@DESCRIPTION:	<p>Return the cached number of rows in this partition element. This is computed in
		MultiRowTextImager::FillCache (), and specified indirectly via calls (from inside FillCache)
		to MultiRowTextImager::MultiRowPartitionMarker::IncrementRowCountAndFixCacheBuffers ().</p>
	*/
	inline	size_t	MultiRowTextImager::PartitionElementCacheInfo::PeekRowCount () const
		{
			return (fRep->fRowCountCache);
		}
	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::GetLastRow
	@DESCRIPTION:	<p>Return the last valid row index (may invoke FillCache if cached result not
		already available).</p>
	*/
	inline	size_t		MultiRowTextImager::PartitionElementCacheInfo::GetLastRow () const
		{
			return GetRowCount () - 1;
		}
	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowHeight
	@DESCRIPTION:	<p>Return the height - (in whatever unit the GDI is using, but typically pixels) of the given row.
		The internal representation of these things is somewhat obscure for data size reasons, so there is some
		unpacking to be done.</p>
	*/
	inline	Led_Distance	MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowHeight (size_t i) const
		{
			Led_Assert (i < fRep->fRowCountCache);	// MFC Hint - when this assert fails, look closely at your
													// stack-trace - often its cuz some other assert failed in the context
													// of a FillCache, and so the cache info isn't completely filled in
													// yet...
			/*
			 *	A bit of trickery. --- XPLAIN
			 */
			if (fRep->fRowCountCache <= kPackRowHeightCount) {
				// Then we use the pointer to the array as the actual array
				const RowHeight_*	theArray	=	reinterpret_cast<const RowHeight_*> (&fRep->fRowHeightArray);
				return (theArray[i]);
			}
			Led_AssertNotNil (fRep->fRowHeightArray);
			return (fRep->fRowHeightArray[i]);
		}
	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::SetRowHeight
	@DESCRIPTION:	<p>Set the height of a given row. This is typically just done within FillCache().</p>
	*/
	inline	void			MultiRowTextImager::PartitionElementCacheInfo::SetRowHeight (size_t i, Led_Distance rowHeight)
		{
			Led_Assert (i < fRep->fRowCountCache);
			Led_Assert (sizeof (RowHeight_) > 1 or rowHeight <= 0xff);		// be sure value fits..
			Led_Assert (sizeof (RowHeight_) > 2 or rowHeight <= 0xffff);	// be sure value fits.
			/*
			 *	A bit of trickery. --- XPLAIN
			 */
			if (fRep->fRowCountCache <= kPackRowHeightCount) {
				// Then we use the pointer to the array as the actual array
				RowHeight_*	theArray	=	reinterpret_cast<RowHeight_*> (&fRep->fRowHeightArray);
				theArray[i] = RowHeight_ (rowHeight);
			}
			else {
				Led_AssertNotNil (fRep->fRowHeightArray);
				fRep->fRowHeightArray[i] = RowHeight_ (rowHeight);
			}
		}
	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowStart
	@DESCRIPTION:	<p>Return the partition element relative offset of the start of a given row. So for the
		first row, this is always zero.</p>
	*/
	inline	size_t			MultiRowTextImager::PartitionElementCacheInfo::PeekAtRowStart (size_t i) const
		{
			Led_Assert (i < fRep->fRowCountCache);

			if (i == 0) {
				return (0);
			}
			else {
				/*
				 *	A bit of trickery. --- XPLAIN
				 */
				Led_Assert (i >= 1);
				if (fRep->fRowCountCache <= kPackRowStartCount + 1) {
					// Then we use the pointer to the array as the actual array
					const RowStart_*	theArray	=	reinterpret_cast<const RowStart_*> (&fRep->fRowStartArray);
					return (theArray[i-1]);
				}
				Led_AssertNotNil (fRep->fRowStartArray);
				return (fRep->fRowStartArray[i-1]);
			}
		}
	/*
	@METHOD:		MultiRowTextImager::PartitionElementCacheInfo::SetRowStart
	@DESCRIPTION:	<p>Set the partition element relative offset of the start of a given row. So for the
		first row, this is MUST BE zero. This is typically just called during FillCache ().</p>
	*/
	inline	void			MultiRowTextImager::PartitionElementCacheInfo::SetRowStart (size_t i, size_t rowStart)
		{
			Led_Assert (i < fRep->fRowCountCache);
			
			if (i == 0) {
				Led_Assert (rowStart == 0);
			}
			else {
				/*
				 *	A bit of trickery. --- XPLAIN
				 */
				Led_Assert (i >= 1);
				Led_Assert (sizeof (RowStart_) > 1 or rowStart <= 0xff);	// be sure value fits..
				Led_Assert (sizeof (RowStart_) > 2 or rowStart <= 0xffff);	// be sure value fits.
				if (fRep->fRowCountCache <= kPackRowStartCount + 1) {
					// Then we use the pointer to the array as the actual array
					RowStart_*	theArray	=	reinterpret_cast<RowStart_*> (&fRep->fRowStartArray);
					theArray[i-1] = RowStart_ (rowStart);
				}
				else {
					Led_AssertNotNil (fRep->fRowStartArray);
					fRep->fRowStartArray[i-1] = RowStart_ (rowStart);
				}
			}
		}
	inline	size_t	MultiRowTextImager::PartitionElementCacheInfo::GetLineRelativeRowStartPosition (size_t ithRow) const
		{
			return (PeekAtRowStart (ithRow));
		}
	inline	Led_Distance	MultiRowTextImager::PartitionElementCacheInfo::GetRowHeight (size_t ithRow) const
		{
			return (PeekAtRowHeight (ithRow));
		}
	inline	size_t		MultiRowTextImager::PartitionElementCacheInfo::LineRelativePositionInWhichRow (size_t charPos) const
		{
			//	ZERO based charPos - ie zero is just before first byte in first row
			//	Led_Require (charPos >= 0);	// yes I know this is a degenerate test - just for doc purposes...
			//	Led_Assert (charPos < OURLENGTH);
			for (size_t row = fRep->fRowCountCache; row >= 1; row--) {
				if (charPos >= PeekAtRowStart (row-1)) {
					return (row-1);
				}
			}
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma push
				#pragma warn -8008
			#endif
			Led_Assert (false);	return (0);	// if we get here - must have been before our line...
			#if		qSilenceAnnoyingCompilerWarnings && __BCPLUSPLUS__
				#pragma pop
			#endif
		}








//	class	MultiRowTextImager::RowReference
	inline	MultiRowTextImager::RowReference::RowReference (const RowReference& from):
		fPartitionMarker (from.fPartitionMarker),
		fSubRow (from.fSubRow)
		{
		}
	inline	MultiRowTextImager::RowReference&	MultiRowTextImager::RowReference::operator= (const MultiRowTextImager::RowReference& rhs)
		{
			fPartitionMarker = rhs.fPartitionMarker;
			fSubRow = rhs.fSubRow;
			return (*this);
		}
	inline	MultiRowTextImager::PartitionMarker*	MultiRowTextImager::RowReference::GetPartitionMarker () const
		{
			return (fPartitionMarker);
		}
	inline	size_t	MultiRowTextImager::RowReference::GetSubRow () const
		{
			return (fSubRow);
		}




//	class	MultiRowTextImager
	/*
	@METHOD:		MultiRowTextImager::GetNextRowReference
	@DESCRIPTION:	<p>Advance the given row reference argument to the next row. Return true if there
		is a valid next row. And false if <code>adjustMeInPlace</code> was already on the last row.</p>
			<p>See also @'MultiRowTextImager::GetPreviousRowReference'.</p>
	*/
	inline	bool	MultiRowTextImager::GetNextRowReference (RowReference* adjustMeInPlace) const
		{
			Led_RequireNotNil (adjustMeInPlace);
			PartitionMarker*			cur		=	adjustMeInPlace->GetPartitionMarker ();
			size_t						subRow	=	adjustMeInPlace->GetSubRow ();
			PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (cur);
			if (subRow+1 < pmCacheInfo.GetRowCount ()) {
				subRow++;
				*adjustMeInPlace = RowReference (cur, subRow);
				return true;
			}
			else {
				if (cur->GetNext () == NULL) {
					return false;
				}
				else {
					cur = cur->GetNext ();
					subRow = 0;
					*adjustMeInPlace = RowReference (cur, subRow);
					return true;
				}
			}
		}
	/*
	@METHOD:		MultiRowTextImager::GetPreviousRowReference
	@DESCRIPTION:	<p>Retreat the given row reference argument to the previous row. Return true if there
		is a valid previous row. And false if <code>adjustMeInPlace</code> was already on the first row.</p>
			<p>See also @'MultiRowTextImager::GetNextRowReference'.</p>
	*/
	inline	bool	MultiRowTextImager::GetPreviousRowReference (RowReference* adjustMeInPlace) const
		{
			Led_AssertNotNil (adjustMeInPlace);
			PartitionMarker*	cur		=	adjustMeInPlace->GetPartitionMarker ();
			size_t				subRow	=	adjustMeInPlace->GetSubRow ();
			if (subRow > 0) {
				subRow--;
				*adjustMeInPlace = RowReference (cur, subRow);
				return true;
			}
			else {
				if (cur->GetPrevious () == NULL) {
					return false;
				}
				else {
					cur = cur->GetPrevious ();
					PartitionElementCacheInfo	pmCacheInfo	=	GetPartitionElementCacheInfo (cur);
					subRow = pmCacheInfo.GetRowCount () - 1;
					*adjustMeInPlace = RowReference (cur, subRow);
					return true;
				}
			}
		}
	/*
	@METHOD:		MultiRowTextImager::GetIthRowReferenceFromHere
	@DESCRIPTION:	<p>Adjust <code>fromHere</code> by <code>ith</code> rows. If <code>ith</code> is zero, then
		this returns <code>fromHere</code>. It asserts there are at least ith rows from the given one to retrieve.
		It calls @'MultiRowTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
			<p>See also @'MultiRowTextImager::GetNextRowReference', @'MultiRowTextImager::GetPreviousRowReference'.</p>
	*/
	inline	MultiRowTextImager::RowReference	MultiRowTextImager::GetIthRowReferenceFromHere (RowReference fromHere, long ith) const
		{
			bool	result	=	GetIthRowReferenceFromHere (&fromHere, ith);
			Led_Assert (result);
			return (fromHere);
		}
	/*
	@METHOD:		MultiRowTextImager::GetIthRowReference
	@DESCRIPTION:	<p>Get the <code>ith</code> row reference in the document. Asserts value <code>ith</code> refers to
		a valid row number.</p>
			<p>It calls @'MultiRowTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
	*/
	inline	MultiRowTextImager::RowReference	MultiRowTextImager::GetIthRowReference (size_t ith) const
		{
			RowReference	fromHere (GetFirstPartitionMarker (), 0);
			bool	result	=	GetIthRowReferenceFromHere (&fromHere, ith);
			Led_Assert (result);
			return (fromHere);
		}
	/*
	@METHOD:		MultiRowTextImager::GetRowLength
	@DESCRIPTION:	<p>Gets the length of the given row (in @'Led_tChar's).</p>
			<p>See also  @'MultiRowTextImager::GetStartOfRow' and @'MultiRowTextImager::GetEndOfRow'.</p>
	*/
	inline	size_t	MultiRowTextImager::GetRowLength (RowReference row) const
		{
			return (GetEndOfRow (row) - GetStartOfRow (row));
		}
	/*
	@METHOD:		MultiRowTextImager::GetLastRowReferenceInWindow
	@DESCRIPTION:	<p>Returns the last row-reference in the window (end of window).</p>
	*/
	inline	MultiRowTextImager::RowReference	MultiRowTextImager::GetLastRowReferenceInWindow () const
		{
			RowReference	row	=	GetTopRowReferenceInWindow ();
			Led_Assert (GetTotalRowsInWindow_ () >= 1);
			(void)GetIthRowReferenceFromHere (&row, GetTotalRowsInWindow_ () - 1);
			return (row);
		}
	inline	void	MultiRowTextImager::SetTopRowInWindow_ (RowReference row)
		{
			fTopLinePartitionMarkerInWindow = row.GetPartitionMarker ();
			fSubRowInTopLineInWindow = row.GetSubRow ();
			Led_AssertNotNil (fTopLinePartitionMarkerInWindow);
			InvalidateTotalRowsInWindow ();
		}
	/*
	@METHOD:		MultiRowTextImager::GetRowHeight
	@DESCRIPTION:	<p>Returns the height (in standard GDI units, usually pixels) of the given row reference.</p>
	*/
	inline	Led_Distance	MultiRowTextImager::GetRowHeight (RowReference row) const
		{
			Led_AssertNotNil (row.GetPartitionMarker ());
			return GetPartitionElementCacheInfo (row.GetPartitionMarker ()).GetRowHeight (row.GetSubRow ());
		}







	inline	bool	operator== (MultiRowTextImager::RowReference lhs, MultiRowTextImager::RowReference rhs)	
		{
			return (lhs.GetPartitionMarker () == rhs.GetPartitionMarker () and
					lhs.GetSubRow () == rhs.GetSubRow ()
				);
		}
	inline	bool	operator!= (MultiRowTextImager::RowReference lhs, MultiRowTextImager::RowReference rhs)	
		{
			return (lhs.GetPartitionMarker () != rhs.GetPartitionMarker () or
					lhs.GetSubRow () != rhs.GetSubRow ()
				);
		}

#if		qLedUsesNamespaces
}
#endif

#endif	/*__MultiRowTextImager_h__*/


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
