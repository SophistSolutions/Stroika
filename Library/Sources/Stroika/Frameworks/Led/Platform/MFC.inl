/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_MFC_inl_
#define _Stroika_Frameworks_Led_Platform_MFC_inl_ 1

#include "../../../Foundation/Characters/LineEndings.h"

namespace Stroika {
    namespace Frameworks {
        namespace Led {
            namespace Platform {

#if qLedCheckCompilerFlagsConsistency
                namespace LedCheckCompilerFlags_Led_MFC {
                    extern int LedCheckCompilerFlags_ (qMFCRequiresCWndLeftmostBaseClass);

                    struct FlagsChecker {
                        FlagsChecker ()
                        {
                            /*
                             *  See the docs on @'qLedCheckCompilerFlagsConsistency' if this ever fails.
                             */
                            if (LedCheckCompilerFlags_ (qMFCRequiresCWndLeftmostBaseClass) != qMFCRequiresCWndLeftmostBaseClass) {
                                abort ();
                            }
                        }
                    };
                    static struct FlagsChecker sFlagsChecker;
                }
#endif

                /*
                @METHOD:        AsCPoint
                @DESCRIPTION:   Convert a @'Led_Point' to an MFC CPoint.
                    <p>See also @'AsLedPoint', @'AsCRect', @'AsLedRect'.
                */
                inline CPoint AsCPoint (Led_Point p)
                {
                    return CPoint (p.h, p.v);
                }
                /*
                @METHOD:        AsLedPoint
                @DESCRIPTION:   Convert an MFC CPoint to a @'Led_Point'.
                    <p>See also @'AsCPoint', @'AsCRect', @'AsLedRect'.
                */
                inline Led_Point AsLedPoint (CPoint p)
                {
                    return Led_Point (p.y, p.x);
                }
                /*
                @METHOD:        AsCRect
                @DESCRIPTION:   Convert a Led_Rect to an MFC CRect.
                    <p>See also @'AsCPoint', @'AsLedPoint', @'AsLedRect'.
                */
                inline CRect AsCRect (Led_Rect r)
                {
                    return CRect (r.left, r.top, r.right, r.bottom);
                }
                /*
                @METHOD:        AsLedRect
                @DESCRIPTION:   Convert an MFC CRect to a Led_Rect.
                    <p>See also @'AsCPoint', @'AsLedPoint', @'AsCRect'.
                */
                inline Led_Rect AsLedRect (CRect r)
                {
                    return Led_Rect (r.top, r.left, r.Height (), r.Width ());
                }
                /*
                @METHOD:        AsCSize
                @DESCRIPTION:   Convert a Led_Size to an MFC CSize.
                */
                inline CSize AsCSize (Led_Size s)
                {
                    return CSize (s.h, s.v);
                }

                //  class   Led_MFC_CDCFromTablet
                inline Led_MFC_CDCFromTablet::Led_MFC_CDCFromTablet (Led_Tablet t)
                    : fCDC ()
                {
                    RequireNotNull (t);
                    fCDC.m_hDC       = t->m_hDC;
                    fCDC.m_hAttribDC = t->m_hAttribDC;
                    fCDC.m_bPrinting = t->m_bPrinting;
                }
                inline Led_MFC_CDCFromTablet::~Led_MFC_CDCFromTablet ()
                {
                    fCDC.Detach ();
                }
                inline Led_MFC_CDCFromTablet::operator CDC* ()
                {
                    return &fCDC;
                }

                //  class   Led_MFC_TabletFromCDC
                inline Led_MFC_TabletFromCDC::Led_MFC_TabletFromCDC (CDC* pDC)
                    : fTablet ()
                {
                    RequireNotNull (pDC);
                    fTablet.m_hDC       = pDC->m_hDC;
                    fTablet.m_hAttribDC = pDC->m_hAttribDC;
                    fTablet.m_bPrinting = pDC->m_bPrinting;
                }
                inline Led_MFC_TabletFromCDC::~Led_MFC_TabletFromCDC ()
                {
                    fTablet.Detach ();
                }
                inline Led_MFC_TabletFromCDC::operator Led_Tablet ()
                {
                    return &fTablet;
                }
                inline Led_Tablet Led_MFC_TabletFromCDC::operator-> ()
                {
                    return &fTablet;
                }

                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::Led_MFC_Helper ()
                    : LED_WIN32_HELPER ()
                    , MFC_BASE_CLASS ()
                {
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::~Led_MFC_Helper ()
                {
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                HWND Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetHWND () const
                {
                    return m_hWnd;
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                int Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnCreate (LPCREATESTRUCT lpCreateStruct)
                {
                    RequireNotNull (lpCreateStruct);
                    if (MFC_BASE_CLASS::OnCreate (lpCreateStruct) != 0) {
                        return -1; // failed to create
                    }
                    TabletChangedMetrics (); // maybe should catch failures here, and return -1?
                    if ((lpCreateStruct->style & WS_VSCROLL) and GetScrollBarType (v) == eScrollBarNever) {
                        SetScrollBarType (v, eScrollBarAlways);
                    }
                    if ((lpCreateStruct->style & WS_HSCROLL) and GetScrollBarType (h) == eScrollBarNever) {
                        SetScrollBarType (h, eScrollBarAlways);
                    }
                    return 0; // sucess
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnPaint ()
                {
                    CPaintDC dc (this);
                    CRect    boundsRect = CRect (0, 0, 0, 0);
                    Verify (dc.GetClipBox (&boundsRect) != ERROR);
                    WindowDrawHelper (Led_MFC_TabletFromCDC (&dc), AsLedRect (boundsRect), false);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                /*
                @METHOD:        Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::OnSetCursor
                @DESCRIPTION:   Hook the Win32 SDK WM_SETCURSOR message to handle set the cursor to an I-Beam, as appropriate. When over
                    draggable text, instead use a standard arrow cursor.
                */
                BOOL
                Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message)
                {
                    return OnSetCursor_Msg (pWnd->GetSafeHwnd (), nHitTest, message);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                UINT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnGetDlgCode ()
                {
                    return OnGetDlgCode_Msg ();
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
                {
                    OnChar_Msg (nChar, nRepCnt + (nFlags << 16));
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
                {
                    OnKeyDown_Msg (nChar, nRepCnt + (nFlags << 16));
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnMouseMove (UINT nFlags, CPoint oPoint)
                {
                    OnMouseMove_Msg (nFlags, oPoint.x, oPoint.y);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                /*
                @METHOD:        Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::ReplaceWindow
                @DESCRIPTION:   <p>Similar to @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::ReplaceWindow'
                            except that it works with MFC windows and MFC's CWnd message maps etc.
                            </p>
                */
                BOOL
                Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::ReplaceWindow (HWND hWnd)
                {
                    CWnd* parent     = MFC_BASE_CLASS::FromHandle (hWnd)->GetParent ();
                    HWND  hwndParent = parent->GetSafeHwnd ();
                    if (hwndParent == NULL) {
                        return false;
                    }

                    int id = ::GetWindowLong (hWnd, GWL_ID);

                    HWND hwndEdit = ::GetDlgItem (hwndParent, id);
                    AssertNotNull (hwndEdit);

                    DWORD dwStyle = ::GetWindowLong (hwndEdit, GWL_STYLE);
                    DWORD exStyle = ::GetWindowLong (hwndEdit, GWL_EXSTYLE);

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
                                     parent->GetSafeHwnd (), (HMENU)id, NULL);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                /*
                @METHOD:        Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>::GetWindowRect
                @DESCRIPTION:   Return the Led WindowRect. This name is a somewhat unfortunate choice for Windows, because WindowRect means something
                    vaguely similar, but substantially different in the Win32 SDK.
                        <p>In the future, I may need to consider changing this name. But for now, I can think of none better.
                        <p>I provide two overloads of this routine. The one that returns  a Led_Rect returns the Led WindowRect. And the one
                    that takes an LPRECT parameter returns the Win32 SDK WindowRect.
                */
                inline Led_Rect
                Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetWindowRect () const
                {
                    return (TextImager::GetWindowRect ());
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetWindowRect (LPRECT lpRect) const
                {
                    // The CWnd version of GetWindowRect ()
                    const CWnd* cwnd = this;
                    CWnd*       wnd  = const_cast<CWnd*> (cwnd);
                    wnd->GetWindowRect (lpRect);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline DWORD Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetStyle () const
                {
                    return MFC_BASE_CLASS::GetStyle ();
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnLButtonDown (UINT nFlags, CPoint oPoint)
                {
                    OnLButtonDown_Msg (nFlags, oPoint.x, oPoint.y);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnLButtonUp (UINT nFlags, CPoint oPoint)
                {
                    OnLButtonUp_Msg (nFlags, oPoint.x, oPoint.y);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnLButtonDblClk (UINT nFlags, CPoint oPoint)
                {
                    OnLButtonDblClk_Msg (nFlags, oPoint.x, oPoint.y);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnSetFocus (CWnd* pOldWnd)
                {
                    MFC_BASE_CLASS::OnSetFocus (pOldWnd);
                    OnSetFocus_Msg (pOldWnd->GetSafeHwnd ());
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnKillFocus (CWnd* pNewWnd)
                {
                    OnKillFocus_Msg (pNewWnd->GetSafeHwnd ());
                    MFC_BASE_CLASS::OnKillFocus (pNewWnd);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnSize (UINT nType, int cx, int cy)
                {
                    MFC_BASE_CLASS::OnSize (nType, cx, cy);
                    OnSize_Msg ();
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                BOOL Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnEraseBkgnd (CDC* pDC)
                {
                    return OnEraseBkgnd_Msg (pDC->GetSafeHdc ());
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
                {
                    OnVScroll_Msg (nSBCode, nPos, pScrollBar->GetSafeHwnd ());
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
                {
                    OnHScroll_Msg (nSBCode, nPos, pScrollBar->GetSafeHwnd ());
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                BOOL Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnMouseWheel (UINT fFlags, short zDelta, CPoint point)
                {
                    return OnMouseWheel_Msg (MAKEWPARAM (fFlags, zDelta), MAKELPARAM (point.x, point.y));
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnEnable (BOOL bEnable)
                {
                    OnEnable_Msg (!!bEnable);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnTimer (UINT_PTR nEventID)
                {
                    OnTimer_Msg (nEventID, NULL);
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline LRESULT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnUniChar ([[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
                {
#if qWideCharacters
                    return LED_WIN32_HELPER::OnUniChar_Msg (wParam, lParam);
#else
                    return MFC_BASE_CLASS::Default ();
#endif
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline LRESULT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnIMEChar ([[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
                {
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
                    return LED_WIN32_HELPER::OnIMEChar_Msg (wParam, lParam);
#else
                    return MFC_BASE_CLASS::Default ();
#endif
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline LRESULT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnIME_COMPOSITION ([[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
                {
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
                    return LED_WIN32_HELPER::OnIME_COMPOSITION_Msg (wParam, lParam);
#else
                    return MFC_BASE_CLASS::Default ();
#endif
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                inline LRESULT Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnIME_ENDCOMPOSITION ([[maybe_unused]] WPARAM wParam, [[maybe_unused]] LPARAM lParam)
                {
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
                    return LED_WIN32_HELPER::OnIME_ENDCOMPOSITION_Msg (wParam, lParam);
#else
                    return MFC_BASE_CLASS::Default ();
#endif
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnUpdateCommand_MSG (CCmdUI* pCmdUI)
                {
                    RequireNotNull (pCmdUI);
                    if (not OnUpdateCommand (Led_MFC_TmpCmdUpdater (pCmdUI))) {
                        pCmdUI->Enable (false); // disable commands by default (SPR#1462)
                    }
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                void Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::OnPerformCommand_MSG (UINT commandNumber)
                {
                    IdleManager::NonIdleContext nonIdleContext;
                    (void)OnPerformCommand (MFC_CommandNumberMapping::Get ().Lookup (commandNumber));
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                const AFX_MSGMAP* Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetMessageMap () const
                {
                    return GetThisMessageMap ();
                }
                template <typename MFC_BASE_CLASS, typename BASE_INTERACTOR>
                const AFX_MSGMAP* PASCAL Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>::GetThisMessageMap ()
                {
                    using ThisClass    = Led_MFC_Helper<MFC_BASE_CLASS, BASE_INTERACTOR>;
                    using TheBaseClass = MFC_BASE_CLASS;
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
                    static const AFX_MSGMAP_ENTRY _messageEntries[] = {
                        ON_WM_CREATE ()
                            ON_WM_CHAR ()
                                ON_WM_TIMER ()
                                    ON_WM_KEYDOWN ()
                                        ON_WM_PAINT ()
                                            ON_WM_MOUSEMOVE ()
                                                ON_WM_LBUTTONDOWN ()

                                                    ON_WM_SETCURSOR ()
                                                        ON_WM_GETDLGCODE ()
                                                            ON_WM_LBUTTONUP ()
                                                                ON_WM_LBUTTONDBLCLK ()
                                                                    ON_WM_SETFOCUS ()
                                                                        ON_WM_KILLFOCUS ()
                                                                            ON_WM_SIZE ()
                                                                                ON_WM_ERASEBKGND ()
                                                                                    ON_WM_VSCROLL ()
                                                                                        ON_WM_HSCROLL ()
                                                                                            ON_WM_MOUSEWHEEL ()
                                                                                                ON_WM_ENABLE ()
                                                                                                    ON_MESSAGE (WM_UNICHAR, &OnUniChar)
                                                                                                        ON_MESSAGE (WM_IME_CHAR, &OnIMEChar)
                                                                                                            ON_MESSAGE (WM_IME_COMPOSITION, &OnIME_COMPOSITION)
                                                                                                                ON_MESSAGE (WM_IME_ENDCOMPOSITION, &OnIME_ENDCOMPOSITION)

                                                                                                                    LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kUndo_CmdID)
                                                                                                                        LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kRedo_CmdID)
                                                                                                                            LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kSelectAll_CmdID)
                                                                                                                                LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kSelectWord_CmdID)
                                                                                                                                    LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kSelectTextRow_CmdID)
                                                                                                                                        LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kSelectParagraph_CmdID)
                                                                                                                                            LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kCut_CmdID)
                                                                                                                                                LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kCopy_CmdID)
                                                                                                                                                    LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kPaste_CmdID)
                                                                                                                                                        LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kClear_CmdID)
                                                                                                                                                            LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kFind_CmdID)
                                                                                                                                                                LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kFindAgain_CmdID)
                                                                                                                                                                    LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kEnterFindString_CmdID)
                                                                                                                                                                        LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kReplace_CmdID)
                                                                                                                                                                            LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kReplaceAgain_CmdID)
                                                                                                                                                                                LED_MFC_HANDLE_COMMAND_M (BASE_INTERACTOR::kSpellCheck_CmdID){0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0}};
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
                    static const AFX_MSGMAP messageMap = {&TheBaseClass::GetThisMessageMap, &_messageEntries[0]};
                    return &messageMap;
                }

                //  class   Led_MFC_MimicMFCAPIHelper<BASECLASS>
                template <typename BASECLASS>
                inline Led_MFC_MimicMFCAPIHelper<BASECLASS>::Led_MFC_MimicMFCAPIHelper ()
                {
                }
                template <typename BASECLASS>
                inline BOOL Led_MFC_MimicMFCAPIHelper<BASECLASS>::Create (DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
                {
                    return (BASECLASS::Create (NULL, NULL, dwStyle, rect, pParentWnd, nID));
                }
                template <typename BASECLASS>
                inline BOOL Led_MFC_MimicMFCAPIHelper<BASECLASS>::CanUndo () const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_CANUNDO, 0, 0);
                }
                template <typename BASECLASS>
                inline int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLineCount_CEdit () const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_GETLINECOUNT, 0, 0);
                }
                template <typename BASECLASS>
                inline BOOL Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetModify () const
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_GETMODIFY, 0, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetModify (BOOL bModified)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETMODIFY, bModified, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetRect (LPRECT lpRect) const
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetSel (int& nStartChar, int& nEndChar) const
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_GETSEL, (WPARAM)&nStartChar, (LPARAM)&nEndChar);
                }
                template <typename BASECLASS>
                inline DWORD Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetSel () const
                {
                    Assert (::IsWindow (m_hWnd));
                    return ::SendMessage (m_hWnd, EM_GETSEL, 0, 0);
                }
                template <typename BASECLASS>
                inline int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLine (int nIndex, LPTSTR lpszBuffer) const
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
                }
                template <typename BASECLASS>
                inline int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetLine (int nIndex, LPTSTR lpszBuffer, int nMaxLength) const
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    *(LPWORD)lpszBuffer = (WORD)nMaxLength;
                    return (int)::SendMessage (m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::EmptyUndoBuffer ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0);
                }
                template <typename BASECLASS>
                inline BOOL Led_MFC_MimicMFCAPIHelper<BASECLASS>::FmtLines (BOOL bAddEOL)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_FMTLINES, bAddEOL, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::LimitText (int nChars)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_LIMITTEXT, nChars, 0);
                }
                template <typename BASECLASS>
                inline int Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineFromChar (int nIndex) const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_LINEFROMCHAR, nIndex, 0);
                }
                template <typename BASECLASS>
                inline int Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineIndex (int nLine) const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_LINEINDEX, nLine, 0);
                }
                template <typename BASECLASS>
                inline int Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineLength (int nLine) const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_LINELENGTH, nLine, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::LineScroll (int nLines, int nChars)
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_LINESCROLL, nChars, nLines);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::ReplaceSel (LPCTSTR lpszNewText)
                {
                    AssertNotNull (lpszNewText);
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_REPLACESEL, 0, (LPARAM)lpszNewText);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetPasswordChar (TCHAR ch)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETPASSWORDCHAR, ch, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetRect (LPCRECT lpRect)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETRECT, 0, (LPARAM)lpRect);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetRectNP (LPCRECT lpRect)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETRECTNP, 0, (LPARAM)lpRect);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetSel (DWORD dwSelection, BOOL bNoScroll)
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETSEL, LOWORD (dwSelection), HIWORD (dwSelection));
                    if (not bNoScroll) {
                        ::SendMessage (m_hWnd, EM_SCROLLCARET, 0, 0);
                    }
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetSel (int nStartChar, int nEndChar, BOOL bNoScroll)
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, EM_SETSEL, nStartChar, nEndChar);
                    if (not bNoScroll) {
                        ::SendMessage (m_hWnd, EM_SCROLLCARET, 0, 0);
                    }
                }
                template <typename BASECLASS>
                inline BOOL Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops (int nTabStops, LPINT rgTabStops)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops ()
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    Verify (::SendMessage (m_hWnd, EM_SETTABSTOPS, 0, 0));
                }
                template <typename BASECLASS>
                inline BOOL Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetTabStops (const int& cxEachStop)
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_SETTABSTOPS, 1, (LPARAM) (LPINT)&cxEachStop);
                }
                template <typename BASECLASS>
                inline BOOL Led_MFC_MimicMFCAPIHelper<BASECLASS>::Undo ()
                {
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_UNDO, 0, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::Clear ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, WM_CLEAR, 0, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::Copy ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, WM_COPY, 0, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::Cut ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, WM_CUT, 0, 0);
                }
                template <typename BASECLASS>
                inline void Led_MFC_MimicMFCAPIHelper<BASECLASS>::Paste ()
                {
                    Assert (::IsWindow (m_hWnd));
                    ::SendMessage (m_hWnd, WM_PASTE, 0, 0);
                }
                template <typename BASECLASS>
                inline BOOL Led_MFC_MimicMFCAPIHelper<BASECLASS>::SetReadOnly (BOOL bReadOnly)
                {
                    Assert (::IsWindow (m_hWnd));
                    return (BOOL)::SendMessage (m_hWnd, EM_SETREADONLY, bReadOnly, 0L);
                }
                template <typename BASECLASS>
                inline int Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetFirstVisibleLine () const
                {
                    Assert (::IsWindow (m_hWnd));
                    return (int)::SendMessage (m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L);
                }
                template <typename BASECLASS>
                inline TCHAR Led_MFC_MimicMFCAPIHelper<BASECLASS>::GetPasswordChar () const
                {
                    Assert (false); // though THIS code is fine - we haven't yet hooked the callback, so this call will
                    // produce poor results... LGP 941129
                    Assert (::IsWindow (m_hWnd));
                    return (TCHAR)::SendMessage (m_hWnd, EM_GETPASSWORDCHAR, 0, 0L);
                }

                //  class   Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>
                template <typename BASECLASS>
                Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>::Led_MFC_OptionalWin32SDKMessageMimicHelper ()
                    : inherited ()
                {
                }
                template <typename BASECLASS>
                const AFX_MSGMAP* Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>::GetMessageMap () const
                {
                    return GetThisMessageMap ();
                }
                template <typename BASECLASS>
                const AFX_MSGMAP* PASCAL Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>::GetThisMessageMap ()
                {
                    using ThisClass    = Led_MFC_OptionalWin32SDKMessageMimicHelper<BASECLASS>;
                    using TheBaseClass = BASECLASS;
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
                    static const AFX_MSGMAP_ENTRY _messageEntries[] = {
                        ON_MESSAGE (WM_SETTEXT, &OnMFCSDKMessageDispatcher<WM_SETTEXT>)
                            ON_MESSAGE (WM_GETTEXT, &OnMFCSDKMessageDispatcher<WM_GETTEXT>)
                                ON_MESSAGE (WM_GETTEXTLENGTH, &OnMFCSDKMessageDispatcher<WM_GETTEXTLENGTH>)
                                    ON_MESSAGE (EM_GETSEL, &OnMFCSDKMessageDispatcher<EM_GETSEL>)
                                        ON_MESSAGE (EM_SETREADONLY, &OnMFCSDKMessageDispatcher<EM_SETREADONLY>)
                                            ON_MESSAGE (EM_GETFIRSTVISIBLELINE, &OnMFCSDKMessageDispatcher<EM_GETFIRSTVISIBLELINE>)
                                                ON_MESSAGE (EM_LINEINDEX, &OnMFCSDKMessageDispatcher<EM_LINEINDEX>)
                                                    ON_MESSAGE (EM_GETLINECOUNT, &OnMFCSDKMessageDispatcher<EM_GETLINECOUNT>)
                                                        ON_MESSAGE (EM_CANUNDO, &OnMFCSDKMessageDispatcher<EM_CANUNDO>)
                                                            ON_MESSAGE (EM_UNDO, &OnMFCSDKMessageDispatcher<EM_UNDO>)
                                                                ON_MESSAGE (EM_EMPTYUNDOBUFFER, &OnMFCSDKMessageDispatcher<EM_EMPTYUNDOBUFFER>)
                                                                    ON_MESSAGE (WM_CLEAR, &OnMFCSDKMessageDispatcher<WM_CLEAR>)
                                                                        ON_MESSAGE (WM_CUT, &OnMFCSDKMessageDispatcher<WM_CUT>)
                                                                            ON_MESSAGE (WM_COPY, &OnMFCSDKMessageDispatcher<WM_COPY>)
                                                                                ON_MESSAGE (WM_PASTE, &OnMFCSDKMessageDispatcher<WM_PASTE>)
                                                                                    ON_MESSAGE (EM_LINEFROMCHAR, &OnMFCSDKMessageDispatcher<EM_LINEFROMCHAR>)
                                                                                        ON_MESSAGE (EM_LINELENGTH, &OnMFCSDKMessageDispatcher<EM_LINELENGTH>)
                                                                                            ON_MESSAGE (EM_LINESCROLL, &OnMFCSDKMessageDispatcher<EM_LINESCROLL>)
                                                                                                ON_MESSAGE (EM_REPLACESEL, &OnMFCSDKMessageDispatcher<EM_REPLACESEL>)
                                                                                                    ON_MESSAGE (EM_SETSEL, &OnMFCSDKMessageDispatcher<EM_SETSEL>)
                                                                                                        ON_MESSAGE (EM_SCROLLCARET, &OnMFCSDKMessageDispatcher<EM_SCROLLCARET>)
                                                                                                            ON_MESSAGE (WM_GETFONT, &OnMFCSDKMessageDispatcher<WM_GETFONT>)
                                                                                                                ON_MESSAGE (WM_SETFONT, &OnMFCSDKMessageDispatcher<WM_SETFONT>){0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0}};
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
                    static const AFX_MSGMAP messageMap = {&TheBaseClass::GetThisMessageMap, &_messageEntries[0]};
                    return &messageMap;
                }

                //  class   Led_MFC_DragAndDropWindow<BASECLASS>
                template <typename BASECLASS>
                typename Led_MFC_DragAndDropWindow<BASECLASS>::LedStartDragAndDropContext* Led_MFC_DragAndDropWindow<BASECLASS>::sCurrentDragInfo = NULL;
                template <typename BASECLASS>
                Led_MFC_DragAndDropWindow<BASECLASS>::Led_MFC_DragAndDropWindow ()
                    : inherited ()
                    , fDropTarget ()
                    , fDragPoint ()
                    , fDragSize ()
                    , fDragOffset ()
                    , fPrevDropEffect (DROPEFFECT_NONE)
                    , fDragAutoScrollTimerID (0)
                {
                }
                template <typename BASECLASS>
                Led_MFC_DragAndDropWindow<BASECLASS>::~Led_MFC_DragAndDropWindow ()
                {
                    Assert (fDragAutoScrollTimerID == 0);
                }
                template <typename BASECLASS>
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
                inline typename const Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames&
                Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames ()
                {
                    return sCommandNames;
                }
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_DragAndDropWindow<BASECLASS>::SetCommandNames
                @DESCRIPTION:   See @'Led_MFC_DragAndDropWindow<BASECLASS>::GetCommandNames'.
                */
                inline void
                Led_MFC_DragAndDropWindow<BASECLASS>::SetCommandNames (const typename Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames& cmdNames)
                {
                    sCommandNames = cmdNames;
                }
                template <typename BASECLASS>
                typename Led_MFC_DragAndDropWindow<BASECLASS>::CommandNames Led_MFC_DragAndDropWindow<BASECLASS>::sCommandNames;
                template <typename BASECLASS>
                int Led_MFC_DragAndDropWindow<BASECLASS>::OnCreate (LPCREATESTRUCT lpCreateStruct)
                {
                    if (inherited::OnCreate (lpCreateStruct) != 0) {
                        return -1; // failed to create
                    }
                    if (not fDropTarget.Register (this)) {
                        /*
                         *  This can happen if the document associated with the view is not a COleDocument, for
                         *  example. Shouldn't prevent Led view from being used. D&D just won't work...
                         */
                        TRACE (_T ("Warning: failed to register window as drop target\n"));
                        return 0; // treat this as OK - but warn above...
                    }
                    return 0; // success
                }
                template <typename BASECLASS>
                bool Led_MFC_DragAndDropWindow<BASECLASS>::IsADragSelect (Led_Point clickedWhere) const
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
                    Led_Region r;
                    GetSelectionWindowRegion (&r, GetSelectionStart (), GetSelectionEnd ());
                    return !!r.PtInRegion (AsCPoint (clickedWhere));
                }
                template <typename BASECLASS>
                void Led_MFC_DragAndDropWindow<BASECLASS>::HandleDragSelect (UINT /*nFlags*/, CPoint oPoint)
                {
                    const Foundation::Time::DurationSecondsType kTimeEnoughToRestoreSelection = 0.3f;

                    if (GetStyle () & WS_DISABLED) {
                        return;
                    }
                    if (GetFocus () != this) {
                        SetFocus ();
                    }

                    /*
                     *  Be sure to reset these so on a double click after what looked like a single
                     *  click we don't grab the old anchor (see spr#0438).
                     */
                    fMouseTrackingLastPoint = AsLedPoint (oPoint);
                    fDragAnchor             = GetCharAtClickLocation (fMouseTrackingLastPoint);

                    Assert (sCurrentDragInfo == NULL);
                    sCurrentDragInfo = new LedStartDragAndDropContext (this);

                    sCurrentDragInfo->fOurDragStart = GetSelectionStart ();
                    sCurrentDragInfo->fOurDragEnd   = GetSelectionEnd ();

                    try {
                        COleDataSource                  dataSrc;
                        Led_MFCWriterDAndDFlavorPackage flavorPackage (&dataSrc);
                        ExternalizeFlavors (flavorPackage);

                        Led_Region selectionRegion;
                        GetSelectionWindowRegion (&selectionRegion, GetSelectionStart (), GetSelectionEnd ());

                        Foundation::Time::DurationSecondsType startDragSelectAt = Time::GetTickCount (); // Grab it after the ExternalizeFlavors call in case thats slow (SPR#1498).
                        DROPEFFECT                            dropResult        = DROPEFFECT_COPY;
                        if (not(GetStyle () & ES_READONLY)) {
                            // Assure we don't change read-only text.
                            dropResult |= DROPEFFECT_MOVE;
                        }
                        dropResult = dataSrc.DoDragDrop (dropResult);
#if qDebug
                        {
                            if (GetStyle () & ES_READONLY) {
                                Assert (not(dropResult & DROPEFFECT_MOVE));
                            }
                        }
#endif
                        switch (dropResult) {
                            case DROPEFFECT_NONE: {
                                AssertNotNull (sCurrentDragInfo);
                                if (Time::GetTickCount () - startDragSelectAt > kTimeEnoughToRestoreSelection) {
                                    /*
                                        *   SPR#1374 - on aborted drag - restore the selection to its original value, rather
                                        *   than setting it to an empty selection where the user had clicked.
                                        */
                                    SetSelection (sCurrentDragInfo->fOurDragStart, sCurrentDragInfo->fOurDragEnd);
                                }
                                else {
                                    // for a very short click - just treat it as a regular click - changing the
                                    // selection to an insertion point where the user clicked.
                                    size_t ignored = 0;
                                    (void)ProcessSimpleClick (fMouseTrackingLastPoint, 1, false, &ignored);
                                }
                            } break;
                            case DROPEFFECT_MOVE: {
                                /*
                                     *  delete the original text on a MOVE
                                     */
                                InteractiveModeUpdater iuMode (*this);
                                AssertNotNull (sCurrentDragInfo);
                                UndoableContextHelper undoContext (*this,
                                                                   sCurrentDragInfo->fWeRecievedDrop ? Led_SDK_String () : GetCommandNames ().fDragCommandName,
                                                                   sCurrentDragInfo->fOrigSelection.GetStart (),
                                                                   sCurrentDragInfo->fOrigSelection.GetEnd (),
                                                                   true);
                                {
                                    TextInteractor::InteractiveReplace_ (undoContext.GetUndoRegionStart (), undoContext.GetUndoRegionEnd (), LED_TCHAR_OF (""), 0, false, false, eDefaultUpdate);
                                }
                                undoContext.CommandComplete (sCurrentDragInfo->fOrigSelection.GetEnd ());
                            } break;
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
                template <typename BASECLASS>
                void Led_MFC_DragAndDropWindow<BASECLASS>::HandleSharedDragOverUpdateSelCode ()
                {
                    CClientDC dc (this);
                    if (fPrevDropEffect != DROPEFFECT_NONE) {
                        // erase previous focus rect
                        dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
                    }

                    size_t newPos = GetCharAtClickLocation (AsLedPoint (fDragPoint));

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
                template <typename BASECLASS>
                DROPEFFECT Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
                {
                    // We allow dropping any time - I think... - See page 444 of MSVC4.0 Visual C++ Tutorial for hints on how to check if we accept...
                    StartDragAutoscrollTimer ();
                    if (not GetObjectInfo (pDataObject, &fDragSize, &fDragOffset)) {
                        return DROPEFFECT_NONE;
                    }
                    CClientDC dc (NULL);
                    dc.HIMETRICtoDP (&fDragSize);
                    dc.HIMETRICtoDP (&fDragOffset);
                    DROPEFFECT result = HelperDragOver (pDataObject, dwKeyState, point);
                    return result;
                }
                template <typename BASECLASS>
                DROPEFFECT Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragOver (COleDataObject* /*pDataObject*/, DWORD dwKeyState, CPoint point)
                {
                    point -= fDragOffset; //  adjust target rect by cursor offset

                    // See  See page 444 of MSVC4.0 Visual C++ Tutorial for hints on how to check if we accept...

                    // now do draw of hilite...
                    DROPEFFECT de = DROPEFFECT_NONE;

                    if ((dwKeyState & (MK_CONTROL | MK_SHIFT)) == (MK_CONTROL | MK_SHIFT)) {
                        de = DROPEFFECT_NONE; // we don't support linking...
                    }
                    else if ((dwKeyState & MK_CONTROL) == MK_CONTROL) {
                        de = DROPEFFECT_COPY;
                    }
                    else if ((dwKeyState & MK_ALT) == MK_ALT) {
                        de = DROPEFFECT_MOVE;
                    }
                    else {
                        de = DROPEFFECT_MOVE; // an unfortunate choice, but windows defaults to this apparently...
                    }

                    if (point != fDragPoint) {
                        // cursor has moved
                        CClientDC dc (this);
                        if (fPrevDropEffect != DROPEFFECT_NONE) {
                            // erase previous focus rect
                            dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
                        }

                        size_t newPos = GetCharAtClickLocation (AsLedPoint (point));

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
                            newPos <= GetMarkerPositionOfEndOfWindow ()) {
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
                template <typename BASECLASS>
                BOOL Led_MFC_DragAndDropWindow<BASECLASS>::HelperDrop (COleDataObject* pDataObject, DROPEFFECT /*dropEffect*/, CPoint /*point*/)
                {
                    RequireNotNull (pDataObject);

                    // Cleanup focus rect...
                    HelperDragLeave ();

                    if (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) {
                        sCurrentDragInfo->fWeRecievedDrop = true;
                    }

#if 0
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

                    bool         doSmartCNP = GetSmartCutAndPasteMode () and flavors.GetFlavorAvailable_TEXT ();
                    SmartCNPInfo smartCNPInfo;
                    if (doSmartCNP) {
                        size_t                              length     = flavors.GetFlavorSize (kTEXTClipFormat);
                        Led_ClipFormat                      textFormat = kTEXTClipFormat;
                        Memory::SmallStackBuffer<Led_tChar> buf (length); // really could use smaller buffer
                        length = flavors.ReadFlavorData (textFormat, length, buf);
                        if (doSmartCNP) {
                            size_t nTChars = length / sizeof (Led_tChar);
                            if (nTChars > 0) {
                                nTChars--; // on windows, the text buffer contains a trailing NUL-byte
                            }
                            doSmartCNP = LooksLikeSmartPastableText (buf, nTChars, &smartCNPInfo);
                        }
                    }

                    BreakInGroupedCommands ();

                    InteractiveModeUpdater iuMode (*this);
                    size_t                 ucSelStart = (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) ? sCurrentDragInfo->fOrigSelection.GetStart () : GetSelectionStart ();
                    size_t                 ucSelEnd   = (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) ? sCurrentDragInfo->fOrigSelection.GetEnd () : GetSelectionEnd ();
                    UndoableContextHelper  undoContext (
                        *this,
                        (sCurrentDragInfo != NULL and sCurrentDragInfo->fLedMFCView == this) ? GetCommandNames ().fDragNDropCommandName : GetCommandNames ().fDropCommandName,
                        GetSelectionStart (), GetSelectionEnd (),
                        ucSelStart, ucSelEnd,
                        false);
                    {
                        try {
                            InternalizeBestFlavor (flavors);
                        }
                        catch (...) {
#if _MFC_VER > 0x0E00
// Known broken in 0x0421 and 0x0600 anx 0x700 (MSVC.Net), and 0x710 (MSVC.NET 2003), and 0x0A00 (MSVC.net 2010),
// and 0x0B00 = Microsoft Foundation Classes version 11.00
// and 0x0C00 = Microsoft Foundation Classes version 12.00
// and 0x0E00 = MFC included with Visual Studio.net 2017
#pragma message("See if MFC has fixed this bug yet")
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
                template <typename BASECLASS>
                void Led_MFC_DragAndDropWindow<BASECLASS>::HelperDragLeave ()
                {
                    StopDragAutoscrollTimer ();
                    if (fPrevDropEffect != DROPEFFECT_NONE) {
                        CClientDC dc (this);
                        // erase previous focus rect
                        dc.DrawFocusRect (CRect (fDragPoint, fDragSize));
                        fPrevDropEffect = DROPEFFECT_NONE;
                    }
                }
                template <typename BASECLASS>
                bool Led_MFC_DragAndDropWindow<BASECLASS>::GetObjectInfo (COleDataObject* pDataObject, CSize* pSize, CSize* pOffset)
                {
                    RequireNotNull (pDataObject);
                    RequireNotNull (pSize);
                    RequireNotNull (pOffset);

                    static CLIPFORMAT kObjectDescriptorFormatTag = (CLIPFORMAT)::RegisterClipboardFormat (_T("Object Descriptor"));

                    HGLOBAL hObjDesc = pDataObject->GetGlobalData (kObjectDescriptorFormatTag);
                    if (hObjDesc == NULL) {
                        *pSize   = CSize (0, 0);
                        *pOffset = CSize (0, 0);
                        return false;
                    }
                    else {
                        LPOBJECTDESCRIPTOR pObjDesc = (LPOBJECTDESCRIPTOR)::GlobalLock (hObjDesc);
                        if (pObjDesc == nullptr) {
                            memset (pSize, 0, sizeof (*pSize));
                            memset (pOffset, 0, sizeof (*pOffset));
                        }
                        else {
                            pSize->cx   = (int)pObjDesc->sizel.cx;
                            pSize->cy   = (int)pObjDesc->sizel.cy;
                            pOffset->cx = (int)pObjDesc->pointl.x;
                            pOffset->cy = (int)pObjDesc->pointl.y;
                        }
                        ::GlobalUnlock (hObjDesc);
                        ::GlobalFree (hObjDesc);
                        return true;
                    }
                }
                template <typename BASECLASS>
                void Led_MFC_DragAndDropWindow<BASECLASS>::StartDragAutoscrollTimer ()
                {
                    Assert (fDragAutoScrollTimerID == 0); // not sure about this - just for debug sake??? - LGP 960530
                    if (fDragAutoScrollTimerID == 0) {
                        //const int kTimeout    =   25; // 25 milliseconds - update autoscroll every 1/40
                        const int kTimeout = 20; // 20 milliseconds - update autoscroll every 1/50
                        // second.
                        Verify ((fDragAutoScrollTimerID = SetTimer (eAutoscrolling4DragTimerEventID, kTimeout, NULL)) != 0);
                    }
                }
                template <typename BASECLASS>
                void Led_MFC_DragAndDropWindow<BASECLASS>::StopDragAutoscrollTimer ()
                {
                    if (fDragAutoScrollTimerID != 0) {
                        Verify (KillTimer (eAutoscrolling4DragTimerEventID));
                        fDragAutoScrollTimerID = 0;
                    }
                }
                template <typename BASECLASS>
                void Led_MFC_DragAndDropWindow<BASECLASS>::OnTimer (UINT_PTR nEventID)
                {
                    if (nEventID == eAutoscrolling4DragTimerEventID) {
                        HandleSharedDragOverUpdateSelCode ();
                    }
                    else {
                        inherited::OnTimer (nEventID);
                    }
                }
                template <typename BASECLASS>
                const AFX_MSGMAP* Led_MFC_DragAndDropWindow<BASECLASS>::GetMessageMap () const
                {
                    return GetThisMessageMap ();
                }
                template <typename BASECLASS>
                const AFX_MSGMAP* PASCAL Led_MFC_DragAndDropWindow<BASECLASS>::GetThisMessageMap ()
                {
                    using ThisClass    = Led_MFC_DragAndDropWindow<BASECLASS>;
                    using TheBaseClass = BASECLASS;
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
                    static const AFX_MSGMAP_ENTRY _messageEntries[] = {
                        ON_WM_CREATE ()
                            ON_WM_TIMER (){0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0}};
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
                    static const AFX_MSGMAP messageMap = {&TheBaseClass::GetThisMessageMap, &_messageEntries[0]};
                    return &messageMap;
                }

                //  class   Led_MFC_CViewHelper<BASECLASS>
                template <typename BASECLASS>
                Led_MFC_CViewHelper<BASECLASS>::Led_MFC_CViewHelper ()
                    : inherited ()
                    ,
                    // NB: CTOR for Led_TWIPS_Rect is TOP/LEFT/HEIGHT/WIDTH, so we set height/width to zero to get same TLBR.
                    fPrintMargins (Led_TWIPS_Rect (Led_TWIPS (1440), Led_TWIPS (1440), Led_TWIPS (0), Led_TWIPS (0)))
                    , fPrintInfo (NULL)
                {
                }
                template <typename BASECLASS>
                Led_MFC_CViewHelper<BASECLASS>::~Led_MFC_CViewHelper ()
                {
                    Assert (fPrintInfo == NULL);
                }
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins
                @DESCRIPTION:   <p>Return the print margins. These are used by @'Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect'.
                            See also @'Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins'.
                */
                Led_TWIPS_Rect
                Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins () const
                {
                    return fPrintMargins;
                }
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins
                @DESCRIPTION:   <p>See also @'Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins'.
                */
                void
                Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins (const Led_TWIPS_Rect& printMargins)
                {
                    fPrintMargins = printMargins;
                }
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::OnPreparePrinting
                @DESCRIPTION:   Hook the MFC OnPreparePrinting () method to handle printing in the standard MFC fasion.
                     Don't call this directly.
                */
                BOOL
                Led_MFC_CViewHelper<BASECLASS>::OnPreparePrinting (CPrintInfo* pInfo)
                {
                    // default preparation
                    return DoPreparePrinting (pInfo);
                }
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::OnBeginPrinting
                @DESCRIPTION:   Hook the MFC OnBeginPrinting () method to handle printing in the standard MFC fasion.
                    Also, keep track of some internals we will use later in printing, and setup SetForceAllRowsShowing
                    so when we print the last page, we can see lots of nice whatspace at the end. Don't call this directly.
                */
                void
                Led_MFC_CViewHelper<BASECLASS>::OnBeginPrinting (CDC* pDC, CPrintInfo* pInfo)
                {
                    // THIS CODE IS KINDOF HACKISH - SHOULD HAVE A PAGENATE STAGE/FUNCTION!!! TO FILL THIS ARRAY!!!
                    Assert (fPrintInfo == NULL);
                    BASECLASS::OnBeginPrinting (pDC, pInfo);
                    fPrintInfo = new PrintInfo (*this, pDC, GetWindowRect (), GetMarkerPositionOfStartOfWindow (), GetForceAllRowsShowing ());
                    SetForceAllRowsShowing (false);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::OnPrint (CDC* pDC, CPrintInfo* /*pInfo*/)
                {
                    AssertNotNull (fPrintInfo);
                    // Consider different print loop - maybe different print-oriented entry in TextImager?
                    // That doesn't have todo with scrolling? Just DrawFromSize_tOffset? Maybe - into rect.
                    // Make the MRTI::Draw() vector to that?
                    ASSERT_VALID (pDC);
                    Led_MFC_TabletFromCDC tablet (pDC);
                    TemporarilyUseTablet  tmpUseTablet (*this, tablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall);
                    CRect                 boundsRect = CRect (0, 0, 0, 0);
                    Verify (pDC->GetClipBox (&boundsRect) != ERROR);
                    Draw (AsLedRect (boundsRect), true);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::OnEndPrinting (CDC* pDC, CPrintInfo* pInfo)
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
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::OnPrepareDC (CDC* pDC, CPrintInfo* pInfo)
                {
                    if (pInfo != NULL and pDC->IsPrinting ()) { // For some STUPID reason, MFC calls this even if we aren't printing...
                        {
                            Led_WindowDC screenDC (NULL);
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
                            Led_Rect oldWR = GetWindowRect ();
                            Led_Rect newWR = CalculatePrintingRect (pDC);
                            SetWindowRect (newWR);
                        }
                        CView::OnPrepareDC (pDC, pInfo);
                        if (fPrintInfo->fWindowStarts.size () == 0) {
                            fPrintInfo->fWindowStarts.push_back (0);
                            SetTopRowInWindowByMarkerPosition (0, eNoUpdate);
                        }
                        pInfo->m_bContinuePrinting = true; // Assume scroll forward succeeded
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
                            size_t newStart = GetMarkerPositionOfStartOfWindow ();
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
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect
                @DESCRIPTION:   Hook function to change the default size Led will use for printing. Defautlts to
                    FULL page. Override to inset for margins, and/or headers/footers, or call @'Led_MFC_CViewHelper<BASECLASS>::SetPrintMargins'.
                        <p>Don't call directly. Just called after DC setup from @'Led_MFC_CViewHelper<BASECLASS>::OnPrepareDC'.</p>
                        <p>See also @'Led_MFC_CViewHelper<BASECLASS>::GetPrintMargins' to specify the margins that are used by this
                    routine by default.</p>
                */
                Led_Rect
                Led_MFC_CViewHelper<BASECLASS>::CalculatePrintingRect (CDC* pDC) const
                {
                    RequireNotNull (pDC);
                    /*
                     *  See SPR#1079, and SPR#1153 about whats going on here.
                     */
                    CRect winRect = CRect (0, 0, pDC->GetDeviceCaps (HORZRES), pDC->GetDeviceCaps (VERTRES));
                    pDC->DPtoLP (&winRect);
                    Led_Rect useRect    = AsLedRect (winRect);
                    Led_Rect marginRect = Led_MFC_TabletFromCDC (pDC)->CvtFromTWIPS (fPrintMargins);
                    useRect             = Led_Rect (useRect.GetTop () + marginRect.GetTop (),
                                        useRect.GetLeft () + marginRect.GetLeft (),
                                        useRect.GetHeight () - (marginRect.GetTop () + marginRect.GetBottom ()),
                                        useRect.GetWidth () - (marginRect.GetLeft () + marginRect.GetRight ()));
                    return useRect;
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::InvalidateScrollBarParameters ()
                {
                    if (fPrintInfo == NULL) {
                        inherited::InvalidateScrollBarParameters ();
                    }
                }
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::UpdateScrollBars
                @DESCRIPTION:   Avoid errors updating sbars while printing.
                */
                void
                Led_MFC_CViewHelper<BASECLASS>::UpdateScrollBars ()
                {
                    if (fPrintInfo != NULL) {
                        return; // ignore while in print mode...
                    }
                    inherited::UpdateScrollBars ();
                }
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::DeleteContents
                @DESCRIPTION:   Hook the MFC DeleteContents () routine, and simulate the user having deleted all the text in the
                    buffer.
                */
                void
                Led_MFC_CViewHelper<BASECLASS>::DeleteContents ()
                {
                    ASSERT_VALID (this);
                    AssertNotNull (m_hWnd);
                    SetWindowText (NULL);
                    ASSERT_VALID (this);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::Serialize (CArchive& ar)
                {
                    // Probably not the right thing todo, but a simple enuf hack for now - LGP 950511
                    SerializeRaw (ar);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::SerializeRaw (CArchive& ar)
                {
                    ASSERT_VALID (this);
                    AssertNotNull (m_hWnd);
                    if (ar.IsStoring ()) {
                        WriteToArchive (ar);
                    }
                    else {
                        CFile* file = ar.GetFile ();
                        ASSERT_VALID (file);
                        DWORD dwLen = static_cast<DWORD> (file->GetLength ()); // maybe should subtract current offset?
                        ReadFromArchive (ar, (UINT)dwLen);
                    }
                    ASSERT_VALID (this);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::ReadFromArchive (CArchive& ar, UINT nLen)
                {
                    ASSERT_VALID (this);

                    Memory::SmallStackBuffer<Led_tChar> buf (nLen);
                    if (ar.Read (buf, nLen * sizeof (Led_tChar)) != nLen * sizeof (Led_tChar)) {
                        AfxThrowArchiveException (CArchiveException::endOfFile);
                    }
                    // Replace the editing edit buffer with the newly loaded data
                    nLen = static_cast<UINT> (Characters::NormalizeTextToNL<Led_tChar> (buf, nLen, buf, nLen));
                    if (ValidateTextForCharsetConformance (buf, nLen)) {
                        Replace (0, 0, buf, nLen);
                    }
                    else {
                        OnBadUserInput ();
                    }

                    Invalidate ();

                    ASSERT_VALID (this);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::WriteToArchive (CArchive& ar)
                {
                    ASSERT_VALID (this);

                    size_t                              nLen = GetLength ();
                    Memory::SmallStackBuffer<Led_tChar> buf (nLen);
                    CopyOut (0, nLen, buf);
                    Memory::SmallStackBuffer<Led_tChar> buf2 (2 * nLen);
                    nLen = Characters::NLToNative<Led_tChar> (buf, nLen, buf2, 2 * nLen);
                    ar.Write (buf2, static_cast<UINT> (nLen * sizeof (Led_tChar)));

                    ASSERT_VALID (this);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::OnLButtonDown (UINT nFlags, CPoint oPoint)
                {
                    UpdateClickCount (Time::GetTickCount (), AsLedPoint (oPoint));

                    if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
                        // Any time we get a click, make sure there are no active in-place activations.
                        // Any click anyplace outside the active item is ignored, except for deactivating
                        // that item.
                        AssertMember (m_pDocument, COleDocument); //
                        COleDocument&   doc   = *(COleDocument*)m_pDocument;
                        COleClientItem* pItem = doc.GetInPlaceActiveItem (this);
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
                template <typename BASECLASS>
                DROPEFFECT Led_MFC_CViewHelper<BASECLASS>::OnDragEnter (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
                {
                    return HelperDragEnter (pDataObject, dwKeyState, point);
                }
                template <typename BASECLASS>
                DROPEFFECT Led_MFC_CViewHelper<BASECLASS>::OnDragOver (COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
                {
                    return HelperDragOver (pDataObject, dwKeyState, point);
                }
                template <typename BASECLASS>
                BOOL Led_MFC_CViewHelper<BASECLASS>::OnDrop (COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
                {
                    return HelperDrop (pDataObject, dropEffect, point);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::OnDragLeave ()
                {
                    HelperDragLeave ();
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::OnPaint ()
                {
                    CView::OnPaint (); // don't use inherited/Led_MFC_CViewHelper<BASECLASS>_Helper<T> version - cuz misses some MFC hooks
                }
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_CViewHelper<BASECLASS>::OnDraw
                @DESCRIPTION:   Hook the MFC OnDraw () method to invoke the Led drawing mechanism, and redisplay the window.
                */
                void
                Led_MFC_CViewHelper<BASECLASS>::OnDraw (CDC* pDC)
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

                    CRect boundsRect = CRect (0, 0, 0, 0);
                    Verify (pDC->GetClipBox (&boundsRect) != ERROR);
                    WindowDrawHelper (Led_MFC_TabletFromCDC (pDC), AsLedRect (boundsRect), false);
                }
                template <typename BASECLASS>
                int Led_MFC_CViewHelper<BASECLASS>::OnCreate (LPCREATESTRUCT lpCreateStruct)
                {
                    if (inherited::OnCreate (lpCreateStruct) != 0) {
                        return -1; // failed to create
                    }
                    TabletChangedMetrics (); // maybe should catch failures here, and return -1?
                    return 0;                // sucess
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
                {
                    if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
                        // Don't allow scrolling while there is an in-place active item.
                        // Wish we had a better interface for this... Maybe we should be deactivating
                        // SBARS when we activate item??? For now, interpret scroll attempt as a request
                        // to deactivate...
                        // LGP 960530
                        AssertMember (m_pDocument, COleDocument);
                        COleDocument&   doc   = *(COleDocument*)m_pDocument;
                        COleClientItem* pItem = doc.GetInPlaceActiveItem (this);
                        if (pItem != NULL) {
                            pItem->Close ();
                            return;
                        }
                    }
                    inherited::OnVScroll (nSBCode, nPos, pScrollBar);
                }
                template <typename BASECLASS>
                void Led_MFC_CViewHelper<BASECLASS>::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
                {
                    if (m_pDocument != NULL and dynamic_cast<COleDocument*> (m_pDocument) != NULL) {
                        // Don't allow scrolling while there is an in-place active item.
                        // Wish we had a better interface for this... Maybe we should be deactivating
                        // SBARS when we activate item??? For now, interpret scroll attempt as a request
                        // to deactivate...
                        // LGP 960530
                        AssertMember (m_pDocument, COleDocument); //
                        COleDocument&   doc   = *(COleDocument*)m_pDocument;
                        COleClientItem* pItem = doc.GetInPlaceActiveItem (this);
                        if (pItem != NULL) {
                            pItem->Close ();
                            return;
                        }
                    }
                    inherited::OnHScroll (nSBCode, nPos, pScrollBar);
                }
                template <typename BASECLASS>
                const AFX_MSGMAP* Led_MFC_CViewHelper<BASECLASS>::GetMessageMap () const
                {
                    return GetThisMessageMap ();
                }
                template <typename BASECLASS>
                const AFX_MSGMAP* PASCAL Led_MFC_CViewHelper<BASECLASS>::GetThisMessageMap ()
                {
                    using ThisClass    = Led_MFC_CViewHelper<BASECLASS>;
                    using TheBaseClass = BASECLASS;
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
                    static const AFX_MSGMAP_ENTRY _messageEntries[] = {
                        ON_WM_PAINT ()
                            ON_WM_LBUTTONDOWN ()
                                ON_WM_CREATE ()
                                    ON_WM_VSCROLL ()
                                        ON_WM_HSCROLL (){0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0}};
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
                    static const AFX_MSGMAP messageMap = {&TheBaseClass::GetThisMessageMap, &_messageEntries[0]};
                    return &messageMap;
                }

                //  class   Led_MFC_ExceptionHandlerHelper<BASECLASS>
                template <typename BASECLASS>
                /*
                @METHOD:        Led_MFC_ExceptionHandlerHelper<BASECLASS>::HandleException
                @DESCRIPTION:   Override this to provide different exception handling. By default, this calls @'Led_BeepNotify'.
                */
                void
                Led_MFC_ExceptionHandlerHelper<BASECLASS>::HandleException () const
                {
                    Led_BeepNotify ();
                }
                template <typename BASECLASS>
                void Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
                {
                    try {
                        inherited::OnChar (nChar, nRepCnt, nFlags);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template <typename BASECLASS>
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
                template <typename BASECLASS>
                void Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
                {
                    try {
                        inherited::OnKeyDown (nChar, nRepCnt, nFlags);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template <typename BASECLASS>
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
                template <typename BASECLASS>
                void Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonDown (UINT nFlags, CPoint oPoint)
                {
                    try {
                        inherited::OnLButtonDown (nFlags, oPoint);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template <typename BASECLASS>
                void Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonUp (UINT nFlags, CPoint oPoint)
                {
                    try {
                        inherited::OnLButtonUp (nFlags, oPoint);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template <typename BASECLASS>
                void Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnLButtonDblClk (UINT nFlags, CPoint oPoint)
                {
                    try {
                        inherited::OnLButtonDblClk (nFlags, oPoint);
                    }
                    catch (...) {
                        HandleException ();
                    }
                }
                template <typename BASECLASS>
                BOOL Led_MFC_ExceptionHandlerHelper<BASECLASS>::OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
                {
                    try {
                        return inherited::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
                    }
                    catch (...) {
                        HandleException ();
                        return 1;
                    }
                }
                template <typename BASECLASS>
                const AFX_MSGMAP* Led_MFC_ExceptionHandlerHelper<BASECLASS>::GetMessageMap () const
                {
                    return GetThisMessageMap ();
                }
                template <typename BASECLASS>
                const AFX_MSGMAP* PASCAL Led_MFC_ExceptionHandlerHelper<BASECLASS>::GetThisMessageMap ()
                {
                    using ThisClass    = Led_MFC_ExceptionHandlerHelper<BASECLASS>;
                    using TheBaseClass = BASECLASS;
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4407) // Not sure this is safe to ignore but I think it is due to qMFCRequiresCWndLeftmostBaseClass
#endif
                    static const AFX_MSGMAP_ENTRY _messageEntries[] = {
                        ON_WM_CHAR ()
                            ON_MESSAGE (WM_IME_CHAR, &OnIMEChar)
                                ON_WM_KEYDOWN ()
                                    ON_MESSAGE (WM_PASTE, &OnMsgPaste)
                                        ON_WM_LBUTTONDOWN ()
                                            ON_WM_LBUTTONUP ()
                                                ON_WM_LBUTTONDBLCLK (){0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0}};
#if qSilenceAnnoyingCompilerWarnings && _MSC_VER
#pragma warning(pop)
#endif
                    static const AFX_MSGMAP messageMap = {&TheBaseClass::GetThisMessageMap, &_messageEntries[0]};
                    return &messageMap;
                }

                //  class   Led_MFC_X<ChosenInteractor,LEDMFC>
                template <typename ChosenInteractor, typename LEDMFC>
                inline Led_MFC_X<ChosenInteractor, LEDMFC>::Led_MFC_X ()
                    : ChosenInteractor ()
                    , LEDMFC ()
                {
                }
                /*
                @METHOD:        Led_MFC_X<ChosenInteractor,LEDMFC>::AboutToUpdateText
                @DESCRIPTION:   Override to handle mixin ambiguity.
                */
                template <typename ChosenInteractor, typename LEDMFC>
                void Led_MFC_X<ChosenInteractor, LEDMFC>::AboutToUpdateText (const MarkerOwner::UpdateInfo& updateInfo)
                {
                    LEDMFC::AboutToUpdateText (updateInfo);
                    ChosenInteractor::AboutToUpdateText (updateInfo);
                }
                /*
                @METHOD:        Led_MFC_X<ChosenInteractor,LEDMFC>::DidUpdateText
                @DESCRIPTION:   Override to handle mixin ambiguity.
                */
                template <typename ChosenInteractor, typename LEDMFC>
                void Led_MFC_X<ChosenInteractor, LEDMFC>::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo) noexcept
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
                template <typename TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
                CSize Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
                {
                    RequireNotNull (cdc);
                    Led_MFC_TabletFromCDC tmpTablet (cdc);
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
                template <typename TRIVIALWORDWRAPPEDTEXTIMAGER, typename SIMPLETEXTIMAGER, typename TEXTSTORE>
                void Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
                {
                    RequireNotNull (cdc);
                    Led_MFC_TabletFromCDC tmpTablet (cdc);
                    DrawTextBox<TRIVIALWORDWRAPPEDTEXTIMAGER, SIMPLETEXTIMAGER, TEXTSTORE> (tmpTablet, text, AsLedRect (r), wordWrap);
                }

                //  class   Led_MFC_TmpCmdUpdater
                inline Led_MFC_TmpCmdUpdater::Led_MFC_TmpCmdUpdater (CCmdUI* pCmdUI)
                    : fCmdUI (pCmdUI)
                    , fCmdNum (MFC_CommandNumberMapping::Get ().Lookup (pCmdUI->m_nID))
                    , fEnabled (false)
                {
                    RequireNotNull (fCmdUI);
                }
                inline Led_MFC_TmpCmdUpdater::operator Led_MFC_TmpCmdUpdater* ()
                {
                    return this;
                }
                inline Led_MFC_TmpCmdUpdater::operator CCmdUI* ()
                {
                    EnsureNotNull (fCmdUI);
                    return fCmdUI;
                }
            }
        }
    }
}

#endif /*_Stroika_Frameworks_Led_Platform_MFC_inl_*/
