/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ************* Platform::Windows::HRESULTErrorException *************************
             ********************************************************************************
             */
            inline Platform::Windows::HRESULTErrorException::operator HRESULT () const
            {
                return fHResult;
            }
            inline SDKString Platform::Windows::HRESULTErrorException::LookupMessage () const
            {
                return LookupMessage (fHResult);
            }

            inline void ThrowIfErrorHRESULT (HRESULT hr)
            {
                if (not SUCCEEDED (hr)) {
                    Throw (Platform::Windows::HRESULTErrorException (hr));
                }
            }

            template <>
            inline void ThrowIfNull<HRESULT> (const void* p, const HRESULT& hr)
            {
                ThrowIfNull (p, Platform::Windows::HRESULTErrorException (hr));
            }
        }
    }
}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_*/
