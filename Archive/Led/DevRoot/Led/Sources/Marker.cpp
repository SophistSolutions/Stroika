/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/Marker.cpp,v 2.20 2003/11/26 16:18:51 lewis Exp $
 *
 * Changes:
 *	$Log: Marker.cpp,v $
 *	Revision 2.20  2003/11/26 16:18:51  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.19  2003/05/20 21:26:05  lewis
 *	SPR#1492: fixed the semantics of TempMarker to match its documentation - 3rd arg is end - not length
 *	
 *	Revision 2.18  2003/02/28 19:05:58  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.17  2003/01/11 19:28:42  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.16  2003/01/08 15:36:55  lewis
 *	moved TextImager::CopyOut etc to MarkerOwner::CopyOut (same for GetLength/GetEnd/FindNextCharacter/FindPrevCharacter.
 *	Done so they can be somewhat more easily used. No longer inline - but thats not a big deal since not used in
 *	performance critical places (and if they are - you can manually use/cache the TextStore and call its version).
 *	Also - got rid of obsolete TextImager::PeekAtTextStore_ (some old compile BWA)
 *	
 *	Revision 2.15  2002/05/06 21:33:48  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.14  2001/11/27 00:29:53  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.13  2001/08/28 18:43:37  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2000/04/24 21:10:45  lewis
 *	Added TempMarker utility class
 *	
 *	Revision 2.11  1999/11/13 16:32:21  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.10  1999/05/03 22:05:11  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.9  1997/12/24 04:40:13  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.8  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.7  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1997/06/18  02:59:05  lewis
 *	qIncludePrefixFile
 *
 *	Revision 2.5  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.4  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1996/03/16  18:54:31  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.5  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.4  1995/05/18  08:11:33  lewis
 *	Lose old CW5.* bug workarounds (SPR#0249).
 *
 *	Revision 1.3  1995/03/13  03:23:10  lewis
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

#include	"TextStore.h"

#include	"Marker.h"




#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif



#if		qLedUsesNamespaces
	namespace	Led {
#endif



/*
 ********************************************************************************
 ************************************ MarkerOwner *******************************
 ********************************************************************************
 */
/*
@METHOD:		MarkerOwner::FindNextCharacter
@DESCRIPTION:	<p>Return the associated @'TextStore::FindNextCharacter' ().</p>
*/
size_t	MarkerOwner::FindNextCharacter (size_t afterPos) const
{
	return GetTextStore ().FindNextCharacter (afterPos);
}

/*
@METHOD:		MarkerOwner::FindPreviousCharacter
@DESCRIPTION:	<p>Return the associated @'TextStore::FindPreviousCharacter' ().</p>
*/
size_t	MarkerOwner::FindPreviousCharacter (size_t beforePos) const
{
	return GetTextStore ().FindPreviousCharacter (beforePos);
}

/*
@METHOD:		MarkerOwner::GetLength
@DESCRIPTION:	<p>Return the associated @'TextStore::GetLength' ().</p>
*/
size_t	MarkerOwner::GetLength () const
{
	return (GetTextStore ().GetLength ());
}

/*
@METHOD:		MarkerOwner::GetEnd
@DESCRIPTION:	<p>Return the associated @'TextStore::GetEnd' ().</p>
*/
size_t	MarkerOwner::GetEnd () const
{
	return (GetTextStore ().GetEnd ());
}

/*
@METHOD:		MarkerOwner::CopyOut
@DESCRIPTION:	<p>Calls the associated @'TextStore::CopyOut' ().</p>
*/
void	MarkerOwner::CopyOut (size_t from, size_t count, Led_tChar* buffer) const
{
	GetTextStore ().CopyOut (from, count, buffer);
}

#if		qMultiByteCharacters
void	MarkerOwner::Assert_CharPosDoesNotSplitCharacter (size_t charPos) const
{
	#if		qDebug
		GetTextStore ().Assert_CharPosDoesNotSplitCharacter (charPos);
	#endif
}
#endif




/*
 ********************************************************************************
 ************************************ TempMarker ********************************
 ********************************************************************************
 */
TempMarker::TempMarker (TextStore& ts, size_t start, size_t end):
	inherited (),
	fTextStore (ts),
	fMarker ()
{
	Led_Require (start <= end);
	GetTextStore ().AddMarkerOwner (this);
	try {
		GetTextStore ().AddMarker (&fMarker, start, end-start, this);
	}
	catch (...) {
		GetTextStore ().RemoveMarkerOwner (this);
		throw;
	}
}

TempMarker::~TempMarker ()
{
	try {
		GetTextStore ().RemoveMarker (&fMarker);
	}
	catch (...) {
	}
	try {
		GetTextStore ().RemoveMarkerOwner (this);
	}
	catch (...) {
	}
}

TextStore*	TempMarker::PeekAtTextStore () const
{
	return &fTextStore;
}



#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
