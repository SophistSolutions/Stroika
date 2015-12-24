/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_h_  1

#include    "../StroikaPreComp.h"

#include    <atomic>
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
 *      @todo   We should add a RECURSIVE AssertExternallySynchronizedLock (or CTOR flag??). So we can check diff
 *              threads but allow recursive code.
 *
 *      @todo   Consider LOOSER version of Debug::AssertExternallySynchronizedLock with READ and WRITE locks, and then
 *              in Optional � for CONST methods use the READLOCK variant, and, THIS allows multiple readers, so
 *              long as there are no writers. Then we can make OPTIONAL usage even more efficient, allowing for
 *              Optioanl<> use without synchronized, if all readers, but as soon as a writer gets into the party � assert out!
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
             *          inline  void    DoStuffOnData ()
             *          {
             *              lock_guard<const AssertExternallySynchronizedLock> critSec { *this };
             *              // now do what you usually do for DOStuffOnData...
             *          }
             *      };
             *      \endcode
             *
             *  \note   Until Stroika v2.0a119, this was a non-recursive mutex, but it is now recursive.
             */
            class   AssertExternallySynchronizedLock {
            public:
                /**
                 *  \note   Copy/Move constructor locks and unlocks quickly to detect if other locks exist while copying.
                 */
                AssertExternallySynchronizedLock () = default;
                AssertExternallySynchronizedLock (AssertExternallySynchronizedLock&& src);
                AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src);

            public:
                /**
                 *  \note   operator= locks and unlocks quickly to detect if other locks exist while copying.
                 */
                nonvirtual  AssertExternallySynchronizedLock& operator= (AssertExternallySynchronizedLock && rhs);
                nonvirtual  AssertExternallySynchronizedLock& operator= (const AssertExternallySynchronizedLock& rhs);

            public:
                /**
                 *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
                 *          methods without casts.
                 */
                nonvirtual  void    lock () const;

            public:
                /**
                 *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
                 *          methods without casts.
                 */
                nonvirtual  void    unlock () const;

            public:
                /**
                 *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
                 *          methods without casts.
                 *
                 *  @todo NYI
                 */
                nonvirtual  void    lock_shared () const;

            public:
                /**
                 *  \note   method const despite usual lockable rules, since we inherit from this, can use on const
                 *          methods without casts.
                 *
                 *  @todo NYI
                 */
                nonvirtual  void    unlock_shared () const;

#if     qDebug
            private:
                mutable atomic_uint_fast32_t    fLocks_ { 0 };
                mutable atomic_uint_fast32_t    fSharedLocks_ { 0 };
                mutable thread::id              fCurThread_;
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
