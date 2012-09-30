/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef __LedLineItView_h__
#define __LedLineItView_h__ 1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    "Stroika/Frameworks/Led/Platform/MFC.h"
#include    "Stroika/Frameworks/Led/SimpleTextInteractor.h"
#include    "Stroika/Frameworks/Led/SyntaxColoring.h"

#include    "LedLineItConfig.h"

#include    "Resource.h"



using   namespace   Stroika::Foundation;
using   namespace   Stroika::Frameworks::Led;

#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4250)
#endif




#if     qSupportSyntaxColoring
struct  LedLineItMFCBaseClass : public Led_MFC_X<SimpleTextInteractor>, public StyledTextImager {
protected:
    override    Led_Distance    MeasureSegmentHeight (size_t from, size_t to) const {
        return Led_MFC_X<SimpleTextInteractor>::MeasureSegmentHeight (from, to);
    }
    override    Led_Distance    MeasureSegmentBaseLine (size_t from, size_t to) const {
        return Led_MFC_X<SimpleTextInteractor>::MeasureSegmentBaseLine (from, to);
    }
};
#else
struct  LedLineItMFCBaseClass : public Led_MFC_X<SimpleTextInteractor> {
};
#endif









class   LedLineItDocument;

class   LedLineItView : public LedLineItMFCBaseClass {
private:
    typedef LedLineItMFCBaseClass   inherited;

protected: // create from serialization only
    LedLineItView ();
    DECLARE_DYNCREATE(LedLineItView)

public:
    virtual ~LedLineItView ();

protected:
    override    void    OnInitialUpdate ();

#if     qSupportSyntaxColoring
public:
    nonvirtual  void    ResetSyntaxColoringTable ();
protected:
    override    void    HookLosingTextStore ();
    override    void    HookGainedNewTextStore ();
#endif


#if     qSupportSyntaxColoring
protected:
    override    vector<RunElement>  SummarizeStyleMarkers (size_t from, size_t to) const;
    override    vector<RunElement>  SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const;
#endif

public:
    nonvirtual  size_t  GetCurUserLine () const;
    nonvirtual  void    SetCurUserLine (size_t newCurLine); // OK if bad line given

public:
    override    void    UpdateScrollBars ();

public:
    override    Led_Distance    ComputeMaxHScrollPos () const;
private:
    mutable Led_Distance    fCachedLayoutWidth;

public:
    override    void    OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

protected:
    override    const TabStopList&  GetTabStopList (size_t /*containingPos*/) const;
protected:
    override    void    TabletChangedMetrics ();
private:
    SimpleTabStopList   fTabStopList;

public:
    override    void    DidUpdateText (const UpdateInfo& updateInfo) throw ();

public:
    nonvirtual  LedLineItDocument&      GetDocument () const;

public:
    afx_msg     void    OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

protected:
    override    BOOL    IsSelected (const CObject* pDocItem) const; // support for CView/OLE

public:
    afx_msg void    OnUpdateFontNameChangeCommand (CCmdUI* pCmdUI);
    afx_msg void    OnFontNameChangeCommand (UINT cmdNum);
private:
    Led_Distance    PickOtherFontHeight (Led_Distance origHeight);

public:
    afx_msg     void    OnUpdateFontSizeChangeCommand (CCmdUI* pCmdUI);
    afx_msg     void    OnFontSizeChangeCommand (UINT cmdNum);

public:
    afx_msg     void    OnGotoLineCommand ();
    afx_msg     void    OnShiftLeftCommand ();
    afx_msg     void    OnShiftRightCommand ();
    nonvirtual  void    OnShiftNCommand (bool shiftRight);

#if     qSupportGenRandomCombosCommand
public:
    afx_msg     void    OnGenRandomCombosCommand ();
#endif

protected:
    override    SearchParameters    GetSearchParameters () const;
    override    void                SetSearchParameters (const SearchParameters& sp);

protected:
    afx_msg void    OnSetFocus (CWnd* pOldWnd);
    afx_msg void    OnCancelEditCntr ();
    afx_msg void    OnCancelEditSrvr ();
    afx_msg void    OnSelectAllCommand ();
    afx_msg void    OnChooseFontCommand ();
    DECLARE_MESSAGE_MAP()

#if     qSupportSyntaxColoring
private:
#if     qSupportOnlyMarkersWhichOverlapVisibleRegion
    WindowedSyntaxColoringMarkerOwner*  fSyntaxColoringMarkerOwner;
#else
    SimpleSyntaxColoringMarkerOwner*    fSyntaxColoringMarkerOwner;
#endif
#endif

#ifdef _DEBUG
public:
    override    void    AssertValid () const;
    override    void    Dump (CDumpContext& dc) const;
#endif
};






class   FontDlgWithNoColorNoStyles : public CFontDialog {
public:
    FontDlgWithNoColorNoStyles (LOGFONT* lf);
    override    BOOL    OnInitDialog ();
};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if     !qDebug
inline  LedLineItDocument&  LedLineItView::GetDocument () const
{
    return *(LedLineItDocument*)m_pDocument;
}
#endif


#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (pop)
#endif

#endif  /*__LedLineItView_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

