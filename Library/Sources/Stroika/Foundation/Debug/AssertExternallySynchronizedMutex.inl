/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
        shared_lock<const AssertExternallySynchronizedMutex> readLockSrc{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const AssertExternallySynchronizedMutex& src) noexcept
        : AssertExternallySynchronizedMutex{}
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLockSrc{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext, [[maybe_unused]] AssertExternallySynchronizedMutex&& src) noexcept
        : AssertExternallySynchronizedMutex{sharedContext}
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLockSrc{src}; // move we must be able to modify source
    }
    inline AssertExternallySynchronizedMutex::AssertExternallySynchronizedMutex ([[maybe_unused]] AssertExternallySynchronizedMutex&& src) noexcept
        : AssertExternallySynchronizedMutex{}
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLockRHS{src}; // move we must be able to modify source
    }
#endif
    inline AssertExternallySynchronizedMutex& AssertExternallySynchronizedMutex::operator= ([[maybe_unused]] const AssertExternallySynchronizedMutex& rhs) noexcept
    {
        shared_lock<const AssertExternallySynchronizedMutex> readLockRHS{rhs};     // we must be able to read RHS
        lock_guard<const AssertExternallySynchronizedMutex>  writeLockThis{*this}; // we must be able modify this
        return *this;
    }
    inline AssertExternallySynchronizedMutex& AssertExternallySynchronizedMutex::operator= ([[maybe_unused]] AssertExternallySynchronizedMutex&& rhs) noexcept
    {
        lock_guard<const AssertExternallySynchronizedMutex> writeLockRHS{rhs};    // move we must be able to modify rhs to move it
        lock_guard<const AssertExternallySynchronizedMutex> writeLockThis{*this}; // we must be able modify this
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
