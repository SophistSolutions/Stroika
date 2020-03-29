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
    inline void WaitForIOReady<T, TRAITS>::clear ()
    {
        fPollData_.rwget ()->clear ();
    }
    template <typename T, typename TRAITS>
    inline auto WaitForIOReady<T, TRAITS>::GetDescriptors () const -> Containers::Collection<pair<T, TypeOfMonitorSet>>
    {
        return fPollData_;
    }
    template <typename T, typename TRAITS>
    inline void WaitForIOReady<T, TRAITS>::AddAll (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags)
    {
        for (auto i : fds) {
            Add (i, flags);
        }
    }
    template <typename T, typename TRAITS>
    inline void WaitForIOReady<T, TRAITS>::AddAll (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        for (auto i : fds) {
            Add (i.first, i.second);
        }
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
    inline WaitForIOReady<T, TRAITS>::WaitForIOReady (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags)
    {
        AddAll (fds, flags);
    }
    template <typename T, typename TRAITS>
    inline WaitForIOReady<T, TRAITS>::WaitForIOReady (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        AddAll (fds);
    }
    template <typename T, typename TRAITS>
    inline WaitForIOReady<T, TRAITS>::WaitForIOReady (T fd, const TypeOfMonitorSet& flags)
    {
        Add (fd, flags);
    }
    template <typename T, typename TRAITS>
    void WaitForIOReady<T, TRAITS>::Add (T fd, const TypeOfMonitorSet& flags)
    {
        fPollData_.rwget ()->Add (pair<T, TypeOfMonitorSet>{fd, flags});
    }
    template <typename T, typename TRAITS>
    void WaitForIOReady<T, TRAITS>::Remove ([[maybe_unused]] T fd)
    {
        AssertNotImplemented ();
    }
    template <typename T, typename TRAITS>
    void WaitForIOReady<T, TRAITS>::Remove ([[maybe_unused]] T fd, [[maybe_unused]] const TypeOfMonitorSet& flags)
    {
        AssertNotImplemented ();
    }
    template <typename T, typename TRAITS>
    void WaitForIOReady<T, TRAITS>::RemoveAll ([[maybe_unused]] const Traversal::Iterable<T>& fds)
    {
        AssertNotImplemented ();
    }
    template <typename T, typename TRAITS>
    void WaitForIOReady<T, TRAITS>::RemoveAll ([[maybe_unused]] const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        AssertNotImplemented ();
    }
    template <typename T, typename TRAITS>
    void WaitForIOReady<T, TRAITS>::SetDescriptors (const Traversal::Iterable<pair<T, TypeOfMonitorSet>>& fds)
    {
        fPollData_.store (Containers::Collection<pair<T, TypeOfMonitorSet>>{fds});
    }
    template <typename T, typename TRAITS>
    auto WaitForIOReady<T, TRAITS>::WaitUntil (Time::DurationSecondsType timeoutAt) -> Containers::Set<T>
    {
        Containers::Set<T> result = WaitQuietlyUntil (timeoutAt);
        if (result.empty ()) {
            Execution::Throw (Execution::TimeOutException::kThe);
        }
        return result;
    }
    template <typename T, typename TRAITS>
    auto WaitForIOReady<T, TRAITS>::WaitQuietlyUntil (Time::DurationSecondsType timeoutAt) -> Containers::Set<T>
    {
        CheckForThreadInterruption ();
        vector<pair<SDKPollableType, TypeOfMonitorSet>> pollBuffer;
        vector<T>                                       mappedObjectBuffer;
        FillBuffer_ (&pollBuffer, &mappedObjectBuffer);
        Assert (pollBuffer.size () == mappedObjectBuffer.size ());
        Containers::Set<T> result;
        for (size_t i : _WaitQuietlyUntil (Containers::Start (pollBuffer), Containers::End (pollBuffer), timeoutAt)) {
            Assert (i < mappedObjectBuffer.size ());
            result.Add (mappedObjectBuffer[i]);
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
        auto lockedPollData = fPollData_.cget ();
        pollBuffer->reserve (lockedPollData->size ());
        mappedObjectBuffer->reserve (lockedPollData->size ());
        for (auto i : lockedPollData.cref ()) {
            pollBuffer->push_back (pair<SDKPollableType, TypeOfMonitorSet>{TraitsType::GetSDKPollable (i.first), i.second});
            mappedObjectBuffer->push_back (i.first);
        }
    }

}

#endif /*_Stroika_Foundation_Execution_WaitForIOReady_inl_*/
