/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_ 1

#include "../StroikaPreComp.h"

#include <atomic>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <thread>

#include "../Configuration/Common.h"
#include "../Debug/Assertions.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 *  TODO:
 *      @todo   Redo multiset impl so can be used LOCK-FREE - at least 99% of the time.... Locks affect timing, and can hide thread
 *              bugs.
 *
 *      @todo   Reconsider if AssertExternallySynchronizedLock::operator= shoulkd allow for this to be locked
 *              by the current thread. Safe to do later as that would be weakening the current check/requirement.
 */

namespace Stroika::Foundation::Debug {

    /**
     *  \brief      NOT a real lock - just a debugging infrastructure support tool so in debug builds we assure used threadsafe
     *
     *  This class is a 'no op' in production builds, as a 'locker' for a class that needs
     *  no thread locking because its externally synchronized.
     *
     *  This 'lock tester' is recursive (a recursive-mutex).
     *
     *  Externally synchronized means that some external applicaiton control guarantees the seciton of code (or data)
     *  is only accessed by a single thread.
     *
     *  This can be used to guarantee the same level of thread safety as provided in the std c++ libraries:
     *      Allow multiple readers (shared locks) from multiple threads, but if any thread has
     *      a lock (writer), then no other threads my read or write lock (in any order).
     *
     *  In debug builds, it enforces this fact through assertions.
     *
     *  \note   This doesn't gaurnatee catching all races, but it catches many incorrect thread usage cases
     *
     *  \note   This may 'catch' a race by having its internal data structures (multiset) corrupted. Interpret
     *          corrupt multiset as a likely race indicator
     *
     *  \note   ***Not Cancelation Point***
     *
     *  \note   methods all noexcept (just asserts out on problems) - noexcept so debug semantics same as release semantics
     *          Since the DEBUG version will allocate memory, which may fail, those failures trigger assertion failure and abort.
     *
     *  Use this as a BASECLASS instead of directly aggregating, due to C++'s queer
     *  rules about sizeof() and members (all at least sizeof byte), but that does not apply
     *  to base classes:
     *      from http://open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3797.pdf: section 1.8:
     *          ...
     *          "Unless it is a bit-field (9.6), a most derived object shall have a non-zero
     *          size and shall occupy one or more bytes of storage. Base class subobjects may
     *          have zero size"
     *
     *  \par Example Usage
     *      \code
     *      struct foo : private Debug::AssertExternallySynchronizedLock {
     *          inline  void    DoReadWriteStuffOnData ()
     *          {
     *              lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
     *              // now do what you usually do for to modify locked data...
     *          }
     *          inline  void    DoReadOnlyStuffOnData ()
     *          {
     *              shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
     *              // now do what you usually do for DoReadOnlyStuffOnData - reading data only...
     *          }
     *      };
     *      \endcode
     *
     *  \note   Until Stroika v2.0a119, this was a non-recursive mutex, but it is now recursive.
     */
    class AssertExternallySynchronizedLock {
    public:
        /**
         *  \note   Copy/Move constructor checks for existing locks while copying.
         *          Must be able to readlock source on copy, and have zero existing locks on src for move.
         */
#if !qDebug
        constexpr
#endif
            AssertExternallySynchronizedLock () noexcept;
        AssertExternallySynchronizedLock (AssertExternallySynchronizedLock&& src) noexcept;
        AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src) noexcept;

    public:
        /**
         *  \note   operator= checks for existing locks while copying.
         *          Must be able to readlock source on copy, and have zero existing locks on target or move.
         */
        nonvirtual AssertExternallySynchronizedLock& operator= (AssertExternallySynchronizedLock&& rhs) noexcept;
        nonvirtual AssertExternallySynchronizedLock& operator= (const AssertExternallySynchronizedLock& rhs) noexcept;

    public:
        /**
         *  Saves current thread, and increments lock count, and
         *      \req    no pre-existing lock or shared locks on other threads
         *
         *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
         *          methods without casts.
         */
        nonvirtual void lock () const noexcept;

    public:
        /**
         *  Just decrement lock count
         *
         *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
         *          methods without casts.
         *
         *  \req    still running on the same locking thread and locks not unbalanced
         */
        nonvirtual void unlock () const noexcept;

    public:
        /**
         *  Saves current thread (multiset), and increments shared count, and
         *      \req    no pre-existing locks on other threads
         *
         *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
         *          methods without casts.
         */
        nonvirtual void lock_shared () const noexcept;

    public:
        /**
         *  Just decrement shared lock count (remove this thread from shared lock multiset)
         *
         *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
         *          methods without casts.
         *
         *  \req    still running on the same locking thread and locks not unbalanced
         */
        nonvirtual void unlock_shared () const noexcept;

#if qDebug
    private:
        nonvirtual void lock_ () const noexcept;
        nonvirtual void unlock_ () const noexcept;
        nonvirtual void lock_shared_ () const noexcept;
        nonvirtual void unlock_shared_ () const noexcept;

    private:
        mutable atomic_uint_fast32_t fLocks_{0};
        mutable thread::id           fCurLockThread_;
        static mutex&                GetSharedLockMutexThreads_ (); // MUTEX ONLY FOR fSharedLockThreads_ (could do one mutex per AssertExternallySynchronizedLock but static probably performs better)
        mutable multiset<thread::id> fSharedLockThreads_;           // multiset not threadsafe, and this class intrinsically tracks thread Ids across threads, so use GetSharedLockMutexThreads_ () to make safe
#endif
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "AssertExternallySynchronizedLock.inl"

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_*/
