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
    class [[deprecated ("Since v2.1d18, use SystemErrorException{ hr, HRESULT_error_category () }")]]  HRESULTErrorException : public Execution::Exception<>
    {
    private:
        using inherited = Execution::Exception<>;

    public:
        HRESULTErrorException (HRESULT hresult);

    public:
        operator HRESULT () const
        {
            return fHResult;
        }

    public:
        static SDKString LookupMessage (HRESULT hr);
        nonvirtual SDKString LookupMessage () const
        {
            return LookupMessage (fHResult);
        }

    private:
        HRESULT fHResult;
    };

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
    inline void ThrowIfNull<HRESULT> (const void* p, const HRESULT& hr)
    {
        ThrowIfNull (p, SystemErrorException{hr, Platform::Windows::HRESULT_error_category ()});
    }
}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_*/
