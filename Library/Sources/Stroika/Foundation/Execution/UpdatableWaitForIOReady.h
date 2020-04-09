/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
     *   @todo redo by aggregating WaitForIOReady
     * wiith our own collection with extra item...
     *  \note   Calls to Add/Remove/clear/SetDescriptors () cause Wait() to return prematurely, almost as if a timeout, except that
     *        a timeout exception won't be caused by this return (per-se).

     MUST IMPLEMENT TRICK WITH PIPE/socket/fdevent handler to 'wakeup' select without thread interrupt
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
         *  If no flags specified, remove all occurences of fd.
         */
        nonvirtual void Remove (T fd);
        nonvirtual void Remove (T fd, const TypeOfMonitorSet& flags);

    public:
        /**
         */
        nonvirtual void RemoveAll (const Traversal::Iterable<T>& fds);
        nonvirtual void RemoveAll (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds);

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
