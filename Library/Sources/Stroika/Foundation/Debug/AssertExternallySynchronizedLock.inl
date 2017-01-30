/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace Debug {

            /*
             ********************************************************************************
             *************** Debug::AssertExternallySynchronizedLock ************************
             ********************************************************************************
             */
            inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src)
                : AssertExternallySynchronizedLock ()
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec1{src}; // to copy, the src can have shared_locks, but no (write) locks
            }
            inline AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (AssertExternallySynchronizedLock&& src)
                : AssertExternallySynchronizedLock ()
            {
#if qDebug
                lock_guard<mutex> sharedLockProtect{fSharedLockThreadsMutex_.Get ()};
                Require (src.fLocks_ == 0 and src.fSharedLockThreads_.empty ()); // to move, the src can have no locks of any kind (since we change src)
#endif
            }
            inline AssertExternallySynchronizedLock& AssertExternallySynchronizedLock::operator= (const AssertExternallySynchronizedLock& rhs)
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec1{rhs}; // to copy, the src can have shared_locks, but no (write) locks
#if qDebug
                lock_guard<mutex> sharedLockProtect{fSharedLockThreadsMutex_.Get ()};
                Require (rhs.fLocks_ == 0 and rhs.fSharedLockThreads_.empty ()); // We must not have any locks going to replace this
#endif
                return *this;
            }
            inline AssertExternallySynchronizedLock& AssertExternallySynchronizedLock::operator= (AssertExternallySynchronizedLock&& rhs)
            {
#if qDebug
                lock_guard<mutex> sharedLockProtect{fSharedLockThreadsMutex_.Get ()};
                Require (rhs.fLocks_ == 0 and rhs.fSharedLockThreads_.empty ()); // to move, the rhs can have no locks of any kind (since we change rhs)
                Require (fLocks_ == 0 and fSharedLockThreads_.empty ());         // ditto for thing being assigned to
#endif
                return *this;
            }
            inline void AssertExternallySynchronizedLock::lock () const
            {
#if qDebug
                lock_ ();
#endif
            }
            inline void AssertExternallySynchronizedLock::unlock () const
            {
#if qDebug
                unlock_ ();
#endif
            }
            inline void AssertExternallySynchronizedLock::lock_shared () const
            {
#if qDebug
                lock_shared_ ();
#endif
            }
            inline void AssertExternallySynchronizedLock::unlock_shared () const
            {
#if qDebug
                unlock_shared_ ();
#endif
            }
        }
    }
}
#endif /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_*/
