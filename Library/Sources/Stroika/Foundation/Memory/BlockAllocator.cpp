/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Execution/Thread.h"

#include    "BlockAllocator.h"






using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Memory;
using   namespace   Stroika::Foundation::Memory::Private_;
using   namespace   Stroika::Foundation::Execution;

using   namespace   Execution;



Memory::Private_::LockType_*    Memory::Private_::sLock_  =   nullptr;



/*
 ********************************************************************************
 *********************** BlockAllocator_ModuleInit_ *****************************
 ********************************************************************************
 */
BlockAllocator_ModuleInit_::BlockAllocator_ModuleInit_ ()
{
    Require (sLock_ == nullptr);
    sLock_ = new Private_::LockType_ ();
}

BlockAllocator_ModuleInit_::~BlockAllocator_ModuleInit_ ()
{
    RequireNotNull (sLock_);
    delete sLock_;
    sLock_ = nullptr;
}




/*
 ********************************************************************************
 ************** Memory::MakeModuleDependency_BlockAllocator *********************
 ********************************************************************************
 */
Execution::ModuleDependency Memory::MakeModuleDependency_BlockAllocator ()
{
    return Execution::ModuleInitializer<Private_::BlockAllocator_ModuleInit_>::GetDependency ();
}



/*
 ********************************************************************************
 *********** Memory::Private_::DoDeleteHandlingLocksExceptionsEtc_ **************
 ********************************************************************************
 */
void    Memory::Private_::DoDeleteHandlingLocksExceptionsEtc_ (void* p, void** staticNextLinkP) noexcept
{
    /*
     *  Logically this just does a lock acquire and assginemnt through pointers (swap). But
     *  it checks for thread abort exceptions, and supresses that if needed, since this is noexcept
     *  and can be used in DTOR. You can interupt (abort) a thread while it deletes things.
     */
    try {
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (Private_::GetLock_ ());
#else
        auto    critSec  { make_unique_lock (Private_::GetLock_ ()) };
#endif
        // push p onto the head of linked free list
        (*(void**)p) = *staticNextLinkP;
        * staticNextLinkP = p;
    }
    catch (const Execution::Thread::InterruptException&) {
        Execution::Thread::SuppressInterruptionInContext  suppressContext;
#if     qCompilerAndStdLib_make_unique_lock_IsSlow
        MACRO_LOCK_GUARD_CONTEXT (Private_::GetLock_ ());
#else
        auto    critSec  { make_unique_lock (Private_::GetLock_ ()) };
#endif
        // push p onto the head of linked free list
        (*(void**)p) = *staticNextLinkP;
        *staticNextLinkP = p;
    }
}
