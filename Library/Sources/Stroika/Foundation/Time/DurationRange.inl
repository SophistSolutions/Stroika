/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DurationRange_inl_
#define _Stroika_Foundation_Time_DurationRange_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/ModuleInit.h"

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ************************ Private_::DurationRangeTraitsType_ ********************
     ********************************************************************************
     */
    inline Duration Private_::DurationRangeTraitsType_::GetNext (Duration i)
    {
        return Duration{nextafter (i.As<double> (), numeric_limits<double>::max ())};
    }
    inline Duration Private_::DurationRangeTraitsType_::GetPrevious (Duration i)
    {
        return Duration{nextafter (i.As<double> (), numeric_limits<double>::min ())};
    }

    namespace Private_ {
        struct DurationRange_ModuleData_ {
            DurationRange_ModuleData_ ();
            const Duration fLowerBound;
            const Duration fUpperBound;
        };
    }

}

namespace {
    Stroika::Foundation::Execution::ModuleInitializer<Stroika::Foundation::Time::Private_::DurationRange_ModuleData_> _Stroika_Foundation_Time_DurationRange_ModuleData_; // this object constructed for the CTOR/DTOR per-module side-effects
}

#endif /*_Stroika_Foundation_Time_DurationRange_inl_*/
