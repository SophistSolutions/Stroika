/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/StandardStyledTextImager.cpp,v 1.39 2003/04/01 18:18:00 lewis Exp $
 *
 * Changes:
 *	$Log: StandardStyledTextImager.cpp,v $
 *	Revision 1.39  2003/04/01 18:18:00  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment () and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 1.38  2003/02/28 19:07:44  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 1.37  2003/02/01 15:49:18  lewis
 *	last SPR# was really 1279(not 1271)
 *	
 *	Revision 1.36  2003/02/01 15:48:20  lewis
 *	SPR#1271- Added TextImager::GetDefaultSelectionFont () and overrode in StandardStyledTextImager,
 *	and used that in WM_GETFONT Led_Win32 callback
 *	
 *	Revision 1.35  2003/01/11 19:28:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.34  2002/11/27 15:58:51  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with a
 *	'TextLayoutBlock& text' argument
 *	
 *	Revision 1.33  2002/10/07 23:21:49  lewis
 *	SPR#1118 - added AbstractStyleDatabaseRep
 *	
 *	Revision 1.32  2002/09/19 14:14:37  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 1.31  2002/05/06 21:33:50  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.30  2001/11/27 00:29:56  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.29  2001/10/19 20:47:15  lewis
 *	DocComments
 *	
 *	Revision 1.28  2001/10/17 20:42:59  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 1.27  2001/08/29 23:36:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.26  2001/08/28 18:43:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.25  2001/07/19 19:55:08  lewis
 *	SPR#0961- Carbon support
 *	
 *	Revision 1.24  2001/07/19 02:21:47  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 1.23  2001/07/04 14:08:08  lewis
 *	fix some Led23BackCompat code (comaplained about in email by Hordur)
 *	
 *	Revision 1.22  2001/05/01 15:47:55  lewis
 *	minor tweeks to avoid GCC warnings
 *	
 *	Revision 1.21  2000/08/28 20:26:02  lewis
 *	delete StandardStyleMarker::GetPriority () override -
 *	now inherits base class version
 *	
 *	Revision 1.20  2000/05/02 00:31:21  lewis
 *	fix GetContinuous_ to handle TextColor () - and fix OnUpdateUICOmmandFOrColor()
 *	to call AssureCurSelFotnCacheValid - SPR#
 *	
 *	Revision 1.19  2000/04/24 16:47:03  lewis
 *	Very substantial change to FlavorPackage stuff - now NOT mixed into TextInteractor -
 *	but simply owned using Led_RefCntPtr<>. Also - did all the proper subclassing in
 *	WP class etc to make this work (almost - still must fix OLE embeddings) as before. SPR# 0742
 *	
 *	Revision 1.18  2000/04/16 14:39:37  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 1.17  2000/04/14 22:40:26  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 1.16  2000/03/28 03:41:05  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker
 *	class takes extra RunElement argument. Now this new code is called by default
 *	StyledTextImager::DrawText() etc overrides. And now HiddenText code properly
 *	merges at least its own drawing and StandardStyledTextImager::StyleMarker.
 *	All this respecting qOldStyleMarkerBackCompatHelperCode - so old code (not
 *	new hidden text code) should still compile and work roughly as before with
 *	this turned on.
 *	
 *	Revision 1.15  2000/03/18 18:59:46  lewis
 *	More relating to SPR#0718. Lose qNewMarkerCombineCode. Added
 *	qOldStyleMarkerBackCompatHelperCode for backward compat code. Cleanups,
 *	and improved docs, and fixed StyleMarkerSummarySinkForSingleOwner::CombineElements ()
 *	for new API
 *	
 *	Revision 1.14  1999/12/28 18:26:21  lewis
 *	SPR#690- Pass TextImager::GetStaticDefaultFont () to base class MarkerCover<>::CTOR
 *	in StyleDatabase::CTOR
 *	
 *	Revision 1.13  1999/12/19 15:38:24  lewis
 *	SPR#0666 - kill obsolete private function and wrap in qSupportLed23CompatAPI old code
 *	which should be needed cuz its a trivial wrapper - and I can see no need for this functinality anyhow
 *	
 *	Revision 1.12  1999/12/19 15:08:20  lewis
 *	Fix SPR#0665- StandardStyledTextImager::GetContinuousStyleInfo () must check
 *	sub/super scripts.
 *	
 *	Revision 1.11  1999/12/14 18:10:05  lewis
 *	added comments, and preliminary qXWindows support
 *	
 *	Revision 1.10  1999/12/07 20:06:52  lewis
 *	make GetContinuousStyleInfo/ GetContinuousStyleInfo_ const methods. And DOCCOMMENTS.
 *	
 *	Revision 1.9  1999/11/29 20:48:47  lewis
 *	Use Led_GetCurrentGDIPort () instead of peeking at 'qd' global (and
 *	kBlack constnat). SPR#0634 (MACONLY).
 *	
 *	Revision 1.8  1999/11/13 16:32:22  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 1.7  1999/07/14 00:29:02  lewis
 *	Fixup docstrings for SummarizeStyleMarkers etc
 *	
 *	Revision 1.6  1999/05/03 22:05:14  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 1.5  1999/03/25 22:29:19  lewis
 *	react to name change - GetInfo->GetInfoMarkers - GetInfo now returns
 *	vector<pair<MARKERINFOsize_t...>
 *	
 *	Revision 1.4  1999/03/10 15:39:30  lewis
 *	lose assert qSingleByteCharacters- just assume working til I hear bug
 *	reports otherwise (LEC now using withUNICODE without trouble so far)
 *	
 *	Revision 1.3  1999/02/05 01:50:27  lewis
 *	add override of SummarizeStyleMarkers so we can restrict paying
 *	attention to only markers of our current styledatabase (at least by
 *	default - that default can be cahnged in a subclass
 *	
 *	Revision 1.2  1998/10/30 14:40:41  lewis
 *	Use vector<> instead of LEd_Array
 *	
 *	Revision 1.1  1998/04/08  02:05:34  lewis
 *	Initial revision
 *
 *
 *	<<CODE MOVED HERE FROM StyledTextImager.cpp>>
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"StandardStyledTextImager.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif





typedef	StyledTextImager::StyleMarker					StyleMarker;
typedef	StandardStyledTextImager::StyleDatabaseRep		StyleDatabaseRep;
typedef	StandardStyledTextImager::InfoSummaryRecord		InfoSummaryRecord;







/*
 ********************************************************************************
 *************** StandardStyledTextImager::StandardStyleMarker ******************
 ********************************************************************************
 */
typedef	StandardStyledTextImager::StandardStyleMarker	StandardStyleMarker;

void	StandardStyleMarker::DrawSegment (const StyledTextImager* imager, const RunElement& /*runElement*/, Led_Tablet tablet,
											size_t from, size_t to, const TextLayoutBlock& text,
											const Led_Rect& drawInto, const Led_Rect& /*invalidRect*/, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_RequireNotNil (imager);
	imager->DrawSegment_ (tablet, fFontSpecification, from, to, text, drawInto, useBaseLine, pixelsDrawn);
}

void	StandardStyleMarker::MeasureSegmentWidth (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_RequireNotNil (imager);
	imager->MeasureSegmentWidth_ (fFontSpecification, from, to, text, distanceResults);
}

Led_Distance	StandardStyleMarker::MeasureSegmentHeight (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_RequireNotNil (imager);
	return (imager->MeasureSegmentHeight_ (fFontSpecification, from, to));
}

Led_Distance	StandardStyleMarker::MeasureSegmentBaseLine (const StyledTextImager* imager, const RunElement& /*runElement*/, size_t from, size_t to) const
{
	Led_RequireNotNil (imager);
	return (imager->MeasureSegmentBaseLine_ (fFontSpecification, from, to));
}











/*
 ********************************************************************************
 ****************************** StandardStyledTextImager ************************
 ********************************************************************************
 */
StandardStyledTextImager::StandardStyledTextImager ():
	StyledTextImager (),
	fStyleDatabase (NULL),
	fICreatedDatabase (false)
{
}

StandardStyledTextImager::~StandardStyledTextImager ()
{
}

void	StandardStyledTextImager::HookLosingTextStore ()
{
	inherited::HookLosingTextStore ();
	HookLosingTextStore_ ();
}

void	StandardStyledTextImager::HookLosingTextStore_ ()
{
	// Only if we created the styledb should we delete it. If it was set by SetStyleDatabase(), don't unset it here.
	if (fICreatedDatabase) {
		fICreatedDatabase = false;
		if (not fStyleDatabase.IsNull ()) {
			fStyleDatabase = NULL;
			HookStyleDatabaseChanged ();
		}
	}
}

void	StandardStyledTextImager::HookGainedNewTextStore ()
{
	inherited::HookGainedNewTextStore ();
	HookGainedNewTextStore_ ();
}

void	StandardStyledTextImager::HookGainedNewTextStore_ ()
{
	if (fStyleDatabase.IsNull ()) {
		fStyleDatabase = new StyleDatabaseRep (GetTextStore ());
		fICreatedDatabase = true;
		HookStyleDatabaseChanged ();
	}
}

void	StandardStyledTextImager::SetStyleDatabase (const StyleDatabasePtr& styleDatabase)
{
	fStyleDatabase = styleDatabase;
	fICreatedDatabase = false;
	if (fStyleDatabase.IsNull () and PeekAtTextStore () != NULL) {
		fStyleDatabase = new StyleDatabaseRep (GetTextStore ());
		fICreatedDatabase = true;
	}
	HookStyleDatabaseChanged ();
}

/*
@METHOD:		StandardStyledTextImager::HookStyleDatabaseChanged
@DESCRIPTION:	<p>Called whenever the @'StandardStyledTextImager::StyleDatabasePtr' associated with this @'StandardStyledTextImager'
	is changed. This means when a new one is provided, created, or disassociated. It does NOT mean that its called when any of the
	data in the style database changes.</p>
*/
void	StandardStyledTextImager::HookStyleDatabaseChanged ()
{
}

Led_FontMetrics	StandardStyledTextImager::GetFontMetricsAt (size_t charAfterPos) const
{
	Tablet_Acquirer	tablet (this);
	Led_AssertNotNil (static_cast<Led_Tablet> (tablet));

	#if		qMultiByteCharacters
		Assert_CharPosDoesNotSplitCharacter (charAfterPos);
	#endif

	FontCacheInfoUpdater	fontCacheUpdater (this, tablet, GetStyleInfo (charAfterPos));
	return (fontCacheUpdater.GetMetrics ());
}

/*
@METHOD:		StandardStyledTextImager::GetDefaultSelectionFont
@DESCRIPTION:	<p>Override @'TextImager::GetDefaultSelectionFont'.</p>
*/
Led_FontSpecification	StandardStyledTextImager::GetDefaultSelectionFont () const
{
	vector<InfoSummaryRecord>	summaryInfo	=	GetStyleInfo (GetSelectionEnd (), 0);
	Led_Assert (summaryInfo.size () == 1);
	return summaryInfo[0];
}

/*
@METHOD:		StandardStyledTextImager::GetContinuousStyleInfo
@DESCRIPTION:	<p>Create a @'Led_IncrementalFontSpecification' with set as valid all attributes which apply to all of the text from
	<code>'from'</code> for <code>'nTChars'</code>.</p>
		<p>So for example - if all text in that range has the same face, but different font sizes, then the face attribute will be
	valid (and set to that common face) and the font size attribute will be set invalid.</p>
		<p>This is useful for setting menus checked or unchecked in a typical word processor font menu.</p>
*/
Led_IncrementalFontSpecification	StandardStyledTextImager::GetContinuousStyleInfo (size_t from, size_t nTChars) const
{
	vector<InfoSummaryRecord>	summaryInfo	=	GetStyleInfo (from, nTChars);
	return (GetContinuousStyleInfo_ (summaryInfo));
}

Led_IncrementalFontSpecification	StandardStyledTextImager::GetContinuousStyleInfo_ (const vector<InfoSummaryRecord>& summaryInfo) const
{
	Led_IncrementalFontSpecification	fontSpec;


	// There are only a certain number of font attributes which can be shared among these InfoSummaryRecords.
	// Each time we note one which cannot be shared - we decrement this count. That way - when we know there can be
	// no shared values - we stop comparing.
	//
	// countOfValidThings is a hack to see if we can skip out of for-loop early without a lot of expensive, and
	// redundant tests.
	//
	// Note - we COULD have simply checked at the end of each loop count a bunch of 'IsValid' booleans. That would have
	// been simpler. But it would have been more costly (performance).
	int	countOfValidThings	=
			7 +
			#if		qMacOS
				4
			#elif	qWindows
				1
			#elif	qXWindows
				0		//	X-TMP-HACK-LGP991213	-- Not quite a hack - but revisit when we have REAL X-Font support
			#endif
		;

	for (size_t i = 0; i < summaryInfo.size (); i++) {
		if (i == 0) {
			fontSpec = Led_IncrementalFontSpecification (summaryInfo[0]);
		}
		else {
			// check each attribute (if not already different) and see if NOW different...

			InfoSummaryRecord	isr	=	summaryInfo[i];

			// Font ID
			if (fontSpec.GetFontNameSpecifier_Valid () and fontSpec.GetFontNameSpecifier () != isr.GetFontNameSpecifier ()) {
				fontSpec.InvalidateFontNameSpecifier ();
				if (--countOfValidThings == 0) 	{ break; }
			}

			// Style Info
			if (fontSpec.GetStyle_Bold_Valid () and fontSpec.GetStyle_Bold () != isr.GetStyle_Bold ()) {
				fontSpec.InvalidateStyle_Bold ();
				if (--countOfValidThings == 0) 	{ break; }
			}
			if (fontSpec.GetStyle_Italic_Valid () and fontSpec.GetStyle_Italic () != isr.GetStyle_Italic ()) {
				fontSpec.InvalidateStyle_Italic ();
				if (--countOfValidThings == 0) 	{ break; }
			}
			if (fontSpec.GetStyle_Underline_Valid () and fontSpec.GetStyle_Underline () != isr.GetStyle_Underline ()) {
				fontSpec.InvalidateStyle_Underline ();
				if (--countOfValidThings == 0) 	{ break; }
			}
			if (fontSpec.GetStyle_SubOrSuperScript_Valid () and fontSpec.GetStyle_SubOrSuperScript () != isr.GetStyle_SubOrSuperScript ()) {
				fontSpec.InvalidateStyle_SubOrSuperScript ();
				if (--countOfValidThings == 0) 	{ break; }
			}
			#if		qMacOS
				if (fontSpec.GetStyle_Outline_Valid () and fontSpec.GetStyle_Outline () != isr.GetStyle_Outline ()) {
					fontSpec.InvalidateStyle_Outline ();
					if (--countOfValidThings == 0) 	{ break; }
				}
				if (fontSpec.GetStyle_Shadow_Valid () and fontSpec.GetStyle_Shadow () != isr.GetStyle_Shadow ()) {
					fontSpec.InvalidateStyle_Shadow ();
					if (--countOfValidThings == 0) 	{ break; }
				}
				if (fontSpec.GetStyle_Condensed_Valid () and fontSpec.GetStyle_Condensed () != isr.GetStyle_Condensed ()) {
					fontSpec.InvalidateStyle_Condensed ();
					if (--countOfValidThings == 0) 	{ break; }
				}
				if (fontSpec.GetStyle_Extended_Valid () and fontSpec.GetStyle_Extended () != isr.GetStyle_Extended ()) {
					fontSpec.InvalidateStyle_Extended ();
					if (--countOfValidThings == 0) 	{ break; }
				}
			#elif	qWindows
				if (fontSpec.GetStyle_Strikeout_Valid () and fontSpec.GetStyle_Strikeout () != isr.GetStyle_Strikeout ()) {
					fontSpec.InvalidateStyle_Strikeout ();
					if (--countOfValidThings == 0) 	{ break; }
				}
			#endif

			// Font Size
			if (fontSpec.GetPointSize_Valid () and fontSpec.GetPointSize () != isr.GetPointSize ()) {
				fontSpec.InvalidatePointSize ();
				if (--countOfValidThings == 0) 	{ break; }
			}

			// Font Color
			if (fontSpec.GetTextColor_Valid () and fontSpec.GetTextColor () != isr.GetTextColor ()) {
				fontSpec.InvalidateTextColor ();
				if (--countOfValidThings == 0) 	{ break; }
			}

		}
	}

	return (fontSpec);
}

#if		qMacOS
bool	StandardStyledTextImager::DoContinuousStyle_Mac (size_t from, size_t nTChars, short* mode, TextStyle* theStyle)
{
	//	Led_Require ((*mode & doColor) == 0);	// NB: we currently don't support 	doColor,  doAll	, addSize
	// Just silently ingore doColor for now since done from TCL - and we just return NO for that style...
	Led_Require ((*mode & addSize) == 0);
	Led_RequireNotNil (theStyle);

	unsigned int	resultMode = *mode;
	Led_IncrementalFontSpecification	resultSpec	=	GetContinuousStyleInfo (from, nTChars);
	if (resultMode & doFont) {
		resultSpec.GetOSRep (&theStyle->tsFont, NULL, NULL);
	}
	if (resultMode & doFace) {
		resultSpec.GetOSRep (NULL, NULL, &theStyle->tsFace);
	}
	if (resultMode & doSize) {
		resultSpec.GetOSRep (NULL, &theStyle->tsSize, NULL);
	}

	bool	result	= (resultMode != *mode);
	*mode = resultMode;
	return (result);
}

vector<StandardStyledTextImager::InfoSummaryRecord>	StandardStyledTextImager::Convert (const ScrpSTElement* teScrapFmt, size_t nElts)
{
	vector<InfoSummaryRecord>	result;
	for (size_t i = 0; i < nElts; i++) {
		Led_IncrementalFontSpecification	fsp;
		fsp.SetOSRep (teScrapFmt[i].scrpFont, teScrapFmt[i].scrpSize, teScrapFmt[i].scrpFace);
		size_t	length=		(i < (nElts-1))? (teScrapFmt[i+1].scrpStartChar - teScrapFmt[i].scrpStartChar): 9999999;
		InfoSummaryRecord	isr (fsp, length);
		result.push_back (isr);
	}
	return (result);
}

void	StandardStyledTextImager::Convert (const vector<InfoSummaryRecord>& fromLedStyleRuns, ScrpSTElement* teScrapFmt)
{
	size_t	nElts		=	fromLedStyleRuns.size ();
	size_t	startChar	=	0;

	GrafPtr		oldPort	=	Led_GetCurrentGDIPort ();
#if		TARGET_CARBON
	CGrafPtr	tmpPort	=	::CreateNewPort ();
	::SetPort (tmpPort);
#else
	CGrafPort	tmpPort;
	::OpenCPort (&tmpPort);
#endif

	for (size_t i = 0; i < nElts; i++) {
		InfoSummaryRecord	isr	=	fromLedStyleRuns[i];
		
		(void)::memset (&teScrapFmt[i], 0, sizeof (teScrapFmt[i]));
		teScrapFmt[i].scrpStartChar = startChar;

		isr.GetOSRep (&teScrapFmt[i].scrpFont, &teScrapFmt[i].scrpSize, &teScrapFmt[i].scrpFace);

		::TextFont (teScrapFmt[i].scrpFont);
		::TextFace (teScrapFmt[i].scrpFace);
		::TextSize (teScrapFmt[i].scrpSize);

		FontInfo	info;
		::GetFontInfo (&info);

		teScrapFmt[i].scrpHeight = info.ascent + info.descent + info.leading;
		teScrapFmt[i].scrpAscent = info.ascent;

		startChar += isr.fLength;
	}

	#if		TARGET_CARBON
		::DisposePort (tmpPort);
	#else
		::CloseCPort (&tmpPort);
	#endif
	::SetPort (oldPort);
}
#endif

#if		qDebug
void	StandardStyledTextImager::Invariant_ () const
{
	StyledTextImager::Invariant_ ();
	if (not fStyleDatabase.IsNull ()) {
		fStyleDatabase->Invariant ();
	}
}
#endif





/*
 ********************************************************************************
 ************** StandardStyledTextImager::AbstractStyleDatabaseRep **************
 ********************************************************************************
 */
#if		qDebug
void	StandardStyledTextImager::AbstractStyleDatabaseRep::Invariant_ () const
{
}
#endif




/*
 ********************************************************************************
 **************** StandardStyledTextImager::StyleDatabaseRep ********************
 ********************************************************************************
 */
StyleDatabaseRep::StyleDatabaseRep (TextStore& textStore):
	inheritedMC (textStore, TextImager::GetStaticDefaultFont ())
{
}

vector<StandardStyledTextImager::InfoSummaryRecord>	StyleDatabaseRep::GetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing) const
{
	MarkerVector	standardStyleMarkers	=	GetInfoMarkers (charAfterPos, nTCharsFollowing);

	vector<InfoSummaryRecord>	result;
	size_t						tCharsSoFar				=	0;
	size_t						nStandardStyleMarkers	=	standardStyleMarkers.size ();
	for (size_t i = 0; i < nStandardStyleMarkers; i++) {
		StandardStyleMarker*	marker	=	standardStyleMarkers[i];
		Led_AssertNotNil (marker);
		size_t	markerStart;
		size_t	markerEnd;
		marker->GetRange (&markerStart, &markerEnd);

		// for i==START and END, we may have to include only partial lengths of the
		// markers - for the INTERNAL markers, use their whole length
		size_t	length	=	markerEnd-markerStart;
		if (i == 0) {
			Led_Assert (charAfterPos >= markerStart);
			Led_Assert (charAfterPos - markerStart < length);
			length -= (charAfterPos - markerStart);
		}
		if (i == nStandardStyleMarkers-1) {
			Led_Assert (length >= nTCharsFollowing - tCharsSoFar);	// must be preserving, or shortening...
			length = nTCharsFollowing - tCharsSoFar;
		}
		Led_Assert (length > 0 or nTCharsFollowing == 0);
		Led_Assert (length <= nTCharsFollowing);
		result.push_back (InfoSummaryRecord (marker->fFontSpecification, length));
		tCharsSoFar += length;
	}
	Led_Assert (tCharsSoFar == nTCharsFollowing);
	return result;
}

void	StyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const Led_IncrementalFontSpecification& styleInfo)
{
	SetInfo (charAfterPos, nTCharsFollowing, styleInfo);
}
#if 0
void	StyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, const vector<InfoSummaryRecord>& styleInfos)
{
	SetStyleInfo (charAfterPos, nTCharsFollowing, styleInfos.size (), &styleInfos.front ());
}
#endif
void	StyleDatabaseRep::SetStyleInfo (size_t charAfterPos, size_t nTCharsFollowing, size_t nStyleInfos, const InfoSummaryRecord* styleInfos)
{
	size_t	setAt			=	charAfterPos;
	size_t	lengthUsedSoFar	=	0;
	for (size_t i = 0; i < nStyleInfos and lengthUsedSoFar < nTCharsFollowing; i++) {
		InfoSummaryRecord	isr	=	styleInfos[i];
		size_t	length	=	isr.fLength;
		Led_Assert (nTCharsFollowing >= lengthUsedSoFar);
		length = Led_Min (nTCharsFollowing-lengthUsedSoFar, length);
		SetStyleInfo (setAt, length, Led_IncrementalFontSpecification (isr));
		setAt += length;
		lengthUsedSoFar += length;
	}
}

#if		qDebug
void	StyleDatabaseRep::Invariant_ () const
{
	inheritedMC::Invariant_ ();
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

