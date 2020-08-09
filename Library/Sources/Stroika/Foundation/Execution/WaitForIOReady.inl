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
        // Containers::Collection{} to force CLONE/FREEZE of data, since elsewise it chould change without this class knowing (iterables not necessarily COW)
        : fPollData_{Containers::Collection<pair<T, TypeOfMonitorSet>>{fds}}
        , fPollable2Wakeup_{pollable2Wakeup}
    {
        //DbgTrace (L"WaitForIOReady::CTOR (%s, %s)", Characters::ToString (fds).c_str (), Characters::ToString (pollable2Wakeup).c_str ());
    }
    template <typename T, typename TRAITS>
    WaitForIOReady<T, TRAITS>::WaitForIOReady (const Traversal::Iterable<T>& fds, const TypeOfMonitorSet& flags, optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup)
        : WaitForIOReady (fds.template Select<pair<T, TypeOfMonitorSet>> ([&] (const T& t) { return make_pair (t, flags); }), pollable2Wakeup)
    {
    }
    template <typename T, typename TRAITS>
    WaitForIOReady<T, TRAITS>::WaitForIOReady (T fd, const TypeOfMonitorSet& flags, optional<pair<SDKPollableType, TypeOfMonitorSet>> pollable2Wakeup)
        : WaitForIOReady (Containers::Collection<pair<T, TypeOfMonitorSet>>{make_pair (fd, flags)}, pollable2Wakeup)
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
        Thread::CheckForInterruption ();
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
        for (const auto& i : fPollData_) {
            pollBuffer->push_back (pair<SDKPollableType, TypeOfMonitorSet>{TRAITS::GetSDKPollable (i.first), i.second});
            mappedObjectBuffer->push_back (i.first);
        }
        if (fPollable2Wakeup_) {
            pollBuffer->push_back (pair<SDKPollableType, TypeOfMonitorSet>{fPollable2Wakeup_.value ().first, fPollable2Wakeup_.value ().second});
        }
    }

}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Execution::WaitForIOReady_Support::WaitForIOReady_Base::TypeOfMonitor> DefaultNames<Execution::WaitForIOReady_Support::WaitForIOReady_Base::TypeOfMonitor>::k{
        EnumNames<Execution::WaitForIOReady_Support::WaitForIOReady_Base::TypeOfMonitor>::BasicArrayInitializer{{
            {Execution::WaitForIOReady_Support::WaitForIOReady_Base::TypeOfMonitor::eRead, L"Read"},
            {Execution::WaitForIOReady_Support::WaitForIOReady_Base::TypeOfMonitor::eWrite, L"Write"},
            {Execution::WaitForIOReady_Support::WaitForIOReady_Base::TypeOfMonitor::eError, L"Error"},
            {Execution::WaitForIOReady_Support::WaitForIOReady_Base::TypeOfMonitor::eHUP, L"HUP"},
        }}};
}

#endif /*_Stroika_Foundation_Execution_WaitForIOReady_inl_*/
