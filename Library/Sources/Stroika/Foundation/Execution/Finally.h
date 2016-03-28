/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Finally_h_
#define _Stroika_Foundation_Execution_Finally_h_  1

#include    "../StroikaPreComp.h"

#include    <functional>

#include    "../Configuration/Common.h"



/**
 *  \file
 *
 *      Finally utility - to appoximate C++ try/finally support.
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  This helpful utility to plug a missing feature from C++11 - to have a block of code run at the end
             *  of a scope - regardless of whether you use normal exit or exception based exit.
             *
             *  This has some defects:
             *      o   The cleanup code must be wrapped in a lambda
             *
             *      o   The syntax is ugly - the cleanup having to be declared
             *          BEFORE the block instead of afterwards (languages like C# with try/finally the finally goes
             *          at the end)
             *
             *      o   Exceptions inside of cleanupCodeBlock are suppressed
             *
             *  Inspired by:
             *      http://nerds-central.blogspot.com/2012/03/c11-trycatchfinally-pattern-using-raii.html
             *
             *  \note   For cleanup which is important/critical to be completed, its sometimes wise to include
             *              Thread::SuppressInterruptionInContext suppressThreadInterupts;
             *          in the function body
             */
            DISABLE_COMPILER_GCC_WARNING_START("GCC diagnostic ignored \"[-Wdeprecated-declarations\"");
            class   _Deprecated_("Finally NOW DEPRECATED - USE mkFinally (soon to be replaced with again Finally, but now function)") Finally {
            public:
                Finally (const function<void()>& cleanupCodeBlock);
                template <typename FUNCTION>
                Finally (FUNCTION f, typename is_function<FUNCTION>::type* = nullptr);
                Finally () = delete;
                Finally (const Finally&) = delete;

            public:
                ~Finally();

            public:
                nonvirtual  Finally& operator= (const Finally&) = delete;

            private:
                function<void()> fCleanupCodeBlock_;
            };
            DISABLE_COMPILER_GCC_WARNING_END("GCC diagnostic ignored \"[-Wdeprecated-declarations\"");


            /**
             *  @see Finally<FUNCTION>
             */
            template <typename FUNCTION>
            class   FinallySentry {
            public:
                FinallySentry () = delete;
                FinallySentry (FUNCTION f);
                FinallySentry (FinallySentry&&) = delete;
                FinallySentry (const FinallySentry&) = delete;

            public:
                ~FinallySentry ();

            public:
                nonvirtual  FinallySentry& operator= (const FinallySentry&) = delete;

            private:
                FUNCTION    fCleanupCodeBlock_;
            };


            /**
             *  This helpful utility to plug a missing feature from C++11 - to have a block of code run at the end
             *  of a scope - regardless of whether you use normal exit or exception based exit.
             *
             *  This has some defects:
             *      o   The syntax is ugly - the cleanup having to be declared
             *          BEFORE the block instead of afterwards (languages like C# with try/finally the finally goes
             *          at the end)
             *
             *      o   Exceptions inside of cleanupCodeBlock must be suppressed
             *
             *  Inspired by:
             *      http://nerds-central.blogspot.com/2012/03/c11-trycatchfinally-pattern-using-raii.html
             *
             *  \note   For cleanup which is important/critical to be completed, its sometimes wise to include
             *              Thread::SuppressInterruptionInContext suppressThreadInterupts;
             *          in the function body
             *
             *  \note   This is (now) done in such a way that no locks are used or memory allocated, except if
             *          argument lambda uses them/does it.
             *
             *  EXMAPLE USGE:
             *      auto&& cleanup  =   mkFinally ([] () noexcept { Require (not sKnownBadBeforeMainOrAfterMain_); });
             */
            template <typename FUNCTION>
            auto    mkFinally (FUNCTION f) -> FinallySentry<FUNCTION>;


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Finally.inl"

#endif  /*_Stroika_Foundation_Execution_Finally_h_*/
