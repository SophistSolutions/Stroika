/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_SystemPerformance_Support_WMICollector_inl_
#define _Stroika_Framework_SystemPerformance_Support_WMICollector_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::SystemPerformance::Support {

    /*
     ********************************************************************************
     ******************** SystemPerformance::Support::WMICollector ******************
     ********************************************************************************
     */
    inline Foundation::Time::DurationSecondsType WMICollector::GetTimeOfLastCollection () const
    {
        return fTimeOfLastCollection_;
    }

}
#endif /*_Stroika_Framework_SystemPerformance_Support_WMICollector_inl_*/
