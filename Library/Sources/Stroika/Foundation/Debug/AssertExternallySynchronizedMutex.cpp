/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/ToString.h"
#include "../Execution/Thread.h"

#include "Sanitizer.h"
#include "Trace.h"

#include "AssertExternallySynchronizedMutex.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Debug;

#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
/*
 ********************************************************************************
 ****************** Debug::AssertExternallySynchronizedMutex ********************
 ********************************************************************************
 */
void AssertExternallySynchronizedMutex::lock_ () noexcept
{
    try {
        SharedContext* sharedContext = fSharedContext_.get ();
        if (sharedContext->fFullLocks_++ == 0) {
            // If first time in, save thread-id
            sharedContext->fThreadWithFullLock_ = this_thread::get_id ();
            if (not sharedContext->GetSharedLockEmpty_ ()) {
                // If first already shared locks - OK - so long as same thread
                Require (sharedContext->CountOfIInSharedLockThreads_ (sharedContext->fThreadWithFullLock_) ==
                         sharedContext->GetSharedLockThreadsCount_ ());
            }
        }
        else {
            // If first already locked - OK - so long as same thread
            if (sharedContext->fThreadWithFullLock_ != this_thread::get_id ()) {
                // Duplicate the  Require() below, but with more debug information, because this is a COMMON and IMPORANT case;
                // If this happens, this means one thread has (the object containing this) is using this object (fake locked)
                // while we are trying to use it (again doing fake write lock) - so we want to PRINT INFO about that thread!!!
                DbgTrace (
                    L"ATTEMPT TO modify (lock for write) an object which is already in use (debuglocked) in another thread (thisthread=%s)",
                    Characters::ToString (this_thread::get_id ()).c_str ());
                DbgTrace ("Original thread holding lock: threadID=%s, and DbgTraceThreadName=%s",
                          Execution::Thread::FormatThreadID_A (sharedContext->fThreadWithFullLock_).c_str (),
                          Debug::GetDbgTraceThreadName_A (sharedContext->fThreadWithFullLock_).c_str ());
            }
            Require (sharedContext->fThreadWithFullLock_ == this_thread::get_id ());
        }
    }
    catch (...) {
        AssertNotReached ();
    }
}

void AssertExternallySynchronizedMutex::unlock_ () noexcept
{
    SharedContext* sharedContext = fSharedContext_.get ();
    Require (sharedContext->fThreadWithFullLock_ == this_thread::get_id ());
    Require (sharedContext->fFullLocks_ > 0); // else unbalanced
    --sharedContext->fFullLocks_;
    // Note at this point - it would make some sense to CLEAR fThreadWithFullLock_, but that could be a race, cuz someone
    // else could lock just as we are unlocking...
}

void AssertExternallySynchronizedMutex::lock_shared_ () const noexcept
{
    try {
        SharedContext* sharedContext = fSharedContext_.get ();
        // OK to shared lock from various threads
        // But if already locked, NOT OK (would have blocked in real impl) - if you try to shared lock from another thread while locked
        if (sharedContext->fFullLocks_ != 0) {
            // If first already locks - OK - so long as same thread
            if (sharedContext->fThreadWithFullLock_ != this_thread::get_id ()) {
                // Duplicate the  Require() below, but with more debug information, because this is a COMMON and IMPORANT case;
                // If this happens, this means one thread has (the object containing this) is using this object (fake locked)
                // while we are trying to use it (again doing fake write lock) - so we want to PRINT INFO about that thread!!!
                DbgTrace (
                    L"ATTEMPT TO shared_lock (lock for READ) an object which is already in use (debuglocked for WRITE) in another thread");
                DbgTrace ("Original thread holding (write) lock: threadID=%s, and DbgTraceThreadName=%s",
                          Execution::Thread::FormatThreadID_A (sharedContext->fThreadWithFullLock_).c_str (),
                          Debug::GetDbgTraceThreadName_A (sharedContext->fThreadWithFullLock_).c_str ());
            }
            Require (sharedContext->fThreadWithFullLock_ == this_thread::get_id ());
        }
        sharedContext->AddSharedLock_ (this_thread::get_id ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

void AssertExternallySynchronizedMutex::unlock_shared_ () const noexcept
{
    try {
        SharedContext* sharedContext = fSharedContext_.get ();
        sharedContext->RemoveSharedLock_ (this_thread::get_id ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

mutex& AssertExternallySynchronizedMutex::GetSharedLockMutexThreads_ ()
{
    static mutex sMutex_; // must be out-of-line so we can have just one mutex object. Could use static member, but then trouble using
                          // AssertExternallySynchronizedMutex before main
    return sMutex_;
}
#endif
