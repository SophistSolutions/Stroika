/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_FileSystem_Exception_inl_
#define _Stroika_Foundation_IO_FileSystem_Exception_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::IO::FileSystem {

    /*
     ********************************************************************************
     ***************************** FileSystem::Exception ***************************
     ********************************************************************************
     */
    inline Exception::Exception (error_code errCode, const path& p1, const path& p2)
        : inherited (mkMsg_ (errCode, p1, p2), mkMsg_ (errCode, p1, p2).AsNarrowSDKString (), p1, p2, errCode)
    {
        Require (not p1.empty () or p2.empty ()); // if only one path provided, provide it first
    }
    inline Exception::Exception (error_code errCode, const Characters::String& message, const path& p1, const path& p2)
        : inherited (mkMsg_ (errCode, message, p1, p2), mkMsg_ (errCode, message, p1, p2).AsNarrowSDKString (), p1, p2, errCode)
    {
        Require (not p1.empty () or p2.empty ()); // if only one path provided, provide it first
    }
#if qPlatform_Windows
    template <typename WINDOWS_API_RESULT>
    inline static void Exception::ThrowIfZeroGetLastError (WINDOWS_API_RESULT test, const path& p1, const path& p2)
    {
        if (test == 0) {
            ThrowSystemErrNo (::GetLastError (), p1, p2);
        }
    }
#endif
    template <typename INT_TYPE>
    inline INT_TYPE Exception::ThrowPOSIXErrNoIfNegative (INT_TYPE returnCode, const path& p1, const path& p2)
    {
        if (returnCode < 0)
            [[UNLIKELY_ATTR]]
            {
                ThrowPOSIXErrNo (errno, p1, p2);
            }
        return returnCode;
    }

}

#endif /*_Stroika_Foundation_IO_FileSystem_Exception_inl_*/
