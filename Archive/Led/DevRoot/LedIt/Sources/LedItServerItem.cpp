/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Sources/LedItServerItem.cpp,v 1.5 2003/03/21 15:01:18 lewis Exp $
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
 *	$Log: LedItServerItem.cpp,v $
 *	Revision 1.5  2003/03/21 15:01:18  lewis
 *	SPR#1368 - added CRTDBG_MAP_ALLOC_NEW support
 *	
 *	Revision 1.4  2002/05/06 21:31:07  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:15  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:19  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:48  lewis
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
#include	"LedItControlItem.h"

#include	"LedItServerItem.h"


#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




/*
 ********************************************************************************
 ******************************** LedItServerItem *******************************
 ********************************************************************************
 */
IMPLEMENT_DYNAMIC (LedItServerItem, COleServerItem)

LedItServerItem::LedItServerItem (LedItDocument* pContainerDoc) :
	COleServerItem (pContainerDoc, TRUE)
{
	// TODO: add one-time construction code here
	//  (eg, adding additional clipboard formats to the item's data source)
}

LedItServerItem::~LedItServerItem ()
{
}

void	LedItServerItem::Serialize (CArchive& ar)
{
	// LedItServerItem::Serialize will be called by the framework if
	//  the item is copied to the clipboard.  This can happen automatically
	//  through the OLE callback OnGetClipboardData.  A good default for
	//  the embedded item is simply to delegate to the document's Serialize
	//  function.  If you support links, then you will want to serialize
	//  just a portion of the document.
	if (!IsLinkedItem()) {
		LedItDocument* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL	LedItServerItem::OnGetExtent (DVASPECT dwDrawAspect, CSize& rSize)
{
	// Most applications, like this one, only handle drawing the content
	//  aspect of the item.  If you wish to support other aspects, such
	//  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
	//  implementation of OnGetExtent should be modified to handle the
	//  additional aspect(s).

	if (dwDrawAspect != DVASPECT_CONTENT) {
		return COleServerItem::OnGetExtent(dwDrawAspect, rSize);
	}

	// LedItServerItem::OnGetExtent is called to get the extent in
	//  HIMETRIC units of the entire item.  The default implementation
	//  here simply returns a hard-coded number of units.

	LedItDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: replace this arbitrary size

	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC units

	return TRUE;
}

BOOL	LedItServerItem::OnDraw (CDC* pDC, CSize& /*rSize*/)
{
	LedItDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: set mapping mode and extent
	//  (The extent is usually the same as the size returned from OnGetExtent)
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

	// TODO: add drawing code here.  Optionally, fill in the HIMETRIC extent.
	//  All drawing takes place in the metafile device context (pDC).

	// TODO: also draw embedded LedItControlItem objects.

	// The following code draws the first item at an arbitrary position.

	// TODO: remove this code when your real drawing code is complete

	POSITION pos = pDoc->GetStartPosition();
	LedItControlItem* pItem = (LedItControlItem*)pDoc->GetNextClientItem(pos);
	if (pItem != NULL) {
		pItem->Draw(pDC, CRect (10, 10, 1010, 1010));
	}
	return TRUE;
}

#ifdef _DEBUG
void	LedItServerItem::AssertValid () const
{
	COleServerItem::AssertValid();
}

void	LedItServerItem::Dump (CDumpContext& dc) const
{
	COleServerItem::Dump(dc);
}
#endif




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
