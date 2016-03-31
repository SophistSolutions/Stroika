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
 *
 *  TODO:
 *      @todo   COMPLETE RENAME mkFinally to Finally()
 *
 *      @todo   Consider checking noexcept (f), and avoiding the IgnoreExceptionsForCall
 *
 *      @todo   Consider adding optional parameter to auto include context to suppress thread exceptions (since thats a common need)
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  @see Finally<FUNCTION>
             *
             *  Dont use this directly.
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
             *  \note   This automatically suppresses any expressions triggered in the argument function, but its usually best
             *          to pass in a noexcept lambda (future versions may be more efficient in that case).
             *
             *  EXMAPLE USGE:
             *      auto&& cleanup  =   mkFinally ([] () noexcept { Require (not sKnownBadBeforeMainOrAfterMain_); });
             */
            template <typename FUNCTION>
            auto    Finally (FUNCTION f) -> FinallySentry<FUNCTION>;


            template <typename FUNCTION>
            _Deprecated_("use Finally")   auto    mkFinally (FUNCTION f) -> FinallySentry<FUNCTION>;


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
