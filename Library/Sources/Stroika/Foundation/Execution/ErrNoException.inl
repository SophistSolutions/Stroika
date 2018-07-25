/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    inline errno_ErrorException::operator errno_t () const
    {
        return fError;
    }
    inline SDKString errno_ErrorException::LookupMessage () const
    {
        return LookupMessage (fError);
    }

    /*
     ********************************************************************************
     ***************************** ThrowErrNoIfNegative *****************************
     ********************************************************************************
     */
    template <typename INT_TYPE>
    inline INT_TYPE ThrowErrNoIfNegative (INT_TYPE returnCode)
    {
        if (returnCode < 0) {
            errno_ErrorException::Throw (errno);
        }
        return returnCode;
    }

    /*
     ********************************************************************************
     ***************************** ThrowErrNoIfNull *********************************
     ********************************************************************************
     */
    inline void ThrowErrNoIfNull (void* returnCode)
    {
        if (returnCode == nullptr) {
            errno_ErrorException::Throw (errno);
        }
    }

    /*
     ********************************************************************************
     ************************ Handle_ErrNoResultInterruption ************************
     ********************************************************************************
     */
    template <typename CALL>
    auto Handle_ErrNoResultInterruption (CALL call) -> decltype (call ())
    {
        decltype (call ()) ret; // intentionally uninitialized since alway set at least once before read
        do {
            ret = call ();
            Execution::CheckForThreadInterruption ();
        } while (ret < 0 and errno == EINTR);
        return ThrowErrNoIfNegative (ret);
    }

    /*
     ********************************************************************************
     *************************************** Throw **********************************
     ********************************************************************************
     */
    template <>
    [[noreturn]] inline void Throw (const errno_ErrorException& e2Throw)
    {
        // Go directly through class Throw() since that may remap to different kinds of exceptions, and already has trace messages
        errno_ErrorException::Throw (e2Throw);
    }

    /*
     ********************************************************************************
     ****************************** ThrowIfError_errno_t ****************************
     ********************************************************************************
     */
    inline void ThrowIfError_errno_t (errno_t e)
    {
        if (e != 0) {
            errno_ErrorException::Throw (e);
        }
    }
}

#endif /*_Stroia_Foundation_Execution_ErrNoException_inl_*/
