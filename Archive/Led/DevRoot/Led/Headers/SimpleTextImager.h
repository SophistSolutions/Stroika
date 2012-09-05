/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__SimpleTextImager_h__
#define	__SimpleTextImager_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/SimpleTextImager.h,v 2.47 2004/01/26 16:42:03 lewis Exp $
 */

/*
@MODULE:	SimpleTextImager
@DESCRIPTION:
		<p>The SimpleTextImager module provides a very fast, lightweight text imaging implementation. This implementation
	does not support wordwrapping (or multiple rows per line (Partition element)</p>

 */


/*
 * Changes:
 *	$Log: SimpleTextImager.h,v $
 *	Revision 2.47  2004/01/26 16:42:03  lewis
 *	SPR#1604: Lose overrides of ComputeRelativePosition () since the speedtweek can be done directly in the TextImager::ComputeRelativePosition () version using GetStartOfNextRowFromRowContainingPosition () etc.
 *	
 *	Revision 2.46  2003/11/29 00:39:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.45  2003/01/29 17:59:47  lewis
 *	SPR#1264- Get rid of most of the SetDefaultFont overrides and crap - and made InteractiveSetFont REALLY do the interactive command setfont - and leave SetDefaultFont to just setting the 'default font'
 *	
 *	Revision 2.44  2002/12/21 03:00:00  lewis
 *	SPR#1217. Added virtual TextImager::InvalidateAllCaches and made SimpleTextImager/MRTI versions
 *	just overrides. No longer need SimpleTextImager::TabletChangedMetrics overrides etc
 *	since now TextImager::TabletChangedMetrics calls TextImager::InvalidateAllCaches.
 *	MultiRowTextImager::PurgeUnneededMemory () no longer needed since TextImager version
 *	now calls InvalidateAllCaches().
 *	
 *	Revision 2.43  2002/12/20 17:55:59  lewis
 *	SPR#1216- get rid of override of GetSelectionWindowRegion. New version in TextImager
 *	now properly handles BIDI, and interline space (interline space not tested,but
 *	not really used much anymore??).
 *	
 *	Revision 2.42  2002/12/02 15:57:51  lewis
 *	SPR#1185 - move DrawRow,DrawRowSegments,DrawRowHilight,DrawInterlineSpace from
 *	MRTI/SimpleTextImager to TextImager (share code. Changed their args slightly. For BIDI work.
 *	
 *	Revision 2.41  2002/12/02 00:52:42  lewis
 *	use TextLayoutBlock for arg to DrawRow etc functions - as I did with MultiRowTextImager - part of SPR#1183 - BIDI
 *	
 *	Revision 2.40  2002/10/25 17:41:27  lewis
 *	SPR#1152 - override SimpleTextImager::MakeDefaultPartition
 *	
 *	Revision 2.39  2002/09/20 15:06:14  lewis
 *	get rid of qTypedefsOfPrivateAccessNamesNotAllowedInSourceFile - wasn't really a bug
 *	
 *	Revision 2.38  2002/05/06 21:33:33  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.37  2001/11/27 00:29:44  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.36  2001/10/20 13:38:56  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.35  2001/10/17 21:46:41  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.34  2001/10/16 15:33:17  lewis
 *	SPR#1062- as a side-effect of this SPR - added default implementation of ComputeMaxHScrollPos ()
 *	
 *	Revision 2.33  2001/09/24 16:28:59  lewis
 *	SPR#0993- made InvalidateAllCaches virtual and override in WordProcessorHScrollbarHelper<BASECLASS>
 *	as part of attempt to fix quirky mac sbar code on threshold between showing/hiding scrollbars
 *	
 *	Revision 2.32  2001/08/28 18:43:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.31  2000/06/17 12:56:48  lewis
 *	implement override of TextImager::GetRowRelativeBaselineOfRowContainingPosition
 *	
 *	Revision 2.30  2000/04/15 14:32:35  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.29  2000/04/14 22:40:21  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.28  2000/03/11 22:04:48  lewis
 *	SPR#0711 - new GetRealEndOfRow* () routines. And fixed PositionWouldFitInWindowWithThisTopRow ()
 *	to call it. And other small fixes to TextInteractor::ScrollToSelection ()  and
 *	Simple/MultiRow::ScrollSoShowing ()
 *	
 *	Revision 2.27  2000/03/09 04:40:29  lewis
 *	replace old MultiRowTextImager::HilightARectanle_ (and SimpleTextImager version)
 *	with TextImager::HilightARectanlge() ETC - SPR#0708
 *	
 *	Revision 2.26  1999/12/21 21:27:52  lewis
 *	override ContainsMappedDisplayCharacters/RemoveMappedDisplayCharacters () so ::GetEndOfRow()
 *	code works right - knows we remove the \ns
 *	
 *	Revision 2.25  1999/11/13 16:32:18  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.24  1999/06/28 14:54:53  lewis
 *	cosmetic clenaup
 *	
 *	Revision 2.23  1999/06/28 14:53:28  lewis
 *	spr#0598- use mutable instead of const_cast, as appopropriate
 *	
 *	Revision 2.22  1999/05/03 22:04:58  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.21  1998/07/24 01:01:27  lewis
 *	GetInterlineSpace virutal/ChangedInterlineSpace virutal (for REMO).
 *	Docs.
 *	GetIthRowReferenceFromHEre() off by one fix
 *	
 *	Revision 2.20  1998/04/08  01:36:10  lewis
 *	Support new Partion design - change our base class to ParittioningTextImager instead of
 *	LineBasedTextImager (now obsolete),
 *	and create linebasedparition by default.
 *
 *	Revision 2.19  1998/03/04  20:17:32  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.17  1997/12/24  03:25:40  lewis
 *	Add SimpleTextImager::GetRowHeight/InvalidateRowHeght() API to make some coding tasks of Remo easier.
 *
 *	Revision 2.16  1997/10/01  01:56:31  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1997/09/29  14:35:45  lewis
 *	rewrote.
 *	Instead of subclassing from MultiRowTextImager, I injected all that code inline.
 *	Then stripped out (MOST OF)
 *	the multirow parts. Now it runs quite a bit smaller and faster.
 *
 *	Revision 2.14  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.13  1997/07/27  15:01:20  lewis
 *	Edit docs
 *
 *	Revision 2.12  1997/07/23  23:04:27  lewis
 *	doc changes
 *
 *	Revision 2.11  1997/07/12  20:02:57  lewis
 *	AutoDoc support.
 *
 *	Revision 2.10  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1997/03/04  20:03:11  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1997/01/10  03:02:41  lewis
 *	Quickie (but real, working) implementation. Just use Multirowtextimager with a FillCache() that
 *	does nothing.
 *	SHOULD sometime soon rewrite this so its much trimmer. And probably rename it, and perhaps
 *	make several related alternate imager classes.
 *
 *	Revision 2.7  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.6  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.4  1996/05/23  19:23:26  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/04/18  15:04:50  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/03/16  18:38:58  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.3  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
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

#include	<limits.h>		// for UINT_MAX
#include	<string.h>

#include	"PartitioningTextImager.h"




#if		qLedUsesNamespaces
namespace	Led {
#endif




/*
@CLASS:			SimpleTextImager
@BASES:			@'PartitioningTextImager'
@DESCRIPTION:
		<p>SimpleTextImager is a simple subclass of @'PartitioningTextImager'. NB: this is a change
	from Led 2.2, when as an implementation detail, it subclassed from MultiRowTextImager.</p>
		<p>SimpleTextImager assumes that all rows have the same height (specified by ReCalcRowHeight)
	and that all lines (paragraphs) are displayed in a single row. It is these assumptions that allow
	this class to display text with much less runtime overhead (both memory and time).</p>
		<p>You should be able to assign an arbitrary partition object to a @'SimpleTextImager' via
	the @'SimpleTextImager::SetPartition' function. And these partition objects should be sharable among
	multiple SimpleTextImager's.</p>
		<p>NB:	Much of this class is subject to change (mostly simplication). This code was originally
	based on the code for MultiRowTextImager, and as such, it has alot of generality built in
	which isn't appropriate (like assuming rows all same height allows alot to be simplfied in
	computations etc).</p>
		<p>Unless you must, avoid counting too much on the internals and details of this class.</p>
*/
class	SimpleTextImager : public PartitioningTextImager {
	private:
		typedef	PartitioningTextImager	inherited;

	protected:
		SimpleTextImager ();
		virtual ~SimpleTextImager ();

	private:
		SimpleTextImager (const SimpleTextImager&);					// don't call. not implemented
		nonvirtual	void	operator= (const SimpleTextImager&);	// don't call. not implemented

	protected:
		override	void	HookLosingTextStore ();
		nonvirtual	void	HookLosingTextStore_ ();
		override	void	HookGainedNewTextStore ();
		nonvirtual	void	HookGainedNewTextStore_ ();

	public:
		nonvirtual	void	SetPartition (const PartitionPtr& partitionPtr);
	private:
		bool	fICreatedPartition;

	public:
		override	PartitionPtr	MakeDefaultPartition () const;

	private:
		class	MyPartitionWatcher : public Partition::PartitionWatcher {
			public:
				nonvirtual	void	Init (PartitionPtr partition, SimpleTextImager* imager);
				nonvirtual	void	UnInit (PartitionPtr partition);
			public:
				override	void	AboutToSplit (PartitionMarker* pm, size_t at, void** infoRecord) const throw ();
				override	void	DidSplit (void* infoRecord) const throw ();
				override	void	AboutToCoalece (PartitionMarker* pm, void** infoRecord) const throw ();
				override	void	DidCoalece (void* infoRecord) const throw ();
			private:
				SimpleTextImager*	fImager;
		};
	private:
		friend	class	MyPartitionWatcher;
		MyPartitionWatcher	fMyPartitionWatcher;

	public:
		nonvirtual	Led_Distance	GetRowHeight () const;
	protected:
		nonvirtual	void			InvalidateRowHeight ();
		virtual		Led_Distance	ReCalcRowHeight () const;
	private:
		Led_Distance	fRowHeight;

	protected:
		override	Led_Distance	MeasureSegmentHeight (size_t from, size_t to) const;
		override	Led_Distance	MeasureSegmentBaseLine (size_t from, size_t to) const;

	protected:
		typedef	Partition::PartitionMarker	PartitionMarker;

	public:
		/*
		@CLASS:			SimpleTextImager::RowReference
		@DESCRIPTION:
				<p>A utility class to represent a row. It is a struct with a parition marker, and a row number.
			 These things are NOT long-lived. And shouldn't be saved anyplace, as no attempt is made to keep
			 them automaticlly up to date as the text is modified.
			 They are just a convenient, short-hand way to navigate through rows of text.</p>
				<p>NOTE - this RowReference stuff is just a design vestige from an earlier implementation. At some point,
			this module should be rewritten/cleaned up to eliminate this (LGP - 2001-10-20).
		*/
		class	RowReference {
			public:
				RowReference (const RowReference& from);
				RowReference (PartitionMarker* partitionMarker);
			private:
				RowReference ();	// left undefined to assure never called...
			public:
				nonvirtual	RowReference&	operator= (const RowReference& rhs);

			public:
				nonvirtual	PartitionMarker*	GetPartitionMarker () const;
			private:
				PartitionMarker*	fPartitionMarker;
		};

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

		// Count the # of rows from one rowreference to the other (order doesn't matter)
		nonvirtual	size_t	CountRowDifference (RowReference lhs, RowReference rhs) const;

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
		override	Led_Distance	ComputeMaxHScrollPos () const;

	public:
		override	Led_Rect	GetCharLocation (size_t afterPosition)	const;
		override	size_t		GetCharAtLocation (const Led_Point& where) const;
		override	Led_Rect	GetCharWindowLocation (size_t afterPosition)	const;
		override	size_t		GetCharAtWindowLocation (const Led_Point& where) const;

	public:
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

	public:
		nonvirtual	Led_Distance	GetInterLineSpace () const;
		nonvirtual	void			SetInterLineSpace (Led_Distance interlineSpace);
	private:
		Led_Distance	fInterlineSpace;

	public:
		virtual	Led_Distance	GetInterLineSpace (PartitionMarker* pm) const;
		virtual	void			ChangedInterLineSpace (PartitionMarker* pm);

	// Hook to invalidate cached info based on fontmetrics
	public:
		override	void	SetDefaultFont (const Led_IncrementalFontSpecification& defaultFont);

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

	protected:
		override	bool	ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const;
		override	size_t	RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const;

	private:
		friend	class	RowReference;
		friend	bool	operator== (RowReference lhs, RowReference rhs);
		friend	bool	operator!= (RowReference lhs, RowReference rhs);
};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
// defined out of order cuz used earlier
	inline	SimpleTextImager::RowReference::RowReference (PartitionMarker* partitionMarker):
		fPartitionMarker (partitionMarker)
		{
		}
	inline	void	SimpleTextImager::InvalidateTotalRowsInWindow ()
		{
			fTotalRowsInWindow = 0;	// zero is sentinal meaning invalid
		}
	inline	SimpleTextImager::RowReference	SimpleTextImager::GetTopRowReferenceInWindow () const
		{
			Led_RequireNotNil (PeekAtTextStore ());	//	Must associate textstore before we can ask for row-references
			Led_EnsureNotNil (fTopLinePartitionMarkerInWindow);
			return (RowReference (fTopLinePartitionMarkerInWindow));
		}
	inline	size_t	SimpleTextImager::GetTotalRowsInWindow_ () const
		{
			if (fTotalRowsInWindow == 0) {	// cached value invalid
				fTotalRowsInWindow = ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ());
			}
			Led_Assert (fTotalRowsInWindow >= 1);	// always have at least one row...
			Led_Assert (fTotalRowsInWindow == ComputeRowsThatWouldFitInWindowWithTopRow (GetTopRowReferenceInWindow ()));
			return (fTotalRowsInWindow);
		}






//	class	SimpleTextImager::RowReference
	inline	SimpleTextImager::RowReference::RowReference (const RowReference& from):
		fPartitionMarker (from.fPartitionMarker)
		{
		}
	inline	SimpleTextImager::RowReference&	SimpleTextImager::RowReference::operator= (const SimpleTextImager::RowReference& rhs)
		{
			fPartitionMarker = rhs.fPartitionMarker;
			return (*this);
		}
	inline	SimpleTextImager::PartitionMarker*	SimpleTextImager::RowReference::GetPartitionMarker () const
		{
			return (fPartitionMarker);
		}




//	class	SimpleTextImager
	/*
	@METHOD:		SimpleTextImager::GetRowHeight
	@DESCRIPTION:	<p>Gets the row height for all rows in the Imager. Note that for SimpleTextImager, all rows have
				the same height. This function caches the value (computed by @'SimpleTextImager::ReCalcRowHeight' ()). To cause
				the value to be recomputed, call @'SimpleTextImager::InvalidateRowHeight' (). This is automatically done for you
				from </p>
	*/
	inline	Led_Distance	SimpleTextImager::GetRowHeight () const
		{
			if (fRowHeight == Led_Distance (-1)) {
				// use mutable when available
				const_cast<SimpleTextImager*> (this)->fRowHeight = ReCalcRowHeight ();
			}
			Led_Ensure (fRowHeight > 0);
			Led_Ensure (fRowHeight != Led_Distance (-1));
			return fRowHeight;
		}
	/*
	@METHOD:		SimpleTextImager::InvalidateRowHeight
	@DESCRIPTION:	<p>Note that the rowHeight associated with this TextImager is invalid. Next time the value is
				requested (via @'SimpleTextImager::GetRowHeight' ()), recalculate it first.</p>
	*/
	inline	void	SimpleTextImager::InvalidateRowHeight ()
		{
			fRowHeight = Led_Distance (-1);
		}
	/*
	@METHOD:		SimpleTextImager::GetNextRowReference
	@DESCRIPTION:	<p>Advance the given row reference argument to the next row. Return true if there
				is a valid next row. And false if <code>adjustMeInPlace</code> was already on the last row.</p>
					<p>See also @'SimpleTextImager::GetPreviousRowReference'.</p>
	*/
	inline	bool	SimpleTextImager::GetNextRowReference (RowReference* adjustMeInPlace) const
		{
			Led_RequireNotNil (adjustMeInPlace);
			PartitionMarker*	cur		=	adjustMeInPlace->GetPartitionMarker ();
			size_t				subRow	=	0;
		
			if (subRow+1 < 1) {
				subRow++;
				*adjustMeInPlace = RowReference (cur);
				return true;
			}
			else {
				if (cur->GetNext () == NULL) {
					return false;
				}
				else {
					cur = cur->GetNext ();
					subRow = 0;
					*adjustMeInPlace = RowReference (cur);
					return true;
				}
			}
		}
	/*
	@METHOD:		SimpleTextImager::GetPreviousRowReference
	@DESCRIPTION:	<p>Retreat the given row reference argument to the previous row. Return true if there
				is a valid previous row. And false if <code>adjustMeInPlace</code> was already on the first row.</p>
					<p>See also @'SimpleTextImager::GetNextRowReference'.</p>
	*/
	inline	bool	SimpleTextImager::GetPreviousRowReference (RowReference* adjustMeInPlace) const
		{
			Led_AssertNotNil (adjustMeInPlace);
			PartitionMarker*	cur		=	adjustMeInPlace->GetPartitionMarker ();
			size_t						subRow	=	0;
		
			if (subRow > 0) {
				subRow--;
				*adjustMeInPlace = RowReference (cur);
				return true;
			}
			else {
				if (cur->GetPrevious () == NULL) {
					return false;
				}
				else {
					cur = cur->GetPrevious ();
					subRow = 0;
					*adjustMeInPlace = RowReference (cur);
					return true;
				}
			}
		}
	/*
	@METHOD:		SimpleTextImager::GetIthRowReferenceFromHere
	@DESCRIPTION:	<p>Adjust <code>fromHere</code> by <code>ith</code> rows. If <code>ith</code> is zero, then
				this returns <code>fromHere</code>. It asserts there are at least ith rows from the given one to retrieve.
				It calls @'SimpleTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
					<p>See also @'SimpleTextImager::GetNextRowReference', @'SimpleTextImager::GetPreviousRowReference'.</p>
	*/
	inline	SimpleTextImager::RowReference	SimpleTextImager::GetIthRowReferenceFromHere (RowReference fromHere, long ith) const
		{
			bool	result	=	GetIthRowReferenceFromHere (&fromHere, ith);
			Led_Assert (result);
			return (fromHere);
		}
	/*
	@METHOD:		SimpleTextImager::GetIthRowReference
	@DESCRIPTION:	<p>Get the <code>ith</code> row reference in the document. Asserts value <code>ith</code> refers to
				a valid row number.</p>
					<p>It calls @'SimpleTextImager::GetIthRowReferenceFromHere' todo its work (which returns a bool rather than asserting).</p>
	*/
	inline	SimpleTextImager::RowReference	SimpleTextImager::GetIthRowReference (size_t ith) const
		{
			RowReference	fromHere ((PartitionMarker*)GetFirstPartitionMarker ());
			bool	result	=	GetIthRowReferenceFromHere (&fromHere, ith);
			Led_Assert (result);
			return (fromHere);
		}
	/*
	@METHOD:		SimpleTextImager::GetRowLength
	@DESCRIPTION:	<p>Gets the length of the given row (in @'Led_tChar's).</p>
					<p>See also  @'SimpleTextImager::GetStartOfRow' and @'SimpleTextImager::GetEndOfRow'.</p>
	*/
	inline	size_t	SimpleTextImager::GetRowLength (RowReference row) const
		{
			return (GetEndOfRow (row) - GetStartOfRow (row));
		}
	/*
	@METHOD:		SimpleTextImager::GetLastRowReferenceInWindow
	@DESCRIPTION:	<p>Returns the last row-reference in the window (end of window).</p>
	*/
	inline	SimpleTextImager::RowReference	SimpleTextImager::GetLastRowReferenceInWindow () const
		{
			RowReference	row	=	GetTopRowReferenceInWindow ();
			Led_Assert (GetTotalRowsInWindow_ () >= 1);
			(void)GetIthRowReferenceFromHere (&row, GetTotalRowsInWindow_ () - 1);
			return (row);
		}
	inline	void	SimpleTextImager::SetTopRowInWindow_ (RowReference row)
		{
			fTopLinePartitionMarkerInWindow = row.GetPartitionMarker ();
			Led_AssertNotNil (fTopLinePartitionMarkerInWindow);
			InvalidateTotalRowsInWindow ();
		}
	/*
	@METHOD:		SimpleTextImager::GetInterLineSpace
	@DESCRIPTION:	<p>Get the interline space associated with a SimpleTextImager. Set the per-buffer value with
				@'SimpleTextImager::GetInterLineSpace'.</p>
					<p>NB: There is also a one-arg version of @'SimpleTextImager::GetInterLineSpace' which takes a PM
				as arg. See its docs, or @'SimpleTextImager::ChangedInterLineSpace' for more details.</p>
	*/
	inline	Led_Distance	SimpleTextImager::GetInterLineSpace () const
		{
			return (fInterlineSpace);
		}







	inline	bool	operator== (SimpleTextImager::RowReference lhs, SimpleTextImager::RowReference rhs)	
		{
			return (lhs.GetPartitionMarker () == rhs.GetPartitionMarker ());
		}
	inline	bool	operator!= (SimpleTextImager::RowReference lhs, SimpleTextImager::RowReference rhs)	
		{
			return (lhs.GetPartitionMarker () != rhs.GetPartitionMarker ());
		}





#if		qLedUsesNamespaces
}
#endif



#endif	/*__SimpleTextImager_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
