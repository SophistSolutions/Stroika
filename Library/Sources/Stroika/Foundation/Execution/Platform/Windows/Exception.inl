/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Platform_Windows_Exception_inl_
#define _Stroika_Foundation_Execution_Platform_Windows_Exception_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../../Characters/SDKString.h"

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

    /****DEPRECATED APIS*****/

    /**
     */
    class [[deprecated ("Since v2.1d18, use Execution::ThrowSystemErrNo")]] Exception : public Execution::SystemErrorException<>
    {
    public:
        explicit Exception (DWORD error);

        operator DWORD () const
        {
            return fError;
        }

    public:
        // throw Platform::Windows::Exception () - if error is a real error, and map SOME (like #8 to bad_alloc) - but ALWAYS throw
        // someting, regardless of error#
        [[noreturn]] static void Throw (DWORD error);

    public:
        static SDKString LookupMessage (DWORD hr);
        inline SDKString Exception::LookupMessage () const
        {
            return LookupMessage (fError);
        }

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

    [[deprecated ("Since v2.1d18, use ThrowIfZeroGetLastError")]] inline void ThrowIfFalseGetLastError (bool test)
    {
        ThrowIfZeroGetLastError (test);
    }
    [[deprecated ("Since v2.1d18, use ThrowIfZeroGetLastError")]] inline void ThrowIfFalseGetLastError (BOOL test)
    {
        ThrowIfZeroGetLastError (test);
    }

}

namespace Stroika::Foundation::Execution {
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    template <>
    [[noreturn]] inline void Throw (const Platform::Windows::Exception& e2Throw)
    {
        // Go directly through class Throw() since that may remap to different kinds of exceptions, and already has trace messages
        Platform::Windows::Exception::Throw (e2Throw);
    }
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_END (4996);
}

#endif /*_Stroika_Foundation_Execution_Platform_Windows_Exception_inl_*/
