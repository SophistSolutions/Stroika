/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_UpdatableWaitForIOReady_h_
#define _Stroika_Foundation_Execution_UpdatableWaitForIOReady_h_ 1

#include "../StroikaPreComp.h"

#include "WaitForIOReady.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   NYI Remove/RemoveAll - easy but not used yet, so low priority
 *
 */

namespace Stroika::Foundation::Execution {

    /**
     *  \brief Simple wrapper on WaitForIOReady (POSIX select/poll/etc API) - except it allows for the list
     *         if polled descriptors to be updated dynamically from any thread, and have that automatically wakeup any
     *         waiters to restart waiting
     *
     *  \note   Calls to Add/Remove/clear/SetDescriptors () cause Wait() to return prematurely, almost as if a timeout, except that
     *          a timeout exception won't be caused by this return (per-se). This means wait can legitimately return zero items
     *          without throwing a timeout exception
     *
     *  \par Example Usage
     *      \code
     *          ...INSIDE CLASS DECLARATION
     *          struct MyWaitForIOReady_Traits_ {
     *              using HighLevelType = shared_ptr<Connection>;
     *              static inline auto GetSDKPollable (const HighLevelType& t)
     *              {
     *                  return t->GetSocket ().GetNativeSocket ();
     *              }
     *          };
     *          Execution::UpdatableWaitForIOReady<shared_ptr<Connection>, MyWaitForIOReady_Traits_> fSockSetPoller_{};
     *          ...
     *
     *          loopWaitingForWorkTodo()
     *              while (true) {
     *                  for (shared_ptr<Connection> readyConnection : fSockSetPoller_.WaitQuietly ()) {
     *                      ... handle ready connection
     *
     *          ... elsewhere
     *          onAccept(shared_ptr<Connection) c) {
     *              fSockSetPoller_.Add (c);
     *          }
     *
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#Internally-Synchronized-Thread-Safety">Internally-Synchronized-Thread-Safety</a>
     *
     *  \todo @todo DECIDE IF T is a KEY - that is - if allowed to appear more than once in the list.
     *        Update the logic for Add/Remove and constructors accordingly!!!
     *        PROBABLY best definition is that its a KEY, but you can in construcotr/add pass in more than once, and tehy get merged togther like a set.
     *        (NOTE - NYI but easy). Then Remove functions need overloads letting you specify the POLL params too.
     *
     */
    template <typename T = WaitForIOReady_Support::SDKPollableType, typename TRAITS = WaitForIOReady_Support::WaitForIOReady_Traits<T>>
    class UpdatableWaitForIOReady : public WaitForIOReady_Support::WaitForIOReady_Base {
    public:
        /**
         */
        UpdatableWaitForIOReady ();
        UpdatableWaitForIOReady (const UpdatableWaitForIOReady&) = delete;
        UpdatableWaitForIOReady (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds);
        UpdatableWaitForIOReady (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);
        UpdatableWaitForIOReady (const T& fd, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

    public:
        ~UpdatableWaitForIOReady () = default;

    public:
        nonvirtual UpdatableWaitForIOReady& operator= (const UpdatableWaitForIOReady&) = delete;

    public:
        /**
         */
        nonvirtual void Add (T fd, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

    public:
        /**
         */
        nonvirtual void AddAll (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds);
        nonvirtual void AddAll (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

    public:
        /*
         */
        nonvirtual void Remove (T fd);

    public:
        /**
         */
        nonvirtual void RemoveAll (const Traversal::Iterable<T>& fds);

    public:
        /**
         */
        nonvirtual Containers::Collection<pair<T, TypeOfMonitorSet>> GetDescriptors () const;

    public:
        /**
         */
        nonvirtual void SetDescriptors (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds);
        nonvirtual void SetDescriptors (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags = kDefaultTypeOfMonitor);

    public:
        /**
         */
        nonvirtual void clear ();

    public:
        /*
         *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *  \note   Throws TimeOutException () on timeout.
         *  
         *  \note   ***Cancelation Point***
         *
         *  @see WaitQuietly
         *  @see WaitUntil
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> Wait (Time::DurationSecondsType waitFor = Time::kInfinite);
        nonvirtual Containers::Set<T> Wait (const Time::Duration& waitFor);

    public:
        /*
         *  Waits the given amount of time, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *   Returns set of file descriptors which are ready, or empty set if timeout.
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitUntil
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> WaitQuietly (Time::DurationSecondsType waitFor = Time::kInfinite);
        nonvirtual Containers::Set<T> WaitQuietly (const Time::Duration& waitFor);

    public:
        /*
         *  Waits unil the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *  \note   Throws TimeOutException () on timeout.
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitQuietly
         *  @see WaitQuietlyUntil
         */
        nonvirtual Containers::Set<T> WaitUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

    public:
        /*
         *  Waits unil the given timeoutAt, and returns as soon as any one (or more) requires service (see TypeOfMonitor).
         *
         *  Returns set of file descriptors which are ready, or an empty set if time expired before any became ready.
         *
         *  if timeout is <= 0, this will not wait (but may still find some file desciptors ready).
         *
         *  \note   ***Cancelation Point***
         *
         *  @see Wait
         *  @see WaitQuietly
         *  @see WaitUntil
         */
        nonvirtual Containers::Set<T> WaitQuietlyUntil (Time::DurationSecondsType timeoutAt = Time::kInfinite);

    private:
        /*
         *  note: lock design - keep short locks on fData_, and make a new 'waiter' with a copy of the FDs list (COW copy)
         *  so don't hold fData synchronized lock during wait on 'select'
         */
    private:
        nonvirtual WaitForIOReady<T, TRAITS> mkWaiter_ ();

    private:
        shared_ptr<WaitForIOReady_Support::EventFD>                                fEventFD_;
        pair<SDKPollableType, TypeOfMonitorSet>                                    fPollable2Wakeup_;
        Execution::Synchronized<Containers::Collection<pair<T, TypeOfMonitorSet>>> fData_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "UpdatableWaitForIOReady.inl"

#endif /*_Stroika_Foundation_Execution_UpdatableWaitForIOReady_h_*/
