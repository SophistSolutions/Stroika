/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "Sanitizer.h"

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
void AssertExternallySynchronizedLock::lock_ () const noexcept
{
    try {
        if (fLocks_++ == 0) {
            // If first time in, save thread-id
            fCurLockThread_ = this_thread::get_id ();
            lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
            if (not fSharedLockThreads_.empty ()) {
                // If first already shared locks - OK - so long as same thread
                Require (fSharedLockThreads_.count (fCurLockThread_) == fSharedLockThreads_.size ());
            }
        }
        else {
            // If first already locked - OK - so long as same thread
            Require (fCurLockThread_ == this_thread::get_id ());
        }
    }
    catch (...) {
        AssertNotReached ();
    }
}

void AssertExternallySynchronizedLock::unlock_ () const noexcept
{
    Require (fCurLockThread_ == this_thread::get_id ());
    Require (fLocks_ > 0); // else unbalanced
    --fLocks_;
}

// workaround https://stroika.atlassian.net/browse/STK-665
Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE ("address") void AssertExternallySynchronizedLock::lock_shared_ () const noexcept
{
    try {
        // OK to shared lock from various threads
        // But if already locked, NOT OK (would have blocked in real impl) - if you try to shared lock from another thread while locked
        if (fLocks_ != 0) {
            // If first already locks - OK - so long as same thread
            Require (fCurLockThread_ == this_thread::get_id ());
        }
        lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
        fSharedLockThreads_.insert (this_thread::get_id ());
    }
    catch (...) {
        AssertNotReached ();
    }
}

void AssertExternallySynchronizedLock::unlock_shared_ () const noexcept
{
    try {
        lock_guard<mutex>              sharedLockProtect{GetSharedLockMutexThreads_ ()};
        multiset<thread::id>::iterator tti = fSharedLockThreads_.find (this_thread::get_id ());
        Require (tti != fSharedLockThreads_.end ()); // else unbalanced
        fSharedLockThreads_.erase (tti);
    }
    catch (...) {
        AssertNotReached ();
    }
}
#endif
