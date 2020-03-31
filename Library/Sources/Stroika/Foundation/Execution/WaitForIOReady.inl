/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_WaitForIOReady_inl_
#define _Stroika_Foundation_Execution_WaitForIOReady_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "TimeOutException.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************** Execution::WaitForIOReady ***************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline WaitForIOReady<T, TRAITS>::WaitForIOReady (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds, optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup)
        : fPollData_{fds}
        , fPollable2Wakeup_{pollable2Wakeup}
    {
    }
    template <typename T, typename TRAITS>
    inline WaitForIOReady<T, TRAITS>::WaitForIOReady (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags, optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup)
        : fPollData_{}
        , fPollable2Wakeup_{pollable2Wakeup}
    {
        Containers::Collection<pair<T, TypeOfMonitorSet>> tmp;
        for (auto i : fds) {
            tmp.Add (pair<T, TypeOfMonitorSet>{i, flags});
        }
        fPollData_ = tmp;
    }
    template <typename T, typename TRAITS>
    inline WaitForIOReady<T, TRAITS>::WaitForIOReady (T fd, const TypeOfMonitorSet& flags, optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup)
        : fPollData_{Containers::Collection<pair<T, TypeOfMonitorSet>>{pair<T, TypeOfMonitorSet>{fd, flags}}}
        , fPollable2Wakeup_{pollable2Wakeup}
    {
    }
    template <typename T, typename TRAITS>
    inline auto WaitForIOReady<T, TRAITS>::GetDescriptors () const -> Traversal::Iterable<pair<T, TypeOfMonitorSet>>
    {
        return fPollData_;
    }
    template <typename T, typename TRAITS>
    inline auto WaitForIOReady<T, TRAITS>::Wait (Time::DurationSecondsType waitFor) -> Containers::Set<T>
    {
        return WaitUntil (waitFor + Time::GetTickCount ());
    }
    template <typename T, typename TRAITS>
    inline auto WaitForIOReady<T, TRAITS>::Wait (const Time::Duration& waitFor) -> Containers::Set<T>
    {
        return WaitUntil (waitFor.As<Time::DurationSecondsType> () + Time::GetTickCount ());
    }
    template <typename T, typename TRAITS>
    inline auto WaitForIOReady<T, TRAITS>::WaitQuietly (Time::DurationSecondsType waitFor) -> Containers::Set<T>
    {
        return WaitQuietlyUntil (waitFor + Time::GetTickCount ());
    }
    template <typename T, typename TRAITS>
    inline auto WaitForIOReady<T, TRAITS>::WaitQuietly (const Time::Duration& waitFor) -> Containers::Set<T>
    {
        return WaitQuietly (waitFor.As<Time::DurationSecondsType> ());
    }
    template <typename T, typename TRAITS>
    auto WaitForIOReady<T, TRAITS>::WaitUntil (Time::DurationSecondsType timeoutAt) -> Containers::Set<T>
    {
        Containers::Set<T> result = WaitQuietlyUntil (timeoutAt);
        if (result.empty ()) {
            Execution::ThrowTimeoutExceptionAfter (timeoutAt); // maybe returning 0 entries without timeout, because of fPollable2Wakeup_
        }
        return result;
    }
    template <typename T, typename TRAITS>
    auto WaitForIOReady<T, TRAITS>::WaitQuietlyUntil (Time::DurationSecondsType timeoutAt) -> Containers::Set<T>
    {
        CheckForThreadInterruption ();
        vector<pair<SDKPollableType, TypeOfMonitorSet>> pollBuffer;
        vector<T>                                       mappedObjectBuffer;
        // @todo REDO THIS calling FillBuffer_ from CTOR (since always used at least once, but could be more than once.
        FillBuffer_ (&pollBuffer, &mappedObjectBuffer);
        Assert (pollBuffer.size () == mappedObjectBuffer.size () or pollBuffer.size () == mappedObjectBuffer.size () + 1);
        Containers::Set<T> result;
        for (size_t i : _WaitQuietlyUntil (Containers::Start (pollBuffer), Containers::End (pollBuffer), timeoutAt)) {
            if (i == mappedObjectBuffer.size ()) {
                Assert (fPollable2Wakeup_); // externally signalled to wakeup
            }
            else {
                Assert (i < mappedObjectBuffer.size ());
                result.Add (mappedObjectBuffer[i]);
            }
        }
        return result;
    }
    template <typename T, typename TRAITS>
    void WaitForIOReady<T, TRAITS>::FillBuffer_ (vector<pair<SDKPollableType, TypeOfMonitorSet>>* pollBuffer, vector<T>* mappedObjectBuffer)
    {
        RequireNotNull (pollBuffer);
        RequireNotNull (mappedObjectBuffer);
        Require (pollBuffer->size () == 0);
        Require (mappedObjectBuffer->size () == 0);
        pollBuffer->reserve (fPollData_.size ());
        mappedObjectBuffer->reserve (fPollData_.size ());
        for (auto i : fPollData_) {
            pollBuffer->push_back (pair<SDKPollableType, TypeOfMonitorSet>{TRAITS::GetSDKPollable (i.first), i.second});
            mappedObjectBuffer->push_back (i.first);
        }
        if (fPollable2Wakeup_) {
            pollBuffer->push_back (pair<SDKPollableType, TypeOfMonitorSet>{fPollable2Wakeup_.value ().first, fPollable2Wakeup_.value ().second});
        }
    }

    /*
     ********************************************************************************
     ******************* Execution::UpdatableWaitForIOReady *************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline UpdatableWaitForIOReady<T, TRAITS>::UpdatableWaitForIOReady (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
        : fData_{fds}
    {
        /// @todo invoke
        // SET
        // fPollable2Wakeup_
        // here - 3 ways
        // and ALSO include FD for where to WRITE to signal wakeup
    }
    template <typename T, typename TRAITS>
    inline UpdatableWaitForIOReady<T, TRAITS>::UpdatableWaitForIOReady (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags)
        : UpdatableWaitForIOReady (fds.Select<pair<T, TypeOfMonitorSet>> ([&] (const T& t) { return pair<T, TypeOfMonitorSet>{t, flags}; }))
    {
    }
    template <typename T, typename TRAITS>
    inline UpdatableWaitForIOReady<T, TRAITS>::UpdatableWaitForIOReady (T fd, const TypeOfMonitorSet& flags)
        : UpdatableWaitForIOReady (Containers::Collection<pair<T, TypeOfMonitorSet>>{pair<T, TypeOfMonitorSet>{fd, flags}})
    {
    }
    template <typename T, typename TRAITS>
    inline void UpdatableWaitForIOReady<T, TRAITS>::clear ()
    {
        fData_.rwget ().clear ();
    }
    template <typename T, typename TRAITS>
    inline auto UpdatableWaitForIOReady<T, TRAITS>::GetDescriptors () const -> Containers::Collection<pair<T, TypeOfMonitorSet>>
    {
        return fData_.load ();
    }
    template <typename T, typename TRAITS>
    inline void UpdatableWaitForIOReady<T, TRAITS>::AddAll (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags)
    {
        for (auto i : fds) {
            Add (i, flags);
        }
    }
    template <typename T, typename TRAITS>
    inline void UpdatableWaitForIOReady<T, TRAITS>::AddAll (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        for (auto i : fds) {
            Add (i.first, i.second);
        }
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
        fData_.rwget ()->fData_.Add (pair<T, TypeOfMonitorSet>{fd, flags});
        // @todo wakeup
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::Remove ([[maybe_unused]] T fd)
    {
        AssertNotImplemented ();
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::Remove ([[maybe_unused]] T fd, [[maybe_unused]] const TypeOfMonitorSet& flags)
    {
        AssertNotImplemented ();
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::RemoveAll ([[maybe_unused]] const Traversal::Iterable<T>& fds)
    {
        AssertNotImplemented ();
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::RemoveAll ([[maybe_unused]] const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        AssertNotImplemented ();
    }
    template <typename T, typename TRAITS>
    void UpdatableWaitForIOReady<T, TRAITS>::SetDescriptors (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        fData_.store (fds);
        // @todo wakeup
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
        return mkWaiter_ ().WaitQuietlyUntil (timeoutAt);
    }
    template <typename T, typename TRAITS>
    inline WaitForIOReady<T, TRAITS> UpdatableWaitForIOReady<T, TRAITS>::mkWaiter_ ()
    {
        return WaitForIOReady<T, TRAITS>{fData_.load (), fPollable2Wakeup_};
    }

}

#endif /*_Stroika_Foundation_Execution_WaitForIOReady_inl_*/
