/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef __LedLineItServerItem_h__
#define __LedLineItServerItem_h__ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

DISABLE_COMPILER_MSC_WARNING_START (5054)
#include <afxole.h>
DISABLE_COMPILER_MSC_WARNING_END (5054)

#include "Stroika/Frameworks/Led/Support.h"

#include "LedLineItConfig.h"

class LedLineItDocument;

class LedLineItServerItem : public COleServerItem {
public:
    LedLineItServerItem (LedLineItDocument* pContainerDoc);

public:
    nonvirtual LedLineItDocument* GetDocument () const;

public:
    virtual BOOL OnDraw (CDC* pDC, CSize& rSize) override;
    virtual BOOL OnGetExtent (DVASPECT dwDrawAspect, CSize& rSize) override;

protected:
    virtual void Serialize (CArchive& ar); // overridden for document i/o

#ifdef _DEBUG
public:
    virtual void AssertValid () const;
    virtual void Dump (CDumpContext& dc) const;
#endif

private:
    DECLARE_DYNAMIC (LedLineItServerItem)
};

#endif /*__LedLineItServerItem_h__*/
