/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_BlockingQueue_inl_
#define _Stroika_Foundation_Execution_BlockingQueue_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Streams/EOFException.h"
#include "TimeOutException.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************************** BlockingQueue<T> ******************************
     ********************************************************************************
     */
    template <typename T>
    BlockingQueue<T>::BlockingQueue (const Containers::Queue<T>& useQueue)
        : fQueue_{useQueue}
    {
        Require (useQueue.empty ()); // this constructor is only used to control the 'type' (data structure/backend) used by the Blocking Queue
    }
    template <typename T>
    inline void BlockingQueue<T>::AddTail (const T& e, Time::DurationSeconds /*timeout*/)
    {
        // Our locks are short-lived, so its safe to ignore the timeout - this will always be fast
        //
        // note also: this must be NotifyAll, not NotifyOne () - because we could wake a useless, ineffective thread, e.g. https://stackoverflow.com/questions/13774802/notify-instead-of-notifyall-for-blocking-queue
        fCondtionVariable_.MutateDataNotifyAll ([&, this] () {
            Require (not fEndOfInput_);
            fQueue_.AddTail (e);
        });
    }
    template <typename T>
    inline void BlockingQueue<T>::SignalEndOfInput ()
    {
        fCondtionVariable_.MutateDataNotifyAll ([this] () { fEndOfInput_ = true; });
    }
    template <typename T>
    inline bool BlockingQueue<T>::EndOfInputHasBeenQueued () const
    {
        // lock may not always be strictly needed, but could report stale (cross thread) value without lock
        typename ConditionVariable<>::QuickLockType critSection{fCondtionVariable_.fMutex};
        return fEndOfInput_;
    }
    template <typename T>
    inline bool BlockingQueue<T>::QAtEOF () const
    {
        typename ConditionVariable<>::QuickLockType critSection{fCondtionVariable_.fMutex};
        return fEndOfInput_ and fQueue_.empty ();
    }
    template <typename T>
    T BlockingQueue<T>::RemoveHead (Time::DurationSeconds timeout)
    {
        Time::TimePointSeconds waitTil = Time::GetTickCount () + timeout;
        while (true) {
            typename ConditionVariable<>::LockType waitableLock{fCondtionVariable_.fMutex}; // despite appearances to the contrary, not holding lock lock cuz condition variable unlocks before waiting
            if (optional<T> tmp = fQueue_.RemoveHeadIf ()) {
                // Only notify_all() on additions, cuz waiters just looking for more data
                return *tmp;
            }
            if (fEndOfInput_) [[unlikely]] {
                Execution::Throw (Streams::EOFException::kThe); // Since we always must return, and know we never will, throw timeout now
            }
            ThrowTimeoutExceptionAfter (waitTil);
            (void)fCondtionVariable_.wait_until (waitableLock, Time::Pin2SafeSeconds (waitTil),
                                                 [this] () { return fEndOfInput_ or not fQueue_.empty (); });
        }
    }
    template <typename T>
    optional<T> BlockingQueue<T>::RemoveHeadIfPossible (Time::DurationSeconds timeout)
    {
        Time::TimePointSeconds waitTil = Time::GetTickCount () + timeout;
        while (true) {
            typename ConditionVariable<>::LockType waitableLock{fCondtionVariable_.fMutex}; // despite appearances to the contrary, not holding lock lock cuz condition variable unlocks before waiting
            if (optional<T> tmp = fQueue_.RemoveHeadIf ()) {
                return tmp;
            }
            if (fEndOfInput_) {
                return nullopt; // on end of input, no point in waiting
            }
            if (Time::GetTickCount () > waitTil) {
                return nullopt; // on timeout, return 'missing'
            }
            (void)fCondtionVariable_.wait_until (waitableLock, Time::Pin2SafeSeconds (waitTil),
                                                 [this] () { return fEndOfInput_ or not fQueue_.empty (); });
        }
    }
    template <typename T>
    inline optional<T> BlockingQueue<T>::PeekHead () const
    {
        typename ConditionVariable<>::QuickLockType critSection{fCondtionVariable_.fMutex};
        return fQueue_.HeadIf ();
    }
    template <typename T>
    inline bool BlockingQueue<T>::empty () const
    {
        typename ConditionVariable<>::QuickLockType critSection{fCondtionVariable_.fMutex};
        return fQueue_.empty ();
    }
    template <typename T>
    inline size_t BlockingQueue<T>::size () const
    {
        typename ConditionVariable<>::QuickLockType critSection{fCondtionVariable_.fMutex};
        return fQueue_.size ();
    }
    template <typename T>
    inline size_t BlockingQueue<T>::length () const
    {
        return size ();
    }
    template <typename T>
    inline Containers::Queue<T> BlockingQueue<T>::GetQueue () const
    {
        typename ConditionVariable<>::QuickLockType critSection{fCondtionVariable_.fMutex};
        return fQueue_;
    }
    template <typename T>
    inline void BlockingQueue<T>::clear ()
    {
        typename ConditionVariable<>::LockType waitableLock{fCondtionVariable_.fMutex};
        fQueue_.clear ();
        if (fEndOfInput_) {
            fCondtionVariable_.release_and_notify_all (waitableLock); // cuz readers could be waiting and need to know no more
        }
    }

}

#endif /*_Stroika_Foundation_Execution_BlockingQueue_inl_*/
