
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_TimingTrace_inl_
#define _Stroika_Foundation_Debug_TimingTrace_inl_ 1

#include "Trace.h"

namespace Stroika {
    namespace Foundation {
        namespace Debug {

            /*
             ********************************************************************************
             ***************************** Debug::TimingTrace *******************************
             ********************************************************************************
             */
            inline TimingTrace::TimingTrace (Time::DurationSecondsType warnIfLongerThan)
                : WhenTimeExceeded (warnIfLongerThan, [](Time::DurationSecondsType timeTaken) noexcept { DbgTrace ("(timeTaken=%f seconds)", timeTaken); })
            {
            }
#if defined(__clang__) && !defined(__APPLE__)
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wunknown-pragmas\"");
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wunused-lambda-capture\"");
#endif
            inline TimingTrace::TimingTrace (const char* label, Time::DurationSecondsType warnIfLongerThan)
                : WhenTimeExceeded (warnIfLongerThan, [label](Time::DurationSecondsType timeTaken) noexcept { DbgTrace ("%s (timeTaken=%f seconds)", label, timeTaken); })
            {
            }
            inline TimingTrace::TimingTrace (const wchar_t* label, Time::DurationSecondsType warnIfLongerThan)
                : WhenTimeExceeded (warnIfLongerThan, [label](Time::DurationSecondsType timeTaken) noexcept { DbgTrace (L"%s (timeTaken=%f seconds)", label, timeTaken); })
            {
            }
            inline TimingTrace::TimingTrace (const Characters::String& label, Time::DurationSecondsType warnIfLongerThan)
                : WhenTimeExceeded (warnIfLongerThan, [label](Time::DurationSecondsType timeTaken) noexcept { DbgTrace (L"%s (timeTaken=%f seconds)", label.c_str (), timeTaken); })
            {
            }
#if defined(__clang__) && !defined(__APPLE__)
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wunused-lambda-capture\"");
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wunknown-pragmas\"");
#endif
        }
    }
}
#endif /*_Stroika_Foundation_Debug_TimingTrace_inl_*/
