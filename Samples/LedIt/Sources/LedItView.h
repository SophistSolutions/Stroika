/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#ifndef __LedItView_h__
#define __LedItView_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_MacOS
#include "Stroika/Frameworks/Led/Platform/Led_PP_WordProcessor.h"
#elif defined(WIN32)
#include "Stroika/Frameworks/Led/Platform/MFC_WordProcessor.h"
#elif qXWindows
#include "Stroika/Frameworks/Led/Platform/Led_Gtk_WordProcessor.h"
#endif

#include "LedItConfig.h"
#include "LedItResources.h"

#if qPlatform_Windows
class LedItControlItem;
class LedItDocument;
#endif
#if qXWindows
class LedItDocument;
#endif

#if qPlatform_Windows
using LedItViewAlmostBASE = Platform::Led_MFC_X<WordProcessor>;
#endif

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif
class LedItView :
#if qPlatform_MacOS
    public Platform::WordProcessorCommonCommandHelper_PP<Led_PPView_X<WordProcessor>>
#elif qPlatform_Windows
    public Platform::WordProcessorCommonCommandHelper_MFC<LedItViewAlmostBASE>
#elif qXWindows
    public Platform::WordProcessorCommonCommandHelper_Gtk<Led_Gtk_Helper<WordProcessor>>
#endif
{
private:
#if qPlatform_MacOS
    using inherited = Platform::WordProcessorCommonCommandHelper_PP<Led_PPView_X<WordProcessor>>;
#elif qPlatform_Windows
    using inherited = Platform::WordProcessorCommonCommandHelper_MFC<LedItViewAlmostBASE>;
#elif qXWindows
    using inherited = Platform::WordProcessorCommonCommandHelper_Gtk<Led_Gtk_Helper<WordProcessor>>;
#endif

#if qPlatform_MacOS
public:
    LedItView ();
#elif qPlatform_Windows
protected: // create from serialization only
    LedItView ();
    DECLARE_DYNCREATE (LedItView)
#elif qXWindows
public:
    LedItView (LedItDocument* owningDoc);
#endif

public:
    virtual ~LedItView ();

#if qPlatform_Windows
protected:
    virtual void OnInitialUpdate () override;
#endif

#if qPlatform_Windows
public:
    virtual bool OnUpdateCommand (CommandUpdater* enabler) override;
#endif

public:
    nonvirtual void SetWrapToWindow (bool wrapToWindow);

private:
    bool fWrapToWindow;

public:
    virtual void GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const override;
    virtual void SetWindowRect (const Led_Rect& windowRect) override;

#if qPlatform_Windows
public:
    nonvirtual LedItDocument& GetDocument () const;
#endif

#if qPlatform_Windows
    nonvirtual LedItControlItem* GetSoleSelectedOLEEmbedding () const;
#endif

#if qPlatform_MacOS
public:
    virtual void FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName) override;
#endif

#if qPlatform_Windows
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

#if qPlatform_Windows
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
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if !qDebug && qPlatform_Windows
inline LedItDocument& LedItView::GetDocument () const
{
    return *(LedItDocument*)m_pDocument;
}
#endif

#endif /*__LedItView_h__*/
