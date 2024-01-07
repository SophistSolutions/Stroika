/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Configuration/Common.h"
#include "../../../Containers/Common.h"
#include "../../../Debug/Trace.h"
#include "../../../Time/Realtime.h"

#include "../../Thread.h"

#include "WaitSupport.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Execution::Platform;
using namespace Stroika::Foundation::Execution::Platform::Windows;

/*
 ********************************************************************************
 ********************************** WaitSupport *********************************
 ********************************************************************************
 */

/*
 *  Call this if you want to pump messages and want to block/wait for a while if need be (to avoid busy-waiting).
 */
void Windows::WaitAndPumpMessages (HWND dialog, Time::DurationSeconds forNSecs)
{
    Windows::WaitAndPumpMessages (dialog, vector<HANDLE> (), forNSecs);
}

void Windows::WaitAndPumpMessages (HWND dialog, const vector<HANDLE>& waitOn, Time::DurationSeconds forNSecs)
{
    Time::TimePointSeconds startAt = Time::GetTickCount ();
    Time::TimePointSeconds endAt   = startAt + forNSecs;

    for (Time::DurationSeconds timeLeft = endAt - Time::GetTickCount (); timeLeft > 0s; timeLeft = endAt - Time::GetTickCount ()) {
        Thread::CheckForInterruption ();
        DWORD waitResult = ::MsgWaitForMultipleObjectsEx (static_cast<DWORD> (waitOn.size ()), Containers::Start (waitOn),
                                                          Platform::Windows::Duration2Milliseconds (timeLeft), QS_ALLEVENTS, MWMO_INPUTAVAILABLE);
        if (WAIT_OBJECT_0 <= waitResult and waitResult < WAIT_OBJECT_0 + waitOn.size ()) {
            return;
        }
        MSG msg;
        while (::PeekMessage (&msg, nullptr, 0, 0, PM_REMOVE)) {
            try {
                if (dialog == nullptr or not ::IsDialogMessage (dialog, &msg)) {
                    ::TranslateMessage (&msg);
                    ::DispatchMessage (&msg);
                }
            }
            catch (...) {
            }
            timeLeft = endAt - Time::GetTickCount ();
            if (timeLeft < 0s) {
                break;
            }
        }
    }
}

/*
 *  Call this if you want to pump messages, but return immediately if none available (e.g. when you are
 *  doing something else).
 */
void Windows::PumpMessagesWhileInputAvailable (HWND dialog, Time::DurationSeconds atMostNSecs)
{
    Time::TimePointSeconds startAt = Time::GetTickCount ();
    Time::TimePointSeconds endAt   = startAt + atMostNSecs;

    MSG msg;
    while (::PeekMessage (&msg, nullptr, 0, 0, PM_REMOVE)) {
        try {
            if (dialog == nullptr or not ::IsDialogMessage (dialog, &msg)) {
                ::TranslateMessage (&msg);
                ::DispatchMessage (&msg);
            }
        }
        catch (...) {
        }
        if (endAt < Time::GetTickCount ()) {
            break; // should be only rarely triggered, except if atMostNSecs <= 0
        }
    }
}
