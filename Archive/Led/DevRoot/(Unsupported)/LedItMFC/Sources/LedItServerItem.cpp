/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/LedItServerItem.cpp,v 2.9 1998/10/30 14:56:05 lewis Exp $
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
 *	Revision 2.9  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 2.8  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/07/27  16:01:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.6  1997/06/18  03:42:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1996/12/13  18:03:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.4  1996/12/05  21:18:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/09/01  15:43:56  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1996/06/01  02:46:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1996/01/22  05:42:43  lewis
 *	*** empty log message ***
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
#include	"LedItControlItem.h"

#include	"LedItServerItem.h"





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
