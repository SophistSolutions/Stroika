
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_TimingTrace_inl_
#define _Stroika_Foundation_Debug_TimingTrace_inl_ 1

#include "Trace.h"

namespace Stroika::Foundation::Debug {

    /*
     ********************************************************************************
     ***************************** Debug::TimingTrace *******************************
     ********************************************************************************
     */
    inline TimingTrace::TimingTrace (Time::DurationSeconds warnIfLongerThan)
        : WhenTimeExceeded
    {
        warnIfLongerThan,
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
            [] (Time::DurationSeconds timeTaken) noexcept { DbgTrace ("(timeTaken=%f seconds)", timeTaken.count ()); }
#else
            nullptr
#endif
    }
    {
    }
    inline TimingTrace::TimingTrace ([[maybe_unused]] const char* label, Time::DurationSeconds warnIfLongerThan)
        : WhenTimeExceeded
    {
        warnIfLongerThan,
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
            [=] (Time::DurationSeconds timeTaken) noexcept { DbgTrace ("%s (timeTaken=%f seconds)", label, timeTaken.count ()); }
#else
            nullptr
#endif
    }
    {
    }
    inline TimingTrace::TimingTrace ([[maybe_unused]] const wchar_t* label, Time::DurationSeconds warnIfLongerThan)
        : WhenTimeExceeded
    {
        warnIfLongerThan,
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
            [=] (Time::DurationSeconds timeTaken) noexcept { DbgTrace (L"%s (timeTaken=%f seconds)", label, timeTaken.count ()); }
#else
            nullptr
#endif
    }
    {
    }
    inline TimingTrace::TimingTrace ([[maybe_unused]] const Characters::String& label, Time::DurationSeconds warnIfLongerThan)
        : WhenTimeExceeded
    {
        warnIfLongerThan,
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
            [=] (Time::DurationSeconds timeTaken) noexcept {
                DbgTrace (L"%s (timeTaken=%f seconds)", label.As<wstring> ().c_str (), timeTaken.count ());
            }
#else
            nullptr
#endif
    }
    {
    }

}

#endif /*_Stroika_Foundation_Debug_TimingTrace_inl_*/
