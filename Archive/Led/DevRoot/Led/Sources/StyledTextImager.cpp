/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/StyledTextImager.cpp,v 2.77 2003/05/07 18:28:55 lewis Exp $
 *
 * Changes:
 *	$Log: StyledTextImager.cpp,v $
 *	Revision 2.77  2003/05/07 18:28:55  lewis
 *	SPR#1408: add overload of StyleMarkerSummarySinkForSingleOwner::CTOR that takes TextLayoutBlock
 *	
 *	Revision 2.76  2003/04/01 18:18:01  lewis
 *	SPR#1322: Added 'invalidRect' argument to StyledTextImager::StyleMarker::DrawSegment () and subclasses to be able to do logical clipping in tables
 *	
 *	Revision 2.75  2003/02/28 19:12:03  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.74  2003/02/03 17:44:10  lewis
 *	SPR#1293 - not fixed - but just commented on a bit (bidi/measuresegmentwidth)
 *	
 *	Revision 2.73  2003/02/03 17:11:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.72  2003/02/03 17:10:09  lewis
 *	SPR#1292 - slight fixup to StyledTextImager::DrawSegment so it passes along LOGICAL
 *	(not visual) coords to DrawSegment methods it calls
 *	
 *	Revision 2.71  2003/01/29 17:56:38  lewis
 *	Add qAssertWarningForEqualPriorityMarkers
 *	
 *	Revision 2.70  2002/12/17 17:36:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.69  2002/12/17 17:34:49  lewis
 *	SPR#1210- StyledTextImager::DrawSegment/WordProcessor::DrawRowSegments be a bit more careful about
 *	drawInto rect (not 100% fixed but close enuf for now)
 *	
 *	Revision 2.68  2002/12/16 17:29:15  lewis
 *	For SPR#1206 - display of style info wrong for RTL text -
 *	StyledTextImager::StyleMarkerSummarySink::ProduceOutputSummary
 *	now pays attention to the TextLayoutBlock object and returns runs
 *	in virtual order.
 *	
 *	Revision 2.67  2002/12/01 17:52:45  lewis
 *	SPR#1183- summarizestylemark fixes for BIDI
 *	
 *	Revision 2.66  2002/11/28 02:31:17  lewis
 *	A couple simple name changes (GetVirtualText->PeekAtVirtualText and TextLayoutBlock_Subset
 *	to TextLayoutBlock_VirtualSubset
 *	
 *	Revision 2.65  2002/11/27 15:58:55  lewis
 *	SPR#1183 - as part of BIDI effort - DrawSegment now replaces 'Led_tChar* text' argument with
 *	a 'TextLayoutBlock& text' argument
 *	
 *	Revision 2.64  2002/09/22 15:44:49  lewis
 *	SPR#1108 - in StyledTextImager::DrawSegment - be more careful to now allow the tmpDrawInto
 *	rect to become negatively sized
 *	
 *	Revision 2.63  2002/05/06 21:33:56  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.62  2001/11/27 00:30:01  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.61  2001/10/17 20:43:02  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.60  2001/09/26 15:41:22  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.59  2001/09/18 17:44:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.58  2001/08/29 23:36:36  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.57  2001/08/28 18:43:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.56  2001/04/26 16:39:31  lewis
 *	delete ifdefed out code fragments (cleanups)
 *	
 *	Revision 2.55  2001/01/03 14:36:27  Lewis
 *	comment out unused code
 *	
 *	Revision 2.54  2000/08/28 20:31:56  lewis
 *	StyledTextImager::StyleMarker::GetPriority () no nonger pure virtual -
 *	instead has default value. Other cleanups/fixups
 *	
 *	Revision 2.53  2000/07/26 05:46:36  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner
 *	arg to CollectAllMarkersInRagne() APIs. And a few related changes. See the
 *	SPRs for a table of speedups.
 *	
 *	Revision 2.52  2000/06/12 20:08:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.51  2000/04/16 14:39:39  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.50  2000/04/14 22:40:27  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.49  2000/03/29 23:35:33  lewis
 *	SPR#0720 - more cleanups of SimpleStyleMarkerByFontSpec code
 *	
 *	Revision 2.48  2000/03/29 22:27:26  lewis
 *	Added SimpleStyleMarkerByFontSpec/TrivialFontSpecStyleMarker utility classes to
 *	StyledTextImager module, and used that to re-implement the SyntaxColoring style
 *	marker implementations (soon more to simplify). SPR#0720
 *	
 *	Revision 2.47  2000/03/28 03:41:05  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class takes
 *	extra RunElement argument. Now this new code is called by default StyledTextImager::DrawText()
 *	etc overrides. And now HiddenText code properly merges at least its own drawing and
 *	StandardStyledTextImager::StyleMarker. All this respecting qOldStyleMarkerBackCompatHelperCode -
 *	so old code (not new hidden text code) should still compile and work roughly as before
 *	with this turned on.
 *	
 *	Revision 2.46  2000/03/18 18:59:46  lewis
 *	More relating to SPR#0718. Lose qNewMarkerCombineCode. Added qOldStyleMarkerBackCompatHelperCode
 *	for backward compat code. Cleanups, and improved docs, and fixed
 *	StyleMarkerSummarySinkForSingleOwner::CombineElements () for new API
 *	
 *	Revision 2.45  2000/03/18 03:48:49  lewis
 *	SPR#0718 - tmporary new qNewMarkerCombineCode code. Basicly trying to redesign how
 *	we summarize style markers so we keep track of lower priority markers. Idea is that
 *	eventually the high-pri ones will be able to USE the lowPri ones to produce a COMPOSITE
 *	draw routine. Just got first step working - new RunElement xtra data and new Summarize
 *	code working. Still must cleanup, document, deal with backward compat, etc. Then pass
 *	along the new info so it can be used. DID perf-testing on this part so far - and for
 *	the RTF 1.5Doc - no discrernable speed change
 *	
 *	Revision 2.44  1999/12/25 04:14:48  lewis
 *	DOCCOMMENTS
 *	
 *	Revision 2.43  1999/11/13 16:32:24  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.42  1999/07/14 00:29:03  lewis
 *	Fixup docstrings for SummarizeStyleMarkers etc
 *	
 *	Revision 2.41  1999/07/13 17:30:28  lewis
 *	cosmetic cleanup
 *	
 *	Revision 2.40  1999/05/03 22:05:18  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.39  1999/05/03 21:41:34  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.38  1999/02/05 01:48:54  lewis
 *	Changed StyleMarkerSummarySink::Append to call new API method CombineElements () so
 *	we could better impelment the overall policy (in subclass) of making combinging mechanism
 *	more flexible (like ignoring other marker owners as in sucblass StyleMarkerSummarySinkForSingleOwner
 *	where we implemnt that policy). And LOTS of DOCCOMMENTS
 *	
 *	Revision 2.37  1998/10/30 14:36:11  lewis
 *	Use STL vector template instead of Led_Array.
 *	
 *	Revision 2.36  1998/07/24  01:13:33  lewis
 *	StyleMarkerSummarySinkForSingleOwner:
 *
 *	Revision 2.35  1998/04/25  01:34:41  lewis
 *	Had assert this code didn't work with UNICDE/Mutlibyte. But I see no such problems.
 *	Disabled comment. Will fix any problems
 *	that come up, as I'm using it that way now..
 *
 *	Revision 2.34  1998/04/08  02:08:04  lewis
 *	Moved StandardStyledTextImager code to separate module.
 *
 *	Revision 2.33  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.32  1997/10/01  02:03:07  lewis
 *	Added SetStyleInfo version taking count and InfoSummaryRecord* (instead of Led_Array).
 *
 *	Revision 2.31  1997/09/29  15:23:01  lewis
 *	Lose qLedFirstIndex support.
 *	Re-implement StandardStyledTextImager::SharedStyleDatabase using MarkerCover<>
 *	and Led_RefCntPtr<> templates. (use those templates in WordPRocessor code was well
 *	for paragraph styles).
 *	NB: this implied new calling style/behaivor for assigning style databases (simpler/better now).
 *
 *	Revision 2.30  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.29  1997/07/23  23:10:13  lewis
 *	fix spr#0466
 *
 *	Revision 2.28  1997/07/14  00:42:19  lewis
 *	Renamed TextStore_ to TextStore and TextImager_ to TextImager.
 *
 *	Revision 2.27  1997/06/28  17:16:23  lewis
 *	Cleanups.
 *
 *	Revision 2.26  1997/06/24  03:32:55  lewis
 *	Lose old test code
 *
 *	Revision 2.25  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.24  1997/06/18  03:09:02  lewis
 *	Use new CollectAllMarkers ()... stuff with MarkerSink support.
 *	Redo SummarizeStyleMarkers () using new MarkerSink stuff.
 *	Cleanups using typedefs to avoid nested class uglies.
 *	qIncludePrefixFile.
 *
 *	Revision 2.23  1997/03/23  00:47:09  lewis
 *	Fix bug (remo reported) with horizontal scrolling and logical clipping for styled text.
 *	MSVC50 compiler bug workaroudns, and lose old ones.
 *
 *	Revision 2.22  1997/03/04  20:10:43  lewis
 *	GetFontMetricsAt ()/GetStyleInfo fixes
 *
 *	Revision 2.21  1997/01/10  03:15:41  lewis
 *	throw specifiers.
 *
 *	Revision 2.20  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.19  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.17  1996/07/19  16:41:28  lewis
 *	Code cleanups (deleted ifdefed old code).
 *	Added CollectAllStyleMarkersInRange () to StyleDBASE.
 *	DELETE ALL owned markers in STYLEDBASE DTOR.
 *
 *	Revision 2.16  1996/06/01  02:22:09  lewis
 *	Lose no-RTTI-compat code.
 *	Cleanup styledtextimager::DrawSegment code - not sure what else...
 *
 *	Revision 2.15  1996/05/23  20:08:29  lewis
 *	LOTS of changes here. Including - and mostly related to change from
 *	FontSpecication to Led_XX and Led_IncrementalXX formats.
 *	A bunch of optimizations to SetStyleInfo() code.
 *	Rewrote (and renamed) DoContinuousStyle --> GetContinuousStyleInfo()
 *	Use new sorting code to speed up a number of operations.
 *	Fix logical clipping code in StyledTextImager::DrawSegment() - this was a big deal.
 *	probably more...
 *
 *	Revision 2.14  1996/05/14  20:33:49  lewis
 *	Optimize SetStyleInfo (changedAnything) - and fixed end condition
 *	bug in that routine.
 *	DrawSegment () args changed.
 *
 *	Revision 2.13  1996/05/04  21:12:26  lewis
 *	Add assertions to detect if measureSegment() calls don't match the width
 *	returned by the DrawSegmetn() calls.
 *	Fixed bug in SetStyleInfo() where we didn't coalese last segment at end, an
 *	alwasys in haldning end coalese (not at end of buffer) we were doig double
 *	delete (explicitly called AccumulateMarkerForDeletion () when we shoulndt have).
 *
 *	Revision 2.12  1996/04/18  15:34:03  lewis
 *	Lose obsolete override of CalcSegmentSize().
 *	Added needed override of StyledTextImager::GetFontMetricsAt ()
 *	but NYI (assert out).
 *	Fixed bug(s) in coalescing adjacent standard style markers?
 *
 *	Revision 2.11  1996/03/16  18:50:12  lewis
 *	Largely rewritten tabstop support.
 *	Rewrote SetStyleInfo() code to coalesce (big change).
 *	And updated invariants to check coalscing worked - no identical adjacent
 *	markers.
 *
 *	Revision 2.10  1996/03/04  07:52:37  lewis
 *	In createing STYL data from internal style run info, fill in the
 *	scrpHeihgt/scrpAscent fields. We were leaving them zero - which makes
 *	sense to me - but seemed to cause confusion. I guess I sort of hoped since
 *	they were obviously redundant, leavin them zero would cause other programs
 *	to compute them. But no. Other programs just got confused.
 *
 *	Revision 2.9  1996/02/26  22:15:36  lewis
 *	Use Led_Min/etc instead of Min()
 *	CollectAllStyleMarkersInRange -->CollectAllStandardStyleMarkersInRange()
 *
 *	Revision 2.8  1996/02/05  04:21:56  lewis
 *	Redid DoContinuousStyle - mostly to make portable.
 *	Got rid of SpecifyTextStore and instead use new
 *	HookLosting/GainingTextStore instead.
 *
 *	Revision 2.7  1996/01/04  00:00:54  lewis
 *	Mostly just moved AccumulateMarkersForDeleteion stuff into
 *	StyleDatabase, instead of StyledTextImager, so cases like MFC LedIt! where
 *	StyleDBase and TextStore owned by Doc, and we sometimes delete
 *	all text (DeleteConents call) when no view present, and wanted
 *	markers to be updated properly when that happens.
 *
 *	Revision 2.6  1995/12/15  02:19:52  lewis
 *	Added logical clipping support to DrawSegment() support.
 *
 *	Revision 2.5  1995/12/09  05:42:55  lewis
 *	StandardStyledTextImager::SharedStyleDatabase::PeekAtTextStore () method
 *	is virtual, so no longer inline.
 *
 *	Revision 2.4  1995/12/06  01:36:34  lewis
 *	LessThan is now a member function.
 *
 *	Revision 2.3  1995/11/25  00:28:06  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1995/11/04  23:16:11  lewis
 *	New useBaseLine arg in DrawSegment().
 *	New MeasureSegmentBaseLine routine.
 *	MeasureSegmentHeight () no longer takes text arg.
 *
 *	Revision 2.1  1995/11/02  22:34:11  lewis
 *	Again lots of changes.
 *	Fixed SummarizeFontStyleMarkers() to at least coalesce adjacent buckets
 *	This makes a HUGE performance improvemnet. But its still not GREAT.
 *	Good enuf for beta though I think.
 *	Added Covert() routines to handle conerting my style runs to Apples
 *	native formats.
 *	Added SharedStyleDatabase - a big change - so multiple StandardStyledTextImagers
 *	can share dthe same font/style runs when viewing the same textstore (ie
 *	as in OpenDoc).
 *	Added StandardStyledTextImager::DoContinuousStyle () for
 *	updating font / style menus.
 *
 *	Revision 2.0  1995/10/19  23:45:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/10/19  22:32:14  lewis
 *	Massive changes. Now largely functional.
 *	Main thrust was to break up functionality into two class StyledTextImager
 *	and StandardStyledTextImager. The former handles translating Led Styleruns
 *	and Style markers into summarized calls to the right markers.
 *	And then StandardStyledTextImager builds a simple standard style-run
 *	facilty on top of StledTextImager.
 *
 *	Revision 1.1  1995/10/09  22:40:17  lewis
 *	Initial revision
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"StyledTextImager.h"





#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif



typedef	StyledTextImager::StyleMarker				StyleMarker;
typedef	StyledTextImager::RunElement				RunElement;
typedef	StyledTextImager::StyleMarkerSummarySink	StyleMarkerSummarySink;






/*
 ********************************************************************************
 ********************************** StyleMarker *********************************
 ********************************************************************************
 */
/*
@METHOD:		StyledTextImager::StyleMarker::GetPriority
@DESCRIPTION:	<p>Since we can have style markers overlap, we need someway to deal with conflicting
	style information. Since some style elements can be arbitrary drawing code, like
	an OpenDoc part, or a picture, we cannot genericly write code to merge styles.
	So we invoke a somewhat hackish priority scheme, where the marker with the highest
	priority is what is asked todo the drawing.</p>
		<p>The priority of StandardStyleMarker is eBaselinePriority - ZERO - as a reference. So you can specify
	easily either markers that take precedence over, or are always superseded by the
	standard style markers. And this returns eBaselinePriority - ZERO - by default.</p>
 */
int	StyleMarker::GetPriority () const
{
	return eBaselinePriority;
}







/*
 ********************************************************************************
 **************************** StyleMarkerSummarySink ****************************
 ********************************************************************************
 */
StyleMarkerSummarySink::StyleMarkerSummarySink (size_t from, size_t to):
	inherited (),
	fBuckets (),
	fText (NULL),
	fFrom (from),
	fTo (to)
{
	// See SPR#1293 - may want to get rid of this eventually
	Led_Require (from <= to);
	if (from != to) {
		fBuckets.push_back (RunElement (NULL, to-from));
	}
}

StyleMarkerSummarySink::StyleMarkerSummarySink (size_t from, size_t to, const TextLayoutBlock& text):
	inherited (),
	fBuckets (),
	fText (&text),
	fFrom (from),
	fTo (to)
{
	Led_Require (from <= to);
	if (from != to) {
		fBuckets.push_back (RunElement (NULL, to-from));
	}
	typedef	TextLayoutBlock::ScriptRunElt	ScriptRunElt;
	vector<ScriptRunElt>	scriptRuns	=	text.GetScriptRuns ();
	for (vector<ScriptRunElt>::const_iterator i = scriptRuns.begin (); i != scriptRuns.end (); ++i) {
		Led_Assert ((*i).fRealEnd <= (to-from));
		SplitIfNeededAt (from + (*i).fRealEnd);
	}
}

void	StyleMarkerSummarySink::Append (Marker* m)
{
	Led_RequireNotNil (m);
	StyleMarker*	styleMarker	=	dynamic_cast<StyleMarker*>(m);
	if (styleMarker != NULL) {
		size_t	start	=	Led_Max (styleMarker->GetStart (), fFrom);
		size_t	end		=	Led_Min (styleMarker->GetEnd (), fTo);

		/*
		 *	Assure the marker were adding will fit neatly.
		 */
		SplitIfNeededAt (start);
		SplitIfNeededAt (end);
	
		/*
		 *	Now walk the buckets and fit the new marker into each bucket as appropriate.
		 */
		size_t	upTo	=	fFrom;
		for (vector<RunElement>::iterator i = fBuckets.begin (); i != fBuckets.end (); ++i) {
			if (start <= upTo and upTo + (*i).fLength <= end) {
				CombineElements (&*i, styleMarker);
			}
			upTo += (*i).fLength;
		}
	}
}

/*
@METHOD:		StyledTextImager::StyleMarkerSummarySink::SplitIfNeededAt
@DESCRIPTION:	<p>Private routine to split the current list of buckets at a particular position. Called with
			the endpoints of a new marker.</p>
				<p>Note that this routine keeps the buckets in order sorted by their REAL OFFSET (not visual display)
			index. We don't even keep track of the index explicitly in the buckets: we compute it based on the fLength
			field in the buckets and their offset from the summary sink start ('this->fFrom').</p>
*/
void	StyleMarkerSummarySink::SplitIfNeededAt (size_t markerPos)
{
	Led_Require (markerPos >= fFrom);
	Led_Require (markerPos <= fTo);
	size_t	upTo = fFrom;
	for (vector<RunElement>::iterator i = fBuckets.begin (); i != fBuckets.end (); ++i) {
		size_t	eltStart	=	upTo;
		size_t	eltEnd		=	upTo + (*i).fLength;
		if (markerPos >= eltStart and markerPos <= eltEnd and markerPos != eltStart and markerPos != eltEnd) {
			#if		qDebug
				size_t	oldLength	=	(*i).fLength;
			#endif
			// then we need a split at that position.
			RunElement	newElt	=	*i;
			(*i).fLength = markerPos-eltStart;
			newElt.fLength = eltEnd - markerPos;
			Led_Assert (oldLength == (*i).fLength + newElt.fLength);
			Led_Assert ((*i).fLength != 0);
			Led_Assert (newElt.fLength != 0);
			fBuckets.insert (i+1, newElt);
			break;
		}
		upTo += (*i).fLength;
	}
}

/*
@METHOD:		StyledTextImager::StyleMarkerSummarySink::CombineElements
@DESCRIPTION:	<p>When two style markers overlap, which one gets todo the drawing? As part of the summarizing
	process (@'StyledTextImager::SummarizeStyleMarkers'), we must chose how to combine markers.</p>
		<p>This default algorithm simply chooses the one with the higher priority.</p>
		<p>Subclassers can override this behavior, and, for instance, restrict paying attention to only a particular
	subtype of '@StyleMarker's, or maybe to set particular values into one (chosen subtype marker to connote the overlap, and
	allow this to effect the draw. Or, perhaps, a subclass might ingnore markers with a particular owner value.</p>
		<p>Note that markers NOT used can be placed in the @'StyledTextImager::RunElement's 'fSupercededMarkers' array,
	so that the eventual marker which does the drawing <em>can</em> delegate or combine the drawing behaviors of
	different kinds of markers.</p>
		<p>Note also that this routine will somewhat randomly deal with ties. The first element of a given
	priority wins. But - because of how this is typcially called - as the result of a collection of markers
	from a TextStore - that results in random choices. That can cause trouble - so try to avoid ties
	without GOOD motivation.</p>
*/
void	StyleMarkerSummarySink::CombineElements (StyledTextImager::RunElement* runElement, StyleMarker* newStyleMarker)
{
	Led_RequireNotNil (runElement);
	Led_RequireNotNil (newStyleMarker);

	if (runElement->fMarker == NULL) {
		runElement->fMarker = newStyleMarker;
	}
	else {
		bool	newEltStronger	=	runElement->fMarker->GetPriority () < newStyleMarker->GetPriority ();
		#if		qAssertWarningForEqualPriorityMarkers
			Led_Assert (runElement->fMarker->GetPriority () != newStyleMarker->GetPriority ());
		#endif
		if (newEltStronger) {
			runElement->fSupercededMarkers.push_back (runElement->fMarker);
			runElement->fMarker = newStyleMarker;
		}
		else {
			runElement->fSupercededMarkers.push_back (newStyleMarker);
		}
	}
}

/*
@METHOD:		StyledTextImager::StyleMarkerSummarySink::ProduceOutputSummary
@DESCRIPTION:	<p>Create a vector of @'StyledTextImager::RunElement's. Each of these contains a list of
			marker objects for the region and a length field. The elements are returned in VIRTUAL (LTR display)
			order - NOT logical (internal memory buffer) order. The elements are gauranteed not to cross
			any directional boundaries (as returned from the @'TextLayoutBlock::GetScriptRuns' API)</p>
*/
vector<StyledTextImager::RunElement>	StyledTextImager::StyleMarkerSummarySink::ProduceOutputSummary () const
{
	typedef	TextLayoutBlock::ScriptRunElt	ScriptRunElt;
// Soon fix to use fText as a REFERENCE. Then we probably should have this code assure its re-ordering is done only once and then cached,
// LGP 2002-12-16
	if (fText != NULL) {
		vector<RunElement>		runElements;
		vector<ScriptRunElt>	scriptRuns	=	fText->GetScriptRuns ();
		if (scriptRuns.size () > 1) {
			// sort by virtual start
			sort (scriptRuns.begin (), scriptRuns.end (), TextLayoutBlock::LessThanVirtualStart ());
		}
		for (vector<ScriptRunElt>::const_iterator i = scriptRuns.begin (); i != scriptRuns.end (); ++i) {
			// Grab all StyledTextImager::RunElement elements from this run and copy them out
			const ScriptRunElt&	se					=	*i;
			size_t				styleRunStart		=	0;
			size_t				runEltsBucketStart	=	runElements.size ();
			for (vector<RunElement>::const_iterator j = fBuckets.begin (); j != fBuckets.end (); ++j) {
				size_t	styleRunEnd	=	styleRunStart + (*j).fLength;
				if (se.fRealStart <= styleRunStart and styleRunEnd <= se.fRealEnd) {
					if (se.fDirection == eLeftToRight) {
						runElements.push_back (*j);
					}
					else {
						runElements.insert (runElements.begin () + runEltsBucketStart, *j);
					}
				}
				styleRunStart = styleRunEnd;
			}
		}

		Led_Ensure (runElements.size () == fBuckets.size ());
		return runElements;
	}
	return fBuckets;
}







/*
 ********************************************************************************
 ******************** StyleMarkerSummarySinkForSingleOwner **********************
 ********************************************************************************
 */
typedef	StyledTextImager::StyleMarkerSummarySinkForSingleOwner	StyleMarkerSummarySinkForSingleOwner;

StyleMarkerSummarySinkForSingleOwner::StyleMarkerSummarySinkForSingleOwner (const MarkerOwner& owner, size_t from, size_t to):
	inherited (from, to),
	fOwner (owner)
{
}

StyleMarkerSummarySinkForSingleOwner::StyleMarkerSummarySinkForSingleOwner (const MarkerOwner& owner, size_t from, size_t to, const TextLayoutBlock& text):
	inherited (from, to, text),
	fOwner (owner)
{
}

/*
@METHOD:		StyledTextImager::StyleMarkerSummarySinkForSingleOwner::CombineElements
@DESCRIPTION:	<p>Like @'StyledTextImager::StyleMarkerSummarySink::CombineElements', except that matching
	the MarkerOwner is more important than the Marker Priority.</p>
*/
void	StyleMarkerSummarySinkForSingleOwner::CombineElements (StyledTextImager::RunElement* runElement, StyleMarker* newStyleMarker)
{
	Led_RequireNotNil (runElement);
	Led_RequireNotNil (newStyleMarker);

	if (runElement->fMarker == NULL) {
		runElement->fMarker = newStyleMarker;
	}
	else {
		bool	newEltStronger	=	runElement->fMarker->GetPriority () < newStyleMarker->GetPriority ();
		bool	newMatchesOwner	=	newStyleMarker->GetOwner () == &fOwner;
		bool	oldMatchesOwner	=	runElement->fMarker->GetOwner () == &fOwner;
		if (newMatchesOwner != oldMatchesOwner) {
			newEltStronger = newMatchesOwner;
		}
		if (newEltStronger) {
			runElement->fSupercededMarkers.push_back (runElement->fMarker);
			runElement->fMarker = newStyleMarker;
		}
		else {
			runElement->fSupercededMarkers.push_back (newStyleMarker);
		}
	}
}






/*
 ********************************************************************************
 ****************************** StyledTextImager ********************************
 ********************************************************************************
 */
StyledTextImager::StyledTextImager ():
	TextImager ()
{
}

/*
@METHOD:		StyledTextImager::SummarizeStyleMarkers
@DESCRIPTION:	<p>Create a summary of the style markers applied to a given range of text (by default using
	@'StyledTextImager::StyleMarkerSummarySink') into @'StyledTextImager::RunElement's.</p>
*/
vector<RunElement>	StyledTextImager::SummarizeStyleMarkers (size_t from, size_t to) const
{
	// See SPR#1293 - may want to get rid of this eventually
	StyleMarkerSummarySink summary (from, to);
	GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, summary);
	return summary.ProduceOutputSummary ();
}

/*
@METHOD:		StyledTextImager::SummarizeStyleMarkers
@DESCRIPTION:	<p>Create a summary of the style markers applied to a given range of text (by default using
	@'StyledTextImager::StyleMarkerSummarySink') into @'StyledTextImager::RunElement's.</p>
*/
vector<RunElement>	StyledTextImager::SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const
{
	StyleMarkerSummarySink summary (from, to, text);
	GetTextStore ().CollectAllMarkersInRangeInto (from, to, TextStore::kAnyMarkerOwner, summary);
	return summary.ProduceOutputSummary ();
}

/*
@METHOD:		StyledTextImager::DrawSegment
@DESCRIPTION:	<p>Override @'StyledTextImager::DrawSegment' to break the given segment into subsets based on
			what @'StyledTextImager::StyleMarker' are present in the text. This breakup is done by
			@'StyledTextImager::SummarizeStyleMarkers'.</p>
*/
void	StyledTextImager::DrawSegment (Led_Tablet tablet,
										size_t from, size_t to, const TextLayoutBlock& text,
										const Led_Rect& drawInto, const Led_Rect& invalidRect,
										Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
									)
{
	/*
	 *	Note that SummarizeStyleMarkers assures 'outputSummary' comes out in VIRTUAL order.
	 *	Must display text in LTR virtual display order.
	 */
	vector<RunElement>	outputSummary	=	SummarizeStyleMarkers (from, to, text);

	Led_Rect	tmpDrawInto				=	drawInto;
	size_t		outputSummaryLength		=	outputSummary.size ();
	size_t		indexIntoText_VISUAL	=	0;
	if (pixelsDrawn != NULL) {
		*pixelsDrawn = 0;
	}

	for (size_t i = 0; i < outputSummaryLength; i++) {
		const RunElement&	re			=	outputSummary[i];
		size_t				reLength	=	re.fLength;
		size_t				reFrom		=	text.MapVirtualOffsetToReal (indexIntoText_VISUAL) + from;	// IN LOGICAL OFFSETS!!!
		size_t				reTo		=	reFrom + reLength;											// ""
		Led_Assert (indexIntoText_VISUAL <= to-from);
		Led_Assert (reLength > 0);
		/*
		 *	Do logical clipping across segments.
		 *
		 *		We could be more aggressive, and do logical clipping WITHIN segments, but that would have
		 *	little potential benefit. The most important cases are embeddings which are one-char
		 *	long and all-or-nothing draws. Also, todo so would impose greater complexity in
		 *	dealing with the following:
		 *
		 *		We cannot even totally do logical clipping at the segment boundaries. This is because
		 *	we allow a character to draw into an adjacent character cell (but we only allow it to
		 *	overwrite the ONE ADJACENT CELL). This happens, for example, with italics, and ligatures,
		 *	etc.
		 *
		 *		So in our logical clipping, we must not clip out segments which are only outside the
		 *	logical clipping rect by a single character.
		 *
		 *
		 *	NB:	Doing this RIGHT will be easy if we have access to the whole measured-text. But for now we don't.
		 *	So simply (as a temp hack) use some fixed number of pixels to optimize by.
		 *
		 */
		const	Led_Coordinate	kSluffToLeaveRoomForOverhangs	=	20;	// cannot imagine more pixel overhang than this,
																		// and its a tmp hack anyhow - LGP 960516
		if (tmpDrawInto.left - GetHScrollPos () < invalidRect.right+kSluffToLeaveRoomForOverhangs) {
			Led_Distance		pixelsDrawnHere	=	0;

			/*
			 *	This is a BIT of a kludge. No time to throughly clean this up right now. This is vaguely
			 *	related to SPR#1210 and SPR#1108.
			 *
			 *	All our code REALLY pays attention to (for the most part) is the LHS of the rectangle. But occasionally
			 *	we pay attention to the RHS. We have funky sloppy semantics with respect to the GetHScrollPos and this rectangle.
			 *	The rectange - I believe - is supposed to be in WINDOW coordinates. But - its implicitly offset
			 *	for drawing purposes by the GetHScrollPos (SOMEWHAT - not totally because of tabstops).
			 *
			 *	Anyhow - some code wants it one way - and other another way. CLEAN THIS UP in the future - probably when
			 *	I support scaling - probably be RE-STRUCTRURING my coordinate systems (all floats? and inches or TWIPS?).
			 *
			 */
			tmpDrawInto.right = drawInto.right + GetHScrollPos ();
			if (tmpDrawInto.left < tmpDrawInto.right) {
				if (re.fMarker == NULL) {
					DrawSegment_ (tablet, GetDefaultFont (), reFrom, reTo, TextLayoutBlock_VirtualSubset (text, indexIntoText_VISUAL, indexIntoText_VISUAL + reLength), 
									tmpDrawInto, useBaseLine, &pixelsDrawnHere
								);
				}
				else {
					re.fMarker->DrawSegment (this, re, tablet, reFrom, reTo, TextLayoutBlock_VirtualSubset (text, indexIntoText_VISUAL, indexIntoText_VISUAL + reLength),
												tmpDrawInto, invalidRect, useBaseLine, &pixelsDrawnHere
										);
				}
			}
			if (pixelsDrawn != NULL) {
				*pixelsDrawn += pixelsDrawnHere;
			}
			tmpDrawInto.left += pixelsDrawnHere;
		}
		indexIntoText_VISUAL += reLength;
	}
}

void	StyledTextImager::MeasureSegmentWidth (size_t from, size_t to, const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	// See SPR#1293 - may want to pass in TextLayoutBlock here - instead of just plain text...
	vector<RunElement>	outputSummary	=	SummarizeStyleMarkers (from, to);

	size_t	outputSummaryLength	=	outputSummary.size ();
	size_t	indexIntoText		=	0;
	for (size_t i = 0; i < outputSummaryLength; i++) {
		const RunElement&	re	=	outputSummary[i];
		size_t	reFrom		=	indexIntoText + from;
		size_t	reLength	=	re.fLength;
		size_t	reTo		=	reFrom + reLength;
		Led_Assert (indexIntoText <= to-from);
		if (re.fMarker == NULL) {
			MeasureSegmentWidth_ (GetDefaultFont (), reFrom, reTo, &text[indexIntoText], &distanceResults[indexIntoText]);
		}
		else {
			re.fMarker->MeasureSegmentWidth (this, re, reFrom, reTo, &text[indexIntoText], &distanceResults[indexIntoText]);
		}
		if (indexIntoText != 0) {
			Led_Distance	addX	=	distanceResults[indexIntoText-1];
			for (size_t j = 0; j < reLength; j++) {
				distanceResults[indexIntoText + j] += addX;
			}
		}
		indexIntoText += reLength;
	}
}

Led_Distance	StyledTextImager::MeasureSegmentHeight (size_t from, size_t to) const
{
	// See SPR#1293 - may want to pass in TextLayoutBlock here ... and then pass that to SummarizeStyleMarkers ()
	Led_Require (from <= to);
	if (from == to) {			// HACK/TMP? SO WE GET AT LEAST ONE SUMMARY RECORD?? LGP 951018
		to = from + 1;
	}

	vector<RunElement>	outputSummary	=	SummarizeStyleMarkers (from, to);

	/*
	 *	This is somewhat subtle.
	 *
	 *	If we have a mixture of pictures and text on the same line, we want to
	 *	have the pictures resting on the baseline (aligned along the bottom edge
	 *	even if the picts have different height).
	 *
	 *	We also want decenders (like the bottom of a g) to go BELOW the picture.
	 *
	 *	This behavior isn't anything I dreamed up. And its not what I implemented
	 *	originally. I've copied the behavior of other editors. So presumably
	 *	somebody put some thought into the reasons for this. They are lost
	 *	on me :-) -- LGP 960314
	 */
	size_t			outputSummaryLength	=	outputSummary.size ();
	Led_Assert (outputSummaryLength != 0);
	Led_Distance	maxHeightAbove		=	0;
	Led_Distance	maxHeightBelow		=	0;
	size_t			indexIntoText		=	0;
	for (size_t i = 0; i < outputSummaryLength; i++) {
		const RunElement&	re	=	outputSummary[i];
		size_t	reFrom		=	indexIntoText + from;
		size_t	reLength	=	re.fLength;
		size_t	reTo		=	reFrom + reLength;
		Led_Assert (indexIntoText <= to-from);
		Led_Distance	itsBaseline;
		Led_Distance	itsHeight;
		if (re.fMarker == NULL) {
			itsBaseline = MeasureSegmentBaseLine_ (GetDefaultFont (), reFrom, reTo);
			itsHeight =	MeasureSegmentHeight_ (GetDefaultFont (), reFrom, reTo);
		}
		else {
			itsBaseline = re.fMarker->MeasureSegmentBaseLine (this, re, reFrom, reTo);
			itsHeight =	re.fMarker->MeasureSegmentHeight (this, re, reFrom, reTo);
		}
		maxHeightAbove = Led_Max (maxHeightAbove, itsBaseline);
		maxHeightBelow = Led_Max (maxHeightBelow, (itsHeight-itsBaseline));
		indexIntoText += reLength;
	}
	return maxHeightAbove + maxHeightBelow;
}

Led_Distance	StyledTextImager::MeasureSegmentBaseLine (size_t from, size_t to) const
{
	// See SPR#1293 - may want to pass in TextLayoutBlock here ... and then pass that to SummarizeStyleMarkers ()
	Led_Require (from <= to);
	if (from == to) {			// HACK/TMP? SO WE GET AT LEAST ONE SUMMARY RECORD?? LGP 951018
		to = from + 1;
	}

	vector<RunElement>	outputSummary		=	SummarizeStyleMarkers (from, to);
	size_t				outputSummaryLength	=	outputSummary.size ();
	Led_Assert (outputSummaryLength != 0);
	Led_Distance	maxHeight		=	0;
	size_t			indexIntoText	=	0;
	for (size_t i = 0; i < outputSummaryLength; i++) {
		const RunElement&	re	=	outputSummary[i];
		size_t	reFrom		=	indexIntoText + from;
		size_t	reLength	=	re.fLength;
		size_t	reTo		=	reFrom + reLength;
		Led_Assert (indexIntoText <= to-from);
		if (re.fMarker == NULL) {
			maxHeight = Led_Max (maxHeight, MeasureSegmentBaseLine_ (GetDefaultFont (), reFrom, reTo));
		}
		else {
			maxHeight = Led_Max (maxHeight, re.fMarker->MeasureSegmentBaseLine (this, re, reFrom, reTo));
		}
		indexIntoText += reLength;
	}
	return maxHeight;
}

#if		qDebug
void	StyledTextImager::Invariant_ () const
{
}
#endif








/*
 ********************************************************************************
 ************************** TrivialFontSpecStyleMarker **************************
 ********************************************************************************
 */
int		TrivialFontSpecStyleMarker::GetPriority () const
{
	return eBaselinePriority + 1;
}



#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

