/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Platform_Windows_h_
#define _Stroika_Frameworks_Led_Platform_Windows_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <Windows.h>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/LineEndings.h"
#include "Stroika/Foundation/Debug/CompileTimeFlagChecker.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Frameworks/Led/TextInteractor.h"

/*
@MODULE:    Led_Win32
@DESCRIPTION:
        <p>Led_Win32 is a module providing a an interface between the Win32 operating system/platform and
    the Led class library. It is not class-library specific. It only depends on the Win32 API.
    To get support specifically tuned to MFC - for example - see @'Led_MFC'.</p>
 */

namespace Stroika::Frameworks::Led::Platform {

    /*
     **************** Windows Specific configuration variables **************
     */

/*
    @CONFIGVAR:     qSupportWindowsSDKCallbacks
    @DESCRIPTION:   <p>This might be wanted if you have code which currently depends on these
        callbacks, but its probably better, and more portable (across platforms)
        to just override the appropriate Led method. And if you do that, there
        is no reason to pay the performance cost of sending unheeded messages.</p>
            <p>Turn OFF by default - LGP 970710</p>
        */
#ifndef qSupportWindowsSDKCallbacks
#define qSupportWindowsSDKCallbacks 0
#endif

/*
    @CONFIGVAR:     qSupportFunnyMSPageUpDownAdjustSelectionBehavior
    @DESCRIPTION:   <p>OBSOLETE as of Led 3.1a9. Use @'Led_Win32_Helper<BASE_INTERACTOR>::SetFunnyMSPageUpDownAdjustSelectionBehavior' instead.</p>
        */
#ifdef qSupportFunnyMSPageUpDownAdjustSelectionBehavior
#error "This flag is OBSOLETE. Use Led_Win32_Helper<BASE_INTERACTOR>::SetFunnyMSPageUpDownAdjustSelectionBehavior instead".
#endif

/*
    @CONFIGVAR:     qScrollTextDuringThumbTracking
    @DESCRIPTION:   <p>On Windows many applications scroll their content as the thumb is tracking.
        This CAN make thumb movement slower, and may be undesriable for other reasons</p>
            <p>But its common enuf now that I make this behavior ON by default</p>
        */
#ifndef qScrollTextDuringThumbTracking
#define qScrollTextDuringThumbTracking 1
#endif

/*
    @CONFIGVAR:     qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
    @DESCRIPTION:   <p>On Windows 2000, you SUPPOSEDLY can now get UNICODE characters from the new IME.
        However - in practice - unless you build a so-called UNICODE application (really this means call RegisterClassW
        rather than RegisterClassA).
        Then the Win32 API IsWindowUnicode () returns TRUE or FALSE according to which way
        you registered your window class.
        Then the IME decides whether to hand your app UNICODE characters (WM_CHAR/WM_IME_CHAR) based on IsWindowUnicode () API.</p>
            <p>However - UNICODE applications (so-called) cannot run under Win98 and earlier. For many (most applications)
        this is not an acceptable limitation (which is why Led supports so-called 'Partial UNICODE' configurations - @'Led_tChar'
        is UNICODE - but @'Led_SDKChar' is not).</p>
            <p>Another relevant problem is that MFC allows you to build an application that is either fully UNICODE (-D_UNICODE) or
        fully ANSI (not -D_UNICODE). Nothing in between.</p>
            <p>So - if you want your app to use MFC - and you want UNICODE support - you are out of luck getting characters from the IME
        (unless you set your OS SYSTEM DEFAULT LOCALE for the one locale all your characters character set can be found in).</p>
            <p>In steps the qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug - to rescue the day.
        I asked MSFT for help on this question (CASE ID #SRX991213601271). Basicly they weren't very helpful. But after
        much discussion - I seem to have elicited a kludge that seems to work pretty well.</p>
            <p>When ever I get an IME_CHAR message - instead of trying to decode the multibyte character inside
        (which will be bogus frequently) - I call ImmGetCompositionStringW () and then grab the characters out of that
        buffer directly. The only tricky part - and this was VERY tricky - is which characters to grab!</p>
            <p>Eventually, through trial and error - I have evolved into doing this. Set an index counter to zero. And for
        every WM_IME_CHAR - grab the char at that index, and bump my index. And for ever WM_IMECOMPOSITION or WM_IME_ENDCOMPOSITION
        message - reset that counter to zero.</p>
            <p>I've tried this on NT4Japanese, Win98J, and Win2K(RC2) with the default Locale Japanese or English. It seems to work.</p>
            <p>This is probably somewhat error prone or risky. It is for that reason that I'm making this bug workaround
        optional - and easy to shut off. But I leave it on - by default (when building for UNICODE - but without -D_UNICODE), since
        in that case - you will almost certainly want that to work.</p>
            <p>Default Value:   (qWideCharacters && !qTargetPlatformSDKUseswchar_t)</p>
        */
#ifndef qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
#define qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug (!qTargetPlatformSDKUseswchar_t)
#endif

    /*
    @CLASS:     FunnyMSPageUpDownAdjustSelectionHelper
    @DESCRIPTION:   <p>Helper class to implement common MS-Windows UI where  paging up/down, and sometimes other activities try
                to presever the window-relative location of the blinking caret (by adjusting the selection accordingly). I think
                this is a strange UI - but it does apepar to be common.</p>
                    <p>See also @'Led_Win32_Helper<BASE_INTERACTOR>::SetFunnyMSPageUpDownAdjustSelectionBehavior'</p>
        */
    class FunnyMSPageUpDownAdjustSelectionHelper {
    public:
        FunnyMSPageUpDownAdjustSelectionHelper ();

    public:
        nonvirtual void CaptureInfo (TextInteractor& ti);
        nonvirtual void CompleteAdjustment (TextInteractor& ti);

    private:
        size_t fRowNum;
    };

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         Led_Win32_Helper<BASE_INTERACTOR>
    @BASES:         BASE_INTERACTOR = @'TextInteractor'
    @DESCRIPTION:   <p>DOCUMENT SOON - BASED ON @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'</p>
    */
    template <typename BASE_INTERACTOR = TextInteractor>
    class Led_Win32_Helper : public virtual BASE_INTERACTOR {
    private:
        using inherited = BASE_INTERACTOR;

    public:
        using UpdateMode = TextInteractor::UpdateMode;
        using UpdateInfo = MarkerOwner::UpdateInfo;

    protected:
        Led_Win32_Helper ();

    public:
        virtual ~Led_Win32_Helper ();

        // Routines subclasses must override so that this class can work...
    public:
        virtual HWND GetHWND () const = 0;

    protected:
        virtual LRESULT DefWindowProc (UINT message, WPARAM wParam, LPARAM lParam);

        // Message Map Hook Functions (these must hook into whatever message map mechanism you are using - either WndProc or MFC / ATL Message Maps)
    protected:
        virtual LRESULT OnCreate_Msg (LPCREATESTRUCT createStruct);
        virtual void    OnPaint_Msg ();
        virtual void    OnSize_Msg ();
        virtual void    OnChar_Msg (UINT nChar, LPARAM lKeyData);
        virtual LRESULT OnUniChar_Msg (WPARAM nChar, LPARAM lParam);
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
        virtual LONG OnIMEChar_Msg (WPARAM wParam, LPARAM lParam);
        virtual LONG OnIME_COMPOSITION_Msg (WPARAM wParam, LPARAM lParam);
        virtual LONG OnIME_ENDCOMPOSITION_Msg (WPARAM wParam, LPARAM lParam);
#endif
        virtual void OnKeyDown_Msg (UINT nChar, LPARAM lKeyData);
        virtual bool OnSetCursor_Msg (HWND hWnd, UINT nHitTest, UINT message);
        virtual UINT OnGetDlgCode_Msg ();
        virtual void OnSetFocus_Msg (HWND oldWnd);
        virtual void OnKillFocus_Msg (HWND newWnd);
        virtual bool OnEraseBkgnd_Msg (HDC hDC);
        virtual void OnTimer_Msg (UINT_PTR nEventID, TIMERPROC* proc);
        virtual void OnLButtonDown_Msg (UINT nFlags, int x, int y);
        virtual void OnLButtonUp_Msg (UINT nFlags, int x, int y);
        virtual void OnLButtonDblClk_Msg (UINT nFlags, int x, int y);
        virtual void OnMouseMove_Msg (UINT nFlags, int x, int y);
        virtual void OnVScroll_Msg (UINT nSBCode, UINT nPos, HWND hScrollBar);
        virtual void OnHScroll_Msg (UINT nSBCode, UINT nPos, HWND hScrollBar);
        virtual bool OnMouseWheel_Msg (WPARAM wParam, LPARAM lParam);
        virtual void OnEnable_Msg (bool enable);

    protected:
        short fAccumulatedWheelDelta;

    public:
        nonvirtual TWIPS_Rect GetDefaultWindowMargins () const;
        nonvirtual void       SetDefaultWindowMargins (const TWIPS_Rect& defaultWindowMargins);

    private:
        TWIPS_Rect fDefaultWindowMargins;

    public:
        nonvirtual bool GetControlArrowsScroll () const;
        nonvirtual void SetControlArrowsScroll (bool controlArrowsScroll);

    private:
        bool fControlArrowsScroll;

    public:
        nonvirtual bool GetFunnyMSPageUpDownAdjustSelectionBehavior () const;
        nonvirtual void SetFunnyMSPageUpDownAdjustSelectionBehavior (bool funnyMSPageUpDownAdjustSelectionBehavior);

    private:
        bool fFunnyMSPageUpDownAdjustSelectionBehavior;

#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
    protected:
        size_t fIMECurCharIdx;
#endif

        // Message Map Helpers - often do the REAL work of handling the messages...
    protected:
        nonvirtual void OnSize_ ();

        /*
         * Overrides of the Led (TextInteractor) code that must thunk down to Win32 SDK calls
         */
    protected:
        virtual void RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const override;
        virtual void UpdateWindowRect_ (const Led_Rect& windowRectArea) const override;
        virtual bool QueryInputKeyStrokesPending () const override;

        // Tablet API
    public:
        /*
        @CLASS:         Led_Win32_Helper<BASE_CLASS>::TemporarilyUseTablet
        @DESCRIPTION:   <p>Utility class to use (with caution), to temporarily for a given tablet to be
            used for a given Led_Win32_Helper,
            TextImager. NB: This causes the @'TextImager::TabletChangedMetrics' method by default
            (unless called with special arg).</p>
        */
        class TemporarilyUseTablet {
        public:
            enum DoTextMetricsChangedCall {
                eDoTextMetricsChangedCall,
                eDontDoTextMetricsChangedCall
            };
            TemporarilyUseTablet (Led_Win32_Helper<BASE_INTERACTOR>& editor, Tablet* t, DoTextMetricsChangedCall tmChanged = eDoTextMetricsChangedCall);
            ~TemporarilyUseTablet ();

        private:
            Led_Win32_Helper<BASE_INTERACTOR>& fEditor;
            Tablet*                            fOldTablet;
            DoTextMetricsChangedCall           fDoTextMetricsChangedCall;
        };

    private:
        friend class TemporarilyUseTablet;

    protected:
        virtual Tablet* AcquireTablet () const override;
        virtual void    ReleaseTablet (Tablet* tablet) const override;

    private:
        Tablet*        fUpdateTablet;    // assigned in stack-based fasion during update/draw calls.
        mutable Tablet fAllocatedTablet; // if we needed to allocate a tablet, store it here, and on the
        // last release of it, free it...
        mutable size_t fAcquireCount;

    public:
        virtual void WindowDrawHelper (Tablet* tablet, const Led_Rect& subsetToDraw, bool printing);

    protected:
        virtual void EraseBackground (Tablet* tablet, const Led_Rect& subsetToDraw, bool printing) override;

        // Keyboard Processing:
    private:
        nonvirtual bool CheckIfDraggingBeepAndReturn ();

    protected:
        virtual void HandleTabCharacterTyped ();

    public:
        virtual void AboutToUpdateText (const UpdateInfo& updateInfo) override;
        virtual void DidUpdateText (const UpdateInfo& updateInfo) noexcept override;

    protected:
        nonvirtual void DidUpdateText_ (const UpdateInfo& updateInfo) noexcept;

        // Mouse Processing
    protected:
        Led_Point fMouseTrackingLastPoint;

    protected:
        size_t fDragAnchor; // only used while dragging mouse
    private:
#if qScrollTextDuringThumbTracking
        bool fSBarThumbTracking;
#endif

    protected:
        nonvirtual void OnNormalLButtonDown (UINT nFlags, const Led_Point& at);

        // Timer support for autoscrolling (normal and drag).
    private:
        nonvirtual void StartAutoscrollTimer ();
        nonvirtual void StopAutoscrollTimer ();

    private:
        enum {
            eAutoscrollingTimerEventID = 434
        };                           // Magic#
        UINT_PTR fAutoScrollTimerID; // zero means no timer

        // Scrolling Support
    public:
        using ScrollBarType = TextInteractor::ScrollBarType; // redundant typedef to keep compiler happy...LGP 2000-10-05-MSVC60
        using VHSelect      = TextInteractor::VHSelect;      // redundant typedef to keep compiler happy...LGP 2000-10-05-MSVC60

    public:
        virtual void SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType) override;

    protected:
        virtual bool       ShouldUpdateHScrollBar () const;
        virtual bool       ShouldUpdateVScrollBar () const;
        virtual bool       TypeAndScrollInfoSBVisible (ScrollBarType scrollbarAppears, const SCROLLINFO& scrollInfo) const;
        virtual SCROLLINFO GetHScrollInfo (UINT nMask = SIF_ALL) const;
        virtual void       SetHScrollInfo (ScrollBarType scrollbarAppears, const SCROLLINFO& scrollInfo, bool redraw = true);
        nonvirtual void    SetHScrollInfo (const SCROLLINFO& scrollInfo, bool redraw = true);
        virtual SCROLLINFO GetVScrollInfo (UINT nMask = SIF_ALL) const;
        virtual void       SetVScrollInfo (ScrollBarType scrollbarAppears, const SCROLLINFO& scrollInfo, bool redraw = true);
        nonvirtual void    SetVScrollInfo (const SCROLLINFO& scrollInfo, bool redraw = true);

    public:
        virtual void InvalidateScrollBarParameters () override;

    protected:
        nonvirtual void InvalidateScrollBarParameters_ ();

    protected:
        virtual void    InvalidateCaretState () override;
        nonvirtual void UpdateCaretState_ ();

    protected:
        virtual void UpdateScrollBars () override;

        // Clipboard Support
    protected:
        virtual bool OnCopyCommand_Before () override;
        virtual void OnCopyCommand_After () override;
        virtual bool OnPasteCommand_Before () override;
        virtual void OnPasteCommand_After () override;

    public:
        nonvirtual DWORD GetStyle () const;

        // Private Misc Helpers
    private:
        nonvirtual int GetWindowID () const;

    protected:
        nonvirtual HWND GetValidatedHWND () const;
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>
    @BASES:         BASECLASS
    @DESCRIPTION:   <p>Mimicry of the starndard Win32 messages sent to an edit control. We cannot
                mimic ALL the messages. Some just don't make sense (like GETHANDLE). But for those that do make sense, we
                do our best.</p>
    */
    template <typename BASECLASS>
    class Led_Win32_Win32SDKMessageMimicHelper : public BASECLASS {
    private:
        using inherited = BASECLASS;

    protected:
        Led_Win32_Win32SDKMessageMimicHelper ();

    public:
        virtual bool HandleMessage (UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

    protected:
        virtual LRESULT OnMsgGetText (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgSetText (WPARAM /*wParam*/, LPARAM lParam);
        virtual LRESULT OnMsgGetTextLength (WPARAM /*wParam*/, LPARAM /*lParam*/);
        virtual LRESULT OnMsgGetSel (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgSetReadOnly (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgGetFirstVisibleLine (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgLineIndex (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgLineCount (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgCanUndo (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgUndo (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgEmptyUndoBuffer (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgClear (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgCut (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgCopy (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgPaste (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgLineFromChar (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgLineLength (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgLineScroll (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgReplaceSel (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgSetSel (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgScrollCaret (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgGetFont (WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnMsgSetFont (WPARAM wParam, LPARAM lParam);

    private:
        FontObject fDefaultFontCache; // used to be able to answer WM_GETFONT calls...
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

    /*
    @CLASS:         SimpleWin32WndProcHelper
    @DESCRIPTION:   <p>Simple window subclassing/WindowProc hook. Doesn't really work with Led-based code - particular. Helpful for
                other windows you need to hook, like dialogs etc. This could be integrated with Led's window classes
                at some future point.</p>
    */
    class SimpleWin32WndProcHelper {
    public:
        SimpleWin32WndProcHelper ();

    public:
        nonvirtual HWND GetHWND () const;
        nonvirtual void SetHWND (HWND hWnd);
        nonvirtual HWND GetValidatedHWND () const;

    private:
        HWND fHWnd;

    public:
        nonvirtual void Create (LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight,
                                HWND hWndParent, HMENU hMenu, HINSTANCE hInstance);
        nonvirtual void Create (DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth,
                                int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance);

    public:
        nonvirtual bool SubclassWindow (HWND hWnd);
        nonvirtual bool SubclassWindowW (HWND hWnd);

    private:
        WNDPROC fSuperWindowProc;

    public:
        nonvirtual LRESULT SendMessage (UINT msg, WPARAM wParam, LPARAM lParam);

    public:
        nonvirtual bool IsWindowRealized () const;
        nonvirtual void Assert_Window_Realized () const;

    protected:
        nonvirtual void Require_Window_Realized () const;

    public:
        nonvirtual bool IsWindowUNICODE () const;

    public:
        nonvirtual bool IsWindowShown () const;
        nonvirtual void SetWindowVisible (bool shown = true);

    public:
        static LRESULT CALLBACK StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        virtual LRESULT DefWindowProc (UINT message, WPARAM wParam, LPARAM lParam);
    };

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         Led_Win32_SimpleWndProc_Helper
    @BASES:         @'Led_Win32_Helper<BASE_INTERACTOR>'
    @DESCRIPTION:   <p>Add a simple Win32 WndProc static function to be used in a window class, and have that call the right
        entries in BASE_WIN32_HELPER to handle the calls. Not a generic message map mechanism - but instead one simple
        and hardwired for our purposes here.</p>
    */
    template <typename BASE_WIN32_HELPER = Led_Win32_Helper<>>
    class Led_Win32_SimpleWndProc_Helper : public BASE_WIN32_HELPER {
    private:
        using inherited = BASE_WIN32_HELPER;

    public:
        Led_Win32_SimpleWndProc_Helper ();

    public:
        virtual ~Led_Win32_SimpleWndProc_Helper ();

    public:
        nonvirtual void Create (LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight,
                                HWND hWndParent, HMENU hMenu, HINSTANCE hInstance);
        nonvirtual void Create (DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth,
                                int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance);

    public:
        nonvirtual void SetHWND (HWND hWnd);

    private:
        HWND fHWnd;

        // Led_Win32_Helper overrides.
    public:
        virtual HWND GetHWND () const override;

    public:
        virtual void OnNCDestroy_Msg ();

    public:
        nonvirtual bool SubclassWindow (HWND hWnd);
        nonvirtual bool ReplaceWindow (HWND hWnd);

    protected:
        virtual void HookSubclassWindow ();

    private:
        WNDPROC fSuperWindowProc;

    public:
        static LRESULT CALLBACK StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam);

    protected:
        virtual LRESULT DefWindowProc (UINT message, WPARAM wParam, LPARAM lParam) override;
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

    DISABLE_COMPILER_MSC_WARNING_START (4250) // inherits via dominance warning
    /*
    @CLASS:         Led_Win32_SimpleWndProc_HelperWithSDKMessages
    @BASES:         @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>' with BASE_INTERACTOR= @'Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>'
    @DESCRIPTION:   <p>Simple mixin to combine these two.</p>
    */
    template <typename BASE_CLASS>
    class Led_Win32_SimpleWndProc_HelperWithSDKMessages : public Led_Win32_SimpleWndProc_Helper<Led_Win32_Win32SDKMessageMimicHelper<BASE_CLASS>> {
    private:
        using inherited = Led_Win32_SimpleWndProc_Helper<Led_Win32_Win32SDKMessageMimicHelper<BASE_CLASS>>;

    public:
        Led_Win32_SimpleWndProc_HelperWithSDKMessages ();

    protected:
        virtual LRESULT WndProc (UINT message, WPARAM wParam, LPARAM lParam) override;
    };
    DISABLE_COMPILER_MSC_WARNING_END (4250) // inherits via dominance warning

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
#if (_WIN32_WINNT < 0x0501)
//  Make sure WM_UNICHAR & UNICODE_NOCHAR are defined, even if a user builds with old header files
//  or inconsistent settings of _WIN32_WINNT ... LGP 2003-01-29
#ifndef WM_UNICHAR
#define WM_UNICHAR 0x0109
#endif
#ifndef UNICODE_NOCHAR
#define UNICODE_NOCHAR 0xFFFF
#endif
#endif

    namespace Private {
        /*
         * Hack to assure the Led_Win32.o module is linked in. Without it being linked in,
         * the IdleManagerOSImpl_Win32 won't get installed.
         */
        struct IdleMangerLinkerSupport {
            IdleMangerLinkerSupport ();
        };
        static IdleMangerLinkerSupport sIdleMangerLinkerSupport;
    }

    //  class   Led_Win32_Helper<BASE_INTERACTOR>
    template <typename BASE_INTERACTOR>
    Led_Win32_Helper<BASE_INTERACTOR>::Led_Win32_Helper ()
        : fAccumulatedWheelDelta (0)
        , fDefaultWindowMargins ()
        , fControlArrowsScroll (false)
        , fFunnyMSPageUpDownAdjustSelectionBehavior (true)
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
        , fIMECurCharIdx (0)
#endif
        , fUpdateTablet (nullptr)
        , fAllocatedTablet ()
        , fAcquireCount (0)
        , fMouseTrackingLastPoint ()
        , fDragAnchor (0)
#if qScrollTextDuringThumbTracking
        , fSBarThumbTracking (false)
#endif
        , fAutoScrollTimerID (0)
    {
        fDefaultWindowMargins.top    = TWIPS (2 * TWIPS::kPoint);
        fDefaultWindowMargins.left   = TWIPS (2 * TWIPS::kPoint);
        fDefaultWindowMargins.bottom = TWIPS (2 * TWIPS::kPoint);
        fDefaultWindowMargins.right  = TWIPS (2 * TWIPS::kPoint);
    }
    template <typename BASE_INTERACTOR>
    Led_Win32_Helper<BASE_INTERACTOR>::~Led_Win32_Helper ()
    {
        Assert (fUpdateTablet == nullptr);
        Assert (fAllocatedTablet.m_hDC == nullptr);
        Assert (fAllocatedTablet.m_hAttribDC == nullptr);
        Assert (fAcquireCount == 0);
        Assert (fAutoScrollTimerID == 0); // I don't see how we can get destroyed while tracking?
    }
    template <typename BASE_INTERACTOR>
    LRESULT Led_Win32_Helper<BASE_INTERACTOR>::DefWindowProc (UINT message, WPARAM wParam, LPARAM lParam)
    {
        return ::DefWindowProc (this->GetValidatedHWND (), message, wParam, lParam);
    }
    template <typename BASE_INTERACTOR>
    LRESULT Led_Win32_Helper<BASE_INTERACTOR>::OnCreate_Msg (LPCREATESTRUCT createStruct)
    {
        RequireNotNull (createStruct);
        if ((createStruct->style & WS_VSCROLL) and this->GetScrollBarType (TextInteractor::v) == TextInteractor::eScrollBarNever) {
            this->SetScrollBarType (TextInteractor::v, TextInteractor::eScrollBarAlways);
        }
        if ((createStruct->style & WS_HSCROLL) and this->GetScrollBarType (TextInteractor::h) == TextInteractor::eScrollBarNever) {
            this->SetScrollBarType (TextInteractor::h, TextInteractor::eScrollBarAlways);
        }
        return this->DefWindowProc (WM_CREATE, 0, reinterpret_cast<LPARAM> (createStruct));
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnPaint_Msg ()
    {
        HWND        hWnd = this->GetValidatedHWND ();
        PAINTSTRUCT ps;
        HDC         hdc = ::BeginPaint (hWnd, &ps);
        if (hdc != nullptr) {
            RECT boundsRect;
            Verify (::GetClipBox (hdc, &boundsRect) != ERROR);
            Tablet tablet (hdc, Tablet::eDoesntOwnDC);
            try {
                this->WindowDrawHelper (&tablet, AsLedRect (boundsRect), false);
            }
            catch (...) {
                ::EndPaint (hWnd, &ps);
                throw;
            }
            ::EndPaint (hWnd, &ps);
        }
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnSize_Msg ()
    {
        this->OnSize_ ();
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnChar_Msg
    @DESCRIPTION:   <p>Hook the Win32 SDK WM_CHAR message to handle user typing. Probably should not be called directly,
        except maybe to simulate user typing.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::OnChar_Msg (UINT nChar, LPARAM /*lKeyData*/)
    {
#if !qTargetPlatformSDKUseswchar_t
        {
            CodePage useCodePage = Characters::Platform::Windows::Win32PrimaryLangIDToCodePage (LOWORD (::GetKeyboardLayout (nullptr)));
            char     ccc         = nChar;
            wchar_t  outC        = 0;
            int      xx          = ::MultiByteToWideChar (useCodePage, 0, &ccc, 1, &outC, 1);
            if (xx == 1) {
                nChar = outC;
            }
        }
#endif

        if (nChar == VK_ESCAPE) {
            // Ignore when user types ESC key - that is what Windows NotePad and MS Word seem todo...
            return;
        }

        if (this->CheckIfDraggingBeepAndReturn ()) {
            return;
        }

        if (nChar == '\t') {
            this->HandleTabCharacterTyped ();
            return;
        }

        if (nChar == '\r') {
            nChar = '\n';
        }
        if ((nChar == 0x7f) and (::GetKeyState (VK_CONTROL) & 0x8000)) {
            nChar = '\b';
        }

        this->OnTypedNormalCharacter (static_cast<Led_tChar> (nChar), false, !!(::GetKeyState (VK_SHIFT) & 0x8000), false,
                                      !!(::GetKeyState (VK_CONTROL) & 0x8000), !!(::GetKeyState (VK_MENU) & 0x8000));

#if qSupportWindowsSDKCallbacks
        HWND hWnd = this->GetValidatedHWND ();
        (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (GetWindowID (), EN_CHANGE), (LPARAM)hWnd);
#endif
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnUniChar_Msg
    @DESCRIPTION:   <p>Hook the Win32 SDK WM_UNICHAR message to handle user typing. Probably should not be called directly,
        except maybe to simulate user typing.</p>
    */
    LRESULT Led_Win32_Helper<BASE_INTERACTOR>::OnUniChar_Msg (WPARAM nChar, LPARAM /*lParam*/)
    {
        if (nChar == UNICODE_NOCHAR) {
            return 1;
        }

        if (nChar == VK_ESCAPE) {
            // Ignore when user types ESC key - that is what Windows NotePad and MS Word seem todo...
            return 0;
        }

        if (CheckIfDraggingBeepAndReturn ()) {
            return 0;
        }

        if (nChar == '\t') {
            HandleTabCharacterTyped ();
            return 0;
        }

        if (nChar == '\r') {
            nChar = '\n';
        }
        if ((nChar == 0x7f) and (::GetKeyState (VK_CONTROL) & 0x8000)) {
            nChar = '\b';
        }

        this->OnTypedNormalCharacter (static_cast<Led_tChar> (nChar), false, !!(::GetKeyState (VK_SHIFT) & 0x8000), false,
                                      !!(::GetKeyState (VK_CONTROL) & 0x8000), !!(::GetKeyState (VK_MENU) & 0x8000));

#if qSupportWindowsSDKCallbacks
        HWND hWnd = this->GetValidatedHWND ();
        (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (GetWindowID (), EN_CHANGE), (LPARAM)hWnd);
#endif
        return 0;
    }
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnIMEChar_Msg
    @DESCRIPTION:   <p>Part of @'qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug' bug workaround.</p>
    */
    LONG Led_Win32_Helper<BASE_INTERACTOR>::OnIMEChar_Msg (WPARAM wParam, LPARAM lParam)
    {
        /*
         *  Win32 SDK docs don't say what value to return for WM_IME_CHAR - so return 0 for now - LGP20000111
         */
        UINT nChar = wParam;
        if (nChar == VK_ESCAPE) {
            // Ignore when user types ESC key - that is what Windows NotePad and MS Word seem todo...
            return 0;
        }
        if (nChar == '\t') {
            HandleTabCharacterTyped ();
            return 0;
        }

        if (CheckIfDraggingBeepAndReturn ()) {
            return 0;
        }

        if (nChar == '\r') {
            nChar = '\n';
        }

        if (qTargetPlatformSDKUseswchar_t || ::IsWindowUnicode (this->GetValidatedHWND ())) {
            // do nothing - 'nChar' is already a fine UNICODE character
            // NB: we COULD just check qTargetPlatformSDKUseswchar_t. But be nicer that MSFT. Allow for that a user
            // might want to create a UNICODE window without defining -D_UNICODE (see comments in
            // qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug)
        }
#if !qTargetPlatformSDKUseswchar_t
        else {
            wstring tmpIMEBugWorkaroundCompString = IME::Get ().GetCompositionResultStringW (this->GetValidatedHWND ());
            if (fIMECurCharIdx < tmpIMEBugWorkaroundCompString.length ()) {
                nChar = tmpIMEBugWorkaroundCompString[fIMECurCharIdx];
                ++fIMECurCharIdx;
            }
            else {
                /*
                    *  This shouldn't happen. And it won't do much good except on systems where the default locale
                    *  is set to use a characterset used by the IME. But at least for those cases - do what
                    *  we can...
                    */
                wchar_t convertedChars[2];
                memset (&convertedChars, 0, sizeof (convertedChars));
                int     nWideChars    = ::MultiByteToWideChar (CP_ACP, 0, reinterpret_cast<char*> (&nChar), 2, convertedChars, 2);
                wchar_t convertedChar = convertedChars[0];
                if (nWideChars == 0) {
                    OnBadUserInput ();
                    return 0;
                }
                nChar = convertedChar;
            }
        }
#endif

        OnTypedNormalCharacter (nChar, false, !!(::GetKeyState (VK_SHIFT) & 0x8000), false, !!(::GetKeyState (VK_CONTROL) & 0x8000),
                                !!(::GetKeyState (VK_MENU) & 0x8000));

#if qSupportWindowsSDKCallbacks
        HWND hWnd = this->GetValidatedHWND ();
        (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (GetWindowID (), EN_CHANGE), (LPARAM)hWnd);
#endif
        return 0;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnIME_COMPOSITION_Msg
    @DESCRIPTION:   <p>Part of @'qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug' bug workaround.</p>
    */
    LONG Led_Win32_Helper<BASE_INTERACTOR>::OnIME_COMPOSITION_Msg (WPARAM wParam, LPARAM lParam)
    {
        fIMECurCharIdx = 0;
        return DefWindowProc (WM_IME_COMPOSITION, wParam, lParam);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnIME_ENDCOMPOSITION_Msg
    @DESCRIPTION:   <p>Part of @'qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug' bug workaround.</p>
    */
    LONG Led_Win32_Helper<BASE_INTERACTOR>::OnIME_ENDCOMPOSITION_Msg (WPARAM wParam, LPARAM lParam)
    {
        fIMECurCharIdx = 0;
        return DefWindowProc (WM_IME_ENDCOMPOSITION, wParam, lParam);
    }
#endif
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnKeyDown_Msg
    @DESCRIPTION:   <p>Hook the Win32 SDK WM_KEYDOWN message to handle user typing. Most typing handled via Led_MFC::OnChar (). But
        some keystrokes only make it to here, and on the WM_CHAR message, for some reason.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::OnKeyDown_Msg (UINT nChar, LPARAM /*lKeyData*/)
    {
        bool shiftPressed   = !!(::GetKeyState (VK_SHIFT) & 0x8000);
        bool controlPressed = !!(::GetKeyState (VK_CONTROL) & 0x8000);

        using TextInteractor::eDefaultUpdate;
        using TextInteractor::eImmediateUpdate;

        using TextInteractor::eCursorBack;
        using TextInteractor::eCursorByBuffer;
        using TextInteractor::eCursorByChar;
        using TextInteractor::eCursorByLine;
        using TextInteractor::eCursorByRow;
        using TextInteractor::eCursorByWord;
        using TextInteractor::eCursorExtendingSelection;
        using TextInteractor::eCursorForward;
        using TextInteractor::eCursorMoving;
        using TextInteractor::eCursorToEnd;
        using TextInteractor::eCursorToStart;

        /*
         *  There are zillions of these virtual keycodes, and I'm unsure exactly if/how
         *  I'm to respond to each. All the ones here are listed in numeric order (the
         *  order they are found in WinUser.h). We (will) omit any which we do nothing
         *  for - and only include those we react to.
         */
        switch (nChar) {
            case VK_BACK: {
                // Seems to get called in OnChar() - so ignore it here...
            } break;

            case VK_TAB: {
            } break;

            case VK_CLEAR: {
            } break;

            case VK_RETURN: {
            } break;

            case VK_PRIOR: { // page UP
                if (CheckIfDraggingBeepAndReturn ()) {
                    return;
                }
                this->BreakInGroupedCommands ();

                if (controlPressed) {
                    this->SetSelection (this->GetMarkerPositionOfStartOfWindow (), this->GetMarkerPositionOfStartOfWindow ());
                }
                else {
                    FunnyMSPageUpDownAdjustSelectionHelper selectionAdjuster;
                    bool                                   doFunny = this->GetFunnyMSPageUpDownAdjustSelectionBehavior ();
                    if (doFunny) {
                        selectionAdjuster.CaptureInfo (*this);
                    }

                    // NB: this isn't QUITE right for pageup - with differing height rows!!!!
                    // Do the actual scrolling - this is the only part that makes any sense!
                    this->ScrollByIfRoom (-(int)this->GetTotalRowsInWindow ());

                    if (doFunny) {
                        selectionAdjuster.CompleteAdjustment (*this);
                    }
                }
#if qPeekForMoreCharsOnUserTyping
                this->UpdateIfNoKeysPending ();
#endif
            } break;

            case VK_NEXT: { // page DOWN
                if (this->CheckIfDraggingBeepAndReturn ()) {
                    return;
                }
                this->BreakInGroupedCommands ();

                if (controlPressed) {
                    this->SetSelection (this->GetMarkerPositionOfEndOfWindow (), this->GetMarkerPositionOfEndOfWindow ());
                }
                else {
                    FunnyMSPageUpDownAdjustSelectionHelper selectionAdjuster;
                    bool                                   doFunny = this->GetFunnyMSPageUpDownAdjustSelectionBehavior ();
                    if (doFunny) {
                        selectionAdjuster.CaptureInfo (*this);
                    }

                    this->ScrollByIfRoom (this->GetTotalRowsInWindow ());

                    if (doFunny) {
                        selectionAdjuster.CompleteAdjustment (*this);
                    }
                }
#if qPeekForMoreCharsOnUserTyping
                this->UpdateIfNoKeysPending ();
#endif
            } break;

            case VK_END: {
                if (this->CheckIfDraggingBeepAndReturn ()) {
                    return;
                }
                this->BreakInGroupedCommands ();
                this->DoSingleCharCursorEdit (eCursorToEnd, controlPressed ? eCursorByBuffer : eCursorByRow,
                                              shiftPressed ? eCursorExtendingSelection : eCursorMoving,
                                              qPeekForMoreCharsOnUserTyping ? eDefaultUpdate : eImmediateUpdate);
#if qPeekForMoreCharsOnUserTyping
                this->UpdateIfNoKeysPending ();
#endif
            } break;

            case VK_HOME: {
                if (this->CheckIfDraggingBeepAndReturn ()) {
                    return;
                }
                this->BreakInGroupedCommands ();
                this->DoSingleCharCursorEdit (eCursorToStart, controlPressed ? eCursorByBuffer : eCursorByRow,
                                              shiftPressed ? eCursorExtendingSelection : eCursorMoving,
                                              qPeekForMoreCharsOnUserTyping ? eDefaultUpdate : eImmediateUpdate);
#if qPeekForMoreCharsOnUserTyping
                this->UpdateIfNoKeysPending ();
#endif
            } break;

            case VK_LEFT: {
                if (this->CheckIfDraggingBeepAndReturn ()) {
                    return;
                }
                this->BreakInGroupedCommands ();
                this->DoSingleCharCursorEdit (eCursorBack, controlPressed ? eCursorByWord : eCursorByChar,
                                              shiftPressed ? eCursorExtendingSelection : eCursorMoving,
                                              qPeekForMoreCharsOnUserTyping ? eDefaultUpdate : eImmediateUpdate);
#if qPeekForMoreCharsOnUserTyping
                this->UpdateIfNoKeysPending ();
#endif
            } break;

            case VK_UP: {
                if (this->CheckIfDraggingBeepAndReturn ()) {
                    return;
                }
                this->BreakInGroupedCommands ();
                if (controlPressed) {
                    if (this->GetControlArrowsScroll ()) {
                        FunnyMSPageUpDownAdjustSelectionHelper selectionAdjuster;
                        bool                                   doFunny = this->GetFunnyMSPageUpDownAdjustSelectionBehavior ();
                        if (doFunny) {
                            selectionAdjuster.CaptureInfo (*this);
                        }
                        this->ScrollByIfRoom (-1);
                        if (doFunny) {
                            selectionAdjuster.CompleteAdjustment (*this);
                        }
                    }
                    else {
                        this->DoSingleCharCursorEdit (eCursorToStart, eCursorByLine, shiftPressed ? eCursorExtendingSelection : eCursorMoving,
                                                      qPeekForMoreCharsOnUserTyping ? eDefaultUpdate : eImmediateUpdate);
                    }
                }
                else {
                    this->DoSingleCharCursorEdit (eCursorBack, eCursorByRow, shiftPressed ? eCursorExtendingSelection : eCursorMoving,
                                                  qPeekForMoreCharsOnUserTyping ? eDefaultUpdate : eImmediateUpdate);
                }
#if qPeekForMoreCharsOnUserTyping
                this->UpdateIfNoKeysPending ();
#endif
            } break;

            case VK_RIGHT: {
                if (this->CheckIfDraggingBeepAndReturn ()) {
                    return;
                }
                this->BreakInGroupedCommands ();
                this->DoSingleCharCursorEdit (eCursorForward, controlPressed ? eCursorByWord : eCursorByChar,
                                              shiftPressed ? eCursorExtendingSelection : eCursorMoving,
                                              qPeekForMoreCharsOnUserTyping ? eDefaultUpdate : eImmediateUpdate);
#if qPeekForMoreCharsOnUserTyping
                this->UpdateIfNoKeysPending ();
#endif
            } break;

            case VK_DOWN: {
                if (this->CheckIfDraggingBeepAndReturn ()) {
                    return;
                }
                this->BreakInGroupedCommands ();
                if (controlPressed) {
                    if (this->GetControlArrowsScroll ()) {
                        FunnyMSPageUpDownAdjustSelectionHelper selectionAdjuster;
                        bool                                   doFunny = this->GetFunnyMSPageUpDownAdjustSelectionBehavior ();
                        if (doFunny) {
                            selectionAdjuster.CaptureInfo (*this);
                        }
                        this->ScrollByIfRoom (1);
                        if (doFunny) {
                            selectionAdjuster.CompleteAdjustment (*this);
                        }
                    }
                    else {
                        this->DoSingleCharCursorEdit (eCursorToEnd, eCursorByLine, shiftPressed ? eCursorExtendingSelection : eCursorMoving,
                                                      qPeekForMoreCharsOnUserTyping ? eDefaultUpdate : eImmediateUpdate);
                    }
                }
                else {
                    this->DoSingleCharCursorEdit (eCursorForward, eCursorByRow, shiftPressed ? eCursorExtendingSelection : eCursorMoving,
                                                  qPeekForMoreCharsOnUserTyping ? eDefaultUpdate : eImmediateUpdate);
                }
#if qPeekForMoreCharsOnUserTyping
                this->UpdateIfNoKeysPending ();
#endif
            } break;

            case VK_SELECT: {
            } break;

            case VK_PRINT: {
            } break;

            case VK_EXECUTE: {
            } break;

            case VK_SNAPSHOT: {
            } break;

            case VK_INSERT: {
            } break;

            case VK_DELETE: {
                if (this->CheckIfDraggingBeepAndReturn ()) {
                    return;
                }

                this->BreakInGroupedCommands ();

                /*
                 *  If the selection is empty, then delete the following character (if any)
                 *  and if it is non-empty - simply delete its contents.
                 */
                if (this->GetSelectionStart () == this->GetSelectionEnd ()) {
                    // note this doesn't change the selection - since we only delete following
                    // the selection...
                    //
                    // Also note that we count on the fact that it is safe toocall FindNextCharacter ()
                    // at the buffers end and it will pin to that end...
                    this->SetSelection (this->GetSelectionStart (), this->FindNextCharacter (this->GetSelectionStart ()), eDefaultUpdate);
                    this->InteractiveReplace (LED_TCHAR_OF (""), 0);
                }
                else {
                    this->OnPerformCommand (TextInteractor::kClear_CmdID);
                }
                this->ScrollToSelection ();
                this->Update ();
#if qSupportWindowsSDKCallbacks
                // Actually - sends EN_CHANGE even if selStart=sselEnd==ENDOFBUFFER (so nothing changed). But hopefully thats OK...
                HWND hWnd = this->GetValidatedHWND ();
                (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (GetWindowID (), EN_CHANGE), (LPARAM)hWnd);
#endif
            } break;

            case VK_HELP: {
            } break;

            default: {
                // just ignore any of the others...
            } break;
        }
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnSetCursor_Msg
    @DESCRIPTION:   <p>Hook the Win32 SDK WM_SETCURSOR message to handle set the cursor to an I-Beam, as appropriate. When over
        draggable text, instead use a standard arrow cursor.</p>
    */
    bool Led_Win32_Helper<BASE_INTERACTOR>::OnSetCursor_Msg (HWND hWnd, UINT nHitTest, UINT message)
    {
        if (nHitTest == HTCLIENT and hWnd == this->GetValidatedHWND ()) {
            /*
             *  The SDK docs seem to indicate that you should call the inherited one, to see
             *  if your parent changed the cursor. But if you do, it changes the cursor
             *  to an arrow, and we switch back quickly - producing flicker. I probably
             *  should do something closer to what the SDK recmends - but this seems good
             *  enuf for now - LGP 950212
             */
            BOOL result = false;
            if (result) {
                return true; // parent window handled it - see SDK docs
            }

            // If cursor is over draggable text, then change cursor to an arrow.
            // Doug Stein says is according to Apple HIG guidelines - LGP 960804
            // SPR#0371
            if (this->GetSelectionStart () != this->GetSelectionEnd ()) {
                Region r;
                this->GetSelectionWindowRegion (&r, this->GetSelectionStart (), this->GetSelectionEnd ());
                if (r.PtInRegion (AsPOINT (fMouseTrackingLastPoint))) {
                    ::SetCursor (::LoadCursor (nullptr, IDC_ARROW));
                    return true;
                }
            }

            ::SetCursor (::LoadCursor (nullptr, IDC_IBEAM));

            return true;
        }
        else {
            return !!this->DefWindowProc (WM_SETCURSOR, WPARAM (hWnd), MAKELPARAM (nHitTest, message));
        }
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnGetDlgCode_Msg
    @DESCRIPTION:   <p>Hook the Win32 SDK WM_GETDLGCODE message so that windows knows which messages to send to our WindowProc.
        See the Win32 SDK for more details.</p>
    */
    UINT Led_Win32_Helper<BASE_INTERACTOR>::OnGetDlgCode_Msg ()
    {
        DWORD style    = GetStyle ();
        UINT  dlogCode = DLGC_WANTARROWS | DLGC_HASSETSEL | DLGC_WANTCHARS;
        if ((style & ES_MULTILINE) and (style & ES_WANTRETURN)) {
            dlogCode |= DLGC_WANTALLKEYS;
        }
        return (dlogCode);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnSetFocus_Msg
    @DESCRIPTION:   <p>Called by the system when we receive the input focus. React by adjusting IME, showing cursor, etc.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::OnSetFocus_Msg (HWND /*oldWnd*/)
    {
#if qStroika_Frameworks_Led_ProvideIMESupport
        IME::Get ().ForgetPosition ();
        IME::Get ().Enable ();
#endif

        this->SetCaretShown (true);
        this->SetSelectionShown (true);

#if qSupportWindowsSDKCallbacks
        // Notify the parent window...
        HWND hWnd = this->GetValidatedHWND ();
        (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, (WPARAM)MAKELONG (GetWindowID (), EN_SETFOCUS), (LPARAM)(hWnd));
#endif
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::OnSetFocus_Msg
    @DESCRIPTION:   <p>Called by the system when we lose the input focus. React by adjusting IME, showing cursor, etc.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::OnKillFocus_Msg (HWND /*newWnd*/)
    {
        if (this->PeekAtTextStore () == nullptr) {
            /*
             *  SPR#0893 - Its a reasonable practice to call SpecifyTextStore(nullptr) in the DTOR of some subclass of a Led-based editor.
             *  Then - if the window was focused - in later DTORs - the window will be destroyed (Led_Win32_Helper<BASE_INTERACTOR>::DTOR calls
             *  DestroyWindow). That invokes KillFocus. In that case - just silently drop the killfocus on the floor.
             */
            return;
        }
        this->BreakInGroupedCommands ();

        this->SetCaretShown (false);
        this->SetSelectionShown (false);

#if qSupportWindowsSDKCallbacks
        // Notify the parent window...
        HWND hWnd = this->GetValidatedHWND ();
        (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, (WPARAM)MAKELONG (GetWindowID (), EN_KILLFOCUS), (LPARAM)(hWnd));
#endif
    }
    template <typename BASE_INTERACTOR>
    bool Led_Win32_Helper<BASE_INTERACTOR>::OnEraseBkgnd_Msg ([[maybe_unused]] HDC hDC)
    {
        // We don't need our background erased - we take care of that when we draw
        // Allowing it to be erased just produces flicker...
        return true;
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnTimer_Msg (UINT_PTR nEventID, TIMERPROC* proc)
    {
        HWND hWnd = this->GetValidatedHWND ();
        if (nEventID == eAutoscrollingTimerEventID) {
            if (::GetCapture () == hWnd) {
                // Since we have the capture, we got the last mouse moved event, and so we have a VALID value stored in
                // fMouseTrackingLastPoint for the mouse location. Use that.
                this->WhileSimpleMouseTracking (fMouseTrackingLastPoint, fDragAnchor);
            }
            else {
                this->StopAutoscrollTimer ();
            }
        }
        else {
            this->DefWindowProc (WM_TIMER, nEventID, LPARAM (proc));
        }
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnLButtonDown_Msg (UINT nFlags, int x, int y)
    {
        this->UpdateClickCount (Foundation::Time::GetTickCount (), Led_Point (y, x));
        this->OnNormalLButtonDown (nFlags, Led_Point (y, x));
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnLButtonUp_Msg (UINT /*nFlags*/, int x, int y)
    {
        HWND hWnd = this->GetValidatedHWND ();
        AssertNotNull (hWnd);
        if (::GetCapture () == hWnd) {
            this->StopAutoscrollTimer ();
            Verify (::ReleaseCapture ());
            this->WhileSimpleMouseTracking (Led_Point (y, x), fDragAnchor);
        }
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnLButtonDblClk_Msg (UINT /*nFlags*/, int x, int y)
    {
        this->IncrementCurClickCount (Foundation::Time::GetTickCount ());

        bool extendSelection = !!(::GetKeyState (VK_SHIFT) & 0x8000);
        if (not this->ProcessSimpleClick (Led_Point (y, x), this->GetCurClickCount (), extendSelection, &fDragAnchor)) {
            return;
        }

        HWND hWnd = this->GetValidatedHWND ();
        (void)::SetCapture (hWnd); //  We could loop doing a sub-eventloop here - but it seems more common
        //  Windows practice to just store up state information and wait on
        //  WM_MOUSEMOVE and WM_LButtonUp messages...
        this->StartAutoscrollTimer ();
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnMouseMove_Msg (UINT /*nFlags*/, int x, int y)
    {
        HWND hWnd               = this->GetValidatedHWND ();
        fMouseTrackingLastPoint = Led_Point (y, x); // save for OnTimer_Msg ()
        if (::GetCapture () == hWnd) {
            this->WhileSimpleMouseTracking (Led_Point (y, x), fDragAnchor);
        }
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnVScroll_Msg (UINT nSBCode, UINT /*nPos*/, HWND /*hScrollBar*/)
    {
        using namespace Characters::Literals;
        DbgTrace ("Led_Win32_Helper<BASE_INTERACTOR>::OnVScroll_Msg (nSBCode={},...)"_f, nSBCode);

        using TextInteractor::eDefaultUpdate;
        using TextInteractor::eImmediateUpdate;

        /*
         *  NB: the nPos is a 16-bit value - and we could have a 32-bit offset - so use GetScrollInfo  () to get the POS - rather
         *  than using this parameter.
         */
        if (nSBCode == SB_LINEDOWN or nSBCode == SB_LINEUP or nSBCode == SB_PAGEDOWN or nSBCode == SB_PAGEUP) {
            if (not this->DelaySomeForScrollBarClick ()) {
                return;
            }
        }

#if qDynamiclyChooseAutoScrollIncrement
        Foundation::Time::TimePointSeconds        now = Foundation::Time::GetTickCount ();
        static Foundation::Time::TimePointSeconds sLastTimeThrough{};
        const Foundation::Time::DurationSeconds   kClickThreshold = Led_GetDoubleClickTime () / 3;
        bool                                      firstClick      = (now - sLastTimeThrough > kClickThreshold);

        int increment = firstClick ? 1 : 2;
#else
        const int increment = 1;
#endif

#if qScrollTextDuringThumbTracking
        fSBarThumbTracking = (nSBCode == SB_THUMBTRACK);
#endif
        switch (nSBCode) {
            case SB_BOTTOM: {
                this->ScrollSoShowing (this->GetLength ());
            } break;

            case SB_ENDSCROLL: {
                // Ignore
            } break;

            case SB_LINEDOWN: {
                this->ScrollByIfRoom (increment, eImmediateUpdate);
            } break;

            case SB_LINEUP: {
                this->ScrollByIfRoom (-increment, eImmediateUpdate);
            } break;

            case SB_PAGEDOWN: {
                this->ScrollByIfRoom (this->GetTotalRowsInWindow ());
            } break;

            case SB_PAGEUP: {
                this->ScrollByIfRoom (-(int)this->GetTotalRowsInWindow ());
            } break;

            case SB_THUMBTRACK:
#if qScrollTextDuringThumbTracking
// Fall through into SB_THUMBPOSITION code
#else
                break;
#endif
            case SB_THUMBPOSITION: {
                size_t newPos = 0;
                {
                    /*
                     *  It is not totally clear why we use GetVScrollInfo instead of the passed in
                     *  'nPos' for tracking. The current docs (2003-01-20) seem to indicate
                     *  we should use the passed in 'nPos'. Oh well - this seems to be working OK.
                     *      --  LGP 2003-01-20.
                     */
                    SCROLLINFO scrollInfo = this->GetVScrollInfo ();
#if qScrollTextDuringThumbTracking
                    newPos = scrollInfo.nTrackPos;
#else
                    newPos = scrollInfo.nPos;
#endif
                    newPos = min (newPos, this->GetLength ());

                    /*
                     *  Beware about the fact that the verticalWindow size changes as we scroll (since it
                     *  is measured in number of Led_tChars on display in the window).
                     */
                    if (newPos + scrollInfo.nPage >= static_cast<UINT> (scrollInfo.nMax)) {
                        newPos = this->GetLength ();
                    }

#if qScrollTextDuringThumbTracking
                    // Make sure nPos matches nTrackPos after tracking done
                    scrollInfo.cbSize = sizeof (scrollInfo);
                    scrollInfo.fMask  = SIF_POS;
                    scrollInfo.nPos   = static_cast<UINT> (newPos);
                    SetVScrollInfo (this->GetScrollBarType (TextInteractor::v), scrollInfo);
#endif
                }

#if qScrollTextDuringThumbTracking
                this->InvalidateScrollBarParameters (); // In case below SetHScrollPos doesn't cause inval (due to caching), make sure
                                                        // things really get recomputed
#endif

                this->SetTopRowInWindowByMarkerPosition (newPos);
            } break;

            case SB_TOP: {
                this->ScrollSoShowing (0);
            } break;

            default: {
                Assert (false); // should be safe to ignore these - but if there are any xtras
                // I'd like to know...And asserts will be compiled out before
                // shipping- LGP 941026
            } break;
        }
#if qDynamiclyChooseAutoScrollIncrement
        sLastTimeThrough = now;
#endif
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnHScroll_Msg (UINT nSBCode, UINT /*nPos*/, HWND /*hScrollBar*/)
    {
        /*
         *  NB: the nPos is a 16-bit value - and we could have a 32-bit offset - so use GetScrollInfo  () to get the POS - rather
         *  than using this parameter.
         */
        if (nSBCode == SB_LINEDOWN or nSBCode == SB_LINEUP or nSBCode == SB_PAGEDOWN or nSBCode == SB_PAGEUP) {
            if (not this->DelaySomeForScrollBarClick ()) {
                return;
            }
        }

#if qDynamiclyChooseAutoScrollIncrement
        Foundation::Time::TimePointSeconds        now = Foundation::Time::GetTickCount ();
        static Foundation::Time::TimePointSeconds sLastTimeThrough{};
        const Foundation::Time::DurationSeconds   kClickThreshold = Led_GetDoubleClickTime ();
        bool                                      firstClick      = (now - sLastTimeThrough > kClickThreshold);

        int increment = firstClick ? 1 : 10;
#else
        const int increment = 1;
#endif

        switch (nSBCode) {
            case SB_BOTTOM: {
                this->SetHScrollPos (this->ComputeMaxHScrollPos ());
            } break;

            case SB_ENDSCROLL: {
                // ignore end of scrolling
            } break;

            case SB_LINEDOWN: {
                this->SetHScrollPos (min<CoordinateType> (this->GetHScrollPos () + increment, this->ComputeMaxHScrollPos ()));
            } break;

            case SB_LINEUP: {
                if (this->GetHScrollPos () > 0) {
                    this->SetHScrollPos (max<CoordinateType> (0, int (this->GetHScrollPos ()) - increment));
                }
            } break;

            case SB_PAGEDOWN: {
                const CoordinateType kPixelsAtATime = this->GetWindowRect ().GetWidth () / 2;
                this->SetHScrollPos (min<CoordinateType> (this->GetHScrollPos () + kPixelsAtATime, this->ComputeMaxHScrollPos ()));
            } break;

            case SB_PAGEUP: {
                const CoordinateType kPixelsAtATime = this->GetWindowRect ().GetWidth () / 2;
                if (this->GetHScrollPos () > kPixelsAtATime) {
                    this->SetHScrollPos (this->GetHScrollPos () - kPixelsAtATime);
                }
                else {
                    this->SetHScrollPos (0);
                }
            } break;

            case SB_THUMBTRACK:
#if qScrollTextDuringThumbTracking
// Fall through into SB_THUMBPOSITION code
#else
                break;
#endif
            case SB_THUMBPOSITION: {
#if qScrollTextDuringThumbTracking
                fSBarThumbTracking = (nSBCode == SB_THUMBTRACK);
#endif
                /*
                 *  It is not totally clear why we use GetHScrollInfo instead of the passed in
                 *  'nPos' for tracking. The current docs (2003-01-20) seem to indicate
                 *  we should use the passed in 'nPos'. Oh well - this seems to be working OK.
                 *      --  LGP 2003-01-20.
                 */
                SCROLLINFO scrollInfo = this->GetHScrollInfo ();
#if qScrollTextDuringThumbTracking
                size_t newPos = scrollInfo.nTrackPos;
#else
                size_t newPos = scrollInfo.nPos;
#endif

#if qScrollTextDuringThumbTracking
                // Make sure nPos matches nTrackPos after tracking done
                scrollInfo.cbSize = sizeof (scrollInfo);
                scrollInfo.fMask  = SIF_POS;
                scrollInfo.nPos   = static_cast<int> (newPos);
                this->SetHScrollInfo (this->GetScrollBarType (TextInteractor::h), scrollInfo);
#endif

#if qScrollTextDuringThumbTracking
                this->InvalidateScrollBarParameters (); // In case below SetHScrollPos doesn't cause inval (due to caching), make sure
                                                        // things really get recomputed
#endif

                this->SetHScrollPos (min<CoordinateType> (static_cast<CoordinateType> (newPos), this->ComputeMaxHScrollPos ()));
            } break;

            case SB_TOP: {
                this->SetHScrollPos (0);
            } break;

            default: {
                Assert (false); // should be safe to ignore these - but if there are any xtras
                // I'd like to know...And asserts will be compiled out before
                // shipping- LGP 941026
            } break;
        }
#if qDynamiclyChooseAutoScrollIncrement
        sLastTimeThrough = now;
#endif
    }
    template <typename BASE_INTERACTOR>
    bool Led_Win32_Helper<BASE_INTERACTOR>::OnMouseWheel_Msg (WPARAM wParam, LPARAM lParam)
    {
        using TextInteractor::eImmediateUpdate;
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
        UINT keyState = LOWORD (wParam); //GET_KEYSTATE_WPARAM (wParam);
        // we don't handle anything but scrolling just now (comment from MFC).
        // Not really sure how these special keys are SUPPOSED to be treated?
        if (keyState & (MK_SHIFT | MK_CONTROL)) {
            return !!this->DefWindowProc (WM_MOUSEWHEEL, wParam, lParam);
        }

        short zDelta = (short)HIWORD (wParam); //GET_WHEEL_DELTA_WPARAM (wParam);
        if ((zDelta >= 0) != (fAccumulatedWheelDelta >= 0)) {
            fAccumulatedWheelDelta = 0;
        }
        fAccumulatedWheelDelta += zDelta;

        int nTicks = fAccumulatedWheelDelta / WHEEL_DELTA;

        fAccumulatedWheelDelta %= WHEEL_DELTA;

        int scrollLines = 3; //default
        Verify (::SystemParametersInfo (SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0));

        // Pin - cuz result can be 'WHEEL_PAGESCROLL' - or just exceed a single page - and we are only supposed to scroll a max of
        // page per delta/WHEEL_DELTA, according to WM_MOUSEWHEEL docs.
        int totalRowsInWindow = static_cast<int> (this->GetTotalRowsInWindow ());
        scrollLines           = min (scrollLines, totalRowsInWindow);

        this->ScrollByIfRoom (-nTicks * scrollLines, eImmediateUpdate);
#endif
        return true;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::GetDefaultWindowMargins
    @DESCRIPTION:   <p>Arguments can be nullptr, and only non-nullptr pointers filled in.</p>
                <p>See @'Led_Win32_Helper<BASE_INTERACTOR>::SetDefaultWindowMargins'.</p>
    */
    inline TWIPS_Rect Led_Win32_Helper<BASE_INTERACTOR>::GetDefaultWindowMargins () const
    {
        return fDefaultWindowMargins;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::SetDefaultWindowMargins
    @DESCRIPTION:   <p>Sets the 'default window margins', retreivable through calls to
                @'Led_Win32_Helper<BASE_INTERACTOR>::GetDefaultWindowMargins'.</p>
                    <p>These margins are not to be confused with those specified in @'WordWrappedTextImager::GetLayoutMargins'.</p>
                    <p>These margins simply specify a small inset to be automatically applied to the ::GetClientRect()
                in the WM_SIZE message (handler) so that there is a border around this control. The WindowRect is simply
                inset by the amount given (and the borders automatically erased in this classes WM_ERASEBKGND method).</p>
                    <p>Margins default to zero, and so its as if this feature simply defaults to being off.</p>
                    <p>Margins are specified in @'TWIPS'.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::SetDefaultWindowMargins (const TWIPS_Rect& defaultWindowMargins)
    {
        if (fDefaultWindowMargins != defaultWindowMargins) {
            fDefaultWindowMargins = defaultWindowMargins;
            if (this->GetHWND () != nullptr) {
                this->OnSize_ (); // take into account new margins...
            }
        }
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::GetControlArrowsScroll
    @DESCRIPTION:   <p>See also @'Led_Win32_Helper<BASE_INTERACTOR>::SetControlArrowsScroll'</p>
    */
    inline bool Led_Win32_Helper<BASE_INTERACTOR>::GetControlArrowsScroll () const
    {
        return fControlArrowsScroll;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::SetControlArrowsScroll
    @DESCRIPTION:   <p>If this property is true - then when the user does a CNTRL->UP-ARROW key or CNTRL-DOWN-ARROW key,
                this does scrolling. If false - it does the default behavior of going to the start or end of
                the current paragraph (or if already at that end point - to the start/end of the prev/following paragraph)</p>
                    <p>This property defaults to <em>false</em>.</p>
                    <p>See also @'Led_Win32_Helper<BASE_INTERACTOR>::GetControlArrowsScroll'</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::SetControlArrowsScroll (bool controlArrowsScroll)
    {
        fControlArrowsScroll = controlArrowsScroll;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::GetFunnyMSPageUpDownAdjustSelectionBehavior
    @DESCRIPTION:   <p>See @'Led_Win32_Helper<BASE_INTERACTOR>::SetFunnyMSPageUpDownAdjustSelectionBehavior'.</p>
    */
    inline bool Led_Win32_Helper<BASE_INTERACTOR>::GetFunnyMSPageUpDownAdjustSelectionBehavior () const
    {
        return fFunnyMSPageUpDownAdjustSelectionBehavior;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::SetFunnyMSPageUpDownAdjustSelectionBehavior
    @DESCRIPTION:   <p>In Windows page up/down have a very funny interpretation. Playing with
                MSWord and Notepad, I see they each do something different, and I really
                understood neither. But both agreed that after a page up/down, you reset
                the selection somehow. I tried to mimic the behavior of MS-word as well
                as I could. This behavior is SO bizzare, I've made it optional. But since
                it appears to be an MS standard - its ON by default.</p>
                    <p>Turned ON by default</p>
                    <p>See also  @'Led_Win32_Helper<BASE_INTERACTOR>::GetFunnyMSPageUpDownAdjustSelectionBehavior'.</p>
                    <p>See also  @'FunnyMSPageUpDownAdjustSelectionHelper'.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::SetFunnyMSPageUpDownAdjustSelectionBehavior (bool funnyMSPageUpDownAdjustSelectionBehavior)
    {
        fFunnyMSPageUpDownAdjustSelectionBehavior = funnyMSPageUpDownAdjustSelectionBehavior;
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnEnable_Msg (bool /*enable*/)
    {
        this->Refresh ();
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnSize_ ()
    {
        using namespace Characters::Literals;
        this->InvalidateScrollBarParameters (); // Cuz even if no layoutwidth change, we still change page size for scrollbars, if any...
        RECT     cr;
        Led_Rect r;
        {
            HWND hWnd = this->GetValidatedHWND ();
            Verify (::GetClientRect (hWnd, &cr));
            r = AsLedRect (cr);
        }
        try {
            TextInteractor::Tablet_Acquirer tablet (this);
            Led_Rect                        wmr = tablet->CvtFromTWIPS (GetDefaultWindowMargins ());
            r.top += wmr.GetTop ();
            r.left += wmr.GetLeft ();
            r.bottom -= wmr.GetBottom ();
            r.right -= wmr.GetRight ();
        }
        catch (...) {
            Assert (false);
        }
        // Assure margins didn't produce an invalid WindowRect
        if (r.top >= r.bottom) {
            r.bottom = r.top + 1;
        }
        if (r.left >= r.right) {
            r.right = r.left + 1;
        }
        DbgTrace ("Led_Win32_Helper<>::OnSize_ (clientRect=(%d,%d,%d,%d), windowRect <= (%d,%d,%d,%d))"_f, cr.top, cr.left, cr.bottom,
                  cr.right, r.top, r.left, r.bottom, r.right);
        this->SetWindowRect (r);
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const
    {
        HWND hWnd = GetHWND ();
        if (hWnd != nullptr) {
            Assert (::IsWindow (hWnd));
            updateMode = this->RealUpdateMode (updateMode);
            switch (updateMode) {
                case TextInteractor::eDelayedUpdate: {
                    if (not windowRectArea.IsEmpty ()) {
                        RECT tmp = AsRECT (windowRectArea);
                        Verify (::InvalidateRect (hWnd, &tmp, true));
                    }
                } break;
                case TextInteractor::eImmediateUpdate: {
                    if (not windowRectArea.IsEmpty ()) {
                        RECT tmp = AsRECT (windowRectArea);
                        Verify (::RedrawWindow (hWnd, &tmp, nullptr, RDW_INVALIDATE | RDW_UPDATENOW));
                    }
                } break;
            }
        }
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::UpdateWindowRect_ (const Led_Rect& windowRectArea) const
    {
        if (not windowRectArea.IsEmpty ()) {
            // Unclear if I should use RedrawWindow (&windowRectArea, RDW_UPDATENOW or UpdateWindow???
            RECT tmp  = AsRECT (windowRectArea);
            HWND hWnd = GetHWND ();
            if (hWnd != nullptr) {
                Assert (::IsWindow (hWnd));
                Verify (::RedrawWindow (hWnd, &tmp, nullptr, RDW_UPDATENOW));
            }
        }
    }
    template <typename BASE_INTERACTOR>
    bool Led_Win32_Helper<BASE_INTERACTOR>::QueryInputKeyStrokesPending () const
    {
        MSG msg;
        return (!!::PeekMessage (&msg, GetValidatedHWND (), WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE));
    }
    template <typename BASE_INTERACTOR>
    Tablet* Led_Win32_Helper<BASE_INTERACTOR>::AcquireTablet () const
    {
        Require (fAcquireCount < 100); // not really a requirement - but hard to see how this could happen in LEGIT usage...
        // almost certainly a bug...
        if (fUpdateTablet != nullptr) {
            ++fAcquireCount;
            return fUpdateTablet;
        }

        /*
         *  Note that we only allocate the memory once, and don't free it, only constructing the
         *  windows object on the fly so as to avoid memory fragmentation resulting from the
         *  editor (for LEC's memory concerns - LGP 950524).
         */
        if (fAcquireCount == 0) {
            Assert (fAllocatedTablet.m_hDC == nullptr);
            Assert (fAllocatedTablet.m_hAttribDC == nullptr);
            HWND hWnd = GetHWND ();
            if (hWnd == nullptr) {
                throw TextInteractor::NoTabletAvailable ();
            }
            Assert (::IsWindow (hWnd));
            HDC hdc = ::GetWindowDC (hWnd);
            AssertNotNull (hdc);
            Verify (fAllocatedTablet.Attach (hdc));
        }
        Assert (fAllocatedTablet.m_hDC != nullptr);
        Assert (fAllocatedTablet.m_hAttribDC != nullptr);
        fAcquireCount++;
        return (&fAllocatedTablet);
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::ReleaseTablet (Tablet* tablet) const
    {
        AssertNotNull (tablet);
        Assert (fAcquireCount > 0);
        fAcquireCount--;
        Assert (tablet == fUpdateTablet or tablet == &fAllocatedTablet);
        if (fAcquireCount == 0 and tablet == &fAllocatedTablet) {
            Verify (::ReleaseDC (GetValidatedHWND (), fAllocatedTablet.Detach ()));
        }
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_CLASS>::WindowDrawHelper
    @DESCRIPTION:   <p>Share some code among various methods which invoke message-paint-based drawing. Helpful in some
        places like ActiveLedIt! where we don't get message WM_PAINT at all, but get from the control.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::WindowDrawHelper (Tablet* tablet, const Led_Rect& subsetToDraw, bool printing)
    {
        using namespace Characters::Literals;
        DbgTrace ("Led_Win32_Helper<>::WindowDrawHelper (subsetToDraw= (%d, %d, %d, %d))"_f, subsetToDraw.top, subsetToDraw.left,
                  subsetToDraw.bottom, subsetToDraw.right);
        TemporarilyUseTablet tmpUseTablet{*this, tablet, TemporarilyUseTablet::eDontDoTextMetricsChangedCall};
        this->Draw (subsetToDraw, printing);

        /*
         *  The user specified a DefaultWindowMargin - lies outside of our WindowRect, and so
         *  won't get drawn by the normal Led draw mechanism. Make sure it gets erased here.
         */
        Led_Rect   wr = this->GetWindowRect ();
        TWIPS_Rect wm = this->GetDefaultWindowMargins ();
        if (wm.GetTop () != 0 or wm.GetLeft () != 0 or wm.GetBottom () != 0 or wm.GetRight () != 0) {
            Led_Rect wmr = tablet->CvtFromTWIPS (wm);
            // Erase our TOP margin
            {
                Led_Rect barRect = subsetToDraw;
                barRect.bottom   = wr.top;
                if (not barRect.IsEmpty ()) {
                    this->EraseBackground (tablet, barRect, false);
                }
            }
            // Erase our LHS margin
            {
                Led_Rect barRect = subsetToDraw;
                barRect.right    = wr.left;
                if (not barRect.IsEmpty ()) {
                    this->EraseBackground (tablet, barRect, false);
                }
            }
            // Erase our Bottom margin
            {
                Led_Rect barRect = subsetToDraw;
                barRect.top      = wr.bottom;
                if (not barRect.IsEmpty ()) {
                    this->EraseBackground (tablet, barRect, false);
                }
            }
            // Erase our RHS margin
            {
                Led_Rect barRect = subsetToDraw;
                barRect.left     = wr.right;
                if (not barRect.IsEmpty ()) {
                    this->EraseBackground (tablet, barRect, false);
                }
            }
        }

        this->UpdateCaretState_ ();
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::EraseBackground
    @DESCRIPTION:
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::EraseBackground (Tablet* tablet, const Led_Rect& subsetToDraw, bool printing)
    {
        DWORD dwStyle = GetStyle ();
        if (((dwStyle & WS_DISABLED) or (dwStyle & ES_READONLY)) and (not printing)) {
            //const Color   kReadOnlyBackground =   Color (RGB (0xc0,0xc0,0xc0));
            //const Color   kReadOnlyBackground =   Color (::GetSysColor (COLOR_SCROLLBAR));
            const Color kReadOnlyBackground = Color (::GetSysColor (COLOR_BTNFACE));
            tablet->EraseBackground_SolidHelper (subsetToDraw, kReadOnlyBackground);
        }
        else {
            inherited::EraseBackground (tablet, subsetToDraw, printing);
        }
    }
    template <typename BASE_INTERACTOR>
    bool Led_Win32_Helper<BASE_INTERACTOR>::CheckIfDraggingBeepAndReturn ()
    {
        HWND hWnd = GetHWND ();
        if (hWnd != nullptr and ::GetCapture () == hWnd) {
// we must be tracking - drop characters typed at that point on the floor
// send a beep message as well to indicate that the characters are being dropped!
#if 1
            // As temphack to ameliorate SPR#1065 - just directly do a sysbeep here
            Led_BeepNotify ();
#else
            OnBadUserInput ();
#endif
            return true;
        }
        return false;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::HandleTabCharacterTyped
    @DESCRIPTION:   <p>Win32 SDK kind of queer in its OnGetDlgCode () API. In order to get enter keys, we see to have
        to request ALL_KEYS. Then - that means we get TAB keys - even if we didn't want them, and wanted them processed by
        the outer window automatically.</p>
            <p>So - try to guestimate the right default behavior here for handing a tab character. And in the end - leave it
        virtual so users can get what they want.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::HandleTabCharacterTyped ()
    {
        HWND hWnd = this->GetValidatedHWND ();
        if (this->GetStyle () & WS_TABSTOP) {
            HWND parent = ::GetParent (hWnd);
            if (parent != nullptr) {
                bool shiftPressed = !!(::GetKeyState (VK_SHIFT) & 0x8000);
                if (shiftPressed) {
                    ::PostMessage (parent, WM_NEXTDLGCTL, 1, false);
                }
                else {
                    ::PostMessage (parent, WM_NEXTDLGCTL, 0, false);
                }
            }
        }
        else {
            this->OnTypedNormalCharacter ('\t', false, !!(::GetKeyState (VK_SHIFT) & 0x8000), false,
                                          !!(::GetKeyState (VK_CONTROL) & 0x8000), !!(::GetKeyState (VK_MENU) & 0x8000));
#if qSupportWindowsSDKCallbacks
            (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (GetWindowID (), EN_CHANGE), (LPARAM)hWnd);
#endif
        }
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::AboutToUpdateText
    @DESCRIPTION:   <p>Override to hook @'MarkerOwner::AboutToUpdateText' and check for modifications when we are READONLY or DISABLED.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::AboutToUpdateText (const UpdateInfo& updateInfo)
    {
        if (GetHWND () != nullptr and this->CheckIfCurrentUpdateIsInteractive () and updateInfo.fRealContentUpdate) {
            /*
             *  Make sure we've been realized (fully constructed), and also only check if this is an interactive update.
             */
            DWORD dwStyle = this->GetStyle ();
            if (dwStyle & WS_DISABLED || dwStyle & ES_READONLY) {
                this->OnBadUserInput (); // should throw out
            }
        }
        inherited::AboutToUpdateText (updateInfo);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::DidUpdateText
    @DESCRIPTION:   <p>Override to hook @'MarkerOwner::DidUpdateText' and call @'Led_Win32_Helper<BASE_INTERACTOR>::DidUpdateText_'.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::DidUpdateText (const UpdateInfo& updateInfo) noexcept
    {
        inherited::DidUpdateText (updateInfo);
        this->DidUpdateText_ (updateInfo);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::DidUpdateText_
    @DESCRIPTION:   <p>When the text is modified between clicks, that should reset any click count. Besides the logic of this,
        its actually IMPORTANT todo in case the change in text invalidates the fDragAnchor.</p>
    */
    inline void Led_Win32_Helper<BASE_INTERACTOR>::DidUpdateText_ (const UpdateInfo& updateInfo) noexcept
    {
        if (updateInfo.fTextModified) {
            this->SetCurClickCount (0, Foundation::Time::GetTickCount ());
        }
        fDragAnchor = min (fDragAnchor, this->GetEnd ()); // SPR#0637 reported we could sometimes get crash while mousing/typing. This SHOULD prevent fDragAnchor ever
        // getting invalid... NB: I never reproduced the problem.
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType)
    {
        if (this->GetScrollBarType (vh) != scrollBarType) {
            inherited::SetScrollBarType (vh, scrollBarType);
            HWND hWnd = this->GetHWND ();
            if (hWnd != nullptr) {
                ::InvalidateRect (hWnd, nullptr, true);
            }
        }
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::ShouldUpdateHScrollBar
    @DESCRIPTION:   <p>Return true if Led_Win32_Helper<BASE_INTERACTOR> should automaticly generate
            @'Led_Win32_Helper<BASE_INTERACTOR>::SetHScrollInfo' calls.</p>
            <p>See also @'Led_Win32_Helper<BASE_INTERACTOR>::SetHScrollInfo',
            @'Led_Win32_Helper<BASE_INTERACTOR>::ShouldUpdateVScrollBar'.</p>
    */
    bool Led_Win32_Helper<BASE_INTERACTOR>::ShouldUpdateHScrollBar () const
    {
        //NB: we must update sbar even if NEVER - when the style is ON, cuz we may need to HIDE the sbar
        return this->GetScrollBarType (TextInteractor::h) != TextInteractor::eScrollBarNever or (this->GetStyle () & WS_HSCROLL);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::ShouldUpdateHScrollBar
    @DESCRIPTION:   <p>See  @'Led_Win32_Helper<BASE_INTERACTOR>::ShouldUpdateHScrollBar'.</p>
    */
    bool Led_Win32_Helper<BASE_INTERACTOR>::ShouldUpdateVScrollBar () const
    {
        //NB: we must update sbar even if NEVER - when the style is ON, cuz we may need to HIDE the sbar
        return this->GetScrollBarType (TextInteractor::v) != TextInteractor::eScrollBarNever or (GetStyle () & WS_VSCROLL);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::TypeAndScrollInfoSBVisible
    @ACCESS:        protected
    @DESCRIPTION:   <p>Check if given the ScrollBarType and scrollInfo - if the scrollbar itself should be visible (possibly disabled)
                or not (invisible).</p>
    */
    bool Led_Win32_Helper<BASE_INTERACTOR>::TypeAndScrollInfoSBVisible (ScrollBarType scrollbarAppears, const SCROLLINFO& scrollInfo) const
    {
        return (scrollbarAppears == TextInteractor::eScrollBarAlways) or
               (scrollbarAppears == TextInteractor::eScrollBarAsNeeded and
                scrollInfo.nMin + static_cast<int> (scrollInfo.nPage) <= scrollInfo.nMax);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::GetHScrollInfo
    @DESCRIPTION:   <p>Simple, virtual wrapper on GetScrollInfo (SB_HORZ,...) call. Virtual so
        you can easily override it in a Led subclass, and use a source of scrollbars, other than
        the default - and still leverage the scrollbar maintainance logic in Led_Win32_Helper<BASE_INTERACTOR>.
        For example, if you had your OWN scroll bars in some outer window, which OWNED a Led subwindow,
        this would be an easy way to tie them together.</p>
            <p>See also @'Led_Win32_Helper<BASE_INTERACTOR>::SetHScrollInfo', @'Led_Win32_Helper<BASE_INTERACTOR>::GetVScrollInfo'.</p>
    */
    SCROLLINFO Led_Win32_Helper<BASE_INTERACTOR>::GetHScrollInfo (UINT nMask) const
    {
        ::SCROLLINFO scrollInfo{};
        scrollInfo.cbSize = sizeof (scrollInfo);
        scrollInfo.fMask  = nMask;
        Verify (::GetScrollInfo (this->GetValidatedHWND (), SB_HORZ, &scrollInfo));
        return scrollInfo;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::SetHScrollInfo
    @DESCRIPTION:   <p>See @'Led_Win32_Helper<BASE_INTERACTOR>::GetHScrollInfo'</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::SetHScrollInfo (ScrollBarType scrollbarAppears, const SCROLLINFO& scrollInfo, bool redraw)
    {
        using namespace Characters::Literals;
        bool showBar = TypeAndScrollInfoSBVisible (scrollbarAppears, scrollInfo);

        DbgTrace ("Led_Win32_Helper<>::SetHScrollInfo  (scrollbarAppears={}, smin={}, smax={}, nPage={}, nPos={}) ==> showBar={})"_f,
                  static_cast<int> (scrollbarAppears), scrollInfo.nMin, scrollInfo.nMax, scrollInfo.nPage, scrollInfo.nPos, showBar);

        /*
         *  As near as I can tell - the below call to ::SetScrollInfo () should be sufficient to show/hide the SBAR. And - often
         *  it is. But - unfortunately - sometimes its not. This seems to be the only way to assure it always gets re-shown.
         *      -- LGP 2003-11-05
         */
        ::ShowScrollBar (this->GetValidatedHWND (), SB_HORZ, showBar);
        SCROLLINFO si = scrollInfo;
        if (showBar) {
            si.fMask |= SIF_DISABLENOSCROLL;
        }
        else {
            si.fMask &= ~SIF_DISABLENOSCROLL;
        }
        (void)::SetScrollInfo (this->GetValidatedHWND (), SB_HORZ, &si, redraw);
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::SetHScrollInfo (const SCROLLINFO& scrollInfo, bool redraw)
    {
        this->SetHScrollInfo (this->GetScrollBarType (TextInteractor::h), scrollInfo, redraw);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::GetVScrollInfo
    @DESCRIPTION:   <p>See @'Led_Win32_Helper<BASE_INTERACTOR>::GetHScrollInfo'</p>
    */
    SCROLLINFO Led_Win32_Helper<BASE_INTERACTOR>::GetVScrollInfo (UINT nMask) const
    {
        ::SCROLLINFO scrollInfo{};
        scrollInfo.cbSize = sizeof (scrollInfo);
        scrollInfo.fMask  = nMask;
        Verify (::GetScrollInfo (this->GetValidatedHWND (), SB_VERT, &scrollInfo));
        return scrollInfo;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::SetVScrollInfo
    @DESCRIPTION:   <p>See @'Led_Win32_Helper<BASE_INTERACTOR>::GetHScrollInfo'</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::SetVScrollInfo (ScrollBarType scrollbarAppears, const SCROLLINFO& scrollInfo, bool redraw)
    {
        using namespace Characters::Literals;
        bool showBar = TypeAndScrollInfoSBVisible (scrollbarAppears, scrollInfo);
        DbgTrace ("Led_Win32_Helper<>::SetVScrollInfo  (scrollbarAppears={}, smin={}, smax={}, nPage={}, nPos={}) ==> showBar={})"_f,
                  static_cast<int> (scrollbarAppears), scrollInfo.nMin, scrollInfo.nMax, scrollInfo.nPage, scrollInfo.nPos, showBar);

        /*
         *  As near as I can tell - the below call to ::SetScrollInfo () should be sufficient to show/hide the SBAR. And - often
         *  it is. But - unfortunately - sometimes its not. This seems to be the only way to assure it always gets re-shown.
         *      -- LGP 2003-11-05
         */
        ::ShowScrollBar (this->GetValidatedHWND (), SB_VERT, showBar);
        SCROLLINFO si = scrollInfo;
        if (showBar) {
            si.fMask |= SIF_DISABLENOSCROLL;
        }
        else {
            si.fMask &= ~SIF_DISABLENOSCROLL;
        }
        (void)::SetScrollInfo (this->GetValidatedHWND (), SB_VERT, &si, redraw);
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::SetVScrollInfo (const SCROLLINFO& scrollInfo, bool redraw)
    {
        this->SetVScrollInfo (this->GetScrollBarType (TextInteractor::v), scrollInfo, redraw);
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::InvalidateScrollBarParameters ()
    {
        HWND hWnd = this->GetHWND ();
        if (hWnd != nullptr) { // only if we've been realized!
            Assert (::IsWindow (hWnd));
            inherited::InvalidateScrollBarParameters_ ();
            this->InvalidateScrollBarParameters_ ();
        }
    }
    template <typename BASE_INTERACTOR>
    inline void Led_Win32_Helper<BASE_INTERACTOR>::InvalidateScrollBarParameters_ ()
    {
        this->InvalidateCaretState ();
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::InvalidateCaretState ()
    {
        /*
         *  Note that we simply invalidate the area where the caret will go (or where it was)
         *  and we actually update the position in our draw method because otherwise
         *  if the user code changed the selection many times at once - we might see the caret
         *  dance all over the screen. We could force an update on a timer even just
         *  in case we never get a draw event - and that might be wise - but the problem is
         *  that this might still happen while the owning software was performing so
         *  long procedure. For now - we'll keep things simple. Maybe later if a need appears
         *  we can use a timer - or some other notification.
         */
        HWND hWnd = this->GetHWND ();
        if (hWnd != nullptr) {
            Assert (::IsWindow (hWnd));
            ::HideCaret (hWnd);
        }
        inherited::InvalidateCaretState ();
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::UpdateCaretState_ ()
    {
        HWND hWnd = this->GetHWND ();
        if (hWnd != nullptr) {
            if (this->GetCaretShown () and this->GetCaretShownSituation () and ::GetFocus () == hWnd) {
                // also turn off/on based on if empty selection...

                ::HideCaret (hWnd);

                Led_Rect caretRect = this->CalculateCaretRect ();

                if (caretRect.IsEmpty ()) {
#if qStroika_Frameworks_Led_ProvideIMESupport
                    // if caret is to be invisible, then make sure the IME is moved
                    // offscreen (maybe should be hidden?) - SPR#1359
                    Led_Rect wr = this->GetWindowRect ();
                    IME::Get ().NotifyPosition (hWnd, (SHORT)wr.GetLeft (), (SHORT)wr.GetBottom () + 1000);
#endif
                }
                else {
                    // NB: I tried being clever and not calling ::CreateCaret () if the height hadn't changed,
                    // but I just got burned for my troubles. It seems when other windows create the caret, it
                    // obliterates our creation. And we get no notification, and as far as I can tell - cannot
                    // check the characteristics of the current caret. So we must re-create!!! Ugly - but seems
                    // to do no harm (no flicker) - LGP 950215
                    ::CreateCaret (hWnd, (HBITMAP)0, caretRect.GetWidth (), caretRect.GetHeight ());

                    /*
                    *   Somewhat risky approach to updating the caret - but the braindead win32 API really gives
                    *   us little choice. ShowCaret MAY NOT show caret. Either cuz we don't own it, it hasn't
                    *   been created, or cuz there have been too many hidecaret calls. We assure the first two
                    *   aren't the case, and then use a while loop to take care of the third problem. But if MS
                    *   intruduces some forth - and this fails - we could infinite loop here - UGH - LGP 950323
                    */
                    Assert (::IsWindow (hWnd));
                    // This SHOULD work, according to the docs, but seems to hang on HT-J 3.5??? - LGP 950524
                    // Seems to work on all versions since NTJ 3.5 however...
                    while (not ::ShowCaret (hWnd)) {
                    }

                    ::SetCaretPos (caretRect.GetLeft (), caretRect.GetTop ());

// When we support the IME - it probably needs to be notified here!!!
#if qStroika_Frameworks_Led_ProvideIMESupport
                    IME::Get ().NotifyPosition (hWnd, (SHORT)caretRect.GetLeft (), (SHORT)caretRect.GetTop ());
#endif
                }
            }
            else {
                ::HideCaret (hWnd);
            }
        }
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::UpdateScrollBars
    @DESCRIPTION:   <p>Hook the @'TextInteractor::UpdateScrollBars' () notification routine to update the values in
        our scrollbars.</p>
    */
    void Led_Win32_Helper<BASE_INTERACTOR>::UpdateScrollBars ()
    {
        using namespace Characters::Literals;
        DbgTrace ("Led_Win32_Helper<>::UpdateScrollBars () with winStart={}, winEnd={})"_f, this->GetMarkerPositionOfStartOfWindow (),
                  this->GetMarkerPositionOfEndOfWindow ());

// Don't allow SetVScrollInfo/SetHScrollInfo () calls during a thumb track - because MS Windows scrollbar
// control SOMETIMES doesn't react well (npos not properly adjusted) when you reset the page size during
// a drag of the thumb.
// It APEARS BUG IS THAT IT DOESNT CHANGE GRAPHIC SIZE OF THUMB WHILE IN THE MIDDLE OF A THUMB DRAG -
// EXCEPT!!! IF WE RESIZE WINDOW AT SAME TIME (by chaning
// whther HORZ scrollbar is shown due to dynamic compuation of it being needed - and THEN we get it resized at that point).
// That makes compute of if we are at end of sbar or where we are almost impossible (cuz we don't know graphic/visual size of thumb)
//      -- LGP 2003-11-04
#if qScrollTextDuringThumbTracking
        if (fSBarThumbTracking) {
            return;
        }
#endif

        inherited::UpdateScrollBars_ ();

        HWND hWnd = this->GetHWND ();
        if (hWnd != nullptr) {
// Hmm. Think this is wrong ... Wrong place for this ... Leave alone for now. Fix later (and HORZ CASE TOO) LGP 970107
#if qSupportWindowsSDKCallbacks
            (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (GetWindowID (), EN_VSCROLL), (LPARAM)hWnd);
#endif

            if (this->ShouldUpdateHScrollBar ()) {
                ::SCROLLINFO scrollInfo;
                (void)::memset (&scrollInfo, 0, sizeof (scrollInfo));
                scrollInfo.cbSize = sizeof (scrollInfo);
                scrollInfo.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;

                scrollInfo.nMin  = 0; // always use zero as base
                scrollInfo.nPage = this->GetWindowRect ().GetWidth ();
                scrollInfo.nMax  = scrollInfo.nPage + this->ComputeMaxHScrollPos ();
                scrollInfo.nPos  = this->GetHScrollPos ();

                // set pagesize ++ since that appears to be what the MS SDK expects. They expect
                // min+page > (not >=) max implies the evevator is full, and nPos+nPage> (not >=) nMax implies at END OF DOC
                //          -- LGP 2003-11-05
                ++scrollInfo.nPage;

                this->SetHScrollInfo (this->GetScrollBarType (TextInteractor::h), scrollInfo);
            }
            if (this->ShouldUpdateVScrollBar ()) {
                size_t startOfWindow      = this->GetMarkerPositionOfStartOfWindow ();
                size_t endOfWindow        = this->GetMarkerPositionOfEndOfWindow ();
                size_t verticalWindowSpan = endOfWindow - startOfWindow;

                ::SCROLLINFO scrollInfo;
                (void)::memset (&scrollInfo, 0, sizeof (scrollInfo));
                scrollInfo.cbSize = sizeof (scrollInfo);
                scrollInfo.fMask  = SIF_PAGE | SIF_POS | SIF_RANGE;

                scrollInfo.nMin  = 0; // always use zero as base
                scrollInfo.nMax  = static_cast<int> (this->GetLength ());
                scrollInfo.nPage = static_cast<UINT> (verticalWindowSpan);
                scrollInfo.nPos  = static_cast<int> (startOfWindow);

                // set pagesize ++ since that appears to be what the MS SDK expects. They expect
                // min+page > (not >=) max implies the evevator is full, and nPos+nPage> (not >=) nMax implies at END OF DOC
                //          -- LGP 2003-11-05
                ++scrollInfo.nPage;

                this->SetVScrollInfo (this->GetScrollBarType (TextInteractor::v), scrollInfo);
            }
        }
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnNormalLButtonDown (UINT /*nFlags*/, const Led_Point& at)
    {
        this->BreakInGroupedCommands ();
        fMouseTrackingLastPoint = at;
        if (not(this->GetStyle () & WS_DISABLED)) {
            HWND hWnd = this->GetValidatedHWND ();

            if (::GetFocus () != hWnd) {
                (void)::SetFocus (hWnd);
            }

            bool extendSelection = !!(::GetKeyState (VK_SHIFT) & 0x8000);

            if (not this->ProcessSimpleClick (at, this->GetCurClickCount (), extendSelection, &fDragAnchor)) {
                return;
            }

            /*
                *  Note that we do this just AFTER setting the first selection. This is to avoid
                *  (if we had done it earlier) displaying the OLD selection and then quickly
                *  erasing it (flicker). We do an Update () rather than simply use eImmediateUpdate
                *  mode on the SetSelection () because at this point we want to redisplay the entire
                *  window if part of it needed it beyond just the part within the selection.
                */
            //  Update_ ();
            this->Update ();

            (void)::SetCapture (hWnd); //  We could loop doing a sub-eventloop here - but it seems more common
            //  Windows practice to just store up state information and wait on
            //  WM_MOUSEMOVE and WM_LButtonUp messages...
            this->StartAutoscrollTimer ();

            Assert (fDragAnchor <= this->GetEnd ()); // Subtle point. fDragAnchor is passed as a hidden variable to
            // other routines - but ONLY when we have the CAPTURE. If the capture
            // is not set, then 'fDragAnchor' is assumed to have an invalid value,
            // This implies that text shouldn't be changed while we have the mouse
            // captured.
        }
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::StartAutoscrollTimer ()
    {
        /*
         *  Not sure about this - just for debug sake??? - Did LButonUp get dropped on the floor somehow? - LGP 960530
         */
        Assert (fAutoScrollTimerID == 0);

        if (fAutoScrollTimerID == 0) {
            const int kTimeout = 20; // 20 milliseconds - update autoscroll every 1/50
                                     // second.
            Verify ((fAutoScrollTimerID = ::SetTimer (this->GetValidatedHWND (), eAutoscrollingTimerEventID, kTimeout, nullptr)) != 0);
        }
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::StopAutoscrollTimer ()
    {
        if (fAutoScrollTimerID != 0) {
            Verify (::KillTimer (this->GetValidatedHWND (), eAutoscrollingTimerEventID));
            fAutoScrollTimerID = 0;
        }
    }
    template <typename BASE_INTERACTOR>
    bool Led_Win32_Helper<BASE_INTERACTOR>::OnCopyCommand_Before ()
    {
        if (not ::OpenClipboard (this->GetValidatedHWND ())) {
            this->OnBadUserInput ();
            return false;
        }
        (void)::EmptyClipboard (); // should we test for errors?
        bool result = inherited::OnCopyCommand_Before ();
        if (not result) {
            Verify (::CloseClipboard ());
            return false;
        }
        return result;
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnCopyCommand_After ()
    {
        inherited::OnCopyCommand_After ();
        if (not ::CloseClipboard ()) {
            this->OnBadUserInput ();
        }
    }
    template <typename BASE_INTERACTOR>
    bool Led_Win32_Helper<BASE_INTERACTOR>::OnPasteCommand_Before ()
    {
        if (not ::OpenClipboard (this->GetValidatedHWND ())) {
            this->OnBadUserInput ();
            return false;
        }
        bool result = inherited::OnPasteCommand_Before ();
        if (not result) {
            Verify (::CloseClipboard ());
            return false;
        }
        return result;
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_Helper<BASE_INTERACTOR>::OnPasteCommand_After ()
    {
        inherited::OnPasteCommand_After ();
        if (not ::CloseClipboard ()) {
            this->OnBadUserInput ();
        }
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_Helper<BASE_INTERACTOR>::GetStyle
    @DESCRIPTION:   <p>Return the HWND's style (::GetWindowLong(...,GWL_STYLE). If the
                HWND is nullptr, then return a style of 0.</p>
    */
    nonvirtual DWORD Led_Win32_Helper<BASE_INTERACTOR>::GetStyle () const
    {
        HWND hWnd = GetHWND ();
        if (hWnd == nullptr) {
            return 0;
        }
        else {
            Assert (::IsWindow (hWnd));
            return (DWORD)::GetWindowLong (hWnd, GWL_STYLE);
        }
    }
    template <typename BASE_INTERACTOR>
    inline int Led_Win32_Helper<BASE_INTERACTOR>::GetWindowID () const
    {
        return (::GetWindowLong (this->GetValidatedHWND (), GWL_ID));
    }
    template <typename BASE_INTERACTOR>
    inline HWND Led_Win32_Helper<BASE_INTERACTOR>::GetValidatedHWND () const
    {
        HWND hWnd = GetHWND ();
        AssertNotNull (hWnd);
        Assert (::IsWindow (hWnd));
        return (hWnd);
    }

//class Led_Win32_Helper<BASE_INTERACTOR>::TemporarilyUseTablet
#if !qNestedClassesInTemplateClassesDontExpandCompilerBug
    template <typename BASE_INTERACTOR>
    inline Led_Win32_Helper<BASE_INTERACTOR>::TemporarilyUseTablet::TemporarilyUseTablet (Led_Win32_Helper<BASE_INTERACTOR>& editor,
                                                                                          Tablet* t, DoTextMetricsChangedCall tmChanged)
        : fEditor{editor}
        , fOldTablet{editor.fUpdateTablet}
        , fDoTextMetricsChangedCall{tmChanged}
    {
        editor.fUpdateTablet = t;
        if (tmChanged == eDoTextMetricsChangedCall) {
            editor.TabletChangedMetrics ();
        }
    }
    template <typename BASE_INTERACTOR>
    inline Led_Win32_Helper<BASE_INTERACTOR>::TemporarilyUseTablet::~TemporarilyUseTablet ()
    {
        fEditor.fUpdateTablet = fOldTablet;
        if (fDoTextMetricsChangedCall == eDoTextMetricsChangedCall) {
            fEditor.TabletChangedMetrics ();
        }
    }
#endif

    //class Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>
    template <typename BASECLASS>
    Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::Led_Win32_Win32SDKMessageMimicHelper ()
        : inherited{}
    {
    }
    template <typename BASECLASS>
    /*
    @METHOD:        Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::HandleMessage
    @DESCRIPTION:   <p>Handle the given windows message, and return true if handled, and false otherwise. If returns
                true, then 'lResult' is set on exit.</p>
    */
    bool Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::HandleMessage (UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
    {
        RequireNotNull (result);
        switch (message) {
            case WM_SETTEXT:
                *result = this->OnMsgSetText (wParam, lParam);
                return true;
            case WM_GETTEXT:
                *result = this->OnMsgGetText (wParam, lParam);
                return true;
            case WM_GETTEXTLENGTH:
                *result = this->OnMsgGetTextLength (wParam, lParam);
                return true;
            case EM_GETSEL:
                *result = this->OnMsgGetSel (wParam, lParam);
                return true;
            case EM_SETREADONLY:
                *result = this->OnMsgSetReadOnly (wParam, lParam);
                return true;
            case EM_GETFIRSTVISIBLELINE:
                *result = this->OnMsgGetFirstVisibleLine (wParam, lParam);
                return true;
            case EM_LINEINDEX:
                *result = this->OnMsgLineIndex (wParam, lParam);
                return true;
            case EM_GETLINECOUNT:
                *result = this->OnMsgLineCount (wParam, lParam);
                return true;
            case EM_CANUNDO:
                *result = this->OnMsgCanUndo (wParam, lParam);
                return true;
            case EM_UNDO:
                *result = this->OnMsgUndo (wParam, lParam);
                return true;
            case EM_EMPTYUNDOBUFFER:
                *result = this->OnMsgEmptyUndoBuffer (wParam, lParam);
                return true;
            case WM_CLEAR:
                *result = this->OnMsgClear (wParam, lParam);
                return true;
            case WM_CUT:
                *result = this->OnMsgCut (wParam, lParam);
                return true;
            case WM_COPY:
                *result = this->OnMsgCopy (wParam, lParam);
                return true;
            case WM_PASTE:
                *result = this->OnMsgPaste (wParam, lParam);
                return true;
            case EM_LINEFROMCHAR:
                *result = this->OnMsgLineFromChar (wParam, lParam);
                return true;
            case EM_LINELENGTH:
                *result = this->OnMsgLineLength (wParam, lParam);
                return true;
            case EM_LINESCROLL:
                *result = this->OnMsgLineScroll (wParam, lParam);
                return true;
            case EM_REPLACESEL:
                *result = this->OnMsgReplaceSel (wParam, lParam);
                return true;
            case EM_SETSEL:
                *result = this->OnMsgSetSel (wParam, lParam);
                return true;
            case EM_SCROLLCARET:
                *result = this->OnMsgScrollCaret (wParam, lParam);
                return true;
            case WM_GETFONT:
                *result = this->OnMsgGetFont (wParam, lParam);
                return true;
            case WM_SETFONT:
                *result = this->OnMsgSetFont (wParam, lParam);
                return true;
            default:
                return false;
        }
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgGetText (WPARAM wParam, LPARAM lParam)
    {
        using namespace Stroika::Foundation;
        int   cchTextMax = (int)wParam;
        LPSTR lpText     = (LPSTR)lParam;
        Require (cchTextMax > 0); // cuz we require appending NUL character

        size_t                         len = this->GetLength ();
        Memory::StackBuffer<Led_tChar> buf{Memory::eUninitialized, len};
        this->CopyOut (0, len, buf.data ());
        size_t                         len2 = 2 * len;
        Memory::StackBuffer<Led_tChar> buf2{Memory::eUninitialized, len2};
        len2 = Characters::NLToNative<Led_tChar> (buf.data (), len, buf2.data (), len2);
        // Assume they want ANSI code page text?
        int nChars     = ::WideCharToMultiByte (CP_ACP, 0, buf2.data (), static_cast<int> (len2), lpText, cchTextMax - 1, nullptr, nullptr);
        lpText[nChars] = '\0';
        return nChars;
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgSetText (WPARAM /*wParam*/, LPARAM lParam)
    {
        using namespace Stroika::Foundation;
        LPSTR lpText = (LPSTR)lParam;

        /*
            *  Note that by doing a Delete followed by an Insert() instead of just doing
            *  a replace - we are in-effect interpretting this as destroying all
            *  markers. And - also - this SetTextPtr () operation could fail in the middle
            *  leaving an empty text buffer as the result.
            */
        this->Replace (0, this->GetEnd (), LED_TCHAR_OF (""), 0);
        if (lpText != nullptr) {
            size_t                         len = ::strlen (lpText);
            Memory::StackBuffer<Led_tChar> buf{Memory::eUninitialized, len};
            // Assume they want ANSI code page text?
            len = static_cast<size_t> (::MultiByteToWideChar (CP_ACP, 0, lpText, static_cast<int> (len), buf.data (), static_cast<int> (len)));
            len = Characters::NormalizeTextToNL<Led_tChar> (buf.data (), len, buf.data (), len);
            this->Replace (0, 0, buf.data (), len);
        }

        return 0;
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgGetTextLength (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        //*2 cuz of CRLF worst case, and *2 cuz of unicode->MBYTE worst case (is that so - worst case? - maybe not needed to multiply there).
        return (4 * this->GetLength ()); // always enuf for unicode chars...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgGetSel (WPARAM wParam, LPARAM lParam)
    {
        DWORD iSelStart = static_cast<DWORD> (this->GetSelectionStart ());
        DWORD iSelEnd   = static_cast<DWORD> (this->GetSelectionEnd ());
        if (wParam != 0) {
            *reinterpret_cast<DWORD*> (wParam) = iSelStart;
        }
        if (lParam != 0) {
            *reinterpret_cast<DWORD*> (lParam) = iSelEnd;
        }
        DWORD dw = (((WORD)iSelStart) << 16) | ((WORD)iSelEnd);
        return dw;
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgGetFirstVisibleLine (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        Assert (this->GetTopRowInWindow () >= 0);
        return this->GetTopRowInWindow ();
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgSetReadOnly (WPARAM wParam, LPARAM /*lParam*/)
    {
        HWND  hWnd     = this->GetValidatedHWND ();
        DWORD dwStyle  = this->GetStyle ();
        bool  readOnly = !!wParam;
        if (readOnly) {
            dwStyle |= ES_READONLY;
        }
        else {
            dwStyle &= ~ES_READONLY;
        }
        DWORD dwStyle1 = ::GetWindowLong (hWnd, GWL_STYLE);
        if (dwStyle != dwStyle1) {
            ::SetWindowLong (hWnd, GWL_STYLE, dwStyle);
            {
                TextInteractor* ti = this;
                ti->Refresh ();
            } // funky refresh call to avoid ambiguity when baseclass has other 'Refresh' methods - as in COleControl
            if (this->GetCommandHandler () != nullptr) {
                this->GetCommandHandler ()->Commit ();
            }
        }
        return 1;
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgLineIndex (WPARAM wParam, LPARAM /*lParam*/)
    {
        size_t row = (int (wParam) == -1) ? this->GetRowContainingPosition (this->GetSelectionEnd ()) : (size_t (wParam));
        if (row < 0) {
            row = 0;
        }
        row = min (row, this->GetRowCount () - 1);
        return (this->GetStartOfRow (row));
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgLineCount (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        return this->GetRowCount (); // 1-based for SDK API here...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgCanUndo (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        CommandHandler* ch = this->GetCommandHandler ();
        return ch != nullptr and ch->CanUndo ();
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgUndo (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        CommandHandler* ch = this->GetCommandHandler ();
        if (ch != nullptr and ch->CanUndo ()) {
            this->OnUndoCommand ();
            return (true);
        }
        return (false);
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgEmptyUndoBuffer (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        CommandHandler* ch = this->GetCommandHandler ();
        if (ch != nullptr) {
            ch->Commit ();
        }
        return 0; // return value ignored...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgClear (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        this->OnClearCommand ();
#if qSupportWindowsSDKCallbacks
        HWND hWnd = this->GetValidatedHWND ();
        (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (::GetWindowLong (hWnd, GWL_ID), EN_CHANGE), (LPARAM)hWnd);
#endif
        return 0; // return value ignored...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgCut (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        this->OnCutCommand ();
#if qSupportWindowsSDKCallbacks
        HWND hWnd = this->GetValidatedHWND ();
        (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (::GetWindowLong (hWnd, GWL_ID), EN_CHANGE), (LPARAM)hWnd);
#endif
        return 0; // return value ignored...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgCopy (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        this->OnCopyCommand ();
        return 0; // return value ignored...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgPaste (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        this->OnPasteCommand ();
#if qSupportWindowsSDKCallbacks
        HWND hWnd = this->GetValidatedHWND ();
        (void)::SendMessage (::GetParent (hWnd), WM_COMMAND, MAKELONG (::GetWindowLong (hWnd, GWL_ID), EN_CHANGE), (LPARAM)hWnd);
#endif
        return 0; // return value ignored...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgLineFromChar (WPARAM wParam, LPARAM /*lParam*/)
    {
        ptrdiff_t nIndex = ptrdiff_t (wParam);
        if (nIndex == -1) {
            nIndex = static_cast<ptrdiff_t> (this->GetSelectionStart ());
        }
        if (nIndex < 0) {
            nIndex = 0;
        }
        Assert (nIndex >= 0); // so we can cast to size_t safely
        if (size_t (nIndex) > this->GetEnd ()) {
            nIndex = this->GetEnd ();
        }
        return this->GetRowContainingPosition (nIndex);
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgLineLength (WPARAM wParam, LPARAM /*lParam*/)
    {
        size_t row = (int (wParam) == -1) ? this->GetRowContainingPosition (this->GetSelectionEnd ()) : (size_t (wParam));
        if (row < 0) {
            row = 0;
        }
        if (row > this->GetRowCount () - 1) {
            row = this->GetRowCount () - 1;
        }
        return this->GetEndOfRow (row) - this->GetStartOfRow (row); // Not sure if we should count NL, or not???
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgLineScroll (WPARAM wParam, LPARAM lParam)
    {
        [[maybe_unused]] int nChars = int (wParam); // NOTE THAT FOR NOW WE IGNORE nChars since we only
        // support word-wrapped text for this release...
        int nLines = int (lParam);
        this->ScrollByIfRoom (nLines);
        return !!(this->GetStyle () & ES_MULTILINE);
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgReplaceSel ([[maybe_unused]] WPARAM wParam, LPARAM lParam)
    {
        using namespace Stroika::Foundation;
        Assert (wParam == 0);
        LPCTSTR text = (LPCTSTR)lParam;

        size_t                         len = ::_tcslen (text);
        Memory::StackBuffer<Led_tChar> buf{Memory::eUninitialized, len};

#if qTargetPlatformSDKUseswchar_t
        //::_tcscpy (buf, text);
        (void)::memcpy (buf.begin (), text, (len + 1) * sizeof (text[0]));
#else
        len = ::MultiByteToWideChar (CP_ACP, 0, text, len, buf, len); // Assume they want ANSI code page text?
#endif
        size_t nLen = Characters::NormalizeTextToNL<Led_tChar> (buf.data (), len, buf.data (), len);
        Assert (ValidateTextForCharsetConformance (buf.data (), nLen));
        this->Replace (this->GetSelectionStart (), this->GetSelectionEnd (), buf.data (), nLen);
        return 0; // result ignored...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgSetSel (WPARAM wParam, LPARAM lParam)
    {
        // This code doesn't EXACTLY reproduce the quirks of the SDK Docs for this function
        // (in particular if nStart == 0, and removing a caret) - but it should be close
        // enough...
        ptrdiff_t nStart = ptrdiff_t (wParam);
        ptrdiff_t nEnd   = ptrdiff_t (lParam);
        if (nEnd == -1) {
            nEnd = this->GetLength ();
        }
        if (nStart < 0) {
            nStart = 0;
        }
        if (nEnd < 0) {
            nEnd = 0;
        }
        if (size_t (nEnd) > this->GetEnd ()) {
            nEnd = static_cast<ptrdiff_t> (this->GetEnd ());
        }
        if (nStart > nEnd) {
            swap (nStart, nEnd);
        }
        this->SetSelection (static_cast<ptrdiff_t> (nStart), static_cast<ptrdiff_t> (nEnd));
        return 0; // result ignored...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgScrollCaret (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        this->ScrollToSelection ();
        return 0; // result ignored...
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgGetFont (WPARAM /*wParam*/, LPARAM /*lParam*/)
    {
        /*
         *      just use fDefaultFontCache as a convenient way to keep track of this object
         *  so we delete it in the end...
         *
         *      But - we want to be careful not only delete this when need be. We could update
         *  this cache every time the font changes. But I'd rather have all this localized,
         *  and only pay the price when this function is called. This function is probably
         *  quite rarely called (certainly not by anything within Led itself!).
         *
         *      The reason we want to delete this only when we have to, is because we have no
         *  way of knowing who kept refernces to the HFONT we return. So we delete it only
         *  when we change fonts, and someone asks again - and on DTOR of this object.
         */
        FontSpecification defaultFont = this->GetDefaultSelectionFont ();
        LOGFONT           defaultFontLF;
        defaultFont.GetOSRep (&defaultFontLF);

        if (fDefaultFontCache.m_hObject != nullptr) { // Seeing if font changed...
            FontObject tmpHackToTestFont;
            Verify (tmpHackToTestFont.CreateFontIndirect (&defaultFontLF));

            ::LOGFONT tmpHackToTestFontLF{};
            Verify (tmpHackToTestFont.GetObject (sizeof (tmpHackToTestFontLF), &tmpHackToTestFontLF));

            ::LOGFONT currentLF{};
            Verify (fDefaultFontCache.GetObject (sizeof (currentLF), &currentLF));

            if (::memcmp (&currentLF, &tmpHackToTestFontLF, sizeof (currentLF)) != 0) {
                fDefaultFontCache.DeleteObject ();
                Assert (fDefaultFontCache.m_hObject == nullptr);
            }
        }

        if (fDefaultFontCache.m_hObject == nullptr) {
            fDefaultFontCache.CreateFontIndirect (&defaultFontLF);
        }
        return (LRESULT)fDefaultFontCache.m_hObject;
    }
    template <typename BASECLASS>
    LRESULT Led_Win32_Win32SDKMessageMimicHelper<BASECLASS>::OnMsgSetFont (WPARAM wParam, LPARAM lParam)
    {
        IncrementalFontSpecification fontSpec;
        {
            HFONT fontToUse = reinterpret_cast<HFONT> (wParam);
            if (fontToUse == nullptr) {
                fontToUse = reinterpret_cast<HFONT> (::GetStockObject (DEFAULT_GUI_FONT));
            }
            LOGFONT lf;
            ::GetObject (fontToUse, sizeof (lf), &lf);
            fontSpec.SetOSRep (lf);
        }
        this->SetDefaultFont (fontSpec, lParam ? TextInteractor::eDefaultUpdate : TextInteractor::eNoUpdate);
        return 0;
    }

    //  class   SimpleWin32WndProcHelper
    inline SimpleWin32WndProcHelper::SimpleWin32WndProcHelper ()
        : fHWnd{nullptr}
        , fSuperWindowProc{nullptr}
    {
    }
    inline HWND SimpleWin32WndProcHelper::GetHWND () const
    {
        return fHWnd;
    }
    inline void SimpleWin32WndProcHelper::SetHWND (HWND hWnd)
    {
        if (fHWnd != nullptr) {
            ::SetWindowLongPtr (fHWnd, GWLP_USERDATA, 0); // reset back to original value
        }
        fHWnd = hWnd;
        if (fHWnd != nullptr) {
            ::SetWindowLongPtr (fHWnd, GWLP_USERDATA, reinterpret_cast<DWORD_PTR> (this));
        }
    }
    inline HWND SimpleWin32WndProcHelper::GetValidatedHWND () const
    {
        HWND hWnd = this->GetHWND ();
        AssertNotNull (hWnd);
        Assert (::IsWindow (hWnd));
        return (hWnd);
    }
    inline void SimpleWin32WndProcHelper::Create (LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth,
                                                  int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance)
    {
        Create (0L, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance);
    }
    inline LRESULT SimpleWin32WndProcHelper::SendMessage (UINT msg, WPARAM wParam, LPARAM lParam)
    {
#if !qTargetPlatformSDKUseswchar_t
        if (IsWindowUNICODE ()) {
            return ::SendMessageW (this->GetValidatedHWND (), msg, wParam, lParam);
        }
#endif
        return ::SendMessage (this->GetValidatedHWND (), msg, wParam, lParam);
    }
    inline bool SimpleWin32WndProcHelper::IsWindowRealized () const
    {
        return this->GetHWND () != nullptr;
    }
    inline void SimpleWin32WndProcHelper::Assert_Window_Realized () const
    {
        Assert (this->IsWindowRealized ());
    }
    inline void SimpleWin32WndProcHelper::Require_Window_Realized () const
    {
        Require (this->IsWindowRealized ());
    }
    inline bool SimpleWin32WndProcHelper::IsWindowUNICODE () const
    {
        Require_Window_Realized ();
        return !!::IsWindowUnicode (GetHWND ());
    }
    inline bool SimpleWin32WndProcHelper::IsWindowShown () const
    {
        Require_Window_Realized ();
        return !!::IsWindowVisible (GetHWND ());
    }
    inline void SimpleWin32WndProcHelper::SetWindowVisible (bool shown)
    {
        Require_Window_Realized ();
        (void)::ShowWindow (GetHWND (), shown ? SW_SHOW : SW_HIDE);
    }
    /*
    @METHOD:        SimpleWin32WndProcHelper::SubclassWindow
    @DESCRIPTION:   <p>See also @'SimpleWin32WndProcHelper::SubclassWindowW'.</p>
    */
    inline bool SimpleWin32WndProcHelper::SubclassWindow (HWND hWnd)
    {
        Require (fSuperWindowProc == nullptr); // don't call twice!
        Require (fHWnd == nullptr);            // don't call after already created! - use this instead of SetHWnd ()!!!
        RequireNotNull (hWnd);
        Require (::IsWindow (hWnd));
        fSuperWindowProc = reinterpret_cast<WNDPROC> (::SetWindowLongPtr (hWnd, GWLP_WNDPROC, reinterpret_cast<DWORD_PTR> (StaticWndProc)));
        if (fSuperWindowProc == nullptr) {
            return false;
        }
        SetHWND (hWnd);
        return true;
    }
    /*
    @METHOD:        SimpleWin32WndProcHelper::SubclassWindowW
    @DESCRIPTION:   <p>Same as @'SimpleWin32WndProcHelper::SubclassWindow' except that we create a 'UNICODE' window.</p>
    */
    inline bool SimpleWin32WndProcHelper::SubclassWindowW (HWND hWnd)
    {
        Require (fSuperWindowProc == nullptr); // don't call twice!
        Require (fHWnd == nullptr);            // don't call after already created! - use this instead of SetHWnd ()!!!
        RequireNotNull (hWnd);
        Require (::IsWindow (hWnd));
        fSuperWindowProc = reinterpret_cast<WNDPROC> (::SetWindowLongPtr (hWnd, GWLP_WNDPROC, reinterpret_cast<DWORD_PTR> (StaticWndProc)));
        if (fSuperWindowProc == nullptr) {
            return false;
        }
        SetHWND (hWnd);
        return true;
    }
    inline LRESULT SimpleWin32WndProcHelper::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
    {
        return this->DefWindowProc (message, wParam, lParam);
    }
    inline LRESULT SimpleWin32WndProcHelper::DefWindowProc (UINT message, WPARAM wParam, LPARAM lParam)
    {
#if !qTargetPlatformSDKUseswchar_t
        if (IsWindowUNICODE ()) {
            if (fSuperWindowProc == nullptr) {
                return ::DefWindowProcW (this->GetValidatedHWND (), message, wParam, lParam);
            }
            else {
                return ::CallWindowProcW (fSuperWindowProc, this->GetValidatedHWND (), message, wParam, lParam);
            }
        }
#endif
        if (fSuperWindowProc == nullptr) {
            return ::DefWindowProc (this->GetValidatedHWND (), message, wParam, lParam);
        }
        else {
            return ::CallWindowProc (fSuperWindowProc, this->GetValidatedHWND (), message, wParam, lParam);
        }
    }

    //  class   Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>
    template <typename BASE_WIN32_HELPER>
    Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::Led_Win32_SimpleWndProc_Helper ()
        : BASE_WIN32_HELPER{}
        , fHWnd{nullptr}
        , fSuperWindowProc{nullptr}
    {
    }
    template <typename BASE_WIN32_HELPER>
    Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::~Led_Win32_SimpleWndProc_Helper ()
    {
        if (GetHWND () != nullptr) {
            ::DestroyWindow (GetHWND ());
        }
    }
    template <typename BASE_WIN32_HELPER>
    void Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::Create (LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y,
                                                                    int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance)
    {
        Create (0L, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance);
    }
    template <typename BASE_WIN32_HELPER>
    /*
    @METHOD:        Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::Create
    @DESCRIPTION:   <p>Create a window - using the Windows SDK routine CreateWindowEx, but hooked to a Led class.
                </p>
                    <p>See also @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::SubclassWindow' and
                @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::ReplaceWindow'.
                </p>
    */
    void Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::Create (DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName,
                                                                    DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent,
                                                                    HMENU hMenu, HINSTANCE hInstance)
    {
        SDKString tmpClassName;
        if (lpClassName == nullptr) {
            tmpClassName = Foundation::Characters::CString::Format (_T("Led_Win32_SimpleWndProc_Helper<>-%d-%p"), ::GetCurrentProcessId (), &StaticWndProc);
            lpClassName = tmpClassName.c_str ();
            {
                static bool sRegistered = false;
                if (not sRegistered) {
                    WNDCLASSEX wcex;
                    memset (&wcex, 0, sizeof (wcex));
                    wcex.cbSize        = sizeof (WNDCLASSEX);
                    wcex.lpfnWndProc   = (WNDPROC)StaticWndProc;
                    wcex.lpszClassName = lpClassName;
                    ATOM regResult     = ::RegisterClassEx (&wcex);
                    if (regResult == 0) {
                        DWORD lastErr = ::GetLastError ();
                        if (lastErr == ERROR_CLASS_ALREADY_EXISTS) {
                            // Shouldn't happen - but if it does - SB OK since StaticWndProc addr is the same!
                        }
                        else {
                            Assert (false); // could be a problem?
                        }
                    }
                    sRegistered = true;
                }
            }
        }
        [[maybe_unused]] HWND hWnd =
            ::CreateWindowEx (dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, this);
        Assert (hWnd == this->GetValidatedHWND ()); // already pre-set on the WM_CREATE message...
    }
    template <typename BASE_WIN32_HELPER>
    void Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::SetHWND (HWND hWnd)
    {
        if (fHWnd != nullptr) {
            ::SetWindowLongPtr (fHWnd, GWLP_USERDATA, 0); // reset back to original value
        }
        fHWnd = hWnd;
        if (fHWnd != nullptr) {
            ::SetWindowLongPtr (fHWnd, GWLP_USERDATA, reinterpret_cast<DWORD_PTR> (this));
        }
    }
    template <typename BASE_WIN32_HELPER>
    HWND Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::GetHWND () const
    {
        return fHWnd;
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::OnNCDestroy_Msg ()
    {
        (void)DefWindowProc (WM_NCDESTROY, 0, 0);
        SetHWND (nullptr);
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::SubclassWindow
    @DESCRIPTION:   <p>Used instead of @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::Create' to hook an
                existing window.</p>
                    <p>See also @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::ReplaceWindow'.
                </p>
    */
    bool Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::SubclassWindow (HWND hWnd)
    {
        Require (fSuperWindowProc == nullptr); // don't call twice!
        Require (fHWnd == nullptr);            // don't call after already created! - use this instead of SetHWnd ()!!!
        RequireNotNull (hWnd);
        Require (::IsWindow (hWnd));
        fSuperWindowProc = reinterpret_cast<WNDPROC> (::SetWindowLong (hWnd, GWLP_WNDPROC, reinterpret_cast<LONG> (StaticWndProc)));
        if (fSuperWindowProc == nullptr) {
            return false;
        }
        SetHWND (hWnd);
        HookSubclassWindow ();
        return true;
    }
    template <typename BASE_INTERACTOR>
    /*
    @METHOD:        Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::ReplaceWindow
    @DESCRIPTION:   <p>Used instead of @'Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::SubclassWindow' to replace
                an existing window. Instead of using window subclassing, and chaining handlers, just outright REPLACE
                the window.
                </p>
                    <p>This seems like a somewhat unconventional thing todo, but it apears much safer, and less quirky. I've found
                that SUBCLASSING a window often produces slightly weird results (perhaps depnding on WHICH window you choose
                to subclass). Anyhow - this works well if you define a widget in your .rc file, and then use this to replace it
                with the roughly equivalently sized etc Led version.
                    </p>
                    <p>Returns false on failure, and true on success.</p>
    */
    bool Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::ReplaceWindow (HWND hWnd)
    {
        Require (fSuperWindowProc == nullptr); // don't call twice!
        Require (fHWnd == nullptr);            // don't call after already created! - use this instead of SetHWnd ()!!!
        RequireNotNull (hWnd);
        Require (::IsWindow (hWnd));

        HWND parent = ::GetParent (hWnd);
        if (parent == nullptr) {
            return false;
        }
        int id = ::GetWindowLong (hWnd, GWL_ID);

        Assert (hWnd == ::GetDlgItem (parent, id));

        DWORD dwStyle = ::GetWindowLong (hWnd, GWL_STYLE);
        DWORD exStyle = ::GetWindowLong (hWnd, GWL_EXSTYLE);

        // Assume previous widget's position.
        WINDOWPLACEMENT wp;
        memset (&wp, 0, sizeof (wp));
        wp.length = sizeof (wp);
        Verify (::GetWindowPlacement (hWnd, &wp));

        LOGFONT useFont;
        bool    justUseSystemFont = true;
        {
            HFONT hFont = nullptr;
            if ((hFont = (HFONT)::SendMessage (hWnd, WM_GETFONT, 0, 0L)) != nullptr) {
                if (::GetObject (hFont, sizeof (LOGFONT), &useFont)) {
                    justUseSystemFont = false;
                }
            }
        }

        // Delete the old widget window.
        ::DestroyWindow (hWnd);
        DISABLE_COMPILER_MSC_WARNING_START (4312)
        Create (exStyle, nullptr, nullptr, dwStyle | WS_CHILD, wp.rcNormalPosition.left, wp.rcNormalPosition.top,
                wp.rcNormalPosition.right - wp.rcNormalPosition.left, wp.rcNormalPosition.bottom - wp.rcNormalPosition.top, parent, (HMENU)id, nullptr);
        DISABLE_COMPILER_MSC_WARNING_END (4312)

        /*
            *  Copy the font value from the original widget and set it into the replaced one.
            */
        {
            FontObject fontToUse;
            if (not justUseSystemFont) {
                Verify (fontToUse.CreateFontIndirect (&useFont));
            }
            bool redrawFlag = true;
            (void)::SendMessage (this->GetValidatedHWND (), WM_SETFONT,
                                 justUseSystemFont ? NULL : reinterpret_cast<WPARAM> (static_cast<HFONT> (fontToUse)), redrawFlag);
        }
        return true;
    }
    template <typename BASE_INTERACTOR>
    void Led_Win32_SimpleWndProc_Helper<BASE_INTERACTOR>::HookSubclassWindow ()
    {
        HWND  hWnd    = this->GetValidatedHWND ();
        DWORD dwStyle = ::GetWindowLong (hWnd, GWL_STYLE);
        if ((dwStyle & WS_VSCROLL) and this->GetScrollBarType (TextInteractor::v) == TextInteractor::eScrollBarNever) {
            this->SetScrollBarType (TextInteractor::v, TextInteractor::eScrollBarAlways);
        }
        if ((dwStyle & WS_HSCROLL) and this->GetScrollBarType (TextInteractor::h) == TextInteractor::eScrollBarNever) {
            this->SetScrollBarType (TextInteractor::h, TextInteractor::eScrollBarAlways);
        }
        this->OnSize_Msg ();
    }
    template <typename BASE_WIN32_HELPER>
    LRESULT CALLBACK Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_CREATE) {
            ::LPCREATESTRUCT lpcs = (::LPCREATESTRUCT)lParam;
            AssertNotNull (lpcs);
            Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>* pThis =
                reinterpret_cast<Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>*> (lpcs->lpCreateParams);
            Assert (pThis->GetHWND () == nullptr); // cuz not set yet...
            pThis->SetHWND (hWnd);
        }

        Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>* pThis =
            reinterpret_cast<Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>*> (::GetWindowLongPtr (hWnd, GWLP_USERDATA));

        if (pThis == nullptr) {
            /*
             *  The only time I know of where this can happen is that these messages can be passed before the WM_CREATE.
             *  Seeing this assertion isn't necesarily a bug - but its a clue something wrong maybe going on.
             *
             *  As of 2012-09-16 - I see a few new (sb irrelevant) messages...
             */
            Assert (message == WM_GETMINMAXINFO or message == WM_NCCREATE or message == WM_NCCALCSIZE or message == 0x0093 or
                    message == 0x0094 or message == WM_NCMOUSELEAVE);
            return ::DefWindowProc (hWnd, message, wParam, lParam);
        }

        Assert (pThis != nullptr);
        Assert (pThis->GetHWND () == hWnd);
        return pThis->WndProc (message, wParam, lParam);
    }
    template <typename BASE_WIN32_HELPER>
    LRESULT Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
    {
#define MY_GET_X_LPARAM(lp) ((int)(short)LOWORD (lp))
#define MY_GET_Y_LPARAM(lp) ((int)(short)HIWORD (lp))
        switch (message) {
            case WM_CREATE:
                return this->OnCreate_Msg (reinterpret_cast<::LPCREATESTRUCT> (lParam));
            case WM_PAINT:
                this->OnPaint_Msg ();
                break;
            case WM_SIZE:
                this->OnSize_Msg ();
                break;
            case WM_CHAR:
                this->OnChar_Msg (static_cast<UINT> (wParam), lParam);
                break;
            case WM_UNICHAR:
                return this->OnUniChar_Msg (wParam, lParam);
                break;
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
            case WM_IME_CHAR:
                return this->OnIMEChar_Msg (wParam, lParam);
            case WM_IME_COMPOSITION:
                return this->OnIME_COMPOSITION_Msg (wParam, lParam);
            case WM_IME_ENDCOMPOSITION:
                return this->OnIME_ENDCOMPOSITION_Msg (wParam, lParam);
#endif
            case WM_KEYDOWN:
                this->OnKeyDown_Msg (static_cast<UINT> (wParam), lParam);
                break;
            case WM_SETCURSOR:
                return this->OnSetCursor_Msg ((HWND)wParam, LOWORD (lParam), HIWORD (lParam));
            case WM_GETDLGCODE:
                return this->OnGetDlgCode_Msg ();
            case WM_SETFOCUS:
                this->OnSetFocus_Msg (HWND (wParam));
                break;
            case WM_KILLFOCUS:
                this->OnKillFocus_Msg (HWND (wParam));
                break;
            case WM_ERASEBKGND:
                return this->OnEraseBkgnd_Msg (HDC (wParam));
            case WM_TIMER:
                this->OnTimer_Msg (wParam, reinterpret_cast<::TIMERPROC*> (lParam));
                break;
            case WM_LBUTTONDOWN:
                this->OnLButtonDown_Msg (static_cast<UINT> (wParam), MY_GET_X_LPARAM (lParam), MY_GET_Y_LPARAM (lParam));
                break;
            case WM_LBUTTONUP:
                this->OnLButtonUp_Msg (static_cast<UINT> (wParam), MY_GET_X_LPARAM (lParam), MY_GET_Y_LPARAM (lParam));
                break;
            case WM_LBUTTONDBLCLK:
                this->OnLButtonDblClk_Msg (static_cast<UINT> (wParam), MY_GET_X_LPARAM (lParam), MY_GET_Y_LPARAM (lParam));
                break;
            case WM_MOUSEMOVE:
                this->OnMouseMove_Msg (static_cast<UINT> (wParam), MY_GET_X_LPARAM (lParam), MY_GET_Y_LPARAM (lParam));
                break;
            case WM_VSCROLL:
                this->OnVScroll_Msg (LOWORD (wParam), (short)HIWORD (wParam), HWND (lParam));
                break;
            case WM_HSCROLL:
                this->OnHScroll_Msg (LOWORD (wParam), (short)HIWORD (wParam), HWND (lParam));
                break;
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
            case WM_MOUSEWHEEL:
                return this->OnMouseWheel_Msg (wParam, lParam);
#endif
            case WM_ENABLE:
                this->OnEnable_Msg (!!wParam);
                break;
            case WM_NCDESTROY:
                this->OnNCDestroy_Msg ();
                break;
            default:
                return this->DefWindowProc (message, wParam, lParam);
        }
#undef MY_GET_X_LPARAM
#undef MY_GET_Y_LPARAM
        return 0;
    }
    template <typename BASE_WIN32_HELPER>
    LRESULT Led_Win32_SimpleWndProc_Helper<BASE_WIN32_HELPER>::DefWindowProc (UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (fSuperWindowProc == nullptr) {
            return inherited::DefWindowProc (message, wParam, lParam);
        }
        else {
            return (*fSuperWindowProc) (this->GetValidatedHWND (), message, wParam, lParam);
        }
    }

    //  class   Led_Win32_SimpleWndProc_HelperWithSDKMessages<BASE_CLASS>
    template <typename BASE_CLASS>
    inline Led_Win32_SimpleWndProc_HelperWithSDKMessages<BASE_CLASS>::Led_Win32_SimpleWndProc_HelperWithSDKMessages ()
        : inherited{}
    {
    }
    template <typename BASE_CLASS>
    LRESULT Led_Win32_SimpleWndProc_HelperWithSDKMessages<BASE_CLASS>::WndProc (UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT result = 0;
        if (this->HandleMessage (message, wParam, lParam, &result)) {
            return result;
        }
        else {
            return inherited::WndProc (message, wParam, lParam);
        }
    }

}

CompileTimeFlagChecker_HEADER (Stroika::Frameworks::Led::Platform, qSupportWindowsSDKCallbacks, qSupportWindowsSDKCallbacks);
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
CompileTimeFlagChecker_HEADER (Stroika::Frameworks::Led::Platform, qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug, 1);
#else
CompileTimeFlagChecker_HEADER (Stroika::Frameworks::Led::Platform, qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug, 0);
#endif

#endif /*_Stroika_Frameworks_Led_Platform_Windows_h_*/
