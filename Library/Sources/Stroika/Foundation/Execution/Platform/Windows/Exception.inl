/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_Exception_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_Exception_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Exceptions.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {
            namespace Platform {
                namespace Windows {

                    //  class   Exception
                    inline Exception::operator DWORD () const
                    {
                        return fError;
                    }
                    inline SDKString Exception::LookupMessage () const
                    {
                        return LookupMessage (fError);
                    }

                    inline void ThrowIfFalseGetLastError (bool test)
                    {
                        if (not test) {
                            Exception::Throw (::GetLastError ());
                        }
                    }
                    inline void ThrowIfFalseGetLastError (BOOL test)
                    {
                        if (not test) {
                            Exception::Throw (::GetLastError ());
                        }
                    }
                    inline void ThrowIfZeroGetLastError (int test)
                    {
                        if (test == 0) {
                            Exception::Throw (::GetLastError ());
                        }
                    }
                    inline void ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
                    {
                        if (win32ErrCode != ERROR_SUCCESS) {
                            Exception::Throw (win32ErrCode);
                        }
                    }
                    inline void ThrowIfNot_NO_ERROR (DWORD win32ErrCode)
                    {
                        if (win32ErrCode != NO_ERROR) {
                            Exception::Throw (win32ErrCode);
                        }
                    }
                }
            }

            template <>
            [[noreturn]] inline void Throw (const Platform::Windows::Exception& e2Throw)
            {
                // Go directly through class Throw() since that may remap to different kinds of exceptions, and already has trace messages
                Platform::Windows::Exception::Throw (e2Throw);
            }
        }
    }
}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_Exception_inl_*/
