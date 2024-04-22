/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Execution::Platform::Windows {

    /*
     ********************************************************************************
     ****************** Platform::Windows::ThrowIfErrorHRESULT **********************
     ********************************************************************************
     */
    inline void ThrowIfErrorHRESULT (HRESULT hr)
    {
        if (not SUCCEEDED (hr)) {
            Throw (SystemErrorException{hr, HRESULT_error_category ()});
        }
    }

}

namespace Stroika::Foundation::Execution {
    template <>
    inline void ThrowIfNull (const void* p, const HRESULT& hr)
    {
        ThrowIfNull (p, SystemErrorException{hr, Platform::Windows::HRESULT_error_category ()});
    }
}
