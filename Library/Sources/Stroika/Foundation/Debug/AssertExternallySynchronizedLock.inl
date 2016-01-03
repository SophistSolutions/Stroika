/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_
#define _Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <mutex>

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /*
             ********************************************************************************
             *************** Debug::AssertExternallySynchronizedLock ************************
             ********************************************************************************
             */
            inline  AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (const AssertExternallySynchronizedLock& src)
                : AssertExternallySynchronizedLock ()
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec1 { src };    // to copy, the src can have shared_locks, but no (write) locks
            }
            inline  AssertExternallySynchronizedLock::AssertExternallySynchronizedLock (AssertExternallySynchronizedLock&& src)
                : AssertExternallySynchronizedLock ()
            {
                Require (src.fLocks_ == 0 and src.fSharedLocks_ == 0);  // to move, the src can have no locks of any kind (since we change src)
            }
            inline  AssertExternallySynchronizedLock&   AssertExternallySynchronizedLock::operator= (const AssertExternallySynchronizedLock& rhs)
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec1 { rhs };    // to copy, the src can have shared_locks, but no (write) locks
                lock_guard<const AssertExternallySynchronizedLock> critSec2 { *this };  // We must not have any locks going to replace this
                return *this;
            }
            inline  AssertExternallySynchronizedLock&   AssertExternallySynchronizedLock::operator= (AssertExternallySynchronizedLock && rhs)
            {
                Require (rhs.fLocks_ == 0 and rhs.fSharedLocks_ == 0);  // to move, the rhs can have no locks of any kind (since we change rhs)
                return *this;
            }
            inline  void    AssertExternallySynchronizedLock::lock () const
            {
#if     qDebug
                if (fLocks_++ == 0) {
                    if (fSharedLocks_ == 0) {
                        // If first time in, save thread-id
                        fCurThread_ = this_thread::get_id ();
                    }
                    else {
                        // If first already shared locks - OK - so long as same thread
                        Require (fCurThread_ == this_thread::get_id ());
                    }
                }
                else {
                    // If first already locks - OK - so long as same thread
                    Require (fCurThread_ == this_thread::get_id ());
                }
#endif
            }
            inline  void    AssertExternallySynchronizedLock::unlock () const
            {
#if     qDebug
                Require (fCurThread_ == this_thread::get_id ());
                Require (fLocks_ > 0);  // else unbalanced
                --fLocks_;
#endif
            }
            inline  void    AssertExternallySynchronizedLock::lock_shared () const
            {
#if     qDebug
                if (fSharedLocks_++ == 0) {
                    if (fLocks_ == 0) {
                        // If first time in, save thread-id
                        fCurThread_ = this_thread::get_id ();
                    }
                    else {
                        // If first already locks - OK - so long as same thread
                        Require (fCurThread_ == this_thread::get_id ());
                    }
                }
                else {
                    // if incrementing shared lock count, no problem if other shared locks on differnt threads.
                    // @todo NOTE - we MISS the case where we are incrementing shared lock (read lock) when another thread has locked!
                    // cuz we currently only store fCurThread_ in one var for readers and writers. Probs need to separate this!!!
                    // --LGP 2015-12-24
                }
#endif
            }
            inline  void    AssertExternallySynchronizedLock::unlock_shared () const
            {
#if     qDebug
                Require (fCurThread_ == this_thread::get_id ());
                Require (fSharedLocks_ > 0);  // else unbalanced
                --fSharedLocks_;
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_*/
