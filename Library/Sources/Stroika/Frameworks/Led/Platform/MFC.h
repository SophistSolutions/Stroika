/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_MFC_h_
#define _Stroika_Frameworks_Led_Platform_MFC_h_ 1

#include "../../StroikaPreComp.h"

/*
@MODULE:    Led_MFC
@DESCRIPTION:
        <p>Led MFC specific wrappers.</p>
        <p>Note that since MFC provides no abstract base classes for implementing editor-like
    behavior, Led cannot subclass from CEdit or CEditView.</p>
        <p>Also - for some purposes, it would be better to inherit from CWnd. And for others, better to inherit
    from CView. Versions of Led prior to Led 2.3 inherited from CView.</p>
        <p>Now - through the magic of templates - Led supports (almost) any MFC base class (at least
    CView and CWnd) for a Led-based editor, so long as it supports at least the functionality of CWnd/CView.</p>
        <p>The current Led sample applications (LedIt! and LedLineIt! all use Led inheriting from
    a CView, and that is necessary to get the OLE/embedding funcitonality built in. But Led has also been
    tested as a replacemnt edit control inheriting from CWnd (@LEC).</p>
 */

static_assert (qHasFeature_ATLMFC, "Error: Stroika::Framework::Led::Platform MFC code requires the ATLMFC feature to be set true");

#pragma warning(push)
#pragma warning(disable : 4459)
#pragma warning(disable : 5054)
#include <afxole.h>
#pragma warning(pop)

#include <afxwin.h>
#include <oleidl.h>

#include "../IdleManager.h"
#include "../TextInteractor.h"

#include "Windows.h"

namespace Stroika::Frameworks::Led::Platform {

/*
 **************** Windows Specific configuration variables **************
 */

/*
    @CONFIGVAR:     qLedAssertsDefaultToMFCAsserts
    @DESCRIPTION:   <p>By default when using MFC, make the Led asserts fall-thru into the default
        MFC assertion code. Some customers have indicated they prefer this (spr#0424).</p>
            <p>Originally I made this default to TRUE, as the SPR requested. But I personally
        find the MFC Assert stuff a major pain in the donkey. Whenever you get an assert
        due to a display bug, it brings up a dialog, and dimissing that alert triggers
        (due to extra redisplay) extra re-enters into the same code which had the assert
        doing displays.</p>
            <p>Since it was requested, I'll continue to make it easy for others to turn this on.
        But I leave it off by default.</p>
        */
#ifndef qLedAssertsDefaultToMFCAsserts
#define qLedAssertsDefaultToMFCAsserts 0
#endif

/*
    @CONFIGVAR:     qSupportDrawTextGetTextExtent
    @DESCRIPTION:
            <p>Turn OFF by default (because requires including SimpleTextStore and SimpleTextImager and WordWrappedTextImager
        in your projects - which you may not want to do).
            See @'Led_DrawText' and @'Led_GetTextExtent'.</p>
        */
#ifndef qSupportDrawTextGetTextExtent
#define qSupportDrawTextGetTextExtent 0
#endif

/*
    @CONFIGVAR:     qGenerateStandardMFCExceptions
    @DESCRIPTION:
            <p>MFC apps generally throw AfxMemoryException () etc for internal exceptions. And there are standard MFC catchers for
        these types. To make Led utilize the @'Led_Set_OutOfMemoryException_Handler' etc mechanism to use the MFC exception types -
        define this to TRUE. To use more standard Standard C++ or your own types - you may wish to shut this off.</p>
            <p>Defaults to ON.</p>
        */
#ifndef qGenerateStandardMFCExceptions
#define qGenerateStandardMFCExceptions 1
#endif

/*
    @CONFIGVAR:     qProvideLedStubsForOLEACCDLL
    @DESCRIPTION:
            <p>Windows 98 has a problem with OLEACC.DLL. Though it is <em>supposed</em> to be included with Win98-
        people somehow frequently come up with systems that don't have it, or have it corrupted.
            (for example, see MSFT knowledge base article
                http://support.microsoft.com/default.aspx?scid=kb%3Ben-us%3B810684
            )</p>
            <p>As of version 7 of MFC (with Visual Studio.Net), Microsoft added virtual functions that refer to
        this DLL. The virtual function CWnd::EnsureStdObj calls CreateStdAccessibleObject (from OLEACC.DLL),
        the virtual function CWnd::CreateAccessibleProxy calls LresultFromObject (from OLEACC.DLL),
        and CWnd::GetAccessibleChild () which calls AccessibleObjectFromWindow (from OLEACC.DLL). This creates
        a dependency of OLEACC.DLL which is unfortunate for applications that link against MFC and really don't need
        those functions.
            </p>
            <p>We provide a workaround for this problem by providing our own - friendlier stub routines for these
        two OLEACC.DLL functions.</p>
            <p>Defaulted to ON if _MFC_VER>=0x0700.</p>
            <p>NOW Defaults off, since its hard to believe this would still exist</p>
        */
#ifndef qProvideLedStubsForOLEACCDLL
#define qProvideLedStubsForOLEACCDLL 0
#endif

/*
        **************** MFC bug workaround defines **************
        */

// Still broken In MSVC 6.0 (LGP 990304) - CFrameWnd::CreateView() still does cast of void* to CWnd* which breaks unless CWnd* is leftmost
// Still broken In MSVC 5.0 (LGP 970318) - now the bug is in CFrameWnd::CreateView() though...
// Still broken In MSVC 4.0 (LGP 951125)
// (only not broken for old LedTestMFC app - breaks in new LedIt! app however. Maybe
// should re-investigate???)
//  LGP 941129 (updated 950506)
//  The bug is with MFC 3.0/3.1, in CFrameWnd::OnCreateClient()
//  It did a cast of the result of CreateObject() to CWnd*, without
//  knowing the real type it started with, so that code looses if
//  CWnd is not the left-most base class.
#ifndef qMFCRequiresCWndLeftmostBaseClass
#define qMFCRequiresCWndLeftmostBaseClass 1
#endif

    CPoint    AsCPoint (Led_Point p);
    Led_Point AsLedPoint (CPoint p);

    CRect    AsCRect (Led_Rect r);
    Led_Rect AsLedRect (CRect r);

    CSize AsCSize (Led_Size s);

    /*
    @CLASS:         Led_MFC_TmpCDCFromTablet
    @DESCRIPTION:   <p>Helper class to convert a @'Led_Tablet' to an MFC CDC. Just creates a temporary wrapper.</p>
        <p><strong>Caution</strong>:
        You must create a NAMED temporary.
        A named temporary has lifetime til the end of the enclosing scope.
        An unnamed temporary has lifetime only til the next sequence point (less or equal to the rest of the current statement - I believe).</p>
    */
    class Led_MFC_CDCFromTablet {
    public:
        Led_MFC_CDCFromTablet (Led_Tablet t);
        ~Led_MFC_CDCFromTablet ();
        operator CDC* ();

    private:
        CDC fCDC;
    };

    /*
    @CLASS:         Led_MFC_TabletFromCDC
    @DESCRIPTION:   <p>Helper class to convert an MFC CDC to a @'Led_Tablet'. Just creates a temporary wrapper.</p>
        <p><strong>Caution</strong>:
        You must create a NAMED temporary.
        A named temporary has lifetime til the end of the enclosing scope.
        An unnamed temporary has lifetime only til the next sequence point (less or equal to the rest of the current statement - I believe).</p>
    */
    class Led_MFC_TabletFromCDC {
    public:
        Led_MFC_TabletFromCDC (CDC* pDC);
        ~Led_MFC_TabletFromCDC ();

    public:
        operator Led_Tablet ();
        Led_Tablet operator-> ();

    private:
        Led_Tablet_ fTablet;
    };

    /*
    @CLASS:         Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>
    @BASES:         MFC_BASE_CLASS=@'CWnd', @'Led_Win32_Helper<BASE_INTERACTOR>', BASE_INTERACTOR=@'TextInteractor'
    @DESCRIPTION:   <p>There are two obvious ways to hook into MFC. One is to create a subclass of CWnd. The other is
        to create a subclass of CView. Because CView inherits from CWnd, but not virtually, you are strongly encouraged
        to pick one or the other. Alas - neither is really a good choice for ALL applications. For a big, main view, associated
        with a document, you want to subclass from CView. For a little control (as in a dialog), you want to subclass from CWnd.</p>
            <p>This template makes it easier (soon easy, but for now, just easier) to subclass from either one. The default one, and
        the one Led mainly uses is CView. But soon I'll support (better) subclassing from CWnd.</p>
    */
    template <typename MFC_BASE_CLASS = CWnd, typename BASE_INTERACTOR = TextInteractor>
    class Led_MFC_Helper :
#if qMFCRequiresCWndLeftmostBaseClass
        public MFC_BASE_CLASS,
        public Led_Win32_Helper<BASE_INTERACTOR>
#else
        public Led_Win32_Helper<BASE_INTERACTOR>,
        public MFC_BASE_CLASS
#endif
    {
    private:
        using inherited = void*; // avoid using this keyword since this is mixin of two classes
    private:
        using LED_WIN32_HELPER = Led_Win32_Helper<BASE_INTERACTOR>;

    public:
        using UpdateInfo = MarkerOwner::UpdateInfo;

    protected:
        Led_MFC_Helper ();

    public:
        virtual ~Led_MFC_Helper ();

    public:
        nonvirtual BOOL ReplaceWindow (HWND hWnd);

        // Led_Win32_Helper Hooks
    public:
        virtual HWND GetHWND () const override;

        // Window message hooks NEEDED to get basic editor functionality
    protected:
        afx_msg int  OnCreate (LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnPaint ();
        afx_msg BOOL OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
        afx_msg UINT OnGetDlgCode ();
        afx_msg void OnChar (UINT nChar, UINT nRepCnt, UINT /*nFlags*/);
        afx_msg LRESULT OnUniChar (WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnIMEChar (WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnIME_COMPOSITION (WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnIME_ENDCOMPOSITION (WPARAM wParam, LPARAM lParam);
        afx_msg void    OnKeyDown (UINT nChar, UINT nRepCnt, UINT /*nFlags*/);
        afx_msg void    OnMouseMove (UINT /*nFlags*/, CPoint oPoint);
        afx_msg void    OnLButtonDown (UINT nFlags, CPoint oPoint);
        afx_msg void    OnLButtonUp (UINT /*nFlags*/, CPoint oPoint);
        afx_msg void    OnLButtonDblClk (UINT /*nFlags*/, CPoint oPoint);
        afx_msg void    OnSetFocus (CWnd* pOldWnd);
        afx_msg void    OnKillFocus (CWnd* pNewWnd);
        afx_msg void    OnSize (UINT nType, int cx, int cy);
        afx_msg BOOL OnEraseBkgnd (CDC* pDC);
        afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg BOOL OnMouseWheel (UINT fFlags, short zDelta, CPoint point);
        afx_msg void OnEnable (BOOL bEnable);
        afx_msg void OnTimer (UINT_PTR nEventID);

        // Disambiguate mixins
    public:
        virtual Led_Rect GetWindowRect () const;
        nonvirtual void  GetWindowRect (LPRECT lpRect) const;

    public:
        nonvirtual DWORD GetStyle () const;

        // Command handing - map MFC commands to Led/TextInteractor commands
    public:
        afx_msg void OnUpdateCommand_MSG (CCmdUI* pCmdUI);
        afx_msg void OnPerformCommand_MSG (UINT commandNumber);

    protected:
        DECLARE_MESSAGE_MAP ()
    };

    /*
    @CLASS:         Led_MFC_MimicMFCAPIHelper<BASECLASS>
    @BASES:         BASECLASS = @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
    @DESCRIPTION:   <p>Mimicry of the MFC CEdit methods - to facilitate using this class in place of any
        existing CEdit usage. These methods simply do send-message calls just as their
        CEdit counterparts. Then messages are then trapped in the OnMsgXXX handlers.</p>
            <p>NB: Use of this class almost ALWAYS requires also mixing in the template
        @'Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>'. since that is what (by default) implements
        the OnMsgXXX helpers.</p>
    */
    template <typename BASECLASS = Led_MFC_Helper<>>
    class Led_MFC_MimicMFCAPIHelper : public BASECLASS {
    protected:
        Led_MFC_MimicMFCAPIHelper () = default;

    public:
        nonvirtual BOOL Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
        nonvirtual BOOL CanUndo () const;
        nonvirtual int  GetLineCount_CEdit () const; // HAD to rename cuz of conflict...
        nonvirtual BOOL GetModify () const;
        nonvirtual void SetModify (BOOL bModified = TRUE);
        nonvirtual void GetRect (LPRECT lpRect) const;
        nonvirtual DWORD GetSel () const;
        nonvirtual void  GetSel (int& nStartChar, int& nEndChar) const;
        nonvirtual int   GetLine (int nIndex, LPTSTR lpszBuffer) const;
        nonvirtual int   GetLine (int nIndex, LPTSTR lpszBuffer, int nMaxLength) const;
        nonvirtual void  EmptyUndoBuffer ();
        nonvirtual BOOL FmtLines (BOOL bAddEOL);
        nonvirtual void LimitText (int nChars = 0);
        nonvirtual int  LineFromChar (int nIndex = -1) const;
        nonvirtual int  LineIndex (int nLine = -1) const;
        nonvirtual int  LineLength (int nLine = -1) const;
        nonvirtual void LineScroll (int nLines, int nChars = 0);
        nonvirtual void ReplaceSel (LPCTSTR lpszNewText);
        nonvirtual void SetPasswordChar (TCHAR ch);
        nonvirtual void SetRect (LPCRECT lpRect);
        nonvirtual void SetRectNP (LPCRECT lpRect);
        nonvirtual void SetSel (DWORD dwSelection, BOOL bNoScroll = FALSE);
        nonvirtual void SetSel (int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
        nonvirtual BOOL SetTabStops (int nTabStops, LPINT rgTabStops);
        nonvirtual void SetTabStops ();
        nonvirtual BOOL SetTabStops (const int& cxEachStop); // takes an 'int'
        nonvirtual BOOL Undo ();
        nonvirtual void Clear ();
        nonvirtual void Copy ();
        nonvirtual void Cut ();
        nonvirtual void Paste ();
        nonvirtual BOOL SetReadOnly (BOOL bReadOnly = TRUE);
        nonvirtual int  GetFirstVisibleLine () const;
        nonvirtual TCHAR GetPasswordChar () const;
    };

    /*
    @CLASS:         Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>
    @BASES:         BASECLASS = @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
    @DESCRIPTION:   <p>Mimicry of the starndard Win32 messages sent to an edit control. We cannot
        mimic ALL the messages. Some just don't make sense (like GETHANDLE). But for those that do, we
        do our best.</p>
    */
    template <typename BASECLASS = Led_MFC_Helper<>>
    class Led_MFC_OptionalWin32SDKMessageMimicHelper : public Led_Win32_Win32SDKMessageMimicHelper<BASECLASS> {
    private:
        using inherited = Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>;

    protected:
        Led_MFC_OptionalWin32SDKMessageMimicHelper ();

    public:
        template <int MESSAGE_NUMBER>
        afx_msg LRESULT OnMFCSDKMessageDispatcher (WPARAM wParam, LPARAM lParam)
        {
            LRESULT result = 0;
            Verify (HandleMessage (MESSAGE_NUMBER, wParam, lParam, &result));
            return result;
        }

    protected:
        DECLARE_MESSAGE_MAP ()
    };

    /*
    @CLASS:         Led_MFC_DragAndDropWindow<BASECLASS>
    @BASES:         BASECLASS
    @DESCRIPTION:   <p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CWnd.</p>
            <p>Probably eventually migrate more stuff from @'Led_MFC_CViewHelper<BASECLASS>' here to this class. But for now,
        I'm risk averse. I don't want to break the class library. And this is all that is needed to move out to meet the immediate
        complaints.</p>
    */
    template <typename BASECLASS = Led_MFC_Helper<>>
    class Led_MFC_DragAndDropWindow : public BASECLASS {
    private:
        using inherited = BASECLASS;

    protected:
        Led_MFC_DragAndDropWindow ();

    public:
        ~Led_MFC_DragAndDropWindow ();

    public:
        /*
        @CLASS:         Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames
        @DESCRIPTION:   <p>Command names for each of the user-visible commands produced by the @'TextInteractor' module.
                    This name is used used in the constructed Undo command name, as
                    in, "Undo Drag and Drop". You can replace this name with whatever you like.
                    You change this value with @'TextInteractor::SetCommandNames'.</p>
                        <p> The point of this is to allow for different UI-language localizations,
                    without having to change Led itself.</p>
                        <p>See also @'Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames'.</p>
                        <p>See also @'TextInteractor::CommandNames'.</p>
                        <p>Note - also - this class must be declared lexically inside the
                    outer template -and not outside - do to compiler bugs with MSVC60(SP4).
                    Didn't take the time to trace them down carefully - LGP 2000-08-19.</p>
        */
        struct CommandNames {
            CommandNames ()
                : fDragCommandName (Led_SDK_TCHAROF ("Drag"))
                , fDropCommandName (Led_SDK_TCHAROF ("Drop"))
                , fDragNDropCommandName (Led_SDK_TCHAROF ("Drag and Drop"))
            {
            }

            Led_SDK_String fDragCommandName;
            Led_SDK_String fDropCommandName;
            Led_SDK_String fDragNDropCommandName;
        };

        // This class builds commands with command names. The UI may wish to change these
        // names (eg. to customize for particular languages, etc)
        // Just patch these strings here, and commands will be created with these names.
        // (These names appear in text of undo menu item)
    public:
        static const CommandNames& GetCommandNames ();
        static void                SetCommandNames (const CommandNames& cmdNames);

    private:
        static CommandNames sCommandNames;

    protected:
        COleDropTarget fDropTarget;

    protected:
        afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);

        // mouse down tracking... and drag and drop...
    protected:
        virtual bool    IsADragSelect (Led_Point clickedWhere) const;
        nonvirtual void HandleDragSelect (UINT nFlags, CPoint oPoint);
        nonvirtual void HandleSharedDragOverUpdateSelCode ();
        nonvirtual DROPEFFECT HelperDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
        nonvirtual DROPEFFECT HelperDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
        nonvirtual BOOL HelperDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
        nonvirtual void HelperDragLeave ();

    protected:
        CPoint     fDragPoint;  // current position
        CSize      fDragSize;   // size of dragged object
        CSize      fDragOffset; // offset to focus rect
        DROPEFFECT fPrevDropEffect;

    protected:
        nonvirtual bool GetObjectInfo (COleDataObject* pDataObject, CSize* pSize, CSize* pOffset);

    protected:
        afx_msg void OnTimer (UINT_PTR nEventID);

    private:
        nonvirtual void StartDragAutoscrollTimer ();
        nonvirtual void StopDragAutoscrollTimer ();
        enum { eAutoscrolling4DragTimerEventID = 435 }; // Magic#
        UINT_PTR fDragAutoScrollTimerID;                // zero means no timer

    private:
        struct LedStartDragAndDropContext {
        public:
            LedStartDragAndDropContext (Led_MFC_DragAndDropWindow<BASECLASS>* ledMFCView)
                : fOurDrag (false)
                , fOurDragStart (0)
                , fOurDragEnd (0)
                , fWeRecievedDrop (false)
                , fLedMFCView (ledMFCView)
                , fOrigSelection (ledMFCView->GetTextStore (), ledMFCView->GetSelectionStart (), ledMFCView->GetSelectionEnd ())
            {
            }

        public:
            bool                                  fOurDrag;
            size_t                                fOurDragStart;
            size_t                                fOurDragEnd;
            bool                                  fWeRecievedDrop;
            Led_MFC_DragAndDropWindow<BASECLASS>* fLedMFCView;
            TempMarker                            fOrigSelection;
        };

        static LedStartDragAndDropContext* sCurrentDragInfo;

    protected:
        DECLARE_MESSAGE_MAP ()
    };

    /*
    @CLASS:         Led_MFC_CViewHelper<BASECLASS>
    @BASES:         BASECLASS
    @DESCRIPTION:   <p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CView.</p>
    */
    template <typename BASECLASS = Led_MFC_DragAndDropWindow<>>
    class Led_MFC_CViewHelper : public BASECLASS {
    private:
        using inherited = BASECLASS;

    protected:
        Led_MFC_CViewHelper ();

    public:
        virtual ~Led_MFC_CViewHelper ();

        // CEditView storage methods.
    public:
        virtual void    DeleteContents ();
        virtual void    Serialize (CArchive& ar);
        nonvirtual void SerializeRaw (CArchive& ar);
        nonvirtual void ReadFromArchive (CArchive& ar, UINT nLen);
        nonvirtual void WriteToArchive (CArchive& ar);

    protected:
        afx_msg void OnPaint ();
        virtual void OnDraw (CDC* pDC) override;
        afx_msg void OnLButtonDown (UINT nFlags, CPoint oPoint);
        afx_msg int  OnCreate (LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    public:
        nonvirtual Led_TWIPS_Rect GetPrintMargins () const;
        nonvirtual void           SetPrintMargins (const Led_TWIPS_Rect& printMargins);

    private:
        Led_TWIPS_Rect fPrintMargins;

    protected:
        virtual BOOL OnPreparePrinting (CPrintInfo* pInfo) override;
        virtual void OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo) override;
        virtual void OnPrint (CDC* pDC, CPrintInfo* pInfo) override;
        virtual void OnEndPrinting (CDC* pDC, CPrintInfo* pInfo) override;

    public:
        virtual void OnPrepareDC (CDC* pDC, CPrintInfo* pInfo) override;

    protected:
        virtual Led_Rect CalculatePrintingRect (CDC* pDC) const;

    protected:
        struct PrintInfo {
            PrintInfo (BASECLASS& editor, CDC* useTablet, Led_Rect oldWindowRect, size_t savedScrollPos, bool savedForceAlLRowsFlag)
                : fTmpTablet (useTablet)
                , fOldUpdateTablet (editor, fTmpTablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall)
                , fOldWindowRect (oldWindowRect)
                , fSavedScrollPos (savedScrollPos)
                , fSavedForceAllRowsFlag (savedForceAlLRowsFlag)
                , fWindowStarts ()
            {
            }
            Led_MFC_TabletFromCDC                    fTmpTablet;
            typename BASECLASS::TemporarilyUseTablet fOldUpdateTablet;
            Led_Rect                                 fOldWindowRect;
            size_t                                   fSavedScrollPos;
            bool                                     fSavedForceAllRowsFlag;
            vector<size_t>                           fWindowStarts;
        };
        PrintInfo* fPrintInfo; // Non-NULL only during printing...

        // mouse down tracking... and drag and drop...
    protected:
        virtual DROPEFFECT OnDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) override;
        virtual DROPEFFECT OnDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) override;
        virtual BOOL       OnDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) override;
        virtual void       OnDragLeave () override;

    public:
        virtual void InvalidateScrollBarParameters () override;

    protected:
        virtual void UpdateScrollBars () override;

    protected:
        DECLARE_MESSAGE_MAP ()
    };

    /*
    @CLASS:         Led_MFC
    @BASES:         @'Led_MFC_CViewHelper<BASECLASS>', where BASECLASS=@'Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>, where BASECLASS=@'Led_MFC_MimicMFCAPIHelper<BASECLASS>', where BASECLASS == @'Led_MFC_Helper<BASECLASS>', where BASECLASS=CView
    @DESCRIPTION:   <p>This template is provided partly as a convenience, for such a common case. But MOSTLY as backwards compatability
        for Led 2.2. In Led 2.2, Led_MFC was the SOLE class provided to integrated Led with MFC. Now there is a whole suite of
        individually selectable templates to provide that interfacing.</p>
    */
    using Led_MFC = Led_MFC_CViewHelper<Led_MFC_DragAndDropWindow<Led_MFC_OptionalWin32SDKMessageMimicHelper<Led_MFC_MimicMFCAPIHelper<Led_MFC_Helper<CView, TextInteractor>>>>>;

    /*
    @CLASS:         Led_MFC_COleControlHelper<BASECLASS>
    @BASES:         BASECLASS
    @DESCRIPTION:   <p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CView.</p>
    */
    template <typename BASECLASS = Led_MFC_DragAndDropWindow<>>
    class Led_MFC_COleControlHelper : public BASECLASS {
    private:
        using inherited = BASECLASS;

    public:
        using UpdateMode = TextInteractor::UpdateMode;

    protected:
        Led_MFC_COleControlHelper () {}

    public:
        virtual ~Led_MFC_COleControlHelper () {}

    public:
        nonvirtual void Refresh (UpdateMode updateMode = TextInteractor::eDefaultUpdate) const
        {
            const TextInteractor* ti = this;
            ti->Refresh (updateMode);
        }
        nonvirtual void Refresh (const Led_Rect& area, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const
        {
            const TextInteractor* ti = this;
            ti->Refresh (area, updateMode);
        }
        nonvirtual void Refresh (size_t from, size_t to, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const
        {
            const TextInteractor* ti = this;
            ti->Refresh (from, to, updateMode);
        }
        nonvirtual void Refresh (const Marker* range, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const
        {
            const TextInteractor* ti = this;
            ti->Refresh (range, updateMode);
        }
    };

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif
    /*
    @CLASS:         Led_MFC_ExceptionHandlerHelper<BASECLASS>
    @BASES:         BASECLASS = @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
    @DESCRIPTION:   <p>This helper is designed to manage exception handling, when the Led class is being used in a context (say OCX)
        where it cannot throw exceptions in error situations.</p>
            <p>This class wraps many messages the control is likely to get, and when there is an exception, it calls a virtual
        exception-handling method (which by default - beeps), and then returns normally.</p>
    */
    template <typename BASECLASS = Led_MFC>
    class Led_MFC_ExceptionHandlerHelper : public BASECLASS {
    private:
        using inherited = BASECLASS;

    protected:
        virtual void HandleException () const;

    public:
        afx_msg void OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg LRESULT OnIMEChar (WPARAM wParam, LPARAM lParam);
        afx_msg void    OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg LRESULT OnMsgPaste (WPARAM wParam, LPARAM lParam);
        afx_msg void    OnLButtonDown (UINT nFlags, CPoint oPoint);
        afx_msg void    OnLButtonUp (UINT /*nFlags*/, CPoint oPoint);
        afx_msg void    OnLButtonDblClk (UINT /*nFlags*/, CPoint oPoint);
        afx_msg BOOL OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

    protected:
        DECLARE_MESSAGE_MAP ()
    };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4250) // inherits via dominance warning
#endif
    /*
    @CLASS:         Led_MFC_X
    @BASES:         ChosenInteractor, @'Led_MFC'
    @DESCRIPTION:   <p>Utility template to mix together Led_MFC an a <code>ChosenInteractor</code> which already has support
        for a particular TextImager mixed in.</p>
    */
    template <typename ChosenInteractor, typename LEDMFC = Led_MFC>
    class Led_MFC_X :
#if qMFCRequiresCWndLeftmostBaseClass
        public LEDMFC,
        public ChosenInteractor
#else
        public ChosenInteractor,
        public LEDMFC
#endif
    {
    public:
        Led_MFC_X ();

    public:
        virtual void AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo) override;
        virtual void DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) noexcept override;
    };
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif

    /*
    @CLASS:         Led_MFCReaderDAndDFlavorPackage
    @BASES:         @'ReaderFlavorPackage'
    @DESCRIPTION:   <p>ReaderFlavorPackage which reads from a COleDataObject as
        its underlying storage. Useful for Drop (Drag and Drop) processing.</p>
    */
    class Led_MFCReaderDAndDFlavorPackage : public ReaderFlavorPackage {
    public:
        Led_MFCReaderDAndDFlavorPackage (COleDataObject* dataObject);

        virtual bool   GetFlavorAvailable (Led_ClipFormat clipFormat) const override;
        virtual size_t GetFlavorSize (Led_ClipFormat clipFormat) const override;
        virtual size_t ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const override;

        nonvirtual COleDataObject* GetOleDataObject () const;

    private:
        COleDataObject* fDataObject;
    };

    /*
    @CLASS:         Led_MFCWriterDAndDFlavorPackage
    @BASES:         @'WriterFlavorPackage'
    @DESCRIPTION:   <p>A WriterFlavorPackage which writes to a COleDataObject as
        its underlying storage. Useful for Drag (Drag and Drop) processing.</p>
    */
    class Led_MFCWriterDAndDFlavorPackage : public WriterFlavorPackage {
    public:
        Led_MFCWriterDAndDFlavorPackage (COleDataSource* dataObject);

        virtual void AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf) override;

        nonvirtual COleDataSource* GetOleDataSource () const;

    private:
        COleDataSource* fDataObject;
    };

    template <typename TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
    CSize Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);

    template <typename TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
    void Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);

#if qSupportDrawTextGetTextExtent
    CSize Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);
    void  Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);
#endif

    /*
    @CLASS:         MFC_CommandNumberMapping
    @DESCRIPTION:   <p></p>
    */
    class MFC_CommandNumberMapping : public CommandNumberMapping<UINT> {
    protected:
        MFC_CommandNumberMapping ();
    };

    /*
    @CLASS:         Led_MFC_TmpCmdUpdater
    @BASES:         @'TextInteractor::CommandUpdater'
    @DESCRIPTION:   <p>Helper used in @'Led_Win32_Helper<BASE_INTERACTOR>'</p>
    */
    class Led_MFC_TmpCmdUpdater : public TextInteractor::CommandUpdater {
    public:
        explicit Led_MFC_TmpCmdUpdater (CCmdUI* pCmdUI);

    public:
        virtual CommandNumber GetCmdID () const override;
        virtual bool          GetEnabled () const override;
        virtual void          SetEnabled (bool enabled) override;
        virtual void          SetChecked (bool checked) override;
        virtual void          SetText (const Led_SDK_Char* text) override;

    public:
        operator CCmdUI* ();
        operator Led_MFC_TmpCmdUpdater* ();

    private:
        CommandNumber fCmdNum;
        CCmdUI*       fCmdUI;
        bool          fEnabled;
    };

/*
    @CLASS:         LED_MFC_HANDLE_COMMAND
    @DESCRIPTION:   <p>Trivial helper for building MFC message maps.</p>
    */
#define LED_MFC_HANDLE_COMMAND(A)                  \
    ON_COMMAND_RANGE (A, A, &OnPerformCommand_MSG) \
    ON_UPDATE_COMMAND_UI (A, &OnUpdateCommand_MSG)

/*
    @CLASS:         LED_MFC_HANDLE_COMMAND_RANGE
    @DESCRIPTION:   <p>Trivial helper for building MFC message maps.</p>
    */
#define LED_MFC_HANDLE_COMMAND_RANGE(A, B)         \
    ON_COMMAND_RANGE (A, B, &OnPerformCommand_MSG) \
    ON_UPDATE_COMMAND_UI_RANGE (A, B, &OnUpdateCommand_MSG)

/*
    @CLASS:         LED_MFC_HANDLE_COMMAND_M
    @DESCRIPTION:   <p></p>
    */
#define LED_MFC_HANDLE_COMMAND_M(A) \
    LED_MFC_HANDLE_COMMAND (MFC_CommandNumberMapping::Get ().ReverseLookup (A))

/*
    @CLASS:         LED_MFC_HANDLE_COMMAND_RANGE_M
    @DESCRIPTION:   <p></p>
    */
#define LED_MFC_HANDLE_COMMAND_RANGE_M(A, B) \
    LED_MFC_HANDLE_COMMAND_RANGE (MFC_CommandNumberMapping::Get ().ReverseLookup (A), MFC_CommandNumberMapping::Get ().ReverseLookup (B))
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "MFC.inl"

#endif /*_Stroika_Frameworks_Led_Platform_MFC_h_*/
