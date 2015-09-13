/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/LedItControlItem.cpp,v 2.21 2000/04/14 19:08:50 lewis Exp $
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
 *	$Log: LedItControlItem.cpp,v $
 *	Revision 2.21  2000/04/14 19:08:50  lewis
 *	SPR#0739- Added FlavorPackage support
 *	
 *	Revision 2.20  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 2.19  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1997/12/24  04:14:33  lewis
 *	Support changes which allow new RTF saving/restoring of embeddings.
 *
 *	Revision 2.17  1997/07/27  16:01:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.16  1997/07/14  14:50:48  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.15  1997/06/18  03:42:18  lewis
 *	qIncludePrefixFile and cleanups
 *
 *	Revision 2.14  1996/12/13  18:03:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.13  1996/12/05  21:18:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1996/09/01  15:43:56  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.11  1996/06/01  02:44:49  lewis
 *	Mostly just moved implemeantion into Led_MFC code.
 *
 *	Revision 2.10  1996/05/23  20:21:52  lewis
 *	Mostly just reacted to arg chagnes in DrawSegment() can did minor cleanups for args/erasing
 *	no longer needed.
 *
 *	Revision 2.9  1996/05/14  20:43:21  lewis
 *	call Archive.Close instead of archive.Flush() to work around /fix
 *	crash that could sometimes occur in DTOR of archive (see SPR note in
 *	closed bugs file).
 *
 *	Revision 2.8  1996/05/05  14:44:22  lewis
 *	Moved some of our logic about borders to base class within Led, and now
 *	use kDefaultEmbeddingMargin from there.
 *
 *	Revision 2.7  1996/04/18  15:55:40  lewis
 *	Some cleanups, but mostly redid IO stuff to fit within the Led class lib
 *	internalize/externalize flavor package stuff, so undo/drag/drop/cut/paste
 *	all work based on shared led lib code.
 *
 *	Revision 2.6  1996/03/16  19:10:04  lewis
 *	Cleanups
 *	Fixed bugs with resizing embeddings - now we notify imager properly so
 *	it can relayout text.
 *	Especially override Serialize() on read so we set fSize field.
 *	Fix display code so we show selections properly.
 *	Override HandleOpen() todo DoVerb(PRIMARY).
 *
 *	Revision 2.5  1996/03/05  18:41:20  lewis
 *	Renamed Led_ClipboardObjectAcquire::ClipFormat --> Led_ClipFormat.
 *
 *	Revision 2.4  1996/03/04  07:58:36  lewis
 *	Override GetWriteToClipAsFormat(), GetTag, and Write (SinkStream_& sink).
 *
 *	Revision 2.3  1996/02/26  22:48:16  lewis
 *	React to changes in the Led_Rect conversion operator functions etc.
 *
 *	Revision 2.2  1996/01/22  05:41:38  lewis
 *	Use Led_Verify() instead of VERIFY
 *
 *	Revision 2.1  1996/01/11  08:30:59  lewis
 *	Lots of cleanups to positioning/sizing of embeddings.
 *
 *	Revision 2.0  1996/01/04  00:58:05  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:57:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:47:54  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"LedItDocument.h"
#include	"LedItView.h"

#include	"LedItControlItem.h"







/*
 ********************************************************************************
 ******************************** LedItControlItem ******************************
 ********************************************************************************
 */
IMPLEMENT_SERIAL(LedItControlItem, Led_MFC_ControlItem, 0)

LedItControlItem::LedItControlItem (COleDocument* pContainer):
	Led_MFC_ControlItem (pContainer)
{
}

SimpleEmbeddedObjectStyleMarker*	LedItControlItem::mkLedItControlItemStyleMarker (const char* embeddingTag, const void* data, size_t len)
{
	Led_RequireNotNil (DocContextDefiner::GetDoc ());	// See doc in header for class DocContextDefiner.
														// must declare one of these on call stack before calling this
														// method...
	Led_MFC_ControlItem*	e = new LedItControlItem (DocContextDefiner::GetDoc ());
	try {
		return mkLed_MFC_ControlItemStyleMarker_ (embeddingTag, data, len, e);
	}
	catch (...) {
		delete e;
		throw;
	}
	Led_Assert (false); return NULL; //notreached
}

SimpleEmbeddedObjectStyleMarker*	LedItControlItem::mkLedItControlItemStyleMarker (ReaderFlavorPackage& flavorPackage)
{
	Led_RequireNotNil (DocContextDefiner::GetDoc ());	// See doc in header for class DocContextDefiner.
														// must declare one of these on call stack before calling this
														// method...
	Led_MFC_ControlItem*	e = new LedItControlItem (DocContextDefiner::GetDoc ());
	try {
		return mkLed_MFC_ControlItemStyleMarker_ (flavorPackage, e);
	}
	catch (...) {
		delete e;
		throw;
	}
	Led_Assert (false); return NULL; //notreached
}

BOOL	LedItControlItem::CanActivate ()
{
	// Editing in-place while the server itself is being edited in-place
	//  does not work and is not supported.  So, disable in-place
	//  activation in this case.
	LedItDocument&	pDoc = GetDocument ();
	if (pDoc.IsInPlaceActive ()) {
		return FALSE;
	}

	// otherwise, rely on default behavior
	return inherited::CanActivate ();
}

#if		qDebug
LedItDocument&	LedItControlItem::GetDocument () const
{
	LedItDocument*	result	=	(LedItDocument*)COleClientItem::GetDocument();
	Led_EnsureNotNil (result);
	ASSERT_VALID(result);
	ASSERT (result->IsKindOf (RUNTIME_CLASS (LedItDocument)));
	return *result;
}
#endif



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
