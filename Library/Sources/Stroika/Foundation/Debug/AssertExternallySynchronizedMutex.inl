/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_inl_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <algorithm>

#include "Sanitizer.h"

namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     **************** Debug::AssertExternallySynchronizedMutex **********************
     ********************************************************************************
     */
#if qDebug
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext) noexcept
        // https://stroika.atlassian.net/browse/STK-500
        // NOTE - this will generate a throw and std::unexpected violation if there is no memory and multiset CTOR
        // throws. There is no good answer in this case. We declare the constructors noexcept so the footprint of
        // AssertExternallySynchronizedMutex is as light as possible and the same (API/constraints) between debug and release
        // builds. And if we run out of memory here, there isn't much we can do to continue -- LGP 2018-10-02
        : _fSharedContext{sharedContext ? sharedContext : make_shared<SharedContext> ()}
    {
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext, const AssertExternallySynchronizedMutex& src) noexcept
        : AssertExternallySynchronizedMutex{sharedContext}
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec1{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const AssertExternallySynchronizedMutex& src) noexcept
        : AssertExternallySynchronizedMutex{}
    {
        shared_lock<const AssertExternallySynchronizedMutex> critSec1{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext, [[maybe_unused]] AssertExternallySynchronizedMutex&& src) noexcept
        : AssertExternallySynchronizedMutex{sharedContext}
    {
        try {
            Require (src._fSharedContext->fLocks_ == 0 and src._fSharedContext->GetSharedLockEmpty_ ()); // to move, the src can have no locks of any kind (since we change src)
        }
        catch (...) {
            AssertNotReached ();
        }
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex ([[maybe_unused]] AssertExternallySynchronizedMutex&& src) noexcept
        : AssertExternallySynchronizedMutex{}
    {
        try {
            Require (src._fSharedContext->fLocks_ == 0 and src._fSharedContext->GetSharedLockEmpty_ ()); // to move, the src can have no locks of any kind (since we change src)
        }
        catch (...) {
            AssertNotReached ();
        }
    }
#endif
    inline AssertExternallySynchronizedMutex& AssertExternallySynchronizedMutex::operator= ([[maybe_unused]] const AssertExternallySynchronizedMutex& rhs) noexcept
    {
#if qDebug
        try {
            DISABLE_COMPILER_MSC_WARNING_START (26110);                         // to copy, the src can have shared_locks, but no (write) locks
            shared_lock<const AssertExternallySynchronizedMutex> critSec1{rhs}; // ""
            if (this == &rhs) {
                Require (_fSharedContext->fLocks_ == 0 and _fSharedContext->GetSharedLockThreadsCount_ () == 1); // we locked ourselves above
            }
            else {
                // @todo This logic SB same as for MOVE, but for some reason this hasn't been triggered yet...
                // https://stroika.atlassian.net/browse/STK-752
                Require (_fSharedContext->fLocks_ == 0 and _fSharedContext->GetSharedLockEmpty_ ()); // We must not have any locks going to replace this
            }
            DISABLE_COMPILER_MSC_WARNING_END (26110);
        }
        catch (...) {
            AssertNotReached ();
        }
#endif
        return *this;
    }
    inline AssertExternallySynchronizedMutex& AssertExternallySynchronizedMutex::operator= ([[maybe_unused]] AssertExternallySynchronizedMutex&& rhs) noexcept
    {
#if qDebug
        try {
            Require (rhs._fSharedContext->fLocks_ == 0 and rhs._fSharedContext->GetSharedLockEmpty_ ()); // to move, the rhs can have no locks of any kind (since we change rhs)
            // https://stroika.atlassian.net/browse/STK-752
            // review - SB OK to assign over this with locks if the locks are all from this thread
            if (_fSharedContext->fLocks_ != 0) {
                Assert (false); // NYI
            }
            if (not _fSharedContext->GetSharedLockEmpty_ ()) {
                Assert (_fSharedContext->CountSharedLockThreads_ ().second == 0); // not good enuf - must transer lock to RHS
            }
        }
        catch (...) {
            AssertNotReached ();
        }
#endif
        return *this;
    }
#if qDebug
    inline void AssertExternallySynchronizedMutex::SetAssertExternallySynchronizedMutexContext (const shared_ptr<SharedContext>& sharedContext)
    {
        Require (sharedContext != nullptr);
        _fSharedContext = sharedContext;
    }
#endif
    inline void AssertExternallySynchronizedMutex::lock () const noexcept
    {
#if qDebug
        lock_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::unlock () const noexcept
    {
#if qDebug
        unlock_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::lock_shared () const noexcept
    {
#if qDebug
        lock_shared_ ();
#endif
    }
    inline void AssertExternallySynchronizedMutex::unlock_shared () const noexcept
    {
#if qDebug
        unlock_shared_ ();
#endif
    }

}

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_inl_*/
