/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_ErrNoException_inl_
#define _Stroia_Foundation_Execution_ErrNoException_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Thread.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************* errno_ErrorException ***************************
     ********************************************************************************
     */
    DISABLE_COMPILER_MSC_WARNING_START (4996); // this whole procedure deprecated so no need to warn its impl is as well
    inline errno_ErrorException::operator errno_t () const
    {
        return fError;
    }
    inline SDKString errno_ErrorException::LookupMessage () const
    {
        return LookupMessage (fError);
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996); // this whole procedure deprecated so no need to warn its impl is as well

    /*
     ********************************************************************************
     ************************** ThrowPOSIXErrNoIfNull *******************************
     ********************************************************************************
     */

    template <typename INT_TYPE>
    [[deprecated ("Since v2.1d18, use Execution::ThrowPOSIXErrNoIfNegative")]] inline INT_TYPE ThrowErrNoIfNegative (INT_TYPE returnCode)
    {
        return Execution::ThrowPOSIXErrNoIfNegative (returnCode);
    }

    /*
     ********************************************************************************
     *************************************** Throw **********************************
     ********************************************************************************
     */
    DISABLE_COMPILER_MSC_WARNING_START (4996); // this whole procedure deprecated so no need to warn its impl is as well
    template <>
    [[noreturn]] inline void Throw (const errno_ErrorException& e2Throw)
    {
        // Go directly through class Throw() since that may remap to different kinds of exceptions, and already has trace messages
        errno_ErrorException::Throw (e2Throw);
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996); // this whole procedure deprecated so no need to warn its impl is as well

    /*
     ********************************************************************************
     ****************************** ThrowIfError_errno_t ****************************
     ********************************************************************************
     */
    [[deprecated ("Since v2.1d18, use Execution::ThrowPOSIXErrNo (almost never right to look at errno for == OK - only known API where that makes sense is readdir)")]] inline void ThrowIfError_errno_t (errno_t e = errno)
    {
        if (e != 0) {
            ThrowPOSIXErrNo (e);
        }
    }

}

#endif /*_Stroia_Foundation_Execution_ErrNoException_inl_*/
