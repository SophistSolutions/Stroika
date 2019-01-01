/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_WaitSupport_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_WaitSupport_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Execution::Platform::Windows {

    inline DWORD Duration2Milliseconds (Time::DurationSecondsType nSeconds)
    {
        return (nSeconds > numeric_limits<DWORD>::max () / 2) ? INFINITE : static_cast<DWORD> (nSeconds * 1000);
    }

}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_WaitSupport_inl_*/
