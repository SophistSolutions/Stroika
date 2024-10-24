/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/Thread.h"

#include "BlockAllocator.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Memory::Private_;
using namespace Stroika::Foundation::Execution;

using namespace Execution;

/*
 ********************************************************************************
 *********** Memory::Private_::DoDeleteHandlingLocksExceptionsEtc_ **************
 ********************************************************************************
 */
#if !qStroika_Foundation_Memory_BlockAllocator_UseLockFree_
void Memory::Private_::DoDeleteHandlingLocksExceptionsEtc_ (void* p, void** staticNextLinkP) noexcept
{
    /*
     *  Logically this just does a lock acquire and assginemnt through pointers (swap). But
     *  it checks for thread abort exceptions, and supresses that if needed, since this is noexcept
     *  and can be used in DTOR. You can interrupt (abort) a thread while it deletes things.
     */
    try {
        auto critSec = lock_guard{Private_::GetLock_ ()};
        // push p onto the head of linked free list
        (*(void**)p)     = *staticNextLinkP;
        *staticNextLinkP = p;
    }
    catch (const Execution::Thread::AbortException&) {
        Execution::Thread::SuppressInterruptionInContext suppressContext;
        auto                                             critSec = lock_guard{Private_::GetLock_ ()};
        // push p onto the head of linked free list
        (*(void**)p)     = *staticNextLinkP;
        *staticNextLinkP = p;
    }
}
#endif