/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_Exception_h_
#define _Stroika_Foundation_Execution_Platform_Windows_Exception_h_ 1

#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Characters/SDKString.h"
#include "../../../Configuration/Common.h"
#include "../../../Execution/Exceptions.h"

namespace Stroika::Foundation::Execution {
    using Characters::SDKString;
}

namespace Stroika::Foundation::Execution::Platform::Windows {

    /**
     */
    class Exception : public Execution::SystemErrorException<> {
    public:
        explicit Exception (DWORD error);

        operator DWORD () const;

    public:
        // throw Platform::Windows::Exception () - if error is a real error, and map SOME (like #8 to bad_alloc) - but ALWAYS throw
        // someting, regardless of error#
        [[noreturn]] static void Throw (DWORD error);

    public:
        static SDKString LookupMessage (DWORD hr);
        nonvirtual SDKString LookupMessage () const;

    private:
        DWORD fError;

    public:
        static const DWORD kERROR_INTERNET_TIMEOUT             = 12002;
        static const DWORD kERROR_INTERNET_INVALID_URL         = 12005;
        static const DWORD kERROR_INTERNET_UNRECOGNIZED_SCHEME = 12006;
        static const DWORD kERROR_INTERNET_NAME_NOT_RESOLVED   = 12007;
        static const DWORD kERROR_INTERNET_PROTOCOL_NOT_FOUND  = 12008;
        static const DWORD kERROR_INTERNET_CANNOT_CONNECT      = 12029;
    };

    void ThrowIfFalseGetLastError (bool test);
    void ThrowIfFalseGetLastError (BOOL test);
    void ThrowIfZeroGetLastError (int test);
    void ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode);
    void ThrowIfNot_NO_ERROR (DWORD win32ErrCode);
    void ThrowIfShellExecError (HINSTANCE r);

    /**
     *  If you call RegisterDefaultHandler_invalid_parameter at application startup, subsequent calls
     *  to some C-runtime funtions withh invalid parameters will be translated into Platform::Windows::Exception (ERROR_INVALID_PARAMETER)
     *
     *  @see https://msdn.microsoft.com/en-us/library/a9yf33zb.aspx
     */
    void RegisterDefaultHandler_invalid_parameter ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Exception.inl"

#endif /*_Stroika_Foundation_Execution_Platform_Windows_Exception_h_*/
