/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Execution::Platform::Windows {

    /*
     ********************************************************************************
     **************** Platform::Windows::HRESULTErrorException **********************
     ********************************************************************************
     */
    inline HRESULTErrorException::operator HRESULT () const
    {
        return fHResult;
    }
    inline SDKString HRESULTErrorException::LookupMessage () const
    {
        return LookupMessage (fHResult);
    }

    /*
     ********************************************************************************
     ****************** Platform::Windows::ThrowIfErrorHRESULT **********************
     ********************************************************************************
     */
    inline void ThrowIfErrorHRESULT (HRESULT hr)
    {
        if (not SUCCEEDED (hr)) {
            Throw (Platform::Windows::HRESULTErrorException{hr});
        }
    }

}

namespace Stroika::Foundation::Execution {
    template <>
    inline void ThrowIfNull<HRESULT> (const void* p, const HRESULT& hr)
    {
        ThrowIfNull (p, Platform::Windows::HRESULTErrorException{hr});
    }
}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_*/
