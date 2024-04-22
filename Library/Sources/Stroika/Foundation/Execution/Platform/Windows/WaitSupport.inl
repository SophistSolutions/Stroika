/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution::Platform::Windows {

    inline DWORD Duration2Milliseconds (Time::DurationSeconds nSeconds)
    {
        return (nSeconds.count () > numeric_limits<DWORD>::max () / 2) ? INFINITE : static_cast<DWORD> (nSeconds.count () * 1000);
    }

}
