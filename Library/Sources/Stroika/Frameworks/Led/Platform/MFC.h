/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_MFC_h_
#define _Stroika_Frameworks_Led_Platform_MFC_h_ 1

#include    "../../../Foundation/StroikaPreComp.h"

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

#include    <afxwin.h>
#include    <afxole.h>
#include    <oleidl.h>

#include    "../IdleManager.h"
#include    "../TextInteractor.h"

#include    "Windows.h"




namespace   Stroika {
    namespace   Frameworks {
        namespace   Led {
            namespace   Platform {



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
#define qLedAssertsDefaultToMFCAsserts                  0
#endif





                /*
                @CONFIGVAR:     qSupportDrawTextGetTextExtent
                @DESCRIPTION:
                        <p>Turn OFF by default (because requires including SimpleTextStore and SimpleTextImager and WordWrappedTextImager
                    in your projects - which you may not want to do).
                        See @'Led_DrawText' and @'Led_GetTextExtent'.</p>
                 */
#ifndef qSupportDrawTextGetTextExtent
#define qSupportDrawTextGetTextExtent                       0
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
#define qGenerateStandardMFCExceptions                      1
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
                        <p>Defaults to ON if _MFC_VER>=0x0700.</p>
                 */
#ifndef qProvideLedStubsForOLEACCDLL
#define qProvideLedStubsForOLEACCDLL                        (_MFC_VER>=0x0700)
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
#define qMFCRequiresCWndLeftmostBaseClass                       1
#endif






                CPoint      AsCPoint (Led_Point p);
                Led_Point   AsLedPoint (CPoint p);

                CRect       AsCRect (Led_Rect r);
                Led_Rect    AsLedRect (CRect r);

                CSize       AsCSize (Led_Size s);






                /*
                @CLASS:         Led_MFC_TmpCDCFromTablet
                @DESCRIPTION:   <p>Helper class to convert a @'Led_Tablet' to an MFC CDC. Just creates a temporary wrapper.</p>
                    <p><strong>Caution</strong>:
                    You must create a NAMED temporary.
                    A named temporary has lifetime til the end of the enclosing scope.
                    An unnamed temporary has lifetime only til the next sequence point (less or equal to the rest of the current statement - I believe).</p>
                */
                class   Led_MFC_CDCFromTablet {
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
                class   Led_MFC_TabletFromCDC {
                public:
                    Led_MFC_TabletFromCDC (CDC* pDC);
                    ~Led_MFC_TabletFromCDC ();

                public:
                    operator Led_Tablet ();
                    Led_Tablet operator-> ();

                private:
                    Led_Tablet_ fTablet;
                };




#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4250)
#endif









                /*
                @CLASS:         Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>
                @BASES:         MFC_BASE_CLASS=@'CWnd', @'Led_Win32_Helper<BASE_INTERACTOR>', BASE_INTERACTOR=@'TextInteractor'
                @DESCRIPTION:   <p>There are two obvious ways to hook into MFC. One is to create a subclass of CWnd. The other is
                    to create a subclass of CView. Because CView inherits from CWnd, but not virtually, you are strongly encouraged
                    to pick one or the other. Alas - neither is really a good choice for ALL applications. For a big, main view, associated
                    with a document, you want to subclass from CView. For a little control (as in a dialog), you want to subclass from CWnd.</p>
                        <p>This template makes it easier (soon easy, but for now, just easier) to subclass from either one. The default one, and
                    the one Led mainly uses is CView. But soon I'll support (better) subclassing from CWnd.</p>
                        <p>NB: You must invoke the macro DoDeclare_Led_MFC_Helper_MessageMap(MFC_BASE_CLASS, BASE_INTERACTOR) to
                    generate the code for the message
                    map for this template, unless its already been done so for your particular MFC_BASE_CLASS in Led_MFC.cpp.</p>
                */
                template    <typename   MFC_BASE_CLASS = CWnd, typename BASE_INTERACTOR = TextInteractor>
				class   Led_MFC_Helper :
#if     qMFCRequiresCWndLeftmostBaseClass
                    public MFC_BASE_CLASS, public Led_Win32_Helper<BASE_INTERACTOR>
#else
                    public Led_Win32_Helper<BASE_INTERACTOR>, public MFC_BASE_CLASS
#endif
                {
                private:
                    typedef void*   inherited;  // avoid using this keyword since this is mixin of two classes
                private:
                    typedef Led_Win32_Helper<BASE_INTERACTOR>   LED_WIN32_HELPER;
                public:
                    typedef MarkerOwner::UpdateInfo UpdateInfo;

                protected:
                    Led_MFC_Helper ();

                public:
                    virtual ~Led_MFC_Helper ();

#if     qSupportLed30CompatAPI
                public:
                    nonvirtual  BOOL    SubclassWindow (HWND hWnd);
#endif
                public:
                    nonvirtual  BOOL    ReplaceWindow (HWND hWnd);


                    // Led_Win32_Helper Hooks
                public:
                    override    HWND    GetHWND () const;



                    // Window message hooks NEEDED to get basic editor functionality
                protected:
                    afx_msg int     OnCreate (LPCREATESTRUCT lpCreateStruct);
                    afx_msg void    OnPaint ();
                    afx_msg BOOL    OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
                    afx_msg UINT    OnGetDlgCode ();
                    afx_msg void    OnChar (UINT nChar, UINT nRepCnt, UINT /*nFlags*/);
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
                    afx_msg BOOL    OnEraseBkgnd (CDC* pDC);
                    afx_msg void    OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
                    afx_msg void    OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
                    afx_msg BOOL    OnMouseWheel (UINT fFlags, short zDelta, CPoint point);
                    afx_msg void    OnEnable (BOOL bEnable);
                    afx_msg void    OnTimer (UINT nEventID);


                    // Disambigouate mixins
                public:
                    override    Led_Rect    GetWindowRect () const;
                    nonvirtual  void        GetWindowRect (LPRECT lpRect) const;
                public:
                    nonvirtual  DWORD   GetStyle () const;

                    // Command handing - map MFC commands to Led/TextInteractor commands
                public:
                    afx_msg void    OnUpdateCommand_MSG (CCmdUI* pCmdUI);
                    afx_msg void    OnPerformCommand_MSG (UINT commandNumber);

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
                        <p>NB: You must invoke the macro DoDeclare_Led_MFC_MimicMFCAPIHelper_MessageMap(BASECLASS) somewhere to
                    generate the message map code for this template, unless thats already been done for your particular BASECLASS
                    in Led_MFC.cpp.</p>
                */
                template    <typename   BASECLASS = Led_MFC_Helper<> > 
				class   Led_MFC_MimicMFCAPIHelper :
                    public BASECLASS {
                protected:
                    Led_MFC_MimicMFCAPIHelper ();

                public:
                    nonvirtual  BOOL    Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
                    nonvirtual  BOOL    CanUndo () const;
                    nonvirtual  int     GetLineCount_CEdit () const;        // HAD to rename cuz of conflict...
                    nonvirtual  BOOL    GetModify () const;
                    nonvirtual  void    SetModify (BOOL bModified = TRUE);
                    nonvirtual  void    GetRect (LPRECT lpRect) const;
                    nonvirtual  DWORD   GetSel () const;
                    nonvirtual  void    GetSel (int& nStartChar, int& nEndChar) const;
                    nonvirtual  int     GetLine (int nIndex, LPTSTR lpszBuffer) const;
                    nonvirtual  int     GetLine (int nIndex, LPTSTR lpszBuffer, int nMaxLength) const;
                    nonvirtual  void    EmptyUndoBuffer ();
                    nonvirtual  BOOL    FmtLines (BOOL bAddEOL);
                    nonvirtual  void    LimitText (int nChars = 0);
                    nonvirtual  int     LineFromChar (int nIndex = -1) const;
                    nonvirtual  int     LineIndex (int nLine = -1) const;
                    nonvirtual  int     LineLength (int nLine = -1) const;
                    nonvirtual  void    LineScroll (int nLines, int nChars = 0);
                    nonvirtual  void    ReplaceSel (LPCTSTR lpszNewText);
                    nonvirtual  void    SetPasswordChar (TCHAR ch);
                    nonvirtual  void    SetRect (LPCRECT lpRect);
                    nonvirtual  void    SetRectNP (LPCRECT lpRect);
                    nonvirtual  void    SetSel (DWORD dwSelection, BOOL bNoScroll = FALSE);
                    nonvirtual  void    SetSel (int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
                    nonvirtual  BOOL    SetTabStops (int nTabStops, LPINT rgTabStops);
                    nonvirtual  void    SetTabStops ();
                    nonvirtual  BOOL    SetTabStops (const int& cxEachStop);    // takes an 'int'
                    nonvirtual  BOOL    Undo ();
                    nonvirtual  void    Clear ();
                    nonvirtual  void    Copy ();
                    nonvirtual  void    Cut ();
                    nonvirtual  void    Paste ();
                    nonvirtual  BOOL    SetReadOnly (BOOL bReadOnly = TRUE);
                    nonvirtual  int     GetFirstVisibleLine () const;
                    nonvirtual  TCHAR   GetPasswordChar () const;
                protected:
                    DECLARE_MESSAGE_MAP ()
                };




#if     qTemplatedMemberFunctionsFailWithMFCMessageMaps
#define OnMFCSDKMessageDispatcherBWA_DECLARE(MESSAGE_NUMBER)\
    afx_msg LRESULT OnMFCSDKMessageDispatcher_##MESSAGE_NUMBER (WPARAM wParam, LPARAM lParam)\
    {\
        LRESULT result  =   0;\
        Verify (HandleMessage (MESSAGE_NUMBER, wParam, lParam, &result));\
        return result;\
    }
#endif



                /*
                @CLASS:         Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>
                @BASES:         BASECLASS = @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
                @DESCRIPTION:   <p>Mimicry of the starndard Win32 messages sent to an edit control. We cannot
                    mimic ALL the messages. Some just don't make sense (like GETHANDLE). But for those that do, we
                    do our best.</p>
                        <p>NB: You must declare DoDeclare_Led_MFC_OptionalWin32SDKMessageMimicHelper_MessageMap (BASECLASS) to get the
                    message map for this class generated, unless this has already been done in Led_MFC.cpp (for your particular BASECLASS).</p>
                */
                template    <typename   BASECLASS = Led_MFC_Helper<> >  class   Led_MFC_OptionalWin32SDKMessageMimicHelper :
                    public Led_Win32_Win32SDKMessageMimicHelper <BASECLASS> {
                private:
                    typedef Led_Win32_Win32SDKMessageMimicHelper <BASECLASS>    inherited;

                protected:
                    Led_MFC_OptionalWin32SDKMessageMimicHelper ();

                public:
                    template    <int    MESSAGE_NUMBER>
                    afx_msg LRESULT OnMFCSDKMessageDispatcher (WPARAM wParam, LPARAM lParam) {
                        LRESULT result  =   0;
                        Verify (HandleMessage (MESSAGE_NUMBER, wParam, lParam, &result));
                        return result;
                    }

#if     qTemplatedMemberFunctionsFailWithMFCMessageMaps
                public:
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_SETTEXT)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_GETTEXT)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_GETTEXTLENGTH)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_GETSEL)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_SETREADONLY)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_GETFIRSTVISIBLELINE)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_LINEINDEX)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_GETLINECOUNT)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_CANUNDO)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_UNDO)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_EMPTYUNDOBUFFER)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_CLEAR)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_CUT)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_COPY)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_PASTE)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_LINEFROMCHAR)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_LINELENGTH)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_LINESCROLL)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_REPLACESEL)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_SETSEL)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(EM_SCROLLCARET)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_GETFONT)
                    OnMFCSDKMessageDispatcherBWA_DECLARE(WM_SETFONT)
#endif

                protected:
                    DECLARE_MESSAGE_MAP ()
                };






                /*
                @CLASS:         Led_MFC_DragAndDropWindow<BASECLASS>
                @BASES:         BASECLASS
                @DESCRIPTION:   <p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CWnd.</p>
                        <p>To use this class, you must also
                    define DoDeclare_Led_MFC_DragAndDropWindow_MessageMap() to declare the actual message map for the template (unless its already done for yours
                    in Led_MFC.cpp).</p>
                        <p>Probably eventually migrate more stuff from @'Led_MFC_CViewHelper<BASECLASS>' here to this class. But for now,
                    I'm risk averse. I don't want to break the class library. And this is all that is needed to move out to meet the immediate
                    complaints.</p>
                */
                template    <typename   BASECLASS = Led_MFC_Helper<> >
                class   Led_MFC_DragAndDropWindow :
                    public BASECLASS {
                private:
                    typedef BASECLASS   inherited;

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
                    struct  CommandNames {
                        CommandNames ():
                            fDragCommandName (Led_SDK_TCHAROF ("Drag")),
                            fDropCommandName (Led_SDK_TCHAROF ("Drop")),
                            fDragNDropCommandName (Led_SDK_TCHAROF ("Drag and Drop")) {
                        }

                        Led_SDK_String  fDragCommandName;
                        Led_SDK_String  fDropCommandName;
                        Led_SDK_String  fDragNDropCommandName;
                    };

                    // This class builds commands with command names. The UI may wish to change these
                    // names (eg. to customize for particular languages, etc)
                    // Just patch these strings here, and commands will be created with these names.
                    // (These names appear in text of undo menu item)
                public:
                    static  const CommandNames& GetCommandNames ();
                    static  void                SetCommandNames (const CommandNames& cmdNames);
                private:
                    static  CommandNames    sCommandNames;

                protected:
                    COleDropTarget  fDropTarget;

                protected:
                    afx_msg     int     OnCreate (LPCREATESTRUCT lpCreateStruct);

                    // mouse down tracking... and drag and drop...
                protected:
                    virtual     bool        IsADragSelect (Led_Point clickedWhere) const;
                    nonvirtual  void        HandleDragSelect (UINT nFlags, CPoint oPoint);
                    nonvirtual  void        HandleSharedDragOverUpdateSelCode ();
                    nonvirtual  DROPEFFECT  HelperDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
                    nonvirtual  DROPEFFECT  HelperDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
                    nonvirtual  BOOL        HelperDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
                    nonvirtual  void        HelperDragLeave ();

                protected:
                    CPoint          fDragPoint;     // current position
                    CSize           fDragSize;      // size of dragged object
                    CSize           fDragOffset;    // offset to focus rect
                    DROPEFFECT      fPrevDropEffect;

                protected:
                    nonvirtual  bool    GetObjectInfo (COleDataObject* pDataObject, CSize* pSize, CSize* pOffset);

                protected:
                    afx_msg     void    OnTimer (UINT nEventID);
                private:
                    nonvirtual  void    StartDragAutoscrollTimer ();
                    nonvirtual  void    StopDragAutoscrollTimer ();
                    enum    { eAutoscrolling4DragTimerEventID   =   435 };  // Magic#
                    UINT    fDragAutoScrollTimerID;                         // zero means no timer

                private:
                    struct  LedStartDragAndDropContext {
                    public:
                        LedStartDragAndDropContext (Led_MFC_DragAndDropWindow<BASECLASS>* ledMFCView):
                            fOurDrag (false),
                            fOurDragStart (0),
                            fOurDragEnd (0),
                            fWeRecievedDrop (false),
                            fLedMFCView (ledMFCView),
                            fOrigSelection (ledMFCView->GetTextStore (), ledMFCView->GetSelectionStart (), ledMFCView->GetSelectionEnd ()) {
                        }
                    public:
                        bool                                    fOurDrag;
                        size_t                                  fOurDragStart;
                        size_t                                  fOurDragEnd;
                        bool                                    fWeRecievedDrop;
                        Led_MFC_DragAndDropWindow<BASECLASS>*   fLedMFCView;
                        TempMarker                              fOrigSelection;
                    };

                    static  LedStartDragAndDropContext* sCurrentDragInfo;

                protected:
                    DECLARE_MESSAGE_MAP ()
                };








                /*
                @CLASS:         Led_MFC_CViewHelper<BASECLASS>
                @BASES:         BASECLASS
                @DESCRIPTION:   <p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CView.</p>
                        <p>To use this class, you must also
                    define DoDeclare_Led_MFC_CViewHelper_MessageMap() to declare the actual message map for the template (unless its already done for yours
                    in Led_MFC.cpp).</p>
                */
                template    <typename   BASECLASS = Led_MFC_DragAndDropWindow<> >
                class   Led_MFC_CViewHelper :
                    public BASECLASS {
                private:
                    typedef BASECLASS   inherited;

                protected:
                    Led_MFC_CViewHelper ();

                public:
                    virtual ~Led_MFC_CViewHelper ();


                    // CEditView storage methods.
                public:
                    virtual     void    DeleteContents ();
                    virtual     void    Serialize (CArchive& ar);
                    nonvirtual  void    SerializeRaw (CArchive& ar);
                    nonvirtual  void    ReadFromArchive (CArchive& ar, UINT nLen);
                    nonvirtual  void    WriteToArchive (CArchive& ar);

                protected:
                    afx_msg     void    OnPaint ();
                    override    void    OnDraw (CDC* pDC);
                    afx_msg     void    OnLButtonDown (UINT nFlags, CPoint oPoint);
                    afx_msg     int     OnCreate (LPCREATESTRUCT lpCreateStruct);
                    afx_msg     void    OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
                    afx_msg     void    OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

                public:
                    nonvirtual  Led_TWIPS_Rect  GetPrintMargins () const;
                    nonvirtual  void            SetPrintMargins (const Led_TWIPS_Rect& printMargins);
                private:
                    Led_TWIPS_Rect  fPrintMargins;

                protected:
                    override    BOOL    OnPreparePrinting (CPrintInfo* pInfo);
                    override    void    OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo);
                    override    void    OnPrint (CDC* pDC, CPrintInfo* pInfo);
                    override    void    OnEndPrinting (CDC* pDC, CPrintInfo* pInfo);
                public:
                    override    void    OnPrepareDC (CDC* pDC, CPrintInfo* pInfo);
                protected:
                    virtual     Led_Rect    CalculatePrintingRect (CDC* pDC) const;
                protected:
                    struct  PrintInfo {
                        PrintInfo (BASECLASS& editor, CDC* useTablet, Led_Rect oldWindowRect, size_t savedScrollPos, bool savedForceAlLRowsFlag):
                            fTmpTablet (useTablet),
                            fOldUpdateTablet (editor, fTmpTablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall),
                            fOldWindowRect (oldWindowRect),
                            fSavedScrollPos (savedScrollPos),
                            fSavedForceAllRowsFlag (savedForceAlLRowsFlag),
                            fWindowStarts () {
                        }
                        Led_MFC_TabletFromCDC                       fTmpTablet;
                        typename BASECLASS::TemporarilyUseTablet    fOldUpdateTablet;
                        Led_Rect                                    fOldWindowRect;
                        size_t                                      fSavedScrollPos;
                        bool                                        fSavedForceAllRowsFlag;
                        vector<size_t>                              fWindowStarts;
                    };
                    PrintInfo*  fPrintInfo;     // Non-NULL only during printing...


                    // mouse down tracking... and drag and drop...
                protected:
                    override    DROPEFFECT  OnDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
                    override    DROPEFFECT  OnDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
                    override    BOOL        OnDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
                    override    void        OnDragLeave ();

                public:
                    override    void    InvalidateScrollBarParameters ();

                protected:
                    override    void    UpdateScrollBars ();

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
                typedef Led_MFC_CViewHelper<Led_MFC_DragAndDropWindow<Led_MFC_OptionalWin32SDKMessageMimicHelper <Led_MFC_MimicMFCAPIHelper <Led_MFC_Helper<CView, TextInteractor> > > > >   Led_MFC;






                /*
                @CLASS:         Led_MFC_COleControlHelper<BASECLASS>
                @BASES:         BASECLASS
                @DESCRIPTION:   <p>NB: BASECLASS <em>must</em> derive (possibly indirectly) from CView.</p>
                        <p>To use this class, you must also
                    define DoDeclare_Led_MFC_COleControlHelper_MessageMap() to declare the actual message map for the template (unless its already done for yours
                    in Led_MFC.cpp).</p>
                */
                template    <typename   BASECLASS = Led_MFC_DragAndDropWindow<> >
                class   Led_MFC_COleControlHelper :
                    public BASECLASS {
                private:
                    typedef BASECLASS   inherited;
                public:
                    typedef TextInteractor::UpdateMode  UpdateMode;

                protected:
                    Led_MFC_COleControlHelper () {}

                public:
                    virtual ~Led_MFC_COleControlHelper () {}

                public:
                    nonvirtual  void    Refresh (UpdateMode updateMode = TextInteractor::eDefaultUpdate) const {
                        const TextInteractor*   ti  =   this;
                        ti->Refresh (updateMode);
                    }
                    nonvirtual  void    Refresh (const Led_Rect& area, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const {
                        const TextInteractor*   ti  =   this;
                        ti->Refresh (area, updateMode);
                    }
                    nonvirtual  void    Refresh (size_t from, size_t to, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const {
                        const TextInteractor*   ti  =   this;
                        ti->Refresh (from, to, updateMode);
                    }
                    nonvirtual  void    Refresh (const Marker* range, UpdateMode updateMode = TextInteractor::eDefaultUpdate) const {
                        const TextInteractor*   ti  =   this;
                        ti->Refresh (range, updateMode);
                    }

                protected:
                    DECLARE_MESSAGE_MAP ()
                };


//  class   Led_MFC_COleControlHelper<BASECLASS>
#define DoDeclare_Led_MFC_COleControlHelper_MessageMap(MFC_BASE_CLASS)\
    BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_COleControlHelper, MFC_BASE_CLASS, MFC_BASE_CLASS)\
    END_MESSAGE_MAP()




                /*
                @CLASS:         Led_MFC_ExceptionHandlerHelper<BASECLASS>
                @BASES:         BASECLASS = @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'
                @DESCRIPTION:   <p>This helper is designed to manage exception handling, when the Led class is being used in a context (say OCX)
                    where it cannot throw exceptions in error situations.</p>
                        <p>This class wraps many messages the control is likely to get, and when there is an exception, it calls a virtual
                    exception-handling method (which by default - beeps), and then returns normally.</p>
                        <p>To use this class, you must also
                    define DoDeclare_Led_MFC_ExceptionHandlerHelper_MessageMap() to declare the actual message map for the template.</p>
                */
                template    <typename   BASECLASS = Led_MFC >
				class   Led_MFC_ExceptionHandlerHelper :
                    public BASECLASS {
                private:
                    typedef BASECLASS   inherited;

                protected:
                    virtual void    HandleException () const;

                public:
                    afx_msg     void    OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);
                    afx_msg     LRESULT OnIMEChar (WPARAM wParam, LPARAM lParam);
                    afx_msg     void    OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
                    afx_msg     LRESULT OnMsgPaste (WPARAM wParam, LPARAM lParam);
                    afx_msg     void    OnLButtonDown (UINT nFlags, CPoint oPoint);
                    afx_msg     void    OnLButtonUp (UINT /*nFlags*/, CPoint oPoint);
                    afx_msg     void    OnLButtonDblClk (UINT /*nFlags*/, CPoint oPoint);
                    afx_msg     BOOL    OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

                protected:
                    DECLARE_MESSAGE_MAP ()
                };









                /*
                @CLASS:         Led_MFC_X
                @BASES:         ChosenInteractor, @'Led_MFC'
                @DESCRIPTION:   <p>Utility template to mix together Led_MFC an a <code>ChosenInteractor</code> which already has support
                    for a particular TextImager mixed in.</p>
                */
                template    <typename   ChosenInteractor, typename LEDMFC = Led_MFC>
                class   Led_MFC_X :
#if     qMFCRequiresCWndLeftmostBaseClass
                    public LEDMFC, public ChosenInteractor
#else
                    public ChosenInteractor, public LEDMFC
#endif
                {
                public:
                    Led_MFC_X ();

                public:
                    override    void    AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo);
                    override    void    DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw ();
                };













                /*
                @CLASS:         Led_MFCReaderDAndDFlavorPackage
                @BASES:         @'ReaderFlavorPackage'
                @DESCRIPTION:   <p>ReaderFlavorPackage which reads from a COleDataObject as
                    its underlying storage. Useful for Drop (Drag and Drop) processing.</p>
                */
                class   Led_MFCReaderDAndDFlavorPackage : public ReaderFlavorPackage {
                public:
                    Led_MFCReaderDAndDFlavorPackage (COleDataObject* dataObject);

                    override    bool    GetFlavorAvailable (Led_ClipFormat clipFormat) const;
                    override    size_t  GetFlavorSize (Led_ClipFormat clipFormat) const;
                    override    size_t  ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const;

                    nonvirtual  COleDataObject* GetOleDataObject () const;

                private:
                    COleDataObject* fDataObject;
                };






                /*
                @CLASS:         Led_MFCWriterDAndDFlavorPackage
                @BASES:         @'WriterFlavorPackage'
                @DESCRIPTION:   <p>A WriterFlavorPackage which writes to a COleDataObject as
                    its underlying storage. Useful for Drag (Drag and Drop) processing.</p>
                */
                class   Led_MFCWriterDAndDFlavorPackage : public WriterFlavorPackage {
                public:
                    Led_MFCWriterDAndDFlavorPackage (COleDataSource* dataObject);

                    override    void    AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf);

                    nonvirtual  COleDataSource* GetOleDataSource () const;

                private:
                    COleDataSource* fDataObject;
                };










                template    <typename   TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
                CSize   Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);

                template    <typename   TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
                void    Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);

#if     qSupportDrawTextGetTextExtent
                CSize   Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);
                void    Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap);
#endif








                /*
                @CLASS:         MFC_CommandNumberMapping
                @DESCRIPTION:   <p></p>
                */
                class   MFC_CommandNumberMapping : public CommandNumberMapping<UINT> {
                protected:
                    MFC_CommandNumberMapping ();
                };






                /*
                @CLASS:         Led_MFC_TmpCmdUpdater
                @BASES:         @'TextInteractor::CommandUpdater'
                @DESCRIPTION:   <p>Helper used in @'Led_Win32_Helper<BASE_INTERACTOR>'</p>
                */
                class   Led_MFC_TmpCmdUpdater : public TextInteractor::CommandUpdater {
                public:
                    explicit Led_MFC_TmpCmdUpdater (CCmdUI* pCmdUI);

                public:
                    override    CommandNumber   GetCmdID () const;
                    override    bool            GetEnabled () const;
                    override    void            SetEnabled (bool enabled);
                    override    void            SetChecked (bool checked);
                    override    void            SetText (const Led_SDK_Char* text);

                public:
                    operator CCmdUI* ();
                    operator Led_MFC_TmpCmdUpdater* ();

                private:
                    CommandNumber   fCmdNum;
                    CCmdUI*         fCmdUI;
                    bool            fEnabled;
                };






                /*
                @CLASS:         LED_MFC_HANDLE_COMMAND
                @DESCRIPTION:   <p>Trivial helper for building MFC message maps.</p>
                */
#define LED_MFC_HANDLE_COMMAND(A)   \
    ON_COMMAND_RANGE(A,A,OnPerformCommand_MSG)\
    ON_UPDATE_COMMAND_UI(A,OnUpdateCommand_MSG)



                /*
                @CLASS:         LED_MFC_HANDLE_COMMAND_RANGE
                @DESCRIPTION:   <p>Trivial helper for building MFC message maps.</p>
                */
#define LED_MFC_HANDLE_COMMAND_RANGE(A,B)   \
    ON_COMMAND_RANGE(A,B,OnPerformCommand_MSG)\
    ON_UPDATE_COMMAND_UI_RANGE(A,B,OnUpdateCommand_MSG)






                /*
                @CLASS:         LED_MFC_HANDLE_COMMAND_M
                @DESCRIPTION:   <p></p>
                */
#define LED_MFC_HANDLE_COMMAND_M(A) \
    LED_MFC_HANDLE_COMMAND(MFC_CommandNumberMapping::Get ().ReverseLookup (A))

                /*
                @CLASS:         LED_MFC_HANDLE_COMMAND_RANGE_M
                @DESCRIPTION:   <p></p>
                */
#define LED_MFC_HANDLE_COMMAND_RANGE_M(A,B) \
    LED_MFC_HANDLE_COMMAND_RANGE(MFC_CommandNumberMapping::Get ().ReverseLookup (A), MFC_CommandNumberMapping::Get ().ReverseLookup (B))













                /*
                 ********************************************************************************
                 ***************************** Implementation Details ***************************
                 ********************************************************************************
                 */
#if     qLedCheckCompilerFlagsConsistency
                namespace LedCheckCompilerFlags_Led_MFC {
                    extern  int LedCheckCompilerFlags_(qMFCRequiresCWndLeftmostBaseClass);

                    struct  FlagsChecker {
                        FlagsChecker () {
                            /*
                             *  See the docs on @'qLedCheckCompilerFlagsConsistency' if this ever fails.
                             */
                            if (LedCheckCompilerFlags_(qMFCRequiresCWndLeftmostBaseClass) != qMFCRequiresCWndLeftmostBaseClass)     {           abort ();       }
                        }
                    };
                    static  struct  FlagsChecker    sFlagsChecker;
                }
#endif


                /*
                @METHOD:        AsCPoint
                @DESCRIPTION:   Convert a @'Led_Point' to an MFC CPoint.
                    <p>See also @'AsLedPoint', @'AsCRect', @'AsLedRect'.
                */
                inline  CPoint  AsCPoint (Led_Point p)
                {
                    return CPoint (p.h, p.v);
                }
                /*
                @METHOD:        AsLedPoint
                @DESCRIPTION:   Convert an MFC CPoint to a @'Led_Point'.
                    <p>See also @'AsCPoint', @'AsCRect', @'AsLedRect'.
                */
                inline  Led_Point   AsLedPoint (CPoint p)
                {
                    return Led_Point (p.y, p.x);
                }
                /*
                @METHOD:        AsCRect
                @DESCRIPTION:   Convert a Led_Rect to an MFC CRect.
                    <p>See also @'AsCPoint', @'AsLedPoint', @'AsLedRect'.
                */
                inline  CRect   AsCRect (Led_Rect r)
                {
                    return CRect (r.left, r.top, r.right, r.bottom);
                }
                /*
                @METHOD:        AsLedRect
                @DESCRIPTION:   Convert an MFC CRect to a Led_Rect.
                    <p>See also @'AsCPoint', @'AsLedPoint', @'AsCRect'.
                */
                inline  Led_Rect    AsLedRect (CRect r)
                {
                    return Led_Rect (r.top, r.left, r.Height (), r.Width ());
                }
                /*
                @METHOD:        AsCSize
                @DESCRIPTION:   Convert a Led_Size to an MFC CSize.
                */
                inline  CSize   AsCSize (Led_Size s)
                {
                    return CSize (s.h, s.v);
                }





//  class   Led_MFC_CDCFromTablet
                inline  Led_MFC_CDCFromTablet::Led_MFC_CDCFromTablet (Led_Tablet t):
                    fCDC ()
                {
                    RequireNotNull (t);
                    fCDC.m_hDC = t->m_hDC;
                    fCDC.m_hAttribDC = t->m_hAttribDC;
                    fCDC.m_bPrinting = t->m_bPrinting;
                }
                inline  Led_MFC_CDCFromTablet::~Led_MFC_CDCFromTablet ()
                {
                    fCDC.Detach ();
                }
                inline  Led_MFC_CDCFromTablet::operator CDC* ()
                {
                    return &fCDC;
                }



//  class   Led_MFC_TabletFromCDC
                inline  Led_MFC_TabletFromCDC::Led_MFC_TabletFromCDC (CDC* pDC):
                    fTablet ()
                {
                    RequireNotNull (pDC);
                    fTablet.m_hDC = pDC->m_hDC;
                    fTablet.m_hAttribDC = pDC->m_hAttribDC;
                    fTablet.m_bPrinting = pDC->m_bPrinting;
                }
                inline  Led_MFC_TabletFromCDC::~Led_MFC_TabletFromCDC ()
                {
                    fTablet.Detach ();
                }
                inline  Led_MFC_TabletFromCDC::operator Led_Tablet ()
                {
                    return &fTablet;
                }
                inline  Led_Tablet Led_MFC_TabletFromCDC::operator-> ()
                {
                    return &fTablet;
                }



#define BEGIN_TEMPLATE_2_MESSAGE_MAP(theClass, type_name1, type_name2, baseClass)           \
    PTM_WARNING_DISABLE                                                     \
    template < typename type_name1, typename type_name2 >                                           \
    const AFX_MSGMAP* theClass< type_name1, type_name2 >::GetMessageMap() const         \
    { return GetThisMessageMap(); }                                     \
    template < typename type_name1, typename type_name2 >                                           \
    const AFX_MSGMAP* PASCAL theClass< type_name1, type_name2 >::GetThisMessageMap()        \
    {                                                                       \
        typedef theClass< type_name1, type_name2 > ThisClass;                           \
        typedef baseClass TheBaseClass;                                     \
        static const AFX_MSGMAP_ENTRY _messageEntries[] =                   \
                {



//  class   Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>
#define DoDeclare_Led_MFC_Helper_MessageMap(MFC_BASE_CLASS,BASE_INTERACTOR)\
    BEGIN_TEMPLATE_2_MESSAGE_MAP(Led_MFC_Helper, MFC_BASE_CLASS, BASE_INTERACTOR, MFC_BASE_CLASS)\
    ON_WM_CREATE()\
    ON_WM_CHAR()\
    ON_WM_TIMER()\
    ON_WM_KEYDOWN()\
    ON_WM_PAINT ()\
    ON_WM_MOUSEMOVE()\
    ON_WM_LBUTTONDOWN()\
    ON_WM_SETCURSOR()\
    ON_WM_GETDLGCODE ()\
    ON_WM_LBUTTONUP()\
    ON_WM_LBUTTONDBLCLK()\
    ON_WM_SETFOCUS()\
    ON_WM_KILLFOCUS()\
    ON_WM_SIZE      ()\
    ON_WM_ERASEBKGND()\
    ON_WM_VSCROLL   ()\
    ON_WM_HSCROLL   ()\
    ON_WM_MOUSEWHEEL()\
    ON_WM_ENABLE    ()\
    ON_MESSAGE (WM_UNICHAR, OnUniChar)\
    ON_MESSAGE (WM_IME_CHAR, OnIMEChar)\
    ON_MESSAGE (WM_IME_COMPOSITION, OnIME_COMPOSITION)\
    ON_MESSAGE (WM_IME_ENDCOMPOSITION, OnIME_ENDCOMPOSITION)\
    \
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kUndo_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kRedo_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kSelectAll_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kSelectWord_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kSelectTextRow_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kSelectParagraph_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kCut_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kCopy_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kPaste_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kClear_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kFind_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kFindAgain_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kEnterFindString_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kReplace_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kReplaceAgain_CmdID)\
    LED_MFC_HANDLE_COMMAND_M    (BASE_INTERACTOR::kSpellCheck_CmdID)\
    END_MESSAGE_MAP()


#if     qSupportLed30CompatAPI
#define DoDeclare_TextInteractorCommonCommandHelper_MFC_MessageMap(BASECLASS,CMD_INFO,CMD_ENABLER)
#endif


                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline  Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::Led_MFC_Helper ():
                    LED_WIN32_HELPER (),
                    MFC_BASE_CLASS ()
                {
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline  Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::~Led_MFC_Helper ()
                {
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                HWND    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetHWND () const
                {
                    return m_hWnd;
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                int Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnCreate (LPCREATESTRUCT lpCreateStruct)
                {
                    RequireNotNull (lpCreateStruct);
                    if (MFC_BASE_CLASS::OnCreate (lpCreateStruct) != 0) {
                        return -1;  // failed to create
                    }
                    TabletChangedMetrics ();        // maybe should catch failures here, and return -1?
                    if ((lpCreateStruct->style & WS_VSCROLL) and GetScrollBarType (v) == eScrollBarNever) {
                        SetScrollBarType (v, eScrollBarAlways);
                    }
                    if ((lpCreateStruct->style & WS_HSCROLL) and GetScrollBarType (h) == eScrollBarNever) {
                        SetScrollBarType (h, eScrollBarAlways);
                    }
                    return 0;   // sucess
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnPaint ()
                {
                    CPaintDC    dc (this);
                    CRect       boundsRect  =   CRect (0, 0, 0, 0);
                    Verify (dc.GetClipBox (&boundsRect) != ERROR);
                    WindowDrawHelper (Led_MFC_TabletFromCDC (&dc), AsLedRect (boundsRect), false);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                /*
                @METHOD:        Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnSetCursor
                @DESCRIPTION:   Hook the Win32 SDK WM_SETCURSOR message to handle set the cursor to an I-Beam, as appropriate. When over
                    draggable text, instead use a standard arrow cursor.
                */
                BOOL    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message)
                {
                    return OnSetCursor_Msg (pWnd->GetSafeHwnd (), nHitTest, message);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                UINT    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnGetDlgCode ()
                {
                    return OnGetDlgCode_Msg ();
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
                {
                    OnChar_Msg (nChar, nRepCnt + (nFlags << 16));
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
                {
                    OnKeyDown_Msg (nChar, nRepCnt + (nFlags << 16));
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnMouseMove (UINT nFlags, CPoint oPoint)
                {
                    OnMouseMove_Msg (nFlags, oPoint.x, oPoint.y);
                }
#if     qSupportLed30CompatAPI
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                BOOL    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::SubclassWindow (HWND hWnd)
                {
                    /*
                     *  We've never bothered really supporting the SubclassWindow mechanism since it
                     *  didn't seem to work very well. In Led 3.0 and earlier, we REALLY just did
                     *  a REPLACE. Now just cleanup our nomenclature, but keep this routine name
                     *  around for backward compatability.
                     */
                    return ReplaceWindow (hWnd);
                }
#endif
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                /*
                @METHOD:        Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::ReplaceWindow
                @DESCRIPTION:   <p>Similar to @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::ReplaceWindow'
                            except that it works with MFC windows and MFC's CWnd message maps etc.
                            </p>
                */
                BOOL    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::ReplaceWindow (HWND hWnd)
                {
                    CWnd*   parent      =   MFC_BASE_CLASS::FromHandle (hWnd)->GetParent ();
                    HWND    hwndParent  =   parent->GetSafeHwnd ();
                    if (hwndParent == NULL) {
                        return false;
                    }

                    int id = ::GetWindowLong (hWnd, GWL_ID);

                    HWND hwndEdit = ::GetDlgItem (hwndParent, id);
                    AssertNotNull (hwndEdit);

                    DWORD   dwStyle = ::GetWindowLong (hwndEdit, GWL_STYLE);
                    DWORD   exStyle = ::GetWindowLong (hwndEdit, GWL_EXSTYLE);

                    // Assume edit widget's position.
                    WINDOWPLACEMENT wp;
                    memset (&wp, 0, sizeof (wp));
                    wp.length = sizeof (wp);
                    Verify (::GetWindowPlacement (hwndEdit, &wp));

                    // Delete the edit widget window.
                    ::DestroyWindow (hwndEdit);

                    return CreateEx (exStyle, NULL, NULL, dwStyle | WS_CHILD,
                                     wp.rcNormalPosition.left, wp.rcNormalPosition.top,
                                     wp.rcNormalPosition.right - wp.rcNormalPosition.left, wp.rcNormalPosition.bottom - wp.rcNormalPosition.top,
                                     parent->GetSafeHwnd (), (HMENU)id, NULL
                                    );

                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                /*
                @METHOD:        Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::GetWindowRect
                @DESCRIPTION:   Return the Led WindowRect. This name is a somewhat unfortunate choice for Windows, because WindowRect means something
                    vaguely similar, but substantially different in the Win32 SDK.
                        <p>In the future, I may need to consider changing this name. But for now, I can think of none better.
                        <p>I provide two overloads of this routine. The one that returns  a Led_Rect returns the Led WindowRect. And the one
                    that takes an LPRECT parameter returns the Win32 SDK WindowRect.
                */
                inline  Led_Rect    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetWindowRect () const
                {
                    return (TextImager::GetWindowRect ());
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline  void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetWindowRect (LPRECT lpRect) const
                {
                    // The CWnd version of GetWindowRect ()
                    const CWnd* cwnd    =   this;
                    CWnd*       wnd     =   const_cast<CWnd*> (cwnd);
                    wnd->GetWindowRect (lpRect);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline  DWORD   Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetStyle () const
                {
                    return MFC_BASE_CLASS::GetStyle ();
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnLButtonDown (UINT nFlags, CPoint oPoint)
                {
                    OnLButtonDown_Msg (nFlags, oPoint.x, oPoint.y);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnLButtonUp (UINT nFlags, CPoint oPoint)
                {
                    OnLButtonUp_Msg (nFlags, oPoint.x, oPoint.y);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnLButtonDblClk (UINT nFlags, CPoint oPoint)
                {
                    OnLButtonDblClk_Msg (nFlags, oPoint.x, oPoint.y);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnSetFocus (CWnd* pOldWnd)
                {
                    MFC_BASE_CLASS::OnSetFocus (pOldWnd);
                    OnSetFocus_Msg (pOldWnd->GetSafeHwnd ());
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnKillFocus (CWnd* pNewWnd)
                {
                    OnKillFocus_Msg (pNewWnd->GetSafeHwnd ());
                    MFC_BASE_CLASS::OnKillFocus (pNewWnd);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnSize (UINT nType, int cx, int cy)
                {
                    MFC_BASE_CLASS::OnSize (nType, cx, cy);
                    OnSize_Msg ();
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                BOOL    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnEraseBkgnd (CDC* pDC)
                {
                    return OnEraseBkgnd_Msg (pDC->GetSafeHdc ());
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
                {
                    OnVScroll_Msg (nSBCode, nPos, pScrollBar->GetSafeHwnd ());
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
                {
                    OnHScroll_Msg (nSBCode, nPos, pScrollBar->GetSafeHwnd ());
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                BOOL    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnMouseWheel (UINT fFlags, short zDelta, CPoint point)
                {
                    return OnMouseWheel_Msg (MAKEWPARAM (fFlags, zDelta), MAKELPARAM (point.x, point.y));
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnEnable (BOOL bEnable)
                {
                    OnEnable_Msg (bEnable);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnTimer (UINT nEventID)
                {
                    OnTimer_Msg (nEventID, NULL);
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline  LRESULT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnUniChar (WPARAM wParam, LPARAM lParam)
                {
#if     qWideCharacters
                    return LED_WIN32_HELPER::OnUniChar_Msg (wParam, lParam);
#else
                    return MFC_BASE_CLASS::Default ();
#endif
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline  LRESULT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnIMEChar (WPARAM wParam, LPARAM lParam)
                {
#if     qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
                    return LED_WIN32_HELPER::OnIMEChar_Msg (wParam, lParam);
#else
                    return MFC_BASE_CLASS::Default ();
#endif
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline  LRESULT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnIME_COMPOSITION (WPARAM wParam, LPARAM lParam)
                {
#if     qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
                    return LED_WIN32_HELPER::OnIME_COMPOSITION_Msg (wParam, lParam);
#else
                    return MFC_BASE_CLASS::Default ();
#endif
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline  LRESULT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnIME_ENDCOMPOSITION (WPARAM wParam, LPARAM lParam)
                {
#if     qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
                    return LED_WIN32_HELPER::OnIME_ENDCOMPOSITION_Msg (wParam, lParam);
#else
                    return MFC_BASE_CLASS::Default ();
#endif
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnUpdateCommand_MSG (CCmdUI* pCmdUI)
                {
                    RequireNotNull (pCmdUI);
                    if (not OnUpdateCommand (Led_MFC_TmpCmdUpdater (pCmdUI))) {
                        pCmdUI->Enable (false);     // disable commands by default (SPR#1462)
                    }
                }
                template    <typename   MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void    Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnPerformCommand_MSG (UINT commandNumber)
                {
                    IdleManager::NonIdleContext nonIdleContext;
                    (void)OnPerformCommand (MFC_CommandNumberMapping::Get ().Lookup (commandNumber));
                }






//  class   Led_MFC_MimicMFCAPIHelper<BASECLASS>
#define DoDeclare_Led_MFC_MimicMFCAPIHelper_MessageMap(MFC_BASE_CLASS)\
    BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_MimicMFCAPIHelper, MFC_BASE_CLASS, MFC_BASE_CLASS)\
    END_MESSAGE_MAP()
                template    <typename   BASECLASS>
                inline  Led_MFC_MimicMFCAPIHelper<BASECLASS>::Led_MFC_MimicMFCAPIHelper ()
                {
                }
                template    <typename   BASECLASS>
                inline  BOOL    Led_MFC_MimicMFCAPIHelper<BASECLASS>::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
                {
                    return (BASECLASS::Create (NULL, NULL, dwStyle, rect, pParentWnd, nID));
                }
                template    <typename   BASECLASS>
                inline  BOOL    Led_MFC_MimicMFCAPIHelper<BASECLASS>::CanUndo () const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_CANUNDO, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  int     Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLineCount_CEdit () const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_GETLINECOUNT, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  BOOL    Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetModify () const
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_GETMODIFY, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetModify (BOOL bModified)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETMODIFY, bModified, 0);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetRect (LPRECT lpRect) const
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetSel (int& nStartChar, int& nEndChar) const
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_GETSEL, (WPARAM)&nStartChar, (LPARAM)&nEndChar);
                }
                template    <typename   BASECLASS>
                inline  DWORD   Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetSel () const
                {
                    Assert (::IsWindow (m_hWnd));
                    return ::SendMessage (m_hWnd, EM_GETSEL, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLine (int nIndex, LPTSTR lpszBuffer) const
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
                }
                template    <typename   BASECLASS>
                inline  int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLine (int nIndex, LPTSTR lpszBuffer, int nMaxLength) const
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    *(LPWORD)lpszBuffer = (WORD)nMaxLength;
                    return (int)::SendMessage (m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::EmptyUndoBuffer ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  BOOL    Led_MFC_MimicMFCAPIHelper<BASECLASS>::FmtLines (BOOL bAddEOL)
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_FMTLINES, bAddEOL, 0);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::LimitText (int nChars)
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_LIMITTEXT, nChars, 0);
                }
                template    <typename   BASECLASS>
                inline  int     Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineFromChar (int nIndex) const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_LINEFROMCHAR, nIndex, 0);
                }
                template    <typename   BASECLASS>
                inline  int     Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineIndex (int nLine) const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_LINEINDEX, nLine, 0);
                }
                template    <typename   BASECLASS>
                inline  int     Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineLength (int nLine) const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_LINELENGTH, nLine, 0);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineScroll (int nLines, int nChars)
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_LINESCROLL, nChars, nLines);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::ReplaceSel (LPCTSTR lpszNewText)
                {
                    AssertNotNull (lpszNewText);
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_REPLACESEL, 0, (LPARAM)lpszNewText);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetPasswordChar (TCHAR ch)
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETPASSWORDCHAR, ch, 0);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetRect (LPCRECT lpRect)
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETRECT, 0, (LPARAM)lpRect);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetRectNP (LPCRECT lpRect)
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETRECTNP, 0, (LPARAM)lpRect);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetSel (DWORD dwSelection, BOOL bNoScroll)
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection));
                    if (not bNoScroll) {
                        ::SendMessage (m_hWnd, EM_SCROLLCARET, 0, 0);
                    }
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetSel (int nStartChar, int nEndChar, BOOL bNoScroll)
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETSEL, nStartChar, nEndChar);
                    if (not bNoScroll) {
                        ::SendMessage (m_hWnd, EM_SCROLLCARET, 0, 0);
                    }
                }
                template    <typename   BASECLASS>
                inline  BOOL    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops (int nTabStops, LPINT rgTabStops)
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops ()
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    Verify (::SendMessage (m_hWnd, EM_SETTABSTOPS, 0, 0));
                }
                template    <typename   BASECLASS>
                inline  BOOL    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops (const int& cxEachStop)
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_SETTABSTOPS, 1, (LPARAM)(LPINT)&cxEachStop);
                }
                template    <typename   BASECLASS>
                inline  BOOL    Led_MFC_MimicMFCAPIHelper<BASECLASS>::Undo ()
                {
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_UNDO, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::Clear()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, WM_CLEAR, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::Copy ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, WM_COPY, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  void Led_MFC_MimicMFCAPIHelper<BASECLASS>::Cut ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, WM_CUT, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  void    Led_MFC_MimicMFCAPIHelper<BASECLASS>::Paste ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, WM_PASTE, 0, 0);
                }
                template    <typename   BASECLASS>
                inline  BOOL    Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetReadOnly (BOOL bReadOnly )
                {
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_SETREADONLY, bReadOnly, 0L);
                }
                template    <typename   BASECLASS>
                inline  int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetFirstVisibleLine () const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L);
                }
                template    <typename   BASECLASS>
                inline  TCHAR   Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetPasswordChar () const
                {
                    Assert (false);     // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (TCHAR)::SendMessage (m_hWnd, EM_GETPASSWORDCHAR, 0, 0L);
                }










//  class   Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>
#if     qTemplatedMemberFunctionsFailWithMFCMessageMaps
#define DoDeclare_Led_MFC_OptionalWin32SDKMessageMimicHelper_MessageMap(MFC_BASE_CLASS)\
    BEGIN_MESSAGE_MAP(Led_MFC_OptionalWin32SDKMessageMimicHelper, MFC_BASE_CLASS, Led_MFC_OptionalWin32SDKMessageMimicHelper<MFC_BASE_CLASS >::inherited)\
        ON_MESSAGE      (WM_SETTEXT,                    OnMFCSDKMessageDispatcher_##WM_SETTEXT)\
        ON_MESSAGE      (WM_GETTEXT,                    OnMFCSDKMessageDispatcher_##WM_GETTEXT)\
        ON_MESSAGE      (WM_GETTEXTLENGTH,              OnMFCSDKMessageDispatcher_##WM_GETTEXTLENGTH)\
        ON_MESSAGE      (EM_GETSEL,                     OnMFCSDKMessageDispatcher_##EM_GETSEL)\
        ON_MESSAGE      (EM_SETREADONLY,                OnMFCSDKMessageDispatcher_##EM_SETREADONLY)\
        ON_MESSAGE      (EM_GETFIRSTVISIBLELINE,        OnMFCSDKMessageDispatcher_##EM_GETFIRSTVISIBLELINE)\
        ON_MESSAGE      (EM_LINEINDEX,                  OnMFCSDKMessageDispatcher_##EM_LINEINDEX)\
        ON_MESSAGE      (EM_GETLINECOUNT,               OnMFCSDKMessageDispatcher_##EM_GETLINECOUNT)\
        ON_MESSAGE      (EM_CANUNDO,                    OnMFCSDKMessageDispatcher_##EM_CANUNDO)\
        ON_MESSAGE      (EM_UNDO,                       OnMFCSDKMessageDispatcher_##EM_UNDO)\
        ON_MESSAGE      (EM_EMPTYUNDOBUFFER,            OnMFCSDKMessageDispatcher_##EM_EMPTYUNDOBUFFER)\
        ON_MESSAGE      (WM_CLEAR,                      OnMFCSDKMessageDispatcher_##WM_CLEAR)\
        ON_MESSAGE      (WM_CUT,                        OnMFCSDKMessageDispatcher_##WM_CUT)\
        ON_MESSAGE      (WM_COPY,                       OnMFCSDKMessageDispatcher_##WM_COPY)\
        ON_MESSAGE      (WM_PASTE,                      OnMFCSDKMessageDispatcher_##WM_PASTE)\
        ON_MESSAGE      (EM_LINEFROMCHAR,               OnMFCSDKMessageDispatcher_##EM_LINEFROMCHAR)\
        ON_MESSAGE      (EM_LINELENGTH,                 OnMFCSDKMessageDispatcher_##EM_LINELENGTH)\
        ON_MESSAGE      (EM_LINESCROLL,                 OnMFCSDKMessageDispatcher_##EM_LINESCROLL)\
        ON_MESSAGE      (EM_REPLACESEL,                 OnMFCSDKMessageDispatcher_##EM_REPLACESEL)\
        ON_MESSAGE      (EM_SETSEL,                     OnMFCSDKMessageDispatcher_##EM_SETSEL)\
        ON_MESSAGE      (EM_SCROLLCARET,                OnMFCSDKMessageDispatcher_##EM_SCROLLCARET)\
        ON_MESSAGE      (WM_GETFONT,                    OnMFCSDKMessageDispatcher_##WM_GETFONT)\
        ON_MESSAGE      (WM_SETFONT,                    OnMFCSDKMessageDispatcher_##WM_SETFONT)\
    END_MESSAGE_MAP()
#else
#define DoDeclare_Led_MFC_OptionalWin32SDKMessageMimicHelper_MessageMap(MFC_BASE_CLASS)\
    BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_OptionalWin32SDKMessageMimicHelper, MFC_BASE_CLASS, Led_MFC_OptionalWin32SDKMessageMimicHelper<MFC_BASE_CLASS >::inherited)\
    ON_MESSAGE      (WM_SETTEXT,                    OnMFCSDKMessageDispatcher<WM_SETTEXT>)\
    ON_MESSAGE      (WM_GETTEXT,                    OnMFCSDKMessageDispatcher<WM_GETTEXT>)\
    ON_MESSAGE      (WM_GETTEXTLENGTH,              OnMFCSDKMessageDispatcher<WM_GETTEXTLENGTH>)\
    ON_MESSAGE      (EM_GETSEL,                     OnMFCSDKMessageDispatcher<EM_GETSEL>)\
    ON_MESSAGE      (EM_SETREADONLY,                OnMFCSDKMessageDispatcher<EM_SETREADONLY>)\
    ON_MESSAGE      (EM_GETFIRSTVISIBLELINE,        OnMFCSDKMessageDispatcher<EM_GETFIRSTVISIBLELINE>)\
    ON_MESSAGE      (EM_LINEINDEX,                  OnMFCSDKMessageDispatcher<EM_LINEINDEX>)\
    ON_MESSAGE      (EM_GETLINECOUNT,               OnMFCSDKMessageDispatcher<EM_GETLINECOUNT>)\
    ON_MESSAGE      (EM_CANUNDO,                    OnMFCSDKMessageDispatcher<EM_CANUNDO>)\
    ON_MESSAGE      (EM_UNDO,                       OnMFCSDKMessageDispatcher<EM_UNDO>)\
    ON_MESSAGE      (EM_EMPTYUNDOBUFFER,            OnMFCSDKMessageDispatcher<EM_EMPTYUNDOBUFFER>)\
    ON_MESSAGE      (WM_CLEAR,                      OnMFCSDKMessageDispatcher<WM_CLEAR>)\
    ON_MESSAGE      (WM_CUT,                        OnMFCSDKMessageDispatcher<WM_CUT>)\
    ON_MESSAGE      (WM_COPY,                       OnMFCSDKMessageDispatcher<WM_COPY>)\
    ON_MESSAGE      (WM_PASTE,                      OnMFCSDKMessageDispatcher<WM_PASTE>)\
    ON_MESSAGE      (EM_LINEFROMCHAR,               OnMFCSDKMessageDispatcher<EM_LINEFROMCHAR>)\
    ON_MESSAGE      (EM_LINELENGTH,                 OnMFCSDKMessageDispatcher<EM_LINELENGTH>)\
    ON_MESSAGE      (EM_LINESCROLL,                 OnMFCSDKMessageDispatcher<EM_LINESCROLL>)\
    ON_MESSAGE      (EM_REPLACESEL,                 OnMFCSDKMessageDispatcher<EM_REPLACESEL>)\
    ON_MESSAGE      (EM_SETSEL,                     OnMFCSDKMessageDispatcher<EM_SETSEL>)\
    ON_MESSAGE      (EM_SCROLLCARET,                OnMFCSDKMessageDispatcher<EM_SCROLLCARET>)\
    ON_MESSAGE      (WM_GETFONT,                    OnMFCSDKMessageDispatcher<WM_GETFONT>)\
    ON_MESSAGE      (WM_SETFONT,                    OnMFCSDKMessageDispatcher<WM_SETFONT>)\
    END_MESSAGE_MAP()
#endif
                template    <typename   BASECLASS>
                Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>::Led_MFC_OptionalWin32SDKMessageMimicHelper ():
                    inherited ()
                {
                }








//  class   Led_MFC_DragAndDropWindow<BASECLASS>
#define DoDeclare_Led_MFC_DragAndDropWindow_MessageMap(MFC_BASE_CLASS)\
    BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_DragAndDropWindow, MFC_BASE_CLASS, Led_MFC_DragAndDropWindow<MFC_BASE_CLASS >::inherited)\
    ON_WM_CREATE    ()\
    ON_WM_TIMER     ()\
    END_MESSAGE_MAP()

                template    <typename   BASECLASS>
                typename    Led_MFC_DragAndDropWindow<BASECLASS>::LedStartDragAndDropContext*   Led_MFC_DragAndDropWindow<BASECLASS>::sCurrentDragInfo  =   NULL;
                template    <typename   BASECLASS>
                Led_MFC_DragAndDropWindow<BASECLASS>::Led_MFC_DragAndDropWindow ():
                    inherited (),
                    fDropTarget (),
                    fDragPoint (),
                    fDragSize (),
                    fDragOffset (),
                    fPrevDropEffect (DROPEFFECT_NONE),
                    fDragAutoScrollTimerID (0)
                {
                }
                template    <typename   BASECLASS>
                Led_MFC_DragAndDropWindow<BASECLASS>::~Led_MFC_DragAndDropWindow ()
                {
                    Assert (fDragAutoScrollTimerID == 0);
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames
                @DESCRIPTION:   Returns command name for each of the user-visible commands produced by this module.
                    This name is used used in the constructed Undo command name, as
                    in, "Undo Drag and Drop". You can replace this name with whatever you like.You change this value with
                    WordProcessor::SetCommandNames.
                        <p> The point of this is to allow for different UI-language localizations,
                            without having to change Led itself.
                        <p>See also @'Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames'.
                */
                inline  typename const Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames&  Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames ()
                {
                    return sCommandNames;
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_DragAndDropWindow<BASECLASS>::SetCommandNames
                @DESCRIPTION:   See @'Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames'.
                */
                inline  void    Led_MFC_DragAndDropWindow<BASECLASS>::SetCommandNames (const typename Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames& cmdNames)
                {
                    sCommandNames = cmdNames;
                }
                template    <typename   BASECLASS>
                typename Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames Led_MFC_DragAndDropWindow<BASECLASS>::sCommandNames;
                template    <typename   BASECLASS>
                int Led_MFC_DragAndDropWindow<BASECLASS>::OnCreate (LPCREATESTRUCT lpCreateStruct)
                {
                    if (inherited::OnCreate (lpCreateStruct) != 0) {
                        return -1;  // failed to create
                    }
                    if (not fDropTarget.Register (this)) {
                        /*
                         *  This can happen if the document associated with the view is not a COleDocument, for
                         *  example. Shouldn't prevent Led view from being used. D&D just won't work...
                         */
                        TRACE (_T ("Warning: failed to register window as drop target\n"));
                        return 0;   // treat this as OK - but warn above...
                    }
                    return 0;   // success
                }
                template    <typename   BASECLASS>
                bool    Led_MFC_DragAndDropWindow<BASECLASS>::IsADragSelect (Led_Point clickedWhere) const
                {
                    /*
                     *  This seems to be what most people do. But I think this algorithm should
                     *  be embellished, so we detect that the mouse is down for a while (a half second?)
                     *  and then melt the drag into a regular selection. Also, pay attention to keys
                     *  pressed etc as a hint of intent. But immediately return yes if we are moving
                     *  what we've selected.
                     *
                     *  For now though, just do it the simple way...
                     */
                    Led_Region  r;
                    GetSelectionWindowRegion (&r, GetSelectionStart (), GetSelectionEnd ());
                    return r.PtInRegion (AsCPoint (clickedWhere));
                }
                template    <typename   BASECLASS>
                void    Led_MFC_DragAndDropWindow<BASECLASS>::HandleDragSelect (UINT /*nFlags*/, CPoint oPoint)
                {
                    const float kTimeEnoughToRestoreSelection   =   0.3f;

                    if (GetStyle () & WS_DISABLED) {
                        return;
                    }
                    if (GetFocus () != this)    {
                        SetFocus ();
                    }

                    /*
                     *  Be sure to reset these so on a double click after what looked like a single
                     *  click we don't grab the old anchor (see spr#0438).
                     */
                    fMouseTrackingLastPoint = AsLedPoint (oPoint);
                    fDragAnchor = GetCharAtClickLocation (fMouseTrackingLastPoint);

                    Assert (sCurrentDragInfo == NULL);
                    sCurrentDragInfo = new LedStartDragAndDropContext (this);

                    sCurrentDragInfo->fOurDragStart = GetSelectionStart ();
                    sCurrentDragInfo->fOurDragEnd = GetSelectionEnd ();

                    try {
                        COleDataSource      dataSrc;
                        Led_MFCWriterDAndDFlavorPackage flavorPackage (&dataSrc);
                        ExternalizeFlavors (flavorPackage);

                        Led_Region  selectionRegion;
                        GetSelectionWindowRegion (&selectionRegion, GetSelectionStart (), GetSelectionEnd ());

                        float   startDragSelectAt   =   Led_GetTickCount ();    // Grab it after the ExternalizeFlavors call in case thats slow (SPR#1498).
                        DROPEFFECT  dropResult  =   DROPEFFECT_COPY;
                        if (not (GetStyle () & ES_READONLY)) {
                            // Assure we don't change read-only text.
                            dropResult |= DROPEFFECT_MOVE;
                        }
                        dropResult  =   dataSrc.DoDragDrop (dropResult);
#if     qDebug
                        {
                            if (GetStyle () & ES_READONLY) {
                                Assert (not (dropResult & DROPEFFECT_MOVE));
                            }
                        }
#endif
                        switch (dropResult) {
                            case    DROPEFFECT_NONE: {
                                    AssertNotNull (sCurrentDragInfo);
                                    if (Led_GetTickCount () - startDragSelectAt > kTimeEnoughToRestoreSelection) {
                                        /*
                                        *   SPR#1374 - on aborted drag - restore the selection to its original value, rather
                                        *   than setting it to an empty selection where the user had clicked.
                                        */
                                        SetSelection (sCurrentDragInfo->fOurDragStart, sCurrentDragInfo->fOurDragEnd);
                                    }
                                    else {
                                        // for a very short click - just treat it as a regular click - changing the
                                        // selection to an insertion point where the user clicked.
                                        size_t  ignored =   0;
                                        (void)ProcessSimpleClick (fMouseTrackingLastPoint, 1, false, &ignored);
                                    }
                                }
                                break;
                            case    DROPEFFECT_MOVE: {
                                    /*
                                     *  delete the original text on a MOVE
                                     */
                                    InteractiveModeUpdater  iuMode (*this);
                                    AssertNotNull (sCurrentDragInfo);
                                    UndoableContextHelper   undoContext (*this,
                                                                         sCurrentDragInfo->fWeRecievedDrop ? Led_SDK_String () : GetCommandNames ().fDragCommandName,
                                                                         sCurrentDragInfo->fOrigSelection.GetStart (),
                                                                         sCurrentDragInfo->fOrigSelection.GetEnd (),
                                                                         true
                                                                        );
                                    {
                                        TextInteractor::InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0, false, false, eDefaultUpdate);
                                    }
                                    undoContext.CommandComplete (sCurrentDragInfo->fOrigSelection.GetEnd ());
                                }
                                break;
                            default: {
                                    // we do nothing then - I think....
                                }
                        }

                        // must do here, even if NOT a MOVE - cuz we skipped this in the OnDrop code...
                        BreakInGroupedCommands ();
                    }
                    catch (...) {
                        delete sCurrentDragInfo;
                        sCurrentDragInfo = NULL;
                        throw;
                    }

                    delete sCurrentDragInfo;
                    sCurrentDragInfo = NULL;
                }
                template    <typename   BASECLASS>
                void    Led_MFC_DragAndDropWindow<BASECLASS>::HandleSharedDragOverUpdateSelCode ()
                {
                    CClientDC   dc (this);
                    if (fPrevDropEffect != DROPEFFECT_NONE) {
                        // erase previous focus rect
                        dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
                    }

                    size_t  newPos      =   GetCharAtClickLocation (AsLedPoint (fDragPoint));

                    // Auto-scroll if necessary...
                    if (newPos < GetMarkerPositionOfStartOfWindow ()) {
                        ScrollByIfRoom (-1, eImmediateUpdate);
                    }
                    else if (newPos > GetMarkerPositionOfEndOfWindow ()) {
                        ScrollByIfRoom (1, eImmediateUpdate);
                    }

                    if (fPrevDropEffect != DROPEFFECT_NONE) {
                        // draw new focus rect
                        dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
                    }
                }
                template    <typename   BASECLASS>
                DROPEFFECT  Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
                {
                    // We allow dropping any time - I think... - See page 444 of MSVC4.0 Visual C++ Tutorial for hints on how to check if we accept...
                    StartDragAutoscrollTimer ();
                    if (not GetObjectInfo (pDataObject, &fDragSize, &fDragOffset)) {
                        return DROPEFFECT_NONE;
                    }
                    CClientDC   dc (NULL);
                    dc.HIMETRICtoDP (&fDragSize);
                    dc.HIMETRICtoDP (&fDragOffset);
                    DROPEFFECT  result  =   HelperDragOver (pDataObject, dwKeyState, point);
                    return result;
                }
                template    <typename   BASECLASS>
                DROPEFFECT  Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragOver (COleDataObject* /*pDataObject*/, DWORD dwKeyState, CPoint point)
                {
                    point -= fDragOffset;   //  adjust target rect by cursor offset

                    // See  See page 444 of MSVC4.0 Visual C++ Tutorial for hints on how to check if we accept...

                    // now do draw of hilite...
                    DROPEFFECT  de  =   DROPEFFECT_NONE;

                    if ((dwKeyState & (MK_CONTROL | MK_SHIFT)) == (MK_CONTROL | MK_SHIFT)) {
                        de = DROPEFFECT_NONE;   // we don't support linking...
                    }
                    else if ((dwKeyState & MK_CONTROL) == MK_CONTROL) {
                        de = DROPEFFECT_COPY;
                    }
                    else if ((dwKeyState & MK_ALT) == MK_ALT) {
                        de = DROPEFFECT_MOVE;
                    }
                    else {
                        de = DROPEFFECT_MOVE;   // an unfortunate choice, but windows defaults to this apparently...
                    }

                    if (point != fDragPoint) {
                        // cursor has moved
                        CClientDC   dc (this);
                        if (fPrevDropEffect != DROPEFFECT_NONE) {
                            // erase previous focus rect
                            dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
                        }

                        size_t  newPos      =   GetCharAtClickLocation (AsLedPoint (point));

                        // Disalow dragging selection ontop of itself...
                        if (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this and (newPos >= sCurrentDragInfo->fOurDragStart and newPos <= sCurrentDragInfo->fOurDragEnd)) {
                            SetSelection (sCurrentDragInfo->fOurDragStart, sCurrentDragInfo->fOurDragEnd);
                            Update ();
                            fPrevDropEffect = DROPEFFECT_NONE;
                            return DROPEFFECT_NONE;
                        }

                        // either scroll, or adjust the selection...
                        if (newPos < GetMarkerPositionOfStartOfWindow ()) {
                            ScrollByIfRoom (-1, eImmediateUpdate);
                        }
                        else if (newPos > GetMarkerPositionOfEndOfWindow ()) {
                            ScrollByIfRoom (1, eImmediateUpdate);
                        }

                        if (newPos >= GetMarkerPositionOfStartOfWindow () and
                                newPos <= GetMarkerPositionOfEndOfWindow ()
                           ) {
                            SetSelection (newPos, newPos);
                            Update ();
                        }

                        fPrevDropEffect = de;
                        if (fPrevDropEffect != DROPEFFECT_NONE) {
                            // draw new focus rect
                            fDragPoint = point;
                            dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
                        }
                    }

                    return de;
                }
                template    <typename   BASECLASS>
                BOOL    Led_MFC_DragAndDropWindow<BASECLASS>::HelperDrop (COleDataObject* pDataObject, DROPEFFECT /*dropEffect*/, CPoint /*point*/)
                {
                    RequireNotNull (pDataObject);

                    // Cleanup focus rect...
                    HelperDragLeave ();

                    if (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) {
                        sCurrentDragInfo->fWeRecievedDrop = true;
                    }

#if     0
                    // debugging hack- leave in for now in case helpful to see whats in drop package...
                    // LGP 960502
                    {
                        pDataObject->BeginEnumFormats ();
                        FORMATETC   formatC;
                        while (pDataObject->GetNextFormat (&formatC)) {
                            char    buf[1024];
                            int     nChars  =   ::GetClipboardFormatName (formatC.cfFormat, buf, sizeof buf);
                            int     barf    =   1;  // set breakpoint here
                        }
                    }
#endif

                    CWaitCursor busy;

                    Led_MFCReaderDAndDFlavorPackage flavors (pDataObject);

                    bool            doSmartCNP  =   GetSmartCutAndPasteMode () and flavors.GetFlavorAvailable_TEXT ();
                    SmartCNPInfo    smartCNPInfo;
                    if (doSmartCNP) {
                        size_t          length      =   flavors.GetFlavorSize (kTEXTClipFormat);
                        Led_ClipFormat  textFormat  =   kTEXTClipFormat;
                        Memory::SmallStackBuffer<Led_tChar> buf (length);   // really could use smaller buffer
                        length = flavors.ReadFlavorData (textFormat, length, buf);
                        if (doSmartCNP) {
                            size_t  nTChars =   length / sizeof (Led_tChar);
                            if (nTChars > 0) {
                                nTChars--;  // on windows, the text buffer contains a trailing NUL-byte
                            }
                            doSmartCNP = LooksLikeSmartPastableText (buf, nTChars, &smartCNPInfo);
                        }
                    }

                    BreakInGroupedCommands ();

                    InteractiveModeUpdater  iuMode (*this);
                    size_t                  ucSelStart  =   (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) ?
                                                            sCurrentDragInfo->fOrigSelection.GetStart () : GetSelectionStart ()
                                                            ;
                    size_t                  ucSelEnd    =   (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) ?
                                                            sCurrentDragInfo->fOrigSelection.GetEnd () : GetSelectionEnd ()
                                                            ;
                    UndoableContextHelper   undoContext (
                        *this,
                        (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) ? GetCommandNames ().fDragNDropCommandName : GetCommandNames ().fDropCommandName,
                        GetSelectionStart (), GetSelectionEnd (),
                        ucSelStart, ucSelEnd,
                        false
                    );
                    {
                        try {
                            InternalizeBestFlavor (flavors);
                        }
                        catch (...) {
#if     _MFC_VER > 0x0710
                            // Known broken in 0x0421 and 0x0600 anx 0x700 (MSVC.Net), and 0x710 (MSVC.NET 2003)
#pragma message "See if MFC has fixed this bug yet"
#endif
                            // Cannot re-throw here, due to MFC bug in COleDropTarget::XDropTarget::Drop (), where
                            // it fails to do a RELEASE() call in case we throw here. Returning FALSE still signifies the drop fialed, but
                            // then the calling APP may not see a good message about what failed. Too bad.
                            return false;
                            throw;
                        }
                        if (doSmartCNP) {
                            OptionallyAddExtraSpaceForSmartCutAndPasteModeAdds (undoContext.GetUndoRegionStart (), smartCNPInfo);
                        }
                    }
                    undoContext.CommandComplete ();

                    // If WE INITED DRAG - don't do break here, do it in HandleDragSelect () so it can incorporate a DELETE TEXT on MOVE...
                    if (sCurrentDragInfo == NULL or sCurrentDragInfo->fLedMFCView != this) {
                        BreakInGroupedCommands ();
                    }

                    return true;
                }
                template    <typename   BASECLASS>
                void    Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragLeave ()
                {
                    StopDragAutoscrollTimer ();
                    if (fPrevDropEffect != DROPEFFECT_NONE) {
                        CClientDC   dc (this);
                        // erase previous focus rect
                        dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
                        fPrevDropEffect = DROPEFFECT_NONE;
                    }
                }
                template    <typename   BASECLASS>
                bool    Led_MFC_DragAndDropWindow<BASECLASS>::GetObjectInfo (COleDataObject* pDataObject, CSize* pSize, CSize* pOffset)
                {
                    RequireNotNull (pDataObject);
                    RequireNotNull (pSize);
                    RequireNotNull (pOffset);

                    static  CLIPFORMAT  kObjectDescriptorFormatTag  =   (CLIPFORMAT)::RegisterClipboardFormat (_T("Object Descriptor"));

                    HGLOBAL hObjDesc    =   pDataObject->GetGlobalData (kObjectDescriptorFormatTag);
                    if (hObjDesc == NULL) {
                        *pSize = CSize (0, 0);
                        *pOffset = CSize (0, 0);
                        return false;
                    }
                    else {
                        LPOBJECTDESCRIPTOR  pObjDesc    =   (LPOBJECTDESCRIPTOR)::GlobalLock (hObjDesc);
                        AssertNotNull (pObjDesc);
                        pSize->cx = (int)pObjDesc->sizel.cx;
                        pSize->cy = (int)pObjDesc->sizel.cy;
                        pOffset->cx = (int)pObjDesc->pointl.x;
                        pOffset->cy = (int)pObjDesc->pointl.y;
                        ::GlobalUnlock (hObjDesc);
                        ::GlobalFree (hObjDesc);
                        return true;
                    }
                }
                template    <typename   BASECLASS>
                void    Led_MFC_DragAndDropWindow<BASECLASS>::StartDragAutoscrollTimer ()
                {
                    Assert (fDragAutoScrollTimerID == 0);// not sure about this - just for debug sake??? - LGP 960530
                    if (fDragAutoScrollTimerID == 0) {
                        //const int kTimeout    =   25; // 25 milliseconds - update autoscroll every 1/40
                        const   int kTimeout    =   20; // 20 milliseconds - update autoscroll every 1/50
                        // second.
                        Verify (fDragAutoScrollTimerID = SetTimer (eAutoscrolling4DragTimerEventID, kTimeout, NULL));
                    }
                }
                template    <typename   BASECLASS>
                void    Led_MFC_DragAndDropWindow<BASECLASS>::StopDragAutoscrollTimer ()
                {
                    if (fDragAutoScrollTimerID != 0) {
                        Verify (KillTimer (eAutoscrolling4DragTimerEventID));
                        fDragAutoScrollTimerID = 0;
                    }
                }
                template    <typename   BASECLASS>
                void    Led_MFC_DragAndDropWindow<BASECLASS>::OnTimer (UINT nEventID)
                {
                    if (nEventID == eAutoscrolling4DragTimerEventID) {
                        HandleSharedDragOverUpdateSelCode ();
                    }
                    else {
                        inherited::OnTimer (nEventID);
                    }
                }






//  class   Led_MFC_CViewHelper<BASECLASS>
#define DoDeclare_Led_MFC_CViewHelper_MessageMap(MFC_BASE_CLASS)\
    BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_CViewHelper, MFC_BASE_CLASS, Led_MFC_CViewHelper<MFC_BASE_CLASS >::inherited)\
    ON_WM_PAINT()\
    ON_WM_LBUTTONDOWN()\
    ON_WM_CREATE    ()\
    ON_WM_VSCROLL   ()\
    ON_WM_HSCROLL   ()\
    END_MESSAGE_MAP()
                template    <typename   BASECLASS>
                Led_MFC_CViewHelper<BASECLASS>::Led_MFC_CViewHelper ():
                    inherited (),
                    // NB: CTOR for Led_TWIPS_Rect is TOP/LEFT/HEIGHT/WIDTH, so we set height/width to zero to get same TLBR.
#if     qTypedefOfTemplatedTypeCannotBeUsedAsCTORBug
                    fPrintMargins (Rect_Base<Led_TWIPS_Point, Led_TWIPS_Point> (Led_TWIPS (1440), Led_TWIPS (1440), Led_TWIPS (0), Led_TWIPS (0))),
#else
                    fPrintMargins (Led_TWIPS_Rect (Led_TWIPS (1440), Led_TWIPS (1440), Led_TWIPS (0), Led_TWIPS (0))),
#endif
                    fPrintInfo (NULL)
                {
                }
                template    <typename   BASECLASS>
                Led_MFC_CViewHelper<BASECLASS>::~Led_MFC_CViewHelper ()
                {
                    Assert (fPrintInfo == NULL);
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins
                @DESCRIPTION:   <p>Return the print margins. These are used by @'Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect'.
                            See also @'Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins'.
                */
                Led_TWIPS_Rect  Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins () const
                {
                    return fPrintMargins;
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins
                @DESCRIPTION:   <p>See also @'Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins'.
                */
                void    Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins (const Led_TWIPS_Rect& printMargins)
                {
                    fPrintMargins = printMargins;
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::OnPreparePrinting
                @DESCRIPTION:   Hook the MFC OnPreparePrinting () method to handle printing in the standard MFC fasion.
                     Don't call this directly.
                */
                BOOL    Led_MFC_CViewHelper<BASECLASS>::OnPreparePrinting (CPrintInfo* pInfo)
                {
                    // default preparation
                    return DoPreparePrinting (pInfo);
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::OnBeginPrinting
                @DESCRIPTION:   Hook the MFC OnBeginPrinting () method to handle printing in the standard MFC fasion.
                    Also, keep track of some internals we will use later in printing, and setup SetForceAllRowsShowing
                    so when we print the last page, we can see lots of nice whatspace at the end. Don't call this directly.
                */
                void    Led_MFC_CViewHelper<BASECLASS>::OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo)
                {
                    // THIS CODE IS KINDOF HACKISH - SHOULD HAVE A PAGENATE STAGE/FUNCTION!!! TO FILL THIS ARRAY!!!
                    Assert (fPrintInfo == NULL);
                    BASECLASS::OnBeginPrinting (pDC, pInfo);
                    fPrintInfo = new PrintInfo (*this, pDC, GetWindowRect (), GetMarkerPositionOfStartOfWindow (), GetForceAllRowsShowing ());
                    SetForceAllRowsShowing (false);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::OnPrint (CDC* pDC, CPrintInfo* /*pInfo*/)
                {
                    AssertNotNull (fPrintInfo);
                    // Consider different print loop - maybe differnt print-oriented entry in TextImager?
                    // That doesn't have todo with scrolling? Just DrawFromSize_tOffset? Maybe - into rect.
                    // Make the MRTI::Draw() vector to that?
                    ASSERT_VALID (pDC);
                    Led_MFC_TabletFromCDC   tablet (pDC);
                    TemporarilyUseTablet    tmpUseTablet (*this, tablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall);
                    CRect   boundsRect = CRect (0, 0, 0, 0);
                    Verify (pDC->GetClipBox (&boundsRect) != ERROR);
                    Draw (AsLedRect (boundsRect), true);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::OnEndPrinting (CDC* pDC, CPrintInfo* pInfo)
                {
                    if (fPrintInfo == NULL) {
                        // Can be NULL if print fails, cuz no printer installed
                        BASECLASS::OnEndPrinting (pDC, pInfo);
                    }
                    else {
                        SetForceAllRowsShowing (fPrintInfo->fSavedForceAllRowsFlag);
                        SetWindowRect (fPrintInfo->fOldWindowRect);
                        SetTopRowInWindowByMarkerPosition (fPrintInfo->fSavedScrollPos);
                        delete fPrintInfo;
                        fPrintInfo = NULL;
                        BASECLASS::OnEndPrinting (pDC, pInfo);
                    }
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::OnPrepareDC (CDC* pDC, CPrintInfo* pInfo)
                {
                    if (pInfo != NULL and pDC->IsPrinting ()) { // For some STUPID reason, MFC calls this even if we aren't printing...
                        {
                            Led_WindowDC    screenDC (NULL);
                            (void)pDC->SetMapMode (MM_ANISOTROPIC);
                            (void)pDC->SetWindowExt (screenDC.GetDeviceCaps (LOGPIXELSX), screenDC.GetDeviceCaps (LOGPIXELSY));
                            (void)pDC->SetViewportExt (pDC->GetDeviceCaps (LOGPIXELSX), pDC->GetDeviceCaps (LOGPIXELSY));
                            // I REMOVED calls to SetWindowRect/TabletChangedMetrics for WYSYWIG (SPR#0869),
                            // This created a new problem - SPR#1079 - which is now fixed (or largely worked around here).
                            // Probably there is still a real latent problem inside TabletChangedMetrics - but I dont
                            // need to debug that now, and this whole thing will go away if I rewrite the printing code
                            // after 3.0 - as is anticipated.
                            // I changed the args to CalculateWindowRect() to accomodate this bugfix.
                            //
                            Led_Rect    oldWR   =   GetWindowRect ();
                            Led_Rect    newWR   =   CalculatePrintingRect (pDC);
                            SetWindowRect (newWR);
                        }
                        CView::OnPrepareDC (pDC, pInfo);
                        if (fPrintInfo->fWindowStarts.size () == 0) {
                            fPrintInfo->fWindowStarts.push_back (0);
                            SetTopRowInWindowByMarkerPosition (0, eNoUpdate);
                        }
                        pInfo->m_bContinuePrinting = true;  // Assume scroll forward succeeded
                        // First page into our fWindowStarts cache enuf pages to satisfy this 'goto page' request. NB:
                        // In Print (page n..m) requests - this can be starting well past fWindowStarts.size () - contrary
                        // to earlier - incorrect - Led comments - LGP 981007
                        while (fPrintInfo->fWindowStarts.size () < pInfo->m_nCurPage) {
                            // advanced one page...
                            if (GetMarkerPositionOfEndOfWindow () == GetTextStore ().GetEnd ()) {
                                pInfo->m_bContinuePrinting = false; // scroll forward failed
                                return;
                            }
                            SetTopRowInWindowByMarkerPosition (FindNextCharacter (GetMarkerPositionOfEndOfWindow ()), eNoUpdate);
                            size_t  newStart    =   GetMarkerPositionOfStartOfWindow ();
                            if (newStart == fPrintInfo->fWindowStarts.back ()) {
                                pInfo->m_bContinuePrinting = false; // scroll forward failed
                                return;
                            }
                            else {
                                fPrintInfo->fWindowStarts.push_back (newStart);
                            }
                        }
                        // scroll forward/back to some page in our (now filled in cache) page list
                        SetTopRowInWindowByMarkerPosition (fPrintInfo->fWindowStarts[pInfo->m_nCurPage - 1]);
                        if (GetMarkerPositionOfEndOfWindow () == GetTextStore ().GetEnd ()) {
                            pInfo->SetMaxPage (pInfo->m_nCurPage);
                            return;
                        }
                    }
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect
                @DESCRIPTION:   Hook function to change the default size Led will use for printing. Defautlts to
                    FULL page. Override to inset for margins, and/or headers/footers, or call @'Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins'.
                        <p>Don't call directly. Just called after DC setup from @'Led_MFC_CViewHelper<BASECLASS>::OnPrepareDC'.</p>
                        <p>See also @'Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins' to specify the margins that are used by this
                    routine by default.</p>
                */
                Led_Rect    Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect (CDC* pDC) const
                {
                    RequireNotNull (pDC);
                    /*
                     *  See SPR#1079, and SPR#1153 about whats going on here.
                     */
                    CRect       winRect =   CRect (0, 0, pDC->GetDeviceCaps (HORZRES), pDC->GetDeviceCaps (VERTRES));
                    pDC->DPtoLP (&winRect);
                    Led_Rect    useRect     =   AsLedRect (winRect);
                    Led_Rect    marginRect  =   Led_MFC_TabletFromCDC (pDC)->CvtFromTWIPS (fPrintMargins);
                    useRect = Led_Rect (useRect.GetTop () + marginRect.GetTop (),
                                        useRect.GetLeft () + marginRect.GetLeft (),
                                        useRect.GetHeight () - (marginRect.GetTop () + marginRect.GetBottom ()),
                                        useRect.GetWidth () - (marginRect.GetLeft () + marginRect.GetRight ())
                                       );
                    return useRect;
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::InvalidateScrollBarParameters ()
                {
                    if (fPrintInfo == NULL) {
                        inherited::InvalidateScrollBarParameters ();
                    }
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::UpdateScrollBars
                @DESCRIPTION:   Avoid errors updating sbars while printing.
                */
                void    Led_MFC_CViewHelper<BASECLASS>::UpdateScrollBars ()
                {
                    if (fPrintInfo != NULL) {
                        return; // ignore while in print mode...
                    }
                    inherited::UpdateScrollBars ();
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::DeleteContents
                @DESCRIPTION:   Hook the MFC DeleteContents () routine, and simulate the user having deleted all the text in the
                    buffer.
                */
                void    Led_MFC_CViewHelper<BASECLASS>::DeleteContents ()
                {
                    ASSERT_VALID (this);
                    AssertNotNull (m_hWnd);
                    SetWindowText (NULL);
                    ASSERT_VALID (this);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::Serialize (CArchive& ar)
                {
                    // Probably not the right thing todo, but a simple enuf hack for now - LGP 950511
                    SerializeRaw (ar);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::SerializeRaw (CArchive& ar)
                {
                    ASSERT_VALID (this);
                    AssertNotNull (m_hWnd);
                    if (ar.IsStoring()) {
                        WriteToArchive (ar);
                    }
                    else {
                        CFile*  file    =   ar.GetFile ();
                        ASSERT_VALID (file);
                        DWORD   dwLen = static_cast<DWORD> (file->GetLength ());                // maybe should subtract current offset?
                        ReadFromArchive (ar, (UINT)dwLen);
                    }
                    ASSERT_VALID (this);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::ReadFromArchive (CArchive& ar, UINT nLen)
                {
                    ASSERT_VALID (this);

                    Memory::SmallStackBuffer<Led_tChar> buf (nLen);
                    if (ar.Read (buf, nLen * sizeof(Led_tChar)) != nLen * sizeof(Led_tChar)) {
                        AfxThrowArchiveException (CArchiveException::endOfFile);
                    }
                    // Replace the editing edit buffer with the newly loaded data
                    nLen = Led_NormalizeTextToNL (buf, nLen, buf, nLen);
                    if (ValidateTextForCharsetConformance (buf, nLen)) {
                        Replace (0, 0, buf, nLen);
                    }
                    else {
                        OnBadUserInput ();
                    }

                    Invalidate ();

                    ASSERT_VALID (this);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::WriteToArchive (CArchive& ar)
                {
                    ASSERT_VALID (this);

                    UINT nLen = GetLength();
                    Memory::SmallStackBuffer<Led_tChar> buf (nLen);
                    CopyOut (0, nLen, buf);
                    Memory::SmallStackBuffer<Led_tChar> buf2 (2 * nLen);
                    nLen = Led_NLToNative (buf, nLen, buf2, 2 * nLen);
                    ar.Write (buf2, nLen * sizeof(Led_tChar));

                    ASSERT_VALID (this);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::OnLButtonDown (UINT nFlags, CPoint oPoint)
                {
                    UpdateClickCount (Led_GetTickCount (), AsLedPoint (oPoint));

                    if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
                        // Any time we get a click, make sure there are no active in-place activations.
                        // Any click anyplace outside the active item is ingored, except for deactivating
                        // that item.
                        AssertMember (m_pDocument, COleDocument);   //
                        COleDocument&   doc     =   *(COleDocument*)m_pDocument;
                        COleClientItem* pItem   =   doc.GetInPlaceActiveItem (this);
                        if (pItem != NULL) {
                            pItem->Close ();
                            return;
                        }
                    }

                    if (GetCurClickCount () == 1 and IsADragSelect (AsLedPoint (oPoint))) {
                        HandleDragSelect (nFlags, oPoint);
                    }
                    else {
                        OnNormalLButtonDown (nFlags, AsLedPoint (oPoint));
                    }
                }
                template    <typename   BASECLASS>
                DROPEFFECT  Led_MFC_CViewHelper<BASECLASS>::OnDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
                {
                    return HelperDragEnter (pDataObject, dwKeyState, point);
                }
                template    <typename   BASECLASS>
                DROPEFFECT  Led_MFC_CViewHelper<BASECLASS>::OnDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
                {
                    return HelperDragOver (pDataObject, dwKeyState, point);
                }
                template    <typename   BASECLASS>
                BOOL    Led_MFC_CViewHelper<BASECLASS>::OnDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
                {
                    return HelperDrop (pDataObject, dropEffect, point);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::OnDragLeave ()
                {
                    HelperDragLeave ();
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::OnPaint ()
                {
                    CView::OnPaint ();  // don't use inherited/Led_MFC_CViewHelper<BASECLASS>_Helper<T> version - cuz misses some MFC hooks
                }
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::OnDraw
                @DESCRIPTION:   Hook the MFC OnDraw () method to invoke the Led drawing mechanism, and redisplay the window.
                */
                void    Led_MFC_CViewHelper<BASECLASS>::OnDraw (CDC* pDC)
                {
                    ASSERT_VALID (pDC);

                    /*
                     *  Since we currently use the same textimager for printing as for display, we will screw
                     *  up all our cached info for line breaks etc if we display while we are printing.
                     *  If it is desired to make this work then we must use a NEW imager (as with LedIt! PowerPlant)
                     *  to print to.
                     */
                    if (fPrintInfo != NULL) {
                        return;
                    }

                    CRect       boundsRect  =   CRect (0, 0, 0, 0);
                    Verify (pDC->GetClipBox (&boundsRect) != ERROR);
                    WindowDrawHelper (Led_MFC_TabletFromCDC (pDC), AsLedRect (boundsRect), false);
                }
                template    <typename   BASECLASS>
                int Led_MFC_CViewHelper<BASECLASS>::OnCreate (LPCREATESTRUCT lpCreateStruct)
                {
                    if (inherited::OnCreate (lpCreateStruct) != 0) {
                        return -1;  // failed to create
                    }
                    TabletChangedMetrics ();        // maybe should catch failures here, and return -1?
                    return 0;   // sucess
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
                {
                    if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
                        // Don't allow scrolling while there is an in-place active item.
                        // Wish we had a better interface for this... Maybe we should be deactivating
                        // SBARS when we activate item??? For now, interpret scroll attempt as a request
                        // to deactivate...
                        // LGP 960530
                        AssertMember (m_pDocument, COleDocument);
                        COleDocument&   doc     =   *(COleDocument*)m_pDocument;
                        COleClientItem* pItem   =   doc.GetInPlaceActiveItem (this);
                        if (pItem != NULL) {
                            pItem->Close ();
                            return;
                        }
                    }
                    inherited::OnVScroll (nSBCode, nPos, pScrollBar);
                }
                template    <typename   BASECLASS>
                void    Led_MFC_CViewHelper<BASECLASS>::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
                {
                    if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
                        // Don't allow scrolling while there is an in-place active item.
                        // Wish we had a better interface for this... Maybe we should be deactivating
                        // SBARS when we activate item??? For now, interpret scroll attempt as a request
                        // to deactivate...
                        // LGP 960530
                        AssertMember (m_pDocument, COleDocument);   //
                        COleDocument&   doc     =   *(COleDocument*)m_pDocument;
                        COleClientItem* pItem   =   doc.GetInPlaceActiveItem (this);
                        if (pItem != NULL) {
                            pItem->Close ();
                            return;
                        }
                    }
                    inherited::OnHScroll (nSBCode, nPos, pScrollBar);
                }







//  class   Led_MFC_ExceptionHandlerHelper<BASECLASS>
#define DoDeclare_Led_MFC_ExceptionHandlerHelper_MessageMap(MFC_BASE_CLASS)\
    BEGIN_TEMPLATE_MESSAGE_MAP(Led_MFC_ExceptionHandlerHelper, MFC_BASE_CLASS, MFC_BASE_CLASS)\
    ON_WM_CHAR  ()\
    ON_MESSAGE          (WM_IME_CHAR,   OnIMEChar)\
    ON_WM_KEYDOWN       ()\
    ON_MESSAGE          (WM_PASTE,      OnMsgPaste)\
    ON_WM_LBUTTONDOWN   ()\
    ON_WM_LBUTTONUP     ()\
    ON_WM_LBUTTONDBLCLK ()\
    END_MESSAGE_MAP()
                template    <typename   BASECLASS>
                /*
                @METHOD:        Led_MFC_ExceptionHandlerHelper<BASECLASS>::HandleException
                @DESCRIPTION:   Override this to provide different exception handling. By default, this calls @'Led_BeepNotify'.
                */
                void    Led_MFC_ExceptionHandlerHelper<BASECLASS>::HandleException () const
                {
                    Led_BeepNotify ();
                }
                template    <typename   BASECLASS>
                void    Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
                {
                    try {
                        inherited::OnChar (nChar, nRepCnt, nFlags);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template    <typename   BASECLASS>
                LRESULT Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnIMEChar (WPARAM wParam, LPARAM lParam)
                {
                    try {
                        return inherited::OnIMEChar (wParam, lParam);
                    }
                    catch (...) {
                        HandleException ();
                        return 0;
                    }
                }
                template    <typename   BASECLASS>
                void    Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
                {
                    try {
                        inherited::OnKeyDown (nChar, nRepCnt, nFlags);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template    <typename   BASECLASS>
                LRESULT Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnMsgPaste (WPARAM wParam, LPARAM lParam)
                {
                    try {
                        return inherited::OnMsgPaste (wParam, lParam);
                    }
                    catch (...) {
                        HandleException ();
                        return 0;
                    }
                }
                template    <typename   BASECLASS>
                void    Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonDown (UINT nFlags, CPoint oPoint)
                {
                    try {
                        inherited::OnLButtonDown (nFlags, oPoint);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template    <typename   BASECLASS>
                void    Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonUp (UINT nFlags, CPoint oPoint)
                {
                    try {
                        inherited::OnLButtonUp (nFlags, oPoint);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template    <typename   BASECLASS>
                void    Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonDblClk (UINT nFlags, CPoint oPoint)
                {
                    try {
                        inherited::OnLButtonDblClk (nFlags, oPoint);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template    <typename   BASECLASS>
                BOOL    Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
                {
                    try {
                        return inherited::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
                    }
                    catch (...) {
                        HandleException ();
                        return 1;
                    }
                }





//  class   Led_MFC_X<ChosenInteractor,LEDMFC>
                template    <typename   ChosenInteractor, typename LEDMFC>
                inline  Led_MFC_X<ChosenInteractor, LEDMFC>::Led_MFC_X ():
                    ChosenInteractor (),
                    LEDMFC ()
                {
                }
                /*
                @METHOD:        Led_MFC_X<ChosenInteractor,LEDMFC>::AboutToUpdateText
                @DESCRIPTION:   Override to handle mixin ambiguity.
                */
                template    <typename   ChosenInteractor, typename LEDMFC>
                void    Led_MFC_X<ChosenInteractor, LEDMFC>::AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo)
                {
                    LEDMFC::AboutToUpdateText (updateInfo);
                    ChosenInteractor::AboutToUpdateText (updateInfo);
                }
                /*
                @METHOD:        Led_MFC_X<ChosenInteractor,LEDMFC>::DidUpdateText
                @DESCRIPTION:   Override to handle mixin ambiguity.
                */
                template    <typename   ChosenInteractor, typename LEDMFC>
                void    Led_MFC_X<ChosenInteractor, LEDMFC>::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) throw ()
                {
                    LEDMFC::DidUpdateText_ (updateInfo);
                    ChosenInteractor::DidUpdateText (updateInfo);
                }










//  FUNCTION    Led_GetTextExtent ()
                /*
                @METHOD:        Led_GetTextExtent<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
                @DESCRIPTION:   Trivial wrapper on TrivialImager<TEXTSTORE,IMAGER> etc, except it handles
                    case of word wrapping as a parameter. So this is roughly a replacement for the Win32 SDK
                    routine GetTextExtent() - except its implemented by Led (and so UNICODE friendly, for example).
                        <p>Note - this is done as a template - rather than directly as a function - so as to avoid forcing
                    people who include Led_MFC from also including all these other modules required for this. There is
                    a global function version of this function (@'Led_GetTextExtent') which will be enabled/included in
                    your program if you define @'qSupportDrawTextGetTextExtent'.
                        <p>See also Led_DrawText<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
                */
                template    <typename   TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
                CSize   Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
                {
                    RequireNotNull (cdc);
                    Led_MFC_TabletFromCDC   tmpTablet (cdc);
                    return AsCSize (GetTextExtent<TRIVIALWORDWRAPPEDTEXTIMAGER, SIMPLETEXTIMAGER, TEXTSTORE> (tmpTablet, text, AsLedRect (r), wordWrap));
                }

//  FUNCTION    Led_DrawText ()
                /*
                @METHOD:        Led_DrawText<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
                @DESCRIPTION:   Trivial wrapper on TrivialImager<TEXTSTORE,IMAGER> etc, except it handles
                    case of word wrapping as a parameter. So this is roughly a replacement for the Win32 SDK
                    routine DrawText () - except its implemented by Led (and so UNICODE friendly, for example).
                        <p>Note - this is done as a template - rather than directly as a function - so as to avoid forcing
                    people who include Led_MFC from also including all these other modules required for this. There is
                    a global function version of this function (@'Led_DrawText') which will be enabled/included in
                    your program if you define @'qSupportDrawTextGetTextExtent'.
                        <p>See also Led_GetTextExtent<TRIVIALWORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>
                */
                template    <typename   TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
                void    Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
                {
                    RequireNotNull (cdc);
                    Led_MFC_TabletFromCDC   tmpTablet (cdc);
                    DrawTextBox<TRIVIALWORDWRAPPEDTEXTIMAGER, SIMPLETEXTIMAGER, TEXTSTORE> (tmpTablet, text, AsLedRect (r), wordWrap);
                }







//  class   Led_MFC_TmpCmdUpdater
                inline  Led_MFC_TmpCmdUpdater::Led_MFC_TmpCmdUpdater (CCmdUI* pCmdUI):
                    fCmdUI (pCmdUI),
                    fCmdNum (MFC_CommandNumberMapping::Get ().Lookup (pCmdUI->m_nID)),
                    fEnabled (false)
                {
                    RequireNotNull (fCmdUI);
                }
                inline  Led_MFC_TmpCmdUpdater::operator Led_MFC_TmpCmdUpdater* ()
                {
                    return this;
                }
                inline  Led_MFC_TmpCmdUpdater::operator CCmdUI* ()
                {
                    EnsureNotNull (fCmdUI);
                    return fCmdUI;
                }











            }
        }
    }
}

#if     qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning (pop)
#endif



#endif  /*_Stroika_Frameworks_Led_Platform_MFC_h_*/
