/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_ErrNoException_inl_
#define _Stroia_Foundation_Execution_ErrNoException_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/Thread.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            //  class   errno_ErrorException
            inline  errno_ErrorException::operator errno_t () const
            {
                return fError;
            }
            inline  TString errno_ErrorException::LookupMessage () const
            {
                return LookupMessage (fError);
            }


            template    <typename INT_TYPE>
            inline  INT_TYPE    ThrowErrNoIfNegative (INT_TYPE returnCode)
            {
                if (returnCode < 0) {
                    errno_ErrorException::DoThrow (errno);
                }
                return returnCode;
            }


#if     qCanGetAutoDeclTypeStuffWorkingForTemplatedFunction
            template    <typename CALL>
            auto    Handle_ErrNoResultInteruption (CALL call)  -> decltype (call)
            {
                auto    ret;
                do {
                    ret = call ();
                    Execution::CheckForThreadAborting ();
                }
                while (ret < 0 && errno == EINTR);
                return ThrowErrNoIfNegative (ret);
            }
#else
            template    <typename CALL>
            int Handle_ErrNoResultInteruption (CALL call)
            {
                int ret;
                do {
                    ret = call ();
                    Execution::CheckForThreadAborting ();
                }
                while (ret < 0 && errno == EINTR);
                return ThrowErrNoIfNegative (ret);
            }
#endif

            template    <>
            inline  void    _NoReturn_  DoThrow (const errno_ErrorException& e2Throw)
            {
                // Go directly through class DoThrow() since that may remap to different kinds of exceptions, and already has trace messages
                errno_ErrorException::DoThrow (e2Throw);
            }
            inline  void    ThrowIfError_errno_t (errno_t e)
            {
                if (e != 0) {
                    errno_ErrorException::DoThrow (e);
                }
            }

        }
    }
}

#endif  /*_Stroia_Foundation_Execution_ErrNoException_inl_*/
