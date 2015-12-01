/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Debug_TimingTrace_h_
#define _Stroika_Foundation_Debug_TimingTrace_h_  1

#include    "../StroikaPreComp.h"

#include    "../Characters/String.h"
#include    "../Execution/WhenTimeExceeded.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Late">Alpha-Late</a>
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Debug {


            /**
             */
            class TimingTrace : private Execution::WhenTimeExceeded {
            public:
                TimingTrace (Time::DurationSecondsType warnIfLongerThan);
                TimingTrace (Time::DurationSecondsType warnIfLongerThan, const Characters::String& label);
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "TimingTrace.inl"

#endif  /*_Stroika_Foundation_Debug_TimingTrace_h_*/
