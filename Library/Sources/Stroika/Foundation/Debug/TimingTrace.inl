/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

#include "Stroika/Foundation/Characters/Format.h"
#include "Trace.h"

namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     ***************************** Debug::TimingTrace *******************************
     ********************************************************************************
     */
    inline TimingTrace::TimingTrace ([[maybe_unused]] Time::DurationSeconds warnIfLongerThan)
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        : WhenTimeExceeded{warnIfLongerThan, [this] (Time::DurationSeconds timeTaken) noexcept {
                               if (fShowIfTimeExceeded_) {
                                   DbgTrace (Characters::FormatString<char>{"(timeTaken={} seconds)"}, timeTaken.count ());
                               }
                           }}
#endif
    {
    }
    inline TimingTrace::TimingTrace ([[maybe_unused]] const char* label, Time::DurationSeconds warnIfLongerThan)
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        : WhenTimeExceeded{warnIfLongerThan, [=, this] (Time::DurationSeconds timeTaken) noexcept {
                               if (fShowIfTimeExceeded_) {
                                   DbgTrace (Characters::FormatString<char>{"{} (timeTaken={} seconds)"}, Characters::String{label},
                                             timeTaken.count ());
                               }
                           }}
#endif
    {
    }
    inline TimingTrace::TimingTrace ([[maybe_unused]] const wchar_t* label, Time::DurationSeconds warnIfLongerThan)
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        : WhenTimeExceeded{warnIfLongerThan, [=, this] (Time::DurationSeconds timeTaken) noexcept {
                               if (fShowIfTimeExceeded_) {
                                   DbgTrace (Characters::FormatString<char>{"{} (timeTaken={} seconds)"}, label, timeTaken.count ());
                               }
                           }}
#endif
    {
    }
    inline TimingTrace::TimingTrace ([[maybe_unused]] const Characters::String& label, Time::DurationSeconds warnIfLongerThan)
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        : WhenTimeExceeded{warnIfLongerThan, [=, this] (Time::DurationSeconds timeTaken) noexcept {
                               if (fShowIfTimeExceeded_) {
                                   DbgTrace (Characters::FormatString<char>{"{} (timeTaken={} seconds)"}, label, timeTaken.count ());
                               }
                           }}
#endif
    {
    }
    inline void TimingTrace::Suppress ()
    {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        fShowIfTimeExceeded_ = false;
#endif
    }

}
