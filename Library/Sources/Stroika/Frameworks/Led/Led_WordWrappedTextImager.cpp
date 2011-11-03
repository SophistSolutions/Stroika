/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<cctype>

#include	"Led_WordWrappedTextImager.h"








#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {






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




			// Now OVERRIDE the above for soft-breaks...
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





		}
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

