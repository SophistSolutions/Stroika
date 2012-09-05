/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Sources/LedItControlItem.cpp,v 1.4 2002/05/06 21:31:05 lewis Exp $
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
 *	Revision 1.4  2002/05/06 21:31:05  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:15  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:47  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
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
