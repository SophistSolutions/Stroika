/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitableEvent_h_
#define _Stroika_Foundation_Execution_WaitableEvent_h_ 1

#include "../StroikaPreComp.h"

#include <forward_list>
#include <set>

#include "../Configuration/Common.h"

#include "ConditionVariable.h"
#include "Exceptions.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *
 *      @todo   Consider if there is a need for timed mutexes.
 *              Reviewed 2014-02-09, and I'm pretty sure not needed.
 *
 *      @todo   Consider doing an #if qPlatform_Windows implemeantion using Windows 'CreateEvent'. I had this in Stroika
 *              circa 2013-06-01, and could just grab that implementation (really pretty simple).
 *
 *              But unless there is a performance or other such issue, it maybe better to stick to a single portable
 *              implementation.
 */

namespace Stroika::Foundation::Execution {

    /*
     *  UNSURE IF/HOW LONG WE WANT TO SUPPORT THIS API. EXPERIMENTAL!
     *  (introduced in Stroika 2.0a20 - 2014-02-08)
     */
#ifndef qExecution_WaitableEvent_SupportWaitForMultipleObjects
#define qExecution_WaitableEvent_SupportWaitForMultipleObjects 1
#endif

    /**
     *  AutoReset Waitable Event (like Windows' CreateEvent (resetType==eManualReset, false)).
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  \note   \em async-signal-safety - this is NOT safe to use from signals (from http://pubs.opengroup.org/onlinepubs/009695399/functions/pthread_cond_broadcast.html - It is not safe to use the pthread_cond_signal() function in a signal handler that is invoked asynchronously
     *          Use POSIX sem_init/sem_post () 
     *
     *  \note   \em Design Note     Considered making this copyable, or at least movable, but mutex and
     *              other similar classes are not.
     *              and you can easily use shared_ptr<> on an WaitableEvent to make it copyable.
     *
     *  \note   \em Design Note     WaitForAny/WaitForAnyUntil and WaitForMultipleEvents
     *
     *  \note   ***Cancelation Point***
     *
     *  @see    qExecution_WaitableEvent_SupportWaitForMultipleObjects
     *
     *      This appears to be an issue with strong arguments on both sides. I'm very uncertain.
     *
     *      I've used the Windows WaitForMutlipleObjects API for years, and feel pretty comfortable with it.
     *      I find it handy.
     *
     *      However, there are compelling arguments (for example):
     *          https://groups.google.com/forum/#!msg/comp.unix.programmer/q2x0yQR5txk/34v1qQZN_u0J
     *          https://groups.google.com/forum/#!msg/comp.unix.programmer/WsgZZmu4ESA/Rv1MCun1CmUJ
     *
     *      which argue that its a bad idea, and that it leads to bad programming (bugs).
     *
     *      To some extent I we may have addressed the reported concerns by having WaitForAny/WaitForAnyUntil
     *      return the full set of events that were signaled (and the issue about races I don't think applies
     *      to WaitForAll).
     *
     *      Note also that some recommend using the BlockingQueue<> pattern to avoid WaitForMultipleObjects (WaitForAny)
     *      such as:
     *              http://stackoverflow.com/questions/788835/waitformultipleobjects-in-java
     *      Stroika supports this sort of BlockingQueue<>
     *
     *      Note - WaitForAny() takes a templated SET because I don't want to create interdependency
     *      with Containers and something this low level, and yet we want to make it easy for users of this
     *      to use Stroika Set<> objects.
     *
     *      References:
     *          o   Notes on implementing Windows WaitForMultipleEvents API using POSIX (which are similar to stdc++) APIs:
     *              https://www.hackerzvoice.net/madchat/windoz/coding/winapi/waitfor_api.pdf
     *
     *          o   Interesting notes on how to implement WaitForMultipleEvents
     *              http://lists.boost.org/Archives/boost/2004/12/77175.php
     *
     */
    class WaitableEvent {
    public:
        /**
         *  eManualReset is the simplest to understand. Wait only returns when the event is set, and the only thing that can set it
         *  is a call to Set().
         *
         *  eAutoReset is sometimes useful when you have a single thread waiting on the event, and need to wake that thread repeatedly, each time an event happens.
         *
         *  The only difference between eManualReset and eAutoReset is that when a Wait() succeeds, as the very last step in returning
         *  a successful wait, the event is automatically 'Reset'.
         *
         *  \note This means that AutoReset events are unsuitable for use when multiple threads are waiting on an event, and you wish to
         *        make them all up. By definition, only ONE thread wakes up to recieve the 'AutoReset' event.
         */
        enum class ResetType {
            eAutoReset,
            eManualReset,

            Stroika_Define_Enum_Bounds (eAutoReset, eManualReset)
        };

    public:
        static constexpr ResetType eAutoReset = ResetType::eAutoReset;

    public:
        static constexpr ResetType eManualReset = ResetType::eManualReset;

    public:
        /**
         *  \note   WaitableEvent () defaults to 'manual reset' - because it often doesn't matter how the waitable event gets reset (because it never does)
         *          and this (manual reset) is the least surprising behavior.
         */
        WaitableEvent (ResetType resetType = eManualReset);
        WaitableEvent (WaitableEvent&&)      = delete;
        WaitableEvent (const WaitableEvent&) = delete;

    public:
        /**
         *  \note the user of this class must assure all waiters have completed their wait before destrying the event (checked with assertions).
         */
#if qDebug || qStroika_FeatureSupported_Valgrind
        ~WaitableEvent ();
#else
        ~WaitableEvent () = default;
#endif

    public:
        nonvirtual WaitableEvent& operator= (const WaitableEvent&&) = delete;
        nonvirtual WaitableEvent& operator= (const WaitableEvent&) = delete;

    public:
        /**
         *  Set the event to the non-signaled state
         *
         *  \note   This COULD have been called 'UnSet'.
         */
        nonvirtual void Reset ();

    public:
        /**
         *  This checks if the event is currently in a triggered state. Regardless of the type of event
         *  (autoreset or not) - this does not change the trigger state.
         */
        nonvirtual bool PeekIsSet () const noexcept;

    public:
        /**
         *  Set the event to the signaled state.
         *
         *  \note   This COULD have been called 'Signal', or 'SetSignaled'.
         */
        nonvirtual void Set ();

    public:
        /**
         *  Simple wait. Can use operator HANDLE() to do fancier waits.
         *  timeout can be negative (which triggers an immediate exception).
         *
         *  TimeOutException throws if the timeout is exceeeded.
         *
         *  \note Wait (0) will always throw TimeOutException () regardless of the state of the event/trigger
         *
         *  @see WaitQuietly ()
         *  @see PeekIsSet ()
         *
         *  \note   ***Cancelation Point***
         */
        nonvirtual void Wait (Time::DurationSecondsType timeout = Time::kInfinite);

    public:
        /**
         */
        static constexpr bool kWaitQuietlyTimeoutResult{false};

    public:
        /**
         */
        static constexpr bool kWaitQuietlySetResult{true};

    public:
        /**
         *  Wait the given period of time, and return true if event occured (Set called), and false on timeout.
         *  This is mostly useful if we want a wait, for advisory purposes (say to avoid races), but don't
         *  want an exception as its not an issue to handle specially.
         *
         *  Returns:    true (kWaitQuietlySetResult) if event signaled/occurred, and false (kWaitQuietlyTimeoutResult) if timeout
         *
         *  \note   WaitQuietly (0) will always return false regardless of the state of the event/trigger
         *
         *  \note   WaitQuietly() can raise exceptions, but only Thread::InterruptException
         *
         *  @see Wait ()
         *  @see WaitUntil ()
         *  @see WaitUntilQuietly ()
         *  @see PeekIsSet ()
         *
         *  \note   ***Cancelation Point***
         */
        nonvirtual bool WaitQuietly (Time::DurationSecondsType timeout = Time::kInfinite);

    public:
        /**
         *  TimeOutException throws if the event is not signaled before timeoutAt is
         *  exceeeded (includes when reached).
         *
         *  @see Wait ()
         *  @see WaitQuietly ()
         *  @see WaitUntilQuietly ()
         *
         *  \note   ***Cancelation Point***
         */
        nonvirtual void WaitUntil (Time::DurationSecondsType timeoutAt);

    public:
        /**
         *
         *  \note   WaitUntilQuietly() can raise exceptions, but only Thread::AbortException
         *
         *  Returns:    true (kWaitQuietlySetResult) if event signaled/occurred, and false (kWaitQuietlyTimeoutResult) if timeout
         *
         *  @see Wait ()
         *  @see WaitQuietly ()
         *  @see WaitUntil ()
         *
         *  \note   ***Cancelation Point***
         */
        nonvirtual bool WaitUntilQuietly (Time::DurationSecondsType timeoutAt);

#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
    public:
        /**
         *  Note - CONTAINER_OF_WAITABLE_EVENTS - must iterate over WaitableEvent*!
         *
         *  \note   WaitForAny IS EXPERIMENTAL
         *
         *  \note   ***Cancelation Point***
         */
        template <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT = set<WaitableEvent*>>
        static SET_OF_WAITABLE_EVENTS_RESULT WaitForAny (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout = Time::kInfinite);
        template <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT = set<WaitableEvent*>>
        static SET_OF_WAITABLE_EVENTS_RESULT WaitForAny (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout = Time::kInfinite);

    public:
        /**
         *  Note - CONTAINER_OF_WAITABLE_EVENTS - must iterate over WaitableEvent*!
         *
         *  \note   WaitForAny IS EXPERIMENTAL
         *
         *  \note   ***Cancelation Point***
         */
        template <typename CONTAINER_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT = set<WaitableEvent*>>
        static SET_OF_WAITABLE_EVENTS_RESULT WaitForAnyUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt);
        template <typename ITERATOR_OF_WAITABLE_EVENTS, typename SET_OF_WAITABLE_EVENTS_RESULT = set<WaitableEvent*>>
        static SET_OF_WAITABLE_EVENTS_RESULT WaitForAnyUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt);

    public:
        /**
         *  Note - CONTAINER_OF_WAITABLE_EVENTS - must iterate over WaitableEvent*!
         *
         *  \note   WaitForAll IS EXPERIMENTAL
         *
         *  \note   ***Cancelation Point***
         */
        template <typename CONTAINER_OF_WAITABLE_EVENTS>
        static void WaitForAll (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeout = Time::kInfinite);
        template <typename ITERATOR_OF_WAITABLE_EVENTS>
        static void WaitForAll (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeout = Time::kInfinite);

    public:
        /**
         *  Note - CONTAINER_OF_WAITABLE_EVENTS - must iterate over WaitableEvent*!
         *
         *  \note   WaitForAllUntil IS EXPERIMENTAL
         *
         *  \note   ***Cancelation Point***
         */
        template <typename CONTAINER_OF_WAITABLE_EVENTS>
        static void WaitForAllUntil (CONTAINER_OF_WAITABLE_EVENTS waitableEvents, Time::DurationSecondsType timeoutAt);
        template <typename ITERATOR_OF_WAITABLE_EVENTS>
        static void WaitForAllUntil (ITERATOR_OF_WAITABLE_EVENTS waitableEventsStart, ITERATOR_OF_WAITABLE_EVENTS waitableEventsEnd, Time::DurationSecondsType timeoutAt);
#endif

    private:
        struct WE_ {
            ResetType                   fResetType;
            mutable ConditionVariable<> fConditionVariable{};
            bool                        fTriggered{false};

            WE_ (ResetType resetType);
            nonvirtual void Reset ();
            nonvirtual bool PeekIsSet () const noexcept;
            nonvirtual void Set ();
            nonvirtual void WaitUntil (Time::DurationSecondsType timeoutAt);
            nonvirtual bool WaitUntilQuietly (Time::DurationSecondsType timeoutAt);
        };
        WE_ fWE_;
#if qExecution_WaitableEvent_SupportWaitForMultipleObjects
        forward_list<shared_ptr<WE_>> fExtraWaitableEvents_;
#endif
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WaitableEvent.inl"

#endif /*_Stroika_Foundation_Execution_WaitableEvent_h_*/
