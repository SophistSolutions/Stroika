/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/SimpleTextStore.cpp,v 2.44 2003/03/21 14:48:27 lewis Exp $
 *
 * Changes:
 *	$Log: SimpleTextStore.cpp,v $
 *	Revision 2.44  2003/03/21 14:48:27  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.43  2003/03/19 13:46:48  lewis
 *	SPR#1352 - small tweeks - like using BlockAllocation and PUSH_BACK
 *	
 *	Revision 2.42  2003/03/16 16:15:32  lewis
 *	SPR#1345 - as a small part of this SPR - switch to using SimpleUpdater in place of direct
 *	calls to DoAboutToUpdateCalls()
 *	
 *	Revision 2.41  2003/02/28 19:07:43  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.40  2002/11/17 15:55:40  lewis
 *	SPR#1173- Replace/ReplaceWithoutUpdate change
 *	
 *	Revision 2.39  2002/10/24 01:57:19  lewis
 *	GCC bug workaround - qVirtualBaseMixinCallDuringCTORBug
 *	
 *	Revision 2.38  2002/09/13 15:29:26  lewis
 *	Add TextStore::ConstructNewTextStore () support
 *	
 *	Revision 2.37  2002/05/06 21:33:50  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.36  2001/11/27 00:29:55  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.35  2001/09/26 15:41:20  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.34  2001/08/28 18:43:38  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.33  2001/07/19 21:05:21  lewis
 *	fix some small sytnax quirks only complained about by MWERKS compiler
 *	
 *	Revision 2.32  2001/03/05 21:17:05  lewis
 *	SPR#0855- be sure TextStore itself is part of MarkerOwner list (so it gets adjusted by
 *	new code in ChunkedArrayTextStore which keeps separate marker trees for each owner)
 *	
 *	Revision 2.31  2000/08/13 03:30:14  lewis
 *	SPR#0822- added TextStore::PreRemoveMarker
 *	
 *	Revision 2.30  2000/07/26 05:46:36  lewis
 *	SPR#0491/#0814. Lose CompareSameMarkerOwner () stuff, and add new MarkerOwner arg to
 *	CollectAllMarkersInRagne() APIs. And a few related changes. See the SPRs for a table of speedups.
 *	
 *	Revision 2.29  2000/04/14 22:40:26  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.28  1999/11/28 13:50:54  lewis
 *	Fix Typos
 *	
 *	Revision 2.27  1999/11/28 13:38:40  lewis
 *	add extra Led_Require() check in DTOR to make end-user debugging easier
 *	
 *	Revision 2.26  1999/11/13 16:32:22  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.25  1999/05/03 22:05:14  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.24  1999/04/15 20:34:50  lewis
 *	use std::find/erase instead of IndexOf() template - originally cuz of trouble with
 *	new mwerks compiler - buy maybe better this way anyhow
 *	
 *	Revision 2.23  1998/10/30 14:40:22  lewis
 *	Use vector<> instead of Led_Array
 *	
 *	Revision 2.22  1998/04/25  01:32:58  lewis
 *	Fix for UNICODE - multiply howManyToMove (etc) by sizeof (Led_tChar) in memmove/memcpy calls
 *
 *	Revision 2.21  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1997/09/29  15:14:07  lewis
 *	Lose qLedFirstIndex and qSupportLed21CompatAPI support.
 *
 *	Revision 2.19  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.18  1997/07/14  00:44:32  lewis
 *	Support qLedFirstIndex == 0.
 *	Changed name of TextStore_ to TextStore.
 *	Work around qStaticInlineFunctionsInDebugModeNoInliningArentTreatedAsStatic bug.
 *
 *	Revision 2.17  1997/06/24  03:32:33  lewis
 *	Lose old test code.
 *
 *	Revision 2.16  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1997/06/18  03:02:27  lewis
 *	SimpleTextStore::CollectAllMarkersInRangeInto_21Compat and
 *	new CollectAllMarkersInRangeInto (size_t from, size_t to, MarkerSink& output) support.
 *	qIncludePrefixFile.
 *
 *	Revision 2.14  1997/03/22  15:31:28  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1997/01/10  03:10:06  lewis
 *	*** empty log message ***
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
 *	Revision 2.9  1996/08/05  06:03:43  lewis
 *	Change param names to fit new programmer docs.
 *
 *	Revision 2.8  1996/05/23  19:46:23  lewis
 *	Added throw specifiers.
 *	Lose obsolete SimpleTextStore::GetMarkerRange.
 *	Added SimpleTextStoreMarkerHook::GetStartEnd () support
 *
 *	Revision 2.7  1996/03/16  18:53:30  lewis
 *	Invariant() const.
 *
 *	Revision 2.6  1996/02/26  18:56:19  lewis
 *	Fixup RemoveMarkers() so we can move RemoveMarker implementation into
 *	base class.
 *
 *	Revision 2.5  1996/01/03  23:59:06  lewis
 *	Work around msvc qVTablesNotSetupRightDuringCTORSoCallbacksOftenFailBug.
 *
 *	Revision 2.4  1995/12/09  05:41:07  lewis
 *	On adding marker, assert that marker owner has US as textstore.
 *
 *	Revision 2.3  1995/10/19  22:25:11  lewis
 *	Code cleanups and assertions.
 *	Use new DoDidUpdateCalls/DoAbout... instead of manual loops.
 *	New xtra arg to UpdateInfo CTOR.
 *
 *	Revision 2.2  1995/10/09  22:39:41  lewis
 *	Added calls to marker owners for AboutToUpdate and DIdUpdateText calls
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.11  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.10  1995/05/21  17:51:07  lewis
 *	We now require all markers to be removed before destruction.
 *
 *	Revision 1.9  1995/05/20  04:56:58  lewis
 *	Fixed spelling error - Invariant
 *
 *	Revision 1.8  1995/05/06  19:46:20  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *
 *	Revision 1.7  1995/05/05  19:51:41  lewis
 *	Use Led_tChar instead of char - to facilitate later support of UNICODE.
 *
 *	Revision 1.6  1995/04/20  06:47:11  lewis
 *	Simplified precondition tests in Replace to be like Chunked array version.
 *
 *	Revision 1.5  1995/04/18  00:21:09  lewis
 *	Assertions and fixed constness of arg to RemoveMarkers().
 *
 *	Revision 1.4  1995/04/16  19:34:12  lewis
 *	Added override of RemoveMarkers () - SPR 207.
 *
 *	Revision 1.3  1995/03/13  03:26:08  lewis
 *	Renamed Array<T> to Led_Array<T>.
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

#include	<string.h>

#include	"SimpleTextStore.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
namespace	Led {
#endif






	class	SimpleTextStoreMarkerHook : public Marker::HookData {
		public:
			SimpleTextStoreMarkerHook ():
				Marker::HookData (),
				fStart (0),
				fLength (0),
				fOwner (NULL),
				fIsPreRemoved (false)
				{
				}

		public:
			LED_DECLARE_USE_BLOCK_ALLOCATION (SimpleTextStoreMarkerHook);

		public:
			override	MarkerOwner*	GetOwner () const;
			override	size_t			GetStart () const;
			override	size_t			GetEnd () const;
			override	size_t			GetLength () const;
			override	void			GetStartEnd (size_t* start, size_t* end)  const;

			size_t			fStart;
			size_t			fLength;
			MarkerOwner*	fOwner;
			bool			fIsPreRemoved;
	};
MarkerOwner*	SimpleTextStoreMarkerHook::GetOwner () const
{
	return fOwner;
}

size_t		SimpleTextStoreMarkerHook::GetStart () const
{
	Led_Assert (fStart < 0x8000000);		// a really big number - we don't have enough memory to trigger
											// this - only point is to test of accidental cast of negnum to
											// size_t.
	return fStart;
}

size_t			SimpleTextStoreMarkerHook::GetEnd () const
{
	Led_Assert (fStart < 0x8000000);	// See GetStart/GetEnd
	Led_Assert (fLength < 0x8000000);	// See GetStart/GetEnd
	return fStart + fLength;
}

size_t			SimpleTextStoreMarkerHook::GetLength () const
{
	Led_Assert (fLength < 0x8000000);	// See GetStart
	return fLength;
}

void			SimpleTextStoreMarkerHook::GetStartEnd (size_t* start, size_t* end)  const
{
	Led_Assert (fStart < 0x8000000);	// See GetStart
	Led_Assert (fLength < 0x8000000);	//     ''
	Led_RequireNotNil (start);
	Led_RequireNotNil (end);
	*start = fStart;
	*end = fStart + fLength;
}


	#if		qStaticInlineFunctionsInDebugModeNoInliningArentTreatedAsStatic
		#define	OurStuff	SimpleTextStore_OurStuff
	#endif
	static	inline	SimpleTextStoreMarkerHook*	OurStuff (const Marker* marker)
		{
			Led_AssertNotNil (marker);
			Led_AssertNotNil ((SimpleTextStoreMarkerHook*)marker->fTextStoreHook);
			Led_AssertMember ((SimpleTextStoreMarkerHook*)marker->fTextStoreHook, SimpleTextStoreMarkerHook);
			return (SimpleTextStoreMarkerHook*)marker->fTextStoreHook;
		}






/*
 ********************************************************************************
 ******************************** SimpleTextStore *******************************
 ********************************************************************************
 */
SimpleTextStore::SimpleTextStore ():
	TextStore (),
	fLength (0),
	fBuffer (NULL),
	fMarkers ()
{
	fBuffer = new Led_tChar[0];
	Led_AssertNotNil (fBuffer);
}

SimpleTextStore::~SimpleTextStore ()
{
	Led_Require (GetMarkerOwners ().size () == 1);	// Really this should properly be checked in the TextStore::DTOR - and it is.
													// But if this test fails, other tests within THIS DTOR will likely also fail. And
													// those can be confusing. This diagnostic should clearly indicate to users that they've
													// forgotten to remove some MarkerOwners - like Views or MarkerCovers, or ParagraphDatabases,
													// etc.

	Led_Require (fMarkers.size () == 0);		// All must have been removed by caller, otherwise its a user bug.
	delete[] fBuffer;
}

/*
@METHOD:		SimpleTextStore::ConstructNewTextStore
@DESCRIPTION:	<p>See @'TextStore::ConstructNewTextStore' ().</p>
*/
TextStore*	SimpleTextStore::ConstructNewTextStore () const
{
	return new SimpleTextStore ();
}
		
void	SimpleTextStore::CopyOut (size_t from, size_t count, Led_tChar* buffer) const throw ()
{
	// Note that it IS NOT an error to call CopyOut for multibyte characters and split them. This is one of the few
	// API routines where that is so...
	Led_RequireNotNil (buffer);
	Led_Require (from >= 0);
	Led_Require (from + count <= GetEnd ());	// Be sure all Led_tChars requested fall in range
	(void)::memcpy (buffer, &fBuffer[from], count * sizeof (Led_tChar));
}

void	SimpleTextStore::ReplaceWithoutUpdate (size_t from, size_t to, const Led_tChar* withWhat, size_t withWhatCount)
{
	Led_Assert (from <= to);
#if		qMultiByteCharacters
	Led_Assert (Led_IsValidMultiByteString (withWhat, withWhatCount));

	Assert_CharPosDoesNotSplitCharacter (from);
	Assert_CharPosDoesNotSplitCharacter (to);
#endif

	Invariant ();

// THIS ISN't QUITE RIGHT - A GOOD APPROX HOWEVER...
// cuz we don't update markers properly yet... Close - but not quite, a replace
// is treated as a delete/insert - which isn't quite what we want...
	/*
	 *	Though the implication for updating markers is slightly different, for updating just
	 *	the text, we can treat this as a delete, followed by an insert.
	 */
	DeleteAfter_ (to-from, from);
	InsertAfter_ (withWhat, withWhatCount, from);
	Invariant ();
}

void	SimpleTextStore::InsertAfter_ (const Led_tChar* what, size_t howMany, size_t after)
{
	Invariant ();
	size_t	newBufSize	=	howMany + fLength;
	Led_tChar*	newBuf		=	new Led_tChar [newBufSize];
	if (fLength != 0) {
		(void)::memcpy (newBuf, fBuffer, after * sizeof (Led_tChar));
	}
	if (howMany != 0) {
		(void)::memcpy (newBuf + after, what, howMany * sizeof (Led_tChar));
	}
	(void)::memcpy (newBuf + after + howMany, &fBuffer[after], (fLength - after) * sizeof (Led_tChar));
	delete[] fBuffer;
	fBuffer = newBuf;
	fLength = newBufSize;

	// update markers
	// since we did an insert - all this does is increment the start point for all markers that were
	// already past here and the lengths of any where the start is to the left and the right is past here.
	for (size_t i = 0; i < fMarkers.size (); i++) {
		Marker*	mi	=	fMarkers[i];
		size_t	start	=	mi->GetStart ();
		size_t	len		=	mi->GetLength ();
		size_t	end		=	start + len;
		if (after < start) {
			OurStuff (mi)->fStart = start + howMany;
		}
		else if (after >= start and after < end) {
			OurStuff (mi)->fLength = len + howMany;
		}
	}
	Invariant ();
}

void	SimpleTextStore::DeleteAfter_ (size_t howMany, size_t after)
{
	Led_Assert (after >= 0);
	Led_Assert (howMany + after <= fLength);
	Invariant ();

	Led_Assert (fLength >= howMany);
	size_t	newBufSize		=	fLength - howMany;
	size_t	howManyToMove	=	fLength - (after + howMany);
	Led_Assert (howManyToMove <= fLength);
	Led_Assert (howManyToMove + after <= fLength);
	(void)::memmove (&fBuffer[after], &fBuffer[after+howMany], howManyToMove * sizeof (Led_tChar));
	fLength -= howMany;

	// update markers
	for (size_t i = 0; i < fMarkers.size (); i++) {
		Marker*	mi	=	fMarkers[i];
		size_t	start	=	mi->GetStart ();
		size_t	len		=	mi->GetLength ();
		size_t	end		=	start + len;
		if (after < start) {
			size_t	newStart	=	start;
			if (howMany + after <= start) {
				Led_Assert (start >= howMany);
				OurStuff (mi)->fStart = start - howMany;
			}
			else {
				Led_Assert (howMany > (start-after));
				size_t	deleteNCharsOffFront = howMany - (start-after);
				size_t	moveFront = howMany - deleteNCharsOffFront;
				OurStuff (mi)->fStart = start - moveFront;
				/*
				 * Note when the whole extent is deleted - we simply pin the size to zero.
				 */
				OurStuff (mi)->fLength = (len>deleteNCharsOffFront)? (len-deleteNCharsOffFront): 0;
			}
		}
		else if (after >= start and after < end) {
			size_t	newEnd	=	end;
			if (end-after < howMany) {
				newEnd = after;
			}
			else {
				newEnd -= howMany;
			}
			Led_Assert (newEnd >= start);
			size_t	newLen	=	newEnd - start;
			OurStuff (mi)->fLength = newLen;
		}
	}
	Invariant ();
}

void	SimpleTextStore::AddMarker (Marker* marker, size_t lhs, size_t length, MarkerOwner* owner)
{
	Led_RequireNotNil (marker);
	Led_RequireNotNil (owner);
	#if		!qVirtualBaseMixinCallDuringCTORBug
	Led_Require (owner->PeekAtTextStore () == this);
	#endif
	Led_Require (owner == this or IndexOf (GetMarkerOwners (), owner) != kBadIndex);	// new Led 2.3 requirement - not strictly required internally yet - but it will be - LGP 980416
	Led_Require (IndexOf (fMarkers, marker) == kBadIndex);	// better not be there!
	Led_Require (lhs < 0x80000000);				// not real test, just sanity check
	Led_Require (length < 0x80000000);			// not real test, just sanity check
	Invariant ();
	Led_Assert (marker->fTextStoreHook == NULL);
	marker->fTextStoreHook = new SimpleTextStoreMarkerHook ();
	Led_Assert (marker->GetOwner () == NULL);
	OurStuff (marker)->fOwner = owner;
	OurStuff (marker)->fStart = lhs;
	OurStuff (marker)->fLength = length;

	/*
	 *	Insert the marker in-order in the list.
	 */
// For now - assume we always append - for marker sub-order...
	for (size_t i = 0; i < fMarkers.size (); i++) {
		Marker*	mi	=	fMarkers[i];
		if (mi->GetStart () > lhs) {
			// we've gone one too far!!!
			//fMarkers.InsertAt (marker, i);
			fMarkers.insert (fMarkers.begin () + i, marker);
			Invariant ();
			return;
		}
	}
	// if we never found a marker greater - we must be greatest of all - and so append
	//fMarkers.push_back (marker);
	PUSH_BACK (fMarkers, marker);
	Invariant ();
}

void	SimpleTextStore::RemoveMarkers (Marker*const markerArray[], size_t markerCount)
{
	Led_Assert (markerCount == 0 or markerArray != NULL);
	for (size_t i = 0; i < markerCount; i++) {
		Marker*	marker	=	markerArray [i];
		if (marker->fTextStoreHook != NULL) {
			Led_AssertNotNil (marker->GetOwner ());
			Invariant ();
#if 1
			vector<Marker*>::iterator   index   =   std::find (fMarkers.begin (), fMarkers.end (), marker);
			Led_Assert (index != fMarkers.end ());
			fMarkers.erase (index);
#else
			size_t	index	=	IndexOf (fMarkers, marker);
		//	fMarkers.RemoveAt (index);
			fMarkers.erase (fMarkers.begin () + index);
#endif
			OurStuff (marker)->fOwner = NULL;
			Invariant ();
			delete marker->fTextStoreHook;
			marker->fTextStoreHook = NULL;
		}
	}
}

void	SimpleTextStore::PreRemoveMarker (Marker* marker)
{
	Led_RequireNotNil (marker);
	Led_Require (not OurStuff (marker)->fIsPreRemoved);
	OurStuff (marker)->fIsPreRemoved = true;
}

void	SimpleTextStore::SetMarkerRange (Marker* marker, size_t start, size_t end) throw ()
{
	Led_Assert (start >= 0);
	Led_Assert (end >= 0);
	Led_Assert (start <= end);
	Led_AssertNotNil (marker);

	// changing the start may force a re-ordering...
	if (marker->GetStart () == start) {
		OurStuff (marker)->fLength = end-start;
	}
	else {
// Really - we should do better than this and NOT force a re-ordering if not needed...
		MarkerOwner*	owner	=	marker->GetOwner ();
		RemoveMarker (marker);
		AddMarker (marker, start, end-start, owner);
	}
}

void	SimpleTextStore::CollectAllMarkersInRangeInto (size_t from, size_t to, const MarkerOwner* owner, MarkerSink& output) const
{
	Led_RequireNotNil (owner);	// though it can be TextStore::kAnyMarkerOwner.
	for (vector<Marker*>::const_iterator i = fMarkers.begin (); i != fMarkers.end (); i++) {
		Marker*	m	=	*i;
		Led_AssertNotNil (m);
		if (Overlap (*m, from, to)) {
			if (owner == kAnyMarkerOwner or owner == m->GetOwner ()) {
				if (not OurStuff (m)->fIsPreRemoved) {
					output.Append (m);
				}
			}
		}
	}
}

#if		qDebug
void	SimpleTextStore::Invariant_ () const
{
	TextStore::Invariant_ ();
	for (size_t i = 0; i < fMarkers.size (); i++) {
		Marker*	mi	=	fMarkers[i];
		Led_AssertNotNil (mi);
		Led_Assert (IndexOf (fMarkers, mi) == i);	// be sure same marker doesn't appear multiply in the
													// list!!!
		size_t	start	=	mi->GetStart ();
		size_t	len		=	mi->GetLength ();
		size_t	end		=	start + len;
		Led_Assert (start >= 0);
		Led_Assert (start <= end);
		Led_Assert (end <= GetEnd () + 1);	// allowed 1 past last valid markerpos
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
