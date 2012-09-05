/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/LedLineItServerItem.cpp,v 2.7 2002/05/06 21:31:20 lewis Exp $
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
 *	$Log: LedLineItServerItem.cpp,v $
 *	Revision 2.7  2002/05/06 21:31:20  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.6  2001/11/27 00:28:23  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.5  2001/08/30 01:02:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1998/10/30 15:08:09  lewis
 *	lots of little cleanups - using vector instead of Led_Array, mutable instaed
 *	of const cast, new msvc60 warning
 *	stuff, etc.
 *	
 *	Revision 2.3  1997/12/24  04:51:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1997/07/27  16:03:03  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/06/18  03:47:52  lewis
 *	qIncludePrefixFile and cleanups
 *
 *	Revision 2.0  1997/01/10  03:38:30  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"LedLineItDocument.h"

#include	"LedLineItServerItem.h"







/*
 ********************************************************************************
 ******************************** LedLineItServerItem ***************************
 ********************************************************************************
 */
IMPLEMENT_DYNAMIC (LedLineItServerItem, COleServerItem)

LedLineItServerItem::LedLineItServerItem (LedLineItDocument* pContainerDoc) :
	COleServerItem (pContainerDoc, TRUE)
{
}

LedLineItDocument* LedLineItServerItem::GetDocument() const
{
	return (LedLineItDocument*)COleServerItem::GetDocument();
}


void	LedLineItServerItem::Serialize (CArchive& ar)
{
	// LedLineItServerItem::Serialize will be called by the framework if
	//  the item is copied to the clipboard.  This can happen automatically
	//  through the OLE callback OnGetClipboardData.  A good default for
	//  the embedded item is simply to delegate to the document's Serialize
	//  function.  If you support links, then you will want to serialize
	//  just a portion of the document.
	if (!IsLinkedItem()) {
		LedLineItDocument* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL	LedLineItServerItem::OnGetExtent (DVASPECT dwDrawAspect, CSize& rSize)
{
	// Most applications, like this one, only handle drawing the content
	//  aspect of the item.  If you wish to support other aspects, such
	//  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
	//  implementation of OnGetExtent should be modified to handle the
	//  additional aspect(s).

	if (dwDrawAspect != DVASPECT_CONTENT) {
		return COleServerItem::OnGetExtent(dwDrawAspect, rSize);
	}

	// LedLineItServerItem::OnGetExtent is called to get the extent in
	//  HIMETRIC units of the entire item.  The default implementation
	//  here simply returns a hard-coded number of units.

	LedLineItDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: replace this arbitrary size

	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC units

	return TRUE;
}

BOOL	LedLineItServerItem::OnDraw (CDC* pDC, CSize& /*rSize*/)
{
	LedLineItDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: set mapping mode and extent
	//  (The extent is usually the same as the size returned from OnGetExtent)
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

	// TODO: add drawing code here.  Optionally, fill in the HIMETRIC extent.
	//  All drawing takes place in the metafile device context (pDC).

	// TODO: also draw embedded LedLineItControlItem objects.

	// The following code draws the first item at an arbitrary position.

	// TODO: remove this code when your real drawing code is complete

	POSITION pos = pDoc->GetStartPosition();
#if 0
	LedLineItControlItem* pItem = (LedLineItControlItem*)pDoc->GetNextClientItem(pos);
	if (pItem != NULL) {
		pItem->Draw(pDC, CRect (10, 10, 1010, 1010));
	}
#endif
	return TRUE;
}

#ifdef _DEBUG
void	LedLineItServerItem::AssertValid () const
{
	COleServerItem::AssertValid();
}

void	LedLineItServerItem::Dump (CDumpContext& dc) const
{
	COleServerItem::Dump(dc);
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
