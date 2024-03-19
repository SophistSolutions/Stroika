/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ConditionVariable_h_
#define _Stroika_Foundation_Execution_ConditionVariable_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <condition_variable>
#include <mutex>

#include "Stroika/Foundation/Time/Realtime.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  \file
 *
 */

namespace Stroika::Foundation::Execution {

    /**
     *  ConditionVariable is a thin abstraction/wrapper on a std::condition_variable, with support for Stroika thread Cancelation, and 
     *  other shortcuts to simplify use. (combines related mutex with condition variable). Since you always use a
     *  condition variable with a mutex, its helpful to also include the associated mutex in the condition variable (type can be
     *  changed with template parameters).
     *
     *  Intentionally leave 'mutex' and 'condition_variable' elements public, because users will OFTEN
     *  need to directly access a reference to the mutex, and so there is little opportunity to hide.
     *
     *  This is just meant to codify some good practices and share some code. Its a VERY thin wrapper - if even
     *  that - on the std::mutex.
     *
     *  \par Example Usage
     *      \code
     *          ConditionVariable<> fConditionVariable;
     *          bool                fTriggered = false;
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
     *              Throw (TimeOutException::kThe);
     *          }
     *      \endcode
     *
     *  \note   Design Note on implementing thread cancelation
     * (@todo UPDATE DOCS IN LIGHT OF C++20)
     *          >   There are three obvious ways to implement cancelation
     *
     *              >   Uinsg EINTR (POSIX) and alertable states on windows - this is what we do most other places
     *                  in Stroika for thread cancelability.
     *
     *                  Sadly, the POSIX (gcc/clang) of condition_variable don't appear to support EINTR, the windows
     *                  implementation doesn't support alertable states, and (FIND REFERENCE***) I THINK I saw documented
     *                  someplace the standard mentions that this cannot be done (no idea why)
     *
     *              >   Maintain a list of 'waiting' condition variables, and then have thread-abort notify_all() on each of these
     *                  This significantly adds to the cost of waiting (add/remove safely from linked list with thread safety) - but
     *                  thats probably still the best approach.
     *
     *                  Also have to code this very carefully to avoid deadlocks, since the cancelation code needs to lock something to
     *                  allow it to safely signal the condition variable.
     *
     *                  ***Probably best approach but trickier todo***
     *
     *              >   Just wake periodically (shorten the wait time) and look for aborts). This is arguably the most costly approach
     *                  but also simplest to implement, and what we have for now (as of January 2018, version v2.0a225)
     *
     *  \note   From http://en.cppreference.com/w/cpp/thread/condition_variable
     *              "Even if the shared variable is atomic, it must be modified under the mutex in order
     *              to correctly publish the modification to the waiting thread."
     *
     *          I believe this is because:
     *              The total ordering created by this pattern of update is crucial!
     *
     *              Thread A lock and then unlocks mutex, so change of state of triggered always BEFORE (or after)
     *              the change of state in the condition variable mutex, so no ambiguity about when fTriggered set.
     *
     *              If its set before, condition variable sees that on entry (in its pred check).
     *
     *              If its after, then on the mutex unlock (I DON'T KNOW HOW THIS HAPPENS YET) - the Condition
     *              var must somehow get 'awkakened' - probably by a TLS list (queue) of waiters to get notififed
     *              and that gets hooked in the mutex code???? GUESSING.
     *
     *              So anyhow - note - its critical when changing values of underlying 'condition' - to wrap that in mutex
     *              lock/unlock.
     *
     *              This MAYBE related to why "std::condition_variable works only with std::unique_lock<std::mutex>".
     *              BUT - then i don't understand how " std::condition_variable_any provides a condition variable that works with any BasicLockable"
     * 
     *  \note use of condition_variable (not condition_variable_any) with stop_token
     *        See https://stackoverflow.com/questions/66309276/why-does-c20-stdcondition-variable-not-support-stdstop-token
     *              Before Stroika v3, we used 
     *                  typename CONDITION_VARIABLE = conditional_t<is_same_v<mutex, MUTEX>, condition_variable, condition_variable_any>
     *              For better stop_token interop, we've changed the default to
     *                  typename CONDITION_VARIABLE = condition_variable_any
     */
    template <typename MUTEX = mutex, typename CONDITION_VARIABLE = condition_variable_any>
    struct ConditionVariable {
        /**
         *  This is the type of the mutex associated with the condition variable.
         */
        using MutexType = MUTEX;

        /**
         *  This is the type of condition variable. This is generally going to be condition_variable_any (so it will work with stop_tokens)
         *  but could be condition_variable (or others).
         */
        using ConditionVariableType = CONDITION_VARIABLE;

        /**
         *  explicitly unlockable lock (on the mutex). Use, for example, with condition variables, where the apis need to unlock/lock and track
         *  the 'locked' state.
         */
        using LockType = unique_lock<MUTEX>;

        /**
         *  just lock and unlock. Basically the same as LockType, but less flexible (cannot explicitly unlock) and more efficient (cuz no data
         *  to track if locked).
         */
        using QuickLockType = lock_guard<MUTEX>;

        /*
         * https://stackoverflow.com/questions/66309276/why-does-c20-stdcondition-variable-not-support-stdstop-token
         * 
         *  Seems simple enuf to use 
         *        std::stop_callback callback{ stoken, [&cv]{ cv.notify_all(); } };
         *  But according to that stackoverflow link, its unsafe. MSFT implementation basically does this anyhow.
         *  So only support stop_token for condition_variable_any.
         *          -- LGP 2023-10-06
         */
#if __cpp_lib_jthread < 201911
        static constexpr bool kSupportsStopToken = false;
#else
        static constexpr bool kSupportsStopToken = same_as<CONDITION_VARIABLE, condition_variable_any>;
#endif

        /**
         *  sConditionVariableWaitChunkTime is used iff kSupportsStopToken is false.
         * 
         *  When waiting, with kSupportsStopToken false, this max timeout is used chunk the waits into smaller chunks so
         *  we can check for thread cancelation.
         * 
         *      This time value is generally NOT used in condition variable operation, except in a few disparate situations:
         * 
         *              o       stop_token overloads not supported (see kSupportsStopToken)
         *              o       ? may change - cannot interrupt main thread (abort) so have no stop token to use/pass? why does that matter?
         *                
         *      \note in Stroika v2.1 this was called sThreadAbortCheckFrequency_Default
         */
        static inline Time::DurationSeconds sConditionVariableWaitChunkTime{0.25s};

        /**
         */
        ConditionVariable ()                                               = default;
        ConditionVariable (const ConditionVariable&)                       = delete;
        nonvirtual ConditionVariable& operator= (const ConditionVariable&) = delete;

        /**
         *  ConditionVariable is a very THIN abstraction. Callers will often need to explicitly access/use the mutex
         */
        MutexType fMutex;

        /**
         *  ConditionVariable is a very THIN abstraction. Callers will often need to explicitly access/use the condition_variable
         */
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
         *  \note   ***NOT a Cancelation Point***
         */
        nonvirtual void release_and_notify_one (LockType& lock) noexcept;

        /**
         * NOTIFY the condition variable (notify_all), but unlock first due to:                  
         *      http://en.cppreference.com/w/cpp/thread/condition_variable/notify_all
         *
         *          The notifying thread does not need to hold the lock on the same mutex as the 
         *          one held by the waiting thread(s); in fact doing so is a pessimization, since
         *          the notified thread would immediately block again, waiting for the notifying
         *
         *  \note   ***NOT a Cancelation Point***
         */
        nonvirtual void release_and_notify_all (LockType& lock) noexcept;

        /**
         *  \brief forward notify_one () call to underlying std::condition_variable'
         *
         *  \note   ***NOT a Cancelation Point***
         */
        nonvirtual void notify_one () noexcept;

        /**
         *  \brief forward notify_all () call to underlying std::condition_variable'
         *
         *  \note   ***NOT a Cancelation Point***
         */
        nonvirtual void notify_all () noexcept;

        /**
         *  Like condition_variable wait_until, except
         *      using float instead of chrono (fix)
         *      supports Stroika thread interruption
         *
         *      readyToWake    -   predicate which returns false if the waiting should be continued.
         *
         *  Returns:
         *      1) std::cv_status::timeout if the relative timeout specified by rel_time expired, std::cv_status::no_timeout otherwise.
         *      2) true of 'readyToWake' () is reason we woke
         *
         *  \note   ***Cancelation Point***
         *
         *  \note   The intention here is to be semantically IDENTICAL to condition_variable::wait_until () - except
         *          for adding support for thread interruption (and a minor point - Time::TimePointSeconds)
         * 
         *  \note the cv_status returning overload CAN return/wakeup spurriously (not timeout, and not desired condition true)
         *        The PREDICATE (readyToWake) overload, never returns a spurrious wake (so only returns timeout (false) or true if readyToWake returned true.
         *
         *  \req (lock.owns_lock ());
         *  \ensure (lock.owns_lock ());
         * 
         * @todo maybe lose the (no predicate) overload --LGP 2023-10-09
         */
        nonvirtual cv_status wait_until (LockType& lock, Time::TimePointSeconds timeoutAt);
        template <invocable PREDICATE>
        nonvirtual bool wait_until (LockType& lock, Time::TimePointSeconds timeoutAt, PREDICATE&& readyToWake);

        /**
         * Like condition_variable wait_for, except
         *      using float instead of chrono (fix)
         *      supports Stroika thread interruption
         *
         *     readyToWake    -   predicate which returns false if the waiting should be continued.
         *
         * Returns:
         *     1) std::cv_status::timeout if the relative timeout specified by rel_time expired, std::cv_status::no_timeout otherwise.
         *     2) true of 'readyToWake' () is reason we woke
         *
         *  \note   ***Cancelation Point***
         *
         *  \note   The intention here is to be semantically IDENTICAL to condition_variable::wait_for () - except
         *          for adding support for thread interruption (and a minor point - Time::TimePointSeconds)
         * 
         *  \note the cv_status returning overload CAN return/wakeup spurriously (not timeout, and not desired condition true)
         *        The PREDICATE (readyToWake) overload, never returns a spurrious wake (so only returns timeout (false) or true if readyToWake returned true.
         *
         *  \req (lock.owns_lock ());
         *  \ensure (lock.owns_lock ());
         */
        nonvirtual cv_status wait_for (LockType& lock, Time::DurationSeconds timeout);
        template <invocable PREDICATE>
        nonvirtual bool wait_for (LockType& lock, Time::DurationSeconds timeout, PREDICATE&& readyToWake);

        /**
         *  Idea is you pass lambda to do actual data change, and this acquires lock first, and notifies all after.
         *
         *  \par Example Usage
         *      \code
         *          // from BlockingQueue code...
         *          fCondtionVariable_.MutateDataNotifyAll ([=]() { fEndOfInput_ = true; });
         *      \endcode
         *
         *  \note   ***Not Cancelation Point***     -- but perhaps should be???
         */
        template <invocable FUNCTION>
        nonvirtual void MutateDataNotifyAll (FUNCTION&& mutatorFunction);

        /**
         *  Idea is you pass lambda to do actual data change, and this acquires lock first, and notifies one after.
         *
         *  \par Example Usage
         *      \code
         *          // from BlockingQueue code...
         *          fCondtionVariable_.MutateDataNotifyOne ([=]() { fEndOfInput_ = true; });
         *      \endcode
         *
         *  \note   ***Not Cancelation Point***     -- but perhaps should be???
         */
        template <invocable FUNCTION>
        nonvirtual void MutateDataNotifyOne (FUNCTION&& mutatorFunction);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConditionVariable.inl"

#endif /*_Stroika_Foundation_Execution_ConditionVariable_h_*/
