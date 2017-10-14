/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_inl_
#define _Stroika_Foundation_Execution_BlockingQueue_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Time/Duration.h"

#include "TimeOutException.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /*
             ********************************************************************************
             ******************************** BlockingQueue<T> ******************************
             ********************************************************************************
             */
            template <typename T>
            BlockingQueue<T>::BlockingQueue (const Containers::Queue<T>& useQueue)
                : fQueue_ (useQueue)
            {
                Require (useQueue.empty ()); // this constructor is only used to control the 'type' (data structure/backend) used by the Blocking Queue
            }
            template <typename T>
            inline void BlockingQueue<T>::AddTail (const T& e, Time::DurationSecondsType /*timeout*/)
            {
                // Our locks are short-lived, so its safe to ignore the timeout - this will always be fast
                Require (not fEndOfInput_);
                typename LockPlusCondVar_::WaitableLockType waitableLock{fLockPlusCondVar_.fMutex_};
                fQueue_.AddTail (e);
                fLockPlusCondVar_.release_and_notify_all (waitableLock);
            }
            template <typename T>
            inline void BlockingQueue<T>::EndOfInput ()
            {
                typename LockPlusCondVar_::WaitableLockType waitableLock{fLockPlusCondVar_.fMutex_};
                fEndOfInput_ = true;
                fLockPlusCondVar_.release_and_notify_all (waitableLock); // like input cuz readers could be waiting and need to know no more
            }
            template <typename T>
            inline bool BlockingQueue<T>::IsAtEndOfInput () const
            {
                typename LockPlusCondVar_::WaitableLockType waitableLock{fLockPlusCondVar_.fMutex_}; // lock not strictly needed, but it avoids false-positive from lock checking tools
                return fEndOfInput_;
            }
            template <typename T>
            T BlockingQueue<T>::RemoveHead (Time::DurationSecondsType timeout)
            {
                Time::DurationSecondsType waitTil = Time::GetTickCount () + timeout;
                while (true) {
                    typename LockPlusCondVar_::WaitableLockType waitableLock{fLockPlusCondVar_.fMutex_};
                    if (Memory::Optional<T> tmp = fQueue_.RemoveHeadIf ()) {
                        // Only notify_all() on additions
                        return *tmp;
                    }
                    if (fEndOfInput_) {
                        Execution::Throw (Execution::TimeOutException::kThe); // Since we always must return, and know we never will, throw timeout now
                    }
                    ThrowTimeoutExceptionAfter (waitTil);
                    fLockPlusCondVar_.fConditionVariable_.wait_until (waitableLock, Time::DurationSeconds2time_point (waitTil));
                }
            }
            template <typename T>
            Memory::Optional<T> BlockingQueue<T>::RemoveHeadIfPossible (Time::DurationSecondsType timeout)
            {
                Time::DurationSecondsType waitTil = Time::GetTickCount () + timeout;
                while (true) {
                    typename LockPlusCondVar_::WaitableLockType waitableLock{fLockPlusCondVar_.fMutex_};
                    if (Memory::Optional<T> tmp = fQueue_.RemoveHeadIf ()) {
                        return tmp;
                    }
                    ThrowTimeoutExceptionAfter (waitTil);
                    fLockPlusCondVar_.fConditionVariable_.wait_until (waitableLock, Time::DurationSeconds2time_point (waitTil));
                }
            }
            template <typename T>
            inline Memory::Optional<T> BlockingQueue<T>::PeekHead () const
            {
                typename LockPlusCondVar_::QuickLockType quickLock{fLockPlusCondVar_.fMutex_};
                return fQueue_.HeadIf ();
            }
            template <typename T>
            inline bool BlockingQueue<T>::empty () const
            {
                typename LockPlusCondVar_::QuickLockType quickLock{fLockPlusCondVar_.fMutex_};
                return fQueue_.empty ();
            }
            template <typename T>
            inline size_t BlockingQueue<T>::GetLength () const
            {
                typename LockPlusCondVar_::QuickLockType quickLock{fLockPlusCondVar_.fMutex_};
                return fQueue_.GetLength ();
            }
            template <typename T>
            inline size_t BlockingQueue<T>::length () const
            {
                return GetLength ();
            }
            template <typename T>
            inline void BlockingQueue<T>::clear ()
            {
                typename LockPlusCondVar_::QuickLockType quickLock{fLockPlusCondVar_.fMutex_};
                fQueue_.clear ();
            }
        }
    }
}
#endif /*_Stroika_Foundation_Execution_BlockingQueue_inl_*/
