/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_WaitSupport_h_
#define _Stroika_Foundation_Execution_Platform_Windows_WaitSupport_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <vector>

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika::Foundation::Execution::Platform::Windows {

    DWORD Duration2Milliseconds (Time::DurationSeconds nSeconds);

    /**
     * Pump messages and wait the specified amount of time. No exceptions for timeout.
     */
    void WaitAndPumpMessages (HWND dialog = nullptr, Time::DurationSeconds forNSecs = 0.1s);

    /**
     * Pump messages and wait the specified amount of time. No exceptions for timeout. Return if timeout or if handle signaled.
     */
    void WaitAndPumpMessages (HWND dialog, const vector<HANDLE>& waitOn, Time::DurationSeconds forNSecs = 0.1s);

    void PumpMessagesWhileInputAvailable (HWND dialog = nullptr, Time::DurationSeconds atMostNSecs = 0.1s);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WaitSupport.inl"

#endif /*_Stroika_Foundation_Execution_Platform_Windows_WaitSupport_h_*/
