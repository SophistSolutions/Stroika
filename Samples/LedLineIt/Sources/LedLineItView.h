/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef __LedLineItView_h__
#define __LedLineItView_h__ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/Led/Platform/MFC.h"
#include "Stroika/Frameworks/Led/SimpleTextInteractor.h"
#include "Stroika/Frameworks/Led/SyntaxColoring.h"

#include "LedLineItConfig.h"

#include "Resource.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;

DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning

#if qSupportSyntaxColoring
struct LedLineItMFCBaseClass : public Led_MFC_X<SimpleTextInteractor>, public StyledTextImager {
protected:
    virtual Led_Distance MeasureSegmentHeight (size_t from, size_t to) const override
    {
        return Led_MFC_X<SimpleTextInteractor>::MeasureSegmentHeight (from, to);
    }
    virtual Led_Distance MeasureSegmentBaseLine (size_t from, size_t to) const override
    {
        return Led_MFC_X<SimpleTextInteractor>::MeasureSegmentBaseLine (from, to);
    }
};
#else
struct LedLineItMFCBaseClass : public Led_MFC_X<SimpleTextInteractor> {
};
#endif

class LedLineItDocument;

class LedLineItView : public LedLineItMFCBaseClass {
private:
    using inherited = LedLineItMFCBaseClass;

protected: // create from serialization only
    LedLineItView ();
    DECLARE_DYNCREATE (LedLineItView)

public:
    virtual ~LedLineItView ();

protected:
    virtual void OnInitialUpdate () override;

#if qSupportSyntaxColoring
public:
    nonvirtual void ResetSyntaxColoringTable ();

protected:
    virtual void HookLosingTextStore () override;
    virtual void HookGainedNewTextStore () override;
#endif

#if qSupportSyntaxColoring
protected:
    virtual vector<RunElement> SummarizeStyleMarkers (size_t from, size_t to) const override;
    virtual vector<RunElement> SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const override;
#endif

public:
    nonvirtual size_t GetCurUserLine () const;
    nonvirtual void   SetCurUserLine (size_t newCurLine); // OK if bad line given

public:
    virtual void UpdateScrollBars () override;

public:
    virtual Led_Distance ComputeMaxHScrollPos () const override;

private:
    mutable Led_Distance fCachedLayoutWidth;

public:
    virtual void OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed) override;

protected:
    virtual const TabStopList& GetTabStopList (size_t /*containingPos*/) const override;

protected:
    virtual void TabletChangedMetrics () override;

private:
    SimpleTabStopList fTabStopList;

public:
    virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

public:
    nonvirtual LedLineItDocument& GetDocument () const;

public:
    afx_msg void OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

protected:
    virtual BOOL IsSelected (const CObject* pDocItem) const override; // support for CView/OLE

public:
    afx_msg void OnUpdateFontNameChangeCommand (CCmdUI* pCmdUI);
    afx_msg void OnFontNameChangeCommand (UINT cmdNum);

private:
    Led_Distance PickOtherFontHeight (Led_Distance origHeight);

public:
    afx_msg void OnUpdateFontSizeChangeCommand (CCmdUI* pCmdUI);
    afx_msg void OnFontSizeChangeCommand (UINT cmdNum);

public:
    afx_msg void    OnGotoLineCommand ();
    afx_msg void    OnShiftLeftCommand ();
    afx_msg void    OnShiftRightCommand ();
    nonvirtual void OnShiftNCommand (bool shiftRight);

#if qSupportGenRandomCombosCommand
public:
    afx_msg void OnGenRandomCombosCommand ();
#endif

protected:
    virtual SearchParameters GetSearchParameters () const override;
    virtual void             SetSearchParameters (const SearchParameters& sp) override;

protected:
    afx_msg void OnSetFocus (CWnd* pOldWnd);
    afx_msg void OnCancelEditCntr ();
    afx_msg void OnCancelEditSrvr ();
    afx_msg void OnSelectAllCommand ();
    afx_msg void OnChooseFontCommand ();
    DECLARE_MESSAGE_MAP ()

#if qSupportSyntaxColoring
private:
#if qSupportOnlyMarkersWhichOverlapVisibleRegion
    WindowedSyntaxColoringMarkerOwner* fSyntaxColoringMarkerOwner;
#else
    SimpleSyntaxColoringMarkerOwner* fSyntaxColoringMarkerOwner;
#endif
#endif

#ifdef _DEBUG
public:
    virtual void AssertValid () const override;
    virtual void Dump (CDumpContext& dc) const override;
#endif
};
DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

class FontDlgWithNoColorNoStyles : public CFontDialog {
public:
    FontDlgWithNoColorNoStyles (LOGFONT* lf);
    virtual BOOL OnInitDialog () override;
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if !qDebug
inline LedLineItDocument& LedLineItView::GetDocument () const
{
    return *(LedLineItDocument*)m_pDocument;
}
#endif

#endif /*__LedLineItView_h__*/
