/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/HiddenText.cpp,v 2.42 2003/05/20 21:37:55 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: HiddenText.cpp,v $
 *	Revision 2.42  2003/05/20 21:37:55  lewis
 *	SPR#1492: changed 3rd arg to TempMarker
 *	
 *	Revision 2.41  2003/05/08 18:50:19  lewis
 *	SPR#1474: Back doing the a4 release, I revised Led to call SimpleUpdater, rather thanDoABoutTo/DoDidUpdate calls directly. In the HiddenText code - I wasn't carefull enough doing that conversion about nesting the scope of the stack objects (simpleupdater) to make sure the DidUpdates get called early enough. FIXED by basically adding {} in a couple places
 *	
 *	Revision 2.40  2003/03/27 16:16:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.39  2003/03/27 15:55:19  lewis
 *	Delete ifdefed code. SPR#1384: SimpleUpdater realContentUpdate=false for CollapseMarker/ReifyMarker so they dont trigger failure with readonly text and dont make doc dirty
 *	
 *	Revision 2.38  2003/03/21 14:47:39  lewis
 *	added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 2.37  2003/03/19 13:49:17  lewis
 *	SPR#1352- small tweeks to ~HidableTextMarkerOwner ()
 *	
 *	Revision 2.36  2003/03/16 16:15:31  lewis
 *	SPR#1345 - as a small part of this SPR - switch to using SimpleUpdater in place of direct
 *	calls to DoAboutToUpdateCalls()
 *	
 *	Revision 2.35  2002/12/06 15:59:19  lewis
 *	SPR#1197- Use TextStore::SimpleUpdater to properly scope Do/Did update calls around change to fShown
 *	
 *	Revision 2.34  2002/10/02 00:20:45  lewis
 *	SPR#1117- lose commandhandler argument to UniformHidableTextMarkerOwner, ColoredUniformHidableTExtMarkerOwner etc
 *	
 *	Revision 2.33  2002/05/06 21:33:43  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.32  2001/11/27 00:29:50  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.31  2001/10/17 20:42:57  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.30  2001/09/26 15:41:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.29  2001/08/29 23:36:31  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.28  2001/08/28 18:43:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.27  2000/11/17 17:00:57  lewis
 *	SPR#0847- use &* to workaround bug in particular version of STL used by one customer (Remo)
 *	
 *	Revision 2.26  2000/10/05 02:37:35  lewis
 *	added ColoredUniformHidableTextMarkerOwner
 *	
 *	Revision 2.25  2000/08/28 20:25:21  lewis
 *	misc cleanups - relating to LightUnderlineHidableTextMarker and other changes so
 *	hidable text (shown) looks better
 *	
 *	Revision 2.24  2000/08/13 04:37:44  lewis
 *	remove SuppressedDidUpdateText code since not needed anymore
 *	
 *	Revision 2.23  2000/08/13 04:35:19  lewis
 *	remove SuppressedDidUpdateText code since not needed anymore, and ifdef out a bug workaround
 *	hack I think is no longer needed
 *	
 *	Revision 2.22  2000/08/13 04:16:58  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.21  2000/08/13 04:15:06  lewis
 *	SPR#0784- rewrite hidden text code based on setting segwidth to zero, and patching computeNExtCursorPosition.
 *	Not 100% done - but mostly. VERY big change -  and many bug fixes related
 *	to this hidden text stuff folded in as well - but not complete
 *	
 *	Revision 2.20  2000/07/26 05:46:36  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner arg
 *	to CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table of speedups.
 *	
 *	Revision 2.19  2000/05/08 15:31:26  lewis
 *	Work around spr#0757
 *	
 *	Revision 2.18  2000/05/01 19:27:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.17  2000/04/29 23:49:12  lewis
 *	convert tmphack globals for supressing this and that into instance variables and used classes to
 *	safely construct/destruct/set/unset the values (in presence of exceptions)
 *	
 *	Revision 2.16  2000/04/27 02:03:18  lewis
 *	suppress an invariant/assert error with SUPRESS_INVARIANT_LOCALLY helper class
 *	
 *	Revision 2.15  2000/04/26 14:54:18  lewis
 *	many kludges we still must cleanup - but now SPRs#0722 and 0731 mostly (if not entirely) working
 *	
 *	Revision 2.14  2000/04/25 16:16:30  lewis
 *	partial work on SPR#0722- added HidableTextMarkerOwner::GetHidableRegionsWithData () and started using that.
 *	
 *	Revision 2.13  2000/04/25 00:58:02  lewis
 *	do extra CollectAllMarkerInRnage / Do/Did Update calls for UnHideMarkers case - cuz could have
 *	expanded the range! - check RHS of range - finally finishes up (known problems with) SPR# 0724
 *	
 *	Revision 2.12  2000/04/24 21:23:06  lewis
 *	quick cut at using new Internalizer/Externalizer code to save/restore contents of a hidabletext
 *	marker when hidden/restored
 *	
 *	Revision 2.11  2000/04/16 14:39:36  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.10  2000/04/14 22:40:24  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.9  2000/03/31 23:45:37  lewis
 *	SPR#0717- got first cut (imperfect) at saving hidableText in RTF (problem is assumes its currently
 *	being shown). Other cleanups - esp to hidable text code (invariants)
 *	
 *	Revision 2.8  2000/03/31 21:17:40  lewis
 *	Added quick-and-dirty implemention of HidableTextMarkerOwner::GetHidableRegionsContiguous (). And
 *	added HidableTextMarkerOwner::GetHidableRegions () - related to spr#0717.
 *	
 *	Revision 2.7  2000/03/31 00:50:04  lewis
 *	more work on SPR#0717: support RTF reading of \v tags - hidden text. Integrated HiddenText support
 *	into WordProcessor class (in a maner patterend after how we treat WordProcessor::ParagraphDatabasePtr
 *	
 *	Revision 2.6  2000/03/30 16:58:54  lewis
 *	now delete zero-sized hidden markers when they are in the 'shown' state. Other bugfixes
 *	
 *	Revision 2.5  2000/03/30 16:28:15  lewis
 *	renamed HiddenTextMarker* to HidableTextMarker*. Added Invariant () code. Implemented
 *	MakeRegionHidable/MakeRegionUNHidable. Got rid of fMarkers vector<> (after an unsucccessful
 *	experiment with using a map<>).
 *	
 *	Revision 2.4  2000/03/29 23:35:33  lewis
 *	SPR#0720 - more cleanups of SimpleStyleMarkerByFontSpec code
 *	
 *	Revision 2.3  2000/03/28 03:41:04  lewis
 *	more work on qOldStyleMarkerBackCompatHelperCode/SPR#0718. Now StyleMarker class takes extra
 *	RunElement argument. Now this new code is called by default StyledTextImager::DrawText() etc overrides.
 *	And now HiddenText code properly merges at least its own drawing and StandardStyledTextImager::StyleMarker.
 *	All this respecting qOldStyleMarkerBackCompatHelperCode - so old code (not new hidden text code) should
 *	still compile and work roughly as before with this turned on.
 *	
 *	Revision 2.2  2000/03/17 22:32:26  lewis
 *	a bit more work - now partially functional. Can mark discontiguous regions as hidable - and then show/hide commands work
 *	
 *	Revision 2.1  2000/03/16 00:32:32  lewis
 *	Very preliminary first cut at this new module - part of SPR#0717
 *	
 *
 *
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"LedConfig.h"

#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (disable : 4786)
#endif

#include	"StandardStyledTextImager.h"

#include	"HiddenText.h"



#include <iterator>





#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif










/*
 ********************************************************************************
 ****************************** HidableTextMarkerOwner **************************
 ********************************************************************************
 */
HidableTextMarkerOwner::HidableTextMarkerOwner (TextStore& textStore):
	inherited (),
	fInternalizer (),
	fExternalizer (),
	fTextStore (textStore),
	fMarkersToBeDeleted ()
{
	SetInternalizer (NULL);	// sets default
	SetExternalizer (NULL);	// DITTO
	fTextStore.AddMarkerOwner (this);
}

HidableTextMarkerOwner::~HidableTextMarkerOwner ()
{
	Led_Require (fMarkersToBeDeleted.IsEmpty ());
	try {
		MarkerList	markers	=	CollectAllInRange (0, fTextStore.GetLength () + 1);
		if (not markers.empty ()) {
			{
				vector<Marker*>	tmp;
				copy (markers.begin (), markers.end (), inserter (tmp, tmp.begin ()));
				// Note - this kookie &* stuff is to work around bugs in some STLs - that don't let you convert an iterator to a pointer.- SPR#0847
				GetTextStore ().RemoveMarkers (&*tmp.begin (), tmp.size ());
			}
			for (MarkerList::const_iterator i = markers.begin (); i != markers.end (); ++i) {
				delete (*i);
			}
		}
		fTextStore.RemoveMarkerOwner (this);
	}
	catch (...) {
		Led_Assert (false);	// someday - handle exceptions here better - should cause no harm but memory leak (frequently - but sometimes worse),
							// and should be exceedingly rare - LGP 2000/03/30
	}
}

/*
@METHOD:		HidableTextMarkerOwner::HideAll
@DESCRIPTION:	<p>Tell all the existing 'hidden text' markers (optionally restricted to those intersecting
	the 'from' .. 'to' range) to hide themselves. This does <em>not</em> create any new @'HidableTextMarkerOwner::HidableTextMarker's.
	Call @'HidableTextMarkerOwner::MakeRegionHidable' for that.</p>
		<p>See @'HidableTextMarkerOwner::ShowAll' to re-show them.</p>
*/
void	HidableTextMarkerOwner::HideAll ()
{
	HideAll (0, fTextStore.GetEnd () + 1);
}

void	HidableTextMarkerOwner::HideAll (size_t from, size_t to)
{
	Invariant ();
	MarkerList	markers	=	CollectAllInRange (from, to);
	sort (markers.begin (), markers.end (), LessThan<HidableTextMarker> ());
	// proceed in reverse direction - so that any markers being shown won't affect our text offsets
	for (MarkerList::reverse_iterator i = markers.rbegin (); i != markers.rend (); ++i) {
		if (TextStore::Overlap (**i, from, to)) {
			(*i)->Hide ();
		}
	}
	Invariant ();
}

/*
@METHOD:		HidableTextMarkerOwner::ShowAll
@DESCRIPTION:	<p>Tell all the existing 'hidden text' markers (optionally restricted to those intersecting
	the 'from' .. 'to' range) to show themselves. This does <em>not</em> destroy any new @'HidableTextMarkerOwner::HidableTextMarker's.
	It merely re-installs their context into the document so that it can be seen and edited.</p>
		<p>See also @'HidableTextMarkerOwner::HideAll'.</p>
*/
void	HidableTextMarkerOwner::ShowAll ()
{
	ShowAll (0, fTextStore.GetEnd () + 1);
}

void	HidableTextMarkerOwner::ShowAll (size_t from, size_t to)
{
	Invariant ();
	MarkerList	markers	=	CollectAllInRange (from, to);
	sort (markers.begin (), markers.end (), LessThan<HidableTextMarker> ());
	// proceed in reverse direction - so that any markers being shown won't affect our text offsets
	for (MarkerList::reverse_iterator i = markers.rbegin (); i != markers.rend (); ++i) {
		if (TextStore::Overlap (**i, from, to)) {
			#if		qDebug
				HidableTextMarkerOwner::Invariant_ ();	// don't make virtual call - cuz that might not be fully valid til end of routine...
			#endif
			(*i)->Show ();
			#if		qDebug
				HidableTextMarkerOwner::Invariant_ ();	// don't make virtual call - cuz that might not be fully valid til end of routine...
			#endif
		}
	}
	Invariant ();
}

/*
@METHOD:		HidableTextMarkerOwner::MakeRegionHidable
@DESCRIPTION:	<p>Mark the region from 'from' to 'to' as hidable. This could involve coalescing adjacent hidden text markers
	(though even adjacent markers are sometimes <em>NOT</em> coalesced, if they differ in shown/hidden state, or if one is already
	hidden - cuz then it would be too hard to combine the two).</p>
		<p>This routine assures that after the call - all text in the given range is encapsulated by a hidden-text
	marker or markers.</p>
		<p>Note - this does <em>NOT</em> actually hide the text. You must then call @'HidableTextMarkerOwner::HideAll' and give
	it the same range given this function to get the text to actaully disapear from the screen.</p>
		<p>See also @'HidableTextMarkerOwner::MakeRegionUnHidable'</p>
*/
void	HidableTextMarkerOwner::MakeRegionHidable (size_t from, size_t to)
{
	Led_Require (from <= to);
	Invariant ();

	if (from == to) {
		return;			// degenerate behavior
	}

	MarkerList	hidableTextMarkersInRange	=	CollectAllInRange (from, to);

	// short circuit some code as a performance tweek
	if (hidableTextMarkersInRange.size () == 1 and 
		hidableTextMarkersInRange[0]->GetStart () <= from and
		hidableTextMarkersInRange[0]->GetEnd () >= to
		) {
		return;
	}

	sort (hidableTextMarkersInRange.begin (), hidableTextMarkersInRange.end (), LessThan<HidableTextMarker> ());

	HidableTextMarker*	prevNonEmptyMarker	=	NULL;	// used for coalescing...

	if (from > 0) {
		MarkerList		tmp	=	CollectAllInRange (from-1, from);
		// Can sometimes have more than one, if two different hidable region markers didn't get coalesced.
		for (MarkerList::const_iterator i = tmp.begin (); i != tmp.end (); ++i) {
			if ((*i)->IsShown () and (*i)->GetEnd () == from) {
				prevNonEmptyMarker = *i;
				break;
			}
		}
	}

	// Update other markers and owners - since this change can affect the display
	{
		TextStore::SimpleUpdater	updater (fTextStore, from, to);

		// iterate through markers, and eliminate all but one of them. The last one - if it exists - we'll enlarge.
		for (MarkerList::const_iterator i = hidableTextMarkersInRange.begin (); i != hidableTextMarkersInRange.end (); ++i) {
			if (prevNonEmptyMarker == NULL) {
				Led_Assert (i == hidableTextMarkersInRange.begin ());	// must have been first marker...
				prevNonEmptyMarker = *i;
				if (from < prevNonEmptyMarker->GetStart ()) {
					if (prevNonEmptyMarker->IsShown ()) {
						fTextStore.SetMarkerStart (prevNonEmptyMarker, from);
					}
					else {
						// cannot combine hidden and shown markers, so must create a new one
						Led_Assert (prevNonEmptyMarker->GetStart () > from);
						GetTextStore ().AddMarker (MakeHidableTextMarker (), from, prevNonEmptyMarker->GetStart ()-from, this);
					}
				}
			}
			else if (prevNonEmptyMarker->IsShown ()) {
				// If its shown - we can coalesce it - so delete old one
				fMarkersToBeDeleted.AccumulateMarkerForDeletion (*i);
			}
		}
		if (prevNonEmptyMarker == NULL) {
			GetTextStore ().AddMarker (MakeHidableTextMarker (), from, to-from, this);
		}
		else {
			if (prevNonEmptyMarker->GetEnd () < to) {
				if (prevNonEmptyMarker->IsShown ()) {
					fTextStore.SetMarkerEnd (prevNonEmptyMarker, to);
				}
				else {
					// cannot combine hidden and shown markers, so must create a new one
					Led_Assert (to > prevNonEmptyMarker->GetEnd ());
					GetTextStore ().AddMarker (MakeHidableTextMarker (), prevNonEmptyMarker->GetEnd (), to - prevNonEmptyMarker->GetEnd (), this);
				}
			}
		}
	}

	Invariant ();
}

/*
@METHOD:		HidableTextMarkerOwner::MakeRegionUnHidable
@DESCRIPTION:	<p>Remove any hidable-text markers in the given range. The markers could be either hidden or shown
	at the time. This could involce splitting or coalescing adjacent markers.</p>
		<p>See also @'HidableTextMarkerOwner::MakeRegionHidable'.</p>
*/
void	HidableTextMarkerOwner::MakeRegionUnHidable (size_t from, size_t to)
{
	Led_Require (from <= to);
	Invariant ();

	if (from == to) {
		return;			// degenerate behavior
	}

	MarkerList	hidableTextMarkersInRange	=	CollectAllInRange (from, to);
	// short circuit some code as a performance tweek
	if (hidableTextMarkersInRange.empty ()) {
		return;
	}

	TempMarker	pastSelMarker (GetTextStore (), to + 1, to + 1);

	// Update other markers and owners - since this change can affect the display
	{
		TextStore::SimpleUpdater	updater (fTextStore, from, to);
		{
			sort (hidableTextMarkersInRange.begin (), hidableTextMarkersInRange.end (), LessThan<HidableTextMarker> ());

			// iterate through markers, and eliminate all of them, except maybe on the endpoints - if they have stuff outside
			// this range
			for (MarkerList::const_iterator i = hidableTextMarkersInRange.begin (); i != hidableTextMarkersInRange.end (); ++i) {
				if (i == hidableTextMarkersInRange.begin () and (*i)->GetStart () < from) {
					// merely adjust its end-point so not overlapping this region. Be careful if he
					// used to extend past to, and cons up NEW marker for that part.
					size_t	oldEnd	=	(*i)->GetEnd ();
					fTextStore.SetMarkerEnd (*i, from);
					if (oldEnd > to) {
						fTextStore.AddMarker (MakeHidableTextMarker (), to, oldEnd-to, this);
					}
				}
				else if (i+1 == hidableTextMarkersInRange.end () and (*i)->GetEnd () > to) {
					// merely adjust its start-point so not overlapping this region
					fTextStore.SetMarkerStart (*i, to);
				}
				else {
					fMarkersToBeDeleted.AccumulateMarkerForDeletion (*i);
				}
			}
		}
	}

	/*
	 *	If - as a result of the unhiding - we've expanded our text - we may not have done a 'DIDUPDATE' for enough markers.
	 *	So catch the ones we've missed.
	 *
	 *	Note 100% sure this is a good enough test - but I hope so - LGP 2000/04/24
	 */
	{
		TextStore::SimpleUpdater	updater (fTextStore, to, pastSelMarker.GetEnd ());
	}

	Invariant ();
}

/*
@METHOD:		HidableTextMarkerOwner::GetHidableRegions
@DESCRIPTION:	<p>Return a @'DiscontiguousRun<DATA>' list (where DATA=void) of regions of hidable text.
	Regions returned are relative to offset 'from'. So - for example - if we have hidden text from
	5..8, and you call GetHidableRegions (2,8) you'll get back the list [[3,3]].</p>
		<p>NB: this routine only returns the hidable regions which are currently being SHOWN - not any invisible
	ones. This is because otherwise the run information would be useless, and not convey the
	actual sizes of the hidden text.</p>
		<p>See also @'HidableTextMarkerOwner::GetHidableRegionsWithData'.</p>
*/
DiscontiguousRun<bool>	HidableTextMarkerOwner::GetHidableRegions (size_t from, size_t to) const
{
	Invariant ();
	DiscontiguousRun<bool>	result;
	MarkerList	markers	=	CollectAllInRange (from, to);
	sort (markers.begin (), markers.end (), LessThan<HidableTextMarker> ());
	size_t		relStartFrom	=	from;
	for (MarkerList::const_iterator i = markers.begin (); i != markers.end (); ++i) {
		size_t	mStart;
		size_t	mEnd;
		(*i)->GetRange (&mStart, &mEnd);
		{
			mStart = max (mStart, relStartFrom);		// ignore if marker goes back further than our start
			mEnd = min (mEnd, to);						// ignore if end past requested end
			Led_Assert (mStart < mEnd);
			result.push_back (DiscontiguousRunElement<bool> (mStart-relStartFrom, mEnd-mStart, (*i)->IsShown ()));
			relStartFrom = mEnd;
		}
	}
	return result;
}

DiscontiguousRun<bool>	HidableTextMarkerOwner::GetHidableRegions () const
{
	return GetHidableRegions (0, fTextStore.GetEnd () + 1);
}

/*
@METHOD:		HidableTextMarkerOwner::GetHidableRegionsContiguous
@DESCRIPTION:	<p>If 'hidden' is true - then return true - iff the entire region from 'from' to 'to' is hidden.
	If 'hidden' is false, then return true iff the entire region from 'from' to 'to' contains no hidden elements.</p>
*/
bool	HidableTextMarkerOwner::GetHidableRegionsContiguous (size_t from, size_t to, bool hidden) const
{
	Invariant ();
	// Sloppy, inefficient implementation. Can be MUCH faster - since we just need to know if there are ANY in this region!
	DiscontiguousRun<bool>	tmpHack	=		GetHidableRegions (from, to);
	if (tmpHack.size () == 1) {
		return tmpHack[0].fData == hidden and
			tmpHack[0].fOffsetFromPrev == 0 and
			tmpHack[0].fElementLength >= to-from
			;
	}
	else {
		if (hidden) {
			return false;
		}
		else {
			return tmpHack.size () == 0;
		}
	}
}

/*
@METHOD:		HidableTextMarkerOwner::SetInternalizer
@DESCRIPTION:	<p>Sets the internalizer (@'FlavorPackageInternalizer' subclass). to be used with this class.
	It defaults to @'FlavorPackageInternalizer'.</p>
*/
void	HidableTextMarkerOwner::SetInternalizer (const Led_RefCntPtr<FlavorPackageInternalizer>& i)
{
	fInternalizer = i;
	if (fInternalizer.IsNull ()) {
		fInternalizer = new FlavorPackageInternalizer (GetTextStore ());;
	}
}

/*
@METHOD:		HidableTextMarkerOwner::SetExternalizer
@DESCRIPTION:	<p>Sets the externalizer (@'FlavorPackageExternalizer' subclass). to be used with this class.
	It defaults to @'FlavorPackageExternalizer'.</p>
*/
void	HidableTextMarkerOwner::SetExternalizer (const Led_RefCntPtr<FlavorPackageExternalizer>& e)
{
	fExternalizer = e;
	if (fExternalizer.IsNull ()) {
		fExternalizer = new FlavorPackageExternalizer (GetTextStore ());;
	}
}

/*
@METHOD:		HidableTextMarkerOwner::CollapseMarker
@DESCRIPTION:	
*/
void	HidableTextMarkerOwner::CollapseMarker (HidableTextMarker* m)
{
	Led_RequireNotNil (m);
	Led_Require (m->fShown);

	size_t	start	=	0;
	size_t	end		=	0;
	m->GetRange (&start, &end);
	TextStore::SimpleUpdater updater (fTextStore, start, end, false);
	m->fShown = false;
}

/*
@METHOD:		HidableTextMarkerOwner::ReifyMarker
@DESCRIPTION:	
*/
void	HidableTextMarkerOwner::ReifyMarker (HidableTextMarker* m)
{
	Led_RequireNotNil (m);
	Led_Require (not m->fShown);

	{
		size_t	start	=	0;
		size_t	end		=	0;
		m->GetRange (&start, &end);

		TextStore::SimpleUpdater updater (fTextStore, start, end, false);
		m->fShown = true;
	}
}

/*
@METHOD:		HidableTextMarkerOwner::MakeHidableTextMarker
@DESCRIPTION:	<p>This routine creates the actual marker objects to be used to hide text.</p>
				<p>By default - it
			creates @'HidableTextMarkerOwner::FontSpecHidableTextMarker' markers. You can override this to create different
			style markers, or to set differnt color etc attributes for use in your @'HidableTextMarkerOwner' subclass.</p>
*/
HidableTextMarkerOwner::HidableTextMarker*	HidableTextMarkerOwner::MakeHidableTextMarker ()
{
	/*
	 *	Some alternates you may want to consider in your overrides...
	 *

				Led_IncrementalFontSpecification	fontSpec;
				fontSpec.SetTextColor (Led_Color::kRed);
				return new LightUnderlineHidableTextMarker (fontSpec);

		or:
				Led_IncrementalFontSpecification	fontSpec;
				fontSpec.SetTextColor (Led_Color::kRed);
				#if		qWindows
				fontSpec.SetStyle_Strikeout (true);
				#endif
				return new FontSpecHidableTextMarker (fontSpec);

	 */
	return new LightUnderlineHidableTextMarker ();
}

TextStore*	HidableTextMarkerOwner::PeekAtTextStore () const
{
	return &fTextStore;
}

void	HidableTextMarkerOwner::AboutToUpdateText (const UpdateInfo& updateInfo)
{
	Invariant ();
	Led_Assert (fMarkersToBeDeleted.IsEmpty ());
	inherited::AboutToUpdateText (updateInfo);
}

void	HidableTextMarkerOwner::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	inherited::DidUpdateText (updateInfo);
	if (updateInfo.fTextModified) {
		// cull empty markers
		MarkerList	markers	=	CollectAllInRange_OrSurroundings (updateInfo.fReplaceFrom, updateInfo.GetResultingRHS ());
		for (MarkerList::const_iterator i = markers.begin (); i != markers.end (); ++i) {
			HidableTextMarker*	m	=	*i;
			if (m->GetLength () == 0) {
				fMarkersToBeDeleted.AccumulateMarkerForDeletion (m);
			}
		}
	}
	fMarkersToBeDeleted.FinalizeMarkerDeletions ();
	Invariant ();
}

HidableTextMarkerOwner::MarkerList	HidableTextMarkerOwner::CollectAllInRange (size_t from, size_t to) const
{
	MarkersOfATypeMarkerSink2Vector<HidableTextMarker>	result;
	fTextStore.CollectAllMarkersInRangeInto (from, to, this, result);
	return result.fResult;
}

HidableTextMarkerOwner::MarkerList	HidableTextMarkerOwner::CollectAllInRange_OrSurroundings (size_t from, size_t to) const
{
	MarkersOfATypeMarkerSink2Vector<HidableTextMarker>	result;
	fTextStore.CollectAllMarkersInRangeInto_OrSurroundings (from, to, this, result);
	return result.fResult;
}

#if		qDebug
void	HidableTextMarkerOwner::Invariant_ () const
{
	MarkerList		markers	=	CollectAllInRange (0, fTextStore.GetEnd () + 1);

	sort (markers.begin (), markers.end (), LessThan<HidableTextMarker> ());

	// Walk through - and see we are non-overlapping, and have no empties (unless hidden)
	// Note - we DON'T require adjacent ones be coalesced, though we try to arrange for that if possible.
	// We don't always coalece cuz if one already hidden, and we try to make a new one - its too hard to combine the adjacent
	// stored readwritepackages.
	size_t	lastEnd	=	0;
	for (size_t i = 0; i < markers.size (); i++) {
		HidableTextMarker*	m	=	markers[i];
		Led_Assert (m->GetLength () > 0);
		Led_Assert (m->GetStart () >= lastEnd);
		lastEnd = m->GetEnd ();
	}
	Led_Assert (lastEnd <= fTextStore.GetLength () + 1);
}
#endif







/*
 ********************************************************************************
 *********************** UniformHidableTextMarkerOwner **************************
 ********************************************************************************
 */
UniformHidableTextMarkerOwner::UniformHidableTextMarkerOwner (TextStore& textStore):
	inherited (textStore),
	fHidden (false)
{
}

void	UniformHidableTextMarkerOwner::HideAll ()
{
	if (not fHidden) {
		inherited::HideAll ();
		fHidden = true;
	}
}

void	UniformHidableTextMarkerOwner::ShowAll ()
{
	if (fHidden) {
		inherited::ShowAll ();
		fHidden = false;
	}
}

void	UniformHidableTextMarkerOwner::MakeRegionHidable (size_t from, size_t to)
{
	Led_Require (from <= to);

	//Not so great implemenation - could look at particular objects created - and make sure THEY have the hidden bit set...
	inherited::MakeRegionHidable (from, to);
	if (fHidden) {
		inherited::HideAll ();
	}
	else {
		inherited::ShowAll ();
	}
}








/*
 ********************************************************************************
 ************* HidableTextMarkerOwner::FontSpecHidableTextMarker ****************
 ********************************************************************************
 */
Led_FontSpecification		HidableTextMarkerOwner::FontSpecHidableTextMarker::MakeFontSpec (const StyledTextImager* /*imager*/, const RunElement& runElement) const
{
	Led_FontSpecification	fsp;
	for (vector<StyledTextImager::StyleMarker*>::const_iterator i = runElement.fSupercededMarkers.begin (); i != runElement.fSupercededMarkers.end (); ++i) {
		if (StandardStyledTextImager::StandardStyleMarker* m = dynamic_cast<StandardStyledTextImager::StandardStyleMarker*> (*i)) {
			fsp.MergeIn (m->fFontSpecification);
		}
	}
	fsp.MergeIn (fFontSpecification);	// give our fontSpec last dibs - so 'deletion' hilighting takes precedence
	return fsp;
}






/*
 ********************************************************************************
 ********* HidableTextMarkerOwner::LightUnderlineHidableTextMarker **************
 ********************************************************************************
 */
HidableTextMarkerOwner::LightUnderlineHidableTextMarker::LightUnderlineHidableTextMarker (const Led_IncrementalFontSpecification& fsp)
{
	fFontSpecification = fsp;
}

Led_Color	HidableTextMarkerOwner::LightUnderlineHidableTextMarker::GetUnderlineBaseColor () const
{
	if (fFontSpecification.GetTextColor_Valid ()) {
		return fFontSpecification.GetTextColor ();
	}
	else {
		return inherited::GetUnderlineBaseColor ();
	}
}





/*
 ********************************************************************************
 ******************* ColoredUniformHidableTextMarkerOwner ***********************
 ********************************************************************************
 */
void	ColoredUniformHidableTextMarkerOwner::FixupSubMarkers ()
{
	// Now walk all existing markers, and set their fColor field!!!
	MarkerList	markers	=	CollectAllInRange_OrSurroundings (0, GetTextStore ().GetEnd () + 1);
	for (MarkerList::const_iterator i = markers.begin (); i != markers.end (); ++i) {
		LightUnderlineHidableTextMarker*	m	=	dynamic_cast<LightUnderlineHidableTextMarker*> (*i);
		Led_AssertNotNil (m);
		if (fColored) {
			m->fFontSpecification.SetTextColor (fColor);
		}
		else {
			m->fFontSpecification.InvalidateTextColor ();
		}
	}
}

ColoredUniformHidableTextMarkerOwner::HidableTextMarker*	ColoredUniformHidableTextMarkerOwner::MakeHidableTextMarker ()
{
	Led_IncrementalFontSpecification	fontSpec;
	if (fColored) {
		fontSpec.SetTextColor (fColor);
	}
	return new LightUnderlineHidableTextMarker (fontSpec);
}



#if		qLedUsesNamespaces
}
#endif



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***


