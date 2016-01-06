/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_  1

#include    "../StroikaPreComp.h"

#include    <atomic>
#include    <set>
#include    <thread>

#include    "../Configuration/Common.h"
#include    "../Debug/Assertions.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo   Shared Lock and Lock code now uncertain, and probably still a bit buggy, but closer to right semantics...
 *              Review/test...
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /**
             *  \brief      NOT a real lock - just a debugging infrastructure support tool so in debug builds we assure used threadsafe
             *
             * This class is used as a 'no op' in production builds, as a 'locker' for a class that needs
             *  no thread locking because its externally synchronized.
             *
             *  This 'lock tester' is recursive (a recursive-mutex).
             *
             *  Externally synchronized means that some external applicaiton control guarantees the seciton of code (or data)
             *  is only accessed by a single thread.
             *
             *  In debug builds, it enforces this fact through assertions.
             *
             *  \note   This doesn't gaurnatee catching all races, but it catches many incorrect thread usage cases
             *
             *  \note   This may 'catch' a race by having its internal data structures (multiset) corrupted. Interpret
             *          corrupt multiset as a likely race indicator
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
            class   AssertExternallySynchronizedLock {
            public:
                /**
                 *  \note   Copy/Move constructor checks for existing locks while copying.
                 *          Must be able to readlock source on copy, and have zero existing locks on target or move.
                 */
#if     !qDebug
                constexpr
#endif
                AssertExternallySynchronizedLock () = default;
                AssertExternallySynchronizedLock (AssertExternallySynchronizedLock&& src);
                AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src);

            public:
                /**
                 *  \note   operator= checks for existing locks while copying.
                 *          Must be able to readlock source on copy, and have zero existing locks on target or move.
                 */
                nonvirtual  AssertExternallySynchronizedLock& operator= (AssertExternallySynchronizedLock && rhs);
                nonvirtual  AssertExternallySynchronizedLock& operator= (const AssertExternallySynchronizedLock& rhs);

            public:
                /**
                 *  Saves current thread, and increments lock count, and
                 *      \req    no pre-existing lock or shared locks on other threads
                 *
                 *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
                 *          methods without casts.
                 */
                nonvirtual  void    lock () const;

            public:
                /**
                 *  Just decrement lock count
                 *
                 *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
                 *          methods without casts.
                 *
                 *  \req    still running on the same locking thread and locks not unbalanced
                 */
                nonvirtual  void    unlock () const;

            public:
                /**
                 *  Saves current thread, and increments shared count, and
                 *      \req    no pre-existing locks on other threads
                 *
                 *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
                 *          methods without casts.
                 */
                nonvirtual  void    lock_shared () const;

            public:
                /**
                 *  Just decrement shared lock count
                 *
                 *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
                 *          methods without casts.
                 *
                 *  \req    still running on the same locking thread and locks not unbalanced
                 */
                nonvirtual  void    unlock_shared () const;

#if     qDebug
            private:
                mutable atomic_uint_fast32_t    fLocks_ { 0 };
                mutable thread::id              fCurLockThread_;
                mutable multiset<thread::id>    fSharedLockThreads_;
#endif
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "AssertExternallySynchronizedLock.inl"

#endif  /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_*/
