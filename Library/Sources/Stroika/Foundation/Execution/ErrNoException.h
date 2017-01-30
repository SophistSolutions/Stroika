/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroia_Foundation_Execution_ErrNoException_h_
#define _Stroia_Foundation_Execution_ErrNoException_h_ 1

#include "../StroikaPreComp.h"

#include <cerrno>
#include <exception>

#include "StringException.h"

/**
 *  TODO:
 *
 *  @todo   Redo exceptions to be more C++11 frinedly
 *          (DOCUMENTED � but NYI)
 *          <system_error>
 *          http://en.cppreference.com/w/cpp/error
 *          It has building copyale excpetions.
 *          It has builtin errno type exceptions and error CLASSIFICATION (very heflpful)
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            using Characters::SDKString;

#if qCompilerAndStdLib_Supports_errno_t
            using errno_t = ::errno_t;
#else
            using errno_t = int;
#endif

            /**
             */
            class errno_ErrorException : public StringException {
            public:
                explicit errno_ErrorException (errno_t e);

                operator errno_t () const;

            public:
                static SDKString LookupMessage (errno_t e);
                nonvirtual SDKString LookupMessage () const;

            public:
                // throw errno_ErrorException () - unless I can find another Win32Exception, or bad_alloc() or some such which is
                // as good a fit.
                [[noreturn]] static void Throw (errno_t error);

            private:
                errno_t fError;
            };

            /*
             * Many - mostly POSIX - APIs - return a number which is zero if good, or -1 if errno is set and there is an error.
             * This function is useful for wrapping calls to those style functions. It checks if the argument result is negative (so -1 covers that) and
             * throws and errno_ErrorException () excpetion.
             */
            template <typename INT_TYPE>
            INT_TYPE ThrowErrNoIfNegative (INT_TYPE returnCode);

            void ThrowErrNoIfNull (void* returnCode);

            /**
             *  Run the given (argument) call. After each call, invoke Execution::CheckForThreadInterruption ().
             *  If the call returns < 0 and errno == EINTR, repeat the call.
             *  If the result was < 0, but errno != EINTR, then ThrowErrNoIfNegative ();
             *  Then return the result.
             *
             *  This behavior is meant to work with the frequent POSIX API semantics of a return value of < 0
             *  implying an error, and < 0 but errno == EINTR means retry the call. This API also provides a
             *  cancelation point - so it makes otherwise blocking calls (like select, or read) work well with thread
             *  interruption.
             */
            template <typename CALL>
            auto Handle_ErrNoResultInterruption (CALL call) -> decltype (call ());

            // Just pre-declare Throw><> template here so we can specailize
            template <typename T>
            [[noreturn]] void Throw (const T& e2Throw);
            template <>
            [[noreturn]] void Throw (const errno_ErrorException& e2Throw);

            /*
             * Throw if errno is non-zero. Not an error - silently ignored - if errno is 0 (NOERROR)
             */
            void ThrowIfError_errno_t (errno_t e = errno);
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ErrNoException.inl"

#endif /*_Stroia_Foundation_Execution_ErrNoException_h_*/
