/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_h_ 1

#include "../StroikaPreComp.h"

#include <algorithm>
#include <atomic>
#include <forward_list>
#include <memory>
#include <mutex>
#include <optional>
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
 *      @todo   Reconsider if AssertExternallySynchronizedMutex::operator= should allow for this to be locked
 *              by the current thread. Safe to do later as that would be weakening the current check/requirement.
 */

namespace Stroika::Foundation::Debug {

    /**
     *  \brief      NOT a real lock - just a debugging infrastructure support tool so in debug builds can be assured threadsafe
     *
     *  This class is a 'no op' in production builds, as a 'recursive mutex' for a class that needs
     *  no thread locking because its externally synchronized.
     *
     *  AssertExternallySynchronizedMutex is recursive (a recursive-mutex) - or really super-recursive - because it allows
     *  lock/shared_lock to be mixed logically (unlike stdc++ shared_mutex).
     * 
     *  \note This means it is LEGAL to call lock () while holding a shared_lock, IFF that shared_lock is for the
     *        same thread. It is implicitly an 'UpgradeLock'
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
     *  \note   This doesn't guarnantee catching all races, but it catches many incorrect thread usage cases
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
     *  TODO:
     *      \todo since c++20, we can use [[no_unique_address]], instead of this base class hack
     *            https://stroika.atlassian.net/browse/STK-734
     * 
     *          Doing that will simplify usage, avoiding the lock_guard<const AssertExternallySynchronizedMutex>
     *          hack (cuz we'll say lock_guard {fCritSec} instead of lock_gurad<...type> {*this};
     *
     *  \par Example Usage
     *      \code
     *          struct foo : private Debug::AssertExternallySynchronizedMutex {
     *              inline  void    DoReadWriteStuffOnData ()
     *              {
     *                  lock_guard<AssertExternallySynchronizedMutex> writeLock { *this };       // lock_guard or scopedLock
     *                  scoped_lock<AssertExternallySynchronizedMutex> writeLock { *this };
     *                  // now do what you usually do for to modify locked data...
     *              }
     *              inline  void    DoReadOnlyStuffOnData ()
     *              {
     *                  shared_lock<const AssertExternallySynchronizedMutex> readLock { *this };
     *                  // now do what you usually do for DoReadOnlyStuffOnData - reading data only...
     *              }
     *          };
     *      \endcode
     *
     *  \note   The use of explicit <AssertExternallySynchronizedMutex> argument to lock_guard/shared_lock
     *          as opposed to
     *              auto&& critSec = shared_lock {*this};
     *          because then the deduced type would be foo (not AssertExternallySynchronizedMutex) - which then would have
     *          a lock/unlock method, but they would be PRIVATE. You COULD avoid this by using public base class of Debug::AssertExternallySynchronizedMutex
     *          but that would make public an implementation detail that should remain private.
     *          
     *  \note   This is SUPER-RECURSIVE lock. It allows lock() when shared_lock held (by only this thread) - so upgradelock.
     *          And it allows shared_lock when lock held by the same thread. Otherwise it asserts when a thread conflict is found,
     *          not blocking.
     */
    class AssertExternallySynchronizedMutex {
#if qDebug
    public:
        /**
         *  Explicit shared context object, so we can construct multiple AssertExternallySynchronizedMutex which all
         *  share a common 'sharedContext' - representing that they ALL must be externally syncrhonized across all the cooperating objects
         * 
         *  In most cases, just ignore this class.
         * 
         *  To have N cooperating classes (e.g. object, and a few direct members) all share the same rules of single-threading (treating them all
         *  as one object for the purpose of the rules of safe multithread access) - arrange for them to share a common 'sharedContext'
         * 
         *  \note class marked final to make more clear why safe to not have virtual destructor
         */
        struct SharedContext final {
        public:
            SharedContext () noexcept                       = default;
            SharedContext (const SharedContext&)            = delete;
            SharedContext& operator= (const SharedContext&) = delete;
            ~SharedContext ();

        private:
            atomic_uint_fast32_t fFullLocks_{0};
            thread::id           fThreadWithFullLock_; // or value undefined/last value where it had full lock

            // most logically a multiset, but std::multiset is not threadsafe and requires external locking.
            // So does forward_list, but its closer to lock free, so try it for now
            // GetSharedLockMutexThreads_ () used to access fSharedLockThreads_
            forward_list<thread::id> fSharedLockThreads_;

        private:
            bool                 GetSharedLockEmpty_ () const;
            pair<size_t, size_t> CountSharedLockThreads_ () const;
            size_t               GetSharedLockThreadsCount_ () const;
            size_t               CountOfIInSharedLockThreads_ (thread::id i) const;
            void                 AddSharedLock_ (thread::id i);
            void                 RemoveSharedLock_ (thread::id i);

        private:
            friend class AssertExternallySynchronizedMutex;
        };
#endif

    public:
        /**
         *  \note   Copy/Move constructor checks for existing locks while copying.
         *          Must be able to readlock source on copy, and have zero existing locks on src for move.
         *          These 'constructors' don't really do/copy/move anything, but just check the state of their own
         *          lock count and the state of the 'src' lock counts.
         * 
         *          NOTE - the 'SharedContext' does NOT get copied by copy constructors, move constructors etc. Its tied
         *          to the l-value.
         */
#if qDebug
        AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext = nullptr) noexcept;
        AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext, AssertExternallySynchronizedMutex&& src) noexcept;
        AssertExternallySynchronizedMutex (AssertExternallySynchronizedMutex&& src) noexcept;
        AssertExternallySynchronizedMutex (const AssertExternallySynchronizedMutex& src) noexcept;
        AssertExternallySynchronizedMutex (const shared_ptr<SharedContext>& sharedContext, const AssertExternallySynchronizedMutex& src) noexcept;
#else
        constexpr AssertExternallySynchronizedMutex () noexcept                                   = default;
        AssertExternallySynchronizedMutex (AssertExternallySynchronizedMutex&& src) noexcept      = default;
        AssertExternallySynchronizedMutex (const AssertExternallySynchronizedMutex& src) noexcept = default;
#endif

    public:
        /**
         *  \note   operator= checks for existing locks while copying.
         *          Must be able to readlock source on copy, and have zero existing locks on target or move.
         */
        nonvirtual AssertExternallySynchronizedMutex& operator= (AssertExternallySynchronizedMutex&& rhs) noexcept;
        nonvirtual AssertExternallySynchronizedMutex& operator= (const AssertExternallySynchronizedMutex& rhs) noexcept;

#if qDebug
    protected:
        /**
         *  Make it easy for subclasses to expose SetAssertExternallySynchronizedMutexContext () functionality, so those
         *  subclasses can allow users of those classes to share a sharing context.
         * 
         *  \note - this is named without the prefixing '_' (though protected) to make it easier to forward, just using using.
         */
        nonvirtual void SetAssertExternallySynchronizedMutexContext (const shared_ptr<SharedContext>& sharedContext);
#endif

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
        static mutex& GetSharedLockMutexThreads_ (); // MUTEX ONLY FOR fSharedLockThreads_ (could do one mutex per AssertExternallySynchronizedMutex but static probably performs better)
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "AssertExternallySynchronizedMutex.inl"

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedMutex_h_*/
