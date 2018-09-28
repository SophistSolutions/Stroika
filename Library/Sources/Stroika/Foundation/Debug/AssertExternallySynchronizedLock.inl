/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Sanitizer.h"

namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     *************** Debug::AssertExternallySynchronizedLock ************************
     ********************************************************************************
     */
#if qDebug
    // workaround https://stroika.atlassian.net/browse/STK-665, https://stroika.atlassian.net/browse/STK-500 - ARM ONLY
    Stroika_Foundation_Debug_ATTRIBUTE_NO_SANITIZE ("address") 
    DISABLE_COMPILER_MSC_WARNING_START (4297)
    inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock () noexcept try : fSharedLockThreads_ () {
    }
    catch (...) {
        AssertNotReached ();
    }
    DISABLE_COMPILER_MSC_WARNING_END (4297)
#else
    inline constexpr AssertExternallySynchronizedLock::AssertExternallySynchronizedLock () noexcept
    {
    }
#endif
    inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src) noexcept
        : AssertExternallySynchronizedLock ()
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec1{src}; // to copy, the src can have shared_locks, but no (write) locks
    }
    inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock ([[maybe_unused]] AssertExternallySynchronizedLock&& src) noexcept
        : AssertExternallySynchronizedLock ()
    {
#if qDebug
        try {
            lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
            Require (src.fLocks_ == 0 and src.fSharedLockThreads_.empty ()); // to move, the src can have no locks of any kind (since we change src)
        }
        catch (...) {
            AssertNotReached ();
        }
#endif
    }
    inline AssertExternallySynchronizedLock& AssertExternallySynchronizedLock::operator= ([[maybe_unused]] const AssertExternallySynchronizedLock& rhs) noexcept
    {
#if qDebug
        try {
            lock_guard<const AssertExternallySynchronizedLock> critSec1{rhs}; // to copy, the src can have shared_locks, but no (write) locks
            lock_guard<mutex>                                  sharedLockProtect{GetSharedLockMutexThreads_ ()};
            Require (fLocks_ == 0 and fSharedLockThreads_.empty ()); // We must not have any locks going to replace this
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
            lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
            Require (rhs.fLocks_ == 0 and rhs.fSharedLockThreads_.empty ()); // to move, the rhs can have no locks of any kind (since we change rhs)
            Require (fLocks_ == 0 and fSharedLockThreads_.empty ());         // ditto for thing being assigned to
        }
        catch (...) {
            AssertNotReached ();
        }
#endif
        return *this;
    }
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
#if qDebug
    inline mutex& AssertExternallySynchronizedLock::GetSharedLockMutexThreads_ ()
    {
        static mutex sMutex_;
        return sMutex_;
    }
#endif

}

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_*/
