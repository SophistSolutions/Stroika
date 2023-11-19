/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_WaitSupport_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_WaitSupport_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Execution::Platform::Windows {

    inline DWORD Duration2Milliseconds (Time::DurationSeconds nSeconds)
    {
        return (nSeconds.count () > numeric_limits<DWORD>::max () / 2) ? INFINITE : static_cast<DWORD> (nSeconds.count () * 1000);
    }

}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_WaitSupport_inl_*/
