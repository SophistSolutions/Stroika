/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/TextStore.cpp,v 2.63 2004/01/25 23:54:55 lewis Exp $
 *
 * Changes:
 *	$Log: TextStore.cpp,v $
 *	Revision 2.63  2004/01/25 23:54:55  lewis
 *	SPR#1622: fixed special case for TextStore::GetStartOfLine () - with empty last line and we ask get start of that line
 *	
 *	Revision 2.62  2003/12/11 19:14:47  lewis
 *	SPR#1593: Document and overload TextStore::FindWordBreaks () and TextStore::FindLineBreaks (). Overload takes diff TextBreaks proc optinally.
 *	
 *	Revision 2.61  2003/03/27 16:35:40  lewis
 *	SPR#1384: TextStore::Replace () must use UpdateInfo (realContentUpdate <== true)
 *	
 *	Revision 2.60  2003/03/21 14:48:06  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.59  2003/03/19 16:43:47  lewis
 *	SPR#1354 - wrap old version of DoAboutToUpdate/DoDidUpdateCalls with qSupportLed30CompatAPI
 *	
 *	Revision 2.58  2003/03/19 13:44:53  lewis
 *	SPR#1352 - Added new versions of DoAboutToUpdateCalls/DoDIdUpdateCalls taking (essentially)
 *	iteratator boudns instead of vector(must fix vector-based version for backward compat - do soon).
 *	Also Added TextStore::SmallStackBufferMarkerSink class and MarkersOfATypeMarkerSink2SmallStackBuffer
 *	and use SmallStackBufferMarkerSink in SimpleUpdater (and use BlockAllocation for SimpleUpdaters).
 *	A few other cleanups / speed tweeks
 *	
 *	Revision 2.57  2003/03/16 16:15:34  lewis
 *	SPR#1345 - as a small part of this SPR - switch to using SimpleUpdater in place of direct calls
 *	to DoAboutToUpdateCalls()
 *	
 *	Revision 2.56  2003/02/28 19:09:21  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.55  2002/11/18 21:11:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.54  2002/11/17 15:55:33  lewis
 *	SPR#1173- Replace/ReplaceWithoutUpdate change
 *	
 *	Revision 2.53  2002/10/11 01:12:53  lewis
 *	hack so compiles with VC6
 *	
 *	Revision 2.52  2002/10/09 13:50:56  lewis
 *	SPR#1121- TextStore::DoDidUpdateCalls () keeps list of markerowners in a temporary object in case
 *	someone temporarily adds and then removes a markerowner from the markerowners list
 *	
 *	Revision 2.51  2002/09/19 14:14:51  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.50  2002/05/06 21:33:59  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.49  2001/11/27 00:30:04  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.48  2001/10/17 20:43:04  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.47  2001/09/12 16:20:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.46  2001/09/03 18:54:59  lewis
 *	SPR#1009- Added MarkerOwner::EarlyDidUpdateText () and calls to such from TextStore::DoDidUpdateCalls ()
 *	
 *	Revision 2.45  2001/08/29 23:36:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.44  2001/08/28 18:43:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.43  2001/07/31 15:07:52  lewis
 *	use static_cast<> to quiet a few compiler warnings
 *	
 *	Revision 2.42  2001/04/13 16:31:50  lewis
 *	SPR#0864- Added qUseWin32CompareStringCallForCaseInsensitiveSearch support
 *	
 *	Revision 2.41  2001/03/05 21:17:05  lewis
 *	SPR#0855- be sure TextStore itself is part of MarkerOwner list (so it gets adjusted by new
 *	code in ChunkedArrayTextStore which keeps separate marker trees for each owner)
 *	
 *	Revision 2.40  2000/10/19 15:50:56  lewis
 *	bad message on last checkin. Really fixed bug in FIND when called with empty buffer and
 *	empty search string and WHOLEWORD search
 *	
 *	Revision 2.39  2000/10/19 15:50:23  lewis
 *	lots more work on dialogs on Mac. Use dlgx resource to support movable modal. Cleanup display.
 *	Support togglying checkbox, and full grabing / setting of values in dialog. Support event
 *	filterproc. Now portable mac/windows dialog support does about everything I need to
 *	replace what I was getting from PowerPlant
 *	
 *	Revision 2.38  2000/07/26 05:46:37  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner arg to
 *	CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table of speedups.
 *	
 *	Revision 2.37  2000/06/15 22:28:45  lewis
 *	SPR#0778- Made TextStore own a smart pointer to a TextBreak object - and by default
 *	create one with the old algorithms.
 *	
 *	Revision 2.36  2000/06/15 20:17:52  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.35  2000/06/15 20:01:50  lewis
 *	SPR#0778- Broke out old word/line break code from TextStore - in preparation for
 *	making it virtual/generic - and doing different algorithms (UNICODE)Headers/TextBreaks.h
 *	
 *	Revision 2.34  2000/04/16 14:39:39  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.33  2000/04/14 22:40:28  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.32  1999/11/13 16:32:24  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.31  1999/06/28 19:40:40  lewis
 *	spr#0599- DoAboutTo/DoDidUpdate calls now use iterators to navigate, and do
 *	things on DIDUPDATE in reverse order using reverse iterators
 *	
 *	Revision 2.30  1999/06/15 02:31:48  lewis
 *	spr#0584 - fix TextStore::Find () for case where searchTo != eUseSearchParameters
 *	
 *	Revision 2.29  1999/05/03 22:05:22  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.28  1998/10/30 14:33:03  lewis
 *	vector<> instead of Led_Array<>
 *	MSVC60 warnings pragma changes.
 *	
 *	Revision 2.27  1998/07/24  01:16:27  lewis
 *	Fix UNICODE bugs with the KinsokuRule/Jap lin end stuff - it was directlyl calling
 *	CTYPE routines with UNICODE chars. I guess thats
 *	wrong. Use new (private to this file) inline functions to check this stuff.
 *
 *	Revision 2.26  1998/06/03  01:53:40  lewis
 *	Don't assert WideCharToMultiByte() returned 1 or 2 - could return 0 also -
 *	if no SJIS code page
 *	isntalled. In that case - return 0/false for kutenrow, etc...
 *
 *	Revision 2.25  1998/04/25  01:39:34  lewis
 *	Lots of changes - mostly to FindWordBreak/FindLineBReak() code - to support
 *	Kinsoku Rule, UNICODE, and Jap word selection algorithms.
 *	With UNICODE - these all work fine now (based on old Led 1.0 code -
 *	warmed over, and quite ugly).
 *
 *	Revision 2.24  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.23  1997/09/29  15:49:39  lewis
 *	Lose qLedFirstIndex support.
 *	TextStore::VectorMarkerSink
 *
 *	Revision 2.22  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.21  1997/07/23  23:10:52  lewis
 *	docs changes.
 *	And fixed a bunch of bugs (hopefully all?) with qLedFirstIndex
 *
 *	Revision 2.20  1997/07/13  02:17:44  lewis
 *	AutoDoc support
 *	Support qLedFirstIndex == 0.
 *	TextStore_ renamed TextStore.
 *
 *	Revision 2.19  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1997/06/18  02:59:46  lewis
 *	TextStore_::ArrayMarkerSink.
 *	qIncludePrefixFile
 *
 *	Revision 2.17  1997/01/10  03:09:16  lewis
 *	New throw specifers (so no need for debugging crap to catch illicit exceptions).
 *	And AboutToUpdateText () now returns VOID. So no more testing it.
 *
 *	Revision 2.16  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.15  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1996/09/30  14:27:02  lewis
 *	Changed Find() method to take SearchParameters struct - instead of lots of little params.
 *
 *	Revision 2.13  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.12  1996/07/03  01:16:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.11  1996/06/01  02:12:37  lewis
 *	Lose no-RTTI compat code.
 *	New FindFirstWordStartBeforePosition ().
 *	Various word start/word-break bug fixes.
 *
 *	Revision 2.10  1996/05/04  21:06:57  lewis
 *	TextStore::Find() now handles case-insensative and word-boundary
 *	searches correctly.
 *
 *	Revision 2.9  1996/03/16  18:46:50  lewis
 *	Invariant_ () now const
 *
 *	Revision 2.8  1996/02/26  18:55:41  lewis
 *	Use kBadIndex insteead of size_t (-1).
 *	Use Led_Min/etc instead of Min etc.
 *
 *	Revision 2.7  1996/01/22  05:27:21  lewis
 *	Quick and dirty first implementation of TextStore_::Find ().
 *
 *	Revision 2.6  1996/01/11  08:19:46  lewis
 *	TextStore_::FindFirstWordStartStrictlyBeforePosition andTextStore_::FindFirstWordEndAfterPosition now
 *	take xtra wordMustBeReal bool param.
 *
 *	Revision 2.5  1995/12/08  07:10:05  lewis
 *	must const_cast Text_Store_, this in PeekAtTextStore()
 *
 *	Revision 2.4  1995/12/06  01:37:45  lewis
 *	PeekAtTextStore() defaults to returning this, for a textStore.
 *
 *	Revision 2.3  1995/10/19  22:25:53  lewis
 *	New DoAboutToUpdateCalls/DoDidUpdateCalls helper functions.
 *
 *	Revision 2.2  1995/10/09  22:43:23  lewis
 *	Fixed the ASCII mode bug with word wrapping code.
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.13  1995/06/08  05:17:44  lewis
 *	Code cleanups - e.g lose qBenchmarkFindLineBreaks - STERL had
 *	accidentally left this on!!!
 *
 *	Revision 1.12  1995/06/04  04:27:19  lewis
 *	Fix TextStore_::GetStartOfLineContainingPosition () and
 *	TextStore_::GetEndOfLineContainingPosition () bugs with the read-in-chunks
 *	code. Off by one in when to read. Caused occasional assert errors and
 *	occasional failures on dbl-click. SPR 0311.
 *
 *	Revision 1.11  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.10  1995/05/20  04:58:23  lewis
 *	Fix spelling of Invarient
 *
 *	Revision 1.9  1995/05/18  08:11:13  lewis
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *
 *	Revision 1.8  1995/05/12  21:11:10  lewis
 *	Copied old FindLineBreaks to TextStore_::FindLineBreaks_ReferenceImplementation ().
 *	Then did minor tweeks to FindLineBreaks() and check they
 *	are valid with compare.
 *	STERL then added debugging stuff (statictic counts) which will be ripped out
 *	soon.
 *
 *	Revision 1.7  1995/05/06  19:44:40  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *	TextStore_::CharacterToTCharIndex/TextStore_::TCharToCharacterIndex - SPR 0232.
 *
 *	Revision 1.6  1995/05/05  19:51:21  lewis
 *	Led_SmallStackBuffer now templated class.
 *
 *	Revision 1.5  1995/03/23  03:57:23  lewis
 *	Renamed Led_PrevioustChar () ==> Led_PreviousChar () (SPR 178).
 *	Fixed assert error in FindPreviousCharacter () (<= vs <)
 *	Check for valid SJIS in TextStore Invariant (SPR 109).
 *
 *	Revision 1.4  1995/03/13  03:27:29  lewis
 *	Use Led_Array<T> instead of Array<T>.
 *	Fixed bug in FindWordBreaks (LedSPR#165).
 *	Speed tweek to TextStore_::GetStartOfLineContainingPosition () and EOL
 *	(LedSPR#166).
 *
 *	Revision 1.3  1995/03/06  21:33:46  lewis
 *	Fix FindPreviousCharacter() to use Led_IsLeadByte() instead of
 *	not isascii() (see comment in the code for justification).
 *
 *	Also - code to FindWordBreaks() was wrong - it scanned back while cur>startOfText.
 *	It needed to go up to cur >= startOfText so it could notice case of space in
 *	first character (Led SPR #157).
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

#include	"LedConfig.h"

#include	"TextStore.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif





#if		qLedUsesNamespaces
	namespace	Led {
#endif


	inline	bool	IsASCIIAlpha (int c)
		{
			return isascii (c) and isalpha (c);
		}





/*
 ********************************************************************************
 ********************** TextStore::VectorMarkerSink *****************************
 ********************************************************************************
 */
/*
@METHOD:		TextStore::VectorMarkerSink::Append
@DESCRIPTION:	<p>Don't call directly. Called as a by-product of TextStore::CollectAllMarkersInRange ().
	This TextStore::VectorMarkerSink produces a vector for such collect calls.</p>
*/
void	TextStore::VectorMarkerSink::Append (Marker* m)
{
	Led_RequireNotNil (m);
	Led_AssertNotNil (fMarkers);
	//fMarkers->push_back (m);
	PUSH_BACK(*fMarkers, m);
}







/*
 ********************************************************************************
 ******************* TextStore::SmallStackBufferMarkerSink **********************
 ********************************************************************************
 */
/*
@METHOD:		TextStore::SmallStackBufferMarkerSink::Append
@DESCRIPTION:	<p>Don't call directly. Called as a by-product of TextStore::CollectAllMarkersInRange ().
	This TextStore::VectorMarkerSink produces a @'Led_SmallStackBuffer<T>' for such collect calls.</p>
*/
void	TextStore::SmallStackBufferMarkerSink::Append (Marker* m)
{
	Led_RequireNotNil (m);
	Led_AssertNotNil (fMarkers);
	fMarkers.push_back (m);
}








/*
 ********************************************************************************
 ********************************** TextStore ***********************************
 ********************************************************************************
 */
/*
@METHOD:		TextStore::kAnyMarkerOwner
@DESCRIPTION:
	<p>Sentinal value meaning to match on ANY marker owner for @'TextStore::CollectAllMarkersInRangeInto' etc calls.</p>
*/
const	MarkerOwner*	TextStore::kAnyMarkerOwner	=	reinterpret_cast<MarkerOwner*> (1);

TextStore::~TextStore ()
{
	Led_Require (fMarkerOwners.size () == 1 and fMarkerOwners[0] == this);	// better have deleted 'em all by now (except for US)!
}

#if		qMultiByteCharacters
// qSingleByteCharacters || qWideCharacters cases in headers
size_t	TextStore::CharacterToTCharIndex (size_t i)
{
	size_t	tCharIndex = 1;
	int	ii	=	i;
	for (; ii > 0; ii--) {
		tCharIndex = FindNextCharacter (tCharIndex);
	}
	return (tCharIndex);
}
#endif

#if		qMultiByteCharacters
// qSingleByteCharacters || qWideCharacters cases in headers
size_t	TextStore::TCharToCharacterIndex (size_t i)
{
	size_t	charCount	=	1;
	for (size_t	tCharIndex = 0; tCharIndex < i; tCharIndex = FindNextCharacter (tCharIndex)) {
		charCount++;
	}
	return (charCount);
}
#endif

/*
@METHOD:		TextStore::Replace
@DESCRIPTION:	<p>Replace the text in the range {from,to} with the withWhatCount Led_tChars of text pointed
			to by withWhat. This update will have the side effect of notifying MarkerOwners, and affected Markers.
			To avoid the marker/markerowner notification, you can call @'TextStore::ReplaceWithoutUpdate'.</p>
				<p>Note - this used to be pure virtual, and impelemented by subclasses. Now its a trivial wrapper
			on @'TextStore::ReplaceWithoutUpdate'. This change was made in Led 3.1.</p>
*/
void	TextStore::Replace (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount)
{
	Led_Require (from <= to);
	Led_Require (to <= GetEnd ());
	#if		qMultiByteCharacters
		Led_Assert (Led_IsValidMultiByteString (withWhat, withWhatCount));
		Assert_CharPosDoesNotSplitCharacter (from);
		Assert_CharPosDoesNotSplitCharacter (to);
	#endif

	if (from != to or withWhatCount != 0) {
		UpdateInfo		updateInfo (from, to, withWhat, withWhatCount, true, true);
		SimpleUpdater	updater (*this, updateInfo);
		try {
			ReplaceWithoutUpdate (from, to, withWhat, withWhatCount);
		}
		catch (...) {
			updater.Cancel ();
			throw;
		}
	}
}

/*
@METHOD:		TextStore::GetStartOfLine
@DESCRIPTION:
	<p>Returns the marker position of the start of the given line #.</p>
*/
size_t	TextStore::GetStartOfLine (size_t lineNumber) const
{
	Led_Require (lineNumber >= 0);
	Led_Require (lineNumber <= GetLineCount ());

	/*
	 *	Just walk through each char looking for '\n's and count lines. Could be
	 *	far more efficiently done elsewhere/in subclasses, but this tends to not be called directly anyhow.
	 */
	size_t	curLineNum	=	0;
	size_t	len			=	GetLength ();
	for (size_t	i = 0; i < len; i = FindNextCharacter (i)) {
		if (curLineNum == lineNumber) {
			return i;
		}
		Led_tChar	c;
		CopyOut (i, 1, &c);
		if (c == '\n') {
			curLineNum++;
		}
	}
	Led_Assert  (curLineNum == lineNumber);
	return len;	// this can happen when the last line is at end of buffer
}

/*
@METHOD:		TextStore::GetEndOfLine
@DESCRIPTION:
	<p>Returns the marker position of the end of the given line #.</p>
*/
size_t	TextStore::GetEndOfLine (size_t lineNumber) const
{
// LGP 961129 - not 100% sure this is right - quickie implementation...
	Led_Require (lineNumber >= 0);
	Led_Require (lineNumber <= GetLineCount ());

	/*
	 *	Just walk through each char looking for '\n's and count lines. Could be
	 *	far more efficiently done elsewhere/in subclasses, but this tends to not be called directly anyhow.
	 */
	size_t	curLineNum	=	0;
	size_t	len			=	GetLength ();
	for (size_t	i = 0; i < len; i = FindNextCharacter (i)) {
		Led_tChar	c;
		CopyOut (i, 1, &c);
		if (c == '\n') {
			if (curLineNum == lineNumber) {
				return i;
			}
			curLineNum++;
		}
	}
	return len;
}

/*
@METHOD:		TextStore::GetLineContainingPosition
@DESCRIPTION:
	<p>Returns the # of the line which contains the given charPosition. NB: we use charPosition here
	to deal with the ambiguity of what line a markerPosition is at when it straddles two lines.</p>
*/
size_t	TextStore::GetLineContainingPosition (size_t charPosition) const
{
// LGP 961129 - not 100% sure this is right - quickie implementation...
	Led_Assert (charPosition >= 0);
	Led_Assert (charPosition <= GetEnd ());
	size_t	curLineNum	=	0;
	size_t	lineStart	=	0;
	size_t	lineEnd		=	0;
	size_t	len			=	GetLength ();
	for (size_t	i = 0; i < len; i = FindNextCharacter (i)) {
		lineEnd = i+1;
		if (Contains (lineStart, lineEnd, charPosition)) {
			return (curLineNum);
		}
		Led_tChar	c;
		CopyOut (i, 1, &c);
		if (c == '\n') {
			curLineNum++;
			lineStart =	i;
			lineEnd =	i;
		}
	}
	return curLineNum;
}

/*
@METHOD:		TextStore::GetLineCount
@DESCRIPTION:
	<p>Returns the # of the lines in the TextStore.</p>
*/
size_t	TextStore::GetLineCount () const
{
// LGP 961129 - not 100% sure this is right - quickie implementation...
	/*
	 *	Just walk through each char looking for '\n's and count lines. Could be
	 *	far more efficiently done, but this tends to not be called directly anyhow.
	 */
	size_t	curLineNum	=	0;
	size_t	len			=	GetLength ();
	for (size_t	i = 0; i < len; i = FindNextCharacter (i)) {
		Led_tChar	c;
		CopyOut (i, 1, &c);
		if (c == '\n') {
			curLineNum++;
		}
	}
	return curLineNum;
}

/*
@METHOD:		TextStore::GetStartOfLineContainingPosition
@DESCRIPTION:
	<p>Returns the marker position of the start of the line contains the character after 'afterPos'.</p>
*/
size_t	TextStore::GetStartOfLineContainingPosition (size_t afterPos) const
{
	Led_Assert (afterPos >= 0);
	Led_Assert (afterPos <= GetEnd ());

	// Scan back looking for '\n', and the character AFTER that is the start of this line...

	// Note, that this is OK todo a character at a time going back because '\n' is NOT
	// a valid second byte (at least in SJIS - probaly should check out other MBYTE
	// charsets - assert good enuf for now)!!!
	#if		qMultiByteCharacters
		Led_Assert (not Led_IsValidSecondByte ('\n'));
	#endif

	/*
	 *	The main thing making this slightly non-trival is that we try to perform the charsearch
	 *	using chunked reads to make things a bit faster.
	 */
	size_t			lastReadAtPos	=	afterPos;		// set as past where we need the text to be to force a read
	Led_tChar		charBuf[64];
	const size_t	kBufSize		=	sizeof (charBuf)/sizeof(charBuf[0]);
	Led_Assert (afterPos < INT_MAX);	// cuz of casts - cannot go up to UINT_MAX
	for (int curPos = afterPos-1; curPos > -1; curPos--) {
		Led_Assert (curPos >= 0);
		if (lastReadAtPos > static_cast<size_t> (curPos)) {
			if (lastReadAtPos > kBufSize) {
				lastReadAtPos -= kBufSize;
			}
			else {
				lastReadAtPos = 0;
			}
			Led_Assert (lastReadAtPos >= 0);
			CopyOut (lastReadAtPos, Led_Min (kBufSize, GetLength ()), charBuf);
		}
		Led_Assert (curPos-lastReadAtPos < kBufSize);
		Led_tChar&	thisChar	=	charBuf[curPos-lastReadAtPos];

		// sensible todo but too slow - this code is only called in DEBUG mode and its performance critical there...
		#if		qDebug && 0
		{	
			Led_tChar xxx;
			CopyOut (curPos, 1, &xxx);
			Led_Assert (xxx == thisChar);
		}
		#endif

		if (thisChar == '\n') {
			return (curPos + 1);
		}
	}
	return (0);
}

/*
@METHOD:		TextStore::GetEndOfLineContainingPosition
@DESCRIPTION:
	<p>Returns the marker position of the end of the line contains the character after 'afterPos'.</p>
*/
size_t	TextStore::GetEndOfLineContainingPosition (size_t afterPos) const
{
	Led_Assert (afterPos >= 0);
	Led_Assert (afterPos <= GetEnd ());

	// Scan forward looking for '\n' (and don't count NL - just like GetEndOfLine())

	// Note, that this is OK todo a character at a time going back because '\n' is NOT
	// a valid second byte (at least in SJIS - probaly should check out other MBYTE
	// charsets - assert good enuf for now)!!!
	#if		qMultiByteCharacters
		Led_Assert (not Led_IsValidSecondByte ('\n'));
	#endif

	/*
	 *	The main thing making this slightly non-trival is that we try to perform the charsearch
	 *	using chunked reads to make things a bit faster.
	 */
	size_t	lastReadAtPos	=	0;		// flag no read so far
	Led_tChar		charBuf[64];
	const size_t	kBufSize		=	sizeof (charBuf)/sizeof(charBuf[0]);
	for (size_t curPos = afterPos; curPos+1 <= GetLength (); curPos++) {
		if (lastReadAtPos == 0 or lastReadAtPos + kBufSize <= curPos) {
			lastReadAtPos = curPos;
			Led_Assert (lastReadAtPos >= 0);
			CopyOut (lastReadAtPos, Led_Min (kBufSize, GetLength ()-(lastReadAtPos)), charBuf);
		}
		Led_Assert (curPos-lastReadAtPos < kBufSize);
		Led_tChar&	thisChar	=	charBuf[curPos-lastReadAtPos];
		#if		qDebug
		{	
			Led_tChar xxx;
			CopyOut (curPos, 1, &xxx);
			Led_Assert (xxx == thisChar);
		}
		#endif
		if (thisChar == '\n') {
			return (curPos);
		}
	}
	return (GetEnd ());
}

/*
@METHOD:		TextStore::FindPreviousCharacter
@DESCRIPTION:	<p>Returns the marker position of the previous character. If at the start of the buffer, it returns
	0. Use this instead of pos--, in order to deal properly with multibyte character sets.</p>
*/
size_t	TextStore::FindPreviousCharacter (size_t beforePos) const
{
	Led_Assert (beforePos >= 0);
	if (beforePos == 0) {
		return (0);
	}
	else {
		#if		qMultiByteCharacters
			/*
			 *	This code is based on the optimized Led_PreviousChar() in LedSupport, only it has
			 *	been adapted to only call CopyOut() on as much text as it needs.
			 */
			const	size_t	chunkSize	=	16;		// how many characters we read at a time from the TextStore
			Led_tChar		_stackBuf_[1024];
			Led_tChar*		buf = _stackBuf_;

			size_t	bytesReadOutSoFar	=	0;
			const Led_tChar* cur		=	NULL;
			bool	notFoundSyncPoint	=	true;
			for (; notFoundSyncPoint; ) {					// we return out when we find what we are looking for...

				Led_Assert (bytesReadOutSoFar < beforePos);
				bytesReadOutSoFar += chunkSize;
				bytesReadOutSoFar = Led_Min (bytesReadOutSoFar, beforePos-1);		// don't read more than there is!!!
				Led_Assert (bytesReadOutSoFar > 0);
				if (bytesReadOutSoFar > sizeof (_stackBuf_)) {
					if (buf != _stackBuf_) {
						delete[] buf;
					}
					buf = NULL;
					buf = new Led_tChar [bytesReadOutSoFar];
				}
				Led_Assert (beforePos > bytesReadOutSoFar);
				CopyOut (beforePos-bytesReadOutSoFar, bytesReadOutSoFar, buf);

				const Led_tChar* fromHere	=	&buf[bytesReadOutSoFar];

				/*
				 *	If the previous byte is a lead-byte, then the real character boundsary
				 *	is really TWO back.
				 *
				 *	Proof by contradiction:
				 *		Assume prev character is back one byte. Then the chracter it is part of
				 *	contains the first byte of the character we started with. This obviously
				 *	cannot happen. QED.
				 *
				 *		This is actually a worth-while test since lots of second bytes look quite
				 *	a lot like lead-bytes - so this happens a lot.
				 */
				if (Led_IsLeadByte (*(fromHere-1))) {
					Led_Assert (fromHere-buf >= 2);		// else split character...
					return ((fromHere - 2 - buf) + beforePos-bytesReadOutSoFar);
				}

				if (bytesReadOutSoFar == 1) {
					return (beforePos-1);	// if there is only one byte to go back, it must be an ASCII byte
				}

				// we go back by BYTES til we find a syncronization point
				for (cur = fromHere-2; cur > buf; cur--) {
					Led_Assert (cur >= buf);
					if (not Led_IsLeadByte (*cur)) {
						// Then we are in case 1, 2, 3, 4 or 6 (not 5 or 7). So ew know we are looking
						// at an ASCII byte, or a secondbyte. Therefore - the NEXT byte from here must be
						// a valid mbyte char boundary.
						cur++;
						notFoundSyncPoint = false;
						break;
					} 
				}
				if (bytesReadOutSoFar == beforePos-1) {
					// we've read all there is - so we must be at a sync-point - regardless.
					notFoundSyncPoint = false;
				}
			}
			Led_Assert (not notFoundSyncPoint);
			Led_Assert (cur >= buf);

			// Now we are pointing AT LEAST one mbyte char back from 'fromHere' so scan forward as we used
			// to to find the previous character...
			const Led_tChar* fromHere	=	&buf[bytesReadOutSoFar];
			Led_Assert (cur < fromHere);
			Led_Assert (Led_IsValidMultiByteString (cur, bytesReadOutSoFar - (cur-buf)));
			for (; cur < fromHere; ) {
				const Led_tChar*	next = Led_NextChar (cur);
				if (next == fromHere) {
					if (buf != _stackBuf_) {
						delete[] buf;
					}
					return ((cur-buf) + beforePos-bytesReadOutSoFar);
				}
				Led_Assert (next < fromHere);	// if we've gone past - then fromHere must have split a mbyte char!
				cur = next;
			}
			Led_Assert (false);	return (0);
		#else
			size_t	newPos	=	beforePos - 1;
			Led_Ensure (newPos >= 0);	// cuz we cannot split Wide-Characters, and if we were already at beginning
										// of buffer, we'd have never gotten to this code
			return (newPos);
		#endif
	}
}

/*
@METHOD:		TextStore::FindWordBreaks
@DESCRIPTION:	<p>NB: *wordEndResult refers to the MARKER (as opposed to character) position after the
		end of the word - so for example - the length of the word is *wordEndResult-*wordStartResult.
		*wordStartResult and *wordEndResult refer to actual marker positions in this TextStore.</p>
</p>
*/
void	TextStore::FindWordBreaks (size_t afterPosition, size_t* wordStartResult, size_t* wordEndResult, bool* wordReal, TextBreaks* useTextBreaker)
{
	Led_AssertNotNil (wordStartResult);
	Led_AssertNotNil (wordEndResult);
	Led_AssertNotNil (wordReal);
	size_t	startOfThisLine	=	GetStartOfLineContainingPosition (afterPosition);
	size_t	endOfThisLine	=	GetEndOfLineContainingPosition (afterPosition);
	size_t	len				=	endOfThisLine-startOfThisLine;

	Led_SmallStackBuffer<Led_tChar>	buf (len);
	CopyOut (startOfThisLine, len, buf);

	Led_Assert (afterPosition >= startOfThisLine);
	Led_Assert (afterPosition <= endOfThisLine);
	size_t	zeroBasedStart	=	0;
	size_t	zeroBasedEnd	=	0;
	if (useTextBreaker == NULL) {
		useTextBreaker = GetTextBreaker ();
	}
	Led_AssertNotNil (useTextBreaker);
	useTextBreaker->FindWordBreaks (buf, len, afterPosition-startOfThisLine, &zeroBasedStart, &zeroBasedEnd, wordReal);
	Led_Assert (zeroBasedStart <= zeroBasedEnd);
	Led_Assert (zeroBasedEnd <= len);
	*wordStartResult = zeroBasedStart + startOfThisLine;
	*wordEndResult = zeroBasedEnd + startOfThisLine;
}

/*
@METHOD:		TextStore::FindLineBreaks
@DESCRIPTION:	<p>NB: *wordEndResult refers to the MARKER (as opposed to character) position after the
		end of the word. *wordEndResult refers to actual marker position in this TextStore.</p>
</p>
*/
void	TextStore::FindLineBreaks (size_t afterPosition, size_t* wordEndResult, bool* wordReal, TextBreaks* useTextBreaker)
{
	Led_AssertNotNil (wordEndResult);
	Led_AssertNotNil (wordReal);
	size_t	startOfThisLine	=	GetStartOfLineContainingPosition (afterPosition);
	size_t	endOfThisLine	=	GetEndOfLineContainingPosition (afterPosition);
	size_t	len				=	endOfThisLine-startOfThisLine;

	Led_SmallStackBuffer<Led_tChar>	buf (len);
	CopyOut (startOfThisLine, len, buf);

	Led_Assert (afterPosition >= startOfThisLine);
	Led_Assert (afterPosition <= endOfThisLine);
	size_t	zeroBasedEnd	=	0;
	if (useTextBreaker == NULL) {
		useTextBreaker = GetTextBreaker ();
	}
	Led_AssertNotNil (useTextBreaker);
	useTextBreaker->FindLineBreaks (buf, len, afterPosition-startOfThisLine, &zeroBasedEnd, wordReal);
	Led_Assert (zeroBasedEnd <= len);
	*wordEndResult = zeroBasedEnd + startOfThisLine;
}

size_t	TextStore::FindFirstWordStartBeforePosition (size_t position, bool wordMustBeReal)
{
	/*
	 *	Quick and dirty algorithm. This is quite in-efficient - but should do
	 *	for the time being.
	 *
	 *	Start with a goal of position and start looking for words.
	 *	The first time we find one whose start is before position - we are done.
	 *	Keep sliding the goal back til this is true.
	 */
	for (size_t goalPos = position; goalPos > 0; goalPos = FindPreviousCharacter (goalPos)) {
		size_t	wordStart	=	0;
		size_t	wordEnd		=	0;
		bool	wordReal	=	false;
		FindWordBreaks (goalPos, &wordStart, &wordEnd, &wordReal);
		if (wordStart <= position and (not wordMustBeReal or wordReal)) {
			return (wordStart);
		}
	}
	return (0);
}

size_t	TextStore::FindFirstWordStartStrictlyBeforePosition (size_t position, bool wordMustBeReal)
{
	// maybe just FindFirstWordStartBeforePosition (POS-1)????


	/*
	 *	Quick and dirty algorithm. This is quite in-efficient - but should do
	 *	for the time being.
	 *
	 *	Start with a goal of PREVIOUS CHAR (position) and start looking for words.
	 *	The first time we find one whose start is before position - we are done.
	 *	Keep sliding the goal back til this is true.
	 */
	for (size_t goalPos = FindPreviousCharacter (position); goalPos > 0; goalPos = FindPreviousCharacter (goalPos)) {
		size_t	wordStart	=	0;
		size_t	wordEnd		=	0;
		bool	wordReal	=	false;
		FindWordBreaks (goalPos, &wordStart, &wordEnd, &wordReal);
		if (wordStart != wordEnd and wordStart < position and (not wordMustBeReal or wordReal)) {
			return (wordStart);
		}
	}
	return (0);
}

size_t	TextStore::FindFirstWordEndAfterPosition (size_t position, bool wordMustBeReal)
{
	/*
	 *	Quick and dirty algorithm. This is quite in-efficient - but should do
	 *	for the time being.
	 *
	 *	Start with a goal of PREVIOUS CHAR OF POSITION (since position might
	 *	be a word ending) and start looking for words.
	 *	The first time we find one whose end is AFTER position - we are done.
	 */
	for (size_t goalPos = FindPreviousCharacter (position); goalPos < GetEnd (); goalPos = FindNextCharacter (goalPos)) {
		size_t	wordStart	=	0;
		size_t	wordEnd		=	0;
		bool	wordReal	=	false;
		FindWordBreaks (goalPos, &wordStart, &wordEnd, &wordReal);
		if (wordStart != wordEnd and wordEnd >= position and (not wordMustBeReal or wordReal)) {
			return (wordEnd);
		}
	}
	return (GetEnd ());
}

size_t	TextStore::FindFirstWordStartAfterPosition (size_t position)
{
	/*
	 *	Quick and dirty algorithm. This is quite in-efficient - but should do
	 *	for the time being.
	 *
	 *	Start with a goal of position and start looking for words.
	 *	The first time we find one whose end is AFTER position - we are done.
	 */
	for (size_t goalPos = position; goalPos < GetEnd (); goalPos = FindNextCharacter (goalPos)) {
		size_t	wordStart	=	0;
		size_t	wordEnd		=	0;
		bool	wordReal	=	false;
		FindWordBreaks (goalPos, &wordStart, &wordEnd, &wordReal);
		if (wordStart != wordEnd and wordStart >= position and wordReal) {
			return (wordStart);
		}
	}
	return (GetEnd ());
}

/*
@METHOD:		TextStore::Find
@DESCRIPTION:	<p>Search within the given range for the text specified in <code>SearchParameters</code>. The <code>SearchParameters</code>
	specify a bunch of different matching criteria, as well. No 'regexp' searching supported as of yet.</p>
*/
size_t	TextStore::Find (const SearchParameters& params, size_t searchFrom, size_t searchTo)
{
	Led_Require (searchTo == eUseSearchParameters or searchTo <= GetEnd ());

	const	Led_tChar*	pattern		=	params.fMatchString.c_str ();
	bool				matchCase	=	params.fCaseSensativeSearch;
	bool				wholeWords	=	params.fWholeWordSearch;
	bool				wrapAtEnd	=	(searchTo == eUseSearchParameters)? params.fWrapSearch: false;

	bool	wrappedYet	=	false;
	size_t	patternLen	=	Led_tStrlen (pattern);
	size_t	bufferEnd	=	(searchTo == eUseSearchParameters)? GetEnd (): searchTo;
	size_t	searchIdx	=	searchFrom;		// index of where we are currently searching from

	Led_SmallStackBuffer<Led_tChar> lookingAtData (patternLen);

searchSMORE:
	if (searchIdx + patternLen > bufferEnd) {
		goto notFoundByBuffersEnd;
	}
	if (searchIdx >= bufferEnd) {
		goto notFoundByBuffersEnd;
	}

	// See if pattern matches current text
	CopyOut (searchIdx, patternLen, lookingAtData);
	#if		qNoSupportForNewForLoopScopingRules
	{
	#endif
	for (size_t i = 0; i < patternLen; i++) {
		bool	charsEqual	=	(lookingAtData[i] == pattern[i]);
		if (not matchCase and not charsEqual) {
			// if we are doing case-IN-sensative compare, and characters not the same, maybe they are
			// simply of different case?
			#if		qUseWin32CompareStringCallForCaseInsensitiveSearch
				#if		qWideCharacters
					#define	X_COMPARESTRING	::CompareStringW
				#else
					#define	X_COMPARESTRING	::CompareStringA
				#endif
				if (X_COMPARESTRING (LOCALE_USER_DEFAULT, NORM_IGNORECASE, &lookingAtData[i], 1, &pattern[i], 1) == CSTR_EQUAL) {
					charsEqual = true;
				}
				#undef	X_COMPARESTRING
			#else
				if (IsASCIIAlpha (lookingAtData[i]) and IsASCIIAlpha (pattern[i]) and
					(tolower (lookingAtData[i]) == tolower (pattern[i]))
					) {
					charsEqual = true;
				}
			#endif
		}

		if (not charsEqual) {
			searchIdx++;
			goto searchSMORE;
		}
	}
	#if		qNoSupportForNewForLoopScopingRules
	}
	#endif

	// I've never really understood what the heck this means. So maybe I got it wrong.
	// But more likely, there is just no consistently applied definition across systems
	// (MWERKS IDE editor and MSVC editor seem to treat this differently for example).
	// So what I will do is (basicly what MSVC editor docs say) to
	// consider an otherwise good match to be failed if the text matched isn't preceeded
	// by space/puctuation and succeeded by space/puctuation (including text in the string
	// itself).
	// Some Q/A person will tell me if this is wrong :-) - LGP 960502
	//
	// NB: this is REALLY ASCII/English specific. Must find better way of dealing with
	// this for multi-charset stuff...
	if (wholeWords) {
		// find word-start at head of string and word-end at end of string (can be included
		// in matched text, or just outside)
		Led_tChar	c;
		CopyOut (searchIdx, 1, &c);
		bool	boundaryChar	=	(isascii (c) and (isspace (c) or ispunct (c)));
		if (not boundaryChar) {
			// if the selection starts with space, it must contain a whole (possibly zero) number of words
			// only if it DOESNT start with a space character must we look back one, and see that THAT is
			// a space/boundary char
			if (searchIdx > 0) {
				CopyOut (searchIdx-1, 1, &c);
				bool	boundaryChar	=	(isascii (c) and (isspace (c) or ispunct (c)));
				if (not boundaryChar) {
					// If the first char if the match wasn't a space/ending char, and the one
					// preceeding it wasn't, then we've split a word. Try again
					searchIdx++;
					goto searchSMORE;
				}
			}
		}

		// Now similarly check the ending
		if (patternLen > 0) {
			size_t	lastCharMatchedIdx	=	searchIdx + (patternLen-1);
			CopyOut (lastCharMatchedIdx, 1, &c);
			bool	boundaryChar	=	(isascii (c) and (isspace (c) or ispunct (c)));
			if (not boundaryChar) {
				// if the selection ends with space, it must contain a whole (possibly zero) number of words
				// only if it DOESNT end with a space character must we look forward one, and see that THAT is
				// a space/boundary char
				if (lastCharMatchedIdx < GetEnd ()) {
					CopyOut (lastCharMatchedIdx+1, 1, &c);
					bool	boundaryChar	=	(isascii (c) and (isspace (c) or ispunct (c)));
					if (not boundaryChar) {
						// If the first char if the match wasn't a space/ending char, and the one
						// preceeding it wasn't, then we've split a word. Try again
						searchIdx++;
						goto searchSMORE;
					}
				}
			}
		}
	}

	// if I got here, I fell through the loop, and the pattern matches!!!
	return searchIdx;


notFoundByBuffersEnd:
	if (wrapAtEnd) {
		if (wrappedYet) {
			return kBadIndex;
		}
		else {
			wrappedYet = true;
			searchIdx = 0;
			goto searchSMORE;
		}
	}
	// search to end of buffer (or searchTo limit) - well - not found...
	return kBadIndex;
}

#if		qSupportLed30CompatAPI
void	TextStore::DoAboutToUpdateCalls (const UpdateInfo& updateInfo, const vector<Marker*>& markers)
{
	Led_Assert (false);
	DoAboutToUpdateCalls (updateInfo, &*markers.begin (), &*markers.end ());
}
#endif

/*
@METHOD:		TextStore::DoAboutToUpdateCalls
@DESCRIPTION:	<p>Utility to call AboutToUpdateText on registered MarkerOwners, and Markers. Don't call directly.
			Called by concrete TextStore subclasses. Note: we call the AbouToUpdate() calls first on the markers, and
			then on the @'MarkerOwner's. And in @'TextStore::DoDidUpdateCalls' we will call them in the <em>reverse</em>
			order.
			</p>
				<p>Note that if you want to update a region of text - use @'TextStore::SimpleUpdater'.
			</p>
			<p>Note also that this API changed arguments in Led 3.1a4</p>
*/
void	TextStore::DoAboutToUpdateCalls (const UpdateInfo& updateInfo, Marker*const * markersBegin, Marker*const * markersEnd)
{
	/*
	 *	Note that AboutToUpdateText calls ARE allowed to raise exceptions. In which
	 *	case, we abandon the update.
	 */
	{
		vector<MarkerOwner*>::const_iterator	start	=	GetMarkerOwners ().begin ();
		vector<MarkerOwner*>::const_iterator	end		=	GetMarkerOwners ().end ();
		for (vector<MarkerOwner*>::const_iterator i = start; i != end; ++i) {
			(*i)->AboutToUpdateText (updateInfo);
		}
	}
	{
		for (Marker*const* i = markersBegin; i != markersEnd; ++i) {
			(*i)->AboutToUpdateText (updateInfo);
		}
	}
}

#if		qSupportLed30CompatAPI
void	TextStore::DoDidUpdateCalls (const UpdateInfo& updateInfo, const vector<Marker*>& markers)	throw ()
{
	DoDidUpdateCalls (updateInfo, &*markers.begin (), &*markers.end ());
}
#endif

/*
@METHOD:		TextStore::DoDidUpdateCalls
@DESCRIPTION:	<p>Utility to call DidUpdateText on registered MarkerOwners, and Markers. Don't call directly.
			Called by concrete TextStore subclasses.</p>
				<p>NB: See @'TextStore::DoAboutToUpdateCalls'. And note that we call the @'MarkerOwner::DidUpdateText' calls
			in the <em>reverse</em> order in which the @'MarkerOwner::AboutToUpdateText' calls were made.</p>
			<p>Note also that this API changed arguments in Led 3.1a4</p>
*/
void	TextStore::DoDidUpdateCalls (const UpdateInfo& updateInfo, Marker*const * markersBegin, Marker*const * markersEnd)	throw ()
{
	vector<MarkerOwner*>	markerOwners	=	GetMarkerOwners ();
	/*
	 *	Not STRICTLY required that the list of markerowners doesn't change during this call - but would be a possible symptom of
	 *	problems, so check with asserts.
	 */
	{
		vector<MarkerOwner*>::reverse_iterator	start	=	markerOwners.rbegin ();
		vector<MarkerOwner*>::reverse_iterator	end		=	markerOwners.rend ();
		for (vector<MarkerOwner*>::reverse_iterator i = start; i != end; ++i) {
			Led_Assert (GetMarkerOwners () == markerOwners);
			(*i)->EarlyDidUpdateText (updateInfo);
			Led_Assert (GetMarkerOwners () == markerOwners);
		}
	}
#if 1
	{
		for (Marker*const* i = markersEnd; i != markersBegin;) {
			--i;
//			(*(i-1))->DidUpdateText (updateInfo);
			(*i)->DidUpdateText (updateInfo);
			Led_Assert (GetMarkerOwners () == markerOwners);
		}
	}
#else
	{
		vector<Marker*>::const_reverse_iterator	start	=	markers.rbegin ();
		vector<Marker*>::const_reverse_iterator	end		=	markers.rend ();
		for (vector<Marker*>::const_reverse_iterator i = start; i != end; ++i) {
			(*i)->DidUpdateText (updateInfo);
			Led_Assert (GetMarkerOwners () == markerOwners);
		}
	}
#endif
	{
		vector<MarkerOwner*>::reverse_iterator	start	=	markerOwners.rbegin ();
		vector<MarkerOwner*>::reverse_iterator	end		=	markerOwners.rend ();
		for (vector<MarkerOwner*>::reverse_iterator i = start; i != end; ++i) {
			Led_Assert (GetMarkerOwners () == markerOwners);
			(*i)->DidUpdateText (updateInfo);
			Led_Assert (GetMarkerOwners () == markerOwners);
		}
	}
}

/*
@METHOD:		TextStore::PeekAtTextStore
@DESCRIPTION:	<p>Since a TextStore is a MarkerOwner, is must override the PeekAtTextStore method, and return itself.</p>
*/
TextStore*	TextStore::PeekAtTextStore () const
{
	return const_cast<TextStore*>(this);
}

#if		qDebug
/*
@METHOD:		TextStore::Invariant_
@DESCRIPTION:	<p>Check internal consitency of data structures. Don't call this directly. Call TextStore::Invariant instead.
	And only call at quiesent times; not in the midst of some update where data structures might not be fully consistent.</p>
*/
void	TextStore::Invariant_ () const
{
	size_t							len	=	GetLength ();
	Led_SmallStackBuffer<Led_tChar>	buf (len);
	CopyOut (0, len, buf);
	const Led_tChar*	start	=	buf;
	const Led_tChar*	end		=	&start[len];
	const	Led_tChar*	curChar	=	start;
	for (; curChar < end; curChar = Led_NextChar (curChar)) {
		// no need todo anything in here since Led_tChar() checks for 
	}
	Led_Assert (curChar == end);
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
