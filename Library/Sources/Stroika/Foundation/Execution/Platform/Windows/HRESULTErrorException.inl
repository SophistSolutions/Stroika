/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            //  class   HRESULTErrorException
            inline  Platform::Windows::HRESULTErrorException::HRESULTErrorException (HRESULT hresult):
                fHResult (hresult)
            {
            }
            inline  Platform::Windows::HRESULTErrorException::operator HRESULT () const
            {
                return fHResult;
            }
            inline  SDKString Platform::Windows::HRESULTErrorException::LookupMessage () const
            {
                return LookupMessage (fHResult);
            }



            inline  void    ThrowIfErrorHRESULT (HRESULT hr)
            {
                if (not SUCCEEDED (hr)) {
                    DoThrow (Platform::Windows::HRESULTErrorException (hr));
                }
            }



            template<>
            inline  void    ThrowIfNull<HRESULT> (const void* p, const HRESULT& hr)
            {
                ThrowIfNull (p, Platform::Windows::HRESULTErrorException (hr));
            }


            template    <>
            inline  void    _NoReturn_  DoThrow (const Platform::Windows::HRESULTErrorException& e2Throw)
            {
                DbgTrace ("Throwing Platform::Windows::HRESULTErrorException: HRESULT = 0x%x", static_cast<HRESULT> (e2Throw));
                throw e2Throw;
            }

        }
    }
}

#endif  /*_Stroika_Foundation_Execution_Platform_Windows_HRESULTErrorException_inl_*/
