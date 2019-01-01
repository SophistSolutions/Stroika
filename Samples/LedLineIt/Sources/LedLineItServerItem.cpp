/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include "LedLineItDocument.h"

#include "LedLineItServerItem.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

/*
 ********************************************************************************
 ******************************** LedLineItServerItem ***************************
 ********************************************************************************
 */
IMPLEMENT_DYNAMIC (LedLineItServerItem, COleServerItem)

LedLineItServerItem::LedLineItServerItem (LedLineItDocument* pContainerDoc)
    : COleServerItem (pContainerDoc, TRUE)
{
}

LedLineItDocument* LedLineItServerItem::GetDocument () const
{
    return (LedLineItDocument*)COleServerItem::GetDocument ();
}

void LedLineItServerItem::Serialize (CArchive& ar)
{
    // LedLineItServerItem::Serialize will be called by the framework if
    //  the item is copied to the clipboard.  This can happen automatically
    //  through the OLE callback OnGetClipboardData.  A good default for
    //  the embedded item is simply to delegate to the document's Serialize
    //  function.  If you support links, then you will want to serialize
    //  just a portion of the document.
    if (!IsLinkedItem ()) {
        LedLineItDocument* pDoc = GetDocument ();
        ASSERT_VALID (pDoc);
        pDoc->Serialize (ar);
    }
}

BOOL LedLineItServerItem::OnGetExtent (DVASPECT dwDrawAspect, CSize& rSize)
{
    // Most applications, like this one, only handle drawing the content
    //  aspect of the item.  If you wish to support other aspects, such
    //  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
    //  implementation of OnGetExtent should be modified to handle the
    //  additional aspect(s).

    if (dwDrawAspect != DVASPECT_CONTENT) {
        return COleServerItem::OnGetExtent (dwDrawAspect, rSize);
    }

    // LedLineItServerItem::OnGetExtent is called to get the extent in
    //  HIMETRIC units of the entire item.  The default implementation
    //  here simply returns a hard-coded number of units.

    LedLineItDocument* pDoc = GetDocument ();
    ASSERT_VALID (pDoc);

    // TODO: replace this arbitrary size

    rSize = CSize (3000, 3000); // 3000 x 3000 HIMETRIC units

    return TRUE;
}

BOOL LedLineItServerItem::OnDraw (CDC* pDC, CSize& /*rSize*/)
{
    LedLineItDocument* pDoc = GetDocument ();
    ASSERT_VALID (pDoc);

    // TODO: set mapping mode and extent
    //  (The extent is usually the same as the size returned from OnGetExtent)
    pDC->SetMapMode (MM_ANISOTROPIC);
    pDC->SetWindowOrg (0, 0);
    pDC->SetWindowExt (3000, 3000);

    // TODO: add drawing code here.  Optionally, fill in the HIMETRIC extent.
    //  All drawing takes place in the metafile device context (pDC).

    // TODO: also draw embedded LedLineItControlItem objects.

    // The following code draws the first item at an arbitrary position.

    // TODO: remove this code when your real drawing code is complete
#if 0
    LedLineItControlItem* pItem = (LedLineItControlItem*)pDoc->GetNextClientItem(pos);
    if (pItem != NULL) {
        pItem->Draw(pDC, CRect (10, 10, 1010, 1010));
    }
#endif
    return TRUE;
}

#ifdef _DEBUG
void LedLineItServerItem::AssertValid () const
{
    COleServerItem::AssertValid ();
}

void LedLineItServerItem::Dump (CDumpContext& dc) const
{
    COleServerItem::Dump (dc);
}
#endif
