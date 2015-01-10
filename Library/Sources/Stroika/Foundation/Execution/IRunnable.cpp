/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Configuration/StroikaConfig.h"
#include    "../Memory/BlockAllocated.h"

#include    "IRunnable.h"

DISABLE_COMPILER_MSC_WARNING_START(4996)


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;


namespace {
    struct   SimpleRunnable1_ : IRunnable {
        SimpleRunnable1_ (void (*fun2CallOnce) ())
            : IRunnable ()
            , fCall_ (fun2CallOnce)
        {
        }
        virtual void    Run () override
        {
            fCall_ ();
        }
        void (*fCall_) ();
        DECLARE_USE_BLOCK_ALLOCATION(SimpleRunnable1_);
    };
    struct   SimpleRunnable2_ : IRunnable {
        SimpleRunnable2_ (void (*fun2CallOnce) (void* arg), void* arg)
            : IRunnable ()
            , fCall_ (fun2CallOnce)
            , fArg_ (arg)
        {
        }
        virtual void    Run () override
        {
            fCall_ (fArg_);
        }
        void    (*fCall_) (void* arg);
        void*   fArg_;
        DECLARE_USE_BLOCK_ALLOCATION(SimpleRunnable2_);
    };
    struct   SimpleRunnable3_ : IRunnable {
        SimpleRunnable3_ (const function<void()>& fun2CallOnce)
            : IRunnable ()
            , fCall_ (fun2CallOnce)
        {
        }
        virtual void    Run () override
        {
            fCall_ ();
        }
        function<void()>   fCall_;
        DECLARE_USE_BLOCK_ALLOCATION(SimpleRunnable3_);
    };
}





/*
 ********************************************************************************
 *************************** Execution::mkIRunnablePtr **************************
 ********************************************************************************
 */
IRunnablePtr    Execution::mkIRunnablePtr (void (*fun2CallOnce) ())
{
    RequireNotNull (fun2CallOnce);
    return IRunnablePtr (new SimpleRunnable1_ (fun2CallOnce));
}

IRunnablePtr    Execution::mkIRunnablePtr (void (*fun2CallOnce) (void* arg), void* arg)
{
    RequireNotNull (fun2CallOnce);
    return IRunnablePtr (new SimpleRunnable2_ (fun2CallOnce, arg));
}

IRunnablePtr    Execution::mkIRunnablePtr (const function<void()>& fun2CallOnce)
{
    Require (static_cast<bool> (fun2CallOnce));
    return IRunnablePtr (new SimpleRunnable3_ (fun2CallOnce));
}


DISABLE_COMPILER_MSC_WARNING_END(4996)
