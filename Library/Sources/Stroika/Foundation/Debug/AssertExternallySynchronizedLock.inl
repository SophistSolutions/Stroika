/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_ 1

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
     *************** Debug::AssertExternallySynchronizedLock ************************
     ********************************************************************************
     */
#if qDebug
    inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (const shared_ptr<SharedContext>& sharedContext) noexcept
        // https://stroika.atlassian.net/browse/STK-500
        // NOTE - this will generate a throw and std::unexpected violation if there is no memory and multiset CTOR
        // throws. There is no good answer in this case. We declare the constructors noexcept so the footprint of
        // AssertExternallySynchronizedLock is as light as possible and the same (API/constraints) between debug and release
        // builds. And if we run out of memory here, there isn't much we can do to continue -- LGP 2018-10-02
        : _fSharedContext{sharedContext ? sharedContext : make_shared<SharedContext> ()}
    {
    }
    inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (const shared_ptr<SharedContext>& sharedContext, const AssertExternallySynchronizedLock& src) noexcept
        : AssertExternallySynchronizedLock{sharedContext}
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src) noexcept
        : AssertExternallySynchronizedLock{}
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (const shared_ptr<SharedContext>& sharedContext, [[maybe_unused]] AssertExternallySynchronizedLock&& src) noexcept
        : AssertExternallySynchronizedLock{sharedContext}
    {
        try {
            Require (src._fSharedContext->fLocks_ == 0 and src._fSharedContext->GetSharedLockEmpty ()); // to move, the src can have no locks of any kind (since we change src)
        }
        catch (...) {
            AssertNotReached ();
        }
    }
    inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock ([[maybe_unused]] AssertExternallySynchronizedLock&& src) noexcept
        : AssertExternallySynchronizedLock{}
    {
        try {
            Require (src._fSharedContext->fLocks_ == 0 and src._fSharedContext->GetSharedLockEmpty ()); // to move, the src can have no locks of any kind (since we change src)
        }
        catch (...) {
            AssertNotReached ();
        }
    }
#endif
    inline AssertExternallySynchronizedLock& AssertExternallySynchronizedLock::operator= ([[maybe_unused]] const AssertExternallySynchronizedLock& rhs) noexcept
    {
#if qDebug
        try {
            DISABLE_COMPILER_MSC_WARNING_START (26110);                        // to copy, the src can have shared_locks, but no (write) locks
            shared_lock<const AssertExternallySynchronizedLock> critSec1{rhs}; // ""
            if (this == &rhs) {
                Require (_fSharedContext->fLocks_ == 0 and _fSharedContext->GetSharedLockThreadsCount () == 1); // we locked ourselves above
            }
            else {
                Require (_fSharedContext->fLocks_ == 0 and _fSharedContext->GetSharedLockEmpty ()); // We must not have any locks going to replace this
            }
            DISABLE_COMPILER_MSC_WARNING_END (26110);
        }
        catch (...) {
            AssertNotReached ();
        }
#endif
        return *this;
    }
    inline AssertExternallySynchronizedLock& AssertExternallySynchronizedLock::operator= ([[maybe_unused]] AssertExternallySynchronizedLock&& rhs) noexcept
    {
#if qDebug
        try {
            Require (rhs._fSharedContext->fLocks_ == 0 and rhs._fSharedContext->GetSharedLockEmpty ()); // to move, the rhs can have no locks of any kind (since we change rhs)
            Require (_fSharedContext->fLocks_ == 0 and _fSharedContext->GetSharedLockEmpty ());         // ditto for thing being assigned to
        }
        catch (...) {
            AssertNotReached ();
        }
#endif
        return *this;
    }
#if qDebug
    inline void AssertExternallySynchronizedLock::SetAssertExternallySynchronizedLockContext (const shared_ptr<SharedContext>& sharedContext)
    {
        Require (sharedContext != nullptr);
        _fSharedContext = sharedContext;
    }
#endif
    inline void AssertExternallySynchronizedLock::lock () const noexcept
    {
#if qDebug
        lock_ ();
#endif
    }
    inline void AssertExternallySynchronizedLock::unlock () const noexcept
    {
#if qDebug
        unlock_ ();
#endif
    }
    inline void AssertExternallySynchronizedLock::lock_shared () const noexcept
    {
#if qDebug
        lock_shared_ ();
#endif
    }
    inline void AssertExternallySynchronizedLock::unlock_shared () const noexcept
    {
#if qDebug
        unlock_shared_ ();
#endif
    }

}

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_*/
