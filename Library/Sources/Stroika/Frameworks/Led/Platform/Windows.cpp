/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Frameworks/Led/IdleManager.h"

#include "Windows.h"

using namespace Stroika::Foundation;

CompileTimeFlagChecker_SOURCE (Stroika::Frameworks::Led::Platform, qSupportWindowsSDKCallbacks, qSupportWindowsSDKCallbacks);
#if qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug
CompileTimeFlagChecker_SOURCE (Stroika::Frameworks::Led::Platform, qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug, 1);
#else
CompileTimeFlagChecker_SOURCE (Stroika::Frameworks::Led::Platform, qHookIMEEndCompositionMessageToWorkAroundWin2KIMEForNonUNICODEBug, 0);
#endif

namespace Stroika::Frameworks::Led::Platform {

    namespace Private {
        IdleMangerLinkerSupport::IdleMangerLinkerSupport ()
        {
        }
    }

    /*
    @CLASS:         IdleManagerOSImpl_Win32
    @BASES:         @'IdleManager::IdleManagerOSImpl'
    @ACCESS:        public
    @DESCRIPTION:   <p>Implemenation detail of the idle-task management system. This can generally be ignored by Led users.
                </p>
    */
    class IdleManagerOSImpl_Win32 : public IdleManager::IdleManagerOSImpl {
    public:
        IdleManagerOSImpl_Win32 ();
        ~IdleManagerOSImpl_Win32 ();

    public:
        virtual void                              StartSpendTimeCalls () override;
        virtual void                              TerminateSpendTimeCalls () override;
        virtual Foundation::Time::DurationSeconds GetSuggestedFrequency () const override;
        virtual void                              SetSuggestedFrequency (Foundation::Time::DurationSeconds suggestedFrequency) override;

    protected:
        nonvirtual void OnTimer_Msg (UINT_PTR nEventID, TIMERPROC* proc);

    protected:
        nonvirtual void CheckAndCreateIdleWnd ();

    public:
        static LRESULT CALLBACK StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    private:
        enum {
            eTimerEventID = 34252
        }; // Magic#
        HWND                              fIdleWnd;
        Foundation::Time::DurationSeconds fSuggestedFrequency;
        UINT_PTR                          fTimerID;
    };

    namespace {
        /*
            *  Code to automatically install and remove our idle manager.
            */
        struct IdleMangerSetter {
            IdleMangerSetter ()
            {
                IdleManager::SetIdleManagerOSImpl (&fIdleManagerOSImpl);
            }
            ~IdleMangerSetter ()
            {
                IdleManager::SetIdleManagerOSImpl (NULL);
            }
            IdleManagerOSImpl_Win32 fIdleManagerOSImpl;
        };
        struct IdleMangerSetter sIdleMangerSetter;
    }

    /*
        ********************************************************************************
        ******************** FunnyMSPageUpDownAdjustSelectionHelper ********************
        ********************************************************************************
        */
    FunnyMSPageUpDownAdjustSelectionHelper::FunnyMSPageUpDownAdjustSelectionHelper ()
        : fRowNum (0)
    {
    }

    void FunnyMSPageUpDownAdjustSelectionHelper::CaptureInfo (TextInteractor& ti)
    {
        size_t pinPoint = ti.GetSelectionStart ();
        ptrdiff_t rowNum = ti.CalculateRowDeltaFromCharDeltaFromTopOfWindow (long (pinPoint) - long (ti.GetMarkerPositionOfStartOfWindow ()));
        fRowNum = ::abs (rowNum);
    }

    void FunnyMSPageUpDownAdjustSelectionHelper::CompleteAdjustment (TextInteractor& ti)
    {
        // Finish the crazy caret adjustments!
        size_t totalRowsInWindow = ti.GetTotalRowsInWindow (); // not can have changed from above due to scrolling
        Assert (totalRowsInWindow >= 1);
        fRowNum = min (fRowNum, totalRowsInWindow - 1);

        size_t newRowStart = ti.CalculateCharDeltaFromRowDeltaFromTopOfWindow (fRowNum) + ti.GetMarkerPositionOfStartOfWindow ();
        size_t newRowEnd   = ti.GetEndOfRowContainingPosition (newRowStart);
        Assert (newRowEnd >= newRowStart);

        TextImager::GoalColumnRecomputerControlContext skipRecompute (ti, true);

        size_t newSelStart = ti.GetRowRelativeCharAtLoc (TextImager::Tablet_Acquirer (&ti)->CvtFromTWIPSH (ti.GetSelectionGoalColumn ()), newRowStart);

        newSelStart = min (newSelStart, newRowEnd); // pin to END of row

        // Ha! Finally!
        ti.SetSelection (newSelStart, newSelStart);
    }

    /*
        ********************************************************************************
        **************************** SimpleWin32WndProcHelper **************************
        ********************************************************************************
        */

    /*
    @METHOD:        SimpleWin32WndProcHelper::Create
    @DESCRIPTION:   <p></p>
    */
    void SimpleWin32WndProcHelper::Create (DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y,
                                           int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance)
    {
        SDKString tmpClassName;
        if (lpClassName == NULL) {
            tmpClassName = Characters::CString::Format (_T("LED_SimpleWin32WndProcHelper-%d-%p"), ::GetCurrentProcessId (), &StaticWndProc);
            lpClassName  = tmpClassName.c_str ();
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
                        DWORD laserErr = ::GetLastError ();
                        if (laserErr == ERROR_CLASS_ALREADY_EXISTS) {
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
        Assert (hWnd == GetValidatedHWND ()); // already pre-set on the WM_CREATE message...
    }

    LRESULT CALLBACK SimpleWin32WndProcHelper::StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_CREATE) {
            LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
            AssertNotNull (lpcs);
            SimpleWin32WndProcHelper* pThis = reinterpret_cast<SimpleWin32WndProcHelper*> (lpcs->lpCreateParams);
            Assert (pThis->GetHWND () == NULL); // cuz not set yet...
            pThis->SetHWND (hWnd);
        }

        SimpleWin32WndProcHelper* pThis = reinterpret_cast<SimpleWin32WndProcHelper*> (::GetWindowLongPtr (hWnd, GWLP_USERDATA));

        if (pThis == NULL) {
            /*
                *   The only time I know of where this can happen is that these messages can be passed before the WM_CREATE.
                *   Seeing this assertion isn't necesarily a bug - but its a clue something wrong maybe going on.
                */
            Assert (message == WM_GETMINMAXINFO or message == WM_NCCREATE or message == WM_NCCALCSIZE);
            return ::DefWindowProc (hWnd, message, wParam, lParam);
        }

        Assert (pThis != NULL);
        Assert (pThis->GetHWND () == hWnd);
        return pThis->WndProc (message, wParam, lParam);
    }

    /*
        ********************************************************************************
        **************************** IdleManagerOSImpl_Win32 ***************************
        ********************************************************************************
        */
    IdleManagerOSImpl_Win32::IdleManagerOSImpl_Win32 ()
        : fIdleWnd (NULL)
        , fSuggestedFrequency (0)
        , fTimerID (0)
    {
    }

    IdleManagerOSImpl_Win32::~IdleManagerOSImpl_Win32 ()
    {
        if (fIdleWnd != NULL) {
            ::DestroyWindow (fIdleWnd);
        }
    }

    void IdleManagerOSImpl_Win32::StartSpendTimeCalls ()
    {
        CheckAndCreateIdleWnd ();
        AssertNotNull (fIdleWnd);
        // ignore if already started
        if (fTimerID == 0) {
            int timeout = static_cast<int> (fSuggestedFrequency.count () * 1000); // cvt specified frequency to milliseconds
            Verify ((fTimerID = ::SetTimer (fIdleWnd, eTimerEventID, timeout, NULL)) != 0);
        }
    }

    void IdleManagerOSImpl_Win32::TerminateSpendTimeCalls ()
    {
        if (fTimerID != 0) {
            AssertNotNull (fIdleWnd);
            Verify (::KillTimer (fIdleWnd, eTimerEventID));
            fTimerID = 0;
        }
    }

    Foundation::Time::DurationSeconds IdleManagerOSImpl_Win32::GetSuggestedFrequency () const
    {
        return fSuggestedFrequency;
    }

    void IdleManagerOSImpl_Win32::SetSuggestedFrequency (Foundation::Time::DurationSeconds suggestedFrequency)
    {
        if (fSuggestedFrequency != suggestedFrequency) {
            fSuggestedFrequency = suggestedFrequency;
            if (fTimerID != 0) {
                TerminateSpendTimeCalls ();
                StartSpendTimeCalls ();
            }
        }
    }

    LRESULT CALLBACK IdleManagerOSImpl_Win32::StaticWndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_CREATE) {
            LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT> (lParam);
            AssertNotNull (lpcs);
            IdleManagerOSImpl_Win32* pThis = reinterpret_cast<IdleManagerOSImpl_Win32*> (lpcs->lpCreateParams);
            ::SetWindowLongPtr (hWnd, GWLP_USERDATA, reinterpret_cast<DWORD_PTR> (pThis));
        }

        IdleManagerOSImpl_Win32* pThis = reinterpret_cast<IdleManagerOSImpl_Win32*> (::GetWindowLongPtr (hWnd, GWLP_USERDATA));

        if (pThis == NULL) {
            return ::DefWindowProc (hWnd, message, wParam, lParam);
        }

        Assert (pThis != NULL);
        switch (message) {
            case WM_TIMER:
                pThis->OnTimer_Msg (wParam, reinterpret_cast<TIMERPROC*> (lParam));
                return 0;
        }
        return ::DefWindowProc (hWnd, message, wParam, lParam);
    }

    void IdleManagerOSImpl_Win32::OnTimer_Msg (UINT_PTR /*nEventID*/, TIMERPROC* /*proc*/)
    {
/*
            *  Check if any input or paint messages pending, and if so - ignore the timer message as
            *  this isn't really IDLE time.
            */
#if defined(PM_QS_INPUT) || defined(PM_QS_PAINT)
        MSG msg;
        if (::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE | PM_QS_INPUT | PM_QS_PAINT) == 0) {
            CallSpendTime ();
        }
#else
        //PM_QS_INPUT and PM_QS_PAINT not defined in MSVC60
        CallSpendTime ();
#endif
    }

    void IdleManagerOSImpl_Win32::CheckAndCreateIdleWnd ()
    {
        if (fIdleWnd == NULL) {
            // Because of SPR#1549 - make sure the className depends on the ADDRESS of StaticWndProc
            SDKString className = Characters::CString::Format (Led_SDK_TCHAROF ("Led::IdleManagerOSImpl_Win32 (%p)"), &StaticWndProc);
            static SDKString sRegisteredClassName;
            if (sRegisteredClassName != className) {
                WNDCLASSEX wcex;
                memset (&wcex, 0, sizeof (wcex));
                wcex.cbSize        = sizeof (WNDCLASSEX);
                wcex.lpfnWndProc   = (WNDPROC)StaticWndProc;
                wcex.lpszClassName = className.c_str ();
                ATOM regResult     = ::RegisterClassEx (&wcex);
                if (regResult == 0) {
                    DWORD x = ::GetLastError ();
                    if (x == ERROR_CLASS_ALREADY_EXISTS) {
                        // ignore - probably multiple template instantiations, or something like that. SB OK. They are all identical.
                    }
                    else {
                        Assert (false); // could be a problem?
                    }
                }
                sRegisteredClassName = className;
            }
            fIdleWnd = ::CreateWindowEx (0, className.c_str (), _T(""), 0, 0, 0, 1, 1, NULL, NULL, NULL, this);
        }
    }
}
