/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#ifndef __LedItView_h__
#define __LedItView_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if defined(WIN32)
#include "Stroika/Frameworks/Led/Platform/MFC_WordProcessor.h"
#endif

#include "LedItConfig.h"
#include "LedItResources.h"

#if qStroika_Platform_Windows
class LedItControlItem;
class LedItDocument;
#endif

#if qStroika_Platform_Windows
using LedItViewAlmostBASE = Platform::Led_MFC_X<WordProcessor>;
#endif

DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
class LedItView :
#if qStroika_Platform_Windows
    public Platform::WordProcessorCommonCommandHelper_MFC<LedItViewAlmostBASE>
#endif
{
private:
#if qStroika_Platform_Windows
    using inherited = Platform::WordProcessorCommonCommandHelper_MFC<LedItViewAlmostBASE>;
#endif

#if qStroika_Platform_Windows
protected: // create from serialization only
    LedItView ();
    DECLARE_DYNCREATE (LedItView)
#endif

public:
    virtual ~LedItView ();

#if qStroika_Platform_Windows
protected:
    virtual void OnInitialUpdate () override;
#endif

#if qStroika_Platform_Windows
public:
    virtual bool OnUpdateCommand (CommandUpdater* enabler) override;
#endif

public:
    nonvirtual void SetWrapToWindow (bool wrapToWindow);

private:
    bool fWrapToWindow;

public:
    virtual void GetLayoutMargins (RowReference row, CoordinateType* lhs, CoordinateType* rhs) const override;
    virtual void SetWindowRect (const Led_Rect& windowRect) override;

#if qStroika_Platform_Windows
public:
    nonvirtual LedItDocument& GetDocument () const;
#endif

#if qStroika_Platform_Windows
    nonvirtual LedItControlItem* GetSoleSelectedOLEEmbedding () const;
#endif

#if qStroika_Platform_Windows
public:
    afx_msg void OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

protected:
    virtual BOOL IsSelected (const CObject* pDocItem) const override; // support for CView/OLE
#endif

public:
    nonvirtual IncrementalParagraphInfo GetParaFormatSelection ();
    nonvirtual void                     SetParaFormatSelection (const IncrementalParagraphInfo& pf);

public:
    virtual void OnShowHideGlyphCommand (CommandNumber cmdNum) override;

protected:
    virtual SearchParameters GetSearchParameters () const override;
    virtual void             SetSearchParameters (const SearchParameters& sp) override;

public:
    nonvirtual void SetShowHiddenText (bool showHiddenText);

#if qStroika_Platform_Windows
protected:
    afx_msg void OnSetFocus (CWnd* pOldWnd);
    afx_msg void OnSize (UINT nType, int cx, int cy);
    afx_msg void OnInsertObject ();
    afx_msg void OnCancelEditCntr ();
    afx_msg void OnCancelEditSrvr ();
    afx_msg void OnBarReturn (NMHDR*, LRESULT*);
    DECLARE_MESSAGE_MAP ()

#ifdef _DEBUG
public:
    virtual void AssertValid () const override;
    virtual void Dump (CDumpContext& dc) const override;
#endif
#endif
};
DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if !qStroika_Foundation_Debug_AssertionsChecked && qStroika_Platform_Windows
inline LedItDocument& LedItView::GetDocument () const
{
    return *(LedItDocument*)m_pDocument;
}
#endif

#endif /*__LedItView_h__*/
