/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */

#ifndef __LedItServerItem_h__
#define __LedItServerItem_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

class LedItServerItem : public COleServerItem {
    DECLARE_DYNAMIC (LedItServerItem)

    // Constructors
public:
    LedItServerItem (LedItDocument* pContainerDoc);

    // Attributes
    LedItDocument* GetDocument () const
    {
        return (LedItDocument*)COleServerItem::GetDocument ();
    }

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(LedItServerItem)
public:
    virtual BOOL OnDraw (CDC* pDC, CSize& rSize);
    virtual BOOL OnGetExtent (DVASPECT dwDrawAspect, CSize& rSize);
    //}}AFX_VIRTUAL

    // Implementation
public:
    ~LedItServerItem ();
#ifdef _DEBUG
    virtual void AssertValid () const;
    virtual void Dump (CDumpContext& dc) const;
#endif

protected:
    virtual void Serialize (CArchive& ar); // overridden for document i/o
};

#endif /*__LedItServerItem_h__*/
