/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Execution/Thread.h"

#include "Sanitizer.h"
#include "Trace.h"

#include "AssertExternallySynchronizedLock.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Debug;

#if qDebug
/*
 ********************************************************************************
 ****************** Debug::AssertExternallySynchronizedLock *********************
 ********************************************************************************
 */
AssertExternallySynchronizedLock::AssertExternallySynchronizedLock () noexcept
    // https://stroika.atlassian.net/browse/STK-500
    // NOTE - this will generate a throw and std::unexpected violation if there is no memory and multiset CTOR
    // throws. There is no good answer in this case. We declare the constructors noexcept so the footprint of
    // AssertExternallySynchronizedLock is as light as possible and the same (API/constraints) between debug and release
    // builds. And if we run out of memory here, there isn't much we can do to continue -- LGP 2018-10-02
    : AssertExternallySynchronizedLock{make_shared<SharedContext> ()}
{
}

void AssertExternallySynchronizedLock::lock_ () const noexcept
{
    try {
        SharedContext* sharedContext = _fSharedContext.get ();
        if (sharedContext->fLocks_++ == 0) {
            // If first time in, save thread-id
            sharedContext->fCurLockThread_ = this_thread::get_id ();
            if (not sharedContext->GetSharedLockEmpty ()) {
                // If first already shared locks - OK - so long as same thread
                Require (sharedContext->CountOfIInSharedLockThreads (sharedContext->fCurLockThread_) == sharedContext->GetSharedLockThreadsCount ());
            }
        }
        else {
            // If first already locked - OK - so long as same thread
            if (sharedContext->fCurLockThread_ != this_thread::get_id ()) {
                // Duplicate the  Require() below, but with more debug information, because this is a COMMON and IMPORANT case;
                // If this happens, this means one thread has (the object containing this) is using this object (fake locked)
                // while we are trying to use it (again doing fake write lock) - so we want to PRINT INFO about that thread!!!
                DbgTrace (L"ATTEMPT TO modify (lock for write) an object which is already in use (debuglocked) in another thread");
                DbgTrace ("Original thread holding lock: threadID=%s, and DbgTraceThreadName=%s", Execution::Thread::FormatThreadID_A (sharedContext->fCurLockThread_).c_str (), Debug::GetDbgTraceThreadName_A (sharedContext->fCurLockThread_).c_str ());
            }
            Require (sharedContext->fCurLockThread_ == this_thread::get_id ());
        }
    }
    catch (...) {
        AssertNotReached ();
    }
}

void AssertExternallySynchronizedLock::unlock_ () const noexcept
{
    SharedContext* sharedContext = _fSharedContext.get ();
    Require (sharedContext->fCurLockThread_ == this_thread::get_id ());
    Require (sharedContext->fLocks_ > 0); // else unbalanced
    --sharedContext->fLocks_;
}

void AssertExternallySynchronizedLock::lock_shared_ () const noexcept
{
    try {
        SharedContext* sharedContext = _fSharedContext.get ();
        // OK to shared lock from various threads
        // But if already locked, NOT OK (would have blocked in real impl) - if you try to shared lock from another thread while locked
        if (sharedContext->fLocks_ != 0) {
            // If first already locks - OK - so long as same thread
            if (sharedContext->fCurLockThread_ != this_thread::get_id ()) {
                // Duplicate the  Require() below, but with more debug information, because this is a COMMON and IMPORANT case;
                // If this happens, this means one thread has (the object containing this) is using this object (fake locked)
                // while we are trying to use it (again doing fake write lock) - so we want to PRINT INFO about that thread!!!
                DbgTrace (L"ATTEMPT TO sharked_lock (lock for READ) an object which is already in use (debuglocked for WRITE) in another thread");
                DbgTrace ("Original thread holding (write) lock: threadID=%s, and DbgTraceThreadName=%s", Execution::Thread::FormatThreadID_A (sharedContext->fCurLockThread_).c_str (), Debug::GetDbgTraceThreadName_A (sharedContext->fCurLockThread_).c_str ());
            }
            Require (sharedContext->fCurLockThread_ == this_thread::get_id ());
        }
        sharedContext->AddSharedLock (this_thread::get_id ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

void AssertExternallySynchronizedLock::unlock_shared_ () const noexcept
{
    try {
        SharedContext*    sharedContext = _fSharedContext.get ();
        sharedContext->RemoveSharedLock (this_thread::get_id ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

mutex& AssertExternallySynchronizedLock::GetSharedLockMutexThreads_ ()
{
    static mutex sMutex_; // must be out-of-line so we can have just one mutex object. Could use static member, but then trouble using
                          // AssertExternallySynchronizedLock before main
    return sMutex_;
}
#endif
