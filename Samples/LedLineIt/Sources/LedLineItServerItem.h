/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __LedLineItServerItem_h__
#define __LedLineItServerItem_h__ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 5054)
#endif
#include <afxole.h>
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

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
