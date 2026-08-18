/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedChecker_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedChecker_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <forward_list>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <thread>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Sanitizer.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Release">Release</a>
 *
 *  TODO:
 *      @todo   see if fSharedLocks_ can be replaced with LOCK-FREE - at least 99% of the time.... Locks affect timing, and can hide thread
 *              bugs. Quickie attempt at profiling yields that that time is NOT spent with the locks but with the remove()
 *              code (since I switched from multiset to forward_list, so maybe cuz of that). Or could be bad measurement (I just
 *              test on DEBUG builds).
 * 
 *              Since Stroika 2.1b10 we do have a lock/free forward_list class I could try. But I'm not yet confident
 *              in its stability, so maybe sometime down the road... 
 *
 *              @see https://github.com/SophistSolutions/Stroika/issues/676 (STK-540) for details on stuff todo above
 *
 *      @todo   Reconsider if AssertExternallySynchronizedChecker::operator= should allow for this to be locked
 *              by the current thread. Safe to do later as that would be weakening the current check/requirement.
 */

namespace Stroika::Foundation::Debug {

    /**
     *  \brief qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled controls if this threaded access protection
     * 
     *      The compilation compile-time macro qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled can be used
     *      to control if AssertExternallySynchronizedChecker checking is enabled.
     * 
     *      If its not defined (typical), we look at qStroika_Foundation_Debug_AssertionsChecked. If that is false, qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled is disabled.
     * 
     *      If qStroika_Foundation_Debug_AssertionsChecked is true, BUT, we have TSAN enabled, we STILL (change in Stroika v3.0d1) - DISABLE kAssertExternallySynchronizedCheckerEnabled
     *      since its slow, and redundant.
     * 
     *      Only if qStroika_Foundation_Debug_AssertionsChecked is true, there is no TSAN, and qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled is made
     *      do we turn on qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled.
     * 
     *  \note TRIED to do this with constexpr bool kAssertExternallySynchronizedCheckerEnabled, but as of C++20 rules
     *        still too much of a PITA to use: cannot conditionally define classes, and nearly anything
     *        based on requires/if constexpr, unless it is a template.
     */
// DEPRECATED spelling (before Stroika v3.0d24 this class was called AssertExternallySynchronizedMutex).
// Honored so that an existing build defining the old macro keeps the setting it asked for, rather than
// silently falling through to the default below. Remove when the deprecated header goes.
#if defined(qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled) and                                                       \
    not defined(qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled)
#define qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled                                                              \
    qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
#endif

#if not defined(qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled)
#if qStroika_Foundation_Debug_AssertionsChecked and not Stroika_Foundation_Debug_Sanitizer_HAS_ThreadSanitizer
#define qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled 1
#else
#define qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled 0
#endif
#endif

    /**
     *  \brief NOT a real mutex - just a debugging infrastructure support tool so in debug builds can be assured threadsafe, which is syntactically used like a mutex, for SIMILAR reasons in similar places
     *
     *  This class is a 'no op' in production builds (so zero cost in release builds, assumes caller ensures thread safety).
     *
     *  AssertExternallySynchronizedChecker follows the pattern of (a recursive-mutex) - or really super-recursive - because it allows
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
     *  \note   This doesn't guarantee catching all races (with no happens-before), but it catches many incorrect thread usage cases
     * 
     *  \note   ***Not Cancelation Point***
     *
     *  \note   methods all noexcept (just asserts out on problems) - noexcept so debug semantics same as release semantics
     *          Since the DEBUG version will allocate memory, which may fail, those failures trigger assertion failure and abort.
     *
     *  \note   typically used as
     *              qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
     *
     *  \note Satisfies Concepts:
     *      o   movable<AssertExternallySynchronizedChecker>
     *      o   copyable<AssertExternallySynchronizedChecker>
     *      o   Common::StdCompat::Lockable<AssertExternallySynchronizedChecker>
     *
     *  \note   movable/copyable is REQUIRED, and is where this most visibly parts company with a real mutex
     *          (std::mutex is neither). Objects embed one of these to be checked, and Stroika objects are
     *          copy-by-value, so the checker has to copy and move along with its owner. See the static_assert
     *          at the bottom of this file for the full rationale.
     *
     *  \par Example Usage
     *      \code
     *          struct foo   {
     *              qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
     *              inline  void    DoReadWriteStuffOnData ()
     *              {
     *                  AssertExternallySynchronizedChecker::WriteContext declareContext { fThisAssertExternallySynchronized_ };
     *                  // now do what you usually do for to modify locked data...
     *              }
     *              inline  void    DoReadOnlyStuffOnData ()
     *              {
     *                  AssertExternallySynchronizedChecker::ReadContext declareContext { fThisAssertExternallySynchronized_ };
     *                  // now do what you usually do for DoReadOnlyStuffOnData - reading data only...
     *              }
     *          };
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // this style of use - subclassing - is especially useful if the object foo will be subclassed, and checked throughout the
     *          // code (or subclasses) with Debug::AssertExternallySynchronizedChecker::ReadContext (or WriteContext)
     *          struct foo : public Debug::AssertExternallySynchronizedChecker {
     *              inline  void    DoReadWriteStuffOnData ()
     *              {
     *                  AssertExternallySynchronizedChecker::WriteContext declareContext { *this };       // lock_guard or scopedLock or unique_lock
     *                  // now do what you usually do for to modify locked data...
     *              }
     *              inline  void    DoReadOnlyStuffOnData ()
     *              {
     *                  AssertExternallySynchronizedChecker::ReadContext declareContext { *this };
     *                  // now do what you usually do for DoReadOnlyStuffOnData - reading data only...
     *              }
     *          };
     *      \endcode
     *
     *  \note   This is SUPER-RECURSIVE lock. It allows lock() when shared_lock held (by only this thread) - so upgrades the lock.
     *          And it allows shared_lock when lock held by the same thread. Otherwise it asserts when a thread conflict is found.
     *          lock() and shared_lock () - here - are NEVER blocking. They just assert there is no conflict.
     */
    class AssertExternallySynchronizedChecker {
#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
    public:
        /**
         *  Explicit shared context object, so we can construct multiple AssertExternallySynchronizedChecker which all
         *  share a common 'sharedContext' - representing that they ALL must be externally synchronized across all the cooperating objects
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

        private:
            // Use of inline array avoids mallocs, and makes this run slightly faster. No semantic differerence,
            // just makes debug mode a bit faster.
            static constexpr size_t kInlineSharedLockBufSize_ = 2;
            struct {
                // most logically a multiset, but std::multiset is not threadsafe and requires external locking.
                // So does forward_list, but its closer to lock free, so try it for now
                // GetSharedLockMutexThreads_ () used to access fSharedLocks_
                array<thread::id, kInlineSharedLockBufSize_> fInitialThreads_;
                uint8_t                                      fInitialThreadsSize_{0}; // not sure how to add this field only conditionally
                forward_list<thread::id>                     fOverflowThreads_;
            } fSharedLocks_;

        private:
            bool                 GetSharedLockEmpty_ () const;
            pair<size_t, size_t> CountSharedLockThreads_ () const;
            size_t               GetSharedLockThreadsCount_ () const;
            size_t               CountOfIInSharedLockThreads_ (thread::id i) const;
            void                 AddSharedLock_ (thread::id i);
            void                 RemoveSharedLock_ (thread::id i);

        private:
            friend class AssertExternallySynchronizedChecker;
        };
#endif

    public:
        /**
         *  \note   Copy/Move constructor checks for existing locks while copying.
         *          Must be able to read lock source on copy, and have zero existing locks on src for move.
         *          These 'constructors' don't really do/copy/move anything, but just check the state of their own
         *          lock count and the state of the 'src' lock counts.
         * 
         *          NOTE - the 'SharedContext' does NOT get copied by copy constructors, move constructors etc. Its tied
         *          to the l-value.
         */
#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
        AssertExternallySynchronizedChecker (const shared_ptr<SharedContext>& sharedContext = nullptr) noexcept;
        AssertExternallySynchronizedChecker (const shared_ptr<SharedContext>& sharedContext, AssertExternallySynchronizedChecker&& src) noexcept;
        AssertExternallySynchronizedChecker (AssertExternallySynchronizedChecker&&) noexcept;
        AssertExternallySynchronizedChecker (const AssertExternallySynchronizedChecker&) noexcept;
        AssertExternallySynchronizedChecker (const shared_ptr<SharedContext>& sharedContext, const AssertExternallySynchronizedChecker& src) noexcept;
#else
        constexpr AssertExternallySynchronizedChecker () noexcept                                 = default;
        AssertExternallySynchronizedChecker (AssertExternallySynchronizedChecker&&) noexcept      = default;
        AssertExternallySynchronizedChecker (const AssertExternallySynchronizedChecker&) noexcept = default;
#endif

    public:
        /**
         *  \note   operator= checks for existing locks while copying.
         *          Must be able to read lock source on copy, and have zero existing locks on target or move.
         */
        nonvirtual AssertExternallySynchronizedChecker& operator= (AssertExternallySynchronizedChecker&& rhs) noexcept;
        nonvirtual AssertExternallySynchronizedChecker& operator= (const AssertExternallySynchronizedChecker& rhs) noexcept;

#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
    public:
        nonvirtual shared_ptr<SharedContext> GetSharedContext () const;

    public:
        /**
         *  Make it easy for subclasses to expose SetAssertExternallySynchronizedCheckerContext () functionality, so those
         *  subclasses can allow users of those classes to share a sharing context.
         * 
         *  \note - this is named without the prefixing '_' (though protected) to make it easier to forward, just using using.
         */
        nonvirtual void SetAssertExternallySynchronizedCheckerContext (const shared_ptr<SharedContext>& sharedContext);
#endif

    public:
        /**
         *  Saves current thread, and increments lock count, and
         *      \pre    already locked by this thread or no existing locks (either shared or exclusive)
         *
         *  \note   method non-const (can always const_cast if needed) because of standard C++ convention of non-const objects
         *          for write-lock
         */
        nonvirtual void lock () noexcept;

    public:
        /**
         * \brief Like lock() - if it would succeed, same then, but if would fail instead of assert out, just return false.
         */
        nonvirtual bool try_lock () noexcept;

    public:
        /**
         *  Just decrement lock count
         *
         *  \pre    still running on the same locking thread and locks not unbalanced
         */
        nonvirtual void unlock () noexcept;

    public:
        /**
         *  Saves current thread (multiset), and increments shared count, and
         *      \pre    no pre-existing locks on other threads
         *
         *  \note   method const despite usual lockable rules, so easier to work with 'const' objects being 'marked' as doing a read operation.
         */
        nonvirtual void lock_shared () const noexcept;

    public:
        /**
         *  Just decrement shared lock count (remove this thread from shared lock multiset)
         *
         *  \note   see lock_shard for why const.
         *
         *  \pre    still running on the same locking thread and locks not unbalanced
         */
        nonvirtual void unlock_shared () const noexcept;

    public:
        /**
         *  \brief Instantiate AssertExternallySynchronizedChecker::ReadContext to designate an area of code where protected data will be read
         * 
         *  This type alias makes a little more clear in reading code that the 'lock' is really just an assertion about thread safety
         * 
         *  Since AssertExternallySynchronizedChecker follows the concept 'mutex' you can obviously use any
         *  of the standard lockers in std::c++, but using AssertExternallySynchronizedChecker::ReadContext - makes it a little more clear
         *  self-documenting in your code, that you are doing this in a context where you are only reading the pseudo-locked data.
         * 
         *  \note we get away with 'const' in shared_lock<const AssertExternallySynchronizedChecker> because we chose to make
         *        lock_shared, and unlock_shared const methods (see their docs above).
         * 
         *  \note - though CTOR not declared noexcept, ReadContext cannot throw an exception (it asserts out on failure)
         */
        using ReadContext = shared_lock<const AssertExternallySynchronizedChecker>;
        static_assert (movable<ReadContext> and not copyable<ReadContext>);

    public:
        /**
         *  \brief Instantiate AssertExternallySynchronizedChecker::WriteContext to designate an area of code where protected data will be written
         * 
         *  This type alias makes a little more clear in reading code that the 'lock' is really just an assertion about thread safety
         * 
         *  Since AssertExternallySynchronizedChecker follows the concept 'mutex' you can obviously use any
         *  of the standard lockers in std::c++, but using AssertExternallySynchronizedChecker::WriteContext - makes it a little more clear
         *  self-documenting in your code, that you are doing this in a context where you are only writing the pseudo-locked data.
         * 
         *  Plus, the fact that it forces a non-const interpretation on the object in question (by using lock_guard of a non-const AssertExternallySynchronizedChecker)
         *  makes it a little easier to catch cases where you accidentally use WriteContext and meant ReadContext.
         * 
         *  \note - used lock_guard before Stroika v3.0d10, but switched to unique_lock so movable (handy in some cases). 
         *          And performance not much of an issue since this is all debug-only code.
         * 
         *  \note - though CTOR not declared noexcept, WriteContext cannot throw an exception (it asserts out on failure)
         */
        using WriteContext = unique_lock<AssertExternallySynchronizedChecker>;
        static_assert (movable<WriteContext> and not copyable<WriteContext>);

#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
    private:
        nonvirtual void lock_ () noexcept;
        nonvirtual bool try_lock_ () noexcept;
        nonvirtual void unlock_ () noexcept;
        nonvirtual void lock_shared_ () const noexcept;
        nonvirtual void unlock_shared_ () const noexcept;

    private:
        shared_ptr<SharedContext> fSharedContext_;

    private:
        static mutex& GetSharedLockMutexThreads_ (); // MUTEX ONLY FOR fSharedLocks_ (could do one mutex per AssertExternallySynchronizedChecker but static probably performs better)
#endif
    };
    /*
     *  See 'Satisfies Concepts' in the class docs above.
     *
     *  The copyable/movable half is DELIBERATE and load-bearing - do not "correct" it to match std::mutex,
     *  which is neither copyable nor movable. That difference is not an oversight, it follows from this not
     *  being a mutex: it is a checker that objects EMBED as a data member (or inherit from). Stroika is a
     *  copy-by-value framework - String, Sequence<T>, Mapping<T> and friends must stay copyable and movable -
     *  so anything they embed must be too. Deleting these operations here would make every object holding one
     *  non-copyable, which is ~181 files at last count.
     *
     *  What copy/move actually do is check, not transfer: no lock state is carried across, and the
     *  SharedContext stays tied to the l-value. They assert the source (and for move, the target) is not
     *  locked out from under the operation - see the constructor and operator= notes above.
     */
    static_assert (movable<AssertExternallySynchronizedChecker> and copyable<AssertExternallySynchronizedChecker> and
                   Common::StdCompat::Lockable<AssertExternallySynchronizedChecker>);
    static_assert (not movable<mutex> and not copyable<mutex>); // the contrast above, made checkable

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "AssertExternallySynchronizedChecker.inl"

#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedChecker_h_*/
