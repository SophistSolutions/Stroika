/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/WordWrappedTextImager.cpp,v 2.56 2003/03/21 14:47:57 lewis Exp $
 *
 * Changes:
 *	$Log: WordWrappedTextImager.cpp,v $
 *	Revision 2.56  2003/03/21 14:47:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.55  2003/03/06 14:05:40  lewis
 *	SPR#1325 - got rid of FindWrapPoint_ReferenceImplementation and related qHeavyDebugging code in
 *	finding wrap points. It called CalcSegmentSize() - whcih in turn called back to our module asking
 *	for row ends. This deadly two-way calling (deadly embrace) caused erratic behavior and allowed the
 *	caches to get filled with bad results. No need for that old reference test/implemeantion anymore
 *	anyhow. The new one has been working fine for quite a while and its pretty well tested adn not
 *	that complex
 *	
 *	Revision 2.54  2003/02/28 19:08:20  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.53  2003/01/11 19:28:49  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.52  2002/12/14 03:38:48  lewis
 *	code cleanups and disable (temporarily?) the assert about 'abs (diff) <= 50'. Triggers alot now with new BIDI
 *	code (probably due to BIDI BUGS). Leave out til we have more BIDI working and maybe redo how we measure text anyhow
 *	
 *	Revision 2.51  2002/12/04 15:37:46  lewis
 *	SPR#1193- replace calls to CalcSegmentSizeWithImmediateText with just CalcSegmentSize
 *	
 *	Revision 2.50  2002/12/03 18:09:05  lewis
 *	part of SPR#1189- drawrowhilight BIDI code. Lose WRTI::DrawRowHilight() code cuz unneeded and obsolete.
 *	Instead handle the hilight logic better in TextImager::DrawRowHilight and add extra arg to
 *	GetIntraRowTextWindowBoundingRect to handle margin case (better than in Led 3.0 - I think). Also -
 *	made GetIntraRowTextWindowBoundingRect protected instead of public
 *	
 *	Revision 2.49  2002/12/03 14:29:07  lewis
 *	SPR#1190 - changed name for (TextImager/Led_Tablet_)::HilightARectangle to HilightArea
 *	
 *	Revision 2.48  2002/11/19 12:38:31  lewis
 *	Added virtual WordWrappedTextImager::AdjustBestRowLength () and called in from FillCache to
 *	handle the logic for soft-row-wrap. Done to support fix for SPR#1174 - so that could be
 *	overridden in subclass for tables
 *	
 *	Revision 2.47  2002/10/30 13:29:43  lewis
 *	SPR#1155 - Added new Rect_Base<> template and made Led_Rect a simple typedef of that. Then TRIED (failed)
 *	to make the top/left/bottom/right fields private by adding accessors. Added the accessors and used them
 *	in tons of places - but still TONS more required. Leave that battle for another day. I'm not even SURE its
 *	a good idea. Anyhow - also added a new typedef for Led_TWIPS_Rect (this was what I was after to begin with
 *	with all this nonsese)
 *	
 *	Revision 2.46  2002/10/21 20:27:48  lewis
 *	SPR#1134- Moved date in PartitionElementCacheInfo into a shared Rep object so copy CTOR/op= would be MUCH faster
 *	(at the cost of extra deref in use)
 *	
 *	Revision 2.45  2002/10/21 14:41:53  lewis
 *	SPR#1134 - partial cleanup of new PartitionElementCacheInfo and PMInfoCacheMgr code
 *	
 *	Revision 2.44  2002/10/20 19:38:28  lewis
 *	SPR#1128 and SPR#1129 - BIG change. Get rid of MultiRowPartitionMarker and MultiRowPartition. Still coded very
 *	slopily (with idefs) and needs major cleanups/tweeks. But - its now functional and testable
 *	
 *	Revision 2.43  2002/05/06 21:34:00  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.42  2001/11/27 00:30:05  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.41  2001/10/17 20:43:05  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.40  2001/09/26 22:15:02  lewis
 *	lose unused variable
 *	
 *	Revision 2.39  2001/09/26 15:41:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.38  2001/08/29 23:36:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.37  2001/08/28 18:43:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.36  2001/08/27 14:47:34  lewis
 *	SPR#0973- revise the display of hilighting (inversion) for the space before the LHS margin.
 *	Only invert that region if the selection extends OVER it in both directions OR if the entire
 *	line is selected. Works very well now - except its slightly quirking when the whole line is
 *	selected EXCEPT the end adn then you select the end. But I think this is the best I can do
 *	
 *	Revision 2.35  2001/05/09 19:31:13  lewis
 *	fix buggy assert in softlinebreak code- OK if softlinebreak char is on same spot as where
 *	line would have broken anyhow - in which case - newRowLen gets bigger by one (I think)
 *	
 *	Revision 2.34  2000/06/15 22:28:45  lewis
 *	SPR#0778- Made TextStore own a smart pointer to a TextBreak object - and by
 *	default create one with the old algorithms.
 *	
 *	Revision 2.33  2000/06/15 20:04:20  lewis
 *	Prelim hacks for SPR#0778- TextStore::FindLineBreaks->TextBreaks ().FindLineBreaks
 *	
 *	Revision 2.32  2000/04/16 14:39:40  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.31  2000/04/14 22:40:29  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.30  2000/01/22 22:16:09  lewis
 *	add comments and disable some assertions (or make them more forgiving)
 *	
 *	Revision 2.29  1999/12/21 03:32:06  lewis
 *	override *MappedDisplayCharacters (3 functions) to implement hiding of new soft-RETURN characters.
 *	Call TextStore::FindLineBreaks directly - without getting unused THIS pointer. Delete (hopefully)
 *	obsolete code to avoid FillBuf on \n character (now usin gnew MappedDisplay technology)
 *	And FillCache/SPLIT on SOFT-RETURN characters. See SPR#0667 and #0669 for details
 *	
 *	Revision 2.28  1999/12/19 16:33:09  lewis
 *	SPR#0667 work- added qDefaultLedSoftLineBreakChar/WordWrappedTextImager::kSoftLineBreakChar
 *	
 *	Revision 2.27  1999/12/14 21:24:09  lewis
 *	New name - qHeavyDebugging instead of qExtraWordWrapDebugging
 *	
 *	Revision 2.26  1999/11/13 16:32:25  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.25  1999/05/03 22:05:24  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.24  1999/02/09 16:32:08  lewis
 *	when we get an exception in the FillCache() code, patch the fPixelHieghtCache back to -1,
 *	so its not marked valid (with bogus values in it)
 *	
 *	Revision 2.23  1998/10/30 14:31:31  lewis
 *	TmpHack support for new GetLayoutMargins() method. NOT FONE/RIGHT YET!!!.
 *	And deleted nonsense line of code which had compiled harmlessly for years (picked up by
 *	new compiler warnings).
 *	
 *	Revision 2.22  1998/07/24  01:17:18  lewis
 *	qExtraWordWrapDebugging support (took old asserts and conditionalized them on this to
 *	make them less annoying to folks at
 *	LEC and still allow me to check up on this stuff).
 *	React to revised ResetTabStops() API.
 *
 *	Revision 2.21  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1997/09/29  15:52:23  lewis
 *	Lose qLedFirstIndex support.
 *	Ues new CalcSegmentSizeWithImmediateText
 *
 *	Revision 2.19  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.18  1997/07/13  02:16:55  lewis
 *	AutoDoc support.
 *	qLedFirstIndex = 0 support.
 *
 *	Revision 2.17  1997/06/24  03:34:25  lewis
 *	Moved LayoutWidth concept here from TextImager_.
 *	Now no longer a SetLayoutWidth() at all. Instead you have a GetLayoutWidth() which takes
 *	a PM as arg. See spr#0450.
 *
 *	Revision 2.16  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1997/06/18  03:13:02  lewis
 *	ResetTabStops takes PM arg.
 *	Fix subtle PC printing word-wrap bug (really could be anywhere) where roundoff on MeasureText cuz
 *	of sub-pixel roundoff. Not fixed perfectly, but close enuf, I think.
 *
 *	Revision 2.14  1997/03/22  15:30:35  lewis
 *	re-enable assert about word-wrap calculations.
 *	get rid of #if qSupportTabStops stuff. Was lost before but I forgot to get rid
 *	of it here.
 *
 *	Revision 2.13  1997/01/20  05:28:37  lewis
 *	Re-disable assert about word-wrapping idiosyncracies. Not going to worry about this now.
 *	Its on SPR list, and seems quite minor...
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
 *	Revision 2.9  1996/05/23  19:45:46  lewis
 *	A few minor cleanups, including using GetRange instead of GetStart/GetEnd() (tweek)
 *
 *	Revision 2.8  1996/04/18  15:40:47  lewis
 *	Moved ResetTabStops to TextImager_.
 *	React to API change - CalcSegmentSize now returns width directly.
 *
 *	Revision 2.7  1996/03/16  18:46:30  lewis
 *	New tabstop support.
 *
 *	Revision 2.6  1996/02/05  04:25:25  lewis
 *	No more TextStore_ arg to CTOR, and use inherited typedef.
 *
 *	Revision 2.5  1996/01/22  05:28:03  lewis
 *	Wrap some tab support code in #if qSupportTabStops.
 *
 *	Revision 2.4  1995/11/04  23:20:29  lewis
 *	Fixed a bug in FillCache where it assumed all rows were of the same height.
 *	Now compute hieght for each row seperately.
 *
 *	Revision 2.3  1995/10/19  22:29:52  lewis
 *	Massive changes.
 *	Threw away lots of uneeded word wrap code that hung around only to support
 *	case where we had no MeasureText (old win32s bug). Just abandon that now.
 *	Also, moved all the actual font rendering/measuring code down to
 *	_ helper routines in TextImager_ itself. Similarly for FontCache etc.
 *	Some utility functions for benchmarking draw speeds are now trashed.
 *	Probably more - massive diffs.
 *
 *	Revision 2.2  1995/10/09  22:45:50  lewis
 *	AquireTablet no longer gaurantess setting port. We must call dosetport
 *	ourselves.
 *	Cleanup some warnings for function calls not using args in case of
 *	ASCII instead of DBCS strings. Had to ifdef whether we define arg names.
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.18  1995/06/08  05:17:06  lewis
 *	Code cleanups.
 *
 *	Revision 1.17  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.16  1995/05/30  00:02:06  lewis
 *	Lots of changes, including deleting commneted ou;code from last set of fixes.
 *	Use new FontCacheInfoUpdater class.
 *	(see SPR 0288).
 *	Use new const_cast () macro - SPR 0289
 *
 *	Revision 1.15  1995/05/25  09:48:55  lewis
 *	Lots of cleanups using new stackbased acquire alloate/dealocation
 *	SPR 0282.
 *	Fix cache setting bug in AssureFontCacheUpToDate() for PC only - SPR 0287.
 *
 *	Revision 1.14  1995/05/20  05:03:26  lewis
 *	Lots of fixes for SPR 0263. Use new LayoutWidth instead of ImageRect and
 *	use WindowRect more correctly.
 *	Several fixes for SPR 0257/0265 - pack MRTIPartMaerks data better, and use
 *	accessors.
 *
 *	Revision 1.13  1995/05/16  06:41:52  lewis
 *	Preovide override of pure virtual GetFontMetricsAt ().
 *
 *	Revision 1.12  1995/05/12  21:08:27  lewis
 *	All STERLs changes to speed tweek word-wrapping, and fix a bug with
 *	tab processing in word-wrap code. Mainly he reduced calls to FindLineBreaks
 *	by guessing a good spot to start calling it based on measuretext info.
 *	This code is ghastly, and really needs major cleanups! The use of multiple
 *	meanings of indexes (what they are relative to) has become VERY confusing.
 *	I just a little nervous about chaning it this late in the release cycle.
 *	SPR 0245/0246.
 *
 *	Revision 1.11  1995/05/10  00:05:12  lewis
 *	Changed tab-width from 8 to 4. Magic # anyhow, and 4 happens to be what
 *	I use in most of my source / log files, so things just look better when
 *	browsing with Led.
 *
 *	Revision 1.10  1995/05/09  23:32:34  lewis
 *	Added qUseWidthOfSpaceForTab so we can use charwidth (\t) rather
 *	than maxwidth of any char. - LedSPR#0242.
 *	Added qSupportWindowsSystemsWithNoMeasureText so we could support
 *	dynamicly chosing between measuretext and textwidth depending
 *	on whether measuretext was available under win32. Cuz for win32s
 *	1.20 - at least - it doesn't work. This is only for windows.
 *	LedSPR#0238
 *	Don't refer to fRowHeightCacheValid - LedSPR#0241
 *
 *	Revision 1.9  1995/05/08  03:14:57  lewis
 *	Override of SetLayoutWidth now simpler - more inval crap done in base class.
 *	Code cleanups.
 *
 *	Revision 1.8  1995/05/06  19:38:44  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *	New define for MeasureText based word-wrapping - qUseMeasureTextForWordWrap.
 *	Optimized WordWrappedTextImager::AssureFontCacheUpToDate ()
 *	See SPR 0180/0234 for word-wrapping/measuretext details/bugs.
 *
 *	Revision 1.7  1995/05/05  19:55:41  lewis
 *	Too much to mention!
 *	Main thing was fixing TextWidth/MeasureText to work with VERY long text,
 *	and have them break up the calls and compute LONG summary when GDI
 *	only supported SHORTS (Win31 and Mac).
 *	Added temporarily - DoProfileOfTextMeasurements.
 *	Did implemenation of MeasureSegmentHeight/Width - on both Mac/PC doing
 *	::MeasureText on Mac and ::GetTextExtentExPoint - or some such on PC.
 *	Probably much more.
 *	SPRs include 0224, 0226.
 *
 *	Revision 1.6  1995/05/03  19:10:50  lewis
 *	Started adding support for PortableGDI_MeasureText ().
 *
 *	Revision 1.5  1995/03/15  01:11:31  lewis
 *	Broke out code for finding wrap point in one long word into separate routine,
 *	and in that rewrite - now guess a point reasonably close to the
 *	proper wrap point. This used to take FOREVER - and now is pretty quick.
 *	Still room for improvement, however.
 *
 *	Revision 1.4  1995/03/13  03:29:18  lewis
 *	Mostly changed Array<T> to Led_Array<T>. Plus one small fix for
 *	LedSPR#160 - I think.
 *
 *	Revision 1.3  1995/03/03  15:53:36  lewis
 *	A number of code cleanups / assertions added (ASSERT_VALOD of windows
 *	GDI objects).
 *	Added tablet->SelectObject (oldFont) call for Windows so as to restore
 *	font forgotten in WordWrappedTextImager::FillCache () which caussed
 *	lots of griping in BoundsChecker on PC.
 *
 *	Revision 1.2  1995/03/02  05:46:24  lewis
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

#include	<cctype>

#include	"WordWrappedTextImager.h"








#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif






	#if		qMultiByteCharacters
	inline	AdjustToValidCharIndex (const Led_tChar* text, size_t index)
		{
			if (Led_FindPrevOrEqualCharBoundary (&text[0], &text[index]) != &text[index]) {
				Led_Assert (index > 0);
				index--;
			}
			return (index);
		}
	#endif

	inline	Led_Distance	LookupLengthInVector (const Led_Distance* widthsVector, size_t startSoFar, size_t i)
		{
			Led_AssertNotNil (widthsVector);
			if (i == 0) {
				return 0;
			}
			Led_Distance	startPointCorrection	=	(startSoFar == 0)? 0: widthsVector[startSoFar-1];
			Led_Assert (i+startSoFar >= 1);
			return (widthsVector[i+startSoFar-1]-startPointCorrection);
		}














/*
 ********************************************************************************
 ********************************* WordWrappedTextImager ************************
 ********************************************************************************
 */

const	Led_tChar	WordWrappedTextImager::kSoftLineBreakChar	=	qDefaultLedSoftLineBreakChar;

/*
@METHOD:		WordWrappedTextImager::FillCache
@DESCRIPTION:	<p>Hook the MultiRowTextImager's FillCache () call to compute the row information for
	the given @'Partition::PartitionMarker'.</p>
		<p>Basicly, this is where we start the word-wrap process (on demand, per paragraph).</p>
*/
void	WordWrappedTextImager::FillCache (PartitionMarker* pm, PartitionElementCacheInfo& cacheInfo)
{
// Need to be careful about exceptions here!!! Probably good enuf to just
// Invalidate the caceh and then propogate exception...
// Perhaps better to place bogus entry in? No - probably not...
	Led_RequireNotNil (pm);

	size_t	start;
	size_t	end;
	pm->GetRange (&start, &end);
	size_t	len		=	end-start;

	Led_Assert (end <= GetEnd () + 1);
	if (end == GetEnd () + 1) {
		end--;		// don't include bogus char at end of buffer
		len--;
	}
	Led_Assert (end <= GetEnd ());

	Led_SmallStackBuffer<Led_tChar>	buf (len);
	CopyOut (start, len, buf);

	Tablet_Acquirer	tablet (this);

	cacheInfo.Clear ();

	try {
		Led_SmallStackBuffer<Led_Distance>	distanceVector (len);
		if (start != end) {
			MeasureSegmentWidth (start, end, buf, distanceVector);
		}

		size_t	startSoFar	=	0;	// make this ZERO relative from start of THIS array
		size_t	leftToGo	=	len-startSoFar;

		/*
		 * As a minor optimization for resetting tabs, we keep track of the index of the last
		 * tab we saw during last scan. If the last tab is before our current wrap point, then
		 * no more tabs to be found. Since tabs are usually at the start of a sentence or paragraph,
		 * this is modestly helpful.
		 */
		size_t	lastTabIndex = startSoFar;	

		while (leftToGo != 0) {
			cacheInfo.IncrementRowCountAndFixCacheBuffers (startSoFar, 0);

			if (lastTabIndex >= startSoFar) {
				lastTabIndex = ResetTabStops (start, buf, leftToGo, distanceVector, startSoFar);
			}

			Led_Distance	wrapWidth;
			{
				// NOT RIGHT - doesn't properly interpret tabstops!!! with respect to left margins!!! LGP 980908
				Led_Coordinate	lhsMargin;
				Led_Coordinate	rhsMargin;
				GetLayoutMargins (RowReference (pm, cacheInfo.GetRowCount () -1), &lhsMargin, &rhsMargin);
				Led_Assert (lhsMargin < rhsMargin);
				wrapWidth = rhsMargin - lhsMargin;
			}
			size_t	bestRowLength	=	FindWrapPointForMeasuredText (buf + startSoFar, leftToGo, wrapWidth, start+startSoFar, distanceVector, startSoFar);

			Led_Assert (bestRowLength != 0);	// FindWrapPoint() could only do this if we gave it a zero leftToGo - but we wouldn't
												// be in the loop in that case!!!




			// Now override the above for soft-breaks...
			{
				Led_Assert (bestRowLength > 0);
				const Led_tChar*	text	=	buf + startSoFar;
				const Led_tChar*	end		=	&text[min (bestRowLength+1, leftToGo)];
				AdjustBestRowLength (start + startSoFar, text, end, &bestRowLength);
				Led_Assert (bestRowLength > 0);
			}


			Led_Distance	newRowHeight	=	MeasureSegmentHeight (start + startSoFar, start + startSoFar + bestRowLength);
			cacheInfo.SetRowHeight (cacheInfo.GetRowCount ()-1, newRowHeight);

			startSoFar += bestRowLength;
			Led_Assert (len >= startSoFar);
			leftToGo = len-startSoFar;
		}

		// always have at least one row...even if there were no bytes in the row
		Led_Assert (len == 0 or cacheInfo.PeekRowCount () != 0);
		if (cacheInfo.PeekRowCount () == 0) {
			Led_Assert (len == 0);
			Led_Assert (startSoFar == 0);
			cacheInfo.IncrementRowCountAndFixCacheBuffers (0, MeasureSegmentHeight (start, end));
		}

		cacheInfo.SetInterLineSpace (CalculateInterLineSpace (pm));
		Led_Assert (cacheInfo.GetRowCount () >= 1);
	}
	catch (...) {
		// If we run into exceptions filling the cache, don't leave it in an inconsistent state.
		// NB: this isn't necessarily the BEST way to leave it. It may mean - for example - trying to draw
		// you get an exception and never will get ANYTHING drawn.. And thats not GREAT. But perahps better
		// than silently putting things in the WRONG state (which would never get recovered) and allow drawing
		// the WRONG thing. But much more likely - this will happen during early init stages when the
		// window/etc aren't yet fully built/connected (NoTablet) - LGP990209
		if (cacheInfo.PeekRowCount () == 0) {
			cacheInfo.IncrementRowCountAndFixCacheBuffers (0, 20);
		}
		throw;
	}
}

/*
@METHOD:		WordWrappedTextImager::AdjustBestRowLength
@DESCRIPTION:	<p>Virtual method called during word-wrapping to give various subclasses a crack at overriding the measured
			wrap point. On function entry - 'rowLength' points to the calculated row length, and on output, it must be a valid
			row length (possibly shorter than the original value, but always greater than zero).</p>
*/
void	WordWrappedTextImager::AdjustBestRowLength (size_t /*textStart*/, const Led_tChar* text, const Led_tChar* end, size_t* rowLength)
{
	Led_Require (*rowLength > 0);
	for (const Led_tChar* cur = &text[0]; cur < end; cur = Led_NextChar (cur)) {
		if (*cur == WordWrappedTextImager::kSoftLineBreakChar) {
			size_t	newBestRowLength	=	(cur-text)+1;
			Led_Assert (newBestRowLength <= *rowLength + 1);	// Assure newBestRowLength is less than it would have been without the
																// softlinebreak character, EXCEPT if the softlinebreak char is already
																// at the spot we would have broken - then the row gets bigger by the
																// one softlinebreak char length...
																// LGP 2001-05-09 (see SPR707 test file-SimpleAlignDivTest.html)
			Led_Assert (newBestRowLength >= 1);
			*rowLength = newBestRowLength;
			break;
		}
	}
}

/*
@METHOD:		WordWrappedTextImager::ContainsMappedDisplayCharacters
@DESCRIPTION:	<p>Override @'TextImager::ContainsMappedDisplayCharacters' to hide kSoftLineBreakChar characters.
	See @'qDefaultLedSoftLineBreakChar'.</p>
*/
bool	WordWrappedTextImager::ContainsMappedDisplayCharacters (const Led_tChar* text, size_t nTChars) const
{
	return 
			ContainsMappedDisplayCharacters_HelperForChar (text, nTChars, kSoftLineBreakChar) or
			inherited::ContainsMappedDisplayCharacters (text, nTChars)
		;
}

/*
@METHOD:		WordWrappedTextImager::RemoveMappedDisplayCharacters
@DESCRIPTION:	<p>Override @'TextImager::RemoveMappedDisplayCharacters' to hide kSoftLineBreakChar characters.</p>
*/
size_t	WordWrappedTextImager::RemoveMappedDisplayCharacters (Led_tChar* copyText, size_t nTChars) const
{
	size_t	newLen	=	inherited::RemoveMappedDisplayCharacters (copyText, nTChars);
	Led_Assert (newLen <= nTChars);
	size_t	newerLen	=	RemoveMappedDisplayCharacters_HelperForChar (copyText, newLen, kSoftLineBreakChar);
	Led_Assert (newerLen <= newLen);
	Led_Assert (newerLen <= nTChars);
	return newerLen;
}

/*
@METHOD:		WordWrappedTextImager::PatchWidthRemoveMappedDisplayCharacters
@DESCRIPTION:	<p>Override @'TextImager::PatchWidthRemoveMappedDisplayCharacters' to hide kSoftLineBreakChar characters.</p>
*/
void	WordWrappedTextImager::PatchWidthRemoveMappedDisplayCharacters (const Led_tChar* srcText, Led_Distance* distanceResults, size_t nTChars) const
{
	inherited::PatchWidthRemoveMappedDisplayCharacters (srcText, distanceResults, nTChars);
	PatchWidthRemoveMappedDisplayCharacters_HelperForChar (srcText, distanceResults, nTChars, kSoftLineBreakChar);
}

/*
@METHOD:		WordWrappedTextImager::FindWrapPointForMeasuredText
@DESCRIPTION:	<p>Helper for word wrapping text. This function is given text, and pre-computed text measurements for the
	width of each character (Led_tChar, more accurately). Before calling this, the offsets have been adjused for tabstops.
	This just computes the appropriate place to break the line into rows (just first row).</p>
*/
size_t	WordWrappedTextImager::FindWrapPointForMeasuredText (const Led_tChar* text, size_t length, Led_Distance wrapWidth,
											size_t offsetToMarkerCoords, const Led_Distance* widthsVector, size_t startSoFar
											)
{
	Led_RequireNotNil (widthsVector);
	Led_Require (wrapWidth >= 1);
#if		qMultiByteCharacters
	Led_Assert (Led_IsValidMultiByteString (text, length));
#endif
	size_t	bestRowLength = 0;

	/*
	 *	SUBTLE!
	 *
	 *		Because we measure the text widths on an entire paragraph at a time (for efficiency),
	 *	we run into the possability of chosing a wrap point wrongly by a fraction of a pixel.
	 *	This is because characters widths don't necessarily fit on pixel boundaries. They can
	 *	overlap one another, and due to things like kerning, the width from one point in the string
	 *	to another can not EXACTLY reflect the length of that string (in pixels) - from measuretext.
	 *
	 *		The problem occurs because offsets in the MIDDLE of a paragraph we re-interpret the offset
	 *	as being zero, and so lose a fraction of a pixel. This means that a row of text can be either
	 *	a fraction of a pixel LONGER than we measure or a fraction of a pixel too short.
	 *
	 *		This COULD - in principle - mean that we get a small fraction of a character drawn cut-off.
	 *	To correct for this, when we are on any starting point in the middle of the paragraph, we lessen
	 *	the wrap-width by one pixel. This guarantees that we never cut anything off. Though in the worse
	 *	case it could mean we word wrap a little over a pixel too soon. And we will generally word-wrap
	 *	a pixel too soon (for rows after the first). It is my judgement - at least for now - that this
	 *	is acceptable, and not generally noticable. Certiainly much less noticable than when a chracter
	 *	gets cut off.
	 *
	 *		For more info, see SPR#435.
	 */
	if (startSoFar != 0) {
		wrapWidth--;
	}

	/*
	 * Try to avoid sweeping the whole line looking for line breaks by
	 * first checking near the end of the line
	 */
	size_t	guessIndex = 0;

	const	size_t	kCharsFromEndToSearchFrom		=		5;	// should be half of average word size (or so)
	size_t	bestBreakPointIndex = 1;
	for (; bestBreakPointIndex <= length; bestBreakPointIndex++) {
		Led_Distance	guessWidth		=	LookupLengthInVector (widthsVector, startSoFar, bestBreakPointIndex);
		if (guessWidth > wrapWidth) {
			if (bestBreakPointIndex > 1) {
				bestBreakPointIndex--;	// because overshot above
			}
			#if		qMultiByteCharacters
				bestBreakPointIndex = AdjustToValidCharIndex (text, bestBreakPointIndex);
			#endif

			if (bestBreakPointIndex > (kCharsFromEndToSearchFrom+5)) {	// no point on a short search
				Led_Assert (bestBreakPointIndex > kCharsFromEndToSearchFrom);
				guessIndex = bestBreakPointIndex - kCharsFromEndToSearchFrom;
				#if		qMultiByteCharacters
					guessIndex = AdjustToValidCharIndex (text, guessIndex);
				#endif
			}
			break;
		}
	}

	if (bestBreakPointIndex >= length) {
		Led_Assert (bestBreakPointIndex <= length+1);	// else last char in text with be 1/2 dbcs char
		bestRowLength = length;
		Led_Assert (guessIndex == 0);
	}
	else {
		size_t	wordWrapMax	=	(Led_NextChar (&text[bestBreakPointIndex]) - text);
		Led_Assert (wordWrapMax <= length);	// cuz only way could fail is if we had split character, or were already at end, in which case
											// we'd be in other part of if-test.

		if (guessIndex != 0) {
			bestRowLength = TryToFindWrapPointForMeasuredText1 (text, length, wrapWidth, offsetToMarkerCoords, widthsVector, startSoFar, guessIndex, wordWrapMax);

			if (bestRowLength == 0) {
				if (bestBreakPointIndex > (kCharsFromEndToSearchFrom*3+5)) {	// no point on a short search
					guessIndex = bestBreakPointIndex - kCharsFromEndToSearchFrom*3;
					#if		qMultiByteCharacters
						guessIndex = AdjustToValidCharIndex (text, guessIndex);
					#endif
					bestRowLength = TryToFindWrapPointForMeasuredText1 (text, length, wrapWidth, offsetToMarkerCoords, widthsVector, startSoFar, guessIndex, wordWrapMax);
				}
			}
		}
		if (bestRowLength == 0) {
			bestRowLength = TryToFindWrapPointForMeasuredText1 (text, length, wrapWidth, offsetToMarkerCoords, widthsVector, startSoFar, 0, wordWrapMax);

			if (bestRowLength == 0) {
				/*
				 *		If we got here then there was no good breaking point - we must have one VERY long word
				 *	(or a relatively narrow layout width).
				 */
				Led_Assert (bestBreakPointIndex == FindWrapPointForOneLongWordForMeasuredText (text, length, wrapWidth, offsetToMarkerCoords, widthsVector, startSoFar));
				bestRowLength = bestBreakPointIndex;
			}
		}
	}

	Led_Assert (bestRowLength > 0);
	#if		qMultiByteCharacters
		Assert_CharPosDoesNotSplitCharacter (offsetToMarkerCoords + bestRowLength);
	#endif
	return (bestRowLength);
}

size_t	WordWrappedTextImager::TryToFindWrapPointForMeasuredText1 (const Led_tChar* text, size_t length, Led_Distance wrapWidth,
											#if		qMultiByteCharacters
												size_t offsetToMarkerCoords,
											#else
												size_t /*offsetToMarkerCoords*/,
											#endif
											const Led_Distance* widthsVector, size_t startSoFar,
											size_t searchStart, size_t wrapLength
									)
{
	Led_AssertNotNil (widthsVector);
#if		qMultiByteCharacters
	Led_Assert (Led_IsValidMultiByteString (text, length));
	Led_Assert (Led_IsValidMultiByteString (text, wrapLength));
	Assert_CharPosDoesNotSplitCharacter (offsetToMarkerCoords + searchStart);
#endif


	Led_Assert (wrapLength <= length);

	Led_RefCntPtr<TextBreaks>	breaker	=	GetTextStore ().GetTextBreaker ();

	/*
	 *	We take a bit of text here - and decide the proper position in the text to make the break.
	 *	return 0 for bestRowLength if we could not find a good breaking point
	 */
	Led_AssertNotNil (text);
	size_t			bestRowLength	=	0;
	Led_Distance	width			=	0;
	size_t			wordEnd			=	0;
	bool			wordReal		=	false;
	size_t	lastLineTest = 0;
	for (size_t	i = searchStart; i < wrapLength;) {
		// skip nonwhitespace, characters - then measure distance.
		lastLineTest = i;
		breaker->FindLineBreaks (text, wrapLength, i, &wordEnd, &wordReal);

		Led_Assert (i < wordEnd);
		width	=	LookupLengthInVector (widthsVector, startSoFar, wordReal? wordEnd: i);
		i = wordEnd;

		Led_Assert (i > 0);
		Led_Assert (i <= wrapLength);

		/*
		 *	This code to only break if wordReal has the effect of "eating" up a string of
		 *	spaces at the end of the row. I'm not 100% this is always the "right" thing
		 *	todo, but seems ot be what is done most of the time in most editors. We will
		 *	do so unconditionarlly - at least for now.
		 */
		if (width > wrapWidth) {
			break;
		}
		else {
			bestRowLength = i;
		}
	}
	
	if ((not wordReal) and (wrapLength < length) and (bestRowLength != 0)) {
		// may be a lot of trailing whitespace that could be lost
		breaker->FindLineBreaks (text, length, lastLineTest, &wordEnd, &wordReal);
		Led_Assert (not wordReal);
		bestRowLength = wordEnd;
	}

	#if		qMultiByteCharacters
		Assert_CharPosDoesNotSplitCharacter (offsetToMarkerCoords + bestRowLength);
	#endif

	return (bestRowLength);
}

size_t	WordWrappedTextImager::FindWrapPointForOneLongWordForMeasuredText (
											#if		qMultiByteCharacters
												const Led_tChar* text,
											#else
												const Led_tChar* /*text*/,
											#endif
											size_t length, Led_Distance wrapWidth,
											size_t offsetToMarkerCoords,
											const Led_Distance* widthsVector, size_t startSoFar
										)
{
	size_t	bestRowLength	=	0;

	// Try binary search to find the best point to break up the
	// really big word. But don't bother with all the fancy stuff. Just take the charwidth of
	// the first character as an estimate, and then spin up or down til we get just the
	// right length...
	size_t			secondCharIdx	=	FindNextCharacter (offsetToMarkerCoords+0);
	Led_Assert (secondCharIdx >= offsetToMarkerCoords);
	Led_Distance	fullWordWidth	=	LookupLengthInVector (widthsVector, startSoFar, length);

	Led_Assert (length >= 1);
	size_t			guessIdx		=	size_t ((length-1) * (float (wrapWidth)/float (fullWordWidth)));
	Led_Assert (guessIdx < length);

	/*
	 *	Note - at this point guessIdx may not be on an even character boundary.
	 *	So our first job is to make sure it doesn't split a character.
	 */
	#if		qMultiByteCharacters
		/*
		 *	See if guessIndex is on a real character boundary - and if not - then step back one.
		 */
		guessIdx = AdjustToValidCharIndex (text, guessIdx);
	#endif
	Led_Assert (guessIdx < length);

	Led_Distance	guessWidth		=	LookupLengthInVector (widthsVector, startSoFar, guessIdx);
	bestRowLength = guessIdx;

	if (guessWidth > wrapWidth) {
		// keeping going down til we are fit.
		for (size_t	j = guessIdx; j >= 1; j = FindPreviousCharacter (offsetToMarkerCoords+j)-offsetToMarkerCoords) {
			Led_Assert (j < length);	// no wrap
			Led_Distance	smallerWidth	=	LookupLengthInVector (widthsVector, startSoFar, j);
			bestRowLength = j;
			if (smallerWidth <= wrapWidth) {
				break;
			}
		}
	}
	else {
		// keeping going down til we are fit.
		for (size_t	j = guessIdx; j < length; j = FindNextCharacter (offsetToMarkerCoords+j)-offsetToMarkerCoords) {
			Led_Assert (j < length);	// no wrap
			Led_Distance	smallerWidth	=	LookupLengthInVector (widthsVector, startSoFar, j);
			if (smallerWidth > wrapWidth) {
				break;
			}
			bestRowLength = j;
		}
	}

	// Must always consume at least one character, even if it won't fit entirely
	if (bestRowLength == 0) {
		#if		qMultiByteCharacters
			if (Led_IsLeadByte (text[0])) {
				bestRowLength = 2;
			}
			else {
				bestRowLength = 1;
			}
		#else
			bestRowLength = 1;
		#endif
	}

	return (bestRowLength);
}





#if		qLedUsesNamespaces
}
#endif



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

