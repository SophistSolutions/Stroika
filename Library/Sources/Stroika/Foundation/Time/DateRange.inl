/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DateRange_inl_
#define _Stroika_Foundation_Time_DateRange_inl_  1


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
                struct DateRange_ModuleData_ {
                    DateRange_ModuleData_ ();
                    const Date  fLowerBound;
                    const Date  fUpperBound;
                };
            }


        }
    }
}
namespace   {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::DateRange_ModuleData_>    _Stroika_Foundation_Time_DateRange_ModuleData_;   // this object constructed for the CTOR/DTOR per-module side-effects
}
#endif  /*_Stroika_Foundation_Time_DateRange_inl_*/
