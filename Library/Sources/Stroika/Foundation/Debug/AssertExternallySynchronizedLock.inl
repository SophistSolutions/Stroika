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
#if     qDebug
                lock_guard<mutex> sharedLockProtect { fSharedLockThreadsMutex_.Get () };
                Require (src.fLocks_ == 0 and src.fSharedLockThreads_.empty ());  // to move, the src can have no locks of any kind (since we change src)
#endif
            }
            inline  AssertExternallySynchronizedLock&   AssertExternallySynchronizedLock::operator= (const AssertExternallySynchronizedLock& rhs)
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec1 { rhs };    // to copy, the src can have shared_locks, but no (write) locks
#if     qDebug
                lock_guard<mutex> sharedLockProtect { fSharedLockThreadsMutex_.Get () };
                Require (rhs.fLocks_ == 0 and rhs.fSharedLockThreads_.empty ());        // We must not have any locks going to replace this
#endif
                return *this;
            }
            inline  AssertExternallySynchronizedLock&   AssertExternallySynchronizedLock::operator= (AssertExternallySynchronizedLock&& rhs)
            {
#if     qDebug
                lock_guard<mutex> sharedLockProtect { fSharedLockThreadsMutex_.Get () };
                Require (rhs.fLocks_ == 0 and rhs.fSharedLockThreads_.empty ());    // to move, the rhs can have no locks of any kind (since we change rhs)
                Require (fLocks_ == 0 and fSharedLockThreads_.empty ());            // ditto for thing being assigned to
#endif
                return *this;
            }
            inline  void    AssertExternallySynchronizedLock::lock () const
            {
#if     qDebug
                if (fLocks_++ == 0) {
                    // If first time in, save thread-id
                    fCurLockThread_ = this_thread::get_id ();
                    lock_guard<mutex> sharedLockProtect { fSharedLockThreadsMutex_.Get () };
                    if (not fSharedLockThreads_.empty ()) {
                        // If first already shared locks - OK - so long as same thread
                        Require (fSharedLockThreads_.count (fCurLockThread_) == fSharedLockThreads_.size ());
                    }
                }
                else {
                    // If first already locked - OK - so long as same thread
                    Require (fCurLockThread_ == this_thread::get_id ());
                }
#endif
            }
            inline  void    AssertExternallySynchronizedLock::unlock () const
            {
#if     qDebug
                Require (fCurLockThread_ == this_thread::get_id ());
                Require (fLocks_ > 0);  // else unbalanced
                --fLocks_;
#endif
            }
            inline  void    AssertExternallySynchronizedLock::lock_shared () const
            {
#if     qDebug
                // OK to shared lock from various threads
                // But if already locked, NOT OK (would have blocked in real impl) - if you try to shared lock from another thread while locked
                if (fLocks_ != 0) {
                    // If first already locks - OK - so long as same thread
                    Require (fCurLockThread_ == this_thread::get_id ());
                }
                lock_guard<mutex> sharedLockProtect { fSharedLockThreadsMutex_.Get () };
                fSharedLockThreads_.insert (this_thread::get_id ());
#endif
            }
            inline  void    AssertExternallySynchronizedLock::unlock_shared () const
            {
#if     qDebug
                lock_guard<mutex> sharedLockProtect { fSharedLockThreadsMutex_.Get () };
                multiset<std::thread::id>::iterator tti = fSharedLockThreads_.find (this_thread::get_id ());
                Require (tti != fSharedLockThreads_.end ());  // else unbalanced
                fSharedLockThreads_.erase (tti);
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Debug_AssertExternallySynchronizedLock_inl_*/
