/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateTimeRange_inl_
#define _Stroika_Foundation_Time_DateTimeRange_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/ModuleInit.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy || 1
            namespace Private_ {
                struct DateTimeRange_ModuleData_ {
                    DateTimeRange_ModuleData_ ();
                    const DateTime  fLowerBound;
                    const DateTime  fUpperBound;
                };
            }
#endif


        }
    }
}
#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy || 1
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::DateTimeRange_ModuleData_>    _Stroika_Foundation_Time_DateTimeRange_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif
#endif  /*_Stroika_Foundation_Time_DateTimeRange_inl_*/
