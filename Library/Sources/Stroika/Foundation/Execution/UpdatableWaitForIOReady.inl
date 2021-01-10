/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_UpdatableWaitForIOReady_inl_
#define _Stroika_Foundation_Execution_UpdatableWaitForIOReady_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "TimeOutException.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ******************* Execution::UpdatableWaitForIOReady *************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    UpdatableWaitForIOReady<T, TRAITS>::UpdatableWaitForIOReady (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
        : fEventFD_ (WaitForIOReady_Support::mkEventFD ())
        , fPollable2Wakeup_{fEventFD_->GetWaitInfo ()}
        , fData_{fds}
    {
    }
    template <typename T, typename TRAITS>
    inline UpdatableWaitForIOReady<T, TRAITS>::UpdatableWaitForIOReady ()
        : UpdatableWaitForIOReady (Traversal::Iterable<pair<T, TypeOfMonitorSet>>{})
    {
    }
    template <typename T, typename TRAITS>
    UpdatableWaitForIOReady<T, TRAITS>::UpdatableWaitForIOReady (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags)
        : UpdatableWaitForIOReady (fds.template Select<pair<T, TypeOfMonitorSet>> ([&] (const T& t) { return make_pair (t, flags); }))
    {
    }
    template <typename T, typename TRAITS>
    UpdatableWaitForIOReady<T, TRAITS>::UpdatableWaitForIOReady (const T& fd, const TypeOfMonitorSet& flags)
        : UpdatableWaitForIOReady (Containers::Collection<pair<T, TypeOfMonitorSet>>{make_pair (fd, flags)})
    {
    }
    template <typename T, typename TRAITS>
    inline void UpdatableWaitForIOReady<T, TRAITS>::clear ()
    {
        fData_.rwget ().clear ();
        fEventFD_->Set (); // force wakeup of any waits
    }
    template <typename T, typename TRAITS>
    inline auto UpdatableWaitForIOReady<T, TRAITS>::GetDescriptors () const -> Containers::Collection<pair<T, TypeOfMonitorSet>>
    {
        return fData_.load ();
    }
    template <typename T, typename TRAITS>
    inline void UpdatableWaitForIOReady<T, TRAITS>::AddAll (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags)
    {
        {
            auto lk = fData_.rwget ();
            for (const auto& i : fds) {
                lk->Add (i, flags);
            }
        }
        fEventFD_->Set (); // force wakeup of any waits
    }
    template <typename T, typename TRAITS>
    inline void UpdatableWaitForIOReady<T, TRAITS>::AddAll (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        fData_.rwget ()->fData_.AddAll (fds);
        fEventFD_->Set (); // force wakeup of any waits
    }
    template <typename T, typename TRAITS>
    inline auto UpdatableWaitForIOReady<T, TRAITS>::Wait (Time::DurationSecondsType waitFor) -> Containers::Set<T>
    {
        return WaitUntil (waitFor + Time::GetTickCount ());
    }
    template <typename T, typename TRAITS>
    inline auto UpdatableWaitForIOReady<T, TRAITS>::Wait (const Time::Duration& waitFor) -> Containers::Set<T>
    {
        return WaitUntil (waitFor.As<Time::DurationSecondsType> () + Time::GetTickCount ());
    }
    template <typename T, typename TRAITS>
    inline auto UpdatableWaitForIOReady<T, TRAITS>::WaitQuietly (Time::DurationSecondsType waitFor) -> Containers::Set<T>
    {
        return WaitQuietlyUntil (waitFor + Time::GetTickCount ());
    }
    template <typename T, typename TRAITS>
    inline auto UpdatableWaitForIOReady<T, TRAITS>::WaitQuietly (const Time::Duration& waitFor) -> Containers::Set<T>
    {
        return WaitQuietly (waitFor.As<Time::DurationSecondsType> ());
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::Add (T fd, const TypeOfMonitorSet& flags)
    {
        fData_.rwget ()->Add (pair<T, TypeOfMonitorSet>{fd, flags});
        fEventFD_->Set (); // force wakeup of any waits
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::Remove ([[maybe_unused]] T fd)
    {
        if (fData_.rwget ()->Remove ([&] (auto p) { return p.first == fd; })) {
            fEventFD_->Set (); // force wakeup of any waits
        }
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::RemoveAll ([[maybe_unused]] const Traversal::Iterable<T>& fds)
    {
        Containers::Set<T> fdsSet{fds};
        if (fData_.rwget ()->RemoveAll ([&] (auto p) { return fdsSet.Contains (p.first); }) != 0) {
            fEventFD_->Set (); // force wakeup of any waits
        }
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::SetDescriptors (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        fData_.store (fds);
        fEventFD_->Set (); // force wakeup of any waits
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::SetDescriptors (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags)
    {
        SetDescriptors (fds.template Select<pair<T, TypeOfMonitorSet>> ([&] (const T& t) { return make_pair (t, flags); }));
    }
    template <typename T, typename TRAITS>
    auto UpdatableWaitForIOReady<T, TRAITS>::WaitUntil (Time::DurationSecondsType timeoutAt) -> Containers::Set<T>
    {
        Containers::Set<T> result = WaitQuietlyUntil (timeoutAt);
        if (result.empty ()) {
            Execution::ThrowTimeoutExceptionAfter (timeoutAt); // maybe returning 0 entries without timeout, because of fPollable2Wakeup_
        }
        return result;
    }
    template <typename T, typename TRAITS>
    inline auto UpdatableWaitForIOReady<T, TRAITS>::WaitQuietlyUntil (Time::DurationSecondsType timeoutAt) -> Containers::Set<T>
    {
        fEventFD_->Clear (); // Clear all pending 'list of sockets' change notificitions before we mkWaiter_ () - which grabs the current list to avoid race
        return mkWaiter_ ().WaitQuietlyUntil (timeoutAt);
    }
    template <typename T, typename TRAITS>
    inline WaitForIOReady<T, TRAITS> UpdatableWaitForIOReady<T, TRAITS>::mkWaiter_ ()
    {
        return WaitForIOReady<T, TRAITS>{fData_.load (), fPollable2Wakeup_};
    }

}

#endif /*_Stroika_Foundation_Execution_UpdatableWaitForIOReady_inl_*/
