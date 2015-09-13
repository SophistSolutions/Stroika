/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__WordWrappedTextImager_h__
#define	__WordWrappedTextImager_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/WordWrappedTextImager.h,v 2.55 2003/03/06 14:05:38 lewis Exp $
 */

/*
@MODULE:	WordWrappedTextImager
@DESCRIPTION:
		<p>This module implements the @'WordWrappedTextImager' class. This class provides a @'TextImager' which wraps
	its text into rows, at word-boudaries, filling in as much of a row of text as possible, before wrapping (subject
	to standard text UI rules).</p>
 */

/*
 * Changes:
 *	$Log: WordWrappedTextImager.h,v $
 *	Revision 2.55  2003/03/06 14:05:38  lewis
 *	SPR#1325 - got rid of FindWrapPoint_ReferenceImplementation and related qHeavyDebugging code in finding wrap points. It called CalcSegmentSize() - whcih in turn called back to our module asking for row ends. This deadly two-way calling (deadly embrace) caused erratic behavior and allowed the caches to get filled with bad results. No need for that old reference test/implemeantion anymore anyhow. The new one has been working fine for quite a while and its pretty well tested adn not that complex
 *	
 *	Revision 2.54  2003/01/29 19:15:12  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.53  2003/01/11 19:28:40  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.52  2002/12/03 18:08:54  lewis
 *	part of SPR#1189- drawrowhilight BIDI code. Lose WRTI::DrawRowHilight() code cuz unneeded and obsolete. Instead
 *	handle the hilight logic better in TextImager::DrawRowHilight and add extra arg to GetIntraRowTextWindowBoundingRect
 *	to handle margin case (better than in Led 3.0 - I think). Also - made GetIntraRowTextWindowBoundingRect
 *	protected instead of public
 *	
 *	Revision 2.51  2002/11/19 12:38:29  lewis
 *	Added virtual WordWrappedTextImager::AdjustBestRowLength () and called in from FillCache to handle the
 *	logic for soft-row-wrap. Done to support fix for SPR#1174 - so that could be overridden in subclass for tables
 *	
 *	Revision 2.50  2002/10/25 12:26:40  lewis
 *	SPR#1149 - WordWrappedTextImager::GetLayoutMargins () docs clarified - margins RELATIVE to LHS of WindowRect
 *	
 *	Revision 2.49  2002/10/21 14:41:40  lewis
 *	SPR#1134 - partial cleanup of new PartitionElementCacheInfo and PMInfoCacheMgr code
 *	
 *	Revision 2.48  2002/10/20 19:38:14  lewis
 *	SPR#1128 and SPR#1129 - BIG change. Get rid of MultiRowPartitionMarker and MultiRowPartition. Still coded very
 *	slopily (with idefs) and needs major cleanups/tweeks. But - its now functional and testable
 *	
 *	Revision 2.47  2002/05/06 21:33:41  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.46  2001/11/27 00:29:48  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.45  2001/10/20 13:38:58  lewis
 *	tons of DocComment changes
 *	
 *	Revision 2.44  2001/10/17 20:42:56  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.43  2001/08/29 23:01:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.42  2001/08/28 18:43:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.41  2001/08/27 14:47:33  lewis
 *	SPR#0973- revise the display of hilighting (inversion) for the space before the
 *	LHS margin. Only invert that region if the selection extends OVER it in both
 *	directions OR if the entire line is selected. Works very well now - except its
 *	slightly quirking when the whole line is selected EXCEPT the end adn then you select
 *	the end. But I think this is the best I can do
 *	
 *	Revision 2.40  2000/10/21 14:17:46  lewis
 *	fix bug I had introduced in not calling SnagAttributesFromTablet- and comment on
 *	TrivialImager methods a bit more - and make the CTORs that dont call SnagAttrs protected
 *	
 *	Revision 2.39  2000/10/03 12:53:55  lewis
 *	clear up docs for TrivialImager<TEXTSTORE,IMAGER> - sample code
 *	
 *	Revision 2.38  2000/04/15 14:32:37  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.37  2000/04/14 22:40:23  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.36  1999/12/28 13:28:44  lewis
 *	SPR#0687- reversed order of template args to TrivialImager<>
 *	
 *	Revision 2.35  1999/12/21 03:32:06  lewis
 *	override *MappedDisplayCharacters (3 functions) to implement hiding of new
 *	soft-RETURN characters. Call TextStore::FindLineBreaks directly - without
 *	getting unused THIS pointer. Delete (hopefully) obsolete code to avoid FillBuf
 *	on \n character (now usin gnew MappedDisplay technology). And FillCache/SPLIT
 *	on SOFT-RETURN characters. See SPR#0667 and #0669 for details
 *	
 *	Revision 2.34  1999/12/19 16:33:09  lewis
 *	SPR#0667 work- added qDefaultLedSoftLineBreakChar/WordWrappedTextImager::kSoftLineBreakChar
 *	
 *	Revision 2.33  1999/12/15 17:53:18  lewis
 *	SPR#0661- TrivialWordWrappedTextImager had hardwired WordWrappedTextImager name in IMPL
 *	
 *	Revision 2.32  1999/12/14 18:57:40  lewis
 *	qExtraWordWrapDebugging obsolete - use qHeavyDebugging instead
 *	
 *	Revision 2.31  1999/11/13 22:33:01  lewis
 *	lose qSupportLed22CompatAPI support
 *	
 *	Revision 2.30  1999/11/13 21:59:53  lewis
 *	spr#0629- allow NULL param to GetLayoutMargins () calls
 *	
 *	Revision 2.29  1999/11/13 16:32:20  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.28  1999/05/03 22:05:05  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.27  1999/05/03 21:41:26  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.26  1999/03/03 01:46:58  lewis
 *	change TrivialImager CTOR to be overloaded to take no argument and call that
 *	version from subclass TrivialWordWrappedTextImager so as to avoid calling
 *	pure virtual func from base-class CTOR during init of derived class
 *	
 *	Revision 2.25  1999/03/01 16:45:22  lewis
 *	fix templating bug on TrivialWordWrappedTextImager(had hardwired ChunkedArrayTextStore)
 *	and fixup comment showing how to use better
 *	
 *	Revision 2.24  1999/02/08 22:30:20  lewis
 *	remove some tmphacks I had put in (qsupportoldgetlayoutwidthgovers=1 etc) so I could get
 *	2.3b3/4 out the door without it all working/thought out. Now really fix things
 *	
 *	Revision 2.23  1999/02/05 13:32:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.22  1998/10/30 14:16:32  lewis
 *	Halfway implmeneted new GetLayoutMargin() API - as a generaliztion of older GetLayoutWidth() API.
 *	Still subject to chagne. But with the qSupportOldGetLayoutWidthOverrides - it should be roughly backward
 *	compatable.
 *	
 *	Revision 2.21  1998/07/24  01:04:03  lewis
 *	New handle TrivialWordWrappedImager<TEMPLATE>
 *	Make qExtraWordWrapDebugging conditional (so I can debug, but other customers of
 *	LEd don't have these occasionally innocious
 *	asserts, and wasted extra time).
 *
 *	Revision 2.20  1998/04/25  01:28:32  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1998/03/04  20:15:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1997/12/24  04:36:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.17  1997/09/29  14:57:45  lewis
 *	Lose qSupportLed21CompatAPI
 *
 *	Revision 2.16  1997/07/27  15:56:47  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.15  1997/07/23  23:06:54  lewis
 *	Docs changes.
 *	GetLayoutWidth default impl for qSupportLed21CompatAPI.
 *
 *	Revision 2.14  1997/07/15  05:25:15  lewis
 *	AutoDoc content.
 *
 *	Revision 2.13  1997/07/12  20:06:53  lewis
 *	AutoDoc.
 *	Renamed TextImager_ to TextImager
 *
 *	Revision 2.12  1997/06/24  03:29:08  lewis
 *	New LayoutWidth strategy. No more SetLayoutWidth () call. And GetLayoutWidth() now
 *	declared here, and takes PartitionMarker arg. See SPR#0450.
 *
 *	Revision 2.11  1997/06/18  20:04:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/06/18  02:28:45  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1997/03/04  20:03:16  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.7  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.5  1996/04/18  15:18:01  lewis
 *	Lose ResetTabStops method - moved to TextImager_
 *
 *	Revision 2.4  1996/03/16  18:29:44  lewis
 *	Rid tabstop support.
 *
 *	Revision 2.3  1996/02/05  04:13:30  lewis
 *	No more TextStore_ arg to CTOR, and typeddfe inherited.
 *
 *	Revision 2.2  1995/10/19  22:10:09  lewis
 *	Moved a bnch of stuff like the font cache, the implementations of
 *	the drawsegment, and measuresegment etc routines to _ routines
 *	in TextImager. Also - releated - FontCacheInfoUpdater; moved
 *	to TextImager_.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.13  1995/06/08  05:11:23  lewis
 *	Code cleanups
 *
 *	Revision 1.12  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.11  1995/05/29  23:39:09  lewis
 *	Added utility class FontCacheInfoUpdater as part of fix for AssureFontCacheUpToDate
 *	- See SPR 0288.
 *
 *	Revision 1.10  1995/05/20  04:50:25  lewis
 *	Renamed SetImageWidth to SetLayoutWidth - SPR 0263.
 *
 *	Revision 1.9  1995/05/16  06:39:18  lewis
 *	Override pure virtual GetFontMetricsAt().
 *
 *	Revision 1.8  1995/05/12  21:04:26  lewis
 *	Sterl added TryToFindWrapPointForMeasuredText1 () as part of his fix
 *	& speed tweeks to word-wrapping & tabstops.
 *
 *	Revision 1.7  1995/05/09  23:19:45  lewis
 *	Added ResetTabStops () method to patch results of MeasureText () as
 *	we wrap text so we have new relative start for tabstops.
 *	(STERL WROTE THIS).
 *
 *	Revision 1.6  1995/05/06  19:36:42  lewis
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *	New wordwrap based on MeasureText!!!!! FAST!!!!!
 *
 *	Revision 1.5  1995/05/05  19:44:04  lewis
 *	Added MeasureSegmentHeight/Width methods.
 *	Cleanups.
 *
 *	Revision 1.4  1995/03/15  01:04:31  lewis
 *	Added private utility method FindWrapPointForOneLongWord() for that
 *	special case.
 *
 *	Revision 1.3  1995/03/13  03:18:09  lewis
 *	Renamed Array<T> to Led_Array<T>.
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

#include	"MultiRowTextImager.h"





#if		qLedUsesNamespaces
namespace	Led {
#endif



/*
@CONFIGVAR:		qDefaultLedSoftLineBreakChar
@DESCRIPTION:	<p>This is the magic (sentinal) character that will appear in a Led text buffer to indicate
			a soft line break. This is generated in MSWord (on Mac 5.1 and Windows WinWord 7.0) by the SHIFT-RETURN
			key combination. I have no idea what value is stored internally in MSWord. This is the value WE will
			store internally for Led. And so its somewhat subject to change - if I find THIS value conflicts with anything
			useful.</p>
 */
#ifndef	qDefaultLedSoftLineBreakChar
	#define	qDefaultLedSoftLineBreakChar	'\001'
#endif





/*
@CLASS:			WordWrappedTextImager
@BASES:			@'MultiRowTextImager'
@DESCRIPTION:	<p>Implement the multirow calculations of a MultiRowTextImager using standard word-wrapping algorithms.
			Though this will be commonly used for word-wrapping text editors, it isn't the only plasible way to break rows.
			Some other sort of semantic content in the text might be taken as the thing which guides breaking rows.</p>
*/
class	WordWrappedTextImager : public MultiRowTextImager {
	protected:
		WordWrappedTextImager ();
		virtual ~WordWrappedTextImager ();

	private:
		typedef	MultiRowTextImager	inherited;


	public:
		/*
		@METHOD:		WordWrappedTextImager::kSoftLineBreakChar
		@DESCRIPTION:
				<p>Sentinal character used to mark a soft line-break.</p>
		*/
		static	const	Led_tChar	kSoftLineBreakChar;

	public:
		/*
		@METHOD:		WordWrappedTextImager::GetLayoutMargins
		@DESCRIPTION:
				<p>NB: this routine must @'Led_Ensure' that the *rhs and *lhs results be valid after a call (garbage in), and must ensure *rhs > *lhs,
			assuming that both 'lhs' and 'rhs' are non-null pointers (either or both of these pointers can be null).</p>
				<p>You must specify the wrap-width for each paragraph in a subclass. This routine can be overriden
			to return a constant width - eg. width of the window, or a different per-paragraph width. But note,
			it is the subclassers responsability to invalidate whatever caches need to be invalidated (typically in MutliRowTextImager)
			when the layout width is changed. (nb: this changed in Led 2.2 - 970623-LGP).</p>
				<p>It is because of the need to notify / invalidate caches that we don't provide a default implementation here.
			The most likely default would be to wrap to the width of the window, and to successfully implement that strategy,
			we would need to override SetWindowRect () here. But that would leave those <em>subclassing us</em> with the job
			of circumventing that needless invalidation, and that seems ugly, and awkward. Better to keep the choice of specifying
			this width in the same place as where we handle the invalidation.</p>
				<P>NEW FOR LED 3.1d1. This routine used to be used AMBIGUOUSLY. Sometimes it was interpretted that the return values
			were RELATIVE to the window rect, and sometimes it was interpretted that they took into account the window rect.
			REWRITE THE ABOVE DOCS SO CLEARER ABOUT THIS CHOICE. FROM NOW ON, RETURNS VALUES RELATIVE TO LHS OF WINDOW RECT - LGP 2002-10-25
			</p>
		*/
		virtual	void	GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const		=	0;

	public:
		override	void	FillCache (PartitionMarker* pm, PartitionElementCacheInfo& cacheInfo);

	protected:
		virtual		void	AdjustBestRowLength (size_t textStart, const Led_tChar* text, const Led_tChar* end, size_t* rowLength);

	protected:
		override	bool	ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const;
		override	size_t	RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const;
		override	void	PatchWidthRemoveMappedDisplayCharacters (const Led_tChar* srcText, Led_Distance* distanceResults, size_t nTChars) const;

	/*
 	 * Word wrapping helpers.
	 */
	private:
		nonvirtual	size_t	FindWrapPointForMeasuredText (const Led_tChar* text, size_t length, Led_Distance wrapWidth,
											size_t offsetToMarkerCoords, const Led_Distance* widthsVector, size_t startSoFar
											);
		nonvirtual	size_t	TryToFindWrapPointForMeasuredText1 (const Led_tChar* text, size_t length, Led_Distance wrapWidth,
											size_t offsetToMarkerCoords, const Led_Distance* widthsVector, size_t startSoFar,
											size_t searchStart, size_t wrapLength);
		nonvirtual	size_t	FindWrapPointForOneLongWordForMeasuredText (const Led_tChar* text, size_t length, Led_Distance wrapWidth,
											size_t offsetToMarkerCoords,
											const Led_Distance* widthsVector, size_t startSoFar
										);
};




/*
@CLASS:			TrivialWordWrappedImager<TEXTSTORE, IMAGER>
@BASES:			@'TrivialImager<TEXTSTORE,IMAGER>'
@DESCRIPTION:	<p>Handy little class to image text directly. Like Mac "TextBox" or Win32 "DrawText", in that
	you can directly use it (no setup), and it images and wraps the text into the given box.</p>
		<p>The usuage can be as simple as:
		<code>
			void	SomeAppDrawCall (Led_Tablet t, const Led_Rect& r, bool printing)
			{
				TrivialWordWrappedImager<ChunkedArrayTextStore> (t, r, LED_TCHAR_OF ("Hi mom")).Draw ();
			}
		</code>
		</p>
		<p>See also @'TrivialImager<TEXTSTORE,IMAGER>', and @'TrivialImager_Interactor<TEXTSTORE,IMAGER>'.</p>
 */
template	<typename TEXTSTORE, typename	IMAGER = WordWrappedTextImager>
	class	TrivialWordWrappedImager : public TrivialImager<TEXTSTORE,IMAGER> {
		protected:
			TrivialWordWrappedImager (Led_Tablet t);
		public:
			TrivialWordWrappedImager (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText = LED_TCHAR_OF(""));

		public:
			override	void			GetLayoutMargins (MultiRowTextImager::RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;
			nonvirtual	Led_Distance	GetHeight () const;
	};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

//	class	WordWrappedTextImager
	inline	WordWrappedTextImager::WordWrappedTextImager ():
		TextImager (),
		MultiRowTextImager ()//,
		{
		}
	inline	WordWrappedTextImager::~WordWrappedTextImager ()
		{
		}



//	class	TrivialWordWrappedImager<TEXTSTORE,IMAGER>
	template	<typename TEXTSTORE, typename	IMAGER>
		/*
		@METHOD:		TrivialWordWrappedImager<TEXTSTORE,IMAGER>::TrivialWordWrappedImager
		@DESCRIPTION:	<p>Two overloaded versions - one protected, and the other public. The protected one
					does NOT call @'TrivialImager<TEXTSTORE,IMAGER>::SnagAttributesFromTablet' - so you must in your subclass.</p>
						<p>Most people will just call the public CTOR - as in the class documentation
					(@'TrivialWordWrappedImager<TEXTSTORE,IMAGER>')</p>
		*/
		TrivialWordWrappedImager<TEXTSTORE,IMAGER>::TrivialWordWrappedImager (Led_Tablet t):
			TrivialImager<TEXTSTORE,IMAGER> (t)
		{
		}
	template	<typename TEXTSTORE, typename	IMAGER>
		TrivialWordWrappedImager<TEXTSTORE,IMAGER>::TrivialWordWrappedImager (Led_Tablet t, Led_Rect bounds, const Led_tString& initialText):
			TrivialImager<TEXTSTORE,IMAGER> (t)
		{
			SnagAttributesFromTablet ();
			SetWindowRect (bounds);
			GetTextStore ().Replace (0, 0, initialText.c_str (), initialText.length ());
		}
	template	<typename TEXTSTORE, typename	IMAGER>
		void	TrivialWordWrappedImager<TEXTSTORE,IMAGER>::GetLayoutMargins (MultiRowTextImager::RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const
		{
			Led_Ensure (GetWindowRect ().GetWidth () >= 1);
			if (lhs != NULL) {
				*lhs = 0;
			}
			if (rhs != NULL) {
				*rhs = GetWindowRect ().GetWidth ();
			}
			#if		qDebug
				if (lhs != NULL and rhs != NULL) {
					Led_Ensure (*rhs > *lhs);
				}
			#endif
		}
	template	<typename TEXTSTORE, typename	IMAGER>
		/*
		@METHOD:		TrivialWordWrappedImager<TEXTSTORE,IMAGER>::GetHeight
		@DESCRIPTION:	<p>Returns the height - in pixels - of the text in the imager.</p>
		*/
		Led_Distance	TrivialWordWrappedImager<TEXTSTORE,IMAGER>::GetHeight () const
		{
			return GetHeightOfRows (0, GetRowCount ());
		}


#if		qLedUsesNamespaces
}
#endif

#endif	/*__WordWrappedTextImager_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
