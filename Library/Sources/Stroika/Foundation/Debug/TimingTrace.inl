
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Debug_TimingTrace_inl_
#define _Stroika_Foundation_Debug_TimingTrace_inl_    1


#include    "Trace.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /*
             ********************************************************************************
             ***************************** Debug::TimingTrace *******************************
             ********************************************************************************
             */
            inline TimingTrace::TimingTrace (Time::DurationSecondsType warnIfLongerThan)
                : WhenTimeExceeded ([] (Time::DurationSecondsType timeTaken) { DbgTrace("(timeTaken=%f seconds)", timeTaken); })
            {
            }
            inline TimingTrace::TimingTrace (Time::DurationSecondsType warnIfLongerThan, const Characters::String& label)
                : WhenTimeExceeded ([label] (Time::DurationSecondsType timeTaken) { DbgTrace (L"%s (timeTaken=%f seconds)", label.c_str (), timeTaken); })
            {
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Debug_TimingTrace_inl_*/
