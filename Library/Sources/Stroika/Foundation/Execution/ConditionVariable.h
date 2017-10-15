/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ConditionVariable_h_
#define _Stroika_Foundation_Execution_ConditionVariable_h_ 1

#include "../StroikaPreComp.h"

#include <condition_variable>
#include <mutex>

/**
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  \file
 *
 *
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             *  A simple wrapper on a std::condition_variable, with support for Stroika thread Cancelation, and 
             *  other shortcuts to simplify use.
             *
             *  Intentionally leave 'mutex' and 'condition_variable' elements public, because users will OFTEN
             *  need to directly access a reference to the mutex, and so there is little opportunity to hide.
             *
             *  This is just meant to codify some good practices and share some code. Its a VERY thin wrapper - if even
             *  that - on the std::mutex.
             */
            template <typename MUTEX = mutex>
            struct ConditionVariable {

                /**
                 *  @todo cleanup docs
                 *
                 *  This API shouldnt be needed - if we had a better underlying implementation, and beware, the API could go away
                 *  if we find a better way. But callers may find it advisible to control this timeout to tune performance.
                 *
                 *  The WaitableEvent class internally uses condition_variable::wait_for () - and this doesn't advertise support for
                 *  EINTR or using Windows SDK 'alertable states' - so its not clear how often it returns to allow checking
                 *  for aborts. This 'feature' allows us to periodically check. You dont want to check too often, or you
                 *  effecitvely busy wait, and this checking is ONLY needed for the specail, rare case of thread abort.
                 */
                static Time::DurationSecondsType sThreadAbortCheckFrequency_Default;
                Time::DurationSecondsType        fThreadAbortCheckFrequency{sThreadAbortCheckFrequency_Default};

                using MutexType = MUTEX;
                using LockType  = std::unique_lock<MUTEX>;

                MutexType          fMutex;
                condition_variable fConditionVariable;

                /**
                 * NOTIFY the condition variable (notify_one), but unlock first due to:                  
                 *      http://en.cppreference.com/w/cpp/thread/condition_variable/notify_all
                 *
                 *          The notifying thread does not need to hold the lock on the same mutex as the 
                 *          one held by the waiting thread(s); in fact doing so is a pessimization, since
                 *          the notified thread would immediately block again, waiting for the notifying
                 *          thread to release the lock.
                 *
                 *  \note https://stroika.atlassian.net/browse/STK-620 - helgrind workaround needed because of this unlock, but the unlock is still correct
                 *
                 */
                void release_and_notify_one (LockType& lock);

                /**
                 * NOTIFY the condition variable (notify_all), but unlock first due to:                  
                 *      http://en.cppreference.com/w/cpp/thread/condition_variable/notify_all
                 *
                 *          The notifying thread does not need to hold the lock on the same mutex as the 
                 *          one held by the waiting thread(s); in fact doing so is a pessimization, since
                 *          the notified thread would immediately block again, waiting for the notifying
                 *          thread to release the lock.
                 *
                 *  \note https://stroika.atlassian.net/browse/STK-620 - helgrind workaround needed because of this unlock, but the unlock is still correct
                 *
                 */
                void release_and_notify_all (LockType& lock);

                /**
                 *  Like condition_variable wait_until, except
                 *      using float instead of chrono (fix)
                 *      supports Stroika thread interruption
                 */
                cv_status wait_until (LockType& lock, Time::DurationSecondsType timeoutAt);
                template <typename _Predicate>
                bool wait_until (LockType& lock, Time::DurationSecondsType timeoutAt, _Predicate& _Pred);

                /**
                  * Like condition_variable wait_for, except
                  *      using float instead of chrono (fix)
                  *      supports Stroika thread interruption
                  */
                cv_status wait_for (LockType& lock, Time::DurationSecondsType timeout);
                template <typename _Predicate>
                bool wait_for (LockType& lock, Time::DurationSecondsType timeout, _Predicate& _Pred);

                /**
                 *  NOT USED YET - ROUGH PROTOTYPE
                 *
                 *  Idea is you pass lambda to do actual data change, and this acquires lock first, and notifies all after.
                 */
                template <typename FUNCTION>
                void MutateDataNotifyAll (FUNCTION mutatorFunction);

                /**
                 *  NOT USED YET - ROUGH PROTOTYPE
                 *
                 *  Idea is you pass lambda to do actual data change, and this acquires lock first, and notifies one after.
                 */
                template <typename FUNCTION>
                void MutateDataNotifyOne (FUNCTION mutatorFunction);
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConditionVariable.inl"

#endif /*_Stroika_Foundation_Execution_ConditionVariable_h_*/
