/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */

#ifndef __LedItView_h__
#define __LedItView_h__ 1

#include    "Stroika/Foundation/StroikaPreComp.h"

#if     qPlatform_MacOS
#include    "Stroika/Frameworks/Led/Platform/Led_PP_WordProcessor.h"
#elif   defined (WIN32)
#include    "Stroika/Frameworks/Led/Platform/MFC_WordProcessor.h"
#elif   qXWindows
#include    "Stroika/Frameworks/Led/Platform/Led_Gtk_WordProcessor.h"
#endif


#include    "LedItConfig.h"
#include    "LedItResources.h"






#if     qPlatform_Windows
class   LedItControlItem;
class   LedItDocument;
#endif
#if     qXWindows
class   LedItDocument;
#endif



#if     qPlatform_Windows
#if     qTypedefConfusedAccessCheckingCompilerBug
class   LedItViewAlmostBASE : public Platform::Led_MFC_X <WordProcessor> {
};
#else
typedef Platform::Led_MFC_X <WordProcessor> LedItViewAlmostBASE;
#endif
#endif

#if     qQuiteAnnoyingDominanceWarnings
#pragma warning (disable : 4250)
#endif
class   LedItView :
#if     qPlatform_MacOS
    public Platform::WordProcessorCommonCommandHelper_PP<Led_PPView_X<WordProcessor> >
#elif   qPlatform_Windows
    public Platform::WordProcessorCommonCommandHelper_MFC <LedItViewAlmostBASE>
#elif   qXWindows
    public Platform::WordProcessorCommonCommandHelper_Gtk<Led_Gtk_Helper<WordProcessor> >
#endif
{
private:
#if     qPlatform_MacOS
    typedef Platform::WordProcessorCommonCommandHelper_PP<Led_PPView_X<WordProcessor> >     inherited;
#elif   qPlatform_Windows
    typedef Platform::WordProcessorCommonCommandHelper_MFC <LedItViewAlmostBASE>            inherited;
#elif   qXWindows
    typedef Platform::WordProcessorCommonCommandHelper_Gtk<Led_Gtk_Helper<WordProcessor> >  inherited;
#endif

#if     qPlatform_MacOS
public:
    LedItView ();
#elif   qPlatform_Windows
protected: // create from serialization only
    LedItView ();
    DECLARE_DYNCREATE (LedItView)
#elif   qXWindows
public:
    LedItView (LedItDocument* owningDoc);
#endif

public:
    virtual ~LedItView ();

#if     qPlatform_Windows
protected:
    override    void    OnInitialUpdate ();
#endif

#if     qPlatform_Windows
public:
    override    bool    OnUpdateCommand (CommandUpdater* enabler);
#endif

public:
    nonvirtual  void    SetWrapToWindow (bool wrapToWindow);
private:
    bool    fWrapToWindow;

public:
    override    void    GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;
    override    void    SetWindowRect (const Led_Rect& windowRect);

#if     qPlatform_Windows
public:
    nonvirtual  LedItDocument&      GetDocument () const;
#endif

#if     qPlatform_Windows
    nonvirtual  LedItControlItem*                   GetSoleSelectedOLEEmbedding () const;
#endif

#if     qPlatform_MacOS
public:
    override    void    FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName);
#endif

#if     qPlatform_Windows
public:
    afx_msg     void    OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

protected:
    override    BOOL    IsSelected (const CObject* pDocItem) const; // support for CView/OLE
#endif

public:
    nonvirtual      IncrementalParagraphInfo    GetParaFormatSelection ();
    nonvirtual      void                        SetParaFormatSelection (const IncrementalParagraphInfo& pf);

public:
    override    void    OnShowHideGlyphCommand (CommandNumber cmdNum);

protected:
    override    SearchParameters    GetSearchParameters () const;
    override    void                SetSearchParameters (const SearchParameters& sp);

public:
    nonvirtual  void    SetShowHiddenText (bool showHiddenText);

#if     qPlatform_Windows
protected:
    afx_msg void    OnSetFocus (CWnd* pOldWnd);
    afx_msg void    OnSize (UINT nType, int cx, int cy);
    afx_msg void    OnInsertObject ();
    afx_msg void    OnCancelEditCntr ();
    afx_msg void    OnCancelEditSrvr ();
    afx_msg void    OnBarReturn (NMHDR*, LRESULT*);
    DECLARE_MESSAGE_MAP()


#ifdef _DEBUG
public:
    override    void    AssertValid () const;
    override    void    Dump (CDumpContext& dc) const;
#endif
#endif
};
#if     qQuiteAnnoyingDominanceWarnings
#pragma warning (default : 4250)
#endif







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if     !qDebug && qPlatform_Windows
inline  LedItDocument&  LedItView::GetDocument () const
{
    return *(LedItDocument*)m_pDocument;
}
#endif

#endif  /*__LedItView_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

