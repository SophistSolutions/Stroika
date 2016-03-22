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
            class   Finally {
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


            /**
             *  DRAFT
             */
            template <typename FUNCTION>
            class   FinallyT {
            public:
                FinallyT (FUNCTION f);
                FinallyT () = delete;
                FinallyT (FinallyT&& src) : fCleanupCodeBlock_ (move (src.fCleanupCodeBlock_)) {}
                FinallyT (const FinallyT&) = delete;

            public:
                ~FinallyT();

            public:
                nonvirtual  FinallyT& operator= (const FinallyT&) = delete;

            private:
                FUNCTION fCleanupCodeBlock_;
            };
            template <typename FUNCTION>
            auto mkFinally (FUNCTION f) -> FinallyT<FUNCTION>&&  { return move (FinallyT<FUNCTION> (f)); }


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
