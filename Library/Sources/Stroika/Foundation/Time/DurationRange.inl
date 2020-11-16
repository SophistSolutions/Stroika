/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_DurationRange_inl_
#define _Stroika_Foundation_Time_DurationRange_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
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

}

#endif /*_Stroika_Foundation_Time_DurationRange_inl_*/
