/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_Exception_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_Exception_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Exceptions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
            namespace   Platform {
                namespace   Windows {

                    //  class   Exception
                    inline  Exception::operator DWORD () const
                    {
                        return fError;
                    }
                    inline  SDKString Exception::LookupMessage () const
                    {
                        return LookupMessage (fError);
                    }

                    inline  void    ThrowIfFalseGetLastError (bool test)
                    {
                        if (not test) {
                            Exception::DoThrow (::GetLastError ());
                        }
                    }
                    inline  void    ThrowIfFalseGetLastError (BOOL test)
                    {
                        if (not test) {
                            Exception::DoThrow (::GetLastError ());
                        }
                    }
                    inline  void    ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
                    {
                        if (win32ErrCode != ERROR_SUCCESS) {
                            Exception::DoThrow (win32ErrCode);
                        }
                    }
                    inline  void    ThrowIfNot_NO_ERROR (DWORD win32ErrCode)
                    {
                        if (win32ErrCode != NO_ERROR) {
                            Exception::DoThrow (win32ErrCode);
                        }
                    }
                }
            }

            template    <>
            inline  void    _NoReturn_  DoThrow (const Platform::Windows::Exception& e2Throw)
            {
                // Go directly through class DoThrow() since that may remap to different kinds of exceptions, and already has trace messages
                Platform::Windows::Exception::DoThrow (e2Throw);
            }
        }
    }
}

#endif  /*_Stroika_Foundation_Execution_Platform_Windows_Exception_inl_*/
