/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_ 1

#include "../StroikaPreComp.h"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <optional>
#include <forward_list>
#include <shared_mutex>
#include <thread>

#include "../Configuration/Common.h"

#include "Assertions.h"
#include "Sanitizer.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   see if fSharedLockThreads_ can be replaced wtih LOCK-FREE - at least 99% of the time.... Locks affect timing, and can hide thread
 *              bugs. Quickie attempt at profiling yeilds that that time is NOT spent with the locks but with the remove()
 *              code (since I switched from multiset to forward_list, so maybe cuz of that). Or could be bad measurement (I just
 *              test on DEBUG builds).
 * 
 *              Since Stroika 2.1b10 we do have a lock/free forward_list class I could try. But I'm not yet confident
 *              in its stability, so maybe sometime down the road... 
 *
 *              @see https://stroika.atlassian.net/browse/STK-540 for details on stuff todo above
 *
 *      @todo   Reconsider if AssertExternallySynchronizedLock::operator= should allow for this to be locked
 *              by the current thread. Safe to do later as that would be weakening the current check/requirement.
 */

namespace Stroika::Foundation::Debug {

    /**
     *  \brief      NOT a real lock - just a debugging infrastructure support tool so in debug builds can be assured threadsafe
     *
     *  This class is a 'no op' in production builds, as a 'locker' for a class that needs
     *  no thread locking because its externally synchronized.
     *
     *  This 'lock tester' is recursive (a recursive-mutex) - or really super-recursive - because it allows
     *  lock/shared_lock to be mixed logically (unlike stdc++ shared_mutex).
     *
     *  Externally synchronized means that some external application control guarantees the section of code (or data)
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
     *          struct foo : private Debug::AssertExternallySynchronizedLock {
     *              inline  void    DoReadWriteStuffOnData ()
     *              {
     *                  lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
     *                  // now do what you usually do for to modify locked data...
     *              }
     *              inline  void    DoReadOnlyStuffOnData ()
     *              {
     *                  shared_lock<const AssertExternallySynchronizedLock> critSec { *this };
     *                  // now do what you usually do for DoReadOnlyStuffOnData - reading data only...
     *              }
     *          };
     *      \endcode
     *
     *  \note   The use of explicit <AssertExternallySynchronizedLock> argument to lock_guard/shared_lock
     *          as opposed to
     *              auto&& critSec = shared_lock {*this};
     *          because then the deduced type would be foo (not AssertExternallySynchronizedLock) - which then would have
     *          a lock/unlock method, but they would be PRIVATE. You COULD avoid this by using public base class of Debug::AssertExternallySynchronizedLock
     *          but that would make public an implementation detail that should remain private.
     *          
     *  \note   This is SUPER-RECURSIVE lock. It allows lock() when shared_lock held (by only this thread) - so upgradelock.
     *          And it allows shared_lock when lock held by the same thread. Otherwise it asserts when a thread conflict is found,
     *          not blocking.
     */
    class AssertExternallySynchronizedLock {
#if qDebug
    public:
        /**
         *  Explicit shared context object, so we can construct multiple AssertExternallySynchronizedLock which all
         *  share a common 'sharedContext' - representing that they ALL must be externally syncrhonized across all the cooperating objects
         * 
         *  In most cases, just ignore this class.
         * 
         *  To have N cooperating classes (e.g. object, and a few direct members) all share the same rules of single-threading (treating them all
         *  as one object for the purpose of the rules of safe multithread access) - arrange for them to share a common 'sharedContext'
         */
        struct SharedContext {
        public:
            SharedContext () = default;
            SharedContext (const SharedContext&) = delete;
            SharedContext& operator= (const SharedContext&) = delete;
            ~SharedContext ()
            {
                Assert (fLocks_ == 0);
                Assert (fSharedLockThreads_.empty ());
            }

        private:
            atomic_uint_fast32_t fLocks_{0}; // refers to FULL locks, not shared locks (use a multiset there to track redundant shared locks)
            thread::id           fCurLockThread_;

            // most logically a multiset, but std::multiset is not threadsafe and requires external locking.
            // So does forward_list, but its closer to lock free, so try it for now
            // GetSharedLockMutexThreads_ () used to access fSharedLockThreads_
            forward_list<thread::id> fSharedLockThreads_;

            bool GetSharedLockEmpty () const
            {
                lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
                return fSharedLockThreads_.empty ();
            }
            size_t GetSharedLockThreadsCount () const
            {
                lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
                size_t            i = 0;
                for ([[maybe_unused]]const auto& x : fSharedLockThreads_) {
                    i++;
                }
                return i;
            }
            size_t CountOfIInSharedLockThreads (thread::id i) const
            {
                lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
                return std::count (fSharedLockThreads_.begin (), fSharedLockThreads_.end (), i);
            }
            void AddSharedLock (thread::id i)
            {
                lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
                fSharedLockThreads_.push_front (i);
            }
            void RemoveSharedLock (thread::id i)
            {
                lock_guard<mutex> sharedLockProtect{GetSharedLockMutexThreads_ ()};
                fSharedLockThreads_.remove (i);
            }

        private:
            friend class AssertExternallySynchronizedLock;
        };
#endif

    public:
        /**
         *  \note   Copy/Move constructor checks for existing locks while copying.
         *          Must be able to readlock source on copy, and have zero existing locks on src for move.
         */
#if qDebug
        AssertExternallySynchronizedLock () noexcept;
        AssertExternallySynchronizedLock (const shared_ptr<SharedContext>& sharedContext) noexcept;
        AssertExternallySynchronizedLock (const shared_ptr<SharedContext>& sharedContext, AssertExternallySynchronizedLock&& src) noexcept;
        AssertExternallySynchronizedLock (AssertExternallySynchronizedLock&& src) noexcept;
        AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src) noexcept;
        AssertExternallySynchronizedLock (const shared_ptr<SharedContext>& sharedContext, const AssertExternallySynchronizedLock& src) noexcept;
#else
        constexpr AssertExternallySynchronizedLock () noexcept                                  = default;
        AssertExternallySynchronizedLock (AssertExternallySynchronizedLock&& src) noexcept      = default;
        AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src) noexcept = default;
#endif

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
         *      \req    already locked by this thread or no existing locks (either shared or exclusive)
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

    protected:
        shared_ptr<SharedContext> _fSharedContext;

    private:
        static mutex& GetSharedLockMutexThreads_ (); // MUTEX ONLY FOR fSharedLockThreads_ (could do one mutex per AssertExternallySynchronizedLock but static probably performs better)
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
