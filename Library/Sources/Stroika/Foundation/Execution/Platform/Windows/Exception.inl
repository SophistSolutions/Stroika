/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Characters/SDKString.h"

#include "../../Throw.h"

namespace Stroika::Foundation::Execution {
    using Characters::SDKString;
}

namespace Stroika::Foundation::Execution::Platform::Windows {

    /*
     ********************************************************************************
     *************************** ThrowIfZeroGetLastError ****************************
     ********************************************************************************
     */
    template <typename WINDOWS_API_RESULT>
    inline void ThrowIfZeroGetLastError (WINDOWS_API_RESULT test)
    {
        if (test == 0) {
            Execution::ThrowSystemErrNo (::GetLastError ()); // NOTE - REQUIRE GetLastError () MUST be != 0!!! Else assert error
        }
    }

    /*
     ********************************************************************************
     *************************** ThrowIfNotERROR_SUCCESS ****************************
     ********************************************************************************
     */
    inline void ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
    {
        if (win32ErrCode != ERROR_SUCCESS) {
            Execution::ThrowSystemErrNo (win32ErrCode);
        }
    }

    /*
     ********************************************************************************
     ***************************** ThrowIfNot_NO_ERROR ******************************
     ********************************************************************************
     */
    inline void ThrowIfNot_NO_ERROR (DWORD win32ErrCode)
    {
        if (win32ErrCode != NO_ERROR) {
            Execution::ThrowSystemErrNo (win32ErrCode);
        }
    }

}
