
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
    inline TimingTrace::TimingTrace (Time::DurationSecondsType warnIfLongerThan)
        : WhenTimeExceeded (warnIfLongerThan, [](Time::DurationSecondsType timeTaken) noexcept { DbgTrace ("(timeTaken=%f seconds)", timeTaken); })
    {
    }
    inline TimingTrace::TimingTrace (const char* label, Time::DurationSecondsType warnIfLongerThan)
        : WhenTimeExceeded (warnIfLongerThan, [=](Time::DurationSecondsType timeTaken) noexcept { DbgTrace ("%s (timeTaken=%f seconds)", label, timeTaken); })
    {
    }
    inline TimingTrace::TimingTrace (const wchar_t* label, Time::DurationSecondsType warnIfLongerThan)
        : WhenTimeExceeded (warnIfLongerThan, [=](Time::DurationSecondsType timeTaken) noexcept { DbgTrace (L"%s (timeTaken=%f seconds)", label, timeTaken); })
    {
    }
    inline TimingTrace::TimingTrace (const Characters::String& label, Time::DurationSecondsType warnIfLongerThan)
        : WhenTimeExceeded (warnIfLongerThan, [=](Time::DurationSecondsType timeTaken) noexcept { DbgTrace (L"%s (timeTaken=%f seconds)", label.c_str (), timeTaken); })
    {
    }

}

#endif /*_Stroika_Foundation_Debug_TimingTrace_inl_*/
