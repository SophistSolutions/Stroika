/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ConditionVariable_h_
#define _Stroika_Foundation_Execution_ConditionVariable_h_ 1

#include "../StroikaPreComp.h"

#include <condition_variable>
#include <mutex>

#include "../Time/Realtime.h"

/**
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 *  \file
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
             *
             *  \par Example Usage
             *      \code
             *          bool fTriggered = false;
             *
             *          // THREAD A:
             *           fConditionVariable.MutateDataNotifyAll ([=]() { fTriggered = true; });
             *
             *          // THREAD B:
             *          std::unique_lock<mutex> lock (fConditionVariable.fMutex);
             *          if (fConditionVariable.wait_until (lock, timeoutAt, [this]() { return fTriggered; })) {
             *              REACT TO CHANGE ();
             *          }
             *          else {
             *              Throw (TimeOutException ());
             *          }
             *      \endcode
             *
             *  \note   @todo FIXUP REST OF THIS DOC - https://stroika.atlassian.net/browse/STK-623
             *          
             *          Important to understand about condition variables, and I've never found documented anywhere
             *          very well, is that the total ordering created by this pattern of update is crucial!
             *
             *          Thread A lock and then unlocks mutex, so change of state of triggered always BEFORE (or after)
             *          the change of state in the condition variable mutex, so no ambiguity about when fTriggered set.
             *
             *          If its set before, condition variable sees that on entry (in its pred check).
             *
             *          If its after, then on the mutex unlock (I DONT KNOW HOW THIS HAPPENS YET) - the COndition
             *          var must somehow get 'awkakened' - probably by a TLS list (queue) of waiters to get notififed
             *          and that gets hooked in the mutex code???? GUESSING.
             *
             *          So anyhow - note - its critical when changing values of underlying 'condition' - to wrap that in mutex
             *          lock/unlock.
             *
             *          SOURCE OF INFO  - http://en.cppreference.com/w/cpp/thread/condition_variable 
             *          "Even if the shared variable is atomic, it must be modified under the mutex in order to correctly publish the modification to the waiting thread."
             *
             *          This is probably why "std::condition_variable works only with std::unique_lock<std::mutex>".
             *          BUT - then i dont understnad how " std::condition_variable_any provides a condition variable that works with any BasicLockable"
             */
            template <typename MUTEX = mutex, typename CONDITION_VARIABLE = typename std::conditional<is_same<mutex, MUTEX>::value, condition_variable, condition_variable_any>::type>
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

                using MutexType             = MUTEX;
                using ConditionVariableType = CONDITION_VARIABLE;
                using LockType              = std::unique_lock<MUTEX>;
                using QuickLockType         = std::lock_guard<MUTEX>;

                MutexType          fMutex;
                CONDITION_VARIABLE fConditionVariable;

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
                nonvirtual void release_and_notify_one (LockType& lock);

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
                nonvirtual void release_and_notify_all (LockType& lock);

                /**
                 *  \brief forward notify_one () call to underlying std::condition_variable'
                 */
                nonvirtual void notify_one () noexcept;

                /**
                 *  \brief forward notify_all () call to underlying std::condition_variable'
                 */
                nonvirtual void notify_all () noexcept;

                /**
                 *  Like condition_variable wait_until, except
                 *      using float instead of chrono (fix)
                 *      supports Stroika thread interruption
                 *
                 *      readToWake    -   predicate which returns false if the waiting should be continued.
                 *
                 *  Returns:
                 *      1) std::cv_status::timeout if the relative timeout specified by rel_time expired, std::cv_status::no_timeout otherwise.
                 *      2) true of 'readyToWake' () is reason we woke
                 *
                 *  \note   The intention here is to be semantically IDENTICAL to condition_variable::wait_until () - except
                 *          for adding support for thread interruption (and a minor point - Time::DurationSecondsType)
                 */
                nonvirtual cv_status wait_until (LockType& lock, Time::DurationSecondsType timeoutAt);
                template <typename PREDICATE>
                nonvirtual bool wait_until (LockType& lock, Time::DurationSecondsType timeoutAt, PREDICATE readToWake);

                /**
                 * Like condition_variable wait_for, except
                 *      using float instead of chrono (fix)
                 *      supports Stroika thread interruption
                 *
                 *     readToWake    -   predicate which returns false if the waiting should be continued.
                 *
                 * Returns:
                 *     1) std::cv_status::timeout if the relative timeout specified by rel_time expired, std::cv_status::no_timeout otherwise.
                 *     2) true of 'readyToWake' () is reason we woke
                 *
                 *  \note   The intention here is to be semantically IDENTICAL to condition_variable::wait_for () - except
                 *          for adding support for thread interruption (and a minor point - Time::DurationSecondsType)
                 */
                nonvirtual cv_status wait_for (LockType& lock, Time::DurationSecondsType timeout);
                template <typename PREDICATE>
                nonvirtual bool wait_for (LockType& lock, Time::DurationSecondsType timeout, PREDICATE readToWake);

                /**
                 *  NOT USED YET - ROUGH PROTOTYPE
                 *
                 *  Idea is you pass lambda to do actual data change, and this acquires lock first, and notifies all after.
                 */
                template <typename FUNCTION>
                nonvirtual void MutateDataNotifyAll (FUNCTION mutatorFunction);

                /**
                 *  NOT USED YET - ROUGH PROTOTYPE
                 *
                 *  Idea is you pass lambda to do actual data change, and this acquires lock first, and notifies one after.
                 */
                template <typename FUNCTION>
                nonvirtual void MutateDataNotifyOne (FUNCTION mutatorFunction);
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
