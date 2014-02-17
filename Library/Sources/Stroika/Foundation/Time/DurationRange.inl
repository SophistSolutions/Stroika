/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DurationRange_inl_
#define _Stroika_Foundation_Time_DurationRange_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/ModuleInit.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            namespace Private_ {
                struct  DurationRange_ModuleData_ {
                    DurationRange_ModuleData_ ();
                    const Duration  fLowerBound;
                    const Duration  fUpperBound;
                };
            }


        }
    }
}
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::DurationRange_ModuleData_>    _Stroika_Foundation_Time_DurationRange_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_Time_DurationRange_inl_*/
